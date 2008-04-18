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

#include "cxmlconfig.h"

#include <boost/shared_array.hpp>
#include <map>
#include <string>

#include "cXmlDoc.h"
#include "cXmlInlinedSchematronValidator.h"
#include "cXsltStylesheet.h"

namespace xmlcpp {

struct RNVValidationContext;
class cXmlValidatorRNVRelaxNG: public cXmlInlinedSchematronValidator {
friend void do_node(cXmlNode& node, RNVValidationContext& ctx);
public:
    enum {
        ERRSTAGE_UNKNOWN =          0,      ///< Unknown stage, probably before parsing began or internal
        ERRSTAGE_RNC,                       ///< RNC lexing error
        ERRSTAGE_RNG_PARSE,                 ///< XML parsing error in rng file
        ERRSTAGE_RNG_VALIDATION,            ///< RNG does not validate against relaxng.rng
        ERRSTAGE_RNG_INCELIM,               ///< Error during incelim transform
        ERRSTAGE_RNG_CONVERSION,            ///< Error during conversion to rnc
        ERRSTAGE_RNG_SHORTRNG,              ///< Error during conversion of ShortRNG to rng
        ERRSTAGE_RNG_EXAMPLOTRON,           ///< Error during conversion of examplotron to rng
        ERRSTAGE_OK =             100
    };
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
    int m_type;
    int m_errstage;

    void Init();
    void claimRNV();
    void releaseRNV();
    //void DeInit();
    //void Parse(int options);
    bool ParseRNC(const std::string& buffer, const char* URL = NULL);
public:
    cXmlValidatorRNVRelaxNG():cXmlInlinedSchematronValidator() { Init(); }
    cXmlValidatorRNVRelaxNG(const char* URL):cXmlInlinedSchematronValidator() {
        Init();
        read(URL);
    }
    virtual ~cXmlValidatorRNVRelaxNG();

    bool readRNC(const std::string& buffer, const char* URL = NULL);
    bool readRNC(const char* URL);

    std::string rnc() { return m_rncbuffer?m_rncbuffer.get():""; }



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

    virtual int getType() const {
        return m_type;
    }
// Slim stuff at end
private:
#ifdef XMLCPP_USE_RNV_RNG
    cXmlDoc m_rng;

    bool ParseRNG(cXmlDoc& schema, const char* URL = NULL);
#ifdef XMLCPP_USE_RNV_SHORTRNG
    bool ParseShortRNG(cXmlDoc& schema, const char* URL = NULL);
#endif
#ifdef XMLCPP_USE_RNV_EXAMPLOTRON
    bool ParseExamplotron(cXmlDoc& schema, const char* URL = NULL);
#endif
    bool parse(cXmlDoc& schema, const std::string& buffer, const char* URL);
#endif
public:
#ifdef XMLCPP_USE_RNV_RNG
    cXmlValidatorRNVRelaxNG(cXmlDoc& schema, const char* URL = NULL):cXmlInlinedSchematronValidator() {
        Init();
        read(schema, URL);
    }

    bool read(cXmlDoc& schema, const char* URL = NULL) {
        return parse(schema, "", URL);
    }
    bool read(const std::string& buffer, const char* URL = NULL);
    bool read(const char* URL);

    // rng reading will go here
    bool readRNG(cXmlDoc& schema, const char* URL = NULL);
    bool readRNG(const std::string& buffer, const char* URL = NULL);
    bool readRNG(const char* URL);

    inline cXmlDoc& rng() {return m_rng;}
#else
    inline bool read(const std::string& buffer, const char* URL = NULL) { return readRNC(buffer, URL); }
    inline bool read(const char* URL) { return readRNC(URL); }
#endif

#ifdef XMLCPP_USE_INLINED_SCHEMATRON
#ifdef XMLCPP_USE_RNV_RNG
    virtual cXmlDoc schematron();
#else
    virtual cXmlDoc schematron() {return cXmlDoc();}
#endif
#endif

};

} // Namespace


#endif // CXMLVALIDATORISOSCHEMATRON_H_INCLUDED
