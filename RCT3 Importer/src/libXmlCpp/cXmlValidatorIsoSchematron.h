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

#ifndef CXMLVALIDATORISOSCHEMATRON_H_INCLUDED
#define CXMLVALIDATORISOSCHEMATRON_H_INCLUDED

#include "cxmlconfig.h"

#ifdef XMLCPP_USE_ISO_SCHEMATRON

#include <map>
#include <string>

#include "cXmlDoc.h"
#include "cXmlValidator.h"
#include "cXsltStylesheet.h"

#define XML_FROM_SCHEMATRONV static_cast<xmlErrorDomain>(30)
#define XML_SCHEMATRONV_ASSERT 4000
#define XML_SCHEMATRONV_REPORT 4001

namespace xmlcpp {

class cXmlValidatorIsoSchematron: public cXmlValidator {
private:
    static bool g_resinit;
    cXsltStylesheet m_transform;
    cXmlDoc m_schema;

    void Init();
    //void DeInit();
    //void Parse(int options);
public:
    cXmlValidatorIsoSchematron():cXmlValidator() { Init(); }
    cXmlValidatorIsoSchematron(cXmlDoc& doc, const std::map<std::string, std::string>& options = (std::map<std::string, std::string>()) ):cXmlValidator() {
        Init();
        read(doc, options);
    }
    cXmlValidatorIsoSchematron(const char* URL, const std::map<std::string, std::string>& options = (std::map<std::string, std::string>()) ):cXmlValidator() {
        Init();
        read(URL, options);
    }
    virtual ~cXmlValidatorIsoSchematron();

    bool read(cXmlDoc& doc, const std::map<std::string, std::string>& options = (std::map<std::string, std::string>()) );
    bool read(const std::string& buffer, const std::map<std::string, std::string>& options = (std::map<std::string, std::string>()) );
    bool read(const char* URL, const std::map<std::string, std::string>& options = (std::map<std::string, std::string>()) );

    inline cXmlDoc& schema() { return m_schema; };
    inline cXsltStylesheet& transform() { return m_transform; };

    virtual cXmlValidatorResult validate(boost::shared_ptr<xmlDoc>& doc, cXmlValidatorResult::LEVEL retlevel = cXmlValidatorResult::VR_ERROR, int options = OPT_NONE);

    virtual bool ok() const { return m_transform.ok(); }
    inline bool operator!() const { return !ok(); }
    virtual int getType() const { return VAL_ISOSCHEMATRON; }
    typedef cXsltStylesheet cXmlValidatorIsoSchematron::*unspecified_bool_type;
    inline operator unspecified_bool_type() const { return ok()?(&cXmlValidatorIsoSchematron::m_transform):NULL; }
};

} // Namespace

#endif

#endif // CXMLVALIDATORISOSCHEMATRON_H_INCLUDED
