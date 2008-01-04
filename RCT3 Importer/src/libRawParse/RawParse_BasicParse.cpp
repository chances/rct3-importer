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

cOvlType cRawParser::ParseType(wxXmlNode* node, const wxString& nodes, const wxString& attribute) {
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

wxString cRawParser::ParseString(wxXmlNode* node, const wxString& nodes, const wxString& attribute, bool* variable, bool addprefix) {
    wxString t = node->GetPropVal(attribute, wxT(""));
    if (variable)
        *variable = MakeVariable(t);
    else
        MakeVariable(t);
    if (t.IsEmpty())
        throw RCT3Exception(nodes+_(" tag misses ") + attribute + _(" attribute."));
    if (addprefix)
        t = wxString(m_prefix.c_str(), wxConvLocal) + t;
    return t;
}

void cRawParser::ParseStringOption(std::string& str, wxXmlNode* node, const wxString& attribute, bool* variable, bool addprefix) {
    wxString t = node->GetPropVal(attribute, wxT(""));
    if (variable)
        *variable = MakeVariable(t);
    else
        MakeVariable(t);
    if (!t.IsEmpty()) {
        if (addprefix) {
            str = m_prefix + std::string(t.mb_str(wxConvLocal));
        } else {
            str = t.mb_str(wxConvLocal);
        }
    }
}

void cRawParser::ParseStringOption(wxString& str, wxXmlNode* node, const wxString& attribute, bool* variable, bool addprefix) {
    wxString t = node->GetPropVal(attribute, wxT(""));
    if (variable)
        *variable = MakeVariable(t);
    else
        MakeVariable(t);
    if (!t.IsEmpty()) {
        if (addprefix) {
            str = wxString(m_prefix.c_str(), wxConvLocal) + t;
        } else {
            str = t;
        }
    }
}

unsigned long cRawParser::ParseUnsigned(wxXmlNode* node, const wxString& nodes, const wxString& attribute) {
    wxString t = node->GetPropVal(attribute, wxT(""));
    MakeVariable(t);
    unsigned long i;
    if (t.IsEmpty())
        throw RCT3Exception(nodes+_(" tag misses ") + attribute + _(" attribute."));
    if (!t.ToULong(&i))
        throw RCT3InvalidValueException(nodes+_(" tag, ") + attribute + _(" attribute: invalid value ")+t);
    return i;
}

long cRawParser::ParseSigned(wxXmlNode* node, const wxString& nodes, const wxString& attribute) {
    wxString t = node->GetPropVal(attribute, wxT(""));
    MakeVariable(t);
    long i;
    if (t.IsEmpty())
        throw RCT3Exception(nodes+_(" tag misses ") + attribute + _(" attribute."));
    if (!t.ToLong(&i))
        throw RCT3InvalidValueException(nodes+_(" tag, ") + attribute + _(" attribute: invalid value ")+t);
    return i;
}

double cRawParser::ParseFloat(wxXmlNode* node, const wxString& nodes, const wxString& attribute) {
    wxString t = node->GetPropVal(attribute, wxT(""));
    MakeVariable(t);
    double i;
    if (t.IsEmpty())
        throw RCT3Exception(nodes+_(" tag misses ") + attribute + _(" attribute."));
    if (!t.ToDouble(&i))
        throw RCT3InvalidValueException(nodes+_(" tag, ") + attribute + _(" attribute: invalid value ")+t);
    return i;
}

void cRawParser::ParseVector(wxXmlNode* node, D3DVECTOR& v, const wxString& nodes) {
    v.x = ParseFloat(node, nodes, wxT("x"));
    v.y = ParseFloat(node, nodes, wxT("y"));
    v.z = ParseFloat(node, nodes, wxT("z"));
}

void cRawParser::ParseSet(wxXmlNode* node, bool command, const wxString& tag) {
    wxString name = ParseString(node, tag, wxT("set"), NULL);
    if (node->HasProp(wxT("to"))) {
        wxString value = ParseString(node, tag, wxT("to"), NULL);
        wxLogVerbose(wxString::Format(_("Setting variable %s to %s."), name.c_str(), value.c_str()));
        if (command)
            m_commandvariables[name] = value;
        else
            m_variables[name] = value;
    } else {
        wxLogVerbose(wxString::Format(_("Setting variable %s."), name.c_str()));
        if (command)
            m_commandvariables[name] = wxT("1");
        else
            m_variables[name] = wxT("1");
    }
}

void cRawParser::ParseUnset(wxXmlNode* node, bool command) {
    wxString name = ParseString(node, RAWXML_UNSET, wxT("unset"), NULL);
    wxLogVerbose(wxString::Format(_("Unsetting variable %s."), name.c_str()));
    if (command)
        m_commandvariables.erase(name);
    else
        m_variables.erase(name);
}

void cRawParser::ParseVariables(wxXmlNode* node, bool command, const wxString& path) {
    wxString inc = wxT("");
    ParseStringOption(inc, node, wxT("include"), NULL);
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
        DO_CONDITION_COMMENT(child);

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

