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

#include "cXmlNode.h"

#include <boost/shared_array.hpp>

#include "cXmlException.h"

//#define DUMPINIT

using namespace std;

namespace xmlcpp {

cXmlNode::cXmlNode() {
#ifdef DUMPINIT
fprintf(stderr, "cXmlNode()\n");
fflush(stderr);
#endif
    Init();
}

cXmlNode::cXmlNode(xmlNodePtr node, const boost::shared_ptr<xmlDoc>& owner) {
#ifdef DUMPINIT
fprintf(stderr, "cXmlNode()\n");
fflush(stderr);
#endif
    Init();
    m_node = node;
    m_owner = owner;
}

cXmlNode::cXmlNode(xmlNodePtr node, bool managed) {
#ifdef DUMPINIT
fprintf(stderr, "cXmlNode()\n");
fflush(stderr);
#endif
    Init();
    m_node = node;
    m_managednode = managed;
}

cXmlNode::cXmlNode(xmlAttrPtr node, const boost::shared_ptr<xmlDoc>& owner) {
#ifdef DUMPINIT
fprintf(stderr, "cXmlNode()\n");
fflush(stderr);
#endif
    Init();
    m_node = reinterpret_cast<xmlNodePtr>(node);
    m_owner = owner;
}

cXmlNode::cXmlNode(xmlAttrPtr node, bool managed) {
#ifdef DUMPINIT
fprintf(stderr, "cXmlNode()\n");
fflush(stderr);
#endif
    Init();
    m_node = reinterpret_cast<xmlNodePtr>(node);
    m_managednode = managed;
}

cXmlNode::cXmlNode(const std::string& nname, xmlNsPtr ns) {
#ifdef DUMPINIT
fprintf(stderr, "cXmlNode()\n");
fflush(stderr);
#endif
    Init();
    m_node = xmlNewNode(ns, reinterpret_cast<const xmlChar*>(nname.c_str()));
    m_managednode = true;
}

cXmlNode::cXmlNode(const char* nname, xmlNsPtr ns) {
#ifdef DUMPINIT
fprintf(stderr, "cXmlNode()\n");
fflush(stderr);
#endif
    Init();
    m_node = xmlNewNode(ns, reinterpret_cast<const xmlChar*>(nname));
    m_managednode = true;
}

cXmlNode::cXmlNode(const std::string& nname, const std::string& ncontent, xmlNsPtr ns) {
#ifdef DUMPINIT
fprintf(stderr, "cXmlNode()\n");
fflush(stderr);
#endif
    Init();
    m_node = xmlNewNode(ns, reinterpret_cast<const xmlChar*>(nname.c_str()));
    m_managednode = true;
    if (ncontent != "")
        content(ncontent.c_str());
}

cXmlNode::cXmlNode(const char* nname, const char* ncontent, xmlNsPtr ns) {
#ifdef DUMPINIT
fprintf(stderr, "cXmlNode()\n");
fflush(stderr);
#endif
    Init();
    m_node = xmlNewNode(ns, reinterpret_cast<const xmlChar*>(nname));
    m_managednode = true;
    if (ncontent)
        content(ncontent);
}

cXmlNode::cXmlNode(const cXmlNode& node) {
#ifdef DUMPINIT
fprintf(stderr, "cXmlNode()\n");
fflush(stderr);
#endif
    Init();
    assign(node);
}


cXmlNode::~cXmlNode() {
#ifdef DUMPINIT
fprintf(stderr, "~cXmlNode()\n");
fflush(stderr);
#endif
    freemanagednode();
}

void cXmlNode::freemanagednode() {
    if (m_managednode) {
        if (m_node) {
            if (attr())
                xmlFreePropList(reinterpret_cast<xmlAttrPtr>(m_node));
            else
                xmlFreeNodeList(m_node);
        }
        m_managednode = false;
    }
}

void cXmlNode::Init() {
    m_node = NULL;
    m_owner.reset();
    m_managednode = false;
}

string cXmlNode::name() const {
    if (!m_node)
        throw eXmlInvalid("Tried to get name for bad node");
    return string(reinterpret_cast<const char*>(m_node->name));
}

void cXmlNode::name(const char* nname) {
    if (!m_node)
        throw eXmlInvalid("Tried to set name for bad node");

    xmlNodeSetName(m_node, reinterpret_cast<const xmlChar*>(nname));
}

string cXmlNode::ns() const {
    if (!m_node)
        throw eXmlInvalid("Tried to get namespace for bad node");
    if (m_node->ns) {
        if (m_node->ns->href) {
            return string(reinterpret_cast<const char*>(m_node->ns->href));
        }
    }
    return "";
}

void cXmlNode::ns(xmlNsPtr nns) {
    if (!m_node)
        throw eXmlInvalid("Tried to set namespace for bad node");
    xmlSetNs(m_node, nns);
}

xmlNsPtr cXmlNode::ns(const char* href, const char* prefix) {
    if (!m_node)
        throw eXmlInvalid("Tried to set namespace for bad node");
    return xmlNewNs(m_node, reinterpret_cast<const xmlChar*>(href), reinterpret_cast<const xmlChar*>(prefix));
}

string cXmlNode::nsname() const {
    return (ns()=="")?name():ns()+':'+name();
}

string cXmlNode::path() {
    if (!m_node)
        throw eXmlInvalid("Tried to get path for bad node");

    string ret;
    boost::shared_array<xmlChar> npath(xmlGetNodePath(m_node), xmlFree);
    if (!npath)
        return "";
    ret = reinterpret_cast<char*>(npath.get());

    return ret;
}

int cXmlNode::type() {
    if (m_node)
        return m_node->type;
    else
        return 0;
}

bool cXmlNode::isBlank() {
    if (!m_node)
        throw eXmlInvalid("Tried to call isBlank for bad node");
    return xmlIsBlankNode(m_node);
}

bool cXmlNode::hasProp(const char* nprop) const {
    if (!m_node)
        throw eXmlInvalid("Tried to call hasProp for bad node");
    return xmlHasProp(m_node, reinterpret_cast<const xmlChar*>(nprop));
}

string cXmlNode::getPropVal(const char* nprop, const char* def) const {
    if (!m_node)
        throw eXmlInvalid("Tried to call getProp for bad node");
    xmlChar* pr = xmlGetProp(m_node, reinterpret_cast<const xmlChar*>(nprop));
    if (pr) {
        string temp = string(reinterpret_cast<const char*>(pr));
        xmlFree(pr);
        return temp;
    } else {
        return def?def:"";
    }
}

bool cXmlNode::getPropVal(const std::string& nprop, std::string* val) const {
    if (!m_node)
        throw eXmlInvalid("Tried to call getProp for bad node");
    if (!val)
        throw eXmlInvalid("Tried to call getProp with NULL pointer");

    xmlChar* pr = xmlGetProp(m_node, reinterpret_cast<const xmlChar*>(nprop.c_str()));
    if (pr) {
        *val = string(reinterpret_cast<const char*>(pr));
        xmlFree(pr);
        return true;
    } else {
        return false;
    }
}


cXmlNode cXmlNode::addProp(const char* nprop, const char* val) {
    if (!m_node)
        throw eXmlInvalid("Tried to add property for bad node");

    return cXmlNode(xmlNewProp(m_node, reinterpret_cast<const xmlChar*>(nprop), reinterpret_cast<const xmlChar*>(val)), m_owner);
}

bool cXmlNode::delProp(const char* nprop) {
    if (!m_node)
        throw eXmlInvalid("Tried to delete property for bad node");
    return !xmlRemoveProp(xmlHasProp(m_node, reinterpret_cast<const xmlChar*>(nprop)));
}

cXmlNode cXmlNode::prop_internal(const char* nprop, bool do_throw) const {
    if (!m_node) {
        if (do_throw) {
            throw eXmlInvalid("Tried to call prop for bad node");
        } else {
            return cXmlNode();
        }
    }
    xmlNodePtr att = reinterpret_cast<xmlNodePtr>(xmlHasProp(m_node, reinterpret_cast<const xmlChar*>(nprop)));
    if (!att)
        return cXmlNode(); // Do not pass owner if not there
    return cXmlNode(att, m_owner);
}

cXmlNode cXmlNode::prop(const char* nprop, const char* val, bool encode_spchars) {
    if (!m_node)
        throw eXmlInvalid("Tried to set prop of a bad node");

    cXmlNode propnode = prop(nprop);
    if (!propnode)
        propnode = addProp(nprop, "");

    if (!propnode)
        return cXmlNode();

    propnode.content(val, encode_spchars);
    return propnode;
}


string cXmlNode::content_internal(bool do_throw) const {
    if (!m_node) {
        if (do_throw) {
            throw eXmlInvalid("Tried to get content of a bad node");
        } else {
            return "";
        }
    }
    xmlChar* pr = xmlNodeGetContent(m_node);
    if (pr) {
        string temp = string(reinterpret_cast<const char*>(pr));
        xmlFree(pr);
        return temp;
    } else {
        return "";
    }
}

void cXmlNode::content(const char* cont, bool encode_spchars) {
    if (!m_node)
        throw eXmlInvalid("Tried to set content of a bad node");

    if (encode_spchars) {
        xmlChar* encoded = xmlEncodeSpecialChars(NULL, reinterpret_cast<const xmlChar*>(cont));
        xmlNodeSetContent(m_node, encoded);
        xmlFree(encoded);
    } else {
        xmlNodeSetContent(m_node, reinterpret_cast<const xmlChar*>(cont));
    }
}

void cXmlNode::unlink() {
    if (!m_node)
        throw eXmlInvalid("Tried to unlink bad node");
    if (m_managednode)
        throw eXmlInvalid("Tried to unlink managed node");

    xmlUnlinkNode(m_node);
    m_owner.reset();
    m_managednode = true;
}

void cXmlNode::detach() {
    if (!m_node)
        throw eXmlInvalid("Tried to detach bad node");
    if (m_managednode)
        throw eXmlInvalid("Tried to detach managed node");

    if (m_node->type == XML_DTD_NODE) {
        // Same as unlink
        xmlDocPtr doc;
        doc = m_node->doc;
        if (doc != NULL) {
            if (doc->intSubset == (xmlDtdPtr) m_node)
                doc->intSubset = NULL;
            if (doc->extSubset == (xmlDtdPtr) m_node)
                doc->extSubset = NULL;
        }
    }
    if (m_node->parent != NULL) {
        xmlNodePtr parent;
        parent = m_node->parent;
        if (m_node->type == XML_ATTRIBUTE_NODE) {
            if (parent->properties == (xmlAttrPtr) m_node)
                parent->properties = NULL;
        } else {
            if (parent->children == m_node)
                parent->children = NULL;
            parent->last = m_node->prev; // Rest will be detached
        }
        m_node->parent = NULL;
    }
    xmlNodePtr walk = m_node->next;
    while (walk) {
        walk->parent = NULL;
        walk = walk->next;
    }
    if (m_node->prev != NULL)
        m_node->prev->next = NULL;
    m_node->prev = NULL;

    m_owner.reset();
    m_managednode = true;
}

cXmlNode cXmlNode::newTextChild(const char* name, const char* content) {
    if (!m_node)
        throw eXmlInvalid("Tried to add new child to bad node");

    return cXmlNode(xmlNewTextChild(m_node, NULL, reinterpret_cast<const xmlChar*>(name), reinterpret_cast<const xmlChar*>(content)), m_owner);
}

cXmlNode cXmlNode::newChild(const char* name, const char* content) {
    if (!m_node)
        throw eXmlInvalid("Tried to add new child to bad node");

    return cXmlNode(xmlNewChild(m_node, NULL, reinterpret_cast<const xmlChar*>(name), reinterpret_cast<const xmlChar*>(content)), m_owner);
}

void cXmlNode::appendChildren(cXmlNode& node) {
    if (!m_node)
        throw eXmlInvalid("Tried to attach node(s) to bad node");
    if (!node.ok())
        throw eXmlInvalid("Tried to attach bad node(s) to node");

    if (!node.managed()) {
        node.detach();
    }

    if (node.getRaw()->next) {
        xmlAddChildList(m_node, node.getRaw());
    } else {
        xmlAddChild(m_node, node.getRaw());
    }
    node.Init();
}

cXmlNode cXmlNode::insertNodeBefore(cXmlNode& node) {
    if (!m_node)
        throw eXmlInvalid("Tried to insert node before bad node");
    if (!node.ok())
        throw eXmlInvalid("Tried to insert bad node before node");
    if (node.managed() && node.isList())
        throw eXmlInvalid("Tried to insert managed node list before node");

    if (node.managed()) {
        // need to be careful
        node.m_managednode = false;
        xmlNodePtr temp = xmlAddPrevSibling(m_node, node.getRaw());
        if (temp) {
            node = cXmlNode(temp, m_owner);
        } else {
            node.m_managednode = true;
            return cXmlNode();
        }
    } else {
        node = cXmlNode(xmlAddPrevSibling(m_node, node.getRaw()), m_owner);
    }
    return node;
}

cXmlNode cXmlNode::insertNodeAfter(cXmlNode& node) {
    if (!m_node)
        throw eXmlInvalid("Tried to insert node after bad node");
    if (!node.ok())
        throw eXmlInvalid("Tried to insert bad node after node");
    if (node.managed() && node.isList())
        throw eXmlInvalid("Tried to insert managed node list after node");

    if (node.managed()) {
        // need to be careful
        node.m_managednode = false;
        xmlNodePtr temp = xmlAddNextSibling(m_node, node.getRaw());
        if (temp) {
            node = cXmlNode(temp, m_owner);
        } else {
            node.m_managednode = true;
            return cXmlNode();
        }
    } else {
        node = cXmlNode(xmlAddNextSibling(m_node, node.getRaw()), m_owner);
    }
    return node;
}

void cXmlNode::insertNodeAsFirstChild(cXmlNode& node) {
    if (children()) {
        children().insertNodeBefore(node);
    } else {
        appendChildren(node);
    }
}

cXmlNode cXmlNode::next() {
    return m_node?cXmlNode(m_node->next, m_owner):cXmlNode();
}

void cXmlNode::go_next() {
    if (m_managednode)
        throw eXmlInvalid("Cannot walk managed node");
    if (m_node)
        m_node = m_node->next;
}

cXmlNode cXmlNode::children() {
    return m_node?cXmlNode(m_node->children, m_owner):cXmlNode();
}

const cXmlNode cXmlNode::children() const {
    if (!m_node)
        throw eXmlInvalid("Tried to get children for bad node");
    return cXmlNode(m_node->children, m_owner);
}

cXmlNode cXmlNode::properties() {
    return m_node?cXmlNode(m_node->properties, m_owner):cXmlNode();
}

cXmlNode& cXmlNode::operator= (const cXmlNode& node) {
    assign(node);
    return *this;
}

void cXmlNode::assign(const cXmlNode& node) {
    freemanagednode();
    Init();
    m_owner = node.m_owner;
    m_managednode = node.m_managednode;
    if (node.m_managednode) {
        m_node = xmlCopyNode(node.m_node, 1);
    } else {
        m_node = node.m_node;
    }
}

/** @brief end
  *
  * @todo: document this function
  */
cXmlNode::const_iterator cXmlNode::end() const {
    return cXmlNode_const_iterator();
}

/** @brief begin
  *
  * @todo: document this function
  */
cXmlNode::const_iterator cXmlNode::begin() const {
    return cXmlNode_const_iterator(*this);
}

/** @brief end
  *
  * @todo: document this function
  */
cXmlNode::iterator cXmlNode::end() {
    return cXmlNode_iterator();
}

/** @brief begin
  *
  * @todo: document this function
  */
cXmlNode::iterator cXmlNode::begin() {
    return cXmlNode_iterator(*this);
}



} // Namespace end
