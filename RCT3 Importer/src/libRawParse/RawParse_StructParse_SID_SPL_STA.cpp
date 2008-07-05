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
#include "ManagerSID.h"
#include "ManagerSTA.h"

void cRawParser::ParseSID(cXmlNode& node) {
    USE_PREFIX(node);
    cSid sid;
    wxString name = ParseString(node, wxT(RAWXML_SID), wxT("name"), NULL, useprefix);
    sid.name = name.ToAscii();
    sid.ovlpath = ParseString(node, wxT(RAWXML_SID), wxT("ovlpath"), NULL).ToAscii();
    sid.ui.name = ParseString(node, wxT(RAWXML_SID), wxT("nametxt"), NULL, useprefix).ToAscii();
    sid.ui.icon = ParseString(node, wxT(RAWXML_SID), wxT("icon"), NULL, useprefix).ToAscii();
    wxString svd = ParseString(node, wxT(RAWXML_SID), wxT("svd"), NULL, useprefix);
    sid.svds.push_back(std::string(svd.ToAscii()));
    wxLogVerbose(wxString::Format(_("Adding sid %s to %s."), wxString(sid.name.c_str(), wxConvLocal).c_str(), m_output.GetFullPath().c_str()));

    cXmlNode child(node.children());
    while (child) {
        DO_CONDITION_COMMENT(child);

        if (child(RAWXML_SID_GROUP)) {
            USE_PREFIX(child);
            wxString name = ParseString(child, wxT(RAWXML_SID_GROUP), wxT("name"), NULL, useprefix);
            wxString icon = ParseString(child, wxT(RAWXML_SID_GROUP), wxT("icon"), NULL, useprefix);
            sid.ui.group = name.ToAscii();
            sid.ui.groupicon = icon.ToAscii();
        } else if (child(RAWXML_SID_TYPE)) {
            OPTION_PARSE(unsigned long, sid.ui.type, ParseUnsigned(child, wxT(RAWXML_SID_TYPE), wxT("scenerytype")));
            OPTION_PARSE(long, sid.ui.cost, ParseSigned(child, wxT(RAWXML_SID_TYPE), wxT("cost")));
            OPTION_PARSE(long, sid.ui.removal_cost, ParseSigned(child, wxT(RAWXML_SID_TYPE), wxT("removalcost")));
        } else if (child(RAWXML_SID_POSITION)) {
            OPTION_PARSE(unsigned long, sid.position.positioningtype, ParseUnsigned(child, wxT(RAWXML_SID_POSITION), wxT("type")));
            OPTION_PARSE(unsigned long, sid.position.xsquares, ParseUnsigned(child, wxT(RAWXML_SID_POSITION), wxT("xsquares")));
            OPTION_PARSE(unsigned long, sid.position.zsquares, ParseUnsigned(child, wxT(RAWXML_SID_POSITION), wxT("zsquares")));
            OPTION_PARSE(float, sid.position.xpos, ParseFloat(child, wxT(RAWXML_SID_POSITION), wxT("xpos")));
            OPTION_PARSE(float, sid.position.ypos, ParseFloat(child, wxT(RAWXML_SID_POSITION), wxT("ypos")));
            OPTION_PARSE(float, sid.position.zpos, ParseFloat(child, wxT(RAWXML_SID_POSITION), wxT("zpos")));
            OPTION_PARSE(float, sid.position.xsize, ParseFloat(child, wxT(RAWXML_SID_POSITION), wxT("xsize")));
            OPTION_PARSE(float, sid.position.ysize, ParseFloat(child, wxT(RAWXML_SID_POSITION), wxT("ysize")));
            OPTION_PARSE(float, sid.position.zsize, ParseFloat(child, wxT(RAWXML_SID_POSITION), wxT("zsize")));
            ParseStringOption(sid.position.supports, child, wxT("supports"), NULL, false);
        } else if (child(RAWXML_SID_COLOURS)) {
            OPTION_PARSE(unsigned long, sid.colours.defaultcol[0], ParseUnsigned(child, wxT(RAWXML_SID_COLOURS), wxT("choice1")));
            OPTION_PARSE(unsigned long, sid.colours.defaultcol[1], ParseUnsigned(child, wxT(RAWXML_SID_COLOURS), wxT("choice2")));
            OPTION_PARSE(unsigned long, sid.colours.defaultcol[2], ParseUnsigned(child, wxT(RAWXML_SID_COLOURS), wxT("choice3")));
        } else if (child(RAWXML_SID_SQUAREUNKNOWNS)) {
            cSidSquareUnknowns squ;
            OPTION_PARSE(long, squ.min_height, ParseSigned(child, wxT(RAWXML_SID_SQUAREUNKNOWNS), wxT("minHeight")));
            OPTION_PARSE(long, squ.max_height, ParseSigned(child, wxT(RAWXML_SID_SQUAREUNKNOWNS), wxT("maxHeight")));
            OPTION_PARSE(unsigned long, squ.supports, ParseUnsigned(child, wxT(RAWXML_SID_SQUAREUNKNOWNS), wxT("supports")));
            OPTION_PARSE(unsigned long, squ.flags, ParseUnsigned(child, wxT(RAWXML_SID_SQUAREUNKNOWNS), wxT("flags")));

/*
            if (child.hasProp("flags")) {
                wxString flags = ParseString(child, wxT(RAWXML_SID_SQUAREUNKNOWNS), wxT("flags"), NULL);
                if (flags.Length() != 32)
                    throw RCT3InvalidValueException(_("The flags attribute of a sidsquareunknowns tag is of invalid length"));
                const char* fl = flags.ToAscii();
                for (int i = 0; i < 32; ++i) {
                    if (fl[i] == '0') {
                        squ.flag[i] = false;
                    } else if (fl[i] == '1') {
                        squ.flag[i] = true;
                    } else {
                        throw RCT3InvalidValueException(_("The flags attribute of a sidsquareunknowns tag has an invalid character"));
                    }
                }
            }
*/
            sid.squareunknowns.push_back(squ);
        } else if (child(RAWXML_SID_IMPORTERUNKNOWNS)) {
            OPTION_PARSE(unsigned long, sid.importerunknowns.unk1, ParseUnsigned(child, wxT(RAWXML_SID_IMPORTERUNKNOWNS), wxT("u1")));
            OPTION_PARSE(unsigned long, sid.importerunknowns.unk2, ParseUnsigned(child, wxT(RAWXML_SID_IMPORTERUNKNOWNS), wxT("u2")));
            OPTION_PARSE(unsigned long, sid.importerunknowns.flags, ParseUnsigned(child, wxT(RAWXML_SID_IMPORTERUNKNOWNS), wxT("flags")));
            /*
            if (child.hasProp("flags")) {
                wxString flags = ParseString(child, wxT(RAWXML_SID_IMPORTERUNKNOWNS), wxT("flags"), NULL);
                if (flags.Length() != 32)
                    throw RCT3InvalidValueException(_("The flags attribute of a sidimporterunknowns tag is of invalid length"));
                //const char* fl = flags.ToAscii();
                for (int i = 0; i < 32; ++i) {
                    if (flags[i] == wxT('0')) {
                        sid.importerunknowns.flag[i] = false;
                    } else if (flags[i] == wxT('1')) {
                        sid.importerunknowns.flag[i] = true;
                    } else {
                        throw RCT3InvalidValueException(_("The flags attribute of a sidimporterunknowns tag has an invalid character"));
                    }
                }
            }
            */
        } else if (child(RAWXML_SID_STALLUNKNOWNS)) {
            sid.stallunknowns.MakeStall();
            OPTION_PARSE(unsigned long, sid.stallunknowns.unk1, ParseUnsigned(child, wxT(RAWXML_SID_STALLUNKNOWNS), wxT("u1")));
            OPTION_PARSE(unsigned long, sid.stallunknowns.unk2, ParseUnsigned(child, wxT(RAWXML_SID_STALLUNKNOWNS), wxT("u2")));
        } else if (child(RAWXML_SID_EXTRA)) {
            OPTION_PARSE(unsigned long, sid.extra.version, ParseUnsigned(child, wxT(RAWXML_SID_EXTRA), wxT("version")));
            OPTION_PARSE(unsigned long, sid.extra.AddonPack, ParseUnsigned(child, wxT(RAWXML_SID_EXTRA), wxT("addonpack")));
            OPTION_PARSE(long, sid.extra.billboardaspect, ParseSigned(child, wxT(RAWXML_SID_EXTRA), wxT("billboardaspect")));
        } else if (child(RAWXML_SVD)) {
            USE_PREFIX(child);
            svd = HandleStringContent(child(), NULL, useprefix);
            if (svd.IsEmpty())
                throw RCT3Exception(FinishNodeError(wxString::Format(_("An svd in sid tag '%s' is empty."), name.c_str()), child));
            sid.svds.push_back(std::string(svd.ToAscii()));
        } else if (child(RAWXML_SID_PARAMETER)) {
            cSidParam param;
            param.name = ParseString(child, wxT(RAWXML_SID_GROUP), wxT("name"), NULL).ToAscii();
            ParseStringOption(param.param, child, wxT("param"), NULL, false);
            if (param.param != "") {
                if (param.param[0] != ' ')
                    param.param = " " + param.param;
            }
            sid.parameters.push_back(param);
        } else if (child.element()) {
            throw RCT3Exception(FinishNodeError(wxString::Format(_("Unknown tag '%s' in sid tag '%s'."), STRING_FOR_FORMAT(child.name()), name.c_str()), child));
        }

        child.go_next();
    }

    ovlSIDManager* c_sid = m_ovl.GetManager<ovlSIDManager>();
    c_sid->AddSID(sid);
}

