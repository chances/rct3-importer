///////////////////////////////////////////////////////////////////////////////
//
// New OVL creation library
// Manager class for SPL structures
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


#include "ManagerSPL.h"

#include "OVLDebug.h"
#include "OVLException.h"

const char* ovlSPLManager::LOADER = "FGDK";
const char* ovlSPLManager::NAME = "Spline";
const char* ovlSPLManager::TAG = "spl";

void ovlSPLManager::AddSpline(const cSpline& item) {
    Check("ovlSPLManager::AddSpline");
    if (item.name == "")
        throw EOvl("ovlSPLManager::AddSpline called without name");
    if (item.nodes.size() == 0)
        throw EOvl("ovlSPLManager::AddSpline called without nodes");

    m_items[item.name] = item;

    m_size += sizeof(Spline);
    m_size += item.nodes.size() * sizeof(SplineNode);
    m_size += item.lengths.size() * sizeof(float);
    m_size += item.unknowndata.size() * 14;

    GetLSRManager()->AddSymbol(OVLT_COMMON);
    GetLSRManager()->AddLoader(OVLT_COMMON);
    GetStringTable()->AddSymbolString(item.name.c_str(), ovlSPLManager::TAG);

}

unsigned char* ovlSPLManager::Make(cOvlInfo* info) {
    DUMP_LOG("Trace: ovlSPLManager::Make()");
    Check("ovlSPLManager::Make");

    m_blobs[0] = cOvlMemBlob(OVLT_COMMON, 2, m_size);
    ovlOVLManager::Make(info);
    unsigned char* c_data = m_blobs[0].data;

    for (map<string, cSpline>::iterator it = m_items.begin(); it != m_items.end(); ++it) {
        // Assign structs
        Spline* c_item = reinterpret_cast<Spline*>(c_data);
        c_data += sizeof(Spline);

        c_item->nodes = reinterpret_cast<SplineNode*>(c_data);
        c_data += it->second.nodes.size() * sizeof(SplineNode);
        GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long*>(&c_item->nodes));

        c_item->lengths = reinterpret_cast<float*>(c_data);
        c_data += it->second.lengths.size() * sizeof(float);
        GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long*>(&c_item->lengths));

        c_item->unk5 = reinterpret_cast<unsigned char*>(c_data);
        c_data += it->second.unknowndata.size() * 14;
        GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long*>(&c_item->unk5));

        it->second.Fill(c_item);

        SymbolStruct* c_symbol = GetLSRManager()->MakeSymbol(OVLT_COMMON, GetStringTable()->FindSymbolString(it->first.c_str(), TAG), reinterpret_cast<unsigned long*>(c_item));
        GetLSRManager()->OpenLoader(OVLT_COMMON, TAG, reinterpret_cast<unsigned long*>(c_item), false, c_symbol);
        GetLSRManager()->CloseLoader(OVLT_COMMON);

    }

    return NULL;
}
