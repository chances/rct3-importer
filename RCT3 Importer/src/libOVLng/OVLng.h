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

/// Ovl creation class
/**
 * This class is responsible for creating ovl files. It needs to be initialized before using by either calling the Init
 * method or using the cOvl(std::string file) constructor. Data is added by requesting manager instances via the
 * GetManager() template method and adding data to them. Finally the ovl is created/saved via the Save() method.
 */

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
    /// Non-initializing default constructor.
    cOvl() {
        m_init = false;
    };
    /// Initializing constructor.
    /**
     * Creates a cOvl instance and initializes it.
     * @param file Ovl base file name.
     * @see Init()
     */
    cOvl(string file);
    virtual ~cOvl();

    /// Initialize a cOvl instance
    /**
     * Initializes a cOvl instance created via the default constructor.
     * @param file Ovl base file name (without .common.ovl or .unique.ovl extension).
     * @see cOvl()
     */
    void Init(string file);

    /// Add a reference to the ovl.
    void AddReference(const char* ref);
    /// Add a simple data symol to the ovl.
    void AddDataSymbol(cOvlType type, const char *symbol, unsigned long data) {
        if (!m_init)
            throw EOvl("cOvl::AddDataSymbol called uninitialized");
        m_lsrmanager.AddSymbol(type, symbol, data);
    }

    /// Create and save the ovl files.
    void Save();

    /// Request a data manager pointer.
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

    /// Detect whether a certain manager is present.
    template <class M>
    bool HasManager() {
        return m_managers.find(M::TAG) != m_managers.end();
    }
};

#endif