void cRawParser::ParseSPL(cXmlNode& node) {
    USE_PREFIX(node);
    cSpline spl;
    wxString name = ParseString(node, wxT(RAWXML_SPL), wxT("name"), NULL, useprefix);
    spl.name = name.ToAscii();
    if (node.hasProp("modelFile")) {
        wxString splinename = ParseString(node, wxT(RAWXML_SPL), wxT("splineObject"), NULL);
        wxFSFileName modelfile = ParseString(node, wxT(RAWXML_SPL), wxT("modelFile"), NULL);
        if (!modelfile.IsAbsolute())
            modelfile.MakeAbsolute(m_input.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME));

        // TODO: baking

        boost::shared_ptr<c3DLoader> model(c3DLoader::LoadFile(modelfile.GetFullPath().c_str()));
        if (!model.get())
            throw RCT3Exception(_("spline tag: Model file not readable or has unknown format: ") + modelfile.GetFullPath());

        if (!model->GetSplines().size()) {
            throw RCT3Exception(_("spline tag: Model file does not contain splines: ") + modelfile.GetFullPath());
        }

        std::map<wxString, c3DSpline>::const_iterator sp = model->GetSplines().find(splinename);
        if (sp == model->GetSplines().end()) {
            throw RCT3Exception(wxString::Format(_("spline tag: Model file '%s' does not contain spline '%s'."), modelfile.GetFullPath().c_str(), splinename.c_str()));
        }
        if (!sp->second.m_nodes.size()) {
            throw RCT3Exception(wxString::Format(_("spline tag: Model file '%s', spline '%s' has no nodes."), modelfile.GetFullPath().c_str(), splinename.c_str()));
        }

        c3DLoaderOrientation ori = ParseOrientation(node, wxString::Format(wxT("spl(%s) tag"), name.c_str()), model->GetOrientation());

        wxLogMessage("%d", static_cast<int>(ori));

        spl.nodes = sp->second.GetFixed(ori);
        spl.cyclic = sp->second.m_cyclic;
    } else {
        if (node.hasProp("totallength")) {
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
                    throw RCT3InvalidValueException(wxString::Format(_("The data attribute of a spl(%s)/spldata tag is of invalid length"), name.c_str()));
                const char* d = datastr.ToAscii();
                for (int i = 0; i < 28; i+=2) {
                    unsigned char dta = 0;
                    unsigned char t = ParseDigit(d[i]);
                    if (t >= 16)
                        throw RCT3InvalidValueException(wxString::Format(_("The data attribute of a spl(%s)/spldata tag has an invalid character"), name.c_str()));
                    dta += t * 16;
                    t = ParseDigit(d[i+1]);
                    if (t >= 16)
                        throw RCT3InvalidValueException(wxString::Format(_("The data attribute of a spl(%s)/spldata tag has an invalid character"), name.c_str()));
                    dta += t;
                    dt.data[i/2] = dta;
                }
                spl.datas.push_back(dt);
            } else if (child.element()) {
                throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in spl(%s) tag."), STRING_FOR_FORMAT(child.name()), name.c_str()));
            }
            child.go_next();
        }
    }

    ovlSPLManager* c_spl = m_ovl.GetManager<ovlSPLManager>();
    c_spl->AddSpline(spl);
}

