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



#include "RawOvlMake.h"

#include <wx/tokenzr.h>

#include "OVLManagers.h"
#include "RCT3Exception.h"
#include "RCT3Macros.h"
#include "SCNFile.h"
#include "gximage.h"
#include "rct3log.h"

#define RAWXML_SECTION wxT("section")

#define RAWXML_CED wxT("ced")
#define RAWXML_CHG wxT("chg")
#define RAWXML_CID wxT("cid")
#define RAWXML_GSI wxT("gsi")
#define RAWXML_SID wxT("sid")
#define RAWXML_SPL wxT("spl")
#define RAWXML_STA wxT("sta")
#define RAWXML_SVD wxT("svd")
#define RAWXML_TEX wxT("tex")
#define RAWXML_TXT wxT("txt")

#define RAWXML_CED_MORE             wxT("cedmore")

#define RAWXML_CHG_IS               wxT("chgis")

#define RAWXML_CID_SHAPE            wxT("cidshape")
#define RAWXML_CID_MORE             wxT("cidmore")
#define RAWXML_CID_EXTRA            wxT("cidextra")
#define RAWXML_CID_SETTINGS         wxT("cidsettings")
#define RAWXML_CID_TRASH            wxT("cidtrash")
#define RAWXML_CID_SOAKED           wxT("cidsoaked")
#define RAWXML_CID_SIZEUNK          wxT("cidsizeunknowns")
#define RAWXML_CID_UNK              wxT("cidunknowns")

#define RAWXML_SID_GROUP            wxT("sidgroup")
#define RAWXML_SID_TYPE             wxT("sidtype")
#define RAWXML_SID_POSITION         wxT("sidposition")
#define RAWXML_SID_COLOURS          wxT("sidcolours")
#define RAWXML_SID_FLAGS            wxT("sidflags")
#define RAWXML_SID_SVD              wxT("sidvisual")
#define RAWXML_SID_IMPORTERUNKNOWNS wxT("sidimporterunknowns")
#define RAWXML_SID_STALLUNKNOWNS    wxT("sidstallunknowns")
#define RAWXML_SID_PARAMETER        wxT("sidparameter")

#define RAWXML_SPL_NODE             wxT("splnode")
#define RAWXML_SPL_LENGTH           wxT("spllength")
#define RAWXML_SPL_DATA             wxT("spldata")

#define RAWXML_STA_IS               wxT("stais")
#define RAWXML_STA_ITEM             wxT("staitem")
#define RAWXML_STA_STALLUNKNOWNS    wxT("stastallunknowns")

#define RAWXML_ATTRACTIONUNKNOWNS   wxT("attractionunknowns")


#define RAWXML_IMPORT    wxT("import")
#define RAWXML_REFERENCE wxT("reference")
#define RAWXML_SYMBOL    wxT("symbol")

#define OPTION_PARSE(t, v, p) \
    try { \
        t x = p; \
        v = x; \
    } catch (RCT3InvalidValueException) { \
        throw; \
    } catch (RCT3Exception) {}

#define OPTION_PARSE_STRING(v, node, a) \
    { \
        wxString t = node->GetPropVal(a, wxT("")); \
        if (!t.IsEmpty()) \
            v = t.c_str(); \
    }

#define DO_CONDITION_COMMENT() \
        if (child->HasProp(wxT("if"))) { \
            if (m_options.Index(child->GetPropVal(wxT("if"), wxT(""))) == wxNOT_FOUND) { \
                child = child->GetNext(); \
                continue; \
            } \
        } \
        if (child->HasProp(wxT("ifnot"))) { \
            if (m_options.Index(child->GetPropVal(wxT("ifnot"), wxT(""))) != wxNOT_FOUND) { \
                child = child->GetNext(); \
                continue; \
            } \
        } \
        if (child->HasProp(wxT("comment"))) { \
            wxLogMessage(child->GetPropVal(wxT("comment"), wxT(""))); \
        }



class RCT3InvalidValueException: public RCT3Exception {
public:
    RCT3InvalidValueException(const wxString& message):RCT3Exception(message){};
};

void cRawOvl::Process(const wxFileName& file, const wxFileName& outputdir, const wxFileName& output) {
    m_input = file;
    m_output = output;
    if (outputdir == wxT("")) {
        m_outputbasedir.SetPath(file.GetPathWithSep());
    } else {
        m_outputbasedir.SetPath(outputdir.GetPathWithSep());
    }
    wxXmlDocument doc;
    if (!doc.Load(file.GetFullPath()))
        throw RCT3Exception(wxT("Error loading xml file ")+file.GetFullPath());

    wxXmlNode* root = doc.GetRoot();
    ParseConditions(root);
    Load(root);
}

void cRawOvl::Process(wxXmlNode* root, const wxFileName& file, const wxFileName& outputdir, const wxFileName& output) {
    m_input = file;
    m_output = output;
    if (outputdir == wxT("")) {
        m_outputbasedir.SetPath(file.GetPathWithSep());
    } else {
        m_outputbasedir.SetPath(outputdir.GetPathWithSep());
    }
    ParseConditions(root);
    Load(root);
}

void cRawOvl::AddConditions(const wxString& options) {
    wxStringTokenizer tok(options, wxT(" "));

    while (tok.HasMoreTokens()) {
        wxString t = tok.GetNextToken();
        if ((m_options.Index(t) == wxNOT_FOUND) && (t != wxT("inherit")))
            m_options.Add(t);
    }
}

void cRawOvl::AddConditions(const wxArrayString& options) {
    for (wxArrayString::const_iterator it = options.begin(); it != options.end(); ++it) {
        if ((m_options.Index(*it) == wxNOT_FOUND) && (*it != wxT("inherit")))
            m_options.Add(*it);
    }
}

void cRawOvl::AddConditions(const wxSortedArrayString& options) {
    for (wxSortedArrayString::const_iterator it = options.begin(); it != options.end(); ++it) {
        if ((m_options.Index(*it) == wxNOT_FOUND) && (*it != wxT("inherit")))
            m_options.Add(*it);
    }
}

