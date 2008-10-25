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

#include "base64.h"

cXmlNode* cRawParser::FindDataReference(const wxString& guid, wxFSFileName& input) {
    std::map<wxString, cXmlNode>::iterator it = m_datareferences.find(guid);
    if (it != m_datareferences.end()) {
        input = m_input;
        return &it->second;
    } else {
        if (m_parent) {
            return m_parent->FindDataReference(guid, input);
        } else {
            return NULL;
        }
    }
}

cRawDatablock cRawParser::MakeDataBlock(const wxString& ref, const wxFSFileName& input, const cXmlNode& node, const cXmlNode& refnode) {
    cXmlNode activenode = refnode?refnode:node;
    cRawDatablock block(node, refnode, refnode?cXmlNode():m_firstchild);

    wxString type = ParseString(activenode, refnode?wxT(RAWXML_DATAREF):wxT(RAWXML_DATA), wxT("type"), NULL);
    wxString datatype;
    ParseStringOption(datatype, activenode, wxT("datatype"), NULL);
    block.datatype(datatype);

    if (type.IsSameAs(wxT("file"))) {
        wxString filename = node.wxcontent();
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
        wxString tex = node.wxcontent();
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
        wxString tex = node.wxcontent();
        MakeVariable(tex);

        const wxCharBuffer buf = tex.mb_str(wxConvUTF8);
        block.datasize(strlen(buf.data()));
        memcpy(block.data().get(), buf.data(), strlen(buf.data()));

    } else {
        throw RCT3Exception(wxString::Format(_("Unknown type '%s' in data tag %s."), type.c_str(), ref.c_str()));
    }

    return block;

}

cRawDatablock cRawParser::GetDataBlock(const wxString& ref, const cXmlNode& node) {
    wxString type = ParseString(node, RAWXML_DATA, "type", NULL);
    if (type.IsSameAs("reference")) {
        wxString guid = node.wxcontent();
        MakeVariable(guid);
        guid.MakeUpper();
        wxFSFileName input;
        cXmlNode* refd = FindDataReference(guid, input);
        if (!refd)
            throw RCT3Exception(wxString::Format(_("GUID '%s' not found in data tag %s."), guid.c_str(), ref.c_str()));
        return MakeDataBlock(ref, input, node, *refd);
    } else {
        return MakeDataBlock(ref, m_input, node);
    }
}


