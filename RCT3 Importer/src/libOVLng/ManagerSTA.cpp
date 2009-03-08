///////////////////////////////////////////////////////////////////////////////
//
// New OVL creation library
// Manager class for STA structures
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

#include "ManagerSTA.h"

#include "ManagerCID.h"
#include "ManagerGSI.h"
#include "ManagerSID.h"
#include "ManagerSPL.h"
#include "ManagerTXT.h"
#include "OVLException.h"

using namespace r3;
using namespace std;

const char* ovlSTAManager::LOADER = "FGDK";
const char* ovlSTAManager::NAME = "Stall";
const char* ovlSTAManager::TAG = "sta";

void cStallUnknowns::Fill(Stall_V* sta) {
    sta->unk11 = unk1;
    sta->unk12 = unk2;
    sta->unk13 = unk3;
    sta->unk14 = unk4;
    sta->unk15 = unk5;
    sta->unk16 = unk6;
}
void cStallUnknowns::Fill(Stall_SW* sta) {
    sta->unk11 = unk1;
    sta->unk12 = unk2;
    sta->unk13 = unk3;
    sta->unk14 = unk4;
    sta->unk15 = unk5;
    sta->unk16 = unk6;
}

void cStall::Fill(Stall_V* sta) {
    attraction.Fill(&sta->att);
    unknowns.Fill(sta);
}
void cStall::Fill(Stall_SW* sta) {
    attraction.Fill(sta->att);
    unknowns.Fill(sta);
}


void ovlSTAManager::AddStall(const cStall& stall) {
    Check("ovlSTAManager::AddStall");
    if (stall.name == "")
        BOOST_THROW_EXCEPTION(EOvl("ovlSTAManager::AddStall called without name"));
    if (m_stalls.find(stall.name) != m_stalls.end())
        BOOST_THROW_EXCEPTION(EOvl("ovlSTAManager::AddStall: Item with name '"+stall.name+"' already exists"));
    if (stall.sid == "")
        BOOST_THROW_EXCEPTION(EOvl("ovlSTAManager::AddStall called without sid"));
    if (stall.attraction.name == "")
        BOOST_THROW_EXCEPTION(EOvl("ovlSTAManager::AddStall called without name text"));
    if (stall.attraction.description == "")
        BOOST_THROW_EXCEPTION(EOvl("ovlSTAManager::AddStall called without description text"));

    m_stalls[stall.name] = stall;

    // The following depends on the type of stall structure we want to write
    if (stall.attraction.type & r3::Constants::Addon::Soaked_Hi) {
        // Stall2
        m_size += sizeof(Stall_SW);
        if ((stall.attraction.type & r3::Constants::Addon::Wild_Hi) == r3::Constants::Addon::Wild_Hi) {
            m_size += sizeof(Attraction_W);
        } else {
            m_size += sizeof(Attraction_S);
        }
    } else {
        // Stall
        m_size += sizeof(Stall_V);
    }
    // Both share items
    m_size += stall.items.size() * sizeof(StallItem);

	cLoader& loader = GetLSRManager()->reserveIndexElement(OVLT_UNIQUE, stall.name, ovlSTAManager::TAG);
	
	stall.attraction.DoAdd(loader);
	loader.reserveSymbolReference(stall.sid, ovlSIDManager::TAG);
	foreach(const cStallItem& sitem, stall.items)
		loader.reserveSymbolReference(sitem.item, ovlCIDManager::TAG);
		
	
	/*
    GetLSRManager()->AddSymbol(OVLT_UNIQUE);
    GetLSRManager()->AddLoader(OVLT_UNIQUE);
    GetStringTable()->AddSymbolString(stall.name.c_str(), ovlSTAManager::TAG);

    GetLSRManager()->AddSymRef(OVLT_UNIQUE);
    GetStringTable()->AddSymbolString(stall.attraction.name.c_str(), ovlTXTManager::TAG);
    GetLSRManager()->AddSymRef(OVLT_UNIQUE);
    GetStringTable()->AddSymbolString(stall.attraction.description.c_str(), ovlTXTManager::TAG);
    GetLSRManager()->AddSymRef(OVLT_UNIQUE);
    GetStringTable()->AddSymbolString(stall.sid.c_str(), ovlSIDManager::TAG);
    // The following is not an error, if not set these are symref'd to :gsi and :spl
    GetLSRManager()->AddSymRef(OVLT_UNIQUE);
    GetStringTable()->AddSymbolString(stall.attraction.icon.c_str(), ovlGSIManager::TAG);
    GetLSRManager()->AddSymRef(OVLT_UNIQUE);
    GetStringTable()->AddSymbolString(stall.attraction.spline.c_str(), ovlSPLManager::TAG);

    for (vector<cStallItem>::const_iterator it = stall.items.begin(); it != stall.items.end(); ++it) {
        GetLSRManager()->AddSymRef(OVLT_UNIQUE);
        GetStringTable()->AddSymbolString(it->item.c_str(), ovlCIDManager::TAG);
    }
	 */

}

