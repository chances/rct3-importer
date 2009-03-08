///////////////////////////////////////////////////////////////////////////////
//
// New OVL creation library
// Manager class for RIC structures
// Copyright (C) 2009 Tobias Minch
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

#include "pretty.h"

#include "ManagerRIC.h"

#include "ManagerSVD.h"
#include "OVLException.h"

using namespace r3;
using namespace std;

const char* ovlRICManager::LOADER = "FGDK";
const char* ovlRICManager::NAME = "RideCar";
const char* ovlRICManager::TAG = "ric";

#define DO_ASSIGN(var, subvar)  var.subvar = subvar
#define DO_ASSIGNR(var, realm, subvar)  var.realm ## _ ## subvar = subvar

void cRideCar::Axis::Fill(r3::RideCar_V& rc, ovlStringTable* tab, ovlRelocationManager* rel) const {
	DO_ASSIGNR(rc, axis, type);
	DO_ASSIGNR(rc, axis, stability);
	DO_ASSIGNR(rc, axis, unk01);
	DO_ASSIGNR(rc, axis, unk02);
	DO_ASSIGNR(rc, axis, momentum);
	DO_ASSIGNR(rc, axis, unk03);
	DO_ASSIGNR(rc, axis, unk04);
	DO_ASSIGNR(rc, axis, maximum);
}

void cRideCar::Bobbing::Fill(r3::RideCar_V& rc, ovlStringTable* tab, ovlRelocationManager* rel) const {
	DO_ASSIGNR(rc, bobbing, flag);
	DO_ASSIGNR(rc, bobbing, x);
	DO_ASSIGNR(rc, bobbing, y);
	DO_ASSIGNR(rc, bobbing, z);
}

void cRideCar::Animations::Fill(r3::RideCar_V& rc, ovlStringTable* tab, ovlRelocationManager* rel) const {
	DO_ASSIGNR(rc, anim, start);
	DO_ASSIGNR(rc, anim, started_idle);
	DO_ASSIGNR(rc, anim, stop);
	DO_ASSIGNR(rc, anim, stopped_idle);
	DO_ASSIGNR(rc, anim, belt_open);
	DO_ASSIGNR(rc, anim, belt_open_idle);
	DO_ASSIGNR(rc, anim, belt_close);
	DO_ASSIGNR(rc, anim, belt_closed_idle);
	DO_ASSIGNR(rc, anim, doors_open);
	DO_ASSIGNR(rc, anim, doors_open_idle);
	DO_ASSIGNR(rc, anim, doors_close);
	DO_ASSIGNR(rc, anim, doors_closed_idle);
	DO_ASSIGNR(rc, anim, row_both);
	DO_ASSIGNR(rc, anim, row_left);
	DO_ASSIGNR(rc, anim, row_right);
	DO_ASSIGNR(rc, anim, canoe_station_idle);
	DO_ASSIGNR(rc, anim, canoe_idle_front);
	DO_ASSIGNR(rc, anim, canoe_idle_back);
	DO_ASSIGNR(rc, anim, canoe_row_idle);
	DO_ASSIGNR(rc, anim, canoe_row);
}

void cRideCar::Animations::Fill(r3::RideCar_W& rc, ovlStringTable* tab, ovlRelocationManager* rel) const {
	Fill(rc.v, tab, rel);
	DO_ASSIGNR(rc.w, anim, rudder);
}

void cRideCar::Wheels::Fill(r3::RideCar_V& rc, ovlStringTable* tab, ovlRelocationManager* rel) const {
	rc.wheels_front_right_flipped = front_right.type;
	rc.wheels_front_left_flipped = front_left.type;
	rc.wheels_back_right_flipped = back_right.type;
	rc.wheels_back_left_flipped = back_left.type;
}

void cRideCar::Axels::Fill(r3::RideCar_V& rc, ovlStringTable* tab, ovlRelocationManager* rel) const {
	rc.axel_front_type = front.type;
	rc.axel_rear_type = rear.type;
}

void cRideCar::Unknowns::Fill(r3::RideCar_V& rc, ovlStringTable* tab, ovlRelocationManager* rel) const {
	DO_ASSIGN(rc, unk54);
	DO_ASSIGN(rc, vs_unk01);
	DO_ASSIGN(rc, vs_unk02);
	DO_ASSIGN(rc, unk60);
}

void cRideCar::Sliding::Fill(r3::RideCar_V& rc, ovlStringTable* tab, ovlRelocationManager* rel) const {
	DO_ASSIGN(rc, skyslide_unk01);
	DO_ASSIGN(rc, skyslide_unk02);
	DO_ASSIGN(rc, skyslide_unk03);
}

