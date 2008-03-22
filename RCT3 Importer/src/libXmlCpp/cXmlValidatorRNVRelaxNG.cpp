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


#include "cXmlValidatorRNVRelaxNG.h"

#include "cXmlException.h"
#include "cXmlInputOutputCallbackString.h"
#include "cXmlNode.h"
#include "cxmlnamespaceconstants.h"

#include "rnv/erbit.h"
#include "rnv/drv.h"
#include "rnv/dsl.h"
#include "rnv/dxl.h"
#include "rnv/rn.h"
#include "rnv/rnc.h"
#include "rnv/rnd.h"
#include "rnv/rnl.h"
#include "rnv/rnv.h"
#include "rnv/rnx.h"

#include "rng/relaxng.rnc.gz.h"
#include "xsl/rng-incelim-1.2/incelim.xsl.gz.h"
#include "xsl/rng-incelim-1.2/inc.xsl.gz.h"
#include "xsl/rng-incelim-1.2/elim.xsl.gz.h"
#include "xsl/rng-incelim-1.2/strip.xsl.gz.h"
#include "xsl/rng-incelim-1.2/clean.xsl.gz.h"
#include "xsl/RngToRnc-1_4/RngToRncClassic.xsl.gz.h"

#include <zlib.h>


//#define DUMP

using namespace std;

