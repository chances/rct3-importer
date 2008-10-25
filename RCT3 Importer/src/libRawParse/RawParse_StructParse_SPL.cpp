///////////////////////////////////////////////////////////////////////////////
//
// raw ovl xml interpreter
// Command line ovl creation utility
// Copyright (C) 2007 Tobias Minch
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
// Written by
//   Tobias Minich - belgabor@gmx.de
//
///////////////////////////////////////////////////////////////////////////////



#include "RawParse_cpp.h"

#include "ManagerSPL.h"

//#include <boost/iostreams/device/array.hpp>
//#include <boost/iostreams/stream.hpp>

#include "3DLoader.h"
#include "matrix.h"
#include "RCT3Structs.h"


void cRawParser::ParseSPL(cXmlNode& node) {
    USE_PREFIX(node);
    cSpline spl;
    wxString name = ParseString(node, RAWXML_SPL, "name", NULL, useprefix);
    spl.name = name.ToAscii();
    if (node.hasProp("modelFile")) {
        wxString splinename = ParseString(node, RAWXML_SPL, "name", NULL);
        ParseStringOption(splinename, node, "modelSpline", NULL);
        wxFSFileName modelfile = ParseString(node, RAWXML_SPL, "modelFile", NULL);
        if (!modelfile.IsAbsolute())
            modelfile.MakeAbsolute(m_input.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME));

        // TODO: baking

        boost::shared_ptr<c3DLoader> model(c3DLoader::LoadFile(modelfile.GetFullPath().c_str()));
        if (!model.get())
            throw MakeNodeException<RCT3Exception>(_("spline tag: Model file not readable or has unknown format: ") + modelfile.GetFullPath(), node);

        if (!model->getSplines().size()) {
            throw MakeNodeException<RCT3Exception>(_("spline tag: Model file does not contain splines: ") + modelfile.GetFullPath(), node);
        }

        std::map<wxString, c3DSpline>::const_iterator sp = model->getSplines().find(splinename);
        if (sp == model->getSplines().end()) {
            throw MakeNodeException<RCT3Exception>(wxString::Format(_("spline tag: Model file '%s' does not contain spline '%s'."), modelfile.GetFullPath().c_str(), splinename.c_str()), node);
        }
        if (!sp->second.m_nodes.size()) {
            throw MakeNodeException<RCT3Exception>(wxString::Format(_("spline tag: Model file '%s', spline '%s' has no nodes."), modelfile.GetFullPath().c_str(), splinename.c_str()), node);
        }

        c3DLoaderOrientation ori = ParseOrientation(node, wxString::Format(wxT("spl(%s) tag"), name.c_str()), model->getOrientation());

        spl.nodes = sp->second.GetFixed(ori);
        spl.cyclic = sp->second.m_cyclic;
    } else {
        if (node.hasProp("totalLength")) {
            spl.calc_length = false;
            spl.totallength = ParseFloat(node, wxT(RAWXML_SPL), wxT("totalLength"));
            spl.inv_totallength = ParseFloat(node, wxT(RAWXML_SPL), wxT("inv_totalLength"));
        }
        OPTION_PARSE(float, spl.max_y, ParseFloat(node, wxT(RAWXML_SPL), wxT("max_y")));
        OPTION_PARSE(unsigned long, spl.cyclic, ParseUnsigned(node, wxT(RAWXML_SPL), wxT("cyclic")));
        OPTION_PARSE(unsigned long, spl.lengthcalcres, ParseUnsigned(node, wxT(RAWXML_SPL), wxT("autoLengthRes")));
        c3DLoaderOrientation ori = ParseOrientation(node, wxString::Format(wxT("spl(%s) tag"), name.c_str()));
        wxLogVerbose(wxString::Format(_("Adding spl %s to %s."), wxString(spl.name.c_str(), wxConvLocal).c_str(), m_output.GetFullPath().c_str()));

        cXmlNode child(node.children());
        while (child) {
            DO_CONDITION_COMMENT(child);

            if (child(RAWXML_SPL_NODE)) {
                SplineNode sp;
                sp.pos.x = ParseFloat(child, wxT(RAWXML_SPL_NODE), wxT("x"));
                sp.pos.y = ParseFloat(child, wxT(RAWXML_SPL_NODE), wxT("y"));
                sp.pos.z = ParseFloat(child, wxT(RAWXML_SPL_NODE), wxT("z"));
                sp.cp1.x = ParseFloat(child, wxT(RAWXML_SPL_NODE), wxT("cp1x"));
                sp.cp1.y = ParseFloat(child, wxT(RAWXML_SPL_NODE), wxT("cp1y"));
                sp.cp1.z = ParseFloat(child, wxT(RAWXML_SPL_NODE), wxT("cp1z"));
                sp.cp2.x = ParseFloat(child, wxT(RAWXML_SPL_NODE), wxT("cp2x"));
                sp.cp2.y = ParseFloat(child, wxT(RAWXML_SPL_NODE), wxT("cp2y"));
                sp.cp2.z = ParseFloat(child, wxT(RAWXML_SPL_NODE), wxT("cp2z"));
                if (ori != ORIENTATION_LEFT_YUP) {
                    doFixOrientation(sp.pos, ori);
                    doFixOrientation(sp.cp1, ori);
                    doFixOrientation(sp.cp2, ori);
                }
                spl.nodes.push_back(sp);
            } else if (child(RAWXML_SPL_LENGTH)) {
                float l = ParseFloat(child, wxT(RAWXML_SPL_LENGTH), wxT("length"));
                spl.lengths.push_back(l);
            } else if (child(RAWXML_SPL_DATA)) {
                cSplineData dt;
                wxString datastr = ParseString(child, wxT(RAWXML_SPL_DATA), wxT("data"), NULL);
                if (datastr.Length() != 28)
                    throw MakeNodeException<RCT3InvalidValueException>(wxString::Format(_("The data attribute of a spl(%s)/spldata tag is of invalid length"), name.c_str()), child);
                const char* d = datastr.ToAscii();
                for (int i = 0; i < 28; i+=2) {
                    unsigned char dta = 0;
                    unsigned char t = ParseDigit(d[i]);
                    if (t >= 16)
                        throw MakeNodeException<RCT3InvalidValueException>(wxString::Format(_("The data attribute of a spl(%s)/spldata tag has an invalid character"), name.c_str()), child);
                    dta += t * 16;
                    t = ParseDigit(d[i+1]);
                    if (t >= 16)
                        throw MakeNodeException<RCT3InvalidValueException>(wxString::Format(_("The data attribute of a spl(%s)/spldata tag has an invalid character"), name.c_str()), child);
                    dta += t;
                    dt.data[i/2] = dta;
                }
                spl.datas.push_back(dt);
            } else if (child.element()) {
                throw MakeNodeException<RCT3Exception>(wxString::Format(_("Unknown tag '%s' in spl(%s) tag."), child.wxname().c_str(), name.c_str()), child);
            }
            child.go_next();
        }
    }

    ovlSPLManager* c_spl = m_ovl.GetManager<ovlSPLManager>();
    c_spl->AddSpline(spl);
}
