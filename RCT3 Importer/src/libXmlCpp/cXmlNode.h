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

#ifdef XMLCPP_USE_WXWIDGETS
#include <wx/string.h>
#endif

#include "cXmlClasses.h"
#include "cXmlInitHandler.h"

namespace xmlcpp {

/// Wraps an xmlNode structure, also handles attributes
class cXmlNode: private cXmlInitHandler {
public:
    cXmlNode();
    cXmlNode(xmlNodePtr node, bool managed = false);
    cXmlNode(xmlNodePtr node, const boost::shared_ptr<xmlDoc>& owner);
    cXmlNode(xmlAttrPtr node, bool managed = false);
    cXmlNode(xmlAttrPtr node, const boost::shared_ptr<xmlDoc>& owner);
    cXmlNode(const std::string& nname, xmlNsPtr ns);
    cXmlNode(const char* nname, xmlNsPtr ns);
    cXmlNode(const std::string& nname, const std::string& ncontent = "", xmlNsPtr ns = NULL);
    cXmlNode(const char* nname, const char* ncontent = NULL, xmlNsPtr ns = NULL);
    cXmlNode(const cXmlNode& node);
    virtual ~cXmlNode();

    /// Get node name
    /**
     * @return std::string node name.
     */
    std::string name() const;

    /// Set node name
    inline void name(const std::string& nname) { name(nname.c_str()); }
    void name(const char* nname);

    /// Get node namespace
    /**
     * @return std::string node namespace uri.
     */
    std::string ns() const;

    /// Set node namespace (existing)
    void ns(xmlNsPtr nns);

    /// Set node namespace (new)
    inline xmlNsPtr ns(const std::string& href, const std::string& prefix = "") { return ns(href.c_str(), (prefix != "")?prefix.c_str():NULL); }
    xmlNsPtr ns(const char* href, const char* prefix = NULL);

    /// Get full node name
    /**
     * @return std::string node namespace uri + name, eg http://www.example.com:node.
     */
    std::string nsname() const;

    std::string path();

    /// Return node type
    /**
     * @return xmlElementType or 0 for invalid.
     */
    int type();

    /// Check node type
    inline bool is(int type) const { return m_node?(m_node->type == type):false; }

    /// Is the node whitespace only?
    bool isBlank();

    /// Does the node contain an certain attribute?
    inline bool hasProp(const std::string& nprop) const { return hasProp(nprop.c_str()); }
    bool hasProp(const char* nprop) const;

    /// Get property value
    inline std::string getPropVal(const std::string& nprop, const std::string& def = "") const { return getPropVal(nprop.c_str(), def.c_str()); }
    std::string getPropVal(const char* nprop, const char* def = NULL) const;
    bool getPropVal(const std::string& nprop, std::string* val) const;

    /// Add a new property
    inline cXmlNode addProp(const std::string& nprop, const std::string& val) { return addProp(nprop.c_str(), val.c_str()); }
    cXmlNode addProp(const char* nprop, const char* val);

    /// Remove property
    inline bool delProp(const std::string& nprop) { return delProp(nprop.c_str()); }
    bool delProp(const char* nprop);

    /// Return property node
    inline cXmlNode prop(const std::string& nprop) const { return prop_internal(nprop.c_str(), true); }
    inline cXmlNode prop(const char* nprop) const { return prop_internal(nprop, true); }

    /// Set property value
    /**
     * Makes sure the property exists with the given value by updating an existing one or
     * adding a new one.
     */
    inline cXmlNode prop(const std::string& nprop, const std::string& val, bool encode_spchars = true) { return prop(nprop.c_str(), val.c_str(), encode_spchars); }
    cXmlNode prop(const char* nprop, const char* val, bool encode_spchars = true);

    /// Get node content
    /**
     * Read the value of a node, this can be either the text carried directly by this node if it's a TEXT node or
     * the aggregate string of the values carried by this node child's (TEXT and ENTITY_REF).
     * Entity references are substituted.
     * (cited from the libxml docs)
     */
    inline std::string content() const { return content_internal(true); }

