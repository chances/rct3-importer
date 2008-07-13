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

#include <map>
#include <stdio.h>
#include <string>

#include "LodSymRefManager.h"
#include "OVLClasses.h"
#include "RelocationManager.h"
#include "StringTable.h"

class cOvl;
class ovlOVLManager {
public:
    static const char* LOADER;
    static const unsigned long TYPE;
protected:
    unsigned long m_size;
    unsigned char* m_data;
    std::map<std::string, cOvlMemBlob> m_blobs;
    bool m_made;
    ovlOVLManager* m_defermake;
    bool m_deferable;

    virtual void Check(const std::string& err);
private:
    cOvl* m_ovl;
public:
    ovlOVLManager();
    virtual ~ovlOVLManager();

    virtual void Init(cOvl* ovl);

    void DeferMake(ovlOVLManager* man);

    bool IsMade() {
        return m_made;
    }

    virtual void Make(cOvlInfo* info) = 0;
    //virtual void WriteLoader(FILE* f);
    virtual void WriteLoader(std::fstream& f);

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
protected:
    ovlLodSymRefManager* GetLSRManager();
    ovlStringTable* GetStringTable();
    ovlRelocationManager* GetRelocationManager();
};

#endif