void cRideCar::Sliding::Fill(r3::RideCar_W& rc, ovlStringTable* tab, ovlRelocationManager* rel) const {
	Fill(rc.v, tab, rel);
	DO_ASSIGN(rc.s, skyslide_unk04);
	DO_ASSIGN(rc.s, slide_unk01);
	DO_ASSIGN(rc.s, skyslide_unk05);
	DO_ASSIGN(rc.s, unstable_unk01);
	DO_ASSIGN(rc.s, slide_unk02);
	DO_ASSIGN(rc.s, slide_unk03);
}

void cRideCar::Soaked::Fill(r3::RideCar_W& rc, ovlStringTable* tab, ovlRelocationManager* rel) const {
	DO_ASSIGN(rc.s, unk62);
	DO_ASSIGN(rc.s, unk63);
	DO_ASSIGN(rc.s, unk64);
	DO_ASSIGN(rc.s, supersoaker_unk01);
	DO_ASSIGN(rc.s, supersoaker_unk02);
	DO_ASSIGN(rc.s, unk70);
	DO_ASSIGN(rc.s, unk72);
}

void cRideCar::Wild::Fill(r3::RideCar_W& rc, ovlStringTable* tab, ovlRelocationManager* rel) const {
	DO_ASSIGN(rc.w, flip_unk);
	DO_ASSIGN(rc.w, unk77);
	DO_ASSIGN(rc.w, drifting_unk);
	DO_ASSIGN(rc.w, unk79);
	DO_ASSIGN(rc.w, paddle_steamer_unk01);
	DO_ASSIGN(rc.w, paddle_steamer_unk02);
	DO_ASSIGN(rc.w, ball_coaster_unk);
}

unsigned long cRideCar::GetCommonSize() const {
	if (seatings.size()>1) {
		return seatings.size() * sizeof(uint32_t);
	} else {
		return 0;
	}
}

unsigned long cRideCar::GetUniqueSize() const {
	switch (version) {
		case 0:
			return sizeof(RideCar_V);
		case 2:
			return sizeof(RideCar_S);
		case 3:
			return sizeof(RideCar_W);
		default:
			BOOST_THROW_EXCEPTION(EOvl("cRideCar::GetUniqueSize() called with illegal version"));
	}
}

void cRideCar::Fill(r3::RideCar_W* rc, ovlStringTable* tab, ovlRelocationManager* rel) const {
    STRINGLIST_ASSIGN(rc->v.name, internalname, true, tab, rel);
    STRINGLIST_ASSIGN(rc->v.username, otherinternalname, true, tab, rel);

	if (seatings.size()>1) {
		rc->v.seat_type_count = seatings.size();
		if (version) {
			rc->v.seating = r3::Constants::SharedRide::Seating::MAX_SW;
		} else {
			rc->v.seating = r3::Constants::SharedRide::Seating::MAX_V;			
		}
		for(int i = 0; i < seatings.size(); ++i) {
			rc->v.seat_types[i] = seatings[i];
		}
	} else {
		rc->v.seat_type_count = 0;
		rc->v.seating = seatings[0];
	}
    rc->v.version = version;
	rc->v.inertia = inertia;
	
	rc->v.moving_inertia = moving.inertia;
	
	axis.Fill(rc->v, tab, rel);
	bobbing.Fill(rc->v, tab, rel);
	wheels.Fill(rc->v, tab, rel);
	axels.Fill(rc->v, tab, rel);
	unknowns.Fill(rc->v, tab, rel);
	
	if (version) {
		sliding.Fill(*rc, tab, rel);
		soaked.Fill(*rc, tab, rel);
		if (version==3) {
			animations.Fill(*rc, tab, rel);
			wild.Fill(*rc, tab, rel);
		} else {
			animations.Fill(rc->v, tab, rel);
		}
	} else {
		animations.Fill(rc->v, tab, rel);
		sliding.Fill(rc->v, tab, rel);
	}

}


