///////////////////////////////////////////////////////////////////////////////
//
// New OVL creation library
// Manager class for CHG structures
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

#include "ManagerCHG.h"

#include "ManagerCID.h"
#include "ManagerGSI.h"
#include "ManagerSID.h"
#include "ManagerSPL.h"
#include "ManagerTXT.h"
#include "OVLException.h"

using namespace r3;
using namespace std;

const char* ovlCHGManager::LOADER = "FGDK";
const char* ovlCHGManager::NAME = "ChangingRoom";
const char* ovlCHGManager::TAG = "chg";

void cChangingRoom::Fill(r3::ChangingRoom* chg) {
    chg->sid_ref = NULL;
    chg->spline_ref = NULL;
    attraction.Fill(chg->att);
}

void ovlCHGManager::AddRoom(const cChangingRoom& item) {
    Check("ovlCHGManager::AddRoom");
    if (item.name == "")
        BOOST_THROW_EXCEPTION(EOvl("ovlCHGManager::AddRoom called without name"));
    if (m_items.find(item.name) != m_items.end())
        BOOST_THROW_EXCEPTION(EOvl("ovlCHGManager::AddRoom: Item with name '"+item.name+"' already exists"));
    if (item.sid == "")
        BOOST_THROW_EXCEPTION(EOvl("ovlCHGManager::AddRoom called without sid"));
    if (item.attraction.name == "")
        BOOST_THROW_EXCEPTION(EOvl("ovlCHGManager::AddRoom called without name text"));
    if (item.attraction.description == "")
        BOOST_THROW_EXCEPTION(EOvl("ovlCHGManager::AddRoom called without description text"));

    m_items[item.name] = item;

    m_size += sizeof(ChangingRoom);
    if ((item.attraction.type & r3::Constants::Addon::Wild_Hi) == r3::Constants::Addon::Wild_Hi) {
        m_size += sizeof(Attraction_W);
    } else {
        m_size += sizeof(Attraction_S);
    }

	cLoader& loader = GetLSRManager()->reserveIndexElement(OVLT_UNIQUE, item.name, ovlCHGManager::TAG);
	
	item.attraction.DoAdd(loader);
	loader.reserveSymbolReference(item.sid, ovlSIDManager::TAG);
	loader.reserveSymbolReference(item.spline, ovlSPLManager::TAG);
	
	/*
    GetLSRManager()->AddSymbol(OVLT_UNIQUE);
    GetLSRManager()->AddLoader(OVLT_UNIQUE);
    GetStringTable()->AddSymbolString(item.name.c_str(), ovlCHGManager::TAG);

    GetLSRManager()->AddSymRef(OVLT_UNIQUE);
    GetStringTable()->AddSymbolString(item.attraction.name.c_str(), ovlTXTManager::TAG);
    GetLSRManager()->AddSymRef(OVLT_UNIQUE);
    GetStringTable()->AddSymbolString(item.attraction.description.c_str(), ovlTXTManager::TAG);
    GetLSRManager()->AddSymRef(OVLT_UNIQUE);
    GetStringTable()->AddSymbolString(item.sid.c_str(), ovlSIDManager::TAG);
    GetLSRManager()->AddSymRef(OVLT_UNIQUE);
    GetStringTable()->AddSymbolString(item.spline.c_str(), ovlSPLManager::TAG);
    // The following is not an error, if not set these are symref'd to :gsi and :spl
    GetLSRManager()->AddSymRef(OVLT_UNIQUE);
    GetStringTable()->AddSymbolString(item.attraction.icon.c_str(), ovlGSIManager::TAG);
    GetLSRManager()->AddSymRef(OVLT_UNIQUE);
    GetStringTable()->AddSymbolString(item.attraction.spline.c_str(), ovlSPLManager::TAG);
	 */

}

void ovlCHGManager::Make(cOvlInfo* info) {
    DUMP_LOG("Trace: ovlCHGManager::Make()");
    Check("ovlCHGManager::Make");

    ovlOVLManager::Make(info);
    unsigned char* c_data = m_data;

    for (map<string, cChangingRoom>::iterator it = m_items.begin(); it != m_items.end(); ++it) {
        // Assign structs
        ChangingRoom* c_item = reinterpret_cast<ChangingRoom*>(c_data);
        c_data += sizeof(ChangingRoom);

        c_item->att = reinterpret_cast<Attraction_S*>(c_data);
        if ((it->second.attraction.type & r3::Constants::Addon::Wild_Hi) == r3::Constants::Addon::Wild_Hi) {
            c_data += sizeof(Attraction_W);
        } else {
            c_data += sizeof(Attraction_S);
        }
        GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long*>(&c_item->att));

        it->second.Fill(c_item);

		cLoader& loader = GetLSRManager()->assignIndexElement(OVLT_UNIQUE, it->first, ovlCHGManager::TAG, c_item);
		it->second.attraction.MakeSymRefs(c_item->att, loader);
		loader.assignSymbolReference(it->second.sid, ovlSIDManager::TAG, &c_item->sid_ref);
		loader.assignSymbolReference(it->second.spline, ovlSPLManager::TAG, &c_item->spline_ref);

		/*
        SymbolStruct* c_symbol = GetLSRManager()->MakeSymbol(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->first.c_str(), TAG), reinterpret_cast<unsigned long*>(c_item));
        GetLSRManager()->OpenLoader(OVLT_UNIQUE, TAG, reinterpret_cast<unsigned long*>(c_item), false, c_symbol);

        GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.attraction.name.c_str(), ovlTXTManager::TAG),
                             reinterpret_cast<unsigned long*>(&c_item->att->v.name_ref));
        GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.attraction.description.c_str(), ovlTXTManager::TAG),
                             reinterpret_cast<unsigned long*>(&c_item->att->v.description_ref));
        GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.sid.c_str(), ovlSIDManager::TAG),
                             reinterpret_cast<unsigned long*>(&c_item->sid_ref));
        GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.attraction.icon.c_str(), ovlGSIManager::TAG),
                             reinterpret_cast<unsigned long*>(&c_item->att->v.icon_ref));
        GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.attraction.spline.c_str(), ovlSPLManager::TAG),
                             reinterpret_cast<unsigned long*>(&c_item->att->v.spline_ref));
        GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.spline.c_str(), ovlSPLManager::TAG),
                             reinterpret_cast<unsigned long*>(&c_item->spline_ref));

        GetLSRManager()->CloseLoader(OVLT_UNIQUE);
		 */

    }

}