void cRawOvl::RemoveConditions(const wxString& options) {
    wxStringTokenizer tok(options, wxT(" "));

    while (tok.HasMoreTokens()) {
        wxString t = tok.GetNextToken();
        if (m_options.Index(t) != wxNOT_FOUND)
            m_options.Remove(t);
    }
}

void cRawOvl::ParseConditions(const wxString& options) {
    wxStringTokenizer tok(options, wxT(" "));

    while (tok.HasMoreTokens()) {
        wxString t = tok.GetNextToken();
        if (t[0] == '-') {
            if (m_options.Index(t.Mid(1)) != wxNOT_FOUND)
                m_options.Remove(t.Mid(1));
        } else if (t[0] == '+') {
            if ((m_options.Index(t.Mid(1)) == wxNOT_FOUND) && (t.Mid(1) != wxT("inherit")))
                m_options.Add(t.Mid(1));
        } else {
            if ((m_options.Index(t) == wxNOT_FOUND) && (t != wxT("inherit")))
                m_options.Add(t);
        }
    }
}

void cRawOvl::ClearConditions() {
    m_options.Clear();
}

void cRawOvl::ParseConditions(wxXmlNode* node) {
    // Preset options overwrite attribute
    if (m_options.size() == 0) {
        if (node->HasProp(wxT("conditions"))) {
            wxString cond = node->GetPropVal(wxT("conditions"), wxT(""));
            ParseConditions(cond);
        }
    }
}

cOvlType cRawOvl::ParseType(wxXmlNode* node, const wxString& nodes, const wxString& attribute) {
    wxString t = node->GetPropVal(attribute, wxT(""));
    if (t.IsEmpty())
        throw RCT3Exception(nodes+_(" tag misses ") + attribute + _(" attribute."));
    if (t == wxT("common")) {
        return OVLT_COMMON;
    } else if (t == wxT("unique")) {
        return OVLT_UNIQUE;
    } else {
        throw RCT3InvalidValueException(nodes+_(" tag, ") + attribute + _(" attribute: invalid value ")+t);
    }
}

wxString cRawOvl::ParseString(wxXmlNode* node, const wxString& nodes, const wxString& attribute) {
    wxString t = node->GetPropVal(attribute, wxT(""));
    if (t.IsEmpty())
        throw RCT3Exception(nodes+_(" tag misses ") + attribute + _(" attribute."));
    return t;
}

unsigned long cRawOvl::ParseUnsigned(wxXmlNode* node, const wxString& nodes, const wxString& attribute) {
    wxString t = node->GetPropVal(attribute, wxT(""));
    unsigned long i;
    if (t.IsEmpty())
        throw RCT3Exception(nodes+_(" tag misses ") + attribute + _(" attribute."));
    if (!t.ToULong(&i))
        throw RCT3InvalidValueException(nodes+_(" tag, ") + attribute + _(" attribute: invalid value ")+t);
    return i;
}

long cRawOvl::ParseSigned(wxXmlNode* node, const wxString& nodes, const wxString& attribute) {
    wxString t = node->GetPropVal(attribute, wxT(""));
    long i;
    if (t.IsEmpty())
        throw RCT3Exception(nodes+_(" tag misses ") + attribute + _(" attribute."));
    if (!t.ToLong(&i))
        throw RCT3InvalidValueException(nodes+_(" tag, ") + attribute + _(" attribute: invalid value ")+t);
    return i;
}

double cRawOvl::ParseFloat(wxXmlNode* node, const wxString& nodes, const wxString& attribute) {
    wxString t = node->GetPropVal(attribute, wxT(""));
    t.Trim();
    double i;
    if (t.IsEmpty())
        throw RCT3Exception(nodes+_(" tag misses ") + attribute + _(" attribute."));
    if (!t.ToDouble(&i))
        throw RCT3InvalidValueException(nodes+_(" tag, ") + attribute + _(" attribute: invalid value ")+t);
    return i;
}

void cRawOvl::ParseCED(wxXmlNode* node) {
    cCarriedItemExtra ced;
    ced.name = ParseString(node, RAWXML_CED, wxT("name")).c_str();
    ced.nametxt = ParseString(node, RAWXML_CED, wxT("nametxt")).c_str();
    ced.icon = ParseString(node, RAWXML_CED, wxT("icon")).c_str();

    wxXmlNode *child = node->GetChildren();
    while (child) {
        DO_CONDITION_COMMENT();

        if (child->GetName() == RAWXML_CED_MORE) {
            OPTION_PARSE(float, ced.hunger, ParseFloat(child, RAWXML_CED_MORE, wxT("hunger")));
            OPTION_PARSE(float, ced.thirst, ParseFloat(child, RAWXML_CED_MORE, wxT("thirst")));
            OPTION_PARSE(unsigned long, ced.unk1, ParseUnsigned(child, RAWXML_CED_MORE, wxT("u1")));
            OPTION_PARSE(unsigned long, ced.unk4, ParseUnsigned(child, RAWXML_CED_MORE, wxT("u4")));
            OPTION_PARSE(float, ced.unk7, ParseFloat(child, RAWXML_CED_MORE, wxT("u7")));
        } else if COMPILER_WRONGTAG(child) {
            throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in ced tag."), child->GetName().c_str()));
        }
        child = child->GetNext();
    }

    ovlCEDManager* c_ced = m_ovl.GetManager<ovlCEDManager>();
    c_ced->AddExtra(ced);
}