void ovlRICManager::AddItem(const cRideCar& item) {
    Check("ovlRICManager::AddItem");
    if (item.name == "")
        BOOST_THROW_EXCEPTION(EOvl("ovlRICManager::AddItem called without name"));
    if (m_items.find(item.name) != m_items.end())
        BOOST_THROW_EXCEPTION(EOvl("ovlRICManager::AddItem: Item with name '"+item.name+"' already exists"));
    if ((item.version == 1) || (item.version > 3))
        BOOST_THROW_EXCEPTION(EOvl("ovlRICManager::AddItem called with illegal version"));
    if (item.internalname == "")
        BOOST_THROW_EXCEPTION(EOvl("ovlRICManager::AddItem called without internal name"));
    if (item.otherinternalname == "")
        BOOST_THROW_EXCEPTION(EOvl("ovlRICManager::AddItem called without other internal name"));
    if (item.svd_ref == "")
        BOOST_THROW_EXCEPTION(EOvl("ovlRICManager::AddItem called without svd reference"));
    if (!item.seatings.size())
        BOOST_THROW_EXCEPTION(EOvl("ovlRICManager::AddItem called without seating"));

    m_items[item.name] = item;

    // Base structures
    m_size += item.GetUniqueSize();
    m_commonsize += item.GetCommonSize();

	cLoader& loader = GetLSRManager()->reserveIndexElement(OVLT_UNIQUE, item.name, ovlRICManager::TAG);
	
    STRINGLIST_ADD(item.internalname, true);
    STRINGLIST_ADD(item.otherinternalname, true);

	loader.reserveSymbolReference(item.svd_ref, ovlSVDManager::TAG);
	loader.reserveSymbolReference(item.moving.svd_ref, ovlSVDManager::TAG, false);

	loader.reserveSymbolReference(item.wheels.front_right.svd, ovlSVDManager::TAG, false);
	loader.reserveSymbolReference(item.wheels.front_left.svd, ovlSVDManager::TAG, false);
	loader.reserveSymbolReference(item.wheels.back_right.svd, ovlSVDManager::TAG, false);
	loader.reserveSymbolReference(item.wheels.back_left.svd, ovlSVDManager::TAG, false);
	
	loader.reserveSymbolReference(item.axels.front.svd, ovlSVDManager::TAG, false);
	loader.reserveSymbolReference(item.axels.rear.svd, ovlSVDManager::TAG, false);

    if (item.version == 3) {
		loader.reserveSymbolReference(item.wild.svd_flipped_ref, ovlSVDManager::TAG, false);
		loader.reserveSymbolReference(item.wild.moving_svd_flipped_ref, ovlSVDManager::TAG, false);
		
		loader.reserveSymbolReference(item.wild.was_ref, ovlWASManager::TAG, false);
    }
/*
    GetLSRManager()->AddSymbol(OVLT_UNIQUE);
    GetLSRManager()->AddLoader(OVLT_UNIQUE);
    GetStringTable()->AddSymbolString(item.name, ovlRICManager::TAG);

    STRINGLIST_ADD(item.internalname, true);
    STRINGLIST_ADD(item.otherinternalname, true);

    SYMREF_ADD(OVLT_UNIQUE, item.svd_ref, ovlSVDManager::TAG, true);
    SYMREF_ADD(OVLT_UNIQUE, item.moving.svd_ref, ovlSVDManager::TAG, false);

    SYMREF_ADD(OVLT_UNIQUE, item.wheels.front_right.svd, ovlSVDManager::TAG, false);
    SYMREF_ADD(OVLT_UNIQUE, item.wheels.front_left.svd, ovlSVDManager::TAG, false);
    SYMREF_ADD(OVLT_UNIQUE, item.wheels.back_right.svd, ovlSVDManager::TAG, false);
    SYMREF_ADD(OVLT_UNIQUE, item.wheels.back_left.svd, ovlSVDManager::TAG, false);
	
    SYMREF_ADD(OVLT_UNIQUE, item.axels.front.svd, ovlSVDManager::TAG, false);
    SYMREF_ADD(OVLT_UNIQUE, item.axels.rear.svd, ovlSVDManager::TAG, false);

    if (item.version == 3) {
		SYMREF_ADD(OVLT_UNIQUE, item.wild.svd_flipped_ref, ovlSVDManager::TAG, false);
		SYMREF_ADD(OVLT_UNIQUE, item.wild.moving_svd_flipped_ref, ovlSVDManager::TAG, false);
		
		SYMREF_ADD(OVLT_UNIQUE, item.wild.was_ref, ovlWASManager::TAG, false);
    }
*/
}

