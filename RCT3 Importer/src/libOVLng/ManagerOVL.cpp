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

#include "OVLDebug.h"

#include <fstream>

#include "ManagerOVL.h"

#include "OVLException.h"
#include "OVLng.h"

using namespace std;
using namespace r3;

const char* ovlOVLManager::LOADER = "RCT3";
const unsigned long ovlOVLManager::TYPE = 1;

ovlOVLManager::ovlOVLManager() {
    m_data = NULL;
    m_size = 0;
    m_ovl = NULL;
    m_defermake = NULL;
    m_deferable = false;
}

void ovlOVLManager::Init(cOvl* ovl) {
    m_ovl = ovl;
    m_made = false;
}

void ovlOVLManager::DeferMake(ovlOVLManager* man) {
    if (!m_deferable)
        BOOST_THROW_EXCEPTION(EOvl("ovlOVLManager("+string(Tag())+")::DeferMake called on undeferable type"));
    DUMP_LOG("Trace: ovlOVLManager::DeferMake(%s)", UNISTR(man->Tag()));
    if (m_defermake) {
        m_defermake->DeferMake(man);
    } else {
        m_defermake = man;
    }
}

ovlOVLManager::~ovlOVLManager() {
    if (m_data)
        delete[] m_data;
}

ovlLodSymRefManager* ovlOVLManager::GetLSRManager() {
    return &m_ovl->m_lsrmanager;
}

ovlStringTable* ovlOVLManager::GetStringTable() {
    return &m_ovl->m_stringtable;
}

ovlRelocationManager* ovlOVLManager::GetRelocationManager() {
    return &m_ovl->m_relmanager;
}

void ovlOVLManager::WriteLoader(fstream& f) {
    if (!f.is_open())
        BOOST_THROW_EXCEPTION(EOvl("ovlOVLManager("+string(Tag())+")::WriteLoader called with closed file"));

    unsigned short len = strlen(Loader());
    f.write(reinterpret_cast<char*>(&len), 2);
    f.write(Loader(), len);

    len = strlen(Name());
    f.write(reinterpret_cast<char*>(&len), 2);
    f.write(Name(), len);

    unsigned long type = Type();
    f.write(reinterpret_cast<char*>(&type), 4);

    len = strlen(Tag());
    f.write(reinterpret_cast<char*>(&len), 2);
    f.write(Tag(), len);
}

void ovlOVLManager::Make(cOvlInfo* info) {
    // No checking, this is done by derived objects
    //m_data = new unsigned char[m_size];
    //memset(m_data, 0, m_size);
    DUMP_LOG("Trace: ovlOVLManager(%s)::Make", UNISTR(Tag()));
    m_made = true;
    if (m_blobs.size()) {
        for (map<string, cOvlMemBlob>::iterator it = m_blobs.begin(); it != m_blobs.end(); ++it) {
            it->second.data = info->OpenFiles[it->second.type].GetBlock(it->second.file, it->second.size);
        }
    } else {
        m_data = info->OpenFiles[OVLT_UNIQUE].GetBlock(2, m_size);
    }
}

void ovlOVLManager::Check(const string& err) {
    DUMP_LOG("Trace: ovlOVLManager(%s)::Check()", UNISTR(err.c_str()));
    if (!m_ovl)
        BOOST_THROW_EXCEPTION(EOvl("ovlOVLManager("+string(Tag())+")::Check failed due to ovl class unavailable in "+err));
    if (m_data)
        BOOST_THROW_EXCEPTION(EOvl("ovlOVLManager("+string(Tag())+")::Check failed as Make was already called in "+err));
}

unsigned char* ovlOVLManager::GetData() {
    if (!m_data)
        BOOST_THROW_EXCEPTION(EOvl("ovlOVLManager("+string(Tag())+")::GetData called with no data"));
    return m_data;
}

const unsigned long ovlOVLManager::GetSize() const {
    return m_size;
}
