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

#include "cXmlInputCallback.h"

#include <libxml/xmlIO.h>

#include "cXmlException.h"

#define DUMP
//#define DUMPINIT

using namespace std;

namespace xmlcpp {

int cXmlInputCallback::g_regid = -1;
deque<cXmlInputCallback*> cXmlInputCallback::g_handlers;
set<cXmlInputCallback::cXmlInputCallbackContext*> cXmlInputCallback::g_contexts;

cXmlInputCallback::cXmlInputCallback() {
#ifdef DUMPINIT
fprintf(stderr, "cXmlInputCallback()\n");
fflush(stderr);
#endif
    reg_in(this);
}

cXmlInputCallback::~cXmlInputCallback() {
#ifdef DUMPINIT
fprintf(stderr, "~cXmlInputCallback() / ");
fflush(stderr);
#endif
    unreg_in(this);
}


void cXmlInputCallback::reg_in(cXmlInputCallback* handler) {
    if (g_regid == -1) {
        g_regid = xmlRegisterInputCallbacks(cXmlInputCallback::matchfunc_in, cXmlInputCallback::openfunc_in,
                                            cXmlInputCallback::readfunc, cXmlInputCallback::closefunc_in);
        if (g_regid == -1)
            throw eXml("Failed to register callbacks for cXmlInputCallback.");
#ifdef DUMP
fprintf(stderr, "Register cXmlInputCallback\n");
fflush(stderr);
#endif
    }
    g_handlers.push_front(handler);
}

void cXmlInputCallback::unreg_in(cXmlInputCallback* handler) {
    if (g_contexts.size()) {
        vector<cXmlInputCallbackContext*> temp;
        for (set<cXmlInputCallbackContext*>::iterator it = g_contexts.begin(); it != g_contexts.end(); ++it) {
            if ((*it)->owner == handler)
                temp.push_back(*it);
        }
        for (vector<cXmlInputCallbackContext*>::iterator it = temp.begin(); it != temp.end(); ++it) {
            cXmlInputCallbackContext* c = *it;
            g_contexts.erase(c);
            delete c;
        }
    }
    for (deque<cXmlInputCallback*>::iterator it = g_handlers.begin(); it != g_handlers.end(); ++it) {
        if ((*it) == handler) {
            g_handlers.erase(it);
            break;
        }
    }
    if (!g_handlers.size()) {
        int regid = xmlPopInputCallbacks();
        if (regid != g_regid)
            throw eXml("Failed to unregister callbacks for cXmlInputCallback.");
        g_regid = -1;
    }
}

int cXmlInputCallback::matchfunc_in(const char* filename) {
#ifdef DUMP
fprintf(stderr, "matchfunc_in %s\n", filename);
fflush(stderr);
#endif
    for (deque<cXmlInputCallback*>::iterator it = g_handlers.begin(); it != g_handlers.end(); ++it) {
        if ((*it)->match_in(filename)) {
            return 1;
        }
    }
    return 0;
}

void* cXmlInputCallback::openfunc_in(const char* filename) {
#ifdef DUMP
fprintf(stderr, "openfunc_in %s\n", filename);
fflush(stderr);
#endif
    for (deque<cXmlInputCallback*>::iterator it = g_handlers.begin(); it != g_handlers.end(); ++it) {
        if ((*it)->match_in(filename)) {
            unsigned long id = (*it)->open_in(filename);
            if (id != -1) {
                cXmlInputCallbackContext* c = new cXmlInputCallbackContext(*it, id);
                g_contexts.insert(c);
                return c;
            } // else give other handlers a chance
        }
    }
    return NULL;
}

int cXmlInputCallback::closefunc_in(void* context) {
    if (g_contexts.find(reinterpret_cast<cXmlInputCallbackContext*>(context)) != g_contexts.end()) {
        reinterpret_cast<cXmlInputCallbackContext*>(context)->close();
        g_contexts.erase(reinterpret_cast<cXmlInputCallbackContext*>(context));
        delete reinterpret_cast<cXmlInputCallbackContext*>(context);
        return 0;
    } else {
        return -1;
    }
}

int cXmlInputCallback::readfunc(void* context, char * buffer, int len) {
    if (g_contexts.find(reinterpret_cast<cXmlInputCallbackContext*>(context)) != g_contexts.end()) {
        return reinterpret_cast<cXmlInputCallbackContext*>(context)->read(buffer, len);
    } else {
        return -1;
    }
}

} // Namespace

