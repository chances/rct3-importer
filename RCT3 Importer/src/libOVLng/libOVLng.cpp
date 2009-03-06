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


#include "OVLng.h"

#include "OVLDebug.h"

#ifdef __BORLANDC__
// Otherwise c++ refuses to build -.-
#define Library
#endif

using namespace std;

////////////////////////////////////////////////////////////////////////////////
//
//  cOVL class
//
////////////////////////////////////////////////////////////////////////////////

cOvl::cOvl(string file) {
    Init(file);
}

void cOvl::Init(string file) {
    m_file = file;
    m_init = true;

    //string temp = m_file + ".common.ovl";
    m_ovlinfo.OpenFiles[OVLT_COMMON].filename = m_file + ".common.ovl";
    //strcpy(m_ovlinfo.OpenFiles[OVL_COMMON].FileName, temp.c_str());
    //m_ovlinfo.OpenFiles[OVL_COMMON].ReferenceCount=0;
    //m_ovlinfo.OpenFiles[OVL_COMMON].References=NULL;
    //memset(m_ovlinfo.OpenFiles[OVL_COMMON].Types,0,sizeof(m_ovlinfo.OpenFiles[OVL_COMMON].Types));

    //temp = m_file + ".unique.ovl";
    m_ovlinfo.OpenFiles[OVLT_UNIQUE].filename = m_file + ".unique.ovl";
    //strcpy(m_ovlinfo.OpenFiles[OVL_UNIQUE].FileName, temp.c_str());
    //memset(m_ovlinfo.OpenFiles[OVL_UNIQUE].Types,0,sizeof(m_ovlinfo.OpenFiles[OVL_UNIQUE].Types));

    m_relmanager.Init(&m_ovlinfo);
    m_lsrmanager.Init(&m_relmanager, &m_stringtable);
}

cOvl::~cOvl() {
//    delete[] m_ovlinfo.OpenFiles[OVL_COMMON].FileName;
//    delete[] m_ovlinfo.OpenFiles[OVL_UNIQUE].FileName;
}

ovlOVLManager* cOvl::GetManager(const char* tag) {
    map<string, ovlOVLManager*>::iterator it = m_managers.find(string(tag));
    if (it != m_managers.end())
        return it->second;
    else
        return NULL;
}

void cOvl::InitAndAddManager(ovlOVLManager* man) {
    if (!man)
        BOOST_THROW_EXCEPTION(EOvl("cOvl::AddAndInitManager called with invalid manager"));

    man->Init(this);
    m_managers[man->Tag()] = man;
}

void cOvl::AddReference(const char* ref) {
    if (!m_init)
        BOOST_THROW_EXCEPTION(EOvl("cOvl::AddReference called uninitialized"));
    //m_references.push_back(string(ref));
    m_ovlinfo.OpenFiles[OVLT_UNIQUE].references.push_back(string(ref));
}

void cOvl::Save() {
    if (!m_init)
        BOOST_THROW_EXCEPTION(EOvl("cOvl::Save called uninitialized"));

    // First make the string table
    m_stringtable.Make(&m_ovlinfo);

    // Then assign space for the loaders, symbols and symbol references
    m_lsrmanager.Assign(&m_ovlinfo);

    // Now make all the managers' contents
    for (map<string, ovlOVLManager*>::iterator it = m_managers.begin(); it != m_managers.end(); ++it) {
        it->second->Make(&m_ovlinfo);
    }

    // Now we make the loaders, symbols and symbol references
    {
        map<string, unsigned long> c_loadertypes;
        unsigned long l = 0;
        for (map<string, ovlOVLManager*>::iterator it = m_managers.begin(); it != m_managers.end(); ++it) {
            DUMP_LOG("Assigning loader type %ld to %s", l, UNISTR(it->second->Tag()));
            c_loadertypes[string(it->second->Tag())] = l++;
        }

        m_lsrmanager.Make(c_loadertypes);
    }

    // First we assign the offsets so relocations can be set up correctly
    m_ovlinfo.MakeOffsets();

    // Then we relsolve the relocations
    m_relmanager.Make();

    // And write the files
    m_ovlinfo.WriteFiles(m_managers, m_lsrmanager);
}

