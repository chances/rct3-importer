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

#ifndef CXMLXPATHRESULT_H
#define CXMLXPATHRESULT_H

#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

#include "cXmlInitHandler.h"
#include "cXmlClasses.h"

namespace xmlcpp {

/// Wraps an xmlXPathObject structure and allows access to node sets
class cXmlXPathResult: private cXmlInitHandler {
public:
    cXmlXPathResult() { Init(); }
    explicit cXmlXPathResult(xmlXPathObjectPtr obj);
    explicit cXmlXPathResult(xmlXPathObjectPtr obj, const boost::shared_ptr<xmlDoc>& doc);
    virtual ~cXmlXPathResult();

    bool assign(xmlXPathObjectPtr obj);
    bool assign(xmlXPathObjectPtr obj, const boost::shared_ptr<xmlDoc>& doc);

    xmlXPathObjectType type();

    cXmlNode operator[](int index) const;
    int size() const;

    inline bool ok() const { return m_obj.get(); }
    inline bool operator!() const { return !ok(); }
    typedef boost::shared_ptr<xmlXPathObject> cXmlXPathResult::*unspecified_bool_type;
    inline operator unspecified_bool_type() const { return ok()?(&cXmlXPathResult::m_obj):NULL; }
    operator std::vector<cXmlNode>() const;
protected:
private:
    boost::shared_ptr<xmlXPathObject> m_obj;
    /// Shared pointer to the xmlDoc that was queried
    /**
     * This keeps the document alive as long as necessary
     */
    boost::shared_ptr<xmlDoc> m_doc;

    void Init();
};

}


#endif // CXMLNODE_H
