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

#ifndef XMLCPP_CXMLVALIDATORMULTI_H
#define XMLCPP_CXMLVALIDATORMULTI_H

#include "cxmlconfig.h"

#include <boost/shared_ptr.hpp>
#include <map>

#include "cXmlValidator.h"

namespace xmlcpp {

class cXmlValidatorMulti : public cXmlValidator {
public:
    enum {
        MULTI_NOERROR = 0,
        MULTI_PRIMARY = 1,
        MULTI_SECONDARY = 2,
        MULTI_BOTH = 3
    };

    cXmlValidatorMulti();
    virtual ~cXmlValidatorMulti();

#ifdef XMLCPP_USE_ISO_SCHEMATRON
    void setSchematronOptions(const std::map<std::string, std::string>& options = (std::map<std::string, std::string>()) ) {
        m_schematronoptions = options;
    }
#endif

    bool read(cXmlDoc& doc, const char* URL = NULL);
    bool read(const std::string& buffer, const char* URL = NULL);
    bool read(const char* URL);
    bool readSecondary(cXmlDoc& doc, const char* URL = NULL);
    bool readSecondary(const std::string& buffer, const char* URL = NULL);
    bool readSecondary(const char* URL);

    boost::shared_ptr<cXmlValidator>& primary() { return m_primary; }
    void primary(const boost::shared_ptr<cXmlValidator>& val) {
        m_primary = val;
    }
    boost::shared_ptr<cXmlValidator>& secondary() { return m_secondary; }
    void secondary(const boost::shared_ptr<cXmlValidator>& val) {
        m_secondary = val;
    }

    virtual cXmlValidatorResult validate(boost::shared_ptr<xmlDoc>& doc, cXmlValidatorResult::LEVEL retlevel = cXmlValidatorResult::VR_ERROR, int options = OPT_NONE);
    virtual bool ok() const {
        if (!m_primary)
            return false;
        if (!m_primary->ok())
            return false;
        if (!m_secondary)
            return true;
        return m_secondary->ok();
    }
    virtual int getType() const;

    inline bool operator!() const { return !ok(); }
    typedef boost::shared_ptr<cXmlValidator> cXmlValidatorMulti::*unspecified_bool_type;
    inline operator unspecified_bool_type() const { return ok()?(&cXmlValidatorMulti::m_primary):NULL; }
protected:
private:
    boost::shared_ptr<cXmlValidator> m_primary;
    boost::shared_ptr<cXmlValidator> m_secondary;
#ifdef XMLCPP_USE_ISO_SCHEMATRON
    std::map<std::string, std::string> m_schematronoptions;
#endif
    bool m_runSecondary;

    void Init(bool construct = false);
    bool read(boost::shared_ptr<cXmlValidator>& into, cXmlDoc& schema, const std::string& buffer, const char* URL = NULL, bool parse_sec = true);
};

} // namespace xmlcpp

#endif // XMLCPP_CXMLVALIDATORMULTI_H
