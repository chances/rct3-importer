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

//#include <boost/algorithm/string.hpp>
//#include <boost/tokenizer.hpp>

#include "ManagerSVD.h"

void cRawParser::ParseSVD(cXmlNode& node) {
    USE_PREFIX(node);
    cSceneryItemVisual svd;
    wxString name = ParseString(node, wxT(RAWXML_SVD), wxT("name"), NULL, useprefix);
    svd.name = name.ToAscii();
    OPTION_PARSE(unsigned long, svd.sivflags, ParseFloat(node, wxT(RAWXML_SVD), wxT("flags")));
    OPTION_PARSE(float, svd.sway, ParseFloat(node, wxT(RAWXML_SVD), wxT("sway")));
    OPTION_PARSE(float, svd.brightness, ParseFloat(node, wxT(RAWXML_SVD), wxT("brightness")));
    OPTION_PARSE(float, svd.scale, ParseFloat(node, wxT(RAWXML_SVD), wxT("scale")));
    wxLogVerbose(wxString::Format(_("Adding svd %s to %s."), name.c_str(), m_output.GetFullPath().c_str()));

    cXmlNode child(node.children());
    while (child) {
        DO_CONDITION_COMMENT(child);

        if (child(RAWXML_UNKNOWNS)) {
            OPTION_PARSE(float, svd.unk4, ParseFloat(child, wxT(RAWXML_UNKNOWNS), wxT("u4")));
            OPTION_PARSE(unsigned long, svd.unk6, ParseUnsigned(child, wxT(RAWXML_UNKNOWNS), wxT("u6")));
            OPTION_PARSE(unsigned long, svd.unk7, ParseUnsigned(child, wxT(RAWXML_UNKNOWNS), wxT("u7")));
            OPTION_PARSE(unsigned long, svd.unk8, ParseUnsigned(child, wxT(RAWXML_UNKNOWNS), wxT("u8")));
            OPTION_PARSE(unsigned long, svd.unk9, ParseUnsigned(child, wxT(RAWXML_UNKNOWNS), wxT("u9")));
            OPTION_PARSE(unsigned long, svd.unk10, ParseUnsigned(child, wxT(RAWXML_UNKNOWNS), wxT("u10")));
            OPTION_PARSE(unsigned long, svd.unk11, ParseUnsigned(child, wxT(RAWXML_UNKNOWNS), wxT("u11")));
        } else if (child(RAWXML_SVD_LOD)) {
            USE_PREFIX(child);
            cSceneryItemVisualLOD lod;
            wxString lodname = ParseString(child, wxT(RAWXML_SVD_LOD), wxT("name"), NULL);
            lod.name = lodname.ToAscii();
            lod.meshtype = ParseUnsigned(child, wxT(RAWXML_SVD_LOD), wxT("type"));
            lod.distance = ParseFloat(child, wxT(RAWXML_SVD_LOD), wxT("distance"));
            ParseStringOption(lod.staticshape, child, wxT("staticshape"), NULL, useprefix);
            ParseStringOption(lod.boneshape, child, wxT("boneshape"), NULL, useprefix);
            ParseStringOption(lod.fts, child, wxT("ftx"), NULL, useprefix);
            ParseStringOption(lod.txs, child, wxT("txs"), NULL);

            cXmlNode subchild(child.children());
            while (subchild) {
                DO_CONDITION_COMMENT(subchild);

                if (subchild(RAWXML_UNKNOWNS)) {
                    OPTION_PARSE(unsigned long, lod.unk2, ParseUnsigned(child, wxT(RAWXML_UNKNOWNS), wxT("u2")));
                    OPTION_PARSE(unsigned long, lod.unk4, ParseUnsigned(child, wxT(RAWXML_UNKNOWNS), wxT("u4")));
                    OPTION_PARSE(float, lod.unk7, ParseFloat(child, wxT(RAWXML_UNKNOWNS), wxT("u7")));
                    OPTION_PARSE(float, lod.unk8, ParseFloat(child, wxT(RAWXML_UNKNOWNS), wxT("u8")));
                    OPTION_PARSE(float, lod.unk9, ParseFloat(child, wxT(RAWXML_UNKNOWNS), wxT("u9")));
                    OPTION_PARSE(float, lod.unk10, ParseFloat(child, wxT(RAWXML_UNKNOWNS), wxT("u10")));
                    OPTION_PARSE(float, lod.unk11, ParseFloat(child, wxT(RAWXML_UNKNOWNS), wxT("u11")));
                    OPTION_PARSE(float, lod.unk12, ParseFloat(child, wxT(RAWXML_UNKNOWNS), wxT("u12")));
                    OPTION_PARSE(unsigned long, lod.unk14, ParseUnsigned(child, wxT(RAWXML_UNKNOWNS), wxT("u14")));
                } else if (subchild(RAWXML_SVD_LOD_ANIMATION)) {
                    USE_PREFIX(subchild);
                    wxString anim = subchild.wxcontent();
                    MakeVariable(anim);
                    if (useprefix)
                        lod.animations.push_back(m_prefix + std::string(anim.ToAscii()));
                    else
                        lod.animations.push_back(std::string(anim.ToAscii()));
                } else if (subchild.element()) {
                    throw MakeNodeException<RCT3Exception>(wxString::Format(_("Unknown tag '%s' in svd(%s)/svdlod(%s)."), subchild.wxname().c_str(), name.c_str(), lodname.c_str()), subchild);
                }

                subchild.go_next();
            }

            svd.lods.push_back(lod);
        } else if (child.element()) {
            throw MakeNodeException<RCT3Exception>(wxString::Format(_("Unknown tag '%s' in svd tag '%s'."), child.wxname().c_str(), name.c_str()), child);
        }

        child.go_next();
    }

    ovlSVDManager* c_svd = m_ovl.GetManager<ovlSVDManager>();
    c_svd->AddSVD(svd);
}
