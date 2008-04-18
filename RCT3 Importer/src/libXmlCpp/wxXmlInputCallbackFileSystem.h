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

#ifndef WXXMLINPUTCALLBACKFILESYSTEM_H
#define WXXMLINPUTCALLBACKFILESYSTEM_H

#include "cxmlconfig.h"

#ifdef XMLCPP_USE_WXWIDGETS

#include <boost/shared_ptr.hpp>
#include <map>
#include <vector>
#include <wx/filesys.h>

#include "cXmlInputCallback.h"

namespace xmlcpp {

class wxXmlInputCallbackFileSystem : public cXmlInitHandler, public cXmlInputCallback {
public:
    static void Init();
    static wxXmlInputCallbackFileSystem* g_instance;
protected:
    virtual bool match_in(const char* filename);
    virtual unsigned long open_in(const char* filename);
    virtual void close_in(unsigned long id);
    virtual int read(unsigned long id, char* buffer, int len);
private:
    unsigned long m_nextid;
    wxFileSystem m_fs;
    std::map<unsigned long, boost::shared_ptr<wxFSFile> > m_files;
    std::vector<unsigned long> m_idpool;

    wxXmlInputCallbackFileSystem();
    virtual ~wxXmlInputCallbackFileSystem();
};

} // Namespace

#endif

#endif // WXXMLINPUTCALLBACKFILESYSTEM_H
