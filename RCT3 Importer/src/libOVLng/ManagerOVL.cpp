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


#include "ManagerOVL.h"

#include "OVLException.h"

using namespace std;

ovlOVLManager::ovlOVLManager() {
    m_data = NULL;
    m_size = 0;
    m_lsrman = NULL;
    m_relman = NULL;
    m_stable = NULL;
}

void ovlOVLManager::Init(ovlLodSymRefManager* lsrman, ovlRelocationManager* relman, ovlStringTable* stable) {
    m_lsrman = lsrman;
    m_relman = relman;
    m_stable = stable;
}

ovlOVLManager::~ovlOVLManager() {
    if (m_data)
        delete[] m_data;
}

void ovlOVLManager::Check(const string& err) {
    if ((!m_lsrman) || (!m_relman) || (!m_stable))
        throw EOvl("ovlOVLManager("+string(Tag())+")::Check failed in "+err);
}

unsigned char* ovlOVLManager::GetData() {
    if (!m_data)
        throw EOvl("ovlOVLManager("+string(Tag())+")::GetData called with no data");
    return m_data;
}

const unsigned long ovlOVLManager::GetSize() const {
    return m_size;
}

ovlOVLManager* ovlOVLManager::MakeManager(const char* tag) {
    return NULL;
}
