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

#include <bitset>

#include "ManagerCommon.h"

cOvlType cRawParser::ParseType(const cXmlNode& node, const wxString& nodes, const wxString& attribute) {
    wxString t(node.getPropVal(attribute.mb_str(wxConvUTF8)).c_str(), wxConvUTF8);
    MakeVariable(t);
    if (t.IsEmpty())
        throw RCT3Exception(FinishNodeError(nodes+_(" tag misses ") + attribute + _(" attribute."), node));
    if (t == wxT("common")) {
        return OVLT_COMMON;
    } else if (t == wxT("unique")) {
        return OVLT_UNIQUE;
    } else {
        throw RCT3InvalidValueException(FinishNodeError(nodes+_(" tag, ") + attribute + _(" attribute: invalid value ")+t, node));
    }
}

wxString cRawParser::ParseString(const cXmlNode& node, const wxString& nodes, const wxString& attribute, bool* variable, bool addprefix) {
    wxString t(HandleStringContent(node.wxgetPropVal(attribute), variable, addprefix));
    if (t.IsEmpty())
        throw RCT3Exception(FinishNodeError(nodes+_(" tag misses ") + attribute + _(" attribute."), node));
    return t;
}

wxString cRawParser::HandleStringContent(const wxString& content, bool* variable, bool addprefix) {
    wxString t(content);
    if (variable)
        *variable = MakeVariable(t);
    else
        MakeVariable(t);
    if (t.IsEmpty())
        return t;
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
    wxString t(node.wxgetPropVal(attribute));
    MakeVariable(t);
    unsigned long i;
    if (t.IsEmpty())
        throw RCT3Exception(FinishNodeError(nodes+_(" tag misses ") + attribute + _(" attribute."), node));

    if (t.StartsWith('b')) {
        try {
            bitset<32> bits(string(t.utf8_str()), 1);
            i = bits.to_ulong();
        } catch (exception& e) {
            throw RCT3InvalidValueException(FinishNodeError(nodes+_(" tag, ") + attribute + _(" attribute: invalid binary value ")+t, node));
        }
    } else if (t.StartsWith('h') || t.StartsWith("0x")) {
        wxString hex;
        if (!t.StartsWith('h', &hex))
            t.StartsWith("0x", &hex);
        if (!parseHexULong(hex, i))
            throw RCT3InvalidValueException(FinishNodeError(nodes+_(" tag, ") + attribute + _(" attribute: invalid hexadecimal value ")+t, node));
    } else if (!t.ToULong(&i))
        throw RCT3InvalidValueException(FinishNodeError(nodes+_(" tag, ") + attribute + _(" attribute: invalid value ")+t, node));
    return i;
}

long cRawParser::ParseSigned(const cXmlNode& node, const wxString& nodes, const wxString& attribute) {
    wxString t(node.getPropVal(attribute.mb_str(wxConvUTF8)).c_str(), wxConvUTF8);
    MakeVariable(t);
    long i;
    if (t.IsEmpty())
        throw RCT3Exception(FinishNodeError(nodes+_(" tag misses ") + attribute + _(" attribute."), node));
    if (!t.ToLong(&i))
        throw RCT3InvalidValueException(FinishNodeError(nodes+_(" tag, ") + attribute + _(" attribute: invalid value ")+t, node));
    return i;
}

double cRawParser::ParseFloat(const cXmlNode& node, const wxString& nodes, const wxString& attribute) {
    wxString t = node.wxgetPropVal(attribute);
    MakeVariable(t);
    double i;
    if (t.IsEmpty())
        throw RCT3Exception(FinishNodeError(nodes+_(" tag misses ") + attribute + _(" attribute."), node));
    string ts = static_cast<const char*>(t.utf8_str());
    if (!parseFloat(ts, i))
        throw RCT3InvalidValueException(FinishNodeError(nodes+_(" tag, ") + attribute + _(" attribute: invalid value ")+t, node));
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
    string temp;
    cXmlNode child(node.children());
    while(child) {
        if (child("row1")) {
            missrow[0] = false;
            temp = child.content();
            if (!parseMatrixRow(temp, m._11, m._12, m._13, m._14))
                throw RCT3InvalidValueException(FinishNodeError(nodes+_(" tag, row1: invalid content: ")+temp, child));
        } else if (child("row2")) {
            missrow[1] = false;
            temp = child.content();
            if (!parseMatrixRow(temp, m._21, m._22, m._23, m._24))
                throw RCT3InvalidValueException(FinishNodeError(nodes+_(" tag, row2: invalid content: ")+temp, child));
        } else if (child("row3")) {
            missrow[2] = false;
            temp = child.content();
            if (!parseMatrixRow(temp, m._31, m._32, m._33, m._34))
                throw RCT3InvalidValueException(FinishNodeError(nodes+_(" tag, row3: invalid content: ")+temp, child));
        } else if (child("row4")) {
            missrow[3] = false;
            temp = child.content();
            if (!parseMatrixRow(temp, m._41, m._42, m._43, m._44))
                throw RCT3InvalidValueException(FinishNodeError(nodes+_(" tag, row4: invalid content: ")+temp, child));
        } else if (child.is(XML_ELEMENT_NODE)) {
            throw RCT3Exception(FinishNodeError(wxString::Format(_("Unknown tag '%s' in %s."), STRING_FOR_FORMAT(child.name()), nodes.c_str()), child));
        }
        child.go_next();
    }
    if (missrow[0] || missrow[1] || missrow[2] || missrow[3])
        throw RCT3Exception(FinishNodeError(wxString::Format(_("Missing matrix row in %s."), nodes.c_str()), node));
}

