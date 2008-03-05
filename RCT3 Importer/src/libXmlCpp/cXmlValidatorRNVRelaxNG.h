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

#ifndef CXMLVALIDATORRNVRELAXNG_H_INCLUDED
#define CXMLVALIDATORRNVRELAXNG_H_INCLUDED

#include <boost/shared_array.hpp>
#include <map>
#include <string>

#include "cXmlValidator.h"
#include "cXsltStylesheet.h"

namespace xmlcpp {

struct RNVValidationContext;
class cXmlValidatorRNVRelaxNG: public cXmlValidator {
friend void do_node(cXmlNode& node, RNVValidationContext& ctx);
private:
    static bool g_resinit;
    static cXmlValidatorRNVRelaxNG* g_rnvclaimed;
    static xmlNodePtr g_nodecontext;
    static void verror_handler_rnl(int erno,va_list ap);
    static void verror_handler_rnv(int erno,va_list ap);
    static void verror_handler(int erno,va_list ap);

    int m_rncstatus;
    std::string m_rncfile;
    boost::shared_array<char> m_rncbuffer;
    int m_rncbuffersize;

    void Init();
    void claimRNV();
    void releaseRNV();
    //void DeInit();
    //void Parse(int options);
public:
    cXmlValidatorRNVRelaxNG():cXmlValidator() { Init(); }
    virtual ~cXmlValidatorRNVRelaxNG();

    bool read(const std::string& buffer);
    bool read(const char* URL);
#ifdef XMLCPP_USE_XSLT
    // rng reading will go here
#endif

    /// Reparse the RNC from memory
    /**
     * For performance evaluation
     */
    void reparse();

    virtual int validate(boost::shared_ptr<xmlDoc>& doc, int options = OPT_NONE);

    virtual bool ok() const { return m_rncstatus; }
    inline bool operator!() const { return !ok(); }
    typedef int cXmlValidatorRNVRelaxNG::*unspecified_bool_type;
    inline operator unspecified_bool_type() const { return ok()?(&cXmlValidatorRNVRelaxNG::m_rncstatus):NULL; }
};

} // Namespace


#endif // CXMLVALIDATORISOSCHEMATRON_H_INCLUDED
