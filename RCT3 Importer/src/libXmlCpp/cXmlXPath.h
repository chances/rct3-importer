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

#ifndef CXMLXPATH_H
#define CXMLXPATH_H

#include "cxmlconfig.h"

#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <string>
#include <boost/shared_ptr.hpp>

#ifdef XMLCPP_USE_WXWIDGETS
#include <wx/string.h>
#endif

#include "cXmlInitHandler.h"
#include "cXmlClasses.h"
#include "cXmlNode.h"
#include "cXmlXPathResult.h"

namespace xmlcpp {

/// Wraps an xmlXPathContext structure
class cXmlXPath: private cXmlInitHandler {
public:
    cXmlXPath() { Init(); }
    explicit cXmlXPath(const boost::shared_ptr<xmlDoc>& doc, const std::string& prefix = "", const std::string& uri = "");
    explicit cXmlXPath(const cXmlDoc& doc, const std::string& prefix = "", const std::string& uri = "");
    explicit cXmlXPath(xmlDocPtr doc, const std::string& prefix = "", const std::string& uri = "");
    explicit cXmlXPath(xmlDocPtr doc, bool grab, const std::string& prefix = "", const std::string& uri = "");
    virtual ~cXmlXPath();

    bool assign(const boost::shared_ptr<xmlDoc>& doc);
    bool assign(const cXmlDoc& doc);
    bool assign(xmlDocPtr doc);
    bool assign(xmlDocPtr doc, bool grab);

    bool registerNs(const std::string& prefix, const std::string& uri);

    inline void setNodeContext(const cXmlNode& node) { setNodeContext(node.getRaw()); }
    void setNodeContext(const xmlNodePtr node);

    inline cXmlXPathResult eval(const std::string& query) { return eval(query.c_str()); }
    inline cXmlXPathResult evaluate(const std::string& query) { return evaluate(query.c_str()); }
    inline cXmlXPathResult operator()(const std::string& query, bool use_eval = false) { return operator()(query.c_str(), use_eval); }
    cXmlXPathResult eval(const char* query);
    cXmlXPathResult evaluate(const char* query);
    cXmlXPathResult operator()(const char* query, bool use_eval = false);

    inline bool ok() const { return m_context.get(); }
    inline bool operator!() const { return !ok(); }
    typedef xmlDocPtr cXmlXPath::*unspecified_bool_type;
    inline operator unspecified_bool_type() const { return ok()?(&cXmlXPath::m_voldoc):NULL; }

    static std::string decorateWithNsPrefix(const std::string& path, const std::string& prefix, bool make_first = false);
protected:
private:
    boost::shared_ptr<xmlXPathContext> m_context;
    /// Shared ponter to the xmlDoc that owns the property
    /**
     * This keeps the document alive as long as there are users for
     * one of its contents around.
     */
    boost::shared_ptr<xmlDoc> m_doc;
    xmlDocPtr m_voldoc;

    inline xmlDocPtr docptr() { return (m_voldoc)?m_voldoc:m_doc.get(); }

    void Init();
    void create();
};

}


#endif // CXMLNODE_H
