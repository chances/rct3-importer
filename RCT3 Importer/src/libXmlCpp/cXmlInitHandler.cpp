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

#include "cXmlInitHandler.h"

#include <libxml/parser.h>
#ifdef XMLCPP_USE_XSLT
#include <libxslt/extra.h>
#include <libxslt/xslt.h>
#endif

#ifdef XMLCPP_USE_EXSLT
#include <libexslt/exslt.h>
#endif

#include "cXmlErrorHandler.h"

//#define DUMP
//#define DUMPINIT
//#define DUMPBASEINIT

using namespace std;

namespace xmlcpp {

int cXmlInitHandler::g_use = 0;
int cXmlInitHandler::g_singletoncount = 0;
set<cXmlInitHandler*> cXmlInitHandler::g_singletons;

cXmlInitHandler::cXmlInitHandler():m_singleton(false) {
    if (!g_use) {
        LIBXML_TEST_VERSION
        xmlInitParser();
        xmlKeepBlanksDefault(0);
        xmlSubstituteEntitiesDefault(1); // For xslt
        xmlLoadExtDtdDefaultValue = 1;
#ifdef XMLCPP_USE_XSLT
        xsltInit();
        xsltRegisterAllExtras();
#endif
#ifdef XMLCPP_USE_EXSLT
        exsltRegisterAll();
#endif
#ifdef DUMPBASEINIT
fprintf(stderr, ".oO XML INIT Oo.\n");
#endif
        cXmlErrorHandler::Init();
    }
    g_use++;
#ifdef DUMPINIT
fprintf(stderr, "cXmlInitHandler() usage %d\n", g_use);
#endif
}

cXmlInitHandler::~cXmlInitHandler() {
    g_use--;
#ifdef DUMPINIT
fprintf(stderr, "~cXmlInitHandler() usage %d, singletons %d\n", g_use, g_singletons.size());
#endif
    if (m_singleton) {
        g_singletons.erase(this);
        g_singletoncount--;
    }
    if (!g_use) {
        xmlCleanupParser();
#ifdef XMLCPP_USE_XSLT
        xsltCleanupGlobals();
#endif
#ifdef DUMPBASEINIT
fprintf(stderr, ".oO XML CLEANUP Oo.\n");
        xmlMemoryDump();
#endif
    }
    if (g_use && (g_use == g_singletoncount)) {
        // Start cleaning up singletons
#ifdef DUMPINIT
fprintf(stderr, "Singleton cleanup initiated.\n");
#endif
        delete *g_singletons.begin();
    }
}

void cXmlInitHandler::make_singleton() {
#ifdef DUMPINIT
fprintf(stderr, "cXmlInitHandler, new singleton 0x%8p\n", this);
#endif
    if (!m_singleton) {
        g_singletons.insert(this);
        g_singletoncount++;
        m_singleton = true;
    }
}

} // Namespace
