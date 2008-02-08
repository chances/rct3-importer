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

#include "cXmlAttr.h"
#include "cXmlException.h"

//#define DUMPINIT

using namespace std;

namespace xmlcpp {

cXmlNode::cXmlNode(xmlNodePtr node, const boost::shared_ptr<xmlDoc>& owner):m_node(node),m_owner(owner) {
#ifdef DUMPINIT
fprintf(stderr, "cXmlNode()\n");
fflush(stderr);
#endif
}

cXmlNode::~cXmlNode() {
#ifdef DUMPINIT
fprintf(stderr, "~cXmlNode()\n");
fflush(stderr);
#endif
}

string cXmlNode::name() {
    if (!m_node)
        throw eXmlInvalid("Tried to get name for bad node");
    return string(reinterpret_cast<const char*>(m_node->name));
}

string cXmlNode::ns() {
    if (!m_node)
        throw eXmlInvalid("Tried to get namespace for bad node");
    if (m_node->ns) {
        if (m_node->ns->href) {
            return string(reinterpret_cast<const char*>(m_node->ns->href));
        }
    }
    return "";
}

string cXmlNode::nsname() {
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

bool cXmlNode::hasProp(const string& prop) {
    if (!m_node)
        throw eXmlInvalid("Tried to call hasProp for bad node");
    return xmlHasProp(m_node, reinterpret_cast<const xmlChar*>(prop.c_str()));
}

string cXmlNode::getProp(const string& prop, const string& def) {
    if (!m_node)
        throw eXmlInvalid("Tried to call getProp for bad node");
    xmlChar* pr = xmlGetProp(m_node, reinterpret_cast<const xmlChar*>(prop.c_str()));
    if (pr) {
        string temp = string(reinterpret_cast<const char*>(pr));
        xmlFree(pr);
        return temp;
    } else {
        return def;
    }
}

string cXmlNode::content() {
    if (!m_node)
        throw eXmlInvalid("Tried to get content of a bad node");
    xmlChar* pr = xmlNodeGetContent(m_node);
    if (pr) {
        string temp = string(reinterpret_cast<const char*>(pr));
        xmlFree(pr);
        return temp;
    } else {
        return "";
    }
}

cXmlNode cXmlNode::next() {
    if (!m_node)
        throw eXmlInvalid("Tried to get next node for bad node");
    return cXmlNode(m_node->next, m_owner);
}

void cXmlNode::go_next() {
    if (m_node)
        m_node = m_node->next;
}

cXmlNode cXmlNode::children() {
    if (!m_node)
        throw eXmlInvalid("Tried to get children for bad node");
    return cXmlNode(m_node->children, m_owner);
}

cXmlAttr cXmlNode::properties() {
    if (!m_node)
        throw eXmlInvalid("Tried to get properties for bad node");
    return cXmlAttr(m_node->properties, m_owner);
}


} // Namespace end
