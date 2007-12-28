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

#include <wx/strconv.h>
#include <wx/tokenzr.h>

#include "base64.h"
#include "counted_array_ptr.h"
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

#define RAWXML_STA_ITEM             wxT("staitem")
#define RAWXML_STA_STALLUNKNOWNS    wxT("stastallunknowns")

#define RAWXML_TEX_FILE             wxT("texturefile")
#define RAWXML_TEX_DATA             wxT("texturedata")

#define RAWXML_ATTRACTION_BASE      wxT("attractionbase")
#define RAWXML_ATTRACTION_UNKNOWNS  wxT("attractionunknowns")


#define RAWXML_IMPORT    wxT("import")
#define RAWXML_REFERENCE wxT("reference")
#define RAWXML_SYMBOL    wxT("symbol")

#define RAWBAKE_ABSOLUTE wxT("absolute")
#define RAWBAKE_XML      wxT("xml")

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
        MakeVariable(t); \
        if (!t.IsEmpty()) \
            v = t.mb_str(wxConvLocal); \
    }

#define OPTION_PARSE_WXSTRING(v, node, a) \
    { \
        wxString t = node->GetPropVal(a, wxT("")); \
        MakeVariable(t); \
        if (!t.IsEmpty()) \
            v = t; \
    }

#define DO_CONDITION_COMMENT() \
        if (m_mode != MODE_BAKE) { \
            if (child->HasProp(wxT("if"))) { \
                bool haveit = true; \
                wxString t = child->GetPropVal(wxT("if"), wxT("")); \
                MakeVariable(t); \
                cRawOvlVars::iterator it = m_commandvariables.find(t); \
                if (it == m_commandvariables.end()) \
                    it = m_variables.find(t); \
                if (it == m_variables.end()) \
                    haveit = false; \
                if (haveit) { \
                    haveit = it->second != wxT("0"); \
                } \
                if (!haveit) { \
                    child = child->GetNext(); \
                    continue; \
                } \
            } \
            if (child->HasProp(wxT("ifnot"))) { \
                bool haveit = true; \
                wxString t = child->GetPropVal(wxT("ifnot"), wxT("")); \
                MakeVariable(t); \
                cRawOvlVars::iterator it = m_commandvariables.find(t); \
                if (it == m_commandvariables.end()) \
                    it = m_variables.find(t); \
                if (it == m_variables.end()) \
                    haveit = false; \
                if (haveit) { \
                    haveit = it->second != wxT("0"); \
                } \
                if (haveit) { \
                    child = child->GetNext(); \
                    continue; \
                } \
            } \
        } \
        if (child->HasProp(wxT("comment"))) { \
            wxString t = child->GetPropVal(wxT("comment"), wxT("")); \
            MakeVariable(t); \
            wxLogMessage(t); \
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
    Load(root);
}

void cRawOvl::AddConditions(const wxString& options) {
    wxStringTokenizer tok(options, wxT(" "));

    while (tok.HasMoreTokens()) {
        wxString t = tok.GetNextToken();
        if ((m_variables.find(t) == m_variables.end()) && (t != wxT("inherit")))
            m_variables[t] = wxT("1");
    }
}

void cRawOvl::AddConditions(const wxArrayString& options) {
    for (wxArrayString::const_iterator it = options.begin(); it != options.end(); ++it) {
        if ((m_variables.find(*it) == m_variables.end()) && (*it != wxT("inherit")))
            m_variables[*it] = wxT("1");
    }
}

void cRawOvl::AddConditions(const wxSortedArrayString& options) {
    for (wxSortedArrayString::const_iterator it = options.begin(); it != options.end(); ++it) {
        if ((m_variables.find(*it) == m_variables.end()) && (*it != wxT("inherit")))
            m_variables[*it] = wxT("1");
    }
}

void cRawOvl::RemoveConditions(const wxString& options) {
    wxStringTokenizer tok(options, wxT(" "));

    while (tok.HasMoreTokens()) {
        wxString t = tok.GetNextToken();
        if (m_variables.find(t) != m_variables.end())
            m_variables.erase(t);
    }
}

void cRawOvl::ParseConditions(const wxString& options) {
    wxStringTokenizer tok(options, wxT(" "));

    while (tok.HasMoreTokens()) {
        wxString t = tok.GetNextToken();
        if (t[0] == '-') {
            if (m_variables.find(t.Mid(1)) != m_variables.end())
                m_variables.erase(t.Mid(1));
        } else if (t[0] == '+') {
            if ((m_variables.find(t.Mid(1)) == m_variables.end()) && (t.Mid(1) != wxT("inherit")))
                m_variables[t.Mid(1)] = wxT("1");
        } else {
            if ((m_variables.find(t) == m_variables.end()) && (t != wxT("inherit")))
                m_variables[t] = wxT("1");
        }
    }
}

