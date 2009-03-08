///////////////////////////////////////////////////////////////////////////////
//
// New OVL creation library
// Manager class for CED structures
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

#include "ManagerCED.h"

#include "ManagerGSI.h"
#include "ManagerTXT.h"
#include "OVLException.h"

using namespace r3;
using namespace std;


const char* ovlCEDManager::LOADER = "FGDK";
const char* ovlCEDManager::NAME = "CarriedItemExtra";
const char* ovlCEDManager::TAG = "ced";

void cCarriedItemExtra::Fill(r3::CarriedItemExtra* ced) {
    ced->unk1 = unk1;
    ced->name = NULL;
    ced->icon = NULL;
    ced->unk4 = unk4;
    ced->hunger = hunger;
    ced->thirst = thirst;
    ced->unk7 = unk7;
}

void ovlCEDManager::AddExtra(const cCarriedItemExtra& item) {
    Check("ovlCEDManager::AddExtra");
    if (item.name == "")
        BOOST_THROW_EXCEPTION(EOvl("ovlCEDManager::AddExtra called without name"));
    if (m_extras.find(item.name) != m_extras.end())
        BOOST_THROW_EXCEPTION(EOvl("ovlCEDManager::AddExtra: Item with name '"+item.name+"' already exists"));
    if (item.nametxt == "")
        BOOST_THROW_EXCEPTION(EOvl("ovlCEDManager::AddExtra called without name text"));
    if (item.icon == "")
        BOOST_THROW_EXCEPTION(EOvl("ovlCEDManager::AddExtra called without icon"));

    m_extras[item.name] = item;

    // Main
    m_size += sizeof(CarriedItemExtra);

	cLoader& loader = GetLSRManager()->reserveIndexElement(OVLT_UNIQUE, item.name, ovlCEDManager::TAG);
	
	loader.reserveSymbolReference(item.nametxt, ovlTXTManager::TAG);
	loader.reserveSymbolReference(item.icon, ovlGSIManager::TAG);
/*
    GetLSRManager()->AddSymbol(OVLT_UNIQUE);
    GetLSRManager()->AddLoader(OVLT_UNIQUE);
    GetStringTable()->AddSymbolString(item.name.c_str(), ovlCEDManager::TAG);

    GetLSRManager()->AddSymRef(OVLT_UNIQUE);
    GetStringTable()->AddSymbolString(item.nametxt.c_str(), ovlTXTManager::TAG);
    GetLSRManager()->AddSymRef(OVLT_UNIQUE);
    GetStringTable()->AddSymbolString(item.icon.c_str(), ovlGSIManager::TAG);
*/
}

void ovlCEDManager::Make(cOvlInfo* info) {
    DUMP_LOG("Trace: ovlCEDManager::Make()");
    Check("ovlCEDManager::Make");

    ovlOVLManager::Make(info);
    unsigned char* c_data = m_data;

    for (map<string, cCarriedItemExtra>::iterator it = m_extras.begin(); it != m_extras.end(); ++it) {
        // Assign structs
        CarriedItemExtra* c_item = reinterpret_cast<CarriedItemExtra*>(c_data);
        c_data += sizeof(CarriedItemExtra);

        it->second.Fill(c_item);

		cLoader& loader = GetLSRManager()->assignIndexElement(OVLT_UNIQUE, it->first, ovlCEDManager::TAG, c_item);
		loader.assignSymbolReference(it->second.nametxt, ovlTXTManager::TAG, &c_item->name);
		loader.assignSymbolReference(it->second.icon, ovlGSIManager::TAG, &c_item->icon);
/*
        SymbolStruct* c_symbol = GetLSRManager()->MakeSymbol(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->first.c_str(), TAG), reinterpret_cast<unsigned long*>(c_item));
        GetLSRManager()->OpenLoader(OVLT_UNIQUE, TAG, reinterpret_cast<unsigned long*>(c_item), false, c_symbol);

        GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.nametxt.c_str(), ovlTXTManager::TAG),
                             reinterpret_cast<unsigned long*>(&c_item->name));
        GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.icon.c_str(), ovlGSIManager::TAG),
                             reinterpret_cast<unsigned long*>(&c_item->icon));

        GetLSRManager()->CloseLoader(OVLT_UNIQUE);
*/
    }
}
