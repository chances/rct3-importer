///////////////////////////////////////////////////////////////////////////////
//
// libXmlCpp
// A light C++ wrapper for libxml2, libxslt and libexslt
// Copyright (C) 2008 Tobias Minch
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 3
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, <http://www.gnu.org/licenses/>.
//
// Written by
//   Tobias Minich - belgabor@gmx.de
//
///////////////////////////////////////////////////////////////////////////////


#ifdef XMLCPP_USE_WXWIDGETS

#include "wxXmlInputCallbackFileSystem.h"

//#define DUMP

namespace xmlcpp {

wxXmlInputCallbackFileSystem* wxXmlInputCallbackFileSystem::g_instance = NULL;

void wxXmlInputCallbackFileSystem::Init() {
    if (!g_instance) {
        g_instance = new wxXmlInputCallbackFileSystem();
    }
}

wxXmlInputCallbackFileSystem::wxXmlInputCallbackFileSystem():cXmlInitHandler(), cXmlInputCallback() {
    m_nextid = 0;
    make_singleton();
}

wxXmlInputCallbackFileSystem::~wxXmlInputCallbackFileSystem() {
    //dtor
}

bool wxXmlInputCallbackFileSystem::match_in(const char* filename) {
#ifdef DUMP
    bool r = wxFileSystem::HasHandlerForPath(wxString(filename, wxConvUTF8));
fprintf(stderr, "wxXmlInputCallbackFileSystem match_in: %s %s\n", filename, r?"YES":"NO");
    return r;
#else
    return wxFileSystem::HasHandlerForPath(wxString(filename, wxConvUTF8));
#endif
}

unsigned long wxXmlInputCallbackFileSystem::open_in(const char* filename) {
    boost::shared_ptr<wxFSFile> f(m_fs.OpenFile(wxString(filename, wxConvUTF8), wxFS_READ));
#ifdef DUMP
fprintf(stderr, "wxXmlInputCallbackFileSystem open_in: %s\n", filename);
#endif
    if (f.get()) {
        unsigned long id;
        if (m_idpool.size()) {
            id = m_idpool.back();
            m_idpool.pop_back();
        } else {
            id = m_nextid++;
        }
        m_files[id] = f;
        return id;
    } else {
#ifdef DUMP
fprintf(stderr, "wxXmlInputCallbackFileSystem open_in: %s FAILED!\n", filename);
#endif
        return -1;
    }
}

void wxXmlInputCallbackFileSystem::close_in(unsigned long id) {
    std::map<unsigned long, boost::shared_ptr<wxFSFile> >::iterator it_f = m_files.find(id);
    if (it_f != m_files.end()) {
        m_files.erase(id);
        m_idpool.push_back(id);
    }
}

int wxXmlInputCallbackFileSystem::read(unsigned long id, char* buffer, int len) {
    std::map<unsigned long, boost::shared_ptr<wxFSFile> >::iterator it_f = m_files.find(id);
    if (it_f != m_files.end()) {
        wxInputStream* is = it_f->second->GetStream();
        is->Read(buffer, len);
        return is->LastRead();
    } else {
        return -1;
    }
}

} // Namespace

#endif
