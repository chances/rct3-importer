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


#include "cXsltStylesheet.h"

#ifdef XMLCPP_USE_XSLT

#include <libxslt/transform.h>
#include <libxslt/xsltutils.h>

#include "cXmlDoc.h"
#include "cXmlException.h"

//#define DUMPINIT
//#define DUMPDEBUG

using namespace std;

namespace xmlcpp {

cXsltStylesheet::cXsltStylesheet() {
#ifdef DUMPINIT
fprintf(stderr, "cXsltStylesheet()\n");
#endif
}

cXsltStylesheet::cXsltStylesheet(const char* filename) {
#ifdef DUMPINIT
fprintf(stderr, "cXsltStylesheet(), filename\n");
#endif
    read(filename);
}

cXsltStylesheet::cXsltStylesheet(cXmlDoc& doc) {
#ifdef DUMPINIT
fprintf(stderr, "cXsltStylesheet(), doc\n");
#endif
    read(doc);
}

cXsltStylesheet::~cXsltStylesheet() {
#ifdef DUMPINIT
fprintf(stderr, "~cXsltStylesheet() / ");
#endif
}

void cXsltStylesheet::Init() {
    //m_ctx.reset();
    m_sheet.reset();
    m_parameters.clear();
#ifdef DUMPDEBUG
    xsltSetLoaderFunc(cXsltStylesheet::xsltDocLoaderFunc);
#endif
}

void cXsltStylesheet::assignSheet(xsltStylesheetPtr newsheet) {
    m_sheet.reset(newsheet, xsltFreeStylesheet);
}

bool cXsltStylesheet::read(cXmlDoc& doc) {
    Init();
    if (!doc.ok())
        throw eXmlInvalid("Tried to parse xslt from bad document");

    xsltSetGenericErrorFunc(this, genericerrorwrap);
    assignSheet(xsltParseStylesheetDoc(xmlCopyDoc(doc.getRaw(), 1)));
    xsltSetGenericErrorFunc(NULL, NULL);
    if (ok())
        m_doc = doc.get();
    return ok();
}

bool cXsltStylesheet::read(const char* filename) {
    Init();
    xsltSetGenericErrorFunc(this, genericerrorwrap);
    assignSheet(xsltParseStylesheetFile(reinterpret_cast<const xmlChar*>(filename)));
    xsltSetGenericErrorFunc(NULL, NULL);
    return ok();
}

void cXsltStylesheet::addParameter(const string& name, const string& value) {
    m_parameters[name] = value;
}

void cXsltStylesheet::setParameters(const std::map<std::string, std::string>& parameters) {
    m_parameters = parameters;
}

#define TRANSFORM_CTX() \
    boost::shared_ptr<xsltTransformContext> ctx(xsltNewTransformContext(m_sheet.get(), doc), xsltFreeTransformContext); \
    xsltSetTransformErrorFunc(ctx.get(), this, genericerrorwrap);


cXmlDoc cXsltStylesheet::transform(cXmlDoc& doc) {
    if (!doc.ok())
        throw eXmlInvalid("Tried to transform bad document");

    return transform(doc.getRaw());
}

cXmlDoc cXsltStylesheet::transform(xmlDocPtr doc) {
    if (!doc)
        throw eXmlInvalid("Tried to transform bad document");
    if (!ok())
        throw eXmlInvalid("Tried to transform document with bad stylesheet");

    TRANSFORM_CTX()

    const char* params[(m_parameters.size()*2) + 1];
    params[m_parameters.size()*2] = NULL;
    int i = 0;
    for (map<string, string>::iterator it = m_parameters.begin(); it != m_parameters.end(); ++it) {
        params[(i*2)] = it->first.c_str();
        params[(i*2)+1] = it->second.c_str();
    }
    //return cXmlDoc(xsltApplyStylesheet(m_sheet.get(), doc, params), true);
    return cXmlDoc(xsltApplyStylesheetUser(m_sheet.get(), doc, params, NULL, NULL, ctx.get()), true);
}

string cXsltStylesheet::transformToString(cXmlDoc& doc) {
    if (!doc.ok())
        throw eXmlInvalid("Tried to transform bad document");

    return transformToString(doc.getRaw());
}

string cXsltStylesheet::transformToString(xmlDocPtr doc) {
    if (!doc)
        throw eXmlInvalid("Tried to transform bad document");
    if (!ok())
        throw eXmlInvalid("Tried to transform document with bad stylesheet");

    const char* params[(m_parameters.size()*2) + 1];
    params[m_parameters.size()*2] = NULL;
    int i = 0;
    for (map<string, string>::iterator it = m_parameters.begin(); it != m_parameters.end(); ++it) {
        params[(i*2)] = it->first.c_str();
        params[(i*2)+1] = it->second.c_str();
    }

    boost::shared_ptr<xmlDoc> tempdoc(xsltApplyStylesheet(m_sheet.get(), doc, params), xmlFreeDoc);
    if (!tempdoc)
        return "";

    xmlChar* tempstr;
    int tempstrlen;
    if (xsltSaveResultToString(&tempstr, &tempstrlen, tempdoc.get(), m_sheet.get()))
        return "";

    string ret = reinterpret_cast<char*>(tempstr);
    xmlFree(tempstr);

    return ret;
}

cXmlDoc cXsltStylesheet::getDoc() {
    if (!ok())
        throw eXmlInvalid("Tried to get document of bad stylesheet");

    if (m_doc.get())
        return cXmlDoc(m_doc);

    cXmlDoc ret;
    ret.read(m_sheet->doc);
    m_doc = ret.get();
    return ret;
}

#ifdef DUMPDEBUG
xmlDocPtr cXsltStylesheet::xsltDocLoaderFunc (const xmlChar* URI, xmlDictPtr dict, int options, void* ctxt, xsltLoadType type) {
fprintf(stderr, "cXsltStylesheet::xsltDocLoaderFunc() %d %s\n", type, URI);
fflush(stderr);

    xsltSetLoaderFunc(NULL);
    xmlDocPtr tmp = xsltDocDefaultLoader(URI, dict, options, ctxt, type);
    xsltSetLoaderFunc(cXsltStylesheet::xsltDocLoaderFunc);
    return tmp;
}
#endif


} // namespace xmlcpp

#endif