void cRawOvl::ParseCHG(wxXmlNode* node) {
    cChangingRoom room;
    room.name = ParseString(node, RAWXML_CHG, wxT("name")).c_str();
    room.attraction.name = ParseString(node, RAWXML_CHG, wxT("nametxt")).c_str();
    room.attraction.description = ParseString(node, RAWXML_CHG, wxT("description")).c_str();
    room.sid = ParseString(node, RAWXML_CHG, wxT("sid")).c_str();
    room.spline = ParseString(node, RAWXML_CHG, wxT("roomspline")).c_str();

    wxXmlNode *child = node->GetChildren();
    while (child) {
        DO_CONDITION_COMMENT();

        if (child->GetName() == RAWXML_CHG_IS) {
            OPTION_PARSE(unsigned long, room.attraction.type, ParseUnsigned(child, RAWXML_CHG_IS, wxT("type")));
            OPTION_PARSE_STRING(room.attraction.icon, child, wxT("icon"));
            OPTION_PARSE_STRING(room.spline, child, wxT("attractionspline"));
        } else if (child->GetName() == RAWXML_ATTRACTIONUNKNOWNS) {
            OPTION_PARSE(unsigned long, room.attraction.unk2, ParseUnsigned(child, RAWXML_ATTRACTIONUNKNOWNS, wxT("u2")));
            OPTION_PARSE(long, room.attraction.unk3, ParseSigned(child, RAWXML_ATTRACTIONUNKNOWNS, wxT("u3")));
            OPTION_PARSE(unsigned long, room.attraction.unk4, ParseUnsigned(child, RAWXML_ATTRACTIONUNKNOWNS, wxT("u4")));
            OPTION_PARSE(unsigned long, room.attraction.unk5, ParseUnsigned(child, RAWXML_ATTRACTIONUNKNOWNS, wxT("u5")));
            OPTION_PARSE(unsigned long, room.attraction.unk6, ParseUnsigned(child, RAWXML_ATTRACTIONUNKNOWNS, wxT("u6")));
            OPTION_PARSE(unsigned long, room.attraction.unk7, ParseUnsigned(child, RAWXML_ATTRACTIONUNKNOWNS, wxT("u7")));
            OPTION_PARSE(unsigned long, room.attraction.unk8, ParseUnsigned(child, RAWXML_ATTRACTIONUNKNOWNS, wxT("u8")));
            OPTION_PARSE(unsigned long, room.attraction.unk9, ParseUnsigned(child, RAWXML_ATTRACTIONUNKNOWNS, wxT("u9")));
            OPTION_PARSE(long, room.attraction.unk10, ParseSigned(child, RAWXML_ATTRACTIONUNKNOWNS, wxT("u10")));
            OPTION_PARSE(unsigned long, room.attraction.unk11, ParseUnsigned(child, RAWXML_ATTRACTIONUNKNOWNS, wxT("u11")));
            OPTION_PARSE(unsigned long, room.attraction.unk12, ParseUnsigned(child, RAWXML_ATTRACTIONUNKNOWNS, wxT("u12")));
            OPTION_PARSE(unsigned long, room.attraction.unk13, ParseUnsigned(child, RAWXML_ATTRACTIONUNKNOWNS, wxT("u13")));
        } else if COMPILER_WRONGTAG(child) {
            throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in sta tag."), child->GetName().c_str()));
        }

        child = child->GetNext();
    }

    ovlCHGManager* c_chg = m_ovl.GetManager<ovlCHGManager>();
    c_chg->AddRoom(room);
}