void cRawParser::ParseSTA(cXmlNode& node) {
    USE_PREFIX(node);
    cStall stall;
    wxString name = ParseString(node, wxT(RAWXML_STA), wxT("name"), NULL, useprefix);
    stall.name = name.ToAscii();
    stall.attraction.name = ParseString(node, wxT(RAWXML_STA), wxT("nametxt"), NULL, useprefix).ToAscii();
    stall.attraction.description = ParseString(node, wxT(RAWXML_STA), wxT("description"), NULL, useprefix).ToAscii();
    stall.sid = ParseString(node, wxT(RAWXML_STA), wxT("sid"), NULL, useprefix).ToAscii();
    wxLogVerbose(wxString::Format(_("Adding sta %s to %s."), wxString(stall.name.c_str(), wxConvLocal).c_str(), m_output.GetFullPath().c_str()));

    cXmlNode child(node.children());
    while (child) {
        DO_CONDITION_COMMENT(child);

        if (child(RAWXML_ATTRACTION)) {
            ParseAttraction(child, stall.attraction);
        } else if (child(RAWXML_STA_ITEM)) {
            USE_PREFIX(child);
            cStallItem item;
            item.item = ParseString(child, wxT(RAWXML_STA_ITEM), wxT("cid"), NULL, useprefix).ToAscii();
            item.cost = ParseUnsigned(child, wxT(RAWXML_STA_ITEM), wxT("cost"));
            stall.items.push_back(item);
        } else if (child(RAWXML_STA_STALLUNKNOWNS)) {
            OPTION_PARSE(unsigned long, stall.unknowns.unk1, ParseUnsigned(child, wxT(RAWXML_STA_STALLUNKNOWNS), wxT("u1")));
            OPTION_PARSE(unsigned long, stall.unknowns.unk2, ParseUnsigned(child, wxT(RAWXML_STA_STALLUNKNOWNS), wxT("u2")));
            OPTION_PARSE(unsigned long, stall.unknowns.unk3, ParseUnsigned(child, wxT(RAWXML_STA_STALLUNKNOWNS), wxT("u3")));
            OPTION_PARSE(unsigned long, stall.unknowns.unk4, ParseUnsigned(child, wxT(RAWXML_STA_STALLUNKNOWNS), wxT("u4")));
            OPTION_PARSE(unsigned long, stall.unknowns.unk5, ParseUnsigned(child, wxT(RAWXML_STA_STALLUNKNOWNS), wxT("u5")));
            OPTION_PARSE(unsigned long, stall.unknowns.unk6, ParseUnsigned(child, wxT(RAWXML_STA_STALLUNKNOWNS), wxT("u6")));
        } else if (child.element()) {
            throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in sta tag '%s'."), STRING_FOR_FORMAT(child.name()), name.c_str()));
        }

        child.go_next();
    }

    ovlSTAManager* c_sta = m_ovl.GetManager<ovlSTAManager>();
    c_sta->AddStall(stall);
}
