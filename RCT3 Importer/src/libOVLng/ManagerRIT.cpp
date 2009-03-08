///////////////////////////////////////////////////////////////////////////////
//
// New OVL creation library
// Manager class for RIT structures
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

#include "ManagerRIT.h"

#include "ManagerRIC.h"
#include "ManagerSPL.h"
#include "OVLException.h"

using namespace r3;
using namespace std;

const char* ovlRITManager::LOADER = "FGDK";
const char* ovlRITManager::NAME = "RideTrain";
const char* ovlRITManager::TAG = "rit";

#define DO_ASSIGN(var, subvar)  var.subvar = subvar
#define DO_ASSIGNR(var, realm, subvar)  var.realm ## _ ## subvar = subvar

void cRideTrain::Cars::Fill(r3::RideTrain_V& rc, ovlStringTable* tab, ovlRelocationManager* rel) const {
	DO_ASSIGNR(rc, car, count_min);
	DO_ASSIGNR(rc, car, count_max);
	DO_ASSIGNR(rc, car, count_def);
}

void cRideTrain::Speed::Fill(r3::RideTrain_V& rc, ovlStringTable* tab, ovlRelocationManager* rel) const {
	DO_ASSIGNR(rc, speed, unk01);
	DO_ASSIGNR(rc, speed, unk02);
	DO_ASSIGNR(rc, speed, unk03);
}

void cRideTrain::Camera::Fill(r3::RideTrain_V& rc, ovlStringTable* tab, ovlRelocationManager* rel) const {
	DO_ASSIGNR(rc, camera, lookahead);
	DO_ASSIGNR(rc, camera, g_displacement_mult);
	DO_ASSIGNR(rc, camera, g_displacement_max);
	DO_ASSIGNR(rc, camera, g_displacement_smoothing);
	DO_ASSIGNR(rc, camera, shake_min_speed);
	DO_ASSIGNR(rc, camera, shake_factor);
	DO_ASSIGNR(rc, camera, shake_h_max);
	DO_ASSIGNR(rc, camera, shake_v_max);
	DO_ASSIGNR(rc, camera, chain_shake_factor);
	DO_ASSIGNR(rc, camera, chain_shake_h_max);
	DO_ASSIGNR(rc, camera, chain_shake_v_max);
	DO_ASSIGNR(rc, camera, smoothing);
	DO_ASSIGNR(rc, camera, la_tilt_factor);
}

void cRideTrain::Water::Fill(r3::RideTrain_V& rc, ovlStringTable* tab, ovlRelocationManager* rel) const {
	DO_ASSIGNR(rc, water, unk01);
	DO_ASSIGNR(rc, water, unk02);
	DO_ASSIGNR(rc, water, unk03);
	DO_ASSIGNR(rc, water, unk04);
	DO_ASSIGNR(rc, water, unk05);
	DO_ASSIGNR(rc, water, unk06);
	DO_ASSIGNR(rc, water, free_roam_curve_angle);
	DO_ASSIGNR(rc, water, unk07);
}

void cRideTrain::Unknowns::Fill(r3::RideTrain_V& rc, ovlStringTable* tab, ovlRelocationManager* rel) const {
	DO_ASSIGN(rc, unk37);
	DO_ASSIGN(rc, unk38);
	DO_ASSIGN(rc, unk39);
	DO_ASSIGN(rc, unk40);
	DO_ASSIGN(rc, unk41);
	DO_ASSIGN(rc, unk42);
	DO_ASSIGN(rc, unk43);
	DO_ASSIGN(rc, unk44);
}

void cRideTrain::Soaked::Fill(r3::RideTrain_W& rc, ovlStringTable* tab, ovlRelocationManager* rel) const {
	DO_ASSIGN(rc.s, icon);
	DO_ASSIGN(rc.s, unk51);
}

void cRideTrain::Wild::Fill(r3::RideTrain_W& rc, ovlStringTable* tab, ovlRelocationManager* rel) const {
	DO_ASSIGN(rc.w, airboat_unk01);
	DO_ASSIGN(rc.w, airboat_unk02);
	DO_ASSIGN(rc.w, airboat_unk03);
	DO_ASSIGN(rc.w, airboat_unk04);
	DO_ASSIGN(rc.w, airboat_unk05);
	DO_ASSIGN(rc.w, airboat_unk06);
	DO_ASSIGN(rc.w, unk59);
	DO_ASSIGN(rc.w, unk60);
	DO_ASSIGN(rc.w, ffsiez_unk);
}