void cRawOvl::ParseCID(wxXmlNode* node) {
    cCarriedItem cid;
    cid.name = ParseString(node, RAWXML_CID, wxT("name")).c_str();
    cid.nametxt = ParseString(node, RAWXML_CID, wxT("nametxt")).c_str();
    cid.pluralnametxt = ParseString(node, RAWXML_CID, wxT("pluralnametxt")).c_str();

    wxXmlNode *child = node->GetChildren();
    while (child) {
        DO_CONDITION_COMMENT();

        if (child->GetName() == RAWXML_CID_SHAPE) {
            cid.shape.shaped = ParseUnsigned(child, RAWXML_CID_SHAPE, wxT("shaped"));
            if (cid.shape.shaped) {
                cid.shape.shape1 = ParseString(child, RAWXML_CID_SHAPE, wxT("shape1"));
                cid.shape.shape2 = ParseString(child, RAWXML_CID_SHAPE, wxT("shape1"));
            } else {
                cid.shape.MakeBillboard();
                cid.shape.fts = ParseString(child, RAWXML_CID_SHAPE, wxT("ftx"));
            }
            OPTION_PARSE(float, cid.shape.unk9, ParseFloat(child, RAWXML_CID_SHAPE, wxT("distance")));
            OPTION_PARSE(unsigned long, cid.shape.defaultcolour, ParseUnsigned(child, RAWXML_CID_SHAPE, wxT("defaultcolour")));
            OPTION_PARSE(float, cid.shape.scalex, ParseFloat(child, RAWXML_CID_SHAPE, wxT("scalex")));
            OPTION_PARSE(float, cid.shape.scaley, ParseFloat(child, RAWXML_CID_SHAPE, wxT("scaley")));
        } else if (child->GetName() == RAWXML_CID_MORE) {
            OPTION_PARSE(unsigned long, cid.addonpack, ParseUnsigned(child, RAWXML_CID_MORE, wxT("addonpack")));
            OPTION_PARSE_STRING(cid.icon, child, wxT("icon"));
        } else if (child->GetName() == RAWXML_CID_EXTRA) {
            wxString extra = ParseString(child, RAWXML_CID_EXTRA, wxT("name"));
            cid.extras.push_back(extra.c_str());
        } else if (child->GetName() == RAWXML_CID_SETTINGS) {
            OPTION_PARSE(unsigned long, cid.settings.flags, ParseUnsigned(child, RAWXML_CID_SETTINGS, wxT("flags")));
            OPTION_PARSE(long, cid.settings.ageclass, ParseSigned(child, RAWXML_CID_SETTINGS, wxT("ageclass")));
            OPTION_PARSE(unsigned long, cid.settings.type, ParseUnsigned(child, RAWXML_CID_SETTINGS, wxT("type")));
            OPTION_PARSE(float, cid.settings.hunger, ParseFloat(child, RAWXML_CID_SETTINGS, wxT("hunger")));
            OPTION_PARSE(float, cid.settings.thirst, ParseFloat(child, RAWXML_CID_SETTINGS, wxT("thirst")));
            OPTION_PARSE(float, cid.settings.lightprotectionfactor, ParseFloat(child, RAWXML_CID_SETTINGS, wxT("lightprotectionfactor")));
        } else if (child->GetName() == RAWXML_CID_TRASH) {
            OPTION_PARSE_STRING(cid.trash.wrapper, child, wxT("wrapper"));
            OPTION_PARSE(float, cid.trash.trash1, ParseFloat(child, RAWXML_CID_TRASH, wxT("trash1")));
            OPTION_PARSE(long, cid.trash.trash2, ParseSigned(child, RAWXML_CID_TRASH, wxT("trash2")));
        } else if (child->GetName() == RAWXML_CID_SOAKED) {
            OPTION_PARSE_STRING(cid.soaked.male_morphmesh_body, child, wxT("malebody"));
            OPTION_PARSE_STRING(cid.soaked.male_morphmesh_legs, child, wxT("malelegs"));
            OPTION_PARSE_STRING(cid.soaked.female_morphmesh_body, child, wxT("femalebody"));
            OPTION_PARSE_STRING(cid.soaked.female_morphmesh_legs, child, wxT("femalelegs"));
            OPTION_PARSE(unsigned long, cid.soaked.unk38, ParseUnsigned(child, RAWXML_CID_SOAKED, wxT("unknown")));
            OPTION_PARSE_STRING(cid.soaked.textureoption, child, wxT("textureoption"));
        } else if (child->GetName() == RAWXML_CID_SIZEUNK) {
            OPTION_PARSE(unsigned long, cid.size.unk17, ParseUnsigned(child, RAWXML_CID_SIZEUNK, wxT("u1")));
            OPTION_PARSE(unsigned long, cid.size.unk18, ParseUnsigned(child, RAWXML_CID_SIZEUNK, wxT("u2")));
            OPTION_PARSE(unsigned long, cid.size.unk19, ParseUnsigned(child, RAWXML_CID_SIZEUNK, wxT("u3")));
            OPTION_PARSE(unsigned long, cid.size.unk20, ParseUnsigned(child, RAWXML_CID_SIZEUNK, wxT("u4")));
        } else if (child->GetName() == RAWXML_CID_UNK) {
            OPTION_PARSE(unsigned long, cid.unknowns.unk1, ParseUnsigned(child, RAWXML_CID_UNK, wxT("u1")));
            OPTION_PARSE(unsigned long, cid.unknowns.unk3, ParseUnsigned(child, RAWXML_CID_UNK, wxT("u3")));
            OPTION_PARSE(unsigned long, cid.unknowns.unk26, ParseUnsigned(child, RAWXML_CID_UNK, wxT("u26")));
            OPTION_PARSE(unsigned long, cid.unknowns.unk27, ParseUnsigned(child, RAWXML_CID_UNK, wxT("u27")));
            OPTION_PARSE(unsigned long, cid.unknowns.unk28, ParseUnsigned(child, RAWXML_CID_UNK, wxT("u28")));
            OPTION_PARSE(unsigned long, cid.unknowns.unk33, ParseUnsigned(child, RAWXML_CID_UNK, wxT("u33")));
        } else if COMPILER_WRONGTAG(child) {
            throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in cid tag."), child->GetName().c_str()));
        }
        child = child->GetNext();
    }

    ovlCIDManager* c_cid = m_ovl.GetManager<ovlCIDManager>();
    c_cid->AddItem(cid);
}

void cRawOvl::ParseSID(wxXmlNode* node) {
    cSid sid;
    sid.name = ParseString(node, RAWXML_SID, wxT("name")).c_str();
    sid.ovlpath = ParseString(node, RAWXML_SID, wxT("ovlpath")).c_str();
    sid.ui.name = ParseString(node, RAWXML_SID, wxT("nametxt")).c_str();
    sid.ui.icon = ParseString(node, RAWXML_SID, wxT("icon")).c_str();
    wxString svd = ParseString(node, RAWXML_SID, wxT("svd"));
    sid.svds.push_back(svd.c_str());

    wxXmlNode *child = node->GetChildren();
    while (child) {
        DO_CONDITION_COMMENT();

        if (child->GetName() == RAWXML_SID_GROUP) {
            wxString name = ParseString(child, RAWXML_SID_GROUP, wxT("name"));
            wxString icon = ParseString(child, RAWXML_SID_GROUP, wxT("icon"));
            sid.ui.group = name.c_str();
            sid.ui.groupicon = icon.c_str();
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
            OPTION_PARSE_STRING(sid.position.supports, child, wxT("supports"));
        } else if (child->GetName() == RAWXML_SID_COLOURS) {
            OPTION_PARSE(unsigned long, sid.colours.defaultcol[0], ParseUnsigned(child, RAWXML_SID_COLOURS, wxT("choice1")));
            OPTION_PARSE(unsigned long, sid.colours.defaultcol[1], ParseUnsigned(child, RAWXML_SID_COLOURS, wxT("choice2")));
            OPTION_PARSE(unsigned long, sid.colours.defaultcol[2], ParseUnsigned(child, RAWXML_SID_COLOURS, wxT("choice3")));
        } else if (child->GetName() == RAWXML_SID_FLAGS) {
            wxString flags = ParseString(child, RAWXML_SID_FLAGS, wxT("set"));
            if (flags.Length() != 64)
                throw RCT3InvalidValueException(_("The set attribute of a sidflags tag is of invalid length"));
            const char* fl = flags.c_str();
            for (int i = 0; i < 64; ++i) {
                if (fl[i] == '0') {
                    sid.flags.flag[i] = false;
                } else if (fl[i] == '1') {
                    sid.flags.flag[i] = true;
                } else {
                    throw RCT3InvalidValueException(_("The set attribute of a sidflags tag has an invalid character"));
                }
            }
        } else if (child->GetName() == RAWXML_SID_IMPORTERUNKNOWNS) {
            OPTION_PARSE(unsigned long, sid.importerunknowns.unk1, ParseUnsigned(child, RAWXML_SID_IMPORTERUNKNOWNS, wxT("u1")));
            OPTION_PARSE(unsigned long, sid.importerunknowns.unk2, ParseUnsigned(child, RAWXML_SID_IMPORTERUNKNOWNS, wxT("u2")));
            OPTION_PARSE(unsigned long, sid.importerunknowns.unk6, ParseUnsigned(child, RAWXML_SID_IMPORTERUNKNOWNS, wxT("u6")));
            OPTION_PARSE(unsigned long, sid.importerunknowns.unk7, ParseUnsigned(child, RAWXML_SID_IMPORTERUNKNOWNS, wxT("u7")));
            if (child->HasProp(wxT("u8"))) {
                OPTION_PARSE(unsigned long, sid.importerunknowns.unk8, ParseUnsigned(child, RAWXML_SID_IMPORTERUNKNOWNS, wxT("u8")));
                sid.importerunknowns.use_unk8 = true;
            }
            OPTION_PARSE(unsigned long, sid.importerunknowns.unk9, ParseUnsigned(child, RAWXML_SID_IMPORTERUNKNOWNS, wxT("u9")));
        } else if (child->GetName() == RAWXML_SID_STALLUNKNOWNS) {
            sid.stallunknowns.MakeStall();
            OPTION_PARSE(unsigned long, sid.stallunknowns.unk1, ParseUnsigned(child, RAWXML_SID_STALLUNKNOWNS, wxT("u1")));
            OPTION_PARSE(unsigned long, sid.stallunknowns.unk2, ParseUnsigned(child, RAWXML_SID_STALLUNKNOWNS, wxT("u2")));
        } else if (child->GetName() == RAWXML_SID_SVD) {
            svd = ParseString(node, RAWXML_SVD, wxT("name"));
            sid.svds.push_back(svd.c_str());
        } else if (child->GetName() == RAWXML_SID_PARAMETER) {
            cSidParam param;
            param.name = ParseString(child, RAWXML_SID_GROUP, wxT("name")).c_str();
            OPTION_PARSE_STRING(param.param, child, wxT("param"));
            sid.parameters.push_back(param);
        } else if COMPILER_WRONGTAG(child) {
            throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in sid tag."), child->GetName().c_str()));
        }

        child = child->GetNext();
    }

    ovlSIDManager* c_sid = m_ovl.GetManager<ovlSIDManager>();
    c_sid->AddSID(sid);
}

