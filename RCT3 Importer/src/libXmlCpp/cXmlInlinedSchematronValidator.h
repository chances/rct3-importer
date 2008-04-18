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

#ifndef CXMLINLINEDSCHEMATRONVALIDATOR_H_INCLUDED
#define CXMLINLINEDSCHEMATRONVALIDATOR_H_INCLUDED

#include "cXmlValidator.h"

namespace xmlcpp {

class cXmlInlinedSchematronValidator: public cXmlValidator {
public:
#ifdef XMLCPP_USE_INLINED_SCHEMATRON
    virtual cXmlDoc schematron() = 0;
#endif
};

};

#endif // CXMLINLINEDSCHEMATRONVALIDATOR_H_INCLUDED
