///////////////////////////////////////////////////////////////////////////////
//
// New OVL creation library
// Manager class for QTD structures
// Copyright (C) 2008 Tobias Minch
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

#include "ManagerQTD.h"

#include "ManagerFTX.h"
#include "ManagerGSI.h"
#include "ManagerTXT.h"
#include "OVLException.h"

using namespace std;
using namespace r3;

const char* ovlQTDManager::NAME = "QueueType";
const char* ovlQTDManager::TAG = "qtd";

void cQueue::DoCopy(ovlStringTable* tab) {
    if (internalname == "")
        internalname = name;
    tab->AddString(internalname);
    tab->AddString(straight);
    tab->AddString(turn_l);
    tab->AddString(turn_r);
    tab->AddString(slopeup);
    tab->AddString(slopedown);
    if (slopestraight[1] == "")
        slopestraight[1] = slopestraight[0];
    tab->AddString(slopestraight[0]);
    tab->AddString(slopestraight[1]);
}

inline void AssignAndRelocateString(char*& pstr, const string& sstr, ovlStringTable* tab, ovlRelocationManager* rel) {
    pstr = tab->FindString(sstr);
    rel->AddRelocation(reinterpret_cast<unsigned long*>(&pstr));
}

void cQueue::Fill(QueueType* qtd, ovlStringTable* tab, ovlRelocationManager* rel) {
    AssignAndRelocateString(qtd->InternalName, internalname, tab, rel);
    AssignAndRelocateString(qtd->Straight, straight, tab, rel);
    AssignAndRelocateString(qtd->TurnL, turn_l, tab, rel);
    AssignAndRelocateString(qtd->TurnR, turn_r, tab, rel);
    AssignAndRelocateString(qtd->SlopeUp, slopeup, tab, rel);
    AssignAndRelocateString(qtd->SlopeDown, slopedown, tab, rel);
    AssignAndRelocateString(qtd->SlopeStraight1, slopestraight[0], tab, rel);
    AssignAndRelocateString(qtd->SlopeStraight2, slopestraight[1], tab, rel);
}

void ovlQTDManager::AddQueue(const cQueue& item) {
    Check("ovlQTDManager::AddQueue");
    if (item.name == "")
        BOOST_THROW_EXCEPTION(EOvl("ovlQTDManager::AddQueue called without name"));
    if (m_items.find(item.name) != m_items.end())
        BOOST_THROW_EXCEPTION(EOvl("ovlQTDManager::AddQueue: Item with name '"+item.name+"' already exists"));
    if (item.nametxt == "")
        BOOST_THROW_EXCEPTION(EOvl("ovlQTDManager::AddQueue called without name text"));
    if (item.icon == "")
        BOOST_THROW_EXCEPTION(EOvl("ovlQTDManager::AddQueue called without icon"));

    m_items[item.name] = item;
    m_items[item.name].DoCopy(GetStringTable());

    // Main
    m_size += sizeof(QueueType);


    GetLSRManager()->AddSymbol(OVLT_UNIQUE);
    GetLSRManager()->AddLoader(OVLT_UNIQUE);
    GetStringTable()->AddSymbolString(item.name.c_str(), ovlQTDManager::TAG);

    GetLSRManager()->AddSymRef(OVLT_UNIQUE);
    GetStringTable()->AddSymbolString(item.nametxt.c_str(), ovlTXTManager::TAG);
    GetLSRManager()->AddSymRef(OVLT_UNIQUE);
    GetStringTable()->AddSymbolString(item.icon.c_str(), ovlGSIManager::TAG);
    GetLSRManager()->AddSymRef(OVLT_UNIQUE);
    GetStringTable()->AddSymbolString(item.texture.c_str(), ovlFTXManager::TAG);

}

void ovlQTDManager::Make(cOvlInfo* info) {
    DUMP_LOG("Trace: ovlQTDManager::Make()");
    Check("ovlQTDManager::Make");

    ovlOVLManager::Make(info);
    unsigned char* c_data = m_data;

    for (map<string, cQueue>::iterator it = m_items.begin(); it != m_items.end(); ++it) {
        // Assign structs
        QueueType* c_item = reinterpret_cast<QueueType*>(c_data);
        c_data += sizeof(QueueType);

        it->second.Fill(c_item, GetStringTable(), GetRelocationManager());

        SymbolStruct* c_symbol = GetLSRManager()->MakeSymbol(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->first.c_str(), TAG), reinterpret_cast<unsigned long*>(c_item));
        GetLSRManager()->OpenLoader(OVLT_UNIQUE, TAG, reinterpret_cast<unsigned long*>(c_item), false, c_symbol);

        GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.nametxt.c_str(), ovlTXTManager::TAG),
                             reinterpret_cast<unsigned long*>(&c_item->Name));
        GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.icon.c_str(), ovlGSIManager::TAG),
                             reinterpret_cast<unsigned long*>(&c_item->gsi));
        GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.texture.c_str(), ovlFTXManager::TAG),
                             reinterpret_cast<unsigned long*>(&c_item->ftx));

        GetLSRManager()->CloseLoader(OVLT_UNIQUE);

    }
}
