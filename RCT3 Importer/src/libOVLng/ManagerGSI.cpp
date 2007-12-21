///////////////////////////////////////////////////////////////////////////////
//
// New OVL creation library
// Manager class for GSI structures
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


#include "ManagerGSI.h"

#include "OVLException.h"

#include "ManagerTEX.h"

const char* ovlGSIManager::LOADER = "FGDK";
const char* ovlGSIManager::NAME = "GUISkinItem";
const char* ovlGSIManager::TAG = "gsi";

void ovlGSIManager::AddItem(const char* name, const char* texture, unsigned long left, unsigned long top, unsigned long right, unsigned long bottom) {
    Check("ovlGSIManager::AddItem");

    m_gsinames.push_back(string(name));
    m_gsitextures.push_back(string(texture));
    GUISkinItemPos pos;
    pos.left = left;
    pos.top = top;
    pos.right = right;
    pos.bottom = bottom;
    m_gsipositions.push_back(pos);

    // Text
    m_size += sizeof(GUISkinItem);
    m_commonsize += sizeof(GUISkinItemPos);

    GetLSRManager()->AddLoader(OVLT_UNIQUE);
    GetLSRManager()->AddSymbol(OVLT_UNIQUE);
    GetLSRManager()->AddSymRef(OVLT_UNIQUE);
    GetStringTable()->AddSymbolString(name, Tag());
    GetStringTable()->AddSymbolString(texture, ovlTEXManager::TAG);
}

void ovlGSIManager::Make(cOvlInfo* info) {
    Check("ovlGSIManager::Make");
    if (!info)
        throw EOvl("ovlGSIManager::Make called without valid info");

    m_blobs["0"] = cOvlMemBlob(OVLT_UNIQUE, 2, m_size);
    m_blobs["1"] = cOvlMemBlob(OVLT_COMMON, 2, m_commonsize);
    ovlOVLManager::Make(info);
    unsigned char* c_data = m_blobs["0"].data;
    unsigned char* c_commondata = m_blobs["1"].data;

    for (unsigned long t = 0; t < m_gsinames.size(); ++t) {
        // Data setup, GUISkinItem
        GUISkinItem* c_gsi = reinterpret_cast<GUISkinItem*>(c_data);
        c_data += sizeof(GUISkinItem);
        c_gsi->tex = NULL;
        c_gsi->unk1 = 0;
        c_gsi->unk2 = 0;

        c_gsi->pos = reinterpret_cast<GUISkinItemPos*>(c_commondata);
        c_commondata += sizeof(GUISkinItemPos);
        GetRelocationManager()->AddRelocation((unsigned long*)&c_gsi->pos);
        *c_gsi->pos = m_gsipositions[t];

        SymbolStruct* s_gsi = GetLSRManager()->MakeSymbol(OVLT_UNIQUE, GetStringTable()->FindSymbolString(m_gsinames[t].c_str(), Tag()), reinterpret_cast<unsigned long*>(c_gsi));
        GetLSRManager()->OpenLoader(OVLT_UNIQUE, TAG, reinterpret_cast<unsigned long*>(c_gsi), 0, s_gsi);
        GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(m_gsitextures[t].c_str(), ovlTEXManager::TAG), reinterpret_cast<unsigned long*>(&c_gsi->tex));
        GetLSRManager()->CloseLoader(OVLT_UNIQUE);
    }

}