unsigned long cRideTrain::GetUniqueSize() const {
	switch (version) {
		case 0:
			return sizeof(RideTrain_V);
		case 2:
			return sizeof(RideTrain_S);
		case 3:
			return sizeof(RideTrain_W);
		default:
			BOOST_THROW_EXCEPTION(EOvl("cRideTrain::GetUniqueSize() called with illegal version"));
	}
}

void cRideTrain::Fill(r3::RideTrain_W* rc, ovlStringTable* tab, ovlRelocationManager* rel) const {
	if (version) {
		STRINGLIST_ASSIGN(rc->s.name, internalname, true, tab, rel);
		rc->v.v2 = -1;
		rc->s.version = version;
	} else {
		STRINGLIST_ASSIGN(rc->v.name, internalname, true, tab, rel);		
	}
    STRINGLIST_ASSIGN(rc->v.description, internaldescription, true, tab, rel);

	rc->v.overtake_flag = overtake;
    STRINGLIST_ASSIGN(rc->v.station, station, false, tab, rel);
	
	cars.Fill(rc->v, tab, rel);
	speed.Fill(rc->v, tab, rel);
	camera.Fill(rc->v, tab, rel);
	water.Fill(rc->v, tab, rel);
	unknowns.Fill(rc->v, tab, rel);
	
	if (version) {
		soaked.Fill(*rc, tab, rel);
		if (version==3) {
			wild.Fill(*rc, tab, rel);
		}
	}

}


void ovlRITManager::AddItem(const cRideTrain& item) {
    Check("ovlRITManager::AddItem");
    if (item.name == "")
        BOOST_THROW_EXCEPTION(EOvl("ovlRITManager::AddItem called without name"));
    if (m_items.find(item.name) != m_items.end())
        BOOST_THROW_EXCEPTION(EOvl("ovlRITManager::AddItem: Item with name '"+item.name+"' already exists"));
    if ((item.version == 1) || (item.version > 3))
        BOOST_THROW_EXCEPTION(EOvl("ovlRITManager::AddItem called with illegal version"));
    if (item.internalname == "")
        BOOST_THROW_EXCEPTION(EOvl("ovlRITManager::AddItem called without internal name"));
    if (item.internaldescription == "")
        BOOST_THROW_EXCEPTION(EOvl("ovlRITManager::AddItem called without internal decription"));
    if (item.cars.front == "")
        BOOST_THROW_EXCEPTION(EOvl("ovlRITManager::AddItem called without front car reference"));

    m_items[item.name] = item;

    // Base structures
    m_size += item.GetUniqueSize();

	cLoader& loader = GetLSRManager()->reserveIndexElement(OVLT_UNIQUE, item.name, ovlRITManager::TAG);
	
    STRINGLIST_ADD(item.internalname, true);
    STRINGLIST_ADD(item.internaldescription, true);
    STRINGLIST_ADD(item.station, false);

	loader.reserveSymbolReference(item.cars.front, ovlRICManager::TAG);
	loader.reserveSymbolReference(item.cars.second, ovlRICManager::TAG, false);
	loader.reserveSymbolReference(item.cars.mid, ovlRICManager::TAG, false);
	loader.reserveSymbolReference(item.cars.penultimate, ovlRICManager::TAG, false);
	loader.reserveSymbolReference(item.cars.rear, ovlRICManager::TAG, false);
	loader.reserveSymbolReference(item.cars.link, ovlRICManager::TAG, false);

	loader.reserveSymbolReference(item.liftsplines.left, ovlSPLManager::TAG, false);
	loader.reserveSymbolReference(item.liftsplines.right, ovlSPLManager::TAG, false);

    if (item.version == 3) {
		loader.reserveSymbolReference(item.cars.unknown, ovlRICManager::TAG, false);
    }

	/*
    GetLSRManager()->AddSymbol(OVLT_UNIQUE);
    GetLSRManager()->AddLoader(OVLT_UNIQUE);
    GetStringTable()->AddSymbolString(item.name, ovlRITManager::TAG);

    STRINGLIST_ADD(item.internalname, true);
    STRINGLIST_ADD(item.internaldescription, true);
    STRINGLIST_ADD(item.station, false);

    SYMREF_ADD(OVLT_UNIQUE, item.cars.front, ovlRICManager::TAG, true);
    SYMREF_ADD(OVLT_UNIQUE, item.cars.second, ovlRICManager::TAG, false);
    SYMREF_ADD(OVLT_UNIQUE, item.cars.mid, ovlRICManager::TAG, false);
    SYMREF_ADD(OVLT_UNIQUE, item.cars.penultimate, ovlRICManager::TAG, false);
    SYMREF_ADD(OVLT_UNIQUE, item.cars.rear, ovlRICManager::TAG, false);
    SYMREF_ADD(OVLT_UNIQUE, item.cars.link, ovlRICManager::TAG, false);

    SYMREF_ADD(OVLT_UNIQUE, item.liftsplines.left, ovlSPLManager::TAG, false);
    SYMREF_ADD(OVLT_UNIQUE, item.liftsplines.right, ovlSPLManager::TAG, false);

    if (item.version == 3) {
		SYMREF_ADD(OVLT_UNIQUE, item.cars.unknown, ovlRICManager::TAG, false);
    }
	 */

}

