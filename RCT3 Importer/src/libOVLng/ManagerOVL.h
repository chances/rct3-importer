///////////////////////////////////////////////////////////////////////////////
//
// New OVL creation library
// Base manager class for structures
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


#ifndef MANAGEROVL_H_INCLUDED
#define MANAGEROVL_H_INCLUDED

#include <string>

#include "LodSymRefManager.h"
#include "RelocationManager.h"
#include "StringTable.h"

using namespace std;

class ovlOVLManager {
public:
    static const char* LOADER;
    static const unsigned long TYPE;
protected:
    unsigned long m_size;
    unsigned char* m_data;
    ovlLodSymRefManager* m_lsrman;
    ovlRelocationManager* m_relman;
    ovlStringTable* m_stable;

    virtual void Check(const string& err);
public:
    ovlOVLManager();
    virtual ~ovlOVLManager();

    virtual void Init(ovlLodSymRefManager* lsrman, ovlRelocationManager* relman, ovlStringTable* stable);

    virtual unsigned char* Make() = 0;
    virtual void WriteLoader(FILE* f);

    virtual const unsigned long GetSize() const;
    virtual unsigned char* GetData();
    virtual const char* Loader() const {
        return LOADER;
    };
    virtual const char* Name() const = 0;
    virtual const char* Tag() const = 0;
    virtual const unsigned long Type() const {
        return TYPE;
    };

    //static ovlOVLManager* MakeManager(const char* tag);
};

#endif
