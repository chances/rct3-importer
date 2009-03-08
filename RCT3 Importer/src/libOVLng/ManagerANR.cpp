///////////////////////////////////////////////////////////////////////////////
//
// New OVL creation library
// Manager class for ANR structures
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

#include "ManagerANR.h"

#include "pretty.h"

#include "ManagerGSI.h"
#include "ManagerSID.h"
#include "ManagerSPL.h"
#include "ManagerTXT.h"
#include "OVLException.h"

using namespace r3;
using namespace std;

const char* ovlANRManager::LOADER = "FGDK";
const char* ovlANRManager::NAME = "AnimatedRide";
const char* ovlANRManager::TAG = "anr";

/** @brief Fill
  *
  * @todo: document this function
  */
void cAnimatedRide::Fill(r3::AnimatedRide_V* anr) const {
    attraction.Fill(&anr->att);
    ride.Fill(&anr->ride);
    anr->sid_ref = NULL;
    anr->unk22 = unk22;
    anr->unk23 = unk23;
    anr->unk24 = unk24;
    anr->unk25 = unk25;
}

/** @brief Fill
  *
  * @todo: document this function
  */
void cAnimatedRide::Fill(r3::AnimatedRide_SW* anr) const {
    if (attraction.type & r3::Constants::Addon::Wild_Hi) {
        ride.Fill(anr->ride_sub_w);
    } else {
        ride.Fill(anr->ride_sub_s);
    }
    // Must come after the above, otherwise the attraction type for some completely mysterious reason gets trashed
    attraction.Fill(anr->ride_sub_s->s.att);
    anr->unk1 = 0xFFFFFFFF;
    anr->unk2 = unk22;
    anr->unk3 = unk23;
    anr->unk4 = unk24;
    anr->unk5 = unk25;
    anr->sid_ref = NULL;
    anr->unk8 = unk8;
    anr->unk9 = unk9;
    anr->showitem_count = showitems.size();
}



void ovlANRManager::AddRide(const cAnimatedRide& item) {
    Check("ovlANRManager::AddRide");
    if (item.name == "")
        BOOST_THROW_EXCEPTION(EOvl("ovlANRManager::AddRide called without name"));
    if (m_items.find(item.name) != m_items.end())
        BOOST_THROW_EXCEPTION(EOvl("ovlANRManager::AddRide: Item with name '"+item.name+"' already exists"));
    if (item.sid == "")
        BOOST_THROW_EXCEPTION(EOvl("ovlANRManager::AddRide called without sid"));
    if (item.attraction.name == "")
        BOOST_THROW_EXCEPTION(EOvl("ovlANRManager::AddRide called without name text"));
    if (item.attraction.description == "")
        BOOST_THROW_EXCEPTION(EOvl("ovlANRManager::AddRide called without description text"));

    m_items[item.name] = item;

    // Base structures
    if (item.attraction.type & r3::Constants::Addon::Soaked_Hi) {
        m_size += sizeof(AnimatedRide_SW);
    } else {
        m_size += sizeof(AnimatedRide_V);
    }
    m_size += item.attraction.GetUniqueSize();
    m_commonsize += item.attraction.GetCommonSize();
    m_size += item.ride.GetUniqueSize(item.attraction);
    m_commonsize += item.ride.GetCommonSize(item.attraction);
    if (item.attraction.isnew()) {
        m_commonsize += item.showitems.size() * sizeof(AnimatedRideShowItem);
        foreach(const cAnimatedRideShowItem& ani, item.showitems) {
            STRINGLIST_ADD(ani.name, true);
        }
    }

	cLoader& loader = GetLSRManager()->reserveIndexElement(OVLT_UNIQUE, item.name, ovlANRManager::TAG);
	
    item.attraction.DoAdd(loader);
    item.ride.DoAdd(loader);
	loader.reserveSymbolReference(item.sid, ovlSIDManager::TAG);
/*
    GetLSRManager()->AddSymbol(OVLT_UNIQUE);
    GetLSRManager()->AddLoader(OVLT_UNIQUE);
    GetStringTable()->AddSymbolString(item.name.c_str(), ovlANRManager::TAG);

    item.attraction.DoAdd(GetStringTable(), GetLSRManager());
    item.ride.DoAdd(GetStringTable(), GetLSRManager());

    GetLSRManager()->AddSymRef(OVLT_UNIQUE);
    GetStringTable()->AddSymbolString(item.sid.c_str(), ovlSIDManager::TAG);
*/
}

