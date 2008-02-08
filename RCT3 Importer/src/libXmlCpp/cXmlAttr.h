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

#ifndef CXMLATTR_H
#define CXMLATTR_H

#include <libxml/parser.h>
#include <string>
#include <boost/shared_ptr.hpp>

#include "cXmlInitHandler.h"
#include "cXmlClasses.h"

namespace xmlcpp {

/// Wraps an xmlAttr (property) structure
class cXmlAttr: private cXmlInitHandler {
public:
    cXmlAttr(xmlAttrPtr node, const boost::shared_ptr<xmlDoc>& owner);
    virtual ~cXmlAttr();

    std::string name();
    std::string ns();
    std::string nsname();

    /// Return node type
    /**
     * @return xmlElementType or 0 for invalid.
     */
    int type();

    /// Is the node whitespace only?
    /**
     * Maps to the content node
     */
    bool isBlank();

    /// Get node content
    /**
     * Read the value of a node, this can be either the text carried directly by this node if it's a TEXT node or
     * the aggregate string of the values carried by this node child's (TEXT and ENTITY_REF).
     * Entity references are substituted.
     * (cited from the libxml docs)
     *
     * Maps to the content node
     */
    std::string content();

    /// Return next sibling property
    cXmlAttr next();
    /// Advance this object to the next sibling node
    /**
     * Note: this function silently fails for an invalid node.
     */
    void go_next();

    /// Node owner/parent of the property
    cXmlNode parent();

    inline bool ok() { return m_attr; }
protected:
private:
    xmlAttrPtr m_attr;
    /// Shared ponter to the xmlDoc that owns the property
    /**
     * This keeps the document allive as long as there are cXmlAttrs for
     * one of its properties around.
     */
    boost::shared_ptr<xmlDoc> m_owner;
};

}


#endif // CXMLNODE_H
