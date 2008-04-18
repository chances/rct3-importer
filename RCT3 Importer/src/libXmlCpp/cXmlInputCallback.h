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

#ifndef CXMLINPUTCALLBACK_H
#define CXMLINPUTCALLBACK_H

#include "cxmlconfig.h"

#include <deque>
#include <set>
#include <vector>

#include "cXmlInitHandler.h"

namespace xmlcpp {

/// Abstract wrapper for libxml2's input callback support
/**
 * IMPORTANT: All subclasses need to inherit from cXmlInitHandler,
 * otherwise hell will break loose if all other cXmlInitHandler subclasses
 * are destroyed.
 * cXmlInputCallback itself cannot inherit from cXmlInitHandler to allow
 * subclasses to inherit from cXmlOutputCallback as well.
 */
class cXmlInputCallback {
private:
    struct cXmlInputCallbackContext {
        cXmlInputCallback* owner;
        unsigned long id;

        cXmlInputCallbackContext(cXmlInputCallback* _owner, unsigned long _id):owner(_owner), id(_id){};
        inline void close() {
            owner->close_in(id);
        }
        inline int read(char* buffer, int len) {
            return owner->read(id, buffer, len);
        }
    };
public:
    cXmlInputCallback();
    virtual ~cXmlInputCallback();
protected:
    virtual bool match_in(const char* filename) = 0;
    virtual unsigned long open_in(const char* filename) = 0;
    virtual void close_in(unsigned long id) = 0;
    virtual int read(unsigned long id, char* buffer, int len) = 0;

private:
    static std::deque<cXmlInputCallback*> g_handlers;
    static std::set<cXmlInputCallbackContext*> g_contexts;
    static void reg_in(cXmlInputCallback* handler);
    static void unreg_in(cXmlInputCallback* handler);

    static int g_regid;
    static int matchfunc_in(const char* filename);
    static void* openfunc_in(char const * filename);
    static int closefunc_in(void * context);
    static int readfunc(void * context, char * buffer, int len);
};

} // Namespace


#endif // CXMLINPUTCALLBACK_H
