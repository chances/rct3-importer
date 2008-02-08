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


#ifdef XMLCPP_USE_XSLT

#include "cXmlValidatorIsoSchematron.h"

#include "cXmlDoc.h"
#include "cXmlException.h"
#include "cXmlInputOutputCallbackString.h"
#include "cXmlNode.h"

#include "xsl/iso_simple.xsl.h"
#include "xsl/iso_schematron_skeleton.xsl.h"

using namespace std;

namespace xmlcpp {

bool cXmlValidatorIsoSchematron::g_resinit = false;

cXmlValidatorIsoSchematron::~cXmlValidatorIsoSchematron() {
    Init();
}

void cXmlValidatorIsoSchematron::Init() {
    if (!g_resinit) {
        cXmlInputOutputCallbackString::Init();
        cXmlInputOutputCallbackString::add(string(cXmlInputOutputCallbackString::INTERNAL)+"/schematron/iso_schematron_skeleton.xsl", reinterpret_cast<const char*>(iso_schematron_skeleton_xsl_h));
        cXmlInputOutputCallbackString::add(string(cXmlInputOutputCallbackString::INTERNAL)+"/schematron/iso_simple.xsl", reinterpret_cast<const char*>(iso_simple_xsl_h));
        g_resinit = true;
    }
    m_transform = cXsltStylesheet();
}

//void cXmlValidatorSchematron::DeInit() {
//    if (m_schema) {
//        xmlSchematronFree(m_schema);
//        m_schema = NULL;
//    }
//    if (m_parser) {
//        xmlSchematronFreeParserCtxt(m_parser);
//        m_parser = NULL;
//    }
//    if (m_context) {
//        xmlSchematronFreeValidCtxt ( m_context );
//        m_context = NULL;
//    }
//}

//void cXmlValidatorSchematron::Parse(int options) {
//    if (!m_parser)
//        throw eXml("Context missing in cXmlValidatorSchematron::Parse()");
//
//    claimErrors();
//    m_schema = xmlSchematronParse ( m_parser );
//    if (m_schema)
//        m_context = xmlSchematronNewValidCtxt ( m_schema, options );
//    if (m_context) {
//        xmlSchematronSetValidStructuredErrors(m_context, structurederrorwrap, this);
//    }
//    releaseErrors();
//}

bool cXmlValidatorIsoSchematron::read(cXmlDoc& doc, const std::map<std::string, std::string>& options) {
    Init();

    if (!doc.ok())
        throw eXml("Tried to read schematron schema from broken document");

    string sch = string(cXmlInputOutputCallbackString::PROTOCOL)+string(cXmlInputOutputCallbackString::INTERNAL)+"/schematron/iso_simple.xsl";
    cXmlDoc schemadoc(sch.c_str());

    cXsltStylesheet schema(schemadoc);
    if (!schema.ok())
        throw eXml("Internal error: could not load schematron xsl.");
    schema.setParameters(options);
    cXmlDoc temp = schema.transform(doc);

    if (!temp.ok()) {
        transferErrors(temp);
        return false;
    }

    m_transform = cXsltStylesheet(temp);

    if (!m_transform.ok()) {
        transferErrors(m_transform);
    }


//    DeInit();
//    claimErrors();
//    m_parser = xmlSchematronNewMemParserCtxt(buffer.c_str(), buffer.size());
//    releaseErrors();
//    if (!m_parser)
//        return false;
//    Parse(options);
//    return valid();
    return ok();
}


bool cXmlValidatorIsoSchematron::read(const std::string& buffer, const std::map<std::string, std::string>& options) {
    cXmlDoc temp(buffer);

    if (!temp.ok()) {
        transferErrors(temp);
        return false;
    }
    temp.xInclude();

    return read(temp, options);
}

bool cXmlValidatorIsoSchematron::read(const char* URL, const std::map<std::string, std::string>& options) {
    cXmlDoc temp(URL);

    if (!temp.ok()) {
        transferErrors(temp);
        return false;
    }
    temp.xInclude();

    return read(temp, options);
}

int cXmlValidatorIsoSchematron::validate(boost::shared_ptr<xmlDoc>& doc, int options) {
    if (!ok())
        throw eXml("Tried to validate with invalid schematron schema");
    if (!doc)
        throw eXml("Tried to validate broken document");

    cXmlDoc result = m_transform.transform(doc.get());
    int errors = 0;

    cXmlNode root(result.getRoot());
    if (!root.ok())
        return -1;

    cXmlNode child(root.children());
    string pattern = "Unnamed pattern";
    while (child.ok()) {
        if (child.name() == "pattern") {
            pattern = child.getProp("name", "Unnamed pattern");
        } else if ((child.name() == "failed-assert") || (child.name() == "successful-report")) {
            cXmlStructuredError error;
            error.domain = XML_FROM_SCHEMATRONV;
            error.code = (child.name() == "failed-assert")?XML_SCHEMATRONV_ASSERT:XML_SCHEMATRONV_REPORT;
            error.message = child.content();
            error.level = XML_ERR_ERROR;
            error.path = child.getProp("brieflocation", "");
            error.str1 = pattern;
            error.str2 = child.getProp("location", "");
            error.str3 = child.getProp("id", "");
            if (child.hasProp("role")) {
                sscanf(child.getProp("role", "0").c_str(), "%d", &error.int1);
            }
            addStructuredError(error);
            errors++;
        }
        child.go_next();
    }
    return errors;
}



} // Namespace

#endif

