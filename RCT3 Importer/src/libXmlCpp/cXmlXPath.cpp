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

#include "cXmlXPath.h"

#include <libxml/xpathInternals.h>
#include <boost/tokenizer.hpp>

#include "cXmlDoc.h"
#include "cXmlException.h"
//#include "cXmlNode.h"
#include "cXmlXPathResult.h"

//#define DUMPINIT
//#define DUMP

using namespace std;

namespace xmlcpp {

cXmlXPath::cXmlXPath(const boost::shared_ptr<xmlDoc>& doc, const std::string& prefix, const std::string& uri) {
#ifdef DUMPINIT
fprintf(stderr, "cXmlXPath(), xmlDoc\n");
fflush(stderr);
#endif
    assign(doc);
    if (prefix != "")
        registerNs(prefix, uri);
}

cXmlXPath::cXmlXPath(const cXmlDoc& doc, const std::string& prefix, const std::string& uri) {
#ifdef DUMPINIT
fprintf(stderr, "cXmlXPath(), cXmlDoc\n");
fflush(stderr);
#endif
    assign(doc.get());
    if (prefix != "")
        registerNs(prefix, uri);
}

cXmlXPath::cXmlXPath(xmlDocPtr doc, const std::string& prefix, const std::string& uri) {
#ifdef DUMPINIT
fprintf(stderr, "cXmlXPath(), cXmlDoc\n");
fflush(stderr);
#endif
    assign(doc);
    if (prefix != "")
        registerNs(prefix, uri);
}

cXmlXPath::cXmlXPath(xmlDocPtr doc, bool grab, const std::string& prefix, const std::string& uri) {
#ifdef DUMPINIT
fprintf(stderr, "cXmlXPath(), cXmlDoc\n");
fflush(stderr);
#endif
    assign(doc, grab);
    if (prefix != "")
        registerNs(prefix, uri);
}

cXmlXPath::~cXmlXPath() {
#ifdef DUMPINIT
fprintf(stderr, "~cXmlXPath()\n");
fflush(stderr);
#endif
}

void cXmlXPath::Init() {
    m_voldoc = NULL;
    m_doc.reset();
    m_context.reset();
}

bool cXmlXPath::assign(const boost::shared_ptr<xmlDoc>& doc) {
    Init();
    m_doc = doc;
    create();
    return ok();
}

bool cXmlXPath::assign(xmlDocPtr doc) {
    Init();
    m_voldoc = doc;
    create();
    return ok();
}

bool cXmlXPath::assign(const cXmlDoc& doc) {
    return assign(doc.get());
}

bool cXmlXPath::assign(xmlDocPtr doc, bool grab) {
    Init();
    m_doc = boost::shared_ptr<xmlDoc>(grab?doc:xmlCopyDoc(doc, 1), xmlFreeDoc);
    create();
    return ok();
}

void cXmlXPath::create() {
    if (!docptr())
        throw eXml("Tried to create XPath from bad document");
    m_context = boost::shared_ptr<xmlXPathContext>(xmlXPathNewContext(docptr()), xmlXPathFreeContext);
}

bool cXmlXPath::registerNs(const std::string& prefix, const std::string& uri) {
    if (!ok())
        return false;
    return xmlXPathRegisterNs(m_context.get(), reinterpret_cast<const xmlChar*>(prefix.c_str()), reinterpret_cast<const xmlChar*>(uri.c_str()));
}

void cXmlXPath::setNodeContext(const xmlNodePtr node) {
    if (!ok())
        throw eXml("Tried to set node context on bad XPath context");
    m_context->node = node;
}

cXmlXPathResult cXmlXPath::eval(const char* query) {
    if (!ok())
        throw eXml("Tried to evaluate from bad XPath context");
    return cXmlXPathResult(xmlXPathEval(reinterpret_cast<const xmlChar*>(query), m_context.get()), m_doc);
}

cXmlXPathResult cXmlXPath::evaluate(const char* query) {
    if (!ok())
        throw eXml("Tried to evaluate from bad XPath context");
    return cXmlXPathResult(xmlXPathEvalExpression(reinterpret_cast<const xmlChar*>(query), m_context.get()), m_doc);
}

cXmlXPathResult cXmlXPath::operator()(const char* query, bool use_eval) {
    return use_eval?eval(query):evaluate(query);
}

std::string cXmlXPath::decorateWithNsPrefix(const std::string& path, const std::string& prefix, bool make_first) {
    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
    tokenizer tokens(path, boost::char_separator<char>("/"));
    string ret;
    for (tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter) {
        ret += '/';
        if (*tok_iter != "") {
            if (((*tok_iter)[0] != '@') && (tok_iter->find(':') == string::npos)) {
                ret += prefix;
                ret += ':';
            }
            ret += *tok_iter;
            if (make_first && ((*tok_iter)[0] != '@') && (tok_iter->find('[') == string::npos)) {
                ret += "[1]";
            }
        }
    }
    return ret;
}


} // Namespace end