void cRawOvl::AddBakeStructures(const wxString& structs) {
    wxStringTokenizer tok(structs, wxT(" "));

    while (tok.HasMoreTokens()) {
        wxString t = tok.GetNextToken();
        if (m_bake.Index(t) == wxNOT_FOUND)
            m_bake.Add(t);
    }
}

bool cRawOvl::MakeVariable(wxString& var) {
    var.Trim();
    var.Trim(false);
    if (var.StartsWith(wxT("$")) && var.EndsWith(wxT("$"))) {
        wxString varname = var.AfterFirst('$').BeforeLast('$');
        cRawOvlVars::iterator it = m_commandvariables.find(varname);
        if (it == m_commandvariables.end())
            it = m_variables.find(varname);
        if (it == m_variables.end()) {
            wxLogWarning(wxString::Format(_("Varible '%s' undefined."), varname.c_str()));
            return false;
        }
        var = it->second;
    }
    return true;
}

cOvlType cRawOvl::ParseType(wxXmlNode* node, const wxString& nodes, const wxString& attribute) {
    wxString t = node->GetPropVal(attribute, wxT(""));
    MakeVariable(t);
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
    MakeVariable(t);
    if (t.IsEmpty())
        throw RCT3Exception(nodes+_(" tag misses ") + attribute + _(" attribute."));
    return t;
}

unsigned long cRawOvl::ParseUnsigned(wxXmlNode* node, const wxString& nodes, const wxString& attribute) {
    wxString t = node->GetPropVal(attribute, wxT(""));
    MakeVariable(t);
    unsigned long i;
    if (t.IsEmpty())
        throw RCT3Exception(nodes+_(" tag misses ") + attribute + _(" attribute."));
    if (!t.ToULong(&i))
        throw RCT3InvalidValueException(nodes+_(" tag, ") + attribute + _(" attribute: invalid value ")+t);
    return i;
}

long cRawOvl::ParseSigned(wxXmlNode* node, const wxString& nodes, const wxString& attribute) {
    wxString t = node->GetPropVal(attribute, wxT(""));
    MakeVariable(t);
    long i;
    if (t.IsEmpty())
        throw RCT3Exception(nodes+_(" tag misses ") + attribute + _(" attribute."));
    if (!t.ToLong(&i))
        throw RCT3InvalidValueException(nodes+_(" tag, ") + attribute + _(" attribute: invalid value ")+t);
    return i;
}

double cRawOvl::ParseFloat(wxXmlNode* node, const wxString& nodes, const wxString& attribute) {
    wxString t = node->GetPropVal(attribute, wxT(""));
    MakeVariable(t);
    double i;
    if (t.IsEmpty())
        throw RCT3Exception(nodes+_(" tag misses ") + attribute + _(" attribute."));
    if (!t.ToDouble(&i))
        throw RCT3InvalidValueException(nodes+_(" tag, ") + attribute + _(" attribute: invalid value ")+t);
    return i;
}

void cRawOvl::ParseSet(wxXmlNode* node, bool command) {
    wxString name = ParseString(node, RAWXML_SET, wxT("name"));
    if (node->HasProp(wxT("value"))) {
        wxString value = ParseString(node, RAWXML_SET, wxT("to"));
        if (command)
            m_commandvariables[name] = value;
        else
            m_variables[name] = value;
    } else {
        if (command)
            m_commandvariables[name] = wxT("1");
        else
            m_variables[name] = wxT("1");
    }
}

void cRawOvl::ParseUnset(wxXmlNode* node, bool command) {
    wxString name = ParseString(node, RAWXML_UNSET, wxT("name"));
    if (command)
        m_commandvariables.erase(name);
    else
        m_variables.erase(name);
}

