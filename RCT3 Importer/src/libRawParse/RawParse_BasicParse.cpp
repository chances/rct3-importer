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

void cRawParser::ParseVector(wxXmlNode* node, VECTOR& v, const wxString& nodes) {
    v.x = ParseFloat(node, nodes, wxT("x"));
    v.y = ParseFloat(node, nodes, wxT("y"));
    v.z = ParseFloat(node, nodes, wxT("z"));
}

void cRawParser::ParseMatrix(wxXmlNode* node, MATRIX& m, const wxString& nodes) {
    m = matrixGetUnity();

    bool missrow[4] = {true, true, true, true};
    wxString temp;
    wxXmlNode* child = node->GetChildren();
    while(child) {
        if (child->GetName() == wxT("row1")) {
            missrow[0] = false;
            temp = child->GetNodeContent();
            if (sscanf(temp.mb_str(wxConvLocal), "%f %f %f %f", &m._11, &m._12, &m._13, &m._14) != 4)
                throw RCT3InvalidValueException(nodes+_(" tag, row1: invalid content: ")+temp);
        } else if (child->GetName() == wxT("row2")) {
            missrow[1] = false;
            temp = child->GetNodeContent();
            if (sscanf(temp.mb_str(wxConvLocal), "%f %f %f %f", &m._21, &m._22, &m._23, &m._24) != 4)
                throw RCT3InvalidValueException(nodes+_(" tag, row2: invalid content: ")+temp);
        } else if (child->GetName() == wxT("row3")) {
            missrow[2] = false;
            temp = child->GetNodeContent();
            if (sscanf(temp.mb_str(wxConvLocal), "%f %f %f %f", &m._31, &m._32, &m._33, &m._34) != 4)
                throw RCT3InvalidValueException(nodes+_(" tag, row3: invalid content: ")+temp);
        } else if (child->GetName() == wxT("row4")) {
            missrow[3] = false;
            temp = child->GetNodeContent();
            if (sscanf(temp.mb_str(wxConvLocal), "%f %f %f %f", &m._41, &m._42, &m._43, &m._44) != 4)
                throw RCT3InvalidValueException(nodes+_(" tag, row4: invalid content: ")+temp);
        } else if COMPILER_WRONGTAG(child) {
            throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in %s."), child->GetName().c_str(), nodes.c_str()));
        }
        child = child->GetNext();
    }
    if (missrow[0] || missrow[1] || missrow[2] || missrow[3])
        throw RCT3Exception(wxString::Format(_("Missing matrix row in %s."), nodes.c_str()));
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
        wxFSFileName src;
        if (path.IsEmpty()) {
            src = m_input.GetPath();
        } else {
            src = wxFSFileName(path, true);
        }
        wxFSFileName filename = inc;
        if (!filename.IsAbsolute())
            filename.MakeAbsolute(src.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME));
        LoadVariables(filename, command, (m_bake.Index(RAWXML_VARIABLES)!=wxNOT_FOUND)?node:NULL );

        if (m_mode == MODE_BAKE) {
            if (m_bake.Index(RAWXML_VARIABLES) == wxNOT_FOUND) {
                wxString newfile;
                if (m_bake.Index(RAWBAKE_ABSOLUTE) == wxNOT_FOUND) {
                    filename.MakeRelativeTo(m_bakeroot.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME));
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

cRawDatablock cRawParser::MakeDataBlock(const wxString& ref, const wxFSFileName& input, wxXmlNode* node, wxXmlNode* refnode) {
    wxXmlNode* activenode = refnode?refnode:node;
    cRawDatablock block(node, refnode, refnode?NULL:m_firstchild);

    wxString type = ParseString(activenode, refnode?RAWXML_DATAREF:RAWXML_DATA, wxT("type"), NULL);
    wxString datatype;
    ParseStringOption(datatype, activenode, wxT("datatype"), NULL);
    block.datatype(datatype);

    if (type.IsSameAs(wxT("file"))) {
        wxString filename = node->GetNodeContent();
        MakeVariable(filename);
        wxFSFileName fname = filename;
        if (!fname.IsAbsolute())
            fname.MakeAbsolute(input.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME));
        if (datatype.IsEmpty())
            block.datatype(fname.GetExt());

        wxFileSystem fs;
        std::auto_ptr<wxFSFile> file(fs.OpenFile(fname.GetFullPath(), wxFS_READ|wxFS_SEEKABLE));
        if (file.get()) {
            wxInputStream* filestream = file->GetStream(); // Stream is destroyed by wxFSFile
            filestream->SeekI(0, wxFromEnd);
            block.datasize(filestream->TellI());
            filestream->SeekI(0);
            std::auto_ptr<wxMemoryOutputStream> buffer(new wxMemoryOutputStream(block.data().get(), block.datasize()));
            buffer->Write(*filestream);
        } else {
            throw RCT3Exception(wxString::Format(_("File '%s' in data tag not found."), fname.GetFullPath().c_str()));
        }
    } else if (type.IsSameAs(wxT("binary"))) {
        wxString tex = node->GetNodeContent();
        MakeVariable(tex);

        unsigned long outlen = tex.Length();
        boost::shared_array<unsigned char> data(new unsigned char[tex.Length()]);
        int bret = base64_decode(reinterpret_cast<const unsigned char*>(static_cast<const char*>(tex.ToAscii())), tex.Length(), data.get(), &outlen);
        switch (bret) {
            case CRYPT_OK:
                break;
            case CRYPT_INVALID_PACKET:
                throw RCT3Exception(wxString(_("Decoding error in data tag ")) + ref);
            case CRYPT_BUFFER_OVERFLOW:
                throw RCT3Exception(wxString(_("Buffer overflow decoding tex tag ")) + ref);
            default:
                throw RCT3Exception(wxString(_("Unknown base64 error decoding tex tag ")) + ref);
        }
        block.datasize(outlen);
        memcpy(block.data().get(), data.get(), outlen);
    } else if (type.IsSameAs(wxT("data"))) {
        wxString tex = node->GetNodeContent();
        MakeVariable(tex);

        const wxCharBuffer buf = tex.mb_str(wxConvUTF8);
        block.datasize(strlen(buf.data()));
        memcpy(block.data().get(), buf.data(), strlen(buf.data()));

    } else {
        throw RCT3Exception(wxString::Format(_("Unknown type '%s' in data tag %s."), type.c_str(), ref.c_str()));
    }

    return block;

}

cRawDatablock cRawParser::GetDataBlock(const wxString& ref, wxXmlNode* node) {
    wxString type = ParseString(node, RAWXML_DATA, wxT("type"), NULL);
    if (type.IsSameAs(wxT("reference"))) {
        wxString guid = node->GetNodeContent();
        MakeVariable(guid);
        guid.MakeUpper();
        wxFSFileName input;
        wxXmlNode* refd = FindDataReference(guid, input);
        if (!refd)
            throw RCT3Exception(wxString::Format(_("GUID '%s' not found in data tag %s."), guid.c_str(), ref.c_str()));
        return MakeDataBlock(ref, input, node, refd);
    } else {
        return MakeDataBlock(ref, m_input, node);
    }
}

