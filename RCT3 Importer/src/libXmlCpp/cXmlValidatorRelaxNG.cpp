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


#include "cXmlValidatorRelaxNG.h"

#include "cXmlDoc.h"
#include "cXmlException.h"

using namespace std;

namespace xmlcpp {

cXmlValidatorRelaxNG::~cXmlValidatorRelaxNG() {
    DeInit();
    xmlRelaxNGCleanupTypes();
}

void cXmlValidatorRelaxNG::Init() {
    m_context = NULL;
    m_parser = NULL;
    m_schema = NULL;
}

void cXmlValidatorRelaxNG::DeInit() {
    if (m_schema) {
        xmlRelaxNGFree(m_schema);
        m_schema = NULL;
    }
    if (m_parser) {
        xmlRelaxNGFreeParserCtxt(m_parser);
        m_parser = NULL;
    }
    if (m_context) {
        xmlRelaxNGFreeValidCtxt ( m_context );
        m_context = NULL;
    }
}

void cXmlValidatorRelaxNG::Parse() {
    if (!m_parser)
        throw eXml("Context missing in cXmlValidatorRelaxNG::Parse()");

    //xmlRelaxNGSetParserErrors(m_parser, relaxngerrorwrapper, relaxngwarningwrapper, this);
    xmlRelaxNGSetParserStructuredErrors(m_parser, structurederrorwrap, this);

    m_schema = xmlRelaxNGParse ( m_parser );
    if (m_schema)
        m_context = xmlRelaxNGNewValidCtxt ( m_schema );
    if (m_context) {
        //xmlRelaxNGSetValidErrors(m_context, relaxngerrorwrapper, relaxngwarningwrapper, this);
        xmlRelaxNGSetValidStructuredErrors(m_context, structurederrorwrap, this);
    }
}

bool cXmlValidatorRelaxNG::read(cXmlDoc& doc) {
    DeInit();
    m_parser = xmlRelaxNGNewDocParserCtxt(doc.getRaw());
    if (!m_parser)
        return false;
    Parse();
    return ok();
}

bool cXmlValidatorRelaxNG::read(const string& buffer) {
    DeInit();
    m_parser = xmlRelaxNGNewMemParserCtxt(buffer.c_str(), buffer.size());
    if (!m_parser)
        return false;
    Parse();
    return ok();
}


bool cXmlValidatorRelaxNG::read(const char* URL) {
    DeInit();
    m_parser = xmlRelaxNGNewParserCtxt(URL);
    if (!m_parser)
        return false;
    Parse();
    return ok();
}

int cXmlValidatorRelaxNG::validate(boost::shared_ptr<xmlDoc>& doc, int options) {
    if (!ok())
        throw eXml("Tried to validate with invalid relaxng schema");
    return xmlRelaxNGValidateDoc(m_context, doc.get());
}



} // Namespace