unsigned char ParseDigit(char x) {
    switch (x) {
        case '0':
            return 0;
        case '1':
            return 1;
        case '2':
            return 2;
        case '3':
            return 3;
        case '4':
            return 4;
        case '5':
            return 5;
        case '6':
            return 6;
        case '7':
            return 7;
        case '8':
            return 8;
        case '9':
            return 9;
        case 'a':
            return 10;
        case 'b':
            return 11;
        case 'c':
            return 12;
        case 'd':
            return 13;
        case 'e':
            return 14;
        case 'f':
            return 15;
        case 'A':
            return 10;
        case 'B':
            return 11;
        case 'C':
            return 12;
        case 'D':
            return 13;
        case 'E':
            return 14;
        case 'F':
            return 15;
        default:
            return 16;
    }
}

void cRawOvl::ParseSPL(wxXmlNode* node) {
    cSpline spl;
    spl.name = ParseString(node, RAWXML_SPL, wxT("name")).c_str();
    spl.unk3 = ParseFloat(node, RAWXML_SPL, wxT("u3"));
    OPTION_PARSE(float, spl.unk6, ParseFloat(node, RAWXML_SPL, wxT("u6")));

    wxXmlNode *child = node->GetChildren();
    while (child) {
        DO_CONDITION_COMMENT();

        if (child->GetName() == RAWXML_SPL_NODE) {
            SplineNode sp;
            sp.x = ParseFloat(child, RAWXML_SPL_NODE, wxT("x"));
            sp.y = ParseFloat(child, RAWXML_SPL_NODE, wxT("y"));
            sp.z = ParseFloat(child, RAWXML_SPL_NODE, wxT("z"));
            sp.cp1x = ParseFloat(child, RAWXML_SPL_NODE, wxT("cp1x"));
            sp.cp1y = ParseFloat(child, RAWXML_SPL_NODE, wxT("cp1y"));
            sp.cp1z = ParseFloat(child, RAWXML_SPL_NODE, wxT("cp1z"));
            sp.cp2x = ParseFloat(child, RAWXML_SPL_NODE, wxT("cp2x"));
            sp.cp2y = ParseFloat(child, RAWXML_SPL_NODE, wxT("cp2y"));
            sp.cp2z = ParseFloat(child, RAWXML_SPL_NODE, wxT("cp2z"));
            spl.nodes.push_back(sp);
        } else if (child->GetName() == RAWXML_SPL_LENGTH) {
            float l = ParseFloat(child, RAWXML_SPL_LENGTH, wxT("length"));
            spl.lengths.push_back(l);
        } else if (child->GetName() == RAWXML_SPL_DATA) {
            cSplineData dt;
            wxString datastr = ParseString(child, RAWXML_SPL_DATA, wxT("data"));
            if (datastr.Length() != 28)
                throw RCT3InvalidValueException(_("The data attribute of a spldata tag is of invalid length"));
            const char* d = datastr.c_str();
            for (int i = 0; i < 28; i+=2) {
                unsigned char dta = 0;
                unsigned char t = ParseDigit(d[i]);
                if (t >= 16)
                    throw RCT3InvalidValueException(_("The data attribute of a spldata tag has an invalid character"));
                dta += t * 16;
                t = ParseDigit(d[i+1]);
                if (t >= 16)
                    throw RCT3InvalidValueException(_("The data attribute of a spldata tag has an invalid character"));
                dta += t;
                dt.data[i/2] = dta;
            }
            spl.unknowndata.push_back(dt);
        } else if COMPILER_WRONGTAG(child) {
            throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in ced tag."), child->GetName().c_str()));
        }
        child = child->GetNext();
    }

    ovlSPLManager* c_spl = m_ovl.GetManager<ovlSPLManager>();
    c_spl->AddSpline(spl);
}