c3DLoaderOrientation cRawParser::ParseOrientation(const cXmlNode& node, const wxString& nodes, c3DLoaderOrientation defori) {
    c3DLoaderOrientation ori = defori;
    wxString hand = UTF8STRINGWRAP(node.getPropVal("handedness"));
    wxString up = UTF8STRINGWRAP(node.getPropVal("up"));
    if (hand.IsEmpty()) {
        if (!up.IsEmpty()) {
            throw RCT3Exception(FinishNodeError(wxString::Format(_("Hand attribute missing in %s."), up.c_str(), nodes.c_str()), node));
        }
        // Fall through to default
    } else if (hand == wxT("left")) {
        if (up.IsEmpty()) {
            throw RCT3Exception(FinishNodeError(wxString::Format(_("Up attribute missing in %s."), up.c_str(), nodes.c_str()), node));
        } else if (up == wxT("x")) {
            ori = ORIENTATION_LEFT_XUP;
        } else if (up == wxT("y")) {
            ori = ORIENTATION_LEFT_YUP;
        } else if (up == wxT("z")) {
            ori = ORIENTATION_LEFT_ZUP;
        } else {
            throw RCT3Exception(FinishNodeError(wxString::Format(_("Unknown value '%s' for up attribute in %s."), up.c_str(), nodes.c_str()), node));
        }
    } else if (hand == wxT("right")) {
        if (up.IsEmpty()) {
            throw RCT3Exception(FinishNodeError(wxString::Format(_("Up attribute missing in %s."), up.c_str(), nodes.c_str()), node));
        } else if (up == wxT("x")) {
            ori = ORIENTATION_RIGHT_XUP;
        } else if (up == wxT("y")) {
            ori = ORIENTATION_RIGHT_YUP;
        } else if (up == wxT("z")) {
            ori = ORIENTATION_RIGHT_ZUP;
        } else {
            throw RCT3Exception(FinishNodeError(wxString::Format(_("Unknown value '%s' for up attribute in %s."), up.c_str(), nodes.c_str()), node));
        }
    } else {
        throw RCT3Exception(FinishNodeError(wxString::Format(_("Unknown value '%s' for handedness attribute in %s."), up.c_str(), nodes.c_str()), node));
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
            throw RCT3Exception(FinishNodeError(wxString::Format(_("Unknown tag '%s' in variables tag."), STRING_FOR_FORMAT(child.name())), child));
        }
        child.go_next();
    }
}

void cRawParser::ParseAttraction(const cXmlNode& node, cAttraction& attraction) {
    USE_PREFIX(node);
    OPTION_PARSE(unsigned long, attraction.type, ParseUnsigned(node, wxT(RAWXML_ATTRACTION), wxT("type")));
    OPTION_PARSE(unsigned char, attraction.version, ParseUnsigned(node, wxT(RAWXML_ATTRACTION), wxT("version")));
    ParseStringOption(attraction.icon, node, "icon", NULL, useprefix);
    ParseStringOption(attraction.spline, node, "loopSpline", NULL, useprefix);

    foreach(const cXmlNode& child, node.children()) {
        DO_CONDITION_COMMENT_FOR(child);

        if (child(RAWXML_ATTRACTION_PATH)) {
            USE_PREFIX(child);
            string path = ParseString(child, RAWXML_ATTRACTION_PATH, "spline", NULL, useprefix);
            attraction.splines.push_back(path);
        } else if (child(RAWXML_ATTRACTION_MISC)) {
            OPTION_PARSE(unsigned long, attraction.baseUpkeep, ParseUnsigned(child, wxT(RAWXML_ATTRACTION_MISC), wxT("baseUpkeep")));
            OPTION_PARSE(unsigned long, attraction.unk2, ParseUnsigned(child, wxT(RAWXML_ATTRACTION_MISC), wxT("u2")));
            OPTION_PARSE(unsigned long, attraction.unk3, ParseUnsigned(child, wxT(RAWXML_ATTRACTION_MISC), wxT("u3")));
            OPTION_PARSE(unsigned long, attraction.flags, ParseUnsigned(child, wxT(RAWXML_ATTRACTION_MISC), wxT("flags")));
            OPTION_PARSE(long, attraction.maxHeight, ParseSigned(child, wxT(RAWXML_ATTRACTION_MISC), wxT("maxHeight")));
        } else if (child(RAWXML_ATTRACTION_STATIC_UNK)) {
            OPTION_PARSE(unsigned long, attraction.unk4, ParseUnsigned(child, wxT(RAWXML_ATTRACTION_STATIC_UNK), wxT("u4")));
            OPTION_PARSE(unsigned long, attraction.unk5, ParseUnsigned(child, wxT(RAWXML_ATTRACTION_STATIC_UNK), wxT("u5")));
        } else if (child(RAWXML_ATTRACTION_EXTENSION)) {
            OPTION_PARSE(unsigned long, attraction.addonascn, ParseUnsigned(child, wxT(RAWXML_ATTRACTION_EXTENSION), wxT("addonAssociation")));
            OPTION_PARSE(unsigned long, attraction.unk12, ParseUnsigned(child, wxT(RAWXML_ATTRACTION_EXTENSION), wxT("u12")));
            OPTION_PARSE(unsigned long, attraction.ui_deactivation, ParseUnsigned(child, wxT(RAWXML_ATTRACTION_EXTENSION), wxT("uiDeactivationFlags")));
        } else if (child.element()) {
            throw RCT3Exception(FinishNodeError(wxString::Format(_("Unknown tag '%s' in attraction tag."), STRING_FOR_FORMAT(child.name())), child));
        }
    }
}

