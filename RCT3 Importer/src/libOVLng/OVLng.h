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
#include "OVLException.h"

using namespace std;

class cOvl {
friend class ovlOVLManager;
private:
    string m_file;
    cOvlInfo m_ovlinfo;
    ovlStringTable m_stringtable;
    ovlRelocationManager m_relmanager;
    ovlLodSymRefManager m_lsrmanager;

    map<string, ovlOVLManager*> m_managers;

    bool m_init;

    ovlOVLManager* GetManager(const char* tag);
    void InitAndAddManager(ovlOVLManager* man);
public:
    cOvl(){
        m_init = false;
    };
    cOvl(string file);
    virtual ~cOvl();

    void Init(string file);

    void AddReference(const char* ref);
    void AddDataSymbol(cOvlType type, const char *symbol, unsigned long data) {
        if (!m_init)
            throw EOvl("cOvl::AddDataSymbol called uninitialized");
        m_lsrmanager.AddSymbol(type, symbol, data);
    }

    void Save();

    template <class M>
    M* GetManager() {
        if (!m_init)
            throw EOvl("cOvl::GetManager called uninitialized");
        M* ret = dynamic_cast<M*>(GetManager(M::TAG));
        if (!ret) {
            ret = new M();
            InitAndAddManager(ret);
        }
        return ret;
    }
};

#endif
