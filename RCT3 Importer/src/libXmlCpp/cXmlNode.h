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

#ifndef CXMLNODE_H
#define CXMLNODE_H

#include <libxml/parser.h>
#include <string>
#include <boost/shared_ptr.hpp>

#include "cXmlClasses.h"
#include "cXmlInitHandler.h"

namespace xmlcpp {

/// Wraps an xmlNode structure
class cXmlNode: private cXmlInitHandler {
public:
    cXmlNode(xmlNodePtr node, const boost::shared_ptr<xmlDoc>& owner = (boost::shared_ptr<xmlDoc>()));
    virtual ~cXmlNode();

    /// Get node name
    /**
     * @return std::string node name.
     */
    std::string name();

    /// Get node namespace
    /**
     * @return std::string node namespace uri.
     */
    std::string ns();

    /// Get full node name
    /**
     * @return std::string node namespace uri + name, eg http://www.example.com:node.
     */
    std::string nsname();

    std::string path();

    /// Return node type
    /**
     * @return xmlElementType or 0 for invalid.
     */
    int type();

    /// Is the node whitespace only?
    bool isBlank();

    /// Does the node contain an certain attribute?
    bool hasProp(const std::string& prop);

    /// Get property value
    std::string getProp(const std::string& prop, const std::string& def);

    /// Get node content
    /**
     * Read the value of a node, this can be either the text carried directly by this node if it's a TEXT node or
     * the aggregate string of the values carried by this node child's (TEXT and ENTITY_REF).
     * Entity references are substituted.
     * (cited from the libxml docs)
     */
    std::string content();

    /// Return next sibling node
    cXmlNode next();
    /// Advance this object to the next sibling node
    /**
     * Note: this function silently fails for an invalid node.
     */
    void go_next();

    /// Get first child node
    cXmlNode children();

    /// Get first property
    cXmlAttr properties();

    inline xmlNodePtr getRaw() const { return m_node; }

    inline bool ok() { return m_node; }
protected:
private:
    xmlNodePtr m_node;
    /// Shared ponter to the xmlDoc that owns the node
    /**
     * This keeps the document allive as long as there are cXmlNodes for
     * one of its nodes around.
     */
    boost::shared_ptr<xmlDoc> m_owner;
};

}


#endif // CXMLNODE_H
