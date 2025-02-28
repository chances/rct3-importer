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


#include "cXmlDoc.h"

#include <sstream>
#include <stdio.h>

#include <libxml/xinclude.h>

#include "cXmlException.h"
#include "cXmlNode.h"
#include "cXmlValidator.h"

//#define DUMP
//#define DUMPINIT

using namespace std;

namespace xmlcpp {

//void errorwrap(void* ptr, xmlErrorPtr err) {
//    if (ptr) {
//        reinterpret_cast<cXmlDoc*>(ptr)->errorcallback(err);
//    }
//}

//vector<string> cXmlDoc::m_errors;
//
//static void errorcallback(void * ctx, const char * msg, ...) {
//    va_list argp;
//    char str[4096];
//fprintf(stderr, "Test\n");
//	va_start(argp, msg);
//	//vsnprintf(str, 4095, msg, argp);
//fprintf(stderr, msg, argp);
//	va_end(argp);
//
//	cXmlDoc::m_errors.push_back(str);
//}

cXmlDoc::cXmlDoc() {
#ifdef DUMPINIT
fprintf(stderr, "cXmlDoc()\n");
#endif
    Init();
}

cXmlDoc::cXmlDoc(bool asempty) {
#ifdef DUMPINIT
fprintf(stderr, "cXmlDoc(), asempty\n");
#endif
    if (asempty)
        empty();
    else
        Init();
}

cXmlDoc::cXmlDoc(xmlDocPtr doc, bool grab) {
#ifdef DUMPINIT
fprintf(stderr, "cXmlDoc(), docptr\n");
#endif
    if (grab)
        assignDoc(doc);
    else
        read(doc);
}

cXmlDoc::cXmlDoc(const boost::shared_ptr<xmlDoc>& sharedoc) {
#ifdef DUMPINIT
fprintf(stderr, "cXmlDoc(), share\n");
#endif
    share(sharedoc);
}

cXmlDoc::cXmlDoc(const char* filename, const char* encoding, int options) {
#ifdef DUMPINIT
fprintf(stderr, "cXmlDoc(), filename\n");
#endif
    read(filename, encoding, options);
}

cXmlDoc::cXmlDoc(const std::string& buffer, const char* URL, const char* encoding, int options) {
#ifdef DUMPINIT
fprintf(stderr, "cXmlDoc(), buffer\n");
#endif
    read(buffer, URL, encoding, options);
}


cXmlDoc::~cXmlDoc() {
//    DeInit();
#ifdef DUMPINIT
fprintf(stderr, "~cXmlDoc() / ");
#endif
}

void cXmlDoc::Init() {
    m_file = "";
    m_doc.reset();
}

//void cXmlDoc::errorcallback(xmlErrorPtr err) {
//    stringstream str;
//    str << err->message << " in line " << err->line;
//    //wxLogError(wxT("%s in line %d. Error %d"), wxString(err->message, wxConvUTF8).c_str(), err->line, err->code);
//    m_errors.push_back(str.str());
//}

//void cXmlDoc::DeInit() {
//    if (m_doc) {
//        xmlFreeDoc(m_doc);
//        m_doc = NULL;
//    }
//}

void cXmlDoc::empty(const char* xmlver) {
    Init();
    assignDoc(xmlNewDoc(reinterpret_cast<const xmlChar*>(xmlver)));
}

bool cXmlDoc::read(const xmlChar* cur, const char* URL, const char* encoding, int options) {
//    DeInit();
#ifdef DUMP
fprintf(stderr, "cXmlDoc::read (xmlChar)\n");
fflush(stderr);
#endif
    Init();
    claimErrors();
    assignDoc(xmlReadDoc(cur, URL, encoding, options));
    releaseErrors();
    return ok();
}

bool cXmlDoc::read(const char* filename, const char* encoding, int options) {
//    DeInit();
#ifdef DUMP
fprintf(stderr, "cXmlDoc::read (file %s)\n", filename);
fflush(stderr);
#endif
    Init();
    claimErrors();
    assignDoc(xmlReadFile(filename, encoding, options));
    releaseErrors();
    return ok();
}

bool cXmlDoc::read(xmlDocPtr doc) {
#ifdef DUMP
fprintf(stderr, "cXmlDoc::read (xmlDocPtr)\n");
fflush(stderr);
#endif
    Init();
    claimErrors();
    assignDoc(xmlCopyDoc(doc, 1));
    releaseErrors();
    return ok();
}

bool cXmlDoc::share(const boost::shared_ptr<xmlDoc>& doc) {
    Init();
    m_doc = doc;
    return ok();
}

string cXmlDoc::write(bool indent, const char* encoding) {
    if (!m_doc)
        throw eXmlInvalid("Tried to write bad document");

    xmlChar* output;
    int outputlen;

    if (indent) {
        xmlDocDumpFormatMemoryEnc(m_doc.get(), &output, &outputlen, encoding, 1);
    } else {
        xmlDocDumpMemoryEnc(m_doc.get(), &output, &outputlen, encoding);
    }

    string ret;
    if (output) {
        ret = reinterpret_cast<char*>(output);
        xmlFree(output);
    }
    return ret;
}

int cXmlDoc::write(const char* filename, bool indent, const char* encoding) {
    if (!m_doc)
        throw eXmlInvalid("Tried to write bad document");

    if (indent) {
        return xmlSaveFormatFileEnc(filename, m_doc.get(), encoding, 1);
    } else {
        return xmlSaveFileEnc(filename, m_doc.get(), encoding);
    }
}

int cXmlDoc::dump(FILE* f) {
    if (!m_doc)
        throw eXmlInvalid("Tried to write bad document");

    return xmlDocDump(f, m_doc.get());
}

cXmlNode cXmlDoc::root() {
    if (!m_doc)
        throw eXmlInvalid("Tried to get root for bad document");
    return cXmlNode(xmlDocGetRootElement(m_doc.get()), m_doc);
}

cXmlNode cXmlDoc::root(cXmlNode& newroot) {
    if (!m_doc)
        throw eXmlInvalid("Tried to set root for bad document");
    return cXmlNode(xmlDocSetRootElement(m_doc.get(), newroot.getRaw(m_doc)), true);
}

std::string cXmlDoc::searchNs(const std::string& prefix, xmlNodePtr node) {
    if (!m_doc)
        throw eXmlInvalid("Tried to search namespace for bad document");

    xmlNsPtr ns = xmlSearchNs(m_doc.get(), node?node:xmlDocGetRootElement(m_doc.get()), (prefix=="")?NULL:reinterpret_cast<const xmlChar*>(prefix.c_str()));
    if (ns) {
        return ns->href?string(reinterpret_cast<const char*>(ns->href)):"";
    } else {
        return "";
    }
}

xmlNsPtr cXmlDoc::searchNsByHref(const std::string& href, xmlNodePtr node) {
    if (!m_doc)
        throw eXmlInvalid("Tried to search namespace for bad document");

    return xmlSearchNsByHref(m_doc.get(), node?node:xmlDocGetRootElement(m_doc.get()), reinterpret_cast<const xmlChar*>(href.c_str()));
}

xmlNsPtr cXmlDoc::getNs(const std::string& prefix, xmlNodePtr node) {
    if (!m_doc)
        throw eXmlInvalid("Tried to search namespace for bad document");

    return xmlSearchNs(m_doc.get(), node?node:xmlDocGetRootElement(m_doc.get()), (prefix=="")?NULL:reinterpret_cast<const xmlChar*>(prefix.c_str()));
}

#define DICT_FREE(str, dict)						\
	if ((str) && ((!dict) || 				\
	    (xmlDictOwns(dict, (const xmlChar *)(str)) == 0)))	\
	    xmlFree((char *)(str));

/** @brief setURL
  *
  * @todo: document this function
  */
void cXmlDoc::setURL(const char* URL) {
    if (!m_doc)
        throw eXmlInvalid("Tried to set URL for bad document");

    DICT_FREE(m_doc->URL, m_doc->dict)
    m_doc->URL = xmlStrdup(reinterpret_cast<const xmlChar*>(URL));
}

/** @brief setName
  *
  * @todo: document this function
  */
void cXmlDoc::setName(const char* name) {
    if (!m_doc)
        throw eXmlInvalid("Tried to set name for bad document");

    DICT_FREE(m_doc->name, m_doc->dict)
    m_doc->name = reinterpret_cast<char*>(xmlStrdup(reinterpret_cast<const xmlChar*>(name)));
}

cXmlValidatorResult cXmlDoc::validate(cXmlValidator& val, cXmlValidatorResult::LEVEL deflevel) {
    return val.validate(m_doc, deflevel, cXmlValidator::OPT_NONE);
}

cXmlValidatorResult cXmlDoc::validate(cXmlValidator& val, int options,  cXmlValidatorResult::LEVEL deflevel) {
    if (!m_doc)
        throw eXmlInvalid("Tried to validate bad document");
    return val.validate(m_doc, deflevel, options);
}

int cXmlDoc::xInclude() {
    if (!m_doc)
        throw eXmlInvalid("Tried to resolve xincludes for bad document");
    return xmlXIncludeProcess(m_doc.get());
}


} // Namespace
