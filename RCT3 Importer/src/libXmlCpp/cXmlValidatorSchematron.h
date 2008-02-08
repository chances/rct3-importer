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

#ifndef CXMLVALIDATORSCHEMATRON_H_INCLUDED
#define CXMLVALIDATORSCHEMATRON_H_INCLUDED

#include "cXmlValidator.h"

#include <libxml/schematron.h>

namespace xmlcpp {

class cXmlValidatorSchematron: public cXmlValidator {
private:
    xmlSchematronValidCtxtPtr m_context;
    xmlSchematronParserCtxtPtr m_parser;
    xmlSchematronPtr m_schema;

    void Init();
    void DeInit();
    void Parse(int options);
public:
    cXmlValidatorSchematron():cXmlValidator() { Init(); }
    virtual ~cXmlValidatorSchematron();

    bool read(cXmlDoc& doc, int options = XML_SCHEMATRON_OUT_ERROR);
    bool read(const std::string& buffer, int options = XML_SCHEMATRON_OUT_ERROR);
    bool read(const char* URL, int options = XML_SCHEMATRON_OUT_ERROR);

    virtual int validate(boost::shared_ptr<xmlDoc>& doc, int options = OPT_NONE);

    virtual bool ok() const { return m_context && m_parser && m_schema; }
};

} // Namespace

#endif // CXMLVALIDATORRLEAXNG_H_INCLUDED
