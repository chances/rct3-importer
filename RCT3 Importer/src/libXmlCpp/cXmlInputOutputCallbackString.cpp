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

#include "cXmlInputOutputCallbackString.h"

#include <boost/shared_array.hpp>
#include <strings.h>
#include <zlib.h>

#include "cXmlException.h"

//#define DUMP
//#define DUMPINIT

using namespace std;

namespace xmlcpp {

const char* cXmlInputOutputCallbackString::PROTOCOL = "string:";
const char* cXmlInputOutputCallbackString::INTERNAL = "/INTERNAL";
cXmlInputOutputCallbackString* cXmlInputOutputCallbackString::g_instance = NULL;
const string cXmlInputOutputCallbackString::EMPTY = "";

bool cXmlInputOutputCallbackString::Init() {
    if (!g_instance) {
        g_instance = new cXmlInputOutputCallbackString();
        return true;
    }
    return false;
}

void cXmlInputOutputCallbackString::add(const std::string& filename, const std::string& content) {
    if (!g_instance)
        throw eXml("cXmlInputOutputCallbackString not initialized");

#ifdef DUMP
fprintf(stderr, "cXmlInputOutputCallbackString add: %s\n", filename.c_str());
#endif
    g_instance->m_files[filename] = content;
}

void cXmlInputOutputCallbackString::add(const std::string& filename, const unsigned char* data, unsigned int datasize, unsigned int unzippedsize) {
    if ((!unzippedsize) || (datasize == unzippedsize)) {
        add(filename, string(reinterpret_cast<const char*>(data)));
    }

    uLongf buf_size = unzippedsize;
    boost::shared_array<char> buf(new char[buf_size]);
    int r = uncompress(reinterpret_cast<Bytef*>(buf.get()), &buf_size, reinterpret_cast<const Bytef*>(data), datasize);
    if (r != Z_OK)
        throw eXml("Uncompressing "+filename+" failed.");
    string t(buf.get(), static_cast<size_t>(buf_size+1));
    add(filename, t);
}

const string& cXmlInputOutputCallbackString::get(const std::string& filename) {
    if (!g_instance)
        throw eXml("cXmlInputOutputCallbackString not initialized");
//fprintf(stderr, "get: %s\n", filename.c_str());

    map<string, string>::iterator it = g_instance->m_files.find(filename);
    if (it != g_instance->m_files.end()) {
        return it->second;
    }
    return EMPTY;
}

void cXmlInputOutputCallbackString::remove(const std::string& filename) {
    if (!g_instance)
        throw eXml("cXmlInputOutputCallbackString not initialized");

    map<string, string>::iterator it = g_instance->m_files.find(filename);
    if (it != g_instance->m_files.end()) {
        g_instance->m_files.erase(it);
    }
}

bool cXmlInputOutputCallbackString::hasFile(const std::string& filename) {
    if (!g_instance)
        throw eXml("cXmlInputOutputCallbackString not initialized");

    return g_instance->m_files.find(filename) != g_instance->m_files.end();
}

cXmlInputOutputCallbackString::cXmlInputOutputCallbackString(): cXmlInitHandler(), cXmlOutputCallback(), cXmlInputCallback() {
#ifdef DUMPINIT
fprintf(stderr, "cXmlInputOutputCallbackString()\n");
fflush(stderr);
#endif
    m_nextoutputid = 0;
    m_nextinputid = 0;
    make_singleton();
}

cXmlInputOutputCallbackString::~cXmlInputOutputCallbackString() {
#ifdef DUMPINIT
fprintf(stderr, "~cXmlInputOutputCallbackString() / ");
fflush(stderr);
#endif
    if (g_instance == this)
        g_instance = NULL;
}

bool cXmlInputOutputCallbackString::match_out(const char* filename) {
#ifdef DUMP
fprintf(stderr, "cXmlInputOutputCallbackString match_out: %s\n", filename);
#endif
    return !strncasecmp(filename, PROTOCOL, strlen(PROTOCOL));
}

string getfilename(const char* filename) {
    const char* fname = strchr(filename, ':');
    if (fname) {
        while ((fname[1] == '/') || (fname[0] == ':'))
            fname++;
        return string(fname);
    }
    return "";
}

bool cXmlInputOutputCallbackString::match_in(const char* filename) {
#ifdef DUMP
fprintf(stderr, "cXmlInputOutputCallbackString match_in: %s\n", filename);
#endif
    if (strncasecmp(filename, PROTOCOL, strlen(PROTOCOL)))
        return false;
    return m_files.find(getfilename(filename)) != m_files.end();
}

unsigned long cXmlInputOutputCallbackString::open_out(const char* filename) {
#ifdef DUMP
fprintf(stderr, "cXmlInputOutputCallbackString open_out: %s\n", filename);
#endif
    string sfname = getfilename(filename);
    if (sfname.size()) {
        unsigned long id = -1;
        if (m_outputidpool.size()) {
            id = m_outputidpool.back();
            m_outputidpool.pop_back();
        } else {
            id = m_nextoutputid++;
        }
        m_outputhandles[id] = cXmlOutputCallbackStringStatus(true, sfname);
        return id;
    }
    return -1;
}

unsigned long cXmlInputOutputCallbackString::open_in(const char* filename) {
#ifdef DUMP
fprintf(stderr, "cXmlInputOutputCallbackString open_in: %s\n", filename);
#endif
    if (!match_in(filename))
        return -1;

    string sfname = getfilename(filename);
    if (sfname.size()) {
        map<string, string>::iterator it = m_files.find(sfname);
        if (it != m_files.end()) {
            unsigned long id = -1;
            if (m_inputidpool.size()) {
                id = m_inputidpool.back();
                m_inputidpool.pop_back();
            } else {
                id = m_nextinputid++;
            }
            m_inputhandles[id] = cXmlInputCallbackStringStatus(it->second);
            return id;
        }
    }
    return -1;
}

void cXmlInputOutputCallbackString::close_out(unsigned long id) {
    map<unsigned long, cXmlOutputCallbackStringStatus>::iterator it_f = m_outputhandles.find(id);
    if (it_f != m_outputhandles.end()) {
        m_files[it_f->second.filename] = it_f->second.content;
        m_outputhandles.erase(it_f);
        m_outputidpool.push_back(id);
    }
}

void cXmlInputOutputCallbackString::close_in(unsigned long id) {
    map<unsigned long, cXmlInputCallbackStringStatus>::iterator it_f = m_inputhandles.find(id);
    if (it_f != m_inputhandles.end()) {
        m_inputhandles.erase(it_f);
        m_inputidpool.push_back(id);
    }
}

int cXmlInputOutputCallbackString::write(unsigned long id, const char* buffer, int len) {
    map<unsigned long, cXmlOutputCallbackStringStatus >::iterator it_f = m_outputhandles.find(id);
    if (it_f != m_outputhandles.end()) {
        it_f->second.content += buffer;
        return len;
    } else {
        return -1;
    }
}

int cXmlInputOutputCallbackString::read(unsigned long id, char* buffer, int len) {
    map<unsigned long, cXmlInputCallbackStringStatus >::iterator it_f = m_inputhandles.find(id);
    if (it_f != m_inputhandles.end()) {
        int to_read = min(len, static_cast<int>(it_f->second.content.size()) - static_cast<int>(it_f->second.position));
        strncpy(buffer, &it_f->second.content[it_f->second.position], to_read);
        it_f->second.position += to_read;
        return to_read;
    } else {
        return -1;
    }
}

} // Namespace