void ovlANRManager::Make(cOvlInfo* info) {
    DUMP_LOG("Trace: ovlANRManager::Make()");
    Check("ovlANRManager::Make");

    m_blobs["0"] = cOvlMemBlob(OVLT_UNIQUE, 2, m_size);
    m_blobs["1"] = cOvlMemBlob(OVLT_COMMON, 2, m_commonsize);
    ovlOVLManager::Make(info);
    unsigned char* c_data = m_blobs["0"].data;
    unsigned char* c_commondata = m_blobs["1"].data;

    for (map<string, cAnimatedRide>::iterator it = m_items.begin(); it != m_items.end(); ++it) {
        if (it->second.attraction.isnew()) {
            // Assign structs
            AnimatedRide_SW* c_item = reinterpret_cast<AnimatedRide_SW*>(c_data);
            c_data += sizeof(AnimatedRide_SW);

            it->second.ride.MakePointers(&c_item->ride_sub_s, c_data, c_commondata, it->second.attraction, GetRelocationManager());

            it->second.Fill(c_item);
            RELOC_ARRAY(it->second.showitems.size(), c_item->showitems, AnimatedRideShowItem, c_commondata);
            for(int i = 0; i < it->second.showitems.size(); ++i) {
                c_item->showitems[i].animation_index = it->second.showitems[i].animation;
                STRINGLIST_ASSIGN(c_item->showitems[i].name, it->second.showitems[i].name, true, GetStringTable(), GetRelocationManager());
            }

			cLoader& loader = GetLSRManager()->assignIndexElement(OVLT_UNIQUE, it->first, ovlANRManager::TAG, c_item);
			
            it->second.ride.MakeSymRefs(c_item->ride_sub_s, it->second.attraction, loader);
			loader.assignSymbolReference(it->second.sid, ovlSIDManager::TAG, &c_item->sid_ref);
			
			/*
            SymbolStruct* c_symbol = GetLSRManager()->MakeSymbol(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->first.c_str(), TAG), reinterpret_cast<unsigned long*>(c_item));
            GetLSRManager()->OpenLoader(OVLT_UNIQUE, TAG, reinterpret_cast<unsigned long*>(c_item), false, c_symbol);

            it->second.ride.MakeSymRefs(c_item->ride_sub_s, it->second.attraction, GetLSRManager(), GetStringTable());

            GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.sid.c_str(), ovlSIDManager::TAG),
                                 reinterpret_cast<unsigned long*>(&c_item->sid_ref));

            GetLSRManager()->CloseLoader(OVLT_UNIQUE);
			 */

        } else {
            // Assign structs
            AnimatedRide_V* c_item = reinterpret_cast<AnimatedRide_V*>(c_data);
            c_data += sizeof(AnimatedRide_V);

            it->second.attraction.MakePointers(&c_item->att, c_data, c_commondata, GetRelocationManager());
            it->second.ride.MakePointers(&c_item->ride, c_data, c_commondata, it->second.attraction, GetRelocationManager());

            it->second.Fill(c_item);

			cLoader& loader = GetLSRManager()->assignIndexElement(OVLT_UNIQUE, it->first, ovlANRManager::TAG, c_item);
			
            it->second.attraction.MakeSymRefs(&c_item->att, loader);
            it->second.ride.MakeSymRefs(&c_item->ride, it->second.attraction, loader);
			loader.assignSymbolReference(it->second.sid, ovlSIDManager::TAG, &c_item->sid_ref);
/*
            SymbolStruct* c_symbol = GetLSRManager()->MakeSymbol(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->first.c_str(), TAG), reinterpret_cast<unsigned long*>(c_item));
            GetLSRManager()->OpenLoader(OVLT_UNIQUE, TAG, reinterpret_cast<unsigned long*>(c_item), false, c_symbol);

            it->second.attraction.MakeSymRefs(&c_item->att, GetLSRManager(), GetStringTable());
            it->second.ride.MakeSymRefs(&c_item->ride, it->second.attraction, GetLSRManager(), GetStringTable());

            GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.sid.c_str(), ovlSIDManager::TAG),
                                 reinterpret_cast<unsigned long*>(&c_item->sid_ref));

            GetLSRManager()->CloseLoader(OVLT_UNIQUE);
*/
        }
    }

}