namespace xmlcpp {

bool cXmlValidatorRNVRelaxNG::g_resinit = false;
cXmlValidatorRNVRelaxNG* cXmlValidatorRNVRelaxNG::g_rnvclaimed = NULL;
xmlNodePtr cXmlValidatorRNVRelaxNG::g_nodecontext = NULL;

void cXmlValidatorRNVRelaxNG::verror_handler_rnl(int erno,va_list ap) {
    verror_handler(erno|ERBIT_RNL,ap);
}

void cXmlValidatorRNVRelaxNG::verror_handler_rnv(int erno,va_list ap) {
    verror_handler(erno|ERBIT_RNV,ap);
}

void cXmlValidatorRNVRelaxNG::verror_handler(int erno,va_list ap) {
    if (g_rnvclaimed) {
        cXmlStructuredError error;
        error.domain = (erno&ERBIT_RNL)?XML_FROM_RELAXNGP:XML_FROM_RELAXNGV;
        int runerrno = erno&~(ERBIT_RNL|ERBIT_RNV);
        error.code = runerrno;
        error.level = XML_ERR_ERROR;

        // Helpers
        char tempbuf[256];
        int tempint;
        char* tempstr;

        if (erno&ERBIT_RNL) {
            // Lexing error
            if (runerrno & ERBIT_RNC) {
                runerrno = runerrno&~ERBIT_RNC;
                tempstr = va_arg(ap, char*);
                if (tempstr)
                    error.file = tempstr;
                error.line = va_arg(ap, int);
                error.int2 = va_arg(ap, int);
                switch (runerrno) {
                case RNC_ER_IO:
                    error.message = "I/O error: ";
                    tempstr = va_arg(ap, char*);
                    if (tempstr) {
                        error.message += tempstr;
                        error.str1 = tempstr;
                    } else
                        error.message += "?";
                    break;
                case RNC_ER_UTF:
                    error.message = "invalid UTF-8 sequence";
                    break;
                case RNC_ER_XESC:
                    error.message = "unterminated escape";
                    break;
                case RNC_ER_LEXP:
                    error.message = "lexical error: '";
                    tempint = va_arg(ap, int);
                    error.message += static_cast<char>(tempint);
                    error.str1 = static_cast<char>(tempint);
                    error.message += "' expected";
                    break;
                case RNC_ER_LLIT:
                    error.message = "lexical error: unterminated literal";
                    break;
                case RNC_ER_LILL:
                    tempint = va_arg(ap, int);
                    snprintf(tempbuf, 255, "%x", tempint);
                    error.message = "lexical error: illegal character \\x{";
                    error.message += tempbuf;
                    error.str1 = tempbuf;
                    error.int1 = tempint;
                    error.message += "}";
                    break;
                case RNC_ER_SEXP:
                    error.message = "syntax error: ";
                    tempstr = va_arg(ap, char*);
                    if (tempstr) {
                        error.message += tempstr;
                        error.str1 = tempstr;
                    } else
                        error.message += "?";
                    error.message += " expected, ";
                    tempstr = va_arg(ap, char*);
                    if (tempstr) {
                        error.message += tempstr;
                        error.str2 = tempstr;
                    } else
                        error.message += "?";
                    error.message += " found";
                    break;
                case RNC_ER_SILL:
                    error.message = "syntax error: ";
                    tempstr = va_arg(ap, char*);
                    if (tempstr) {
                        error.message += tempstr;
                        error.str1 = tempstr;
                    } else
                        error.message += "?";
                    error.message += "unexpected";
                    break;
                case RNC_ER_NOTGR:
                    error.message = "included schema is not a grammar";
                    break;
                case RNC_ER_EXT:
                    error.message = "cannot open external grammar '";
                    tempstr = va_arg(ap, char*);
                    if (tempstr) {
                        error.message += tempstr;
                        error.str1 = tempstr;
                    } else
                        error.message += "?";
                    error.message += "'";
                    break;
                case RNC_ER_DUPNS:
                    error.message = "duplicate namespace prefix '";
                    tempstr = va_arg(ap, char*);
                    if (tempstr) {
                        error.message += tempstr;
                        error.str1 = tempstr;
                    } else
                        error.message += "?";
                    error.message += "'";
                    break;
                case RNC_ER_DUPDT:
                    error.message = "duplicate datatype prefix '";
                    tempstr = va_arg(ap, char*);
                    if (tempstr) {
                        error.message += tempstr;
                        error.str1 = tempstr;
                    } else
                        error.message += "?";
                    error.message += "'";
                    break;
                case RNC_ER_DFLTNS:
                    error.level = XML_ERR_WARNING;
                    error.message = "overriding default namespace prefix '";
                    tempstr = va_arg(ap, char*);
                    if (tempstr) {
                        error.message += tempstr;
                        error.str1 = tempstr;
                    } else
                        error.message += "?";
                    error.message += "'";
                    break;
                case RNC_ER_DFLTDT:
                    error.level = XML_ERR_WARNING;
                    error.message = "overriding default datatype prefix '";
                    tempstr = va_arg(ap, char*);
                    if (tempstr) {
                        error.message += tempstr;
                        error.str1 = tempstr;
                    } else
                        error.message += "?";
                    error.message += "'";
                    break;
                case RNC_ER_NONS:
                    error.message = "undeclared namespace prefix '";
                    tempstr = va_arg(ap, char*);
                    if (tempstr) {
                        error.message += tempstr;
                        error.str1 = tempstr;
                    } else
                        error.message += "?";
                    error.message += "'";
                    break;
                case RNC_ER_NODT:
                    error.message = "undeclared datatype prefix '";
                    tempstr = va_arg(ap, char*);
                    if (tempstr) {
                        error.message += tempstr;
                        error.str1 = tempstr;
                    } else
                        error.message += "?";
                    error.message += "'";
                    break;
                case RNC_ER_NCEX:
                    error.message = "first argument for '-' is not '*' or 'prefix:*'";
                    break;
                case RNC_ER_2HEADS:
                    error.message = "repeated define or start";
                    break;
                case RNC_ER_COMBINE:
                    error.message = "conflicting combine methods in define or start";
                    break;
                case RNC_ER_OVRIDE:
                    error.message = "'";
                    tempstr = va_arg(ap, char*);
                    if (tempstr) {
                        error.message += tempstr;
                        error.str1 = tempstr;
                    } else
                        error.message += "?";
                    error.message += "' overrides nothing";
                    break;
                case RNC_ER_EXPT:
                    error.message = "first argument for '-' is not data";
                    break;
                case RNC_ER_INCONT:
                    error.message = "include inside include";
                    break;
                case RNC_ER_NOSTART:
                    error.message = "missing start";
                    break;
                case RNC_ER_UNDEF:
                    error.message = "undefined reference to '";
                    tempstr = va_arg(ap, char*);
                    if (tempstr) {
                        error.message += tempstr;
                        error.str1 = tempstr;
                    } else
                        error.message += "?";
                    error.message += "'";
                    break;
                default:
                    error.message = "Unknown RNC lexing error.";
                }
            } else if (runerrno & ERBIT_RND) {
                runerrno = runerrno&~ERBIT_RND;
                switch (runerrno) {
                case RND_ER_LOOPST:
                    error.message = "loop in start pattern";
                    break;
                case RND_ER_LOOPEL:
                    error.message = "loop in pattern for element '";
                    tempstr = va_arg(ap, char*);
                    if (tempstr) {
                        error.message += tempstr;
                        error.str1 = tempstr;
                    } else
                        error.message += "?";
                    error.message += "'";
                    break;
                case RND_ER_CTYPE:
                    error.message = "content of element '";
                    tempstr = va_arg(ap, char*);
                    if (tempstr) {
                        error.message += tempstr;
                        error.str1 = tempstr;
                    } else
                        error.message += "?";
                    error.message += "' does not have a content-type";
                    break;
                case RND_ER_BADSTART:
                    error.message = "bad path in start pattern";
                    break;
                case RND_ER_BADMORE:
                    error.message = "bad path before '*' or '+' in element '";
                    tempstr = va_arg(ap, char*);
                    if (tempstr) {
                        error.message += tempstr;
                        error.str1 = tempstr;
                    } else
                        error.message += "?";
                    error.message += "'";
                    break;
                case RND_ER_BADEXPT:
                    error.message = "bad path after '-' in element '";
                    tempstr = va_arg(ap, char*);
                    if (tempstr) {
                        error.message += tempstr;
                        error.str1 = tempstr;
                    } else
                        error.message += "?";
                    error.message += "'";
                    break;
                case RND_ER_BADLIST:
                    error.message = "bad path after 'list' in element '";
                    tempstr = va_arg(ap, char*);
                    if (tempstr) {
                        error.message += tempstr;
                        error.str1 = tempstr;
                    } else
                        error.message += "?";
                    error.message += "'";
                    break;
                case RND_ER_BADATTR:
                    error.message = "bad path in attribute '";
                    tempstr = va_arg(ap, char*);
                    if (tempstr) {
                        error.message += tempstr;
                        error.str1 = tempstr;
                    } else
                        error.message += "?";
                    error.message += "' of element '";
                    tempstr = va_arg(ap, char*);
                    if (tempstr) {
                        error.message += tempstr;
                        error.str2 = tempstr;
                    } else
                        error.message += "?";
                    error.message += "'";
                    break;
                default:
                    error.message = "Unknown RNC lexing error.";
                }
            } else {
                error.message = "Unknown RNC lexing error.";
            }
        } else if (erno&ERBIT_RNV) {
            if (g_nodecontext) {
                error.node = g_nodecontext;
                error.line = g_nodecontext->line;
            }
            switch (runerrno) {
            case RNV_ER_ELEM:
                error.message = "element ";
                tempstr = va_arg(ap, char*);
                if (tempstr) {
                    error.message += tempstr;
                    error.str1 = tempstr;
                    if (error.str1.size())
                        error.message += ":";
                } else
                    error.message += "";
                tempstr = va_arg(ap, char*);
                if (tempstr) {
                    error.message += tempstr;
                    error.str2 = tempstr;
                } else
                    error.message += "?";
                error.message += " not allowed";
                break;
            case RNV_ER_AKEY:
                error.message = "attribute ";
                tempstr = va_arg(ap, char*);
                if (tempstr) {
                    error.message += tempstr;
                    error.str1 = tempstr;
                    if (error.str1.size())
                        error.message += ":";
                } else
                    error.message += "";
                tempstr = va_arg(ap, char*);
                if (tempstr) {
                    error.message += tempstr;
                    error.str2 = tempstr;
                } else
                    error.message += "?";
                error.message += " not allowed";
                break;
            case RNV_ER_AVAL:
                error.message = "attribute ";
                tempstr = va_arg(ap, char*);
                if (tempstr) {
                    error.message += tempstr;
                    error.str1 = tempstr;
                    if (error.str1.size())
                        error.message += ":";
                } else
                    error.message += "";
                tempstr = va_arg(ap, char*);
                if (tempstr) {
                    error.message += tempstr;
                    error.str2 = tempstr;
                } else
                    error.message += "?";
                error.message += " with invalid value \"";
                tempstr = va_arg(ap, char*);
                if (tempstr) {
                    error.message += tempstr;
                    error.str3 = tempstr;
                } else
                    error.message += "?";
                error.message += "\"";
                break;
            case RNV_ER_EMIS:
                error.message = "incomplete content";
                break;
            case RNV_ER_AMIS:
                error.message = "missing attributes of ";
                tempstr = va_arg(ap, char*);
                if (tempstr) {
                    error.message += tempstr;
                    error.str1 = tempstr;
                    if (error.str1.size())
                        error.message += ":";
                } else
                    error.message += "";
                tempstr = va_arg(ap, char*);
                if (tempstr) {
                    error.message += tempstr;
                    error.str2 = tempstr;
                } else
                    error.message += "?";
                break;
            case RNV_ER_UFIN:
                error.message = "unfinished content of element ";
                tempstr = va_arg(ap, char*);
                if (tempstr) {
                    error.message += tempstr;
                    error.str1 = tempstr;
                    if (error.str1.size())
                        error.message += ":";
                } else
                    error.message += "";
                tempstr = va_arg(ap, char*);
                if (tempstr) {
                    error.message += tempstr;
                    error.str2 = tempstr;
                } else
                    error.message += "?";
                break;
            case RNV_ER_TEXT:
                error.message = "invalid data or text not allowed";
                break;
            case RNV_ER_NOTX:
                error.message = "text not allowed";
                break;
            default:
                assert(0);
            }
        }

        g_rnvclaimed->addStructuredError(error);
    }
}

cXmlValidatorRNVRelaxNG::~cXmlValidatorRNVRelaxNG() {
    Init();
}

#define INIT_XSL_RESOURCE(resname) \
        { \
            buf_size = resname ## _xsl_size_unzipped; \
            boost::shared_array<char> buf(new char[buf_size]); \
            r = uncompress(reinterpret_cast<Bytef*>(buf.get()), &buf_size, reinterpret_cast<const Bytef*>(resname ## _xsl), resname ## _xsl_size); \
            if (r != Z_OK) \
                throw eXml("Uncompressing " #resname ".xsl failed."); \
            string t(buf.get(), static_cast<size_t>(buf_size+1)); \
            cXmlInputOutputCallbackString::add(string(cXmlInputOutputCallbackString::INTERNAL)+"/xsl/" #resname ".xsl", t); \
        }

void cXmlValidatorRNVRelaxNG::Init() {
    if (!g_resinit) {
        int r;
        uLongf buf_size;
        cXmlInputOutputCallbackString::Init();

        {
            buf_size = relaxng_rng_size_unzipped;
            boost::shared_array<char> buf(new char[buf_size]);
            r = uncompress(reinterpret_cast<Bytef*>(buf.get()), &buf_size, reinterpret_cast<const Bytef*>(relaxng_rng), relaxng_rng_size);
            if (r != Z_OK)
                throw eXml("Uncompressing relaxng.rng failed.");
            string t(buf.get(), static_cast<size_t>(buf_size+1));
            cXmlInputOutputCallbackString::add(string(cXmlInputOutputCallbackString::INTERNAL)+"/rnc/relaxng.rnc", t);
        }
        INIT_XSL_RESOURCE(incelim)
        INIT_XSL_RESOURCE(inc)
        INIT_XSL_RESOURCE(elim)
        INIT_XSL_RESOURCE(strip)
        INIT_XSL_RESOURCE(clean)
        INIT_XSL_RESOURCE(RngToRncClassic)

//        cXmlInputOutputCallbackString::add(string(cXmlInputOutputCallbackString::INTERNAL)+"/schematron/iso_simple.xsl", reinterpret_cast<const char*>(iso_simple_xsl_stripped_2));
        rnl_init();
        rnl_verror_handler=cXmlValidatorRNVRelaxNG::verror_handler_rnl;
        rnv_init();
        rnv_verror_handler=cXmlValidatorRNVRelaxNG::verror_handler_rnv;
        rnx_init();
//        drv_add_dtl(DXL_URL,&dxl_equal,&dxl_allows);
//        drv_add_dtl(DSL_URL,&dsl_equal,&dsl_allows);

        g_resinit = true;
    }

    rnl_clear();
    rnv_clear();
    rnx_clear();

    m_rncstatus = 0;
    m_rncfile = "";
    m_rncbuffer.reset();
    m_rncbuffersize = 0;
    m_type = VAL_RNV_UNSPECIFIED;
    m_errstage = ERRSTAGE_UNKNOWN;
    m_rng = cXmlDoc();
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

//bool cXmlValidatorRNVRelaxNG::read(cXmlDoc& doc, const std::map<std::string, std::string>& options) {
//    Init();
//
//    if (!doc.ok())
//        throw eXml("Tried to read schematron schema from broken document");
//
//    string sch = string(cXmlInputOutputCallbackString::PROTOCOL)+string(cXmlInputOutputCallbackString::INTERNAL)+"/schematron/iso_simple.xsl";
//    cXmlDoc schemadoc(sch.c_str());
//
//    cXsltStylesheet schema(schemadoc);
//    if (!schema.ok())
//        throw eXml("Internal error: could not load schematron xsl.");
//    schema.setParameters(options);
//    cXmlDoc temp = schema.transform(doc);
//
//    if (!temp.ok()) {
//        transferErrors(temp);
//        return false;
//    }
//
//    m_transform = cXsltStylesheet(temp);
//
//    if (!m_transform.ok()) {
//        transferErrors(m_transform);
//    }
//
//    return ok();
//}


bool cXmlValidatorRNVRelaxNG::readRNC(const std::string& buffer, const char* URL) {
    Init();
    m_type = VAL_RNV_RNC;
    return ParseRNC(buffer, URL);
}

bool cXmlValidatorRNVRelaxNG::ParseRNC(const std::string& buffer, const char* URL) {
    m_rncfile = URL?URL:"";
    m_rncbuffersize = buffer.size()+1;
    m_rncbuffer = boost::shared_array<char>(new char[m_rncbuffersize]);
    memcpy(m_rncbuffer.get(), buffer.c_str(), m_rncbuffersize);

    claimRNV();
    m_rncstatus = rnl_s(const_cast<char*>(m_rncfile.c_str()), m_rncbuffer.get(), m_rncbuffersize);
    releaseRNV();

    m_errstage = ok()?ERRSTAGE_OK:ERRSTAGE_RNC;

    return ok();
}

bool cXmlValidatorRNVRelaxNG::readRNC(const char* URL) {
    Init();
    m_type = VAL_RNV_RNC;

    boost::shared_ptr<xmlParserInputBuffer> buf(xmlParserInputBufferCreateFilename(URL, XML_CHAR_ENCODING_UTF8), xmlFree);
    if (!buf)
        return false;

    char buffer[4096];
    buffer[4095] = 0;
    int readcount = 0;
    string result;

    while ((readcount = buf->readcallback(buf->context, buffer, 4095))) {
        buffer[readcount] = 0; // Ensure null termination
        result += buffer;
    }

    return ParseRNC(result, URL);
/*
    m_rncfile = URL;
    m_rncbuffersize = result.size()+1;
    m_rncbuffer = boost::shared_array<char>(new char[m_rncbuffersize]);
    memcpy(m_rncbuffer.get(), result.c_str(), m_rncbuffersize);

    claimRNV();
    m_rncstatus = rnl_s(const_cast<char*>(m_rncfile.c_str()), m_rncbuffer.get(), m_rncbuffersize);
    releaseRNV();

    return ok();
*/
}

bool cXmlValidatorRNVRelaxNG::ParseRNG(cXmlDoc& schema, const char* URL) {
    string pth = string(cXmlInputOutputCallbackString::PROTOCOL)+string(cXmlInputOutputCallbackString::INTERNAL)+"/rnc/relaxng.rnc";
    cXmlValidatorRNVRelaxNG val;
    val.readRNC(pth.c_str());
    if (!val) {
        genericerrorcallback("Internal Error: Failed to load RelaxNG schema");
        return false;
    }

    if (schema.validate(val)) {
        m_errstage = ERRSTAGE_RNG_VALIDATION;
        transferErrors(val);
        return false;
    }

    pth = string(cXmlInputOutputCallbackString::PROTOCOL)+string(cXmlInputOutputCallbackString::INTERNAL)+"/xsl/incelim.xsl";
    cXsltStylesheet incelim(pth.c_str());

    if (!incelim) {
        transferErrors(incelim);
        genericerrorcallback("Internal Error: Failed to load incelim transform");
        return false;
    }

    cXmlDoc incelimmed(incelim.transform(schema));
    if (!incelimmed) {
        m_errstage = ERRSTAGE_RNG_INCELIM;
        transferErrors(incelimmed);
        return false;
    }

    pth = string(cXmlInputOutputCallbackString::PROTOCOL)+string(cXmlInputOutputCallbackString::INTERNAL)+"/xsl/RngToRncClassic.xsl";
    cXsltStylesheet rncconv(pth.c_str());

    if (!rncconv) {
        transferErrors(rncconv);
        genericerrorcallback("Internal Error: Failed to load RngToRncClassic transform");
        return false;
    }

    string convertedrnc = rncconv.transformToString(incelimmed);
    if (convertedrnc == "") {
        m_errstage = ERRSTAGE_RNG_CONVERSION;
        transferErrors(rncconv);
        return false;
    }

    return ParseRNC(convertedrnc);
}

bool cXmlValidatorRNVRelaxNG::readRNG(cXmlDoc& schema, const char* URL) {
    Init();
    m_type = VAL_RNV_RNG;

    if (!schema) {
        m_errstage = ERRSTAGE_RNG_PARSE;
        transferErrors(schema);
        return false;
    }

    m_rng = schema;

    return ParseRNG(schema, URL);
}

bool cXmlValidatorRNVRelaxNG::readRNG(const std::string& buffer, const char* URL) {
    cXmlDoc schema(buffer, URL, NULL, XML_PARSE_DTDLOAD);
    return readRNG(schema, URL);
}

bool cXmlValidatorRNVRelaxNG::readRNG(const char* URL) {
    cXmlDoc schema(URL, NULL, XML_PARSE_DTDLOAD);
    return readRNG(schema, URL);
}

bool cXmlValidatorRNVRelaxNG::read(const std::string& buffer, const char* URL) {
    cXmlDoc schema(buffer, URL, NULL, XML_PARSE_DTDLOAD);
    if (schema)
        return readRNG(schema, URL);
    else
        return readRNC(buffer, URL);
}

bool cXmlValidatorRNVRelaxNG::read(const char* URL) {
    cXmlDoc schema(URL, NULL, XML_PARSE_DTDLOAD);
    if (schema)
        return readRNG(schema, URL);
    else
        return readRNC(URL);
}

void cXmlValidatorRNVRelaxNG::reparse() {
    // Preformance determination, so we do not check ok
    claimRNV();
    rnl_s(const_cast<char*>(m_rncfile.c_str()), m_rncbuffer.get(), m_rncbuffersize);
    releaseRNV();
}

struct RNVValidationContext {
    int start;
    int mixed;
    int current;
    int previous;
    bool ok;
    int level;
    string text;

    inline void init() {
        current = start;
        previous = start;
        level = 0;
        ok = true;
        text = "";
#ifdef DUMP
fprintf(stderr, "INIT %d %d %d\n", current, previous, level);
fflush(stderr);
#endif
    }
    void flush_text() {
#ifdef DUMP
fprintf(stderr, "+FLUSH %d %d %d %s %d\n", current, previous, level, text.c_str(), text.size());
#endif
        ok = rnv_text(&current, &previous, const_cast<char*>(text.c_str()), text.size(), mixed) && ok;
        text = "";
#ifdef DUMP
fprintf(stderr, "-FLUSH %d %d %d %s %d\n", current, previous, level, text.c_str(), text.size());
#endif
    }
    void start_tag_open(const string& name) {
        if (current!=rn_notAllowed) {
            mixed=1;
            flush_text();
            bool stok = rnv_start_tag_open(&current, &previous, const_cast<char*>(name.c_str()));
            ok = stok && ok;
            //mixed=0;
        } else {
            ++level;
        }
    }
    void attribute(const string& name, const string& val) {
        if (current!=rn_notAllowed) {
            ok = rnv_attribute(&current, &previous, const_cast<char*>(name.c_str()), const_cast<char*>(val.c_str())) && ok;
        }
    }
    void start_tag_close(const string& name) {
        if (current!=rn_notAllowed) {
            ok = rnv_start_tag_close(&current, &previous, const_cast<char*>(name.c_str())) && ok;
        }
        mixed = 0;
    }
    void end_tag(const string& name) {
        if (current != rn_notAllowed) {
            flush_text();
            ok = rnv_end_tag(&current, &previous, const_cast<char*>(name.c_str())) && ok;
            mixed = 1;
        } else {
            if (level == 0)
                current = previous;
            else
                --level;
        }
    }
    void add_text(const string& tx) {
        if (current != rn_notAllowed) {
            text += tx;
        }
    }

};

void do_node(cXmlNode& node, RNVValidationContext& ctx) {
    if (node.type() == XML_ELEMENT_NODE) {
        cXmlValidatorRNVRelaxNG::g_nodecontext = node.getRaw();
        string name = node.nsname();
        ctx.start_tag_open(name);
        cXmlNode attr(node.properties());
        while (attr.ok()) {
            ctx.attribute(attr.nsname(), attr.content());
            attr.go_next();
        }
        ctx.start_tag_close(name);


        cXmlNode child(node.children());
        while (child.ok()) {
            do_node(child, ctx);
            child.go_next();
        }

        cXmlValidatorRNVRelaxNG::g_nodecontext = node.getRaw();
        ctx.end_tag(name);
        cXmlValidatorRNVRelaxNG::g_nodecontext = NULL;
    } else if (node.type() == XML_TEXT_NODE) {
        ctx.add_text(node.content());
    } else if (node.type() == XML_CDATA_SECTION_NODE) {
        // Actually no idea whether this works, probably not.
        ctx.add_text(node.content());
    }
}

int cXmlValidatorRNVRelaxNG::validate(boost::shared_ptr<xmlDoc>& doc, int options) {
    if (!ok())
        throw eXml("Tried to validate with invalid RelaxNG schema");
    if (!doc)
        throw eXml("Tried to validate broken document");

    //cXmlDoc result = m_transform.transform(doc.get());
    cXmlDoc work(doc);

    cXmlNode root(work.root());
    if (!root.ok())
        return -1;

    claimRNV();
    RNVValidationContext ctx;
    ctx.start = rnl_s(const_cast<char*>(m_rncfile.c_str()), m_rncbuffer.get(), m_rncbuffersize);
    //ctx.start = rnl_fn(const_cast<char*>(m_rncfile.c_str()));
    ctx.init();
    do_node(root, ctx);
    releaseRNV();

/*
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
            error.file = child.getProp("location", "");
            error.str1 = pattern;
            error.str2 = child.getProp("brieflocation", "");
            error.str3 = child.getProp("id", "");
            if (child.hasProp("role")) {
                sscanf(child.getProp("role", "0").c_str(), "%d", &error.int1);
            }
            addStructuredError(error);
            errors++;
        }
        child.go_next();
    }
*/
    return ctx.ok?0:1;
}

void cXmlValidatorRNVRelaxNG::claimRNV() {
#ifdef DUMP
fprintf(stderr, "claim rnv errors\n");
fflush(stderr);
#endif
    if (g_rnvclaimed)
        throw eXml("Errors already claimed");
    g_rnvclaimed = this;
    g_nodecontext = NULL;
}

void cXmlValidatorRNVRelaxNG::releaseRNV() {
#ifdef DUMP
fprintf(stderr, "release rnv errors\n");
fflush(stderr);
#endif
    g_nodecontext = NULL;
    g_rnvclaimed = NULL;
    rnl_clear();
    rnl_clear();
    rnl_clear();
}


#ifdef XMLCPP_USE_XSLT
#endif


} // Namespace