void cRawOvl::ParseVariables(wxXmlNode* node, bool command, const wxString& path) {
    wxString inc = wxT("");
    OPTION_PARSE_WXSTRING(inc, node, wxT("include"));
    if (!inc.IsEmpty()) {
        wxFileName src;
        if (path.IsEmpty()) {
            src = wxFileName::DirName(m_input.GetPath());
        } else {
            src = wxFileName::DirName(path);
        }
        wxFileName filename = inc;
        if (!filename.IsAbsolute())
            filename.MakeAbsolute(src.GetPathWithSep());
        LoadVariables(filename, command, (m_bake.Index(RAWXML_VARIABLES)!=wxNOT_FOUND)?node:NULL );

        if (m_mode == MODE_BAKE) {
            if (m_bake.Index(RAWXML_VARIABLES) == wxNOT_FOUND) {
                wxString newfile;
                if (m_bake.Index(RAWBAKE_ABSOLUTE) == wxNOT_FOUND) {
                    filename.MakeRelativeTo(m_bakeroot.GetPathWithSep());
                }
                newfile = filename.GetFullPath();
                node->DeleteProperty(wxT("include"));
                node->AddProperty(wxT("include"), newfile);
            } else {
                node->DeleteProperty(wxT("include"));
            }
        }
    }

    wxXmlNode *child = node->GetChildren();
    while (child) {
        DO_CONDITION_COMMENT();

        if (child->GetName() == RAWXML_SET) {
            ParseSet(child, command);
        } else if (child->GetName() == RAWXML_UNSET) {
            ParseUnset(child, command);
        } else if (child->GetName() == RAWXML_VARIABLES) {
            ParseVariables(child, command, path);
        } else if COMPILER_WRONGTAG(child) {
            throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in variables tag."), child->GetName().c_str()));
        }
        child = child->GetNext();
    }
}