void ovlSTAManager::Make(cOvlInfo* info) {
    DUMP_LOG("Trace: ovlSTAManager::Make()");
    Check("ovlSTAManager::Make");

    ovlOVLManager::Make(info);
    unsigned char* c_data = m_data;

    for (map<string, cStall>::iterator it = m_stalls.begin(); it != m_stalls.end(); ++it) {
        if (it->second.attraction.type & r3::Constants::Addon::Soaked_Hi) {
            // Stall2
            // Assign structs
            Stall_SW* c_stall = reinterpret_cast<Stall_SW*>(c_data);
            c_data += sizeof(Stall_SW);

            c_stall->att = reinterpret_cast<Attraction_S*>(c_data);
            if ((it->second.attraction.type & r3::Constants::Addon::Wild_Hi) == r3::Constants::Addon::Wild_Hi) {
                c_data += sizeof(Attraction_W);
            } else {
                c_data += sizeof(Attraction_S);
            }
            GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long*>(&c_stall->att));

            if (it->second.items.size()) {
                c_stall->item_count = it->second.items.size();
                c_stall->items = reinterpret_cast<StallItem*>(c_data);
                c_data += c_stall->item_count * sizeof(StallItem);
                GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long*>(&c_stall->items));
            } else {
                c_stall->item_count = 0;
                c_stall->items = NULL;
            }
            it->second.Fill(c_stall);

			cLoader& loader = GetLSRManager()->assignIndexElement(OVLT_UNIQUE, it->first, ovlSTAManager::TAG, c_stall);
			it->second.attraction.MakeSymRefs(c_stall->att, loader);
			loader.assignSymbolReference(it->second.sid, ovlSIDManager::TAG, &c_stall->sid_ref);
			
            unsigned long c = 0;
            foreach(const cStallItem& sitem, it->second.items) {
				loader.assignSymbolReference(sitem.item, ovlCIDManager::TAG, &c_stall->items[c].cid_ref);
                c_stall->items[c].cid_ref = NULL;
                c_stall->items[c].cost = sitem.cost;
                c++;
            }

			/*
            SymbolStruct* c_symbol = GetLSRManager()->MakeSymbol(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->first.c_str(), TAG), reinterpret_cast<unsigned long*>(c_stall));
            GetLSRManager()->OpenLoader(OVLT_UNIQUE, TAG, reinterpret_cast<unsigned long*>(c_stall), false, c_symbol);

            GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.attraction.name.c_str(), ovlTXTManager::TAG),
                                 reinterpret_cast<unsigned long*>(&c_stall->att->v.name_ref));
            GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.attraction.description.c_str(), ovlTXTManager::TAG),
                                 reinterpret_cast<unsigned long*>(&c_stall->att->v.description_ref));
            GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.sid.c_str(), ovlSIDManager::TAG),
                                 reinterpret_cast<unsigned long*>(&c_stall->SID));
            GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.attraction.icon.c_str(), ovlGSIManager::TAG),
                                 reinterpret_cast<unsigned long*>(&c_stall->att->v.icon_ref));
            GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.attraction.spline.c_str(), ovlSPLManager::TAG),
                                 reinterpret_cast<unsigned long*>(&c_stall->att->v.spline_ref));
            unsigned long c = 0;
            for (vector<cStallItem>::iterator iit = it->second.items.begin(); iit != it->second.items.end(); ++iit) {
                GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(iit->item.c_str(), ovlCIDManager::TAG),
                                     reinterpret_cast<unsigned long*>(&c_stall->Items[c].cid_ref));
                c_stall->Items[c].cid_ref = NULL;
                c_stall->Items[c].cost = iit->cost;
                c++;
            }

            GetLSRManager()->CloseLoader(OVLT_UNIQUE);
			 */
        } else {
            // Stall
            // Assign structs
            Stall_V* c_stall = reinterpret_cast<Stall_V*>(c_data);
            c_data += sizeof(Stall_V);

            if (it->second.items.size()) {
                c_stall->item_count = it->second.items.size();
                c_stall->items = reinterpret_cast<StallItem*>(c_data);
                c_data += c_stall->item_count * sizeof(StallItem);
                GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long*>(&c_stall->items));
            } else {
                c_stall->item_count = 0;
                c_stall->items = NULL;
            }
            it->second.Fill(c_stall);

			cLoader& loader = GetLSRManager()->assignIndexElement(OVLT_UNIQUE, it->first, ovlSTAManager::TAG, c_stall);
			it->second.attraction.MakeSymRefs(&c_stall->att, loader);
			loader.assignSymbolReference(it->second.sid, ovlSIDManager::TAG, &c_stall->sid_ref);
			
            unsigned long c = 0;
            foreach(const cStallItem& sitem, it->second.items) {
				loader.assignSymbolReference(sitem.item, ovlCIDManager::TAG, &c_stall->items[c].cid_ref);
                c_stall->items[c].cid_ref = NULL;
                c_stall->items[c].cost = sitem.cost;
                c++;
            }
			
			/*
            SymbolStruct* c_symbol = GetLSRManager()->MakeSymbol(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->first.c_str(), TAG), reinterpret_cast<unsigned long*>(c_stall));
            GetLSRManager()->OpenLoader(OVLT_UNIQUE, TAG, reinterpret_cast<unsigned long*>(c_stall), false, c_symbol);

            GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.attraction.name.c_str(), ovlTXTManager::TAG),
                                 reinterpret_cast<unsigned long*>(&c_stall->Name));
            GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.attraction.description.c_str(), ovlTXTManager::TAG),
                                 reinterpret_cast<unsigned long*>(&c_stall->Description));
            GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.sid.c_str(), ovlSIDManager::TAG),
                                 reinterpret_cast<unsigned long*>(&c_stall->SID));
            GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.attraction.icon.c_str(), ovlGSIManager::TAG),
                                 reinterpret_cast<unsigned long*>(&c_stall->GSI));
            GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.attraction.spline.c_str(), ovlSPLManager::TAG),
                                 reinterpret_cast<unsigned long*>(&c_stall->spline));
            unsigned long c = 0;
            for (vector<cStallItem>::iterator iit = it->second.items.begin(); iit != it->second.items.end(); ++iit) {
                GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(iit->item.c_str(), ovlCIDManager::TAG),
                                     reinterpret_cast<unsigned long*>(&c_stall->Items[c].cid_ref));
                c_stall->Items[c].cost = iit->cost;
                c++;
            }

            GetLSRManager()->CloseLoader(OVLT_UNIQUE);
			 */
        }

    }
}
