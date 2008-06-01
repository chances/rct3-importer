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

#ifndef CXMLVALIDATOR_H_INCLUDED
#define CXMLVALIDATOR_H_INCLUDED

#include "cxmlconfig.h"

#include <string>
#include <vector>
#include <libxml/parser.h>
#include <boost/shared_ptr.hpp>

#include "cXmlClasses.h"
#include "cXmlErrorHandler.h"
#include "cXmlValidatorResult.h"

namespace xmlcpp {

class cXmlValidator: public cXmlErrorHandler {
public:
    enum {
        OPT_NONE =                               0,
        OPT_DETERMINE_NODE_BY_XPATH =       1 << 0, ///< Use XPath query to determine nodes for errors
    };
    enum {
        VAL_NONE =                       0,
        VAL_RELAXNG =                   10,
        VAL_SCHEMATRON =                20,
        VAL_RNV_UNSPECIFIED =           30,
        VAL_RNV_RNC,
        VAL_RNV_RNG,
        VAL_RNV_SHORTRNG,
        VAL_RNV_EXAMPLOTRON,
        VAL_RNV_END =                   40,
        VAL_ISOSCHEMATRON =             40,
        VAL_LOWMASK =           0x0000FFFF,
        VAL_MULTI =             0x00010000,
        VAL_PLUS_SCHEMATRON =   0x00020000
    };

    cXmlValidator(){}
    virtual ~cXmlValidator(){}

    virtual cXmlValidatorResult validate(boost::shared_ptr<xmlDoc>& doc, cXmlValidatorResult::LEVEL retlevel = cXmlValidatorResult::VR_ERROR, int options = OPT_NONE) = 0;
    virtual bool ok() const = 0;
    virtual int getType() const = 0;

    static std::string getTypeName(int type) {
        std::string ret;
        if (type & VAL_MULTI) {
            ret = "Multi: ";
        }
        switch (type & VAL_LOWMASK) {
            case VAL_NONE:
                ret += "None";
                break;
            case VAL_RELAXNG:
                ret += "RelaxNG (libxml2)";
                break;
            case VAL_SCHEMATRON:
                ret += "Schematron (libxml2)";
                break;
            case VAL_RNV_UNSPECIFIED:
                ret += "RelaxNG (rnv), unspecified";
                break;
            case VAL_RNV_RNC:
                ret += "RelaxNG (rnv), RNC";
                break;
            case VAL_RNV_RNG:
                ret += "RelaxNG (rnv), RNG";
                break;
            case VAL_RNV_SHORTRNG:
                ret += "RelaxNG (rnv), ShortRNG";
                break;
            case VAL_RNV_EXAMPLOTRON:
                ret += "RelaxNG (rnv), Examplotron";
                break;
            case VAL_ISOSCHEMATRON:
                ret += "Iso Schematron";
                break;
            default:
                ret += "Unknown";
                break;
        }
        if (type & VAL_PLUS_SCHEMATRON) {
            ret += " plus inline Schematron";
        }
        return ret;
    }
};

} // Namespace

#endif // CXMLVALIDATOR_H_INCLUDED