    /// Set node content
    /**
     * If @encode_spchars is true, the node content is erased and the new one set with xmlNodeAddContent().
     * Otherwise xmlNodeSetContent() is used. The difference is the same as using @see newTextChild vs.
     * @see newChild respectively.
     */
    inline void content(const std::string& cont, bool encode_spchars = true) { content(cont.c_str(), encode_spchars); }
    void content(const char* cont, bool encode_spchars = true);

    /// Unlink node from the tree
    /**
     * Rips out the node and it's children.
     * The node goes into managed mode.
     */
    void unlink();

    /// Detach node from the tree
    /**
     * Rips out the node, it's following siblings and it's children.
     * The node goes into managed mode.
     */
    void detach();

    /// Append a new text containing node to this nodes children
    /**
     * This works like the libxml2 method of the same name, reserved xml
     * characters are replaced by their entities.
     */
    inline cXmlNode newTextChild(const std::string& name, const std::string& content) { return newTextChild(name.c_str(), content.c_str()); }
    cXmlNode newTextChild(const char* name, const char* content);

    /// Append a new node to this nodes children
    /**
     * This works like the libxml2 method of the same name, if @content is given,
     * it's expected to have reserved characters as entities.
     */
    inline cXmlNode newChild(const std::string& name, const std::string& content = "") { return newTextChild(name.c_str(), (content=="")?NULL:content.c_str()); }
    cXmlNode newChild(const char* name, const char* content = NULL);

    /// Append a node (may be a list) as this nodes children.
    /**
     * This method acts as a sink, @node is invalidated. There is
     * no sane way to know whether the pointer was deleted by libxml2 or not.
     * If @node is not managed, it is detached as by @see detach().
     */
    void appendChildren(cXmlNode& node);

    /// Prepend a node to this one
    cXmlNode insertNodeBefore(cXmlNode& node);

    /// Append a node to this one
    cXmlNode insertNodeAfter(cXmlNode& node);

    /// Assign as first child
    /**
     * Can act as a sink as it may use appendChildren
     */
    void insertNodeAsFirstChild(cXmlNode& node);

    /// Return next sibling node
    cXmlNode next();
    /// Advance this object to the next sibling node
    /**
     * Note: this function silently fails for an invalid node.
     */
    void go_next();

    /// Get first child node
    cXmlNode children();
    const cXmlNode children() const;

    /// Get first property
    cXmlNode properties();

    inline xmlNodePtr getRaw() const { return m_node; }
    inline xmlNodePtr getRaw(const boost::shared_ptr<xmlDoc>& newowner = boost::shared_ptr<xmlDoc>()) {
        if (newowner) {
            if (!m_managednode) {
                unlink();
            }
            m_managednode = false;
            m_owner = newowner;
        }
        return m_node;
    }
    inline xmlAttrPtr getRawAttr() const { return reinterpret_cast<xmlAttrPtr>(m_node); }
    inline bool managed() const { return m_managednode; }

    inline bool ok() const { return m_node; }
    inline bool attr() const { return is(XML_ATTRIBUTE_NODE); }
    inline bool element() const { return is(XML_ELEMENT_NODE); }
    inline bool isList() const { return m_node?(m_node->next||m_node->prev):false; }
    inline bool operator!() const { return !ok(); }
    typedef xmlNodePtr cXmlNode::*unspecified_bool_type;
    inline operator unspecified_bool_type() const { return ok()?(&cXmlNode::m_node):NULL; }