void cRawOvl::ParseSTA(wxXmlNode* node) {
    cStall stall;
    stall.name = ParseString(node, RAWXML_SID, wxT("name")).c_str();
    stall.attraction.name = ParseString(node, RAWXML_SID, wxT("nametxt")).c_str();
    stall.attraction.description = ParseString(node, RAWXML_SID, wxT("description")).c_str();
    stall.sid = ParseString(node, RAWXML_SID, wxT("sid")).c_str();

    wxXmlNode *child = node->GetChildren();
    while (child) {
        DO_CONDITION_COMMENT();

        if (child->GetName() == RAWXML_STA_IS) {
            OPTION_PARSE(unsigned long, stall.attraction.type, ParseUnsigned(child, RAWXML_STA_IS, wxT("type")));
            OPTION_PARSE_STRING(stall.attraction.icon, child, wxT("icon"));
            OPTION_PARSE_STRING(stall.attraction.spline, child, wxT("spline"));
        } else if (child->GetName() == RAWXML_STA_ITEM) {
            cStallItem item;
            item.item = ParseString(child, RAWXML_STA_ITEM, wxT("cid")).c_str();
            item.cost = ParseUnsigned(child, RAWXML_STA_ITEM, wxT("cost"));
            stall.items.push_back(item);
        } else if (child->GetName() == RAWXML_STA_STALLUNKNOWNS) {
            OPTION_PARSE(unsigned long, stall.unknowns.unk1, ParseUnsigned(child, RAWXML_STA_STALLUNKNOWNS, wxT("u1")));
            OPTION_PARSE(unsigned long, stall.unknowns.unk2, ParseUnsigned(child, RAWXML_STA_STALLUNKNOWNS, wxT("u2")));
            OPTION_PARSE(unsigned long, stall.unknowns.unk3, ParseUnsigned(child, RAWXML_STA_STALLUNKNOWNS, wxT("u3")));
            OPTION_PARSE(unsigned long, stall.unknowns.unk4, ParseUnsigned(child, RAWXML_STA_STALLUNKNOWNS, wxT("u4")));
            OPTION_PARSE(unsigned long, stall.unknowns.unk5, ParseUnsigned(child, RAWXML_STA_STALLUNKNOWNS, wxT("u5")));
            OPTION_PARSE(unsigned long, stall.unknowns.unk6, ParseUnsigned(child, RAWXML_STA_STALLUNKNOWNS, wxT("u6")));
        } else if (child->GetName() == RAWXML_ATTRACTIONUNKNOWNS) {
            OPTION_PARSE(unsigned long, stall.attraction.unk2, ParseUnsigned(child, RAWXML_ATTRACTIONUNKNOWNS, wxT("u2")));
            OPTION_PARSE(long, stall.attraction.unk3, ParseSigned(child, RAWXML_ATTRACTIONUNKNOWNS, wxT("u3")));
            OPTION_PARSE(unsigned long, stall.attraction.unk4, ParseUnsigned(child, RAWXML_ATTRACTIONUNKNOWNS, wxT("u4")));
            OPTION_PARSE(unsigned long, stall.attraction.unk5, ParseUnsigned(child, RAWXML_ATTRACTIONUNKNOWNS, wxT("u5")));
            OPTION_PARSE(unsigned long, stall.attraction.unk6, ParseUnsigned(child, RAWXML_ATTRACTIONUNKNOWNS, wxT("u6")));
            OPTION_PARSE(unsigned long, stall.attraction.unk7, ParseUnsigned(child, RAWXML_ATTRACTIONUNKNOWNS, wxT("u7")));
            OPTION_PARSE(unsigned long, stall.attraction.unk8, ParseUnsigned(child, RAWXML_ATTRACTIONUNKNOWNS, wxT("u8")));
            OPTION_PARSE(unsigned long, stall.attraction.unk9, ParseUnsigned(child, RAWXML_ATTRACTIONUNKNOWNS, wxT("u9")));
            OPTION_PARSE(long, stall.attraction.unk10, ParseSigned(child, RAWXML_ATTRACTIONUNKNOWNS, wxT("u10")));
            OPTION_PARSE(unsigned long, stall.attraction.unk11, ParseUnsigned(child, RAWXML_ATTRACTIONUNKNOWNS, wxT("u11")));
            OPTION_PARSE(unsigned long, stall.attraction.unk12, ParseUnsigned(child, RAWXML_ATTRACTIONUNKNOWNS, wxT("u12")));
            OPTION_PARSE(unsigned long, stall.attraction.unk13, ParseUnsigned(child, RAWXML_ATTRACTIONUNKNOWNS, wxT("u13")));
        } else if COMPILER_WRONGTAG(child) {
            throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in sta tag."), child->GetName().c_str()));
        }

        child = child->GetNext();
    }

    ovlSTAManager* c_sta = m_ovl.GetManager<ovlSTAManager>();
    c_sta->AddStall(stall);
}

void cRawOvl::ParseTEX(wxXmlNode* node) {
    wxString name = ParseString(node, RAWXML_TEX, wxT("name"));
    wxFileName texture = ParseString(node, RAWXML_TEX, wxT("texture"));
    if (!texture.IsAbsolute())
        texture.MakeAbsolute(m_input.GetPathWithSep());
    unsigned char* data = NULL;
    if (!texture.IsFileReadable())
        throw RCT3Exception(_("tex tag: File not readable: ") + texture.GetFullPath());

    try {
        wxGXImage img(texture.GetFullPath(), false);

        int width = img.GetWidth();
        int height = img.GetHeight();
        if (width != height) {
            throw Magick::Exception("Icon texture is not square");
        }
        if ((1 << local_log2(width)) != width) {
            throw Magick::Exception("Icon texture's width/height is not a power of 2");
        }
        img.flip();
        img.dxtCompressionMethod(squish::kColourIterativeClusterFit | squish::kWeightColourByAlpha);
        int datasize = img.GetDxtBufferSize(wxDXT3);
        data = new unsigned char[datasize];
        img.DxtCompress(data, wxDXT3);

        ovlTEXManager* c_tex = m_ovl.GetManager<ovlTEXManager>();
        c_tex->AddTexture(name.c_str(), width, datasize, reinterpret_cast<unsigned long*>(data));
    } catch (Magick::Exception& e) {
        delete[] data;
        throw RCT3Exception(wxString::Format(_("tex tag: GarphicsMagik exception: %s"), e.what()));
    }
    delete[] data;
}