void cRawOvl::ParseCED(wxXmlNode* node) {
    cCarriedItemExtra ced;
    ced.name = ParseString(node, RAWXML_CED, wxT("name")).ToAscii();
    ced.nametxt = ParseString(node, RAWXML_CED, wxT("nametxt")).ToAscii();
    ced.icon = ParseString(node, RAWXML_CED, wxT("icon")).ToAscii();

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
    room.name = ParseString(node, RAWXML_CHG, wxT("name")).ToAscii();
    room.attraction.name = ParseString(node, RAWXML_CHG, wxT("nametxt")).ToAscii();
    room.attraction.description = ParseString(node, RAWXML_CHG, wxT("description")).ToAscii();
    room.sid = ParseString(node, RAWXML_CHG, wxT("sid")).ToAscii();
    room.spline = ParseString(node, RAWXML_CHG, wxT("roomspline")).ToAscii();

    wxXmlNode *child = node->GetChildren();
    while (child) {
        DO_CONDITION_COMMENT();

        if (child->GetName() == RAWXML_ATTRACTION_BASE) {
            OPTION_PARSE(unsigned long, room.attraction.type, ParseUnsigned(child, RAWXML_ATTRACTION_BASE, wxT("type")));
            OPTION_PARSE_STRING(room.attraction.icon, child, wxT("icon"));
            OPTION_PARSE_STRING(room.spline, child, wxT("attractionspline"));
        } else if (child->GetName() == RAWXML_ATTRACTION_UNKNOWNS) {
            OPTION_PARSE(unsigned long, room.attraction.unk2, ParseUnsigned(child, RAWXML_ATTRACTION_UNKNOWNS, wxT("u2")));
            OPTION_PARSE(long, room.attraction.unk3, ParseSigned(child, RAWXML_ATTRACTION_UNKNOWNS, wxT("u3")));
            OPTION_PARSE(unsigned long, room.attraction.unk4, ParseUnsigned(child, RAWXML_ATTRACTION_UNKNOWNS, wxT("u4")));
            OPTION_PARSE(unsigned long, room.attraction.unk5, ParseUnsigned(child, RAWXML_ATTRACTION_UNKNOWNS, wxT("u5")));
            OPTION_PARSE(unsigned long, room.attraction.unk6, ParseUnsigned(child, RAWXML_ATTRACTION_UNKNOWNS, wxT("u6")));
            OPTION_PARSE(unsigned long, room.attraction.unk9, ParseUnsigned(child, RAWXML_ATTRACTION_UNKNOWNS, wxT("u9")));
            OPTION_PARSE(long, room.attraction.unk10, ParseSigned(child, RAWXML_ATTRACTION_UNKNOWNS, wxT("u10")));
            OPTION_PARSE(unsigned long, room.attraction.unk11, ParseUnsigned(child, RAWXML_ATTRACTION_UNKNOWNS, wxT("u11")));
            OPTION_PARSE(unsigned long, room.attraction.unk12, ParseUnsigned(child, RAWXML_ATTRACTION_UNKNOWNS, wxT("u12")));
            OPTION_PARSE(unsigned long, room.attraction.unk13, ParseUnsigned(child, RAWXML_ATTRACTION_UNKNOWNS, wxT("u13")));
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
    cid.name = ParseString(node, RAWXML_CID, wxT("name")).ToAscii();
    cid.nametxt = ParseString(node, RAWXML_CID, wxT("nametxt")).ToAscii();
    cid.pluralnametxt = ParseString(node, RAWXML_CID, wxT("pluralnametxt")).ToAscii();

    wxXmlNode *child = node->GetChildren();
    while (child) {
        DO_CONDITION_COMMENT();

        if (child->GetName() == RAWXML_CID_SHAPE) {
            cid.shape.shaped = ParseUnsigned(child, RAWXML_CID_SHAPE, wxT("shaped"));
            if (cid.shape.shaped) {
                cid.shape.shape1 = ParseString(child, RAWXML_CID_SHAPE, wxT("shape1")).ToAscii();
                cid.shape.shape2 = ParseString(child, RAWXML_CID_SHAPE, wxT("shape1")).ToAscii();
            } else {
                cid.shape.MakeBillboard();
                cid.shape.fts = ParseString(child, RAWXML_CID_SHAPE, wxT("ftx")).ToAscii();
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
            cid.extras.push_back(std::string(extra.ToAscii()));
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
    sid.name = ParseString(node, RAWXML_SID, wxT("name")).ToAscii();
    sid.ovlpath = ParseString(node, RAWXML_SID, wxT("ovlpath")).ToAscii();
    sid.ui.name = ParseString(node, RAWXML_SID, wxT("nametxt")).ToAscii();
    sid.ui.icon = ParseString(node, RAWXML_SID, wxT("icon")).ToAscii();
    wxString svd = ParseString(node, RAWXML_SID, wxT("svd"));
    sid.svds.push_back(std::string(svd.ToAscii()));

    wxXmlNode *child = node->GetChildren();
    while (child) {
        DO_CONDITION_COMMENT();

        if (child->GetName() == RAWXML_SID_GROUP) {
            wxString name = ParseString(child, RAWXML_SID_GROUP, wxT("name"));
            wxString icon = ParseString(child, RAWXML_SID_GROUP, wxT("icon"));
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
            OPTION_PARSE_STRING(sid.position.supports, child, wxT("supports"));
        } else if (child->GetName() == RAWXML_SID_COLOURS) {
            OPTION_PARSE(unsigned long, sid.colours.defaultcol[0], ParseUnsigned(child, RAWXML_SID_COLOURS, wxT("choice1")));
            OPTION_PARSE(unsigned long, sid.colours.defaultcol[1], ParseUnsigned(child, RAWXML_SID_COLOURS, wxT("choice2")));
            OPTION_PARSE(unsigned long, sid.colours.defaultcol[2], ParseUnsigned(child, RAWXML_SID_COLOURS, wxT("choice3")));
        } else if (child->GetName() == RAWXML_SID_FLAGS) {
            wxString flags = ParseString(child, RAWXML_SID_FLAGS, wxT("set"));
            if (flags.Length() != 64)
                throw RCT3InvalidValueException(_("The set attribute of a sidflags tag is of invalid length"));
            const char* fl = flags.ToAscii();
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
            sid.svds.push_back(std::string(svd.ToAscii()));
        } else if (child->GetName() == RAWXML_SID_PARAMETER) {
            cSidParam param;
            param.name = ParseString(child, RAWXML_SID_GROUP, wxT("name")).ToAscii();
            OPTION_PARSE_STRING(param.param, child, wxT("param"));
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
    spl.name = ParseString(node, RAWXML_SPL, wxT("name")).ToAscii();
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
            const char* d = datastr.ToAscii();
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
    stall.name = ParseString(node, RAWXML_SID, wxT("name")).ToAscii();
    stall.attraction.name = ParseString(node, RAWXML_SID, wxT("nametxt")).ToAscii();
    stall.attraction.description = ParseString(node, RAWXML_SID, wxT("description")).ToAscii();
    stall.sid = ParseString(node, RAWXML_SID, wxT("sid")).ToAscii();

    wxXmlNode *child = node->GetChildren();
    while (child) {
        DO_CONDITION_COMMENT();

        if (child->GetName() == RAWXML_ATTRACTION_BASE) {
            OPTION_PARSE(unsigned long, stall.attraction.type, ParseUnsigned(child, RAWXML_ATTRACTION_BASE, wxT("type")));
            OPTION_PARSE_STRING(stall.attraction.icon, child, wxT("icon"));
            OPTION_PARSE_STRING(stall.attraction.spline, child, wxT("spline"));
        } else if (child->GetName() == RAWXML_STA_ITEM) {
            cStallItem item;
            item.item = ParseString(child, RAWXML_STA_ITEM, wxT("cid")).ToAscii();
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

void cRawOvl::ParseTEX(wxXmlNode* node) {
    wxString name = ParseString(node, RAWXML_TEX, wxT("name"));
//    wxFileName texture = ParseString(node, RAWXML_TEX, wxT("texture"));
//    if (!texture.IsAbsolute())
//        texture.MakeAbsolute(m_input.GetPathWithSep());
    counted_array_ptr<unsigned char> data;
    unsigned long dimension = 0;
    unsigned long datasize = 0;

    wxXmlNode* child = node->GetChildren();
    while (child) {
        DO_CONDITION_COMMENT();

        if (child->GetName() == RAWXML_TEX_FILE) {
            if (data.get() && (m_mode != MODE_BAKE))
                throw RCT3Exception(wxString(wxT("tex tag '"))+name+wxT("': multiple datasources."));

            wxString t = child->GetNodeContent();
            MakeVariable(t);
            wxFileName texture = t;
            if (!texture.IsAbsolute())
                texture.MakeAbsolute(m_input.GetPathWithSep());

            if (m_mode == MODE_BAKE) {
                if (m_bake.Index(RAWXML_TEX) == wxNOT_FOUND) {
                    wxString newfile;
                    if (m_bake.Index(RAWBAKE_ABSOLUTE) == wxNOT_FOUND) {
                        texture.MakeRelativeTo(m_bakeroot.GetPathWithSep());
                    }
                    newfile = texture.GetFullPath();
                    delete child->GetChildren();
                    child->SetChildren(new wxXmlNode(NULL, wxXML_TEXT_NODE, wxT(""), newfile));
                    child = child->GetNext();
                    continue;
                }
            }

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
                datasize = img.GetDxtBufferSize(wxDXT3);
                dimension = width;
                data = counted_array_ptr<unsigned char>(new unsigned char[datasize]);
                img.DxtCompress(data.get(), wxDXT3);

                if (m_mode == MODE_BAKE) {
                    wxString temp;
                    unsigned char* buf = reinterpret_cast<unsigned char*>(temp.GetWriteBuf(datasize * 4));
                    unsigned long outlen = datasize * 4;
                    int bret = base64_encode(data.get(), datasize, buf, &outlen);
                    switch (bret) {
                        case CRYPT_OK:
                            break;
                        case CRYPT_BUFFER_OVERFLOW:
                            throw RCT3Exception(wxString(_("Buffer overflow baking texture '")) + texture.GetFullPath() + _("' in tex tag ") + name);
                        default:
                            throw RCT3Exception(wxString(_("Unknown base64 error baking texture '")) + texture.GetFullPath() + _("' in tex tag ") + name);
                    }
                    temp.UngetWriteBuf(outlen);
                    child->SetName(RAWXML_TEX_DATA);
                    child->AddProperty(wxT("dimension"), wxString::Format(wxT("%u"), img.GetWidth()));
                    delete child->GetChildren();
                    child->SetChildren(new wxXmlNode(NULL, wxXML_TEXT_NODE, wxT(""), temp));
                }

            } catch (Magick::Exception& e) {
                throw RCT3Exception(wxString::Format(_("tex tag: GarphicsMagik exception: %s"), e.what()));
            }
        } else if (child->GetName() == RAWXML_TEX_DATA) {
            if (m_mode == MODE_BAKE) {
                child = child->GetNext();
                continue;
            }
            if (data.get())
                throw RCT3Exception(wxString(wxT("tex tag '"))+name+wxT("': multiple datasources."));
            dimension = ParseUnsigned(child, RAWXML_TEX_DATA, wxT("dimension"));
            datasize = squish::GetStorageRequirements(dimension, dimension, wxDXT3);
            unsigned long outlen = datasize;

            data = counted_array_ptr<unsigned char>(new unsigned char[datasize]);
            wxString tex = child->GetNodeContent();
            MakeVariable(tex);
            int bret = base64_decode(reinterpret_cast<const unsigned char*>(static_cast<const char*>(tex.ToAscii())), tex.Length(), data.get(), &outlen);
            switch (bret) {
                case CRYPT_OK:
                    break;
                case CRYPT_INVALID_PACKET:
                    throw RCT3Exception(wxString(_("Decoding error in tex tag ")) + name);
                case CRYPT_BUFFER_OVERFLOW:
                    throw RCT3Exception(wxString(_("Buffer overflow decoding tex tag ")) + name);
                default:
                    throw RCT3Exception(wxString(_("Unknown base64 error decoding tex tag ")) + name);
            }
            if (outlen != datasize)
                throw RCT3Exception(wxString(_("Datasize mismatch error in tex tag ")) + name);

        } else if COMPILER_WRONGTAG(child) {
            throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in tex tag."), child->GetName().c_str()));
        }
        child = child->GetNext();
    }

    if (m_mode != MODE_BAKE) {
        if (!data.get())
            throw RCT3Exception(wxString(wxT("tex tag '"))+name+wxT("': no datasource."));
        ovlTEXManager* c_tex = m_ovl.GetManager<ovlTEXManager>();
        c_tex->AddTexture(std::string(name.ToAscii()), dimension, datasize, reinterpret_cast<unsigned long*>(data.get()));
    }
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
            c_raw.SetOptions(m_mode, m_dryrun);
            c_raw.PassBakeStructures(m_bake);
            if (m_mode == MODE_BAKE)
                c_raw.PassBakeRoot(m_bakeroot);
            c_raw.PassVariables(m_commandvariables, m_variables);
            if (incfile.IsEmpty()) {
                c_raw.Process(child, m_input, m_outputbasedir);
            } else {
                if (child->GetChildren())
                    throw RCT3Exception(wxT("A subovl tag with include attribute may not have content"));

                wxFileName filename = incfile;
                if (!filename.IsAbsolute())
                    filename.MakeAbsolute(m_input.GetPathWithSep());

                if (m_mode == MODE_BAKE) {
                    if (m_bake.Index(RAWBAKE_XML) == wxNOT_FOUND) {
                        wxString newfile;
                        if (m_bake.Index(RAWBAKE_ABSOLUTE) == wxNOT_FOUND) {
                            filename.MakeRelativeTo(m_bakeroot.GetPathWithSep());
                        }
                        newfile = filename.GetFullPath();
                        child->DeleteProperty(wxT("include"));
                        child->AddProperty(wxT("include"), newfile);
                        child = child->GetNext();
                        continue;
                    }
                }

                wxXmlDocument doc;
                doc.Load(filename.GetFullPath());
                wxXmlNode* root = doc.GetRoot();
                c_raw.Process(root, filename, m_outputbasedir);
                if (m_mode == MODE_BAKE) {
                    doc.DetachRoot();
                    root->SetName(RAWXML_SUBROOT);
                    child->SetChildren(root);
                    root->SetParent(child);
                    child->DeleteProperty(wxT("include"));
                }
            }
            for (std::vector<wxFileName>::const_iterator it = c_raw.GetModifiedFiles().begin(); it != c_raw.GetModifiedFiles().end(); ++it) {
                m_modifiedfiles.push_back(*it);
            }
            for (std::vector<wxFileName>::const_iterator it = c_raw.GetNewFiles().begin(); it != c_raw.GetNewFiles().end(); ++it) {
                m_newfiles.push_back(*it);
            }
        } else if (child->GetName() == RAWXML_SET) {
            ParseSet(child);
        } else if (child->GetName() == RAWXML_UNSET) {
            ParseUnset(child);
        } else if (child->GetName() == RAWXML_VARIABLES) {
            ParseVariables(child);
        } else if (m_dryrun) {
            // The rest creates the file
        } else if (child->GetName() == RAWXML_IMPORT) {
            // <import file="scenery file" (name="internal svd name") />
            if (m_mode == MODE_BAKE) {
                child = child->GetNext();
                continue;
            }
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
            if (m_mode == MODE_BAKE) {
                child = child->GetNext();
                continue;
            }
            ParseCED(child);
        } else if (child->GetName() == RAWXML_CHG) {
            if (m_mode == MODE_BAKE) {
                child = child->GetNext();
                continue;
            }
            ParseCHG(child);
        } else if (child->GetName() == RAWXML_CID) {
            if (m_mode == MODE_BAKE) {
                child = child->GetNext();
                continue;
            }
            ParseCID(child);
        } else if (child->GetName() == RAWXML_GSI) {
            // <gsi name="string" tex="string" top="long int" left="long int" bottom="long int" right="long int" />
            if (m_mode == MODE_BAKE) {
                child = child->GetNext();
                continue;
            }
            wxString name = ParseString(child, RAWXML_GSI, wxT("name"));
            wxString tex = ParseString(child, RAWXML_GSI, wxT("tex"));
            unsigned long top = ParseUnsigned(child, RAWXML_GSI, wxT("top"));
            unsigned long left = ParseUnsigned(child, RAWXML_GSI, wxT("left"));
            unsigned long bottom = ParseUnsigned(child, RAWXML_GSI, wxT("bottom"));
            unsigned long right = ParseUnsigned(child, RAWXML_GSI, wxT("right"));

            ovlGSIManager* c_gsi = m_ovl.GetManager<ovlGSIManager>();
            c_gsi->AddItem(name.ToAscii(), tex.ToAscii(), left, top, right, bottom);
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
            if (m_mode == MODE_BAKE) {
                child = child->GetNext();
                continue;
            }
            ParseSID(child);
        } else if (child->GetName() == RAWXML_SPL) {
            if (m_mode == MODE_BAKE) {
                child = child->GetNext();
                continue;
            }
            ParseSPL(child);
        } else if (child->GetName() == RAWXML_STA) {
            // <sta name="string" nametxt="txt" description="txt" sid="sid">
            //     <stais type="long int" icon="gsi" spline="spl" />
            //     <staitem cid="cid" cost="long int" />
            //     <stastallunknowns u1="long int" u2="long int" u3="long int" u4="long int" u5="long int" u6="long int" />
            // </sta>
            if (m_mode == MODE_BAKE) {
                child = child->GetNext();
                continue;
            }
            ParseSTA(child);
        } else if (child->GetName() == RAWXML_TEX) {
            // <tex name="string" texture="texture file" />
//            if ((m_mode == MODE_BAKE) && (m_bake.Index(RAWXML_TEX) == wxNOT_FOUND)) {
//                child = child->GetNext();
//                continue;
//            }
            ParseTEX(child);
        } else if (child->GetName() == RAWXML_TXT) {
            // <txt name="string" text="string" />
            wxString name = ParseString(child, RAWXML_TXT, wxT("name"));
            wxString type = ParseString(child, RAWXML_TXT, wxT("type"));
            if (type == wxT("text")) {
                if ((m_mode == MODE_BAKE) && (m_bake.Index(RAWXML_TXT) == wxNOT_FOUND)) {
                    child = child->GetNext();
                    continue;
                }
                wxString text = child->GetNodeContent();
                MakeVariable(text);

                if (m_mode == MODE_BAKE) {
                    int wlength = text.Length()+1;
                    counted_array_ptr<wchar_t> tempch(new wchar_t[wlength]);
#ifdef wxUSE_UNICODE
                    wcscpy(tempch.get(), text.wc_str(wxCSConv(wxFONTENCODING_UTF16LE)));
#else
                    mbstowcs(tempch.get(), text.c_str(), wlength);
#endif
                    counted_array_ptr<unsigned char> buf = counted_array_ptr<unsigned char>(new unsigned char[wlength * 2 * 4]);
                    unsigned long outlen = wlength * 2 * 4;
                    int bret = base64_encode(reinterpret_cast<unsigned char*>(tempch.get()), wlength * 2, buf.get(), &outlen);
                    switch (bret) {
                        case CRYPT_OK:
                            break;
                        case CRYPT_BUFFER_OVERFLOW:
                            throw RCT3Exception(wxString(_("Buffer overflow baking text '")) + text + _("' in txt tag ") + name);
                        default:
                            throw RCT3Exception(wxString(_("Unknown base64 error baking text '")) + text + _("' in txt tag ") + name);
                    }
                    child->DeleteProperty(wxT("type"));
                    child->AddProperty(wxT("type"), wxT("raw"));
                    delete child->GetChildren();
                    child->SetChildren(new wxXmlNode(NULL, wxXML_TEXT_NODE, wxT(""), wxString(reinterpret_cast<char*>(buf.get()), wxConvLocal)));
                } else {
                    ovlTXTManager* c_txt = m_ovl.GetManager<ovlTXTManager>();
#ifdef wxUSE_UNICODE
                    c_txt->AddText(name.ToAscii(), text.wc_str(wxCSConv(wxFONTENCODING_UTF16LE)));
#else
                    c_txt->AddText(name.ToAscii(), text.c_str());
#endif
                }
            } else if (type == wxT("raw")) {
                if (m_mode == MODE_BAKE) {
                    child = child->GetNext();
                    continue;
                }
                wxString text = child->GetNodeContent();
                MakeVariable(text);

                unsigned long datasize = text.Length();
                unsigned long outlen = datasize;
                counted_array_ptr<wchar_t> data = counted_array_ptr<wchar_t>(new wchar_t[datasize/2]);

                int bret = base64_decode(reinterpret_cast<const unsigned char*>(text.ToAscii().data()), text.Length(), reinterpret_cast<unsigned char*>(data.get()), &outlen);
                switch (bret) {
                    case CRYPT_OK:
                        break;
                    case CRYPT_INVALID_PACKET:
                        throw RCT3Exception(wxString(_("Decoding error in txt tag ")) + name);
                    case CRYPT_BUFFER_OVERFLOW:
                        throw RCT3Exception(wxString(_("Buffer overflow decoding txt tag ")) + name);
                    default:
                        throw RCT3Exception(wxString(_("Unknown base64 error decoding txt tag ")) + name);
                }
                ovlTXTManager* c_txt = m_ovl.GetManager<ovlTXTManager>();
                c_txt->AddText(name.ToAscii(), data.get());

            } else {
                throw RCT3Exception(wxString::Format(_("Unknown type '%s' in txt tag '%s'."), type.c_str(), name.c_str()));
            }

        } else if (child->GetName() == RAWXML_REFERENCE) {
            // <reference path="relative path" />
            if (m_mode == MODE_BAKE) {
                child = child->GetNext();
                continue;
            }
            wxString ref = child->GetPropVal(wxT("path"), wxT(""));
            if (ref.IsEmpty())
                throw RCT3Exception(_("REFERENCE tag misses path attribute."));
            m_ovl.AddReference(ref.mb_str(wxConvFile));
        } else if (child->GetName() == RAWXML_SYMBOL) {
            // <symbol target="common|unique" name="string" type="int|float" data="data" />
            if (m_mode == MODE_BAKE) {
                child = child->GetNext();
                continue;
            }
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
            m_ovl.AddDataSymbol(target, name.ToAscii(), data);
        } else if COMPILER_WRONGTAG(child) {
            throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in rawovl or subovl tag."), child->GetName().c_str()));
        }

        child = child->GetNext();
    }

}

void cRawOvl::Load(wxXmlNode* root) {
    if (!root)
        throw RCT3Exception(wxT("cRawOvl::Load, root is null"));
    bool subonly = false;

    if (m_bakeroot == wxT(""))
        m_bakeroot = m_input;

    if (m_mode != MODE_BAKE) {
        if (root->HasProp(wxT("conditions"))) {
            wxString cond = root->GetPropVal(wxT("conditions"), wxT(""));
            ParseConditions(cond);
        }
    }

    if ((root->GetName() == RAWXML_ROOT) || (root->GetName() == RAWXML_SUBROOT)) {
        // <rawovl|subovl file="outputfile" basedir="dir outputfile is relative to">
        {
            wxString basedir = root->GetPropVal(wxT("basedir"), wxT(""));
            if ((m_mode == MODE_INSTALL) && root->HasProp(wxT("installdir"))) {
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
            if ((!m_dryrun) && (m_mode != MODE_BAKE)) {
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

            {
                wxFileName temp = m_output;
                temp.SetExt(wxT("common.ovl"));
                if (temp.FileExists()) {
                    m_modifiedfiles.push_back(m_output);
                } else {
                    m_newfiles.push_back(m_output);
                }
            }
            if ((!m_dryrun) && (m_mode != MODE_BAKE)) {
                m_ovl.Init(m_output.GetFullPath().mb_str(wxConvFile).data());
            }
            wxLogMessage(_("Writing ovl from raw: ")+m_output.GetFullPath()+wxT(".common.ovl"));
        } else {
            subonly = true;
            wxLogMessage(_("Parsing subovl-only raw. Base output dir: ")+m_outputbasedir.GetPathWithSep());
        }

        // Start parsing
        Parse(root);

        if ((!subonly) && (!m_dryrun) && (m_mode != MODE_BAKE)) {
            m_ovl.Save();
            wxLogMessage(_("OVL written successfully"));
        }
    } else {
        throw RCT3Exception(wxT("cRawOvl::Load, wrong root"));
    }

}

void cRawOvl::LoadVariables(const wxFileName& fn, bool command, wxXmlNode* target) {
    wxXmlDocument doc;
    doc.Load(fn.GetFullPath());

    wxXmlNode* root = doc.GetRoot();

    if (root->GetName() == RAWXML_VARIABLES) {
        ParseVariables(root, command, fn.GetPathWithSep());
        if (target && (m_mode == MODE_BAKE) && (m_bake.Index(RAWXML_VARIABLES) != wxNOT_FOUND)) {
            wxXmlNode* newvars = root->GetChildren();
            if (newvars) {
                root->SetChildren(NULL);
                wxXmlNode* oldvars = target->GetChildren();
                if (oldvars)
                    oldvars->SetParent(NULL);
                target->SetChildren(newvars);
                if (oldvars) {
                    wxXmlNode* child = newvars;
                    wxXmlNode* lastchild = newvars;
                    while (child) {
                        child = child->GetNext();
                        if (child)
                            lastchild = child;
                    }
                    lastchild->SetNext(oldvars);
                }
            }
        }
    } else {
        throw RCT3Exception(wxT("cRawOvl::LoadVariables, wrong root"));
    }
}
