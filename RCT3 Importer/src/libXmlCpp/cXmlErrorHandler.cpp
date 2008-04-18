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


#include "cXmlErrorHandler.h"

#include <sstream>

#include "cXmlException.h"
#include "cXmlNode.h"

//#define DUMP
//#define DUMPINIT

using namespace std;

namespace xmlcpp {

vector<cXmlStructuredError> cXmlErrorHandler::g_structurederrors;
vector<string> cXmlErrorHandler::g_genericerrors;
cXmlErrorHandler* cXmlErrorHandler::g_claimed = NULL;


void genericerrorwrap(void* ptr, const char* err, ...) {
#ifdef DUMP
fprintf(stderr, "genericerrorwrap\n");
fflush(stderr);
#endif
    va_list argp;
    char str[4096];
	va_start(argp, err);
	vsnprintf(str, 4095, err, argp);
	va_end(argp);

	string error = str;

    if (ptr) {
        static_cast<cXmlErrorHandler*>(ptr)->genericerrorcallback(error);
    } else {
        cXmlErrorHandler::global_genericerrorcallback(error);
    }
}

void structurederrorwrap(void* ptr, xmlErrorPtr err) {
#ifdef DUMP
fprintf(stderr, "structurederrorwrap\n");
fflush(stderr);
#endif
    if (ptr) {
        static_cast<cXmlErrorHandler*>(ptr)->structurederrorcallback(err);
    } else {
        cXmlErrorHandler::global_structurederrorcallback(err);
    }
}

#define DO_XMLERR(a) \
    if (error->a) \
        a = error->a

cXmlStructuredError::cXmlStructuredError(xmlErrorPtr error) {
    domain = 0;
    code = 0;
    level = XML_ERR_NONE;
    line = 0;
    int1 = 0;
    int2 = 0;
    ctxt = NULL;
    node = NULL;
#ifdef DUMP
fprintf(stderr, "Create error\n");
fflush(stderr);
#endif
    if (error) {
#ifdef DUMP
fprintf(stderr, "Create error %d %s\n", error->line, error->message);
fflush(stderr);
#endif
        domain = error->domain;
        code = error->code;
        DO_XMLERR(message);
        level = error->level;
        DO_XMLERR(file);
        line = error->line;
        DO_XMLERR(str1);
        DO_XMLERR(str2);
        DO_XMLERR(str3);
        int1 = error->int1;
        int2 = error->int2;
        ctxt = error->ctxt;
        node = error->node;
    }
}

string cXmlStructuredError::getPath() const {
    if (path != "")
        return path;
    if (!node)
        return "";
    cXmlNode temp(reinterpret_cast<xmlNodePtr>(node));
    if (!temp.ok())
        return "";
    std::string ret = temp.path();
    if (attribute != "")
        ret += "/@"+attribute;
    return ret;
}

void cXmlErrorHandler::Init() {
#ifdef DUMP
fprintf(stderr, "Errorhandler Init\n");
fflush(stderr);
#endif
    xmlSetGenericErrorFunc(0, genericerrorwrap);
    xmlSetStructuredErrorFunc(0, structurederrorwrap);
    g_structurederrors.clear();
    g_genericerrors.clear();
    g_claimed = NULL;
}

void cXmlErrorHandler::global_structurederrorcallback(xmlErrorPtr err) {
#ifdef DUMP
fprintf(stderr, "global structerror\n");
fflush(stderr);
#endif
    if (g_claimed) {
        g_claimed->structurederrorcallback(err);
    } else {
        g_structurederrors.push_back(cXmlStructuredError(err));
    }
}

void cXmlErrorHandler::global_genericerrorcallback(const string& err) {
    if (g_claimed) {
        g_claimed->genericerrorcallback(err);
    } else {
        g_genericerrors.push_back(err);
    }
}

void cXmlErrorHandler::structurederrorcallback(xmlErrorPtr err) {
#ifdef DUMP
fprintf(stderr, "local structerror\n");
fflush(stderr);
#endif
    m_structurederrors.push_back(cXmlStructuredError(err));
}

void cXmlErrorHandler::genericerrorcallback(const string& err) {
    m_genericerrors.push_back(err);
}

void cXmlErrorHandler::claimErrors() {
#ifdef DUMP
fprintf(stderr, "claim errors\n");
fflush(stderr);
#endif
    if (g_claimed)
        throw eXml("Errors already claimed");
    g_claimed = this;
}

void cXmlErrorHandler::releaseErrors() {
#ifdef DUMP
fprintf(stderr, "release errors\n");
fflush(stderr);
#endif
    g_claimed = NULL;
}

void cXmlErrorHandler::transferErrors(cXmlErrorHandler& from) {
    if (this != &from) {
        m_structurederrors.insert(m_structurederrors.end(), from.m_structurederrors.begin(), from.m_structurederrors.end());
        m_genericerrors.insert(m_genericerrors.end(), from.m_genericerrors.begin(), from.m_genericerrors.end());
    }
}

} // Namespace

