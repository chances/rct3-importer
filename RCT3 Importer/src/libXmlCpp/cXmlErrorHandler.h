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

#ifndef CXMLERRORHANDLER_H_INCLUDED
#define CXMLERRORHANDLER_H_INCLUDED

#include "cxmlconfig.h"

#include <string>
#include <vector>
#include <libxml/xmlerror.h>

namespace xmlcpp {

void structurederrorwrap(void* ptr, xmlErrorPtr err);
void genericerrorwrap(void* ptr, const char* err, ...);

struct cXmlStructuredError {
    int             domain;
    int             code;
    std::string     message;
    xmlErrorLevel   level;
    std::string     file;
    int             line;
    std::string     str1;
    std::string     str2;
    std::string     str3;
    int             int1;
    int             int2;
    void*           ctxt;
    void*           node;
    std::string     path;
    std::string     attribute; ///< Error is about an attribute in a node. Used for full path calculation

    cXmlStructuredError(xmlErrorPtr error = NULL);
    std::string getPath() const;
};

class cXmlErrorHandler {
friend void structurederrorwrap(void* ptr, xmlErrorPtr err);
friend void genericerrorwrap(void* ptr, const char* err, ...);
friend class cXmlInitHandler;
private:
    std::vector<cXmlStructuredError> m_structurederrors;
    std::vector<std::string> m_genericerrors;
    static std::vector<cXmlStructuredError> g_structurederrors;
    static std::vector<std::string> g_genericerrors;

    static cXmlErrorHandler* g_claimed;
    static void global_structurederrorcallback(xmlErrorPtr err);
    static void global_genericerrorcallback(const std::string& err);
    static void Init();
protected:
    virtual void structurederrorcallback(xmlErrorPtr err);
    virtual void genericerrorcallback(const std::string& err);

    void claimErrors();
    void releaseErrors();
    void transferErrors(cXmlErrorHandler& from);
    inline void addStructuredError(const cXmlStructuredError& err) { m_structurederrors.push_back(err); }
public:
    inline const std::vector<cXmlStructuredError>& getStructuredErrors() { return m_structurederrors; }
    inline void clearStructuredErrors() { m_structurederrors.clear(); }
    inline const std::vector<std::string>& getGenericErrors() { return m_genericerrors; }
    inline void clearGenericErrors() { m_genericerrors.clear(); }

    inline static const std::vector<cXmlStructuredError>& GetGlobalStructuredErrors() { return g_structurederrors; }
    inline static void ClearGlobalStructuredErrors() { g_structurederrors.clear(); }
    inline static const std::vector<std::string>& GetGlobalGenericErrors() { return g_genericerrors; }
    inline static void ClearGlobalGenericErrors() { g_genericerrors.clear(); }

    virtual ~cXmlErrorHandler() {}
};

} // Namespace

#endif // CXMLERRORHANDLER_H_INCLUDED
