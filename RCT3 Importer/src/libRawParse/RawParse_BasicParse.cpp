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

cOvlType cRawParser::ParseType(const cXmlNode& node, const wxString& nodes, const wxString& attribute) {
    wxString t(node.getPropVal(attribute.mb_str(wxConvUTF8)).c_str(), wxConvUTF8);
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

wxString cRawParser::ParseString(const cXmlNode& node, const wxString& nodes, const wxString& attribute, bool* variable, bool addprefix) {
    wxString t(node.getPropVal(attribute.mb_str(wxConvUTF8)).c_str(), wxConvUTF8);
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

void cRawParser::ParseStringOption(std::string& str, const cXmlNode& node, const wxString& attribute, bool* variable, bool addprefix) {
    wxString t(node.getPropVal(attribute.mb_str(wxConvUTF8)).c_str(), wxConvUTF8);
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

void cRawParser::ParseStringOption(wxString& str, const cXmlNode& node, const wxString& attribute, bool* variable, bool addprefix) {
    wxString t(node.getPropVal(attribute.mb_str(wxConvUTF8)).c_str(), wxConvUTF8);
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

unsigned long cRawParser::ParseUnsigned(const cXmlNode& node, const wxString& nodes, const wxString& attribute) {
    wxString t(node.getPropVal(attribute.mb_str(wxConvUTF8)).c_str(), wxConvUTF8);
    MakeVariable(t);
    unsigned long i;
    if (t.IsEmpty())
        throw RCT3Exception(nodes+_(" tag misses ") + attribute + _(" attribute."));
    if (!t.ToULong(&i))
        throw RCT3InvalidValueException(nodes+_(" tag, ") + attribute + _(" attribute: invalid value ")+t);
    return i;
}

long cRawParser::ParseSigned(const cXmlNode& node, const wxString& nodes, const wxString& attribute) {
    wxString t(node.getPropVal(attribute.mb_str(wxConvUTF8)).c_str(), wxConvUTF8);
    MakeVariable(t);
    long i;
    if (t.IsEmpty())
        throw RCT3Exception(nodes+_(" tag misses ") + attribute + _(" attribute."));
    if (!t.ToLong(&i))
        throw RCT3InvalidValueException(nodes+_(" tag, ") + attribute + _(" attribute: invalid value ")+t);
    return i;
}

double cRawParser::ParseFloat(const cXmlNode& node, const wxString& nodes, const wxString& attribute) {
    wxString t(node.getPropVal(attribute.mb_str(wxConvUTF8)).c_str(), wxConvUTF8);
    MakeVariable(t);
    double i;
    if (t.IsEmpty())
        throw RCT3Exception(nodes+_(" tag misses ") + attribute + _(" attribute."));
    if (!t.ToDouble(&i))
        throw RCT3InvalidValueException(nodes+_(" tag, ") + attribute + _(" attribute: invalid value ")+t);
    return i;
}

void cRawParser::ParseVector(const cXmlNode& node, VECTOR& v, const wxString& nodes) {
    v.x = ParseFloat(node, nodes, wxT("x"));
    v.y = ParseFloat(node, nodes, wxT("y"));
    v.z = ParseFloat(node, nodes, wxT("z"));
}

void cRawParser::ParseMatrix(const cXmlNode& node, MATRIX& m, const wxString& nodes) {
    m = matrixGetUnity();

    bool missrow[4] = {true, true, true, true};
    wxString temp;
    cXmlNode child(node.children());
    while(child) {
        if (child("row1")) {
            missrow[0] = false;
            temp = wxString(child.content().c_str(), wxConvUTF8);
            if (sscanf(temp.mb_str(wxConvLocal), "%f %f %f %f", &m._11, &m._12, &m._13, &m._14) != 4)
                throw RCT3InvalidValueException(nodes+_(" tag, row1: invalid content: ")+temp);
        } else if (child("row2")) {
            missrow[1] = false;
            temp = wxString(child.content().c_str(), wxConvUTF8);
            if (sscanf(temp.mb_str(wxConvLocal), "%f %f %f %f", &m._21, &m._22, &m._23, &m._24) != 4)
                throw RCT3InvalidValueException(nodes+_(" tag, row2: invalid content: ")+temp);
        } else if (child("row3")) {
            missrow[2] = false;
            temp = wxString(child.content().c_str(), wxConvUTF8);
            if (sscanf(temp.mb_str(wxConvLocal), "%f %f %f %f", &m._31, &m._32, &m._33, &m._34) != 4)
                throw RCT3InvalidValueException(nodes+_(" tag, row3: invalid content: ")+temp);
        } else if (child("row4")) {
            missrow[3] = false;
            temp = wxString(child.content().c_str(), wxConvUTF8);
            if (sscanf(temp.mb_str(wxConvLocal), "%f %f %f %f", &m._41, &m._42, &m._43, &m._44) != 4)
                throw RCT3InvalidValueException(nodes+_(" tag, row4: invalid content: ")+temp);
        } else if (child.is(XML_ELEMENT_NODE)) {
            throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in %s."), STRING_FOR_FORMAT(child.name()), nodes.c_str()));
        }
        child.go_next();
    }
    if (missrow[0] || missrow[1] || missrow[2] || missrow[3])
        throw RCT3Exception(wxString::Format(_("Missing matrix row in %s."), nodes.c_str()));
}

c3DLoaderOrientation cRawParser::ParseOrientation(const cXmlNode& node, const wxString& nodes) {
    c3DLoaderOrientation ori = ORIENTATION_LEFT_YUP;
    wxString hand = UTF8STRINGWRAP(node.getPropVal("handedness", "left"));
    wxString up = UTF8STRINGWRAP(node.getPropVal("up", "y"));
    if (hand == wxT("left")) {
        if (up == wxT("x")) {
            ori = ORIENTATION_LEFT_XUP;
        } else if (up == wxT("y")) {
            ori = ORIENTATION_LEFT_YUP;
        } else if (up == wxT("z")) {
            ori = ORIENTATION_LEFT_ZUP;
        } else {
            throw RCT3Exception(wxString::Format(_("Unknown value '%s' for up attribute in %s."), up.c_str(), nodes.c_str()));
        }
    } else if (hand == wxT("right")) {
        if (up == wxT("x")) {
            ori = ORIENTATION_RIGHT_XUP;
        } else if (up == wxT("y")) {
            ori = ORIENTATION_RIGHT_YUP;
        } else if (up == wxT("z")) {
            ori = ORIENTATION_RIGHT_ZUP;
        } else {
            throw RCT3Exception(wxString::Format(_("Unknown value '%s' for up attribute in %s."), up.c_str(), nodes.c_str()));
        }
    } else {
        throw RCT3Exception(wxString::Format(_("Unknown value '%s' for handedness attribute in %s."), up.c_str(), nodes.c_str()));
    }
    return ori;
}

void cRawParser::ParseSet(const cXmlNode& node, bool command, const wxString& tag) {
    wxString name = ParseString(node, tag, wxT("set"), NULL);
    if (node.hasProp("to")) {
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

void cRawParser::ParseUnset(const cXmlNode& node, bool command) {
    wxString name = ParseString(node, wxT(RAWXML_UNSET), wxT("unset"), NULL);
    wxLogVerbose(wxString::Format(_("Unsetting variable %s."), name.c_str()));
    if (command)
        m_commandvariables.erase(name);
    else
        m_variables.erase(name);
}

void cRawParser::ParseVariables(cXmlNode& node, bool command, const wxString& path) {
    wxString inc = wxT("");
    ParseStringOption(inc, node, wxT("include"), NULL);
    if (!inc.IsEmpty()) {
        wxFSFileName src;
        if (path.IsEmpty()) {
            src = m_input.GetPath();
        } else {
            src = wxFSFileName(path, true);
        }
        wxFSFileName filename = inc;
        if (!filename.IsAbsolute())
            filename.MakeAbsolute(src.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME));
        LoadVariables(filename, command, (m_bake.Index(wxT(RAWXML_VARIABLES))!=wxNOT_FOUND)?&node:NULL );

        if (m_mode == MODE_BAKE) {
            if (m_bake.Index(wxT(RAWXML_VARIABLES)) == wxNOT_FOUND) {
                wxString newfile;
                if (m_bake.Index(wxT(RAWBAKE_ABSOLUTE)) == wxNOT_FOUND) {
                    filename.MakeRelativeTo(m_bakeroot.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME));
                }
                newfile = filename.GetFullPath();
                node.delProp("include");
                node.addProp("include", newfile.mb_str(wxConvUTF8));
            } else {
                node.delProp("include");
            }
        }
    }

    cXmlNode child(node.children());
    while (child) {
        DO_CONDITION_COMMENT(child);

        if (child(RAWXML_SET)) {
            ParseSet(child, command);
        } else if (child(RAWXML_UNSET)) {
            ParseUnset(child, command);
        } else if (child(RAWXML_VARIABLES)) {
            ParseVariables(child, command, path);
        } else if (child.is(XML_ELEMENT_NODE)) {
            throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in variables tag."), STRING_FOR_FORMAT(child.name())));
        }
        child.go_next();
    }
}

