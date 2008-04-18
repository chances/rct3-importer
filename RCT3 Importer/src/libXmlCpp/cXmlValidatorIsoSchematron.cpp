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


#include "cXmlValidatorIsoSchematron.h"

#ifdef XMLCPP_USE_ISO_SCHEMATRON

#include "cXmlException.h"
#include "cXmlInputOutputCallbackString.h"
#include "cXmlNode.h"
#include "cXmlXPath.h"
#include "cXmlXPathResult.h"

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
    m_schema = cXmlDoc();
}

bool cXmlValidatorIsoSchematron::read(cXmlDoc& doc, const std::map<std::string, std::string>& options) {
    Init();

    if (!doc.ok())
        throw eXml("Tried to read schematron schema from broken document");

    string sch = string(cXmlInputOutputCallbackString::PROTOCOL)+string(cXmlInputOutputCallbackString::INTERNAL)+"/schematron/iso_simple.xsl";
    cXmlDoc schemadoc(sch.c_str());

    m_schema = cXmlDoc(doc.getRaw(), false);

    cXsltStylesheet schemash(schemadoc);
    if (!schemash.ok())
        throw eXml("Internal error: could not load schematron xsl.");
    schemash.setParameters(options);
    cXmlDoc temp = schemash.transform(doc);

    if (!temp.ok()) {
        transferErrors(temp);
        return false;
    }

    m_transform = cXsltStylesheet(temp);

    if (!m_transform.ok()) {
        transferErrors(m_transform);
    }

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

    clearGenericErrors();
    clearStructuredErrors();

    cXmlXPath path;
    string ns;
    if (options & OPT_DETERMINE_NODE_BY_XPATH) {
        path.assign(doc);
        ns = cXmlDoc(doc).searchNs();
        if (ns != "")
            path.registerNs("utterridiculousxpathns", ns);
    }

    cXmlDoc result = m_transform.transform(doc.get());
    int errors = 0;

    cXmlNode root(result.root());
    if (!root.ok())
        return -1;

    cXmlNode child(root.children());
    string pattern = "Unnamed pattern";
    while (child.ok()) {
        if (child.name() == "pattern") {
            pattern = child.getPropVal("name", "Unnamed pattern");
        } else if ((child.name() == "failed-assert") || (child.name() == "successful-report")) {
            cXmlStructuredError error;
            error.domain = XML_FROM_SCHEMATRONV;
            error.code = (child.name() == "failed-assert")?XML_SCHEMATRONV_ASSERT:XML_SCHEMATRONV_REPORT;
            error.message = child.content();
            error.level = XML_ERR_ERROR;
            error.path = child.getPropVal("brieflocation", "");
            error.str1 = pattern;
            error.str2 = child.getPropVal("location", "");
            error.str3 = child.getPropVal("id", "");
            if (child.hasProp("role")) {
                string role = child.getPropVal("role", "0");
                if (role == "none") {
                    error.level = XML_ERR_NONE;
                    errors--; // Will be upped down below
                } else if (role == "warning") {
                    error.level = XML_ERR_WARNING;
                } else if (role == "fatal") {
                    error.level = XML_ERR_FATAL;
                } else {
                    sscanf(role.c_str(), "%d", &error.int1);
                }
            }

            if (path) {
                string elempath = (ns == "")?error.path:cXmlXPath::decorateWithNsPrefix(error.path, "utterridiculousxpathns", true);
                cXmlXPathResult res = path(elempath, true);
                if (res.size()) {
                    xmlNodePtr errnode = reinterpret_cast<xmlNodePtr>(res[0].getRaw());
                    if (errnode) {
                        if (errnode->type == XML_ATTRIBUTE_NODE) {
                            if (errnode->parent) {
                                error.line = errnode->parent->line;
                            }
                        } else {
                            error.line = errnode->line;
                        }
                    }
/*
                    error.node = res[0].getRaw();
                    if (error.node) {
                        xmlNodePtr errnode = reinterpret_cast<xmlNodePtr>(error.node);
                        if (errnode->type == XML_ATTRIBUTE_NODE) {
                            if (errnode->parent) {
                                error.line = errnode->parent->line;
                            }
                        } else {
                            error.line = errnode->line;
                        }
                    }
*/
                }
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