    inline std::string operator() () const { return content_internal(false); }
    inline bool operator() (const std::string& comp, bool with_ns = false) { return (with_ns)?(!nsname().compare(comp)):(!name().compare(comp)); }
    inline bool operator() (const char* comp, bool with_ns = false) { return (with_ns)?(!nsname().compare(comp)):(!name().compare(comp)); }
    inline cXmlNode operator[] (const std::string& nprop) const { return prop_internal(nprop.c_str(), false); }
    inline cXmlNode operator[] (const char* nprop) const { return prop_internal(nprop, false); }
    inline cXmlNode& operator++ () { go_next(); return *this; }
    inline cXmlNode operator++ (int) { cXmlNode t = *this; go_next(); return t; }
    inline cXmlNode& operator<< (cXmlNode& node) { appendChildren(node); return *this; }

    cXmlNode& operator= (const cXmlNode& node);
    bool operator== (const cXmlNode& node) { return (m_node == node.m_node); }

    // Convenience wrappers
    inline cXmlNode prop(const char* nprop, const std::string& val, bool encode_spchars = true) { return prop(nprop, val.c_str(), encode_spchars); }
    inline cXmlNode prop(const std::string& nprop, const char* val, bool encode_spchars = true) { return prop(nprop.c_str(), val, encode_spchars); }

#ifdef XMLCPP_USE_WXWIDGETS
    inline wxString wxns() const { return wxString::FromUTF8(ns().c_str()); }
    inline wxString wxname() const { return wxString::FromUTF8(name().c_str()); }

    inline wxString wxpath() { return wxString::FromUTF8(path().c_str()); }

    inline wxString wxcontent() const { return wxString::FromUTF8(content_internal(true).c_str()); }
    inline void wxcontent(const wxString& cont, bool encode_spchars = true) { content(cont.utf8_str(), encode_spchars); }

    inline cXmlNode wxprop(const wxString& nprop, const wxString& val, bool encode_spchars = true) { return prop(static_cast<const char*>(nprop.utf8_str()), static_cast<const char*>(val.utf8_str()), encode_spchars); }
    inline cXmlNode wxprop(const char* nprop, const wxString& val, bool encode_spchars = true) { return prop(nprop, static_cast<const char*>(val.utf8_str()), encode_spchars); }
    inline cXmlNode wxprop(const wxString& nprop, const char* val, bool encode_spchars = true) { return prop(static_cast<const char*>(nprop.utf8_str()), val, encode_spchars); }

    inline wxString wxgetPropVal(const wxString& nprop, const wxString& def) const {
        return wxString::FromUTF8(getPropVal(static_cast<const char*>(nprop.utf8_str()), static_cast<const char*>(def.utf8_str())).c_str());
    }
    inline wxString wxgetPropVal(const char* nprop, const wxString& def) const {
        return wxString::FromUTF8(getPropVal(nprop, static_cast<const char*>(def.utf8_str())).c_str());
    }
    inline wxString wxgetPropVal(const char* nprop, const char* def) const {
        return wxString::FromUTF8(getPropVal(nprop, def).c_str());
    }
    inline wxString wxgetPropVal(const wxString& nprop) const {
        return wxString::FromUTF8(getPropVal(static_cast<const char*>(nprop.utf8_str()), NULL).c_str());
    }
    inline wxString wxgetPropVal(const char* nprop) const {
        return wxString::FromUTF8(getPropVal(nprop, NULL).c_str());
    }
    inline bool wxgetPropVal(const wxString& nprop, wxString* val) const {
        std::string temp;
        if (getPropVal(static_cast<const char*>(nprop.utf8_str()), &temp)) {
            *val = wxString::FromUTF8(temp.c_str());
            return true;
        }
        return false;
    }
#endif

protected:
private:
    xmlNodePtr m_node;
    /// Shared ponter to the xmlDoc that owns the node
    /**
     * This keeps the document alive as long as there are cXmlNodes for
     * one of its nodes around.
     */
    boost::shared_ptr<xmlDoc> m_owner;
    bool m_managednode;

    void Init();
    void assign(const cXmlNode& node);
    void freemanagednode();

    std::string content_internal(bool do_throw) const;
    cXmlNode prop_internal(const char* nprop, bool do_throw) const;

};

}


#endif // CXMLNODE_H