void cRawParser::ParseRide(const xmlcpp::cXmlNode& node, cRide& ride) {
    OPTION_PARSE(unsigned long, ride.seating, ParseUnsigned(node, wxT(RAWXML_RIDE), wxT("seating")));
    OPTION_PARSE(unsigned long, ride.attractivity, ParseUnsigned(node, wxT(RAWXML_RIDE), wxT("attractivity")));
    OPTION_PARSE(unsigned long, ride.entry_fee, ParseUnsigned(node, wxT(RAWXML_RIDE), wxT("entryFee")));
    OPTION_PARSE(unsigned long, ride.min_circuits, ParseUnsigned(node, wxT(RAWXML_RIDE), wxT("minCircuits")));
    OPTION_PARSE(long, ride.max_circuits, ParseSigned(node, wxT(RAWXML_RIDE), wxT("maxCircuits")));

    foreach(const cXmlNode& child, node.children()) {
        DO_CONDITION_COMMENT_FOR(child);

        if (child(RAWXML_RIDE_OPTION)) {
            cRideOption option;
            option.type = ParseUnsigned(child, RAWXML_RIDE_OPTION, "type");
            OPTION_PARSE(unsigned short, option.option.suboption, ParseUnsigned(child, wxT(RAWXML_RIDE_OPTION), wxT("suboptionValue")));
            OPTION_PARSE(unsigned short, option.option.group, ParseUnsigned(child, wxT(RAWXML_RIDE_OPTION), wxT("suboptionGroup")));
            int p = 0;
            foreach(const cXmlNode& param, child.children()) {
                if (param(RAWXML_RIDE_OPTION_PARAMETER)) {
                    if (p >= 4)
                        break;
                    parseFloatC(param(), option.param[p]);
                    ++p;
                }
            }
            ride.options.push_back(option);
        } else if (child(RAWXML_RIDE_STATIC_UNK)) {
            OPTION_PARSE(unsigned long, ride.unk11, ParseUnsigned(child, wxT(RAWXML_RIDE_STATIC_UNK), wxT("u11")));
            OPTION_PARSE(unsigned long, ride.unk12, ParseUnsigned(child, wxT(RAWXML_RIDE_STATIC_UNK), wxT("u12")));
            OPTION_PARSE(long, ride.unk13, ParseSigned(child, wxT(RAWXML_RIDE_STATIC_UNK), wxT("u13")));
            OPTION_PARSE(long, ride.unk14, ParseSigned(child, wxT(RAWXML_RIDE_STATIC_UNK), wxT("u14")));
            OPTION_PARSE(long, ride.unk15, ParseSigned(child, wxT(RAWXML_RIDE_STATIC_UNK), wxT("u15")));
            OPTION_PARSE(unsigned long, ride.unk16, ParseUnsigned(child, wxT(RAWXML_RIDE_STATIC_UNK), wxT("u16")));
            OPTION_PARSE(unsigned long, ride.unk17, ParseUnsigned(child, wxT(RAWXML_RIDE_STATIC_UNK), wxT("u17")));
        } else if (child(RAWXML_RIDE_STATION_LIMIT)) {
            cRideStationLimit limit;
            ParseRideStationLimit(child, limit);
            ride.stationlimits.push_back(limit);
        } else if (child.element()) {
            throw RCT3Exception(FinishNodeError(wxString::Format(_("Unknown tag '%s' in ride tag."), STRING_FOR_FORMAT(child.name())), child));
        }
    }
}

void cRawParser::ParseRideStationLimit(const cXmlNode& node, cRideStationLimit& limit) {
    limit.x_pos = ParseSigned(node, wxT(RAWXML_RIDE_STATION_LIMIT), wxT("xPos"));
    limit.z_pos = ParseSigned(node, wxT(RAWXML_RIDE_STATION_LIMIT), wxT("zPos"));
    OPTION_PARSE(long, limit.height, ParseSigned(node, wxT(RAWXML_RIDE_STATION_LIMIT), wxT("height")));
    OPTION_PARSE(unsigned long, limit.flags, ParseUnsigned(node, wxT(RAWXML_RIDE_STATION_LIMIT), wxT("flags")));
}
