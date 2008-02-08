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

void cRawParser::ParseSID(wxXmlNode* node) {
    USE_PREFIX(node);
    cSid sid;
    sid.name = ParseString(node, RAWXML_SID, wxT("name"), NULL, useprefix).ToAscii();
    sid.ovlpath = ParseString(node, RAWXML_SID, wxT("ovlpath"), NULL).ToAscii();
    sid.ui.name = ParseString(node, RAWXML_SID, wxT("nametxt"), NULL, useprefix).ToAscii();
    sid.ui.icon = ParseString(node, RAWXML_SID, wxT("icon"), NULL, useprefix).ToAscii();
    wxString svd = ParseString(node, RAWXML_SID, wxT("svd"), NULL, useprefix);
    sid.svds.push_back(std::string(svd.ToAscii()));
    wxLogVerbose(wxString::Format(_("Adding sid %s to %s."), wxString(sid.name.c_str(), wxConvLocal).c_str(), m_output.GetFullPath().c_str()));

    wxXmlNode *child = node->GetChildren();
    while (child) {
        DO_CONDITION_COMMENT(child);

        if (child->GetName() == RAWXML_SID_GROUP) {
            USE_PREFIX(child);
            wxString name = ParseString(child, RAWXML_SID_GROUP, wxT("name"), NULL, useprefix);
            wxString icon = ParseString(child, RAWXML_SID_GROUP, wxT("icon"), NULL, useprefix);
            sid.ui.group = name.ToAscii();
            sid.ui.groupicon = icon.ToAscii();
        } else if (child->GetName() == RAWXML_SID_TYPE) {
            OPTION_PARSE(unsigned long, sid.ui.type, ParseUnsigned(child, RAWXML_SID_TYPE, wxT("scenerytype")));
            OPTION_PARSE(long, sid.ui.cost, ParseSigned(child, RAWXML_SID_TYPE, wxT("cost")));
            OPTION_PARSE(long, sid.ui.removal_cost, ParseSigned(child, RAWXML_SID_TYPE, wxT("removalcost")));
        } else if (child->GetName() == RAWXML_SID_POSITION) {
            OPTION_PARSE(unsigned long, sid.position.positioningtype, ParseUnsigned(child, RAWXML_SID_POSITION, wxT("type")));
            OPTION_PARSE(unsigned long, sid.position.xsquares, ParseUnsigned(child, RAWXML_SID_POSITION, wxT("xsquares")));
            OPTION_PARSE(unsigned long, sid.position.ysquares, ParseUnsigned(child, RAWXML_SID_POSITION, wxT("ysquares")));
            OPTION_PARSE(float, sid.position.xpos, ParseFloat(child, RAWXML_SID_POSITION, wxT("xpos")));
            OPTION_PARSE(float, sid.position.ypos, ParseFloat(child, RAWXML_SID_POSITION, wxT("ypos")));
            OPTION_PARSE(float, sid.position.zpos, ParseFloat(child, RAWXML_SID_POSITION, wxT("zpos")));
            OPTION_PARSE(float, sid.position.xsize, ParseFloat(child, RAWXML_SID_POSITION, wxT("xsize")));
            OPTION_PARSE(float, sid.position.ysize, ParseFloat(child, RAWXML_SID_POSITION, wxT("ysize")));
            OPTION_PARSE(float, sid.position.zsize, ParseFloat(child, RAWXML_SID_POSITION, wxT("zsize")));
            ParseStringOption(sid.position.supports, child, wxT("supports"), NULL, false);
        } else if (child->GetName() == RAWXML_SID_COLOURS) {
            OPTION_PARSE(unsigned long, sid.colours.defaultcol[0], ParseUnsigned(child, RAWXML_SID_COLOURS, wxT("choice1")));
            OPTION_PARSE(unsigned long, sid.colours.defaultcol[1], ParseUnsigned(child, RAWXML_SID_COLOURS, wxT("choice2")));
            OPTION_PARSE(unsigned long, sid.colours.defaultcol[2], ParseUnsigned(child, RAWXML_SID_COLOURS, wxT("choice3")));
        } else if (child->GetName() == RAWXML_SID_SQUAREUNKNOWNS) {
            cSidSquareUnknowns squ;
            OPTION_PARSE(unsigned long, squ.unk6, ParseUnsigned(child, RAWXML_SID_SQUAREUNKNOWNS, wxT("u6")));
            OPTION_PARSE(unsigned long, squ.unk7, ParseUnsigned(child, RAWXML_SID_SQUAREUNKNOWNS, wxT("u7")));
            if (child->HasProp(wxT("u8"))) {
                OPTION_PARSE(unsigned long, squ.unk8, ParseUnsigned(child, RAWXML_SID_SQUAREUNKNOWNS, wxT("u8")));
                squ.use_unk8 = true;
            }
            OPTION_PARSE(unsigned long, squ.unk9, ParseUnsigned(child, RAWXML_SID_SQUAREUNKNOWNS, wxT("u9")));

            if (child->HasProp(wxT("flags"))) {
                wxString flags = ParseString(child, RAWXML_SID_SQUAREUNKNOWNS, wxT("flags"), NULL);
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

            sid.squareunknowns.push_back(squ);
        } else if (child->GetName() == RAWXML_SID_IMPORTERUNKNOWNS) {
            OPTION_PARSE(unsigned long, sid.importerunknowns.unk1, ParseUnsigned(child, RAWXML_SID_IMPORTERUNKNOWNS, wxT("u1")));
            OPTION_PARSE(unsigned long, sid.importerunknowns.unk2, ParseUnsigned(child, RAWXML_SID_IMPORTERUNKNOWNS, wxT("u2")));
            if (child->HasProp(wxT("flags"))) {
                wxString flags = ParseString(child, RAWXML_SID_IMPORTERUNKNOWNS, wxT("flags"), NULL);
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

        } else if (child->GetName() == RAWXML_SID_STALLUNKNOWNS) {
            sid.stallunknowns.MakeStall();
            OPTION_PARSE(unsigned long, sid.stallunknowns.unk1, ParseUnsigned(child, RAWXML_SID_STALLUNKNOWNS, wxT("u1")));
            OPTION_PARSE(unsigned long, sid.stallunknowns.unk2, ParseUnsigned(child, RAWXML_SID_STALLUNKNOWNS, wxT("u2")));
        } else if (child->GetName() == RAWXML_SID_SVD) {
            USE_PREFIX(child);
            svd = ParseString(node, RAWXML_SVD, wxT("name"), NULL, useprefix);
            sid.svds.push_back(std::string(svd.ToAscii()));
        } else if (child->GetName() == RAWXML_SID_PARAMETER) {
            cSidParam param;
            param.name = ParseString(child, RAWXML_SID_GROUP, wxT("name"), NULL).ToAscii();
            ParseStringOption(param.param, child, wxT("param"), NULL, false);
            if (param.param != "") {
                if (param.param[0] != ' ')
                    param.param = " " + param.param;
            }
            sid.parameters.push_back(param);
        } else if COMPILER_WRONGTAG(child) {
            throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in sid tag."), child->GetName().c_str()));
        }

        child = child->GetNext();
    }

    ovlSIDManager* c_sid = m_ovl.GetManager<ovlSIDManager>();
    c_sid->AddSID(sid);
}

void cRawParser::ParseSPL(wxXmlNode* node) {
    USE_PREFIX(node);
    cSpline spl;
    wxString name = ParseString(node, RAWXML_SPL, wxT("name"), NULL, useprefix);
    spl.name = name.ToAscii();
    if (node->HasProp(wxT("totallength"))) {
        spl.calc_length = false;
        spl.totallength = ParseFloat(node, RAWXML_SPL, wxT("totallength"));
        spl.inv_totallength = ParseFloat(node, RAWXML_SPL, wxT("inv_totallength"));
    }
    OPTION_PARSE(float, spl.max_y, ParseFloat(node, RAWXML_SPL, wxT("max_y")));
    OPTION_PARSE(unsigned long, spl.cyclic, ParseUnsigned(node, RAWXML_SPL, wxT("cyclic")));
    OPTION_PARSE(unsigned long, spl.lengthcalcres, ParseUnsigned(node, RAWXML_SPL, wxT("autolengthres")));
    c3DLoaderOrientation ori = ParseOrientation(node, wxString::Format(wxT("spl(%s) tag"), name.c_str()));
    wxLogVerbose(wxString::Format(_("Adding spl %s to %s."), wxString(spl.name.c_str(), wxConvLocal).c_str(), m_output.GetFullPath().c_str()));

    wxXmlNode *child = node->GetChildren();
    while (child) {
        DO_CONDITION_COMMENT(child);

        if (child->GetName() == RAWXML_SPL_NODE) {
            SplineNode sp;
            sp.pos.x = ParseFloat(child, RAWXML_SPL_NODE, wxT("x"));
            sp.pos.y = ParseFloat(child, RAWXML_SPL_NODE, wxT("y"));
            sp.pos.z = ParseFloat(child, RAWXML_SPL_NODE, wxT("z"));
            sp.cp1.x = ParseFloat(child, RAWXML_SPL_NODE, wxT("cp1x"));
            sp.cp1.y = ParseFloat(child, RAWXML_SPL_NODE, wxT("cp1y"));
            sp.cp1.z = ParseFloat(child, RAWXML_SPL_NODE, wxT("cp1z"));
            sp.cp2.x = ParseFloat(child, RAWXML_SPL_NODE, wxT("cp2x"));
            sp.cp2.y = ParseFloat(child, RAWXML_SPL_NODE, wxT("cp2y"));
            sp.cp2.z = ParseFloat(child, RAWXML_SPL_NODE, wxT("cp2z"));
            if (ori != ORIENTATION_LEFT_YUP) {
                doFixOrientation(sp.pos, ori);
                doFixOrientation(sp.cp1, ori);
                doFixOrientation(sp.cp2, ori);
            }
            spl.nodes.push_back(sp);
        } else if (child->GetName() == RAWXML_SPL_LENGTH) {
            float l = ParseFloat(child, RAWXML_SPL_LENGTH, wxT("length"));
            spl.lengths.push_back(l);
        } else if (child->GetName() == RAWXML_SPL_DATA) {
            cSplineData dt;
            wxString datastr = ParseString(child, RAWXML_SPL_DATA, wxT("data"), NULL);
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
        } else if COMPILER_WRONGTAG(child) {
            throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in spl(%s) tag."), child->GetName().c_str(), name.c_str()));
        }
        child = child->GetNext();
    }

    ovlSPLManager* c_spl = m_ovl.GetManager<ovlSPLManager>();
    c_spl->AddSpline(spl);
}

void cRawParser::ParseSTA(wxXmlNode* node) {
    USE_PREFIX(node);
    cStall stall;
    stall.name = ParseString(node, RAWXML_STA, wxT("name"), NULL, useprefix).ToAscii();
    stall.attraction.name = ParseString(node, RAWXML_STA, wxT("nametxt"), NULL, useprefix).ToAscii();
    stall.attraction.description = ParseString(node, RAWXML_STA, wxT("description"), NULL, useprefix).ToAscii();
    stall.sid = ParseString(node, RAWXML_STA, wxT("sid"), NULL, useprefix).ToAscii();
    wxLogVerbose(wxString::Format(_("Adding sta %s to %s."), wxString(stall.name.c_str(), wxConvLocal).c_str(), m_output.GetFullPath().c_str()));

    wxXmlNode *child = node->GetChildren();
    while (child) {
        DO_CONDITION_COMMENT(child);

        if (child->GetName() == RAWXML_ATTRACTION_BASE) {
            USE_PREFIX(child);
            OPTION_PARSE(unsigned long, stall.attraction.type, ParseUnsigned(child, RAWXML_ATTRACTION_BASE, wxT("type")));
            ParseStringOption(stall.attraction.icon, child, wxT("icon"), NULL, useprefix);
            ParseStringOption(stall.attraction.spline, child, wxT("spline"), NULL, useprefix);
        } else if (child->GetName() == RAWXML_STA_ITEM) {
            USE_PREFIX(child);
            cStallItem item;
            item.item = ParseString(child, RAWXML_STA_ITEM, wxT("cid"), NULL, useprefix).ToAscii();
            item.cost = ParseUnsigned(child, RAWXML_STA_ITEM, wxT("cost"));
            stall.items.push_back(item);
        } else if (child->GetName() == RAWXML_STA_STALLUNKNOWNS) {
            OPTION_PARSE(unsigned long, stall.unknowns.unk1, ParseUnsigned(child, RAWXML_STA_STALLUNKNOWNS, wxT("u1")));
            OPTION_PARSE(unsigned long, stall.unknowns.unk2, ParseUnsigned(child, RAWXML_STA_STALLUNKNOWNS, wxT("u2")));
            OPTION_PARSE(unsigned long, stall.unknowns.unk3, ParseUnsigned(child, RAWXML_STA_STALLUNKNOWNS, wxT("u3")));
            OPTION_PARSE(unsigned long, stall.unknowns.unk4, ParseUnsigned(child, RAWXML_STA_STALLUNKNOWNS, wxT("u4")));
            OPTION_PARSE(unsigned long, stall.unknowns.unk5, ParseUnsigned(child, RAWXML_STA_STALLUNKNOWNS, wxT("u5")));
            OPTION_PARSE(unsigned long, stall.unknowns.unk6, ParseUnsigned(child, RAWXML_STA_STALLUNKNOWNS, wxT("u6")));
        } else if (child->GetName() == RAWXML_ATTRACTION_UNKNOWNS) {
            OPTION_PARSE(unsigned long, stall.attraction.unk2, ParseUnsigned(child, RAWXML_ATTRACTION_UNKNOWNS, wxT("u2")));
            OPTION_PARSE(long, stall.attraction.unk3, ParseSigned(child, RAWXML_ATTRACTION_UNKNOWNS, wxT("u3")));
            OPTION_PARSE(unsigned long, stall.attraction.unk4, ParseUnsigned(child, RAWXML_ATTRACTION_UNKNOWNS, wxT("u4")));
            OPTION_PARSE(unsigned long, stall.attraction.unk5, ParseUnsigned(child, RAWXML_ATTRACTION_UNKNOWNS, wxT("u5")));
            OPTION_PARSE(unsigned long, stall.attraction.unk6, ParseUnsigned(child, RAWXML_ATTRACTION_UNKNOWNS, wxT("u6")));
            OPTION_PARSE(unsigned long, stall.attraction.unk9, ParseUnsigned(child, RAWXML_ATTRACTION_UNKNOWNS, wxT("u9")));
            OPTION_PARSE(long, stall.attraction.unk10, ParseSigned(child, RAWXML_ATTRACTION_UNKNOWNS, wxT("u10")));
            OPTION_PARSE(unsigned long, stall.attraction.unk11, ParseUnsigned(child, RAWXML_ATTRACTION_UNKNOWNS, wxT("u11")));
            OPTION_PARSE(unsigned long, stall.attraction.unk12, ParseUnsigned(child, RAWXML_ATTRACTION_UNKNOWNS, wxT("u12")));
            OPTION_PARSE(unsigned long, stall.attraction.unk13, ParseUnsigned(child, RAWXML_ATTRACTION_UNKNOWNS, wxT("u13")));
        } else if COMPILER_WRONGTAG(child) {
            throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in sta tag."), child->GetName().c_str()));
        }

        child = child->GetNext();
    }

    ovlSTAManager* c_sta = m_ovl.GetManager<ovlSTAManager>();
    c_sta->AddStall(stall);
}
