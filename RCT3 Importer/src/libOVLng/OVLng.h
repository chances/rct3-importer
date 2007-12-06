///////////////////////////////////////////////////////////////////////////////
//
// New OVL creation library
// OVL creation class
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


#ifndef OVLNG_H_INCLUDED
#define OVLNG_H_INCLUDED

#include <map>
#include <string>

#include "LodSymRefManager.h"
#include "ManagerOVL.h"
#include "ovlstructs.h"
#include "RelocationManager.h"
#include "StringTable.h"

using namespace std;

class cOvl {
private:
    string m_file;
    cOvlInfo m_ovlinfo;
    ovlStringTable m_stringtable;
    ovlRelocationManager m_relmanager;
    ovlLodSymRefManager m_lsrmanager;

    map<string, ovlOVLManager*> m_managers;

    ovlOVLManager* GetManager(const char* tag);
    void InitAndAddManager(ovlOVLManager* man);
public:
    cOvl(string file);
    virtual ~cOvl();

    void AddReference(const char* ref);

    void Save();

    template <class M>
    M* GetManager() {
        M* ret = dynamic_cast<M*>(GetManager(M::TAG));
        if (!ret) {
            ret = new M();
            InitAndAddManager(ret);
        }
        return ret;
    }
};

#endif
