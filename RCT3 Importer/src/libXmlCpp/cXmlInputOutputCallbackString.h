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

#include "cxmlconfig.h"

#include <map>
#include <string>
#include <vector>

#include "cXmlException.h"
#include "cXmlInputCallback.h"
#include "cXmlOutputCallback.h"

namespace xmlcpp {

#define XMLCPP_RES_ADD(resname, type) xmlcpp::cXmlInputOutputCallbackString::addInternal("/" #type "/" #resname "." #type, resname ## _ ## type, resname ## _ ## type ## _size, resname ## _ ## type ## _size_unzipped)
#define XMLCPP_RES_USE(resname, type) xmlcpp::cXmlInputOutputCallbackString::getInternalName("/" #type "/" #resname "." #type)
#define XMLCPP_RES_HAS(resname, type) xmlcpp::cXmlInputOutputCallbackString::hasFileInternal("/" #type "/" #resname "." #type)
#define XMLCPP_RES_GET(resname, type) xmlcpp::cXmlInputOutputCallbackString::getInternal("/" #type "/" #resname "." #type)
#define XMLCPP_RES_ADD_ONCE(resname, type) \
    if (!XMLCPP_RES_HAS(resname, type)) { \
        XMLCPP_RES_ADD(resname, type); \
    }
class cXmlInputOutputCallbackString : public cXmlInitHandler, public cXmlOutputCallback, public cXmlInputCallback {
public:
    static const char* PROTOCOL;
    static const char* INTERNAL;
    static bool Init();
    static void add(const std::string& filename, const std::string& content);
    static void add(const std::string& filename, const unsigned char* data, unsigned int datasize, unsigned int unzippedsize = 0);
    static inline void addInternal(const std::string& filename, const std::string& content) {
        add(INTERNAL + filename, content);
    }
    static inline void addInternal(const std::string& filename, const unsigned char* data, unsigned int datasize, unsigned int unzippedsize = 0) {
        add(INTERNAL + filename, data, datasize, unzippedsize);
    }
    static inline std::string getInternalName(const std::string& filename) {
#ifdef DEBUG
		if (!hasFileInternal(filename)) {
			throw eXml(std::string("getInternalName, Internal file missing: ")+filename);
		}
#endif
        return std::string(cXmlInputOutputCallbackString::PROTOCOL)+std::string(cXmlInputOutputCallbackString::INTERNAL)+filename;
    }
    static const std::string& get(const std::string& filename);
    static inline const std::string& getInternal(const std::string& filename) {
        return get(std::string(cXmlInputOutputCallbackString::INTERNAL)+filename);
    }
    static void remove(const std::string& filename);
    static bool hasFile(const std::string& filename);
    static inline bool hasFileInternal(const std::string& filename) {
        return hasFile(std::string(cXmlInputOutputCallbackString::INTERNAL)+filename);
    }

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