void cRawOvl::Parse(wxXmlNode* node) {
    wxXmlNode *child = node->GetChildren();
    while (child) {
        DO_CONDITION_COMMENT();

        if (child->GetName() == RAWXML_SECTION) {
            Parse(child);
        } else if (child->GetName() == RAWXML_SUBROOT) {
            // see above or
            // <subovl include="raw xml file" />
            wxString incfile = child->GetPropVal(wxT("include"), wxT(""));

            cRawOvl c_raw;
            c_raw.SetOptions(m_install, m_dryrun);
            if (child->HasProp(wxT("conditions"))) {
                wxString cond = node->GetPropVal(wxT("conditions"), wxT(""));
                if (cond.StartsWith(wxT("inherit"))) {
                    c_raw.AddConditions(m_options);
                }
                c_raw.ParseConditions(cond);
            } else {
                c_raw.AddConditions(m_options);
            }
            if (incfile.IsEmpty()) {
                c_raw.Process(child, m_input, m_outputbasedir);
            } else {
                c_raw.Process(incfile, m_input, m_outputbasedir);
            }
            for (std::vector<wxFileName>::const_iterator it = c_raw.GetDryrun().begin(); it != c_raw.GetDryrun().end(); ++it) {
                m_dryrunfiles.push_back(*it);
            }
        } else if (m_dryrun) {
            // The rest creates the file
        } else if (child->GetName() == RAWXML_IMPORT) {
            // <import file="scenery file" (name="internal svd name") />
            wxFileName filename = ParseString(child, RAWXML_IMPORT, wxT("file"));
            wxString name = child->GetPropVal(wxT("name"), wxT(""));
            if (!filename.IsAbsolute())
                filename.MakeAbsolute(m_input.GetPathWithSep());
            cSCNFile c_scn(filename.GetFullPath());
            if (!name.IsEmpty()) {
                c_scn.name = name;
            }
            c_scn.MakeToOvl(m_ovl);
        } else if (child->GetName() == RAWXML_CED) {
            ParseCED(child);
        } else if (child->GetName() == RAWXML_CHG) {
            ParseCHG(child);
        } else if (child->GetName() == RAWXML_CID) {
            ParseCID(child);
        } else if (child->GetName() == RAWXML_GSI) {
            // <gsi name="string" tex="string" top="long int" left="long int" bottom="long int" right="long int" />
            wxString name = ParseString(child, RAWXML_GSI, wxT("name"));
            wxString tex = ParseString(child, RAWXML_GSI, wxT("tex"));
            unsigned long top = ParseUnsigned(child, RAWXML_GSI, wxT("top"));
            unsigned long left = ParseUnsigned(child, RAWXML_GSI, wxT("left"));
            unsigned long bottom = ParseUnsigned(child, RAWXML_GSI, wxT("bottom"));
            unsigned long right = ParseUnsigned(child, RAWXML_GSI, wxT("right"));

            ovlGSIManager* c_gsi = m_ovl.GetManager<ovlGSIManager>();
            c_gsi->AddItem(name.c_str(), tex.c_str(), left, top, right, bottom);
        } else if (child->GetName() == RAWXML_SID) {
            // <sid name="string" ovlpath="ovl path, relative to install dir" nametxt="txt" icon="gsi" svd="svd">
            //     (All following tags and attributes are optional, if not mentioned otherwise)
            //     <sidgroup name="txt" icon="gsi" /> (both name and gsi must be present)
            //     <sidtype scenerytype="long int" cost="long int" removalcost="long int" />
            //       (costs are signed integers)
            //     <sidposition type="short int" xsquares="long int" ysquares="long int"
            //               xpos="float" ypos="float" zpos="float"
            //               xsize="float" ysize="float" zsize="float"
            //               supports="string" />
            //     <sidcolours choice1="long int" choice2="long int" choice3="long int" />
            //     <sidflags set="1010101010101010101010101010101010101010101010101010101010101010" />
            //       (flags are all or nothing, 64 0's and 1's. 1 set, 0 not set. 1-64 from left to right)
            //     <sidimporterunknowns u1="long int" u2="long int" u6="long int" u7="long int" u8="long int" u9="long int" />
            //       (if you leave out u8, it will be disabled)
            //     <sidstallunknowns u1="long int" u2="long int" />
            //       (if you leave out both attributes, the stall defaults will be used (1440/2880))
            //     (The next tags can be used multiple times)
            //     <sidvisual name="svd" />
            //       (add another svd to the sid. As far as I know only used for trees (2d/3d versions).
            //     <sidparameter name="string" param="string" />
            //       (add a parameter. name is required, param is optional in most cases. If present, it usually starts with a space.)
            // </sid>
            ParseSID(child);
        } else if (child->GetName() == RAWXML_SPL) {
            ParseSPL(child);
        } else if (child->GetName() == RAWXML_STA) {
            // <sta name="string" nametxt="txt" description="txt" sid="sid">
            //     <stais type="long int" icon="gsi" spline="spl" />
            //     <staitem cid="cid" cost="long int" />
            //     <stastallunknowns u1="long int" u2="long int" u3="long int" u4="long int" u5="long int" u6="long int" />
            // </sta>
            ParseSTA(child);
        } else if (child->GetName() == RAWXML_TEX) {
            // <tex name="string" texture="texture file" />
            ParseTEX(child);
        } else if (child->GetName() == RAWXML_TXT) {
            // <txt name="string" text="string" />
            wxString name = ParseString(child, RAWXML_TXT, wxT("name"));
            wxString text = ParseString(child, RAWXML_TXT, wxT("text"));

            ovlTXTManager* c_txt = m_ovl.GetManager<ovlTXTManager>();
            c_txt->AddText(name.c_str(), text.c_str());
        } else if (child->GetName() == RAWXML_REFERENCE) {
            // <reference path="relative path" />
            wxString ref = child->GetPropVal(wxT("path"), wxT(""));
            if (ref.IsEmpty())
                throw RCT3Exception(_("REFERENCE tag misses path attribute."));
            m_ovl.AddReference(ref);
        } else if (child->GetName() == RAWXML_SYMBOL) {
            // <symbol target="common|unique" name="string" type="int|float" data="data" />
            cOvlType target = ParseType(child, RAWXML_SYMBOL);
            wxString name = ParseString(child, RAWXML_SYMBOL, wxT("name"));
            wxString type = ParseString(child, RAWXML_SYMBOL, wxT("type"));
            unsigned long data;
            if (type == wxT("int")) {
                name += wxT(":int");
                data = ParseUnsigned(child, RAWXML_SYMBOL, wxT("data"));
            } else if (type == wxT("float")) {
                name += wxT(":flt");
                float f = ParseFloat(child, RAWXML_SYMBOL, wxT("data"));
                data = *reinterpret_cast<unsigned long*>(&f);
            } else {
                throw RCT3Exception(_("symbol tag has unimplemented type value."));
            }
            m_ovl.AddDataSymbol(target, name.c_str(), data);
        } else if COMPILER_WRONGTAG(child) {
            throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in rawovl tag."), child->GetName().c_str()));
        }

        child = child->GetNext();
    }

}