void ovlRICManager::Make(cOvlInfo* info) {
    DUMP_LOG("Trace: ovlRICManager::Make()");
    Check("ovlRICManager::Make");

    m_blobs["0"] = cOvlMemBlob(OVLT_UNIQUE, 2, m_size);
	if (m_commonsize)
		m_blobs["1"] = cOvlMemBlob(OVLT_COMMON, 2, m_commonsize);
    ovlOVLManager::Make(info);
    unsigned char* c_data = m_blobs["0"].data;
    unsigned char* c_commondata = NULL;
	if (m_commonsize)
		c_commondata = m_blobs["1"].data;

    foreach(const cRideCar::mapentry item, m_items) {
        RideCar_W* c_item = reinterpret_cast<RideCar_W*>(c_data);
        if (item.second.version) {
            if (item.second.version == 2) {
                c_data += sizeof(RideCar_S);
            } else {
                c_data += sizeof(RideCar_W);
            }
        } else {
            c_data += sizeof(RideCar_V);
        }

        if (item.second.seatings.size()) {
            c_item->v.seat_types = reinterpret_cast<uint32_t*>(c_commondata);
			c_commondata += item.second.GetCommonSize();
            GetRelocationManager()->AddRelocation(&c_item->v.seat_types);
        }

        item.second.Fill(c_item, GetStringTable(), GetRelocationManager());

		cLoader& loader = GetLSRManager()->assignIndexElement(OVLT_UNIQUE, item.second.name, ovlRICManager::TAG, c_item);
		
		loader.assignSymbolReference(item.second.svd_ref, ovlSVDManager::TAG, &c_item->v.svd_ref);
		loader.assignSymbolReference(item.second.moving.svd_ref, ovlSVDManager::TAG, &c_item->v.moving_svd_ref, false);

		loader.assignSymbolReference(item.second.wheels.front_right.svd, ovlSVDManager::TAG, &c_item->v.wheels_front_right_ref, false);
		loader.assignSymbolReference(item.second.wheels.front_left.svd, ovlSVDManager::TAG, &c_item->v.wheels_front_left_ref, false);
		loader.assignSymbolReference(item.second.wheels.back_right.svd, ovlSVDManager::TAG, &c_item->v.wheels_back_right_ref, false);
		loader.assignSymbolReference(item.second.wheels.back_left.svd, ovlSVDManager::TAG, &c_item->v.wheels_back_left_ref, false);
		
		loader.assignSymbolReference(item.second.axels.front.svd, ovlSVDManager::TAG, &c_item->v.axel_front_ref, false);
		loader.assignSymbolReference(item.second.axels.rear.svd, ovlSVDManager::TAG, &c_item->v.axel_rear_ref, false);

		if (item.second.version == 3) {
			loader.assignSymbolReference(item.second.wild.svd_flipped_ref, ovlSVDManager::TAG, &c_item->w.svd_flipped_ref, false);
			loader.assignSymbolReference(item.second.wild.moving_svd_flipped_ref, ovlSVDManager::TAG, &c_item->w.moving_svd_flipped_ref, false);
			
			loader.assignSymbolReference(item.second.wild.was_ref, ovlWASManager::TAG, &c_item->w.was_ref, false);
		}
/*
        SymbolStruct* c_symbol = GetLSRManager()->MakeSymbol(OVLT_UNIQUE, GetStringTable()->FindSymbolString(item.first, TAG), reinterpret_cast<unsigned long*>(c_item));
        GetLSRManager()->OpenLoader(OVLT_UNIQUE, TAG, reinterpret_cast<unsigned long*>(c_item), false, c_symbol);

        SYMREF_MAKE(OVLT_UNIQUE, item.second.svd_ref, ovlSVDManager::TAG, &c_item->v.svd_ref, true);
        SYMREF_MAKE(OVLT_UNIQUE, item.second.moving.svd_ref, ovlSVDManager::TAG, &c_item->v.moving_svd_ref, false);

        SYMREF_MAKE(OVLT_UNIQUE, item.second.wheels.front_right.svd, ovlSVDManager::TAG, &c_item->v.wheels_front_right_ref, false);
        SYMREF_MAKE(OVLT_UNIQUE, item.second.wheels.front_left.svd, ovlSVDManager::TAG, &c_item->v.wheels_front_left_ref, false);
        SYMREF_MAKE(OVLT_UNIQUE, item.second.wheels.back_right.svd, ovlSVDManager::TAG, &c_item->v.wheels_back_right_ref, false);
        SYMREF_MAKE(OVLT_UNIQUE, item.second.wheels.back_left.svd, ovlSVDManager::TAG, &c_item->v.wheels_back_left_ref, false);

        SYMREF_MAKE(OVLT_UNIQUE, item.second.axels.front.svd, ovlSVDManager::TAG, &c_item->v.axel_front_ref, false);
        SYMREF_MAKE(OVLT_UNIQUE, item.second.axels.rear.svd, ovlSVDManager::TAG, &c_item->v.axel_rear_ref, false);
		
		if (item.second.version == 3) {
			SYMREF_MAKE(OVLT_UNIQUE, item.second.wild.svd_flipped_ref, ovlSVDManager::TAG, &c_item->w.svd_flipped_ref, false);
			SYMREF_MAKE(OVLT_UNIQUE, item.second.wild.moving_svd_flipped_ref, ovlSVDManager::TAG, &c_item->w.moving_svd_flipped_ref, false);
			
			SYMREF_MAKE(OVLT_UNIQUE, item.second.wild.was_ref, ovlWASManager::TAG, &c_item->w.was_ref, false);
		}

        GetLSRManager()->CloseLoader(OVLT_UNIQUE);
*/
    }

}
