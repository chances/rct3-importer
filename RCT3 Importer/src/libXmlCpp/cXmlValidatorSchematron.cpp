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

#include "cXmlValidatorSchematron.h"

#ifdef XMLCPP_USE_SCHEMATRON_PATCHED_LIBXML

#include "cXmlDoc.h"
#include "cXmlException.h"

using namespace std;

namespace xmlcpp {

cXmlValidatorSchematron::~cXmlValidatorSchematron() {
    DeInit();
}

void cXmlValidatorSchematron::Init() {
    m_context = NULL;
    m_parser = NULL;
    m_schema = NULL;
}

void cXmlValidatorSchematron::DeInit() {
    if (m_schema) {
        xmlSchematronFree(m_schema);
        m_schema = NULL;
    }
    if (m_parser) {
        xmlSchematronFreeParserCtxt(m_parser);
        m_parser = NULL;
    }
    if (m_context) {
        xmlSchematronFreeValidCtxt ( m_context );
        m_context = NULL;
    }
}

void cXmlValidatorSchematron::Parse(int options) {
    if (!m_parser)
        throw eXml("Context missing in cXmlValidatorSchematron::Parse()");

    claimErrors();
    m_schema = xmlSchematronParse ( m_parser );
    if (m_schema)
        m_context = xmlSchematronNewValidCtxt ( m_schema, options );
    if (m_context) {
        xmlSchematronSetValidStructuredErrors(m_context, structurederrorwrap, this);
    }
    releaseErrors();
}

bool cXmlValidatorSchematron::read(cXmlDoc& doc, int options) {
    DeInit();
    claimErrors();
    m_parser = xmlSchematronNewDocParserCtxt(doc.getRaw());
    releaseErrors();
    if (!m_parser)
        return false;
    Parse(options);
    return ok();
}

bool cXmlValidatorSchematron::read(const string& buffer, int options) {
    DeInit();
    claimErrors();
    m_parser = xmlSchematronNewMemParserCtxt(buffer.c_str(), buffer.size());
    releaseErrors();
    if (!m_parser)
        return false;
    Parse(options);
    return ok();
}


bool cXmlValidatorSchematron::read(const char* URL, int options) {
    DeInit();
    claimErrors();
    m_parser = xmlSchematronNewParserCtxt(URL);
    releaseErrors();
    if (!m_parser)
        return false;
    Parse(options);
    return ok();
}

cXmlValidatorResult cXmlValidatorSchematron::validate(boost::shared_ptr<xmlDoc>& doc, cXmlValidatorResult::LEVEL retlevel, int options) {
    if (!ok())
        throw eXml("Tried to validate with invalid schematron schema");

    cXmlValidatorResult res(retlevel);

    clearGenericErrors();
    clearStructuredErrors();

    int valok = xmlSchematronValidateDoc(m_context, doc.get());
    res.error = getStructuredErrors().size() + getGenericErrors().size();
    if ((!res.error) && valok) {
        // Something went wrong, but no error was reported?!?
        res.internal_error = true;
    }

    return res;
}



} // Namespace

#endif