void cRawOvl::Load(wxXmlNode* root) {
    if (!root)
        throw RCT3Exception(wxT("cRawOvl::Load, root is null"));
    bool subonly = false;

    if ((root->GetName() == RAWXML_ROOT) || (root->GetName() == RAWXML_SUBROOT)) {
        // <rawovl|subovl file="outputfile" basedir="dir outputfile is relative to">
        {
            wxString basedir = root->GetPropVal(wxT("basedir"), wxT(""));
            if (m_install && root->HasProp(wxT("installdir"))) {
                basedir = root->GetPropVal(wxT("installdir"), wxT(""));
            }
            if (!basedir.IsEmpty()) {
                wxFileName temp;
                temp.SetPath(basedir);
                if (!temp.IsAbsolute()) {
                    temp.MakeAbsolute(m_outputbasedir.GetPathWithSep());
                }
                m_outputbasedir = temp;
            }
        }

        if ((!m_outputbasedir.DirExists()) && (!m_dryrun)) {
            //if (!::wxMkDir(m_outputbasedir.GetPathWithSep()))
            //    throw RCT3Exception(_("Failed to create directory: ")+m_outputbasedir.GetPathWithSep());
            //m_outputbasedir.Mkdir(0777, wxPATH_MKDIR_FULL);
            if (!m_outputbasedir.Mkdir(0777, wxPATH_MKDIR_FULL))
                throw RCT3Exception(_("Failed to create directory: ")+m_output.GetPathWithSep());
        }
        if (m_output == wxT("")) {
            /*
            wxString op;
            if (!root->GetPropVal(wxT("file"), &op)) {
                // No guessing output name
                throw RCT3Exception(wxT("cRawOvl::Load, no output file given"));
            }
            m_output = op;
            */
            m_output = root->GetPropVal(wxT("file"), wxT(""));
        }

        if (m_output != wxT("")) {
            if (!m_output.IsAbsolute())
                m_output.MakeAbsolute(m_outputbasedir.GetPathWithSep());
            m_output.SetExt(wxT(""));
            if (m_output.GetName().EndsWith(wxT(".common"))) {
                wxString temp = m_output.GetName();
                temp.Replace(wxT(".common"), wxT(""), true);
                m_output.SetName(temp);
            }

            // Cannot check writability as we don't create dirs
            if (!m_dryrun) {
                if (!m_output.DirExists()) {
                    //if (!::wxMkDir(m_output.GetPathWithSep()))
                    //    throw RCT3Exception(_("Failed to create directory: ")+m_output.GetPathWithSep());
                    // Doesn't report success correctly -.-
                    //::wxMkDir(m_output.GetPathWithSep());
                    if (!m_output.Mkdir(0777, wxPATH_MKDIR_FULL))
                        throw RCT3Exception(_("Failed to create directory: ")+m_output.GetPathWithSep());
                }

                {
                    wxFileName temp = m_output;
                    temp.SetExt(wxT("common.ovl"));
                    if (temp.GetPath().IsEmpty())
                        temp.SetPath(wxT("."));
                    if (temp.FileExists()) {
                        if (!temp.IsFileWritable())
                            throw RCT3Exception(_("Cannot write output file ")+temp.GetFullPath());
                    } else {
                        if (!temp.IsDirWritable())
                            throw RCT3Exception(_("Cannot write output file ")+temp.GetFullPath());
                    }
                }
            }

            if (m_dryrun) {
                m_dryrunfiles.push_back(m_output);
            } else {
                m_ovl.Init(m_output.GetFullPath().c_str());
            }
            wxLogMessage(_("Writing ovl from raw: ")+m_output.GetFullPath()+wxT(".common.ovl"));
        } else {
            subonly = true;
            wxLogMessage(_("Parsing subovl-only raw. Base output dir: ")+m_outputbasedir.GetPathWithSep());
        }

        // Start parsing
        Parse(root);

        if ((!subonly) && (!m_dryrun)) {
            m_ovl.Save();
            wxLogMessage(_("OVL written successfully"));
        }
    } else {
        throw RCT3Exception(wxT("cRawOvl::Load, wrong root"));
    }

}
