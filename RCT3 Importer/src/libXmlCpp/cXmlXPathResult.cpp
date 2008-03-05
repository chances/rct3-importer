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

#include "cXmlXPathResult.h"

#include "cXmlDoc.h"
#include "cXmlException.h"
#include "cXmlNode.h"

//#define DUMPINIT
//#define DUMP

using namespace std;

namespace xmlcpp {

cXmlXPathResult::cXmlXPathResult(xmlXPathObjectPtr obj) {
    assign(obj);
}

cXmlXPathResult::cXmlXPathResult(xmlXPathObjectPtr obj, const boost::shared_ptr<xmlDoc>& doc) {
    assign(obj, doc);
}

cXmlXPathResult::~cXmlXPathResult() {
#ifdef DUMPINIT
fprintf(stderr, "~cXmlXPathResult()\n");
fflush(stderr);
#endif
}

void cXmlXPathResult::Init() {
    m_doc.reset();
    m_obj.reset();
}

bool cXmlXPathResult::assign(xmlXPathObjectPtr obj) {
    Init();
    m_obj = boost::shared_ptr<xmlXPathObject>(obj, xmlXPathFreeObject);
    return ok();
}

bool cXmlXPathResult::assign(xmlXPathObjectPtr obj, const boost::shared_ptr<xmlDoc>& doc) {
    Init();
    m_obj = boost::shared_ptr<xmlXPathObject>(obj, xmlXPathFreeObject);
    m_doc = doc;
    return ok();
}

xmlXPathObjectType cXmlXPathResult::type() {
    if (!ok())
        return XPATH_UNDEFINED;
    return m_obj->type;
}

cXmlNode cXmlXPathResult::operator[](int index) const {
    if (!ok())
        return cXmlNode();
    if ((m_obj->type != XPATH_NODESET) || (index < 0) || (!m_obj->nodesetval) || (index >= m_obj->nodesetval->nodeNr))
        return cXmlNode();
    return cXmlNode(m_obj->nodesetval->nodeTab[index], m_doc);
}


int cXmlXPathResult::size() const {
    if (!ok())
        return 0;
    if ((m_obj->type != XPATH_NODESET) || (!m_obj->nodesetval))
        return 0;
    return m_obj->nodesetval->nodeNr;
}

cXmlXPathResult::operator vector<cXmlNode>() const {
    vector<cXmlNode> ret;
    if (size()) {
        for (int i = 0; i < size(); ++i)
            ret.push_back(operator[](i));
    }
    return ret;
}

} // Namespace end
