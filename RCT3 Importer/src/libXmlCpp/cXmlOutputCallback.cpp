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

#include "cXmlOutputCallback.h"

#include <libxml/xmlio.h>

#include "cXmlException.h"

//#define DUMP
//#define DUMPINIT

using namespace std;

namespace xmlcpp {

int cXmlOutputCallback::g_regid = -1;
deque<cXmlOutputCallback*> cXmlOutputCallback::g_handlers;
set<cXmlOutputCallback::cXmlOutputCallbackContext*> cXmlOutputCallback::g_contexts;

cXmlOutputCallback::cXmlOutputCallback() {
#ifdef DUMPINIT
fprintf(stderr, "cXmlOutputCallback()\n");
fflush(stderr);
#endif
    reg_out(this);
}

cXmlOutputCallback::~cXmlOutputCallback() {
#ifdef DUMPINIT
fprintf(stderr, "~cXmlOutputCallback() / ");
fflush(stderr);
#endif
    unreg_out(this);
}


void cXmlOutputCallback::reg_out(cXmlOutputCallback* handler) {
    if (g_regid == -1) {
        g_regid = xmlRegisterOutputCallbacks(cXmlOutputCallback::matchfunc_out, cXmlOutputCallback::openfunc_out,
                                             cXmlOutputCallback::writefunc, cXmlOutputCallback::closefunc_out);
        if (g_regid == -1)
            throw eXml("Failed to register callbacks for cXmlOutputCallback.");
#ifdef DUMP
fprintf(stderr, "Register cXmlOutputCallback\n");
#endif
    }
    g_handlers.push_front(handler);
}

void cXmlOutputCallback::unreg_out(cXmlOutputCallback* handler) {
    if (g_contexts.size()) {
        vector<cXmlOutputCallbackContext*> temp;
        for (set<cXmlOutputCallbackContext*>::iterator it = g_contexts.begin(); it != g_contexts.end(); ++it) {
            if ((*it)->owner == handler)
                temp.push_back(*it);
        }
        for (vector<cXmlOutputCallbackContext*>::iterator it = temp.begin(); it != temp.end(); ++it) {
            cXmlOutputCallbackContext* c = *it;
            g_contexts.erase(c);
            delete c;
        }
    }
    for (deque<cXmlOutputCallback*>::iterator it = g_handlers.begin(); it != g_handlers.end(); ++it) {
        if ((*it) == handler) {
            g_handlers.erase(it);
            break;
        }
    }
//    if (!g_handlers.size()) {
//        int regid = xmlPopOutputCallbacks();
//        if (regid != g_regid)
//            throw eXml("Failed to unregister callbacks for cXmlOutputCallback.");
//        g_regid = -1;
//    }
}

int cXmlOutputCallback::matchfunc_out(const char* filename) {
#ifdef DUMP
fprintf(stderr, "matchfunc_out %s\n", filename);
#endif
    for (deque<cXmlOutputCallback*>::iterator it = g_handlers.begin(); it != g_handlers.end(); ++it) {
        if ((*it)->match_out(filename)) {
            return 1;
        }
    }
    return 0;
}

void* cXmlOutputCallback::openfunc_out(const char* filename) {
#ifdef DUMP
fprintf(stderr, "openfunc_out %s\n", filename);
#endif
    for (deque<cXmlOutputCallback*>::iterator it = g_handlers.begin(); it != g_handlers.end(); ++it) {
        if ((*it)->match_out(filename)) {
            unsigned long id = (*it)->open_out(filename);
            if (id != -1) {
                cXmlOutputCallbackContext* c = new cXmlOutputCallbackContext(*it, id);
                g_contexts.insert(c);
                return c;
            } // else give other handlers a chance
        }
    }
    return NULL;
}

int cXmlOutputCallback::closefunc_out(void* context) {
    if (g_contexts.find(reinterpret_cast<cXmlOutputCallbackContext*>(context)) != g_contexts.end()) {
        reinterpret_cast<cXmlOutputCallbackContext*>(context)->close();
        g_contexts.erase(reinterpret_cast<cXmlOutputCallbackContext*>(context));
        delete reinterpret_cast<cXmlOutputCallbackContext*>(context);
        return 0;
    } else {
        return -1;
    }
}

int cXmlOutputCallback::writefunc(void* context, const char * buffer, int len) {
    if (g_contexts.find(reinterpret_cast<cXmlOutputCallbackContext*>(context)) != g_contexts.end()) {
        return reinterpret_cast<cXmlOutputCallbackContext*>(context)->write(buffer, len);
    } else {
        return -1;
    }
}

} // Namespace

