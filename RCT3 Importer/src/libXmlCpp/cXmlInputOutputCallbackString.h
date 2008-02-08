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

#ifndef CXMLOUTPUTCALLBACKSTRING_H
#define CXMLOUTPUTCALLBACKSTRING_H

#include <map>
#include <string>
#include <vector>

#include "cXmlInputCallback.h"
#include "cXmlOutputCallback.h"

namespace xmlcpp {

class cXmlInputOutputCallbackString : public cXmlInitHandler, public cXmlOutputCallback, public cXmlInputCallback {
public:
    static const char* PROTOCOL;
    static const char* INTERNAL;
    static void Init();
    static void add(const std::string& filename, const std::string& content);
    static const std::string& get(const std::string& filename);
    static void remove(const std::string& filename);
    static bool hasFile(const std::string& filename);

    static cXmlInputOutputCallbackString* g_instance;
private:
    static const std::string EMPTY;
private:
    cXmlInputOutputCallbackString();
    virtual ~cXmlInputOutputCallbackString();
protected:
    virtual bool match_out(const char* filename);
    virtual unsigned long open_out(const char* filename);
    virtual void close_out(unsigned long id);
    virtual int write(unsigned long id, const char* buffer, int len);

    virtual bool match_in(const char* filename);
    virtual unsigned long open_in(const char* filename);
    virtual void close_in(unsigned long id);
    virtual int read(unsigned long id, char* buffer, int len);
private:
    struct cXmlOutputCallbackStringStatus {
        std::string filename;
        std::string content;
        bool opened;
        cXmlOutputCallbackStringStatus(): opened(false) {}
        cXmlOutputCallbackStringStatus(bool _opened, const std::string& _filename): filename(_filename), opened(_opened) {}
    };
    unsigned long m_nextoutputid;
    std::map<unsigned long, cXmlOutputCallbackStringStatus > m_outputhandles;
    std::vector<unsigned long> m_outputidpool;

    struct cXmlInputCallbackStringStatus {
        std::string content;
        int position;
        cXmlInputCallbackStringStatus(): position(0) {}
        cXmlInputCallbackStringStatus(const std::string& _content): content(_content), position(0) {}
    };
    unsigned long m_nextinputid;
    std::map<unsigned long, cXmlInputCallbackStringStatus > m_inputhandles;
    std::vector<unsigned long> m_inputidpool;

    std::map<std::string, std::string> m_files;
};

} // Namespace

#endif // WXXMLINPUTCALLBACKFILESYSTEM_H
