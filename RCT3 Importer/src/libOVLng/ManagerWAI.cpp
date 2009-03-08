///////////////////////////////////////////////////////////////////////////////
//
// New OVL creation library
// Manager class for WAI structures
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

#include "ManagerWAI.h"

#include "ManagerGSI.h"
#include "ManagerSHS.h"
#include "ManagerTXT.h"
#include "OVLException.h"

using namespace r3;
using namespace std;

const char* ovlWAIManager::LOADER = "FGDK";
const char* ovlWAIManager::NAME = "WildAnimalItem";
const char* ovlWAIManager::TAG = "wai";


void cWildAnimalItem::Fill(r3::WildAnimalItem* wai) {
    wai->unk1 = unk1;
    wai->unk2 = unk2;
    wai->unk3 = unk3;
    wai->unk4 = unk4;
    wai->staticshape = NULL;
    wai->distance = distance;
    wai->cost = cost;
    wai->refund = refund;
    wai->invdurability = invdurability;
    wai->unk11 = unk11;
    wai->x_size = x_size;
    wai->y_size = y_size;
    wai->z_size = z_size;
    wai->name = NULL;
    wai->icon = NULL;
    wai->unk17 = unk17;
    wai->unk18 = unk18;
    wai->unk19 = unk19;
    wai->unk20 = unk20;
    wai->flags = flags;
    wai->description = NULL;
    wai->weight = weight;
}

void ovlWAIManager::AddItem(const cWildAnimalItem& item) {
    Check("ovlWAIManager::AddItem");
    if (item.name == "")
        BOOST_THROW_EXCEPTION(EOvl("ovlWAIManager::AddItem called without name"));
    if (m_items.find(item.name) != m_items.end())
        BOOST_THROW_EXCEPTION(EOvl("ovlWAIManager::AddItem: Item with name '"+item.name+"' already exists"));
    if (item.nametxt == "")
        BOOST_THROW_EXCEPTION(EOvl("ovlWAIManager::AddItem called without name text"));
    if (item.description == "")
        BOOST_THROW_EXCEPTION(EOvl("ovlWAIManager::AddItem called without description"));
    if (item.shortname == "")
        BOOST_THROW_EXCEPTION(EOvl("ovlWAIManager::AddItem called without short name"));
    if (item.staticshape == "")
        BOOST_THROW_EXCEPTION(EOvl("ovlWAIManager::AddItem called without static shape"));
    if (item.icon == "")
        BOOST_THROW_EXCEPTION(EOvl("ovlWAIManager::AddItem called without icon"));

    m_items[item.name] = item;

    // Main
    m_size += sizeof(WildAnimalItem);


/*
    GetLSRManager()->AddSymbol(OVLT_UNIQUE);
    GetLSRManager()->AddLoader(OVLT_UNIQUE);
    GetStringTable()->AddSymbolString(item.name.c_str(), ovlWAIManager::TAG);

    GetLSRManager()->AddSymRef(OVLT_UNIQUE);
    GetStringTable()->AddSymbolString(item.nametxt.c_str(), ovlTXTManager::TAG);
    GetLSRManager()->AddSymRef(OVLT_UNIQUE);
    GetStringTable()->AddSymbolString(item.description.c_str(), ovlTXTManager::TAG);
    GetLSRManager()->AddSymRef(OVLT_UNIQUE);
    GetStringTable()->AddSymbolString(item.icon.c_str(), ovlGSIManager::TAG);
    GetLSRManager()->AddSymRef(OVLT_UNIQUE);
    GetStringTable()->AddSymbolString(item.staticshape.c_str(), ovlSHSManager::TAG);
*/
	cLoader& loader = GetLSRManager()->reserveIndexElement(OVLT_UNIQUE, item.name, ovlWAIManager::TAG);
	loader.reserveSymbolReference(item.nametxt, ovlTXTManager::TAG);
	loader.reserveSymbolReference(item.description, ovlTXTManager::TAG);
	loader.reserveSymbolReference(item.icon, ovlGSIManager::TAG);
	loader.reserveSymbolReference(item.staticshape, ovlSHSManager::TAG);

    GetStringTable()->AddString(item.shortname.c_str());

}

void ovlWAIManager::Make(cOvlInfo* info) {
    DUMP_LOG("Trace: ovlWAIManager::Make()");
    Check("ovlWAIManager::Make");

    ovlOVLManager::Make(info);
    unsigned char* c_data = m_data;

    for (map<string, cWildAnimalItem>::iterator it = m_items.begin(); it != m_items.end(); ++it) {
        // Assign structs
        WildAnimalItem* c_item = reinterpret_cast<WildAnimalItem*>(c_data);
        c_data += sizeof(WildAnimalItem);

        it->second.Fill(c_item);

        c_item->shortname = GetStringTable()->FindString(it->second.shortname.c_str());
        GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long*>(&c_item->shortname));
		
		cLoader& loader = GetLSRManager()->assignIndexElement(OVLT_UNIQUE, it->first, ovlWAIManager::TAG, c_item);
		loader.assignSymbolReference(it->second.nametxt, ovlTXTManager::TAG, &c_item->name);
		loader.assignSymbolReference(it->second.description, ovlTXTManager::TAG, &c_item->description);
		loader.assignSymbolReference(it->second.icon, ovlGSIManager::TAG, &c_item->icon);
		loader.assignSymbolReference(it->second.staticshape, ovlSHSManager::TAG, &c_item->staticshape);
		
		/*
        SymbolStruct* c_symbol = GetLSRManager()->MakeSymbol(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->first.c_str(), TAG), reinterpret_cast<unsigned long*>(c_item));
        GetLSRManager()->OpenLoader(OVLT_UNIQUE, TAG, reinterpret_cast<unsigned long*>(c_item), false, c_symbol);

        GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.nametxt.c_str(), ovlTXTManager::TAG),
                             reinterpret_cast<unsigned long*>(&c_item->name));
        GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.description.c_str(), ovlTXTManager::TAG),
                             reinterpret_cast<unsigned long*>(&c_item->description));
        GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.icon.c_str(), ovlGSIManager::TAG),
                             reinterpret_cast<unsigned long*>(&c_item->icon));
        GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.staticshape.c_str(), ovlSHSManager::TAG),
                             reinterpret_cast<unsigned long*>(&c_item->staticshape));


        GetLSRManager()->CloseLoader(OVLT_UNIQUE);
		*/
    }
}
