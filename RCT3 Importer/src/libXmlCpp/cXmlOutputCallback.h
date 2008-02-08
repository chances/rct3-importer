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

#ifndef CXMLOUTPUTCALLBACK_H
#define CXMLOUTPUTCALLBACK_H

#include <deque>
#include <set>
#include <vector>

#include "cXmlInitHandler.h"

namespace xmlcpp {

/// Abstract wrapper for libxml2's output callback support
/**
 * IMPORTANT: All subclasses need to inherit from cXmlInitHandler,
 * otherwise hell will break loose if all other cXmlInitHandler subclasses
 * are destroyed.
 * cXmlOutputCallback itself cannot inherit from cXmlInitHandler to allow
 * subclasses to inherit from cXmlInputCallback as well.
 */
class cXmlOutputCallback {
private:
    struct cXmlOutputCallbackContext {
        cXmlOutputCallback* owner;
        unsigned long id;

        cXmlOutputCallbackContext(cXmlOutputCallback* _owner, unsigned long _id):owner(_owner), id(_id){};
        inline void close() {
            owner->close_out(id);
        }
        inline int write(const char* buffer, int len) {
            return owner->write(id, buffer, len);
        }
    };
public:
    cXmlOutputCallback();
    virtual ~cXmlOutputCallback();
protected:
    virtual bool match_out(const char* filename) = 0;
    virtual unsigned long open_out(const char* filename) = 0;
    virtual void close_out(unsigned long id) = 0;
    virtual int write(unsigned long id, const char* buffer, int len) = 0;

private:
    static std::deque<cXmlOutputCallback*> g_handlers;
    static std::set<cXmlOutputCallbackContext*> g_contexts;
    static void reg_out(cXmlOutputCallback* handler);
    static void unreg_out(cXmlOutputCallback* handler);

    static int g_regid;
    static int matchfunc_out(const char* filename);
    static void* openfunc_out(char const * filename);
    static int closefunc_out(void * context);
    static int writefunc(void * context, const char * buffer, int len);
};

} // Namespace


#endif // CXMLINPUTCALLBACK_H
