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

#include <string>
#include <vector>
#include <libxml/parser.h>
#include <boost/shared_ptr.hpp>

#include "cXmlClasses.h"
#include "cXmlErrorHandler.h"

namespace xmlcpp {

class cXmlValidator: public cXmlErrorHandler {
public:
    enum {
        OPT_NONE =                               0,
        OPT_DETERMINE_NODE_BY_XPATH =       1 << 0, ///< Use XPath query to determine nodes for errors
    };

    cXmlValidator(){}
    virtual ~cXmlValidator(){}

    virtual int validate(boost::shared_ptr<xmlDoc>& doc, int options = OPT_NONE) = 0;
    virtual bool ok() const = 0;
};

} // Namespace

#endif // CXMLVALIDATOR_H_INCLUDED
