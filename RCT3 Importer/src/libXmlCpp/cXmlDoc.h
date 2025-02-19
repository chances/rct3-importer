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

#ifndef CXMLDOC_H_INCLUDED
#define CXMLDOC_H_INCLUDED

#include "cxmlconfig.h"

#ifdef XMLCPP_USE_WXWIDGETS
#include <wx/string.h>
#endif

#include <string>
#include <vector>
#include <libxml/parser.h>
#include <boost/shared_ptr.hpp>

#include "cXmlClasses.h"
#include "cXmlInitHandler.h"
#include "cXmlErrorHandler.h"
#include "cXmlValidatorResult.h"

namespace xmlcpp {

    class cXmlDoc: private cXmlInitHandler, public cXmlErrorHandler  {
//    friend void errorwrap(void* ptr, xmlErrorPtr err);
    private:
        boost::shared_ptr<xmlDoc> m_doc;
        std::string m_file;

        void Init();
//        void DeInit();
        inline void assignDoc(xmlDocPtr newdoc) {
            m_doc.reset(newdoc, xmlFreeDoc);
        }

    public:
        cXmlDoc();
        explicit cXmlDoc(bool asempty);
        explicit cXmlDoc(xmlDocPtr doc, bool grab);
        explicit cXmlDoc(const boost::shared_ptr<xmlDoc>& sharedoc);
        explicit cXmlDoc(const char* filename, const char* encoding = NULL, int options = 0);
        explicit cXmlDoc(const std::string& buffer, const char* URL = NULL, const char* encoding = NULL, int options = 0);

        virtual ~cXmlDoc();

        void empty(const char* xmlver = "1.0");

        bool read(const xmlChar* cur, const char* URL, const char* encoding, int options = 0);
        inline bool read(const std::string& buffer, const char* URL = NULL, const char* encoding = NULL, int options = 0) {
            return read(reinterpret_cast<const xmlChar*>(buffer.c_str()), URL, encoding, options);
        }
        bool read(const char* filename, const char* encoding, int options = 0);
        /// Clones a document tree from a pointer
        bool read(xmlDocPtr doc);

        /// Share the document from another source
        bool share(const boost::shared_ptr<xmlDoc>& doc);


        std::string write(bool indent = false, const char* encoding = NULL);
        int write(const char* filename, bool indent = false, const char* encoding = NULL);
        int dump(FILE* f);

        cXmlNode root();
        cXmlNode root(cXmlNode& newroot);

        std::string searchNs(const std::string& prefix = "", xmlNodePtr node = NULL);
        xmlNsPtr searchNsByHref(const std::string& href, xmlNodePtr node = NULL);
        xmlNsPtr getNs(const std::string& prefix = "", xmlNodePtr node = NULL);
        inline xmlNsPtr defaultNs(xmlNodePtr node = NULL) { return getNs("", node); }

        void setName(const char* name);
        inline const char* getName() const { return ok()?m_doc->name:NULL; }
        void setURL(const char* URL);
        inline const char* getURL() const { return ok()?reinterpret_cast<const char*>(m_doc->URL):NULL; }

        /// Validate the document with a validator
        /**
         * Returns 0 on success, -1 on internal error and number of errors otherwise.
         */
        cXmlValidatorResult validate(cXmlValidator& val, cXmlValidatorResult::LEVEL deflevel = cXmlValidatorResult::VR_ERROR);
        cXmlValidatorResult validate(cXmlValidator& val, int options, cXmlValidatorResult::LEVEL deflevel = cXmlValidatorResult::VR_ERROR);
        int xInclude();

        inline bool ok() const { return m_doc.get(); }
        inline bool operator!() const { return !ok(); }
        typedef std::string cXmlDoc::*unspecified_bool_type;
        inline operator unspecified_bool_type() const { return ok()?(&cXmlDoc::m_file):NULL; }

        inline xmlDocPtr getRaw() { return m_doc.get(); }
        inline const xmlDocPtr getRaw() const { return m_doc.get(); }
        inline boost::shared_ptr<xmlDoc>& get() { return m_doc; }
        inline const boost::shared_ptr<xmlDoc>& get() const { return m_doc; }

#ifdef XMLCPP_USE_WXWIDGETS
        inline bool read(const wxString& filename, const wxString& encoding = wxT(""), int options = 0) { return read(filename.utf8_str(), encoding.IsEmpty()?NULL:static_cast<const char*>(encoding.utf8_str()), options); }
        int write(const wxString& filename, bool indent = false, const wxString& encoding = wxString()) {
            return write(static_cast<const char*>(filename.utf8_str()), indent, encoding.IsSameAs(wxString())?NULL:static_cast<const char*>(encoding.utf8_str()));
        }
        inline wxString wxwrite(bool indent = false) {
            return wxString::FromUTF8(write(indent, "UTF-8").c_str());
        }
#endif
    };

} // Namespace

#endif // CXMLDOC_H_INCLUDED
