///////////////////////////////////////////////////////////////////////////////
//
// New OVL creation library
// Manager class for SND structures
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


#include "ManagerSND.h"


#include "ManagerCommon.h"
#include "OVLException.h"

#include "pretty.h"

#include <iostream>
#include <sstream>

using namespace r3;
using namespace std;

const char* ovlSNDManager::LOADER = "FGDK";
const char* ovlSNDManager::NAME = "Sound";
const char* ovlSNDManager::TAG = "snd";

#define DO_ASSIGN(var, subvar)  var.subvar = subvar

/** @brief Fill
  *
  * @todo: document this function
  */
void cSound::Fill(r3::Sound& snd) const {
    DO_ASSIGN(snd, format);
    DO_ASSIGN(snd, unk6);
    DO_ASSIGN(snd, unk7);
    DO_ASSIGN(snd, unk8);
    DO_ASSIGN(snd, unk9);
    DO_ASSIGN(snd, unk10);
    DO_ASSIGN(snd, unk11);
    DO_ASSIGN(snd, unk12);
    DO_ASSIGN(snd, unk13);
    DO_ASSIGN(snd, unk14);
    DO_ASSIGN(snd, unk15);
    DO_ASSIGN(snd, loop);
    DO_ASSIGN(snd, channel1_size);
    DO_ASSIGN(snd, channel2_size);
    if (channel1_size)
        memcpy(snd.channel1, channel1.get(), channel1_size);
    if (channel2_size)
        memcpy(snd.channel2, channel2.get(), channel2_size);
}


void ovlSNDManager::AddItem(const cSound& item) {
    Check("ovlSNDManager::AddItem");
    if (item.name == "")
        throw EOvl("ovlSNDManager::AddItem called without name");
    if (m_items.find(item.name) != m_items.end())
        throw EOvl("ovlSNDManager::AddItem: Item with name '"+item.name+"' already exists");

    m_items[item.name] = item;

    // Data
    m_blobs[item.name] = cOvlMemBlob(OVLT_COMMON, 2, sizeof(Sound) + item.channel1_size + item.channel2_size);

    GetLSRManager()->AddSymbol(OVLT_COMMON);
    GetLSRManager()->AddLoader(OVLT_COMMON);
    GetStringTable()->AddSymbolString(item.name.c_str(), ovlSNDManager::TAG);
}

void ovlSNDManager::Make(cOvlInfo* info) {
    Check("ovlSNDManager::Make");
    if (!info)
        throw EOvl("ovlSNDManager::Make called without valid info");

    ovlOVLManager::Make(info);

    foreach(const cSound::mapentry& item, m_items) {
        unsigned char* c_data = m_blobs[item.first].data;
        Sound* c_sound = reinterpret_cast<Sound*>(c_data);
        c_data += sizeof(Sound);
        memset(c_sound, 0, sizeof(Sound));

        if (item.second.channel1_size) {
            RELOC_ARRAY(item.second.channel1_size / 2, c_sound->channel1, int16_t, c_data);
        }
        if (item.second.channel2_size) {
            RELOC_ARRAY(item.second.channel2_size / 2, c_sound->channel2, int16_t, c_data);
        }
        item.second.Fill(*c_sound);

        SymbolStruct* s_item = GetLSRManager()->MakeSymbol(OVLT_COMMON, GetStringTable()->FindSymbolString(item.first, Tag()), reinterpret_cast<unsigned long*>(c_sound));
        GetLSRManager()->OpenLoader(OVLT_COMMON, TAG, reinterpret_cast<unsigned long*>(c_sound), false, s_item);
        GetLSRManager()->CloseLoader(OVLT_COMMON);
    }
}