void ovlRITManager::Make(cOvlInfo* info) {
    DUMP_LOG("Trace: ovlRITManager::Make()");
    Check("ovlRITManager::Make");

    ovlOVLManager::Make(info);
    unsigned char* c_data = m_data;

    foreach(const cRideTrain::mapentry item, m_items) {
        RideTrain_W* c_item = reinterpret_cast<RideTrain_W*>(c_data);
        if (item.second.version) {
            if (item.second.version == 2) {
                c_data += sizeof(RideTrain_S);
            } else {
                c_data += sizeof(RideTrain_W);
            }
        } else {
            c_data += sizeof(RideTrain_V);
        }

        item.second.Fill(c_item, GetStringTable(), GetRelocationManager());
		
		cLoader& loader = GetLSRManager()->assignIndexElement(OVLT_UNIQUE, item.second.name, ovlRITManager::TAG, c_item);
		
		loader.assignSymbolReference(item.second.cars.front, ovlRICManager::TAG, &c_item->v.frontcar_ref);
		loader.assignSymbolReference(item.second.cars.second, ovlRICManager::TAG, &c_item->v.secondcar_ref, false);
		loader.assignSymbolReference(item.second.cars.mid, ovlRICManager::TAG, &c_item->v.midcar_ref, false);
		loader.assignSymbolReference(item.second.cars.penultimate, ovlRICManager::TAG, &c_item->v.penultimatecar_ref, false);
		loader.assignSymbolReference(item.second.cars.rear, ovlRICManager::TAG, &c_item->v.rearcar_ref, false);
		loader.assignSymbolReference(item.second.cars.link, ovlRICManager::TAG, &c_item->v.link_ref, false);

		loader.assignSymbolReference(item.second.liftsplines.left, ovlSPLManager::TAG, &c_item->v.liftcar_left_ref, false);
		loader.assignSymbolReference(item.second.liftsplines.right, ovlSPLManager::TAG, &c_item->v.liftcar_right_ref, false);

		if (item.second.version == 3) {
			loader.assignSymbolReference(item.second.cars.unknown, ovlRICManager::TAG, &c_item->w.unknown_ric_ref, false);
		}

/*
        SymbolStruct* c_symbol = GetLSRManager()->MakeSymbol(OVLT_UNIQUE, GetStringTable()->FindSymbolString(item.first, TAG), reinterpret_cast<unsigned long*>(c_item));
        GetLSRManager()->OpenLoader(OVLT_UNIQUE, TAG, reinterpret_cast<unsigned long*>(c_item), false, c_symbol);

        SYMREF_MAKE(OVLT_UNIQUE, item.second.cars.front, ovlRICManager::TAG, &c_item->v.frontcar_ref, true);
        SYMREF_MAKE(OVLT_UNIQUE, item.second.cars.second, ovlRICManager::TAG, &c_item->v.secondcar_ref, false);
        SYMREF_MAKE(OVLT_UNIQUE, item.second.cars.mid, ovlRICManager::TAG, &c_item->v.midcar_ref, false);
        SYMREF_MAKE(OVLT_UNIQUE, item.second.cars.penultimate, ovlRICManager::TAG, &c_item->v.penultimatecar_ref, false);
        SYMREF_MAKE(OVLT_UNIQUE, item.second.cars.rear, ovlRICManager::TAG, &c_item->v.rearcar_ref, false);
        SYMREF_MAKE(OVLT_UNIQUE, item.second.cars.link, ovlRICManager::TAG, &c_item->v.link_ref, false);

        SYMREF_MAKE(OVLT_UNIQUE, item.second.liftsplines.left, ovlSPLManager::TAG, &c_item->v.liftcar_left_ref, false);
        SYMREF_MAKE(OVLT_UNIQUE, item.second.liftsplines.right, ovlSPLManager::TAG, &c_item->v.liftcar_right_ref, false);
		
		if (item.second.version == 3) {
			SYMREF_MAKE(OVLT_UNIQUE, item.second.cars.unknown, ovlRICManager::TAG, &c_item->w.unknown_ric_ref, false);
		}

        GetLSRManager()->CloseLoader(OVLT_UNIQUE);
*/
    }

}
