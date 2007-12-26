///////////////////////////////////////////////////////////////////////////////
//
// New OVL creation library
// Manager class for TXT structures
// Copyright (C) 2007 Tobias Minch
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
// Written by
//   Tobias Minich - belgabor@gmx.de
//
// Based on libOVL by Jonathan Wilson
//
///////////////////////////////////////////////////////////////////////////////


#ifndef MANAGERTXT_H_INCLUDED
#define MANAGERTXT_H_INCLUDED

#include <string>
#include <vector>

#include "counted_array_ptr.h"

#include "ManagerOVL.h"

using namespace std;

class ovlTXTManager: public ovlOVLManager {
public:
    static const char* LOADER;
    static const char* NAME;
    static const char* TAG;
    static const unsigned long TYPE;
private:
    map<string, counted_array_ptr<wchar_t> > m_items;
public:
    ovlTXTManager(): ovlOVLManager() {};
    virtual ~ovlTXTManager(){};

    void AddText(const char* name, const char* str);
    void AddText(const char* name, const wchar_t* str);

    virtual void Make(cOvlInfo* info);

    virtual const char* Loader() const {
        return LOADER;
    };
    virtual const char* Name() const {
        return NAME;
    };
    virtual const char* Tag() const {
        return TAG;
    };
    virtual const unsigned long Type() const {
        return TYPE;
    };
};

#endif
