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
    anr->showitem_count = 0;
    anr->showitems = NULL;
}



void ovlANRManager::AddRide(const cAnimatedRide& item) {
    Check("ovlANRManager::AddRide");
    if (item.name == "")
        throw EOvl("ovlANRManager::AddRide called without name");
    if (m_items.find(item.name) != m_items.end())
        throw EOvl("ovlANRManager::AddRide: Item with name '"+item.name+"' already exists");
    if (item.sid == "")
        throw EOvl("ovlANRManager::AddRide called without sid");
    if (item.attraction.name == "")
        throw EOvl("ovlANRManager::AddRide called without name text");
    if (item.attraction.description == "")
        throw EOvl("ovlANRManager::AddRide called without description text");

    m_items[item.name] = item;

    // Base structures
    if (item.attraction.type & r3::Constants::Addon::Soaked_Hi) {
        m_size += sizeof(AnimatedRide_SW);
        if ((item.attraction.type & r3::Constants::Addon::Wild_Hi) == r3::Constants::Addon::Wild_Hi) {
            m_size += sizeof(Attraction_W);
            m_size += sizeof(Ride_W);
        } else {
            m_size += sizeof(Attraction_S);
            m_size += sizeof(Ride_S);
        }
    } else {
        m_size += sizeof(AnimatedRide_V);
    }
    // Spline pointers
    m_size += item.attraction.splines.size() * 4;

    // Options
    m_commonsize += (item.ride.options.size()+1) * 4;
    m_commonsize += item.ride.GetOptionsSize();

    GetLSRManager()->AddSymbol(OVLT_UNIQUE);
    GetLSRManager()->AddLoader(OVLT_UNIQUE);
    GetStringTable()->AddSymbolString(item.name.c_str(), ovlANRManager::TAG);

    GetLSRManager()->AddSymRef(OVLT_UNIQUE);
    GetStringTable()->AddSymbolString(item.attraction.name.c_str(), ovlTXTManager::TAG);
    GetLSRManager()->AddSymRef(OVLT_UNIQUE);
    GetStringTable()->AddSymbolString(item.attraction.description.c_str(), ovlTXTManager::TAG);
    GetLSRManager()->AddSymRef(OVLT_UNIQUE);
    GetStringTable()->AddSymbolString(item.sid.c_str(), ovlSIDManager::TAG);
    // The following is not an error, if not set these are symref'd to :gsi and :spl
    GetLSRManager()->AddSymRef(OVLT_UNIQUE);
    GetStringTable()->AddSymbolString(item.attraction.icon.c_str(), ovlGSIManager::TAG);
    GetLSRManager()->AddSymRef(OVLT_UNIQUE);
    GetStringTable()->AddSymbolString(item.attraction.spline.c_str(), ovlSPLManager::TAG);

    for (vector<string>::const_iterator it = item.attraction.splines.begin(); it != item.attraction.splines.end(); ++it) {
        GetLSRManager()->AddSymRef(OVLT_UNIQUE);
        GetStringTable()->AddSymbolString(it->c_str(), ovlSPLManager::TAG);
    }

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
        if (it->second.attraction.type & r3::Constants::Addon::Soaked_Hi) {
            // Assign structs
            AnimatedRide_SW* c_item = reinterpret_cast<AnimatedRide_SW*>(c_data);
            c_data += sizeof(AnimatedRide_SW);

            c_item->ride_sub_s = reinterpret_cast<Ride_S*>(c_data);
            if ((it->second.attraction.type & r3::Constants::Addon::Wild_Hi) == r3::Constants::Addon::Wild_Hi) {
                c_data += sizeof(Ride_W);
            } else {
                c_data += sizeof(Ride_S);
            }
            GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long*>(&c_item->ride_sub_s));

            c_item->ride_sub_s->s.att = reinterpret_cast<Attraction_S*>(c_data);
            if ((it->second.attraction.type & r3::Constants::Addon::Wild_Hi) == r3::Constants::Addon::Wild_Hi) {
                c_data += sizeof(Attraction_W);
            } else {
                c_data += sizeof(Attraction_S);
            }
            GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long*>(&c_item->ride_sub_s->s.att));

            if (it->second.attraction.splines.size()) {
                c_item->ride_sub_s->s.att->v.paths_ref = reinterpret_cast<Spline**>(c_data);
                c_data += it->second.attraction.splines.size() * 4;
                GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long*>(&c_item->ride_sub_s->s.att->v.paths_ref));
            }

            c_item->ride_sub_s->v.options = reinterpret_cast<RideOption**>(c_commondata);
            c_commondata += (it->second.ride.options.size()+1) * 4;
            GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long*>(&c_item->ride_sub_s->v.options));
            for (int i = 0; i < it->second.ride.options.size(); ++i) {
                c_item->ride_sub_s->v.options[i] = reinterpret_cast<RideOption*>(c_commondata);
                c_commondata += it->second.ride.options[i].GetSize();
                GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long*>(&c_item->ride_sub_s->v.options[i]));
            }
            c_item->ride_sub_s->v.options[it->second.ride.options.size()] = NULL;

            it->second.Fill(c_item);

            SymbolStruct* c_symbol = GetLSRManager()->MakeSymbol(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->first.c_str(), TAG), reinterpret_cast<unsigned long*>(c_item));
            GetLSRManager()->OpenLoader(OVLT_UNIQUE, TAG, reinterpret_cast<unsigned long*>(c_item), false, c_symbol);

            GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.attraction.name.c_str(), ovlTXTManager::TAG),
                                 reinterpret_cast<unsigned long*>(&c_item->ride_sub_s->s.att->v.name_ref));
            GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.attraction.description.c_str(), ovlTXTManager::TAG),
                                 reinterpret_cast<unsigned long*>(&c_item->ride_sub_s->s.att->v.description_ref));
            GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.sid.c_str(), ovlSIDManager::TAG),
                                 reinterpret_cast<unsigned long*>(&c_item->sid_ref));
            GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.attraction.icon.c_str(), ovlGSIManager::TAG),
                                 reinterpret_cast<unsigned long*>(&c_item->ride_sub_s->s.att->v.icon_ref));
            GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.attraction.spline.c_str(), ovlSPLManager::TAG),
                                 reinterpret_cast<unsigned long*>(&c_item->ride_sub_s->s.att->v.spline_ref));

            for (int i = 0; i < it->second.attraction.splines.size(); ++i) {
                GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.attraction.splines[i].c_str(), ovlSPLManager::TAG),
                                     reinterpret_cast<unsigned long*>(&c_item->ride_sub_s->s.att->v.paths_ref[i]));
            }

            GetLSRManager()->CloseLoader(OVLT_UNIQUE);

        } else {
            // Assign structs
            AnimatedRide_V* c_item = reinterpret_cast<AnimatedRide_V*>(c_data);
            c_data += sizeof(AnimatedRide_V);

            if (it->second.attraction.splines.size()) {
                c_item->att.paths_ref = reinterpret_cast<Spline**>(c_data);
                c_data += it->second.attraction.splines.size() * 4;
                GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long*>(&c_item->att.paths_ref));
            }

            c_item->ride.options = reinterpret_cast<RideOption**>(c_commondata);
            c_commondata += (it->second.ride.options.size()+1) * 4;
            GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long*>(&c_item->ride.options));
            for (int i = 0; i < it->second.ride.options.size(); ++i) {
                c_item->ride.options[i] = reinterpret_cast<RideOption*>(c_commondata);
                c_commondata += it->second.ride.options[i].GetSize();
                GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long*>(&c_item->ride.options[i]));
            }
            c_item->ride.options[it->second.ride.options.size()] = NULL;

            it->second.Fill(c_item);

            SymbolStruct* c_symbol = GetLSRManager()->MakeSymbol(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->first.c_str(), TAG), reinterpret_cast<unsigned long*>(c_item));
            GetLSRManager()->OpenLoader(OVLT_UNIQUE, TAG, reinterpret_cast<unsigned long*>(c_item), false, c_symbol);

            GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.attraction.name.c_str(), ovlTXTManager::TAG),
                                 reinterpret_cast<unsigned long*>(&c_item->att.name_ref));
            GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.attraction.description.c_str(), ovlTXTManager::TAG),
                                 reinterpret_cast<unsigned long*>(&c_item->att.description_ref));
            GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.sid.c_str(), ovlSIDManager::TAG),
                                 reinterpret_cast<unsigned long*>(&c_item->sid_ref));
            GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.attraction.icon.c_str(), ovlGSIManager::TAG),
                                 reinterpret_cast<unsigned long*>(&c_item->att.icon_ref));
            GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.attraction.spline.c_str(), ovlSPLManager::TAG),
                                 reinterpret_cast<unsigned long*>(&c_item->att.spline_ref));

            for (int i = 0; i < it->second.attraction.splines.size(); ++i) {
                GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.attraction.splines[i].c_str(), ovlSPLManager::TAG),
                                     reinterpret_cast<unsigned long*>(&c_item->att.paths_ref[i]));
            }

            GetLSRManager()->CloseLoader(OVLT_UNIQUE);

        }
    }

}
