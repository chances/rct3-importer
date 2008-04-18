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

#ifndef XMLCPP_CXSLTSTYLESHEET_H
#define XMLCPP_CXSLTSTYLESHEET_H

#include "cxmlconfig.h"

#ifdef XMLCPP_USE_XSLT

#include <map>
#include <string>
#include <libxml/parser.h>
#include <libxslt/xsltInternals.h>
#include <boost/shared_ptr.hpp>

#include "cXmlClasses.h"
#include "cXmlInitHandler.h"
#include "cXmlErrorHandler.h"

namespace xmlcpp {

class cXsltStylesheet: public cXmlInitHandler, public cXmlErrorHandler {
public:
    cXsltStylesheet();
    cXsltStylesheet(const char* filename);
    cXsltStylesheet(cXmlDoc& doc);
    virtual ~cXsltStylesheet();

    bool read(cXmlDoc& doc);
    bool read(const char* filename);

    void addParameter(const std::string& name, const std::string& value);
    void setParameters(const std::map<std::string, std::string>& parameters);

    cXmlDoc transform(cXmlDoc& doc);
    cXmlDoc transform(xmlDocPtr doc);
    std::string transformToString(cXmlDoc& doc);
    std::string transformToString(xmlDocPtr doc);

    //xmlDocPtr getDoc();
    cXmlDoc getDoc();

    inline bool ok() const { return m_sheet.get(); }
    inline bool operator!() const { return !ok(); }
    typedef boost::shared_ptr<xsltStylesheet> cXsltStylesheet::*unspecified_bool_type;
    inline operator unspecified_bool_type() const { return ok()?(&cXsltStylesheet::m_sheet):NULL; }
protected:
private:
    boost::shared_ptr<xsltStylesheet> m_sheet;
    boost::shared_ptr<xmlDoc> m_doc;
    std::map<std::string, std::string> m_parameters;

    void Init();
    void assignSheet(xsltStylesheetPtr newsheet);
};

} // namespace xmlcpp

#endif

#endif // XMLCPP_CXSLTSTYLESHEET_H
