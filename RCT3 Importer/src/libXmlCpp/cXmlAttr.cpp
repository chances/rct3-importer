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

#include "cXmlAttr.h"

#include "cXmlException.h"
#include "cXmlNode.h"

//#define DUMPINIT
//#define DUMP

using namespace std;

namespace xmlcpp {

cXmlAttr::cXmlAttr(xmlAttrPtr attr, const boost::shared_ptr<xmlDoc>& owner):m_attr(attr),m_owner(owner) {
#ifdef DUMPINIT
fprintf(stderr, "cXmlAttr()\n");
fflush(stderr);
#endif
}

cXmlAttr::~cXmlAttr() {
#ifdef DUMPINIT
fprintf(stderr, "~cXmlAttr()\n");
fflush(stderr);
#endif
}

string cXmlAttr::name() {
    if (!m_attr)
        throw eXmlInvalid("Tried to get name for bad attr");
    return string(reinterpret_cast<const char*>(m_attr->name));
}

string cXmlAttr::ns() {
    if (!m_attr)
        throw eXmlInvalid("Tried to get namespace for bad node");
#ifdef DUMP
fprintf(stderr, "cXmlAttr::ns()\n");
fflush(stderr);
#endif
    if (m_attr->ns) {
#ifdef DUMP
fprintf(stderr, "cXmlAttr::ns(), ns\n");
fflush(stderr);
#endif
        if (m_attr->ns->href) {
#ifdef DUMP
fprintf(stderr, "cXmlAttr::ns(), href\n");
fflush(stderr);
#endif
            return string(reinterpret_cast<const char*>(m_attr->ns->href));
        }
/*
    } else {
        if (m_attr->parent) {
            if (m_attr->parent->ns) {
                if (m_attr->parent->ns->href) {
                    return string(reinterpret_cast<const char*>(m_attr->parent->ns->href));
                }
            }
        }
*/
    }
    return "";
}

string cXmlAttr::nsname() {
    return (ns()=="")?name():ns()+':'+name();
}


int cXmlAttr::type() {
    if (m_attr)
        return m_attr->type;
    else
        return 0;
}

bool cXmlAttr::isBlank() {
    if (!m_attr)
        throw eXmlInvalid("Tried to call isBlank for bad attr");
    if (m_attr->children) {
        return xmlIsBlankNode(m_attr->children);
    } else {
        return true;
    }
}

string cXmlAttr::content() {
    if (!m_attr)
        throw eXmlInvalid("Tried to get content of a bad attr");
    if (!m_attr->children)
        return "";

    xmlChar* pr = xmlNodeGetContent(m_attr->children);
    if (pr) {
        string temp = string(reinterpret_cast<const char*>(pr));
        xmlFree(pr);
        return temp;
    } else {
        return "";
    }
}

cXmlAttr cXmlAttr::next() {
    if (!m_attr)
        throw eXmlInvalid("Tried to get next attr for bad attr");
    return cXmlAttr(m_attr->next, m_owner);
}

void cXmlAttr::go_next() {
    if (m_attr)
        m_attr = m_attr->next;
}

cXmlNode cXmlAttr::parent() {
    if (!m_attr)
        throw eXmlInvalid("Tried to get owner for bad attr");
    return cXmlNode(m_attr->parent, m_owner);
}



} // Namespace end
