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

#include "cXmlValidatorMulti.h"

#include "cXmlDoc.h"
#include "cXmlException.h"
#include "cXmlNode.h"
#include "cXmlValidatorIsoSchematron.h"
#include "cXmlValidatorRNVRelaxNG.h"
#include "cXmlValidatorSchematron.h"
#include "cxmlnamespaceconstants.h"

using namespace std;

namespace xmlcpp {

cXmlValidatorMulti::cXmlValidatorMulti() {
    Init(true);
}

cXmlValidatorMulti::~cXmlValidatorMulti()
{
    //dtor
}

void cXmlValidatorMulti::Init(bool construct) {
    if (construct) {
        m_runSecondary = false;
    }

    m_primary.reset();
    m_secondary.reset();
    clearGenericErrors();
    clearStructuredErrors();
};

bool cXmlValidatorMulti::read(cXmlDoc& doc, const char* URL) {
    return read(m_primary, doc, "", URL);
}
bool cXmlValidatorMulti::read(const std::string& buffer, const char* URL) {
    cXmlDoc schema(buffer, URL, NULL, XML_PARSE_DTDLOAD);
    return read(m_primary, schema, buffer, URL);
}
bool cXmlValidatorMulti::read(const char* URL) {
    cXmlDoc schema(URL, NULL, XML_PARSE_DTDLOAD);
    return read(m_primary, schema, "", URL);
}

bool cXmlValidatorMulti::readSecondary(cXmlDoc& doc, const char* URL) {
    return read(m_secondary, doc, "", URL, false);
}
bool cXmlValidatorMulti::readSecondary(const std::string& buffer, const char* URL) {
    cXmlDoc schema(buffer, URL, NULL, XML_PARSE_DTDLOAD);
    return read(m_secondary, schema, buffer, URL, false);
}
bool cXmlValidatorMulti::readSecondary(const char* URL) {
    cXmlDoc schema(URL, NULL, XML_PARSE_DTDLOAD);
    return read(m_secondary, schema, "", URL, false);
}

bool cXmlValidatorMulti::read(boost::shared_ptr<cXmlValidator>& into, cXmlDoc& schema, const string& buffer, const char* URL, bool parse_sec) {
    Init();

    if (!schema) {
        // Either RNC or nothing.
        if ((schema.getStructuredErrors()[0].domain == XML_FROM_PARSER) &&
            (schema.getStructuredErrors()[0].code == XML_ERR_DOCUMENT_EMPTY)) {
            // Not XML, try to load as RNC
            into.reset(new cXmlValidatorRNVRelaxNG());
            if (buffer == "") {
                reinterpret_cast<cXmlValidatorRNVRelaxNG*>(into.get())->readRNC(URL);
            } else {
                reinterpret_cast<cXmlValidatorRNVRelaxNG*>(into.get())->readRNC(buffer, URL);
            }
        } else {
            // Broken XML
            transferErrors(schema);
            if (!hasErrors())
                addGenericError("Internal error: XML broken, but no errors reported!");
            return false;
        }
    } else {
        string rootns = schema.root().ns();
        if (false) {
#ifdef XMLCPP_USE_SCHEMATRON_PATCHED_LIBXML
        } else if (rootns == NAMESPACE_SCHEMATRON) {
            into.reset(new cXmlValidatorSchematron(schema));
#endif
#ifdef XMLCPP_USE_RNV_RNG
        } else if (rootns == NAMESPACE_RELAXNG) {
            into.reset(new cXmlValidatorRNVRelaxNG(schema, URL));
#ifdef XMLCPP_USE_RNV_SHORTRNG
        } else if (rootns == NAMESPACE_SHORTRNG) {
            into.reset(new cXmlValidatorRNVRelaxNG(schema, URL));
#endif //XMLCPP_USE_RNV_SHORTRNG
#endif // XMLCPP_USE_RNV_RNG
#ifdef XMLCPP_USE_ISO_SCHEMATRON
        } else if (rootns == NAMESPACE_ISOSCHEMATRON) {
            into.reset(new cXmlValidatorIsoSchematron(schema, m_schematronoptions));
#endif //XMLCPP_USE_ISO_SCHEMATRON
#ifdef XMLCPP_USE_RNV_EXAMPLOTRON
        } else {
            if (schema.searchNsByHref(NAMESPACE_EXAMPLOTRON)) {
                into.reset(new cXmlValidatorRNVRelaxNG(schema, URL));
            }
#endif //XMLCPP_USE_RNV_EXAMPLOTRON
        }
    }

    if (!into) {
        addGenericError("Passed unsupported schema type to multi validator.");
        return false;
    }

    if (!into->ok()) {
        transferErrors(*into.get());
        if (!hasErrors())
            addGenericError("Internal error: First validator broken, but no errors reported! Type: " + cXmlValidator::getTypeName(into->getType()));
        return false;
    }

#ifdef XMLCPP_USE_INLINED_SCHEMATRON
    if (parse_sec) {
        cXmlInlinedSchematronValidator* val = dynamic_cast<cXmlInlinedSchematronValidator*>(into.get());
        if (val) {
            cXmlDoc schematron = val->schematron();
            if (schematron) {
                // RNG2Schtrn always creates an empty diagnostics element -.-
                if (schematron.root().name() == "schema") {
                    if (schematron.root().children()) {
                        if ((schematron.root().children().name() != "diagnostics") || (schematron.root().children().children())) {
                            m_secondary.reset(new cXmlValidatorIsoSchematron(schematron, m_schematronoptions));
                        }
                    }
                }
            }
        }
    }
#endif
    return ok();
}

int cXmlValidatorMulti::getType() const {
    int type = VAL_MULTI;
    if (m_primary) {
        type |= m_primary->getType();
    }
    if (m_secondary) {
        type |= VAL_PLUS_SCHEMATRON;
    }
    return type;
}


cXmlValidatorResult cXmlValidatorMulti::validate(boost::shared_ptr<xmlDoc>& doc, cXmlValidatorResult::LEVEL retlevel, int options) {
    if (!ok())
        throw eXml("Tried to validate with invalid multi validator");
    if (!doc)
        throw eXml("Tried to validate broken document");

    //cXmlValidatorResult res(retlevel);

    clearGenericErrors();
    clearStructuredErrors();

    //int ret = MULTI_NOERROR;

    cXmlValidatorResult first = m_primary->validate(doc, retlevel, options);
    if (!first) {
        if (m_secondary) {
            cXmlValidatorResult sec = m_secondary->validate(doc, retlevel, options);
            if (sec) {
                first.multi = MULTI_SECONDARY;
                first.merge(sec);
            }
        }
    } else {
        first.multi = MULTI_PRIMARY;
        if (m_secondary) {
            m_secondary->clearGenericErrors();
            m_secondary->clearStructuredErrors();
            if (m_runSecondary) {
                cXmlValidatorResult sec = m_secondary->validate(doc, retlevel, options);
                if (sec) {
                    first.multi = MULTI_BOTH;
                    first.merge(sec);
                }
            }
        }
    }
    transferErrors(*m_primary.get());
    if (m_secondary) {
        transferErrors(*m_secondary.get());
    }
    return first;
}


} // namespace xmlcpp
