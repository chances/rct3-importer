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

#include "ManagerTER.h"

#include "ManagerTEX.h"
#include "OVLException.h"

using namespace r3;
using namespace std;

const char* ovlTERManager::NAME = "TerrainType";
const char* ovlTERManager::TAG = "ter";

#define DO_ASSIGN(var, subvar)  var.subvar = subvar

void cTerrain::Parameters::Fill(r3::TerrainType& rc, ovlStringTable* tab, ovlRelocationManager* rel) const {
	DO_ASSIGN(rc, inv_width);
	DO_ASSIGN(rc, inv_height);
	rc.colour_simple_int = colour01;
	rc.colour_map_int = colour02;
}

void cTerrain::Unknowns::Fill(r3::TerrainType& rc, ovlStringTable* tab, ovlRelocationManager* rel) const {
	DO_ASSIGN(rc, unk02);
	DO_ASSIGN(rc, unk13);
	DO_ASSIGN(rc, unk14);
	DO_ASSIGN(rc, unk15);
}


unsigned long cTerrain::GetUniqueSize() const {
	return sizeof(TerrainType);
}

void cTerrain::Fill(r3::TerrainType* rc, ovlStringTable* tab, ovlRelocationManager* rel) const {
    STRINGLIST_ASSIGN(rc->description_name, description_name, true, tab, rel);
    STRINGLIST_ASSIGN(rc->icon_name, icon_name, true, tab, rel);
	
	rc->unk01 = version;
	rc->addon = addon;
	rc->number = number;
	rc->type = type;
	
	parameters.Fill(*rc, tab, rel);
	unknowns.Fill(*rc, tab, rel);

}


void ovlTERManager::AddItem(const cTerrain& item) {
    Check("ovlTERManager::AddItem");
    if (item.name == "")
        BOOST_THROW_EXCEPTION(EOvl("ovlTERManager::AddItem called without name"));
    if (m_items.find(item.name) != m_items.end())
        BOOST_THROW_EXCEPTION(EOvl("ovlTERManager::AddItem: Item with name '"+item.name+"' already exists"));
    if (item.description_name == "")
        BOOST_THROW_EXCEPTION(EOvl("ovlTERManager::AddItem called without decription name"));
    if (item.icon_name == "")
        BOOST_THROW_EXCEPTION(EOvl("ovlTERManager::AddItem called without icon name"));
    if (item.texture == "")
        BOOST_THROW_EXCEPTION(EOvl("ovlTERManager::AddItem called without texture"));

    m_items[item.name] = item;

    // Base structures
    m_size += item.GetUniqueSize();

	cLoader& loader = GetLSRManager()->reserveIndexElement(OVLT_UNIQUE, item.name, ovlTERManager::TAG);
    //GetLSRManager()->AddSymbol(OVLT_UNIQUE);
    //GetLSRManager()->AddLoader(OVLT_UNIQUE);
    //GetStringTable()->AddSymbolString(item.name, ovlTERManager::TAG);

    STRINGLIST_ADD(item.description_name, true);
    STRINGLIST_ADD(item.icon_name, true);

    //SYMREF_ADD(OVLT_UNIQUE, item.texture, ovlTEXManager::TAG, true);
	loader.reserveSymbolReference(item.texture, ovlTEXManager::TAG);
}

void ovlTERManager::Make(cOvlInfo* info) {
    DUMP_LOG("Trace: ovlTERManager::Make()");
    Check("ovlTERManager::Make");

    ovlOVLManager::Make(info);
    unsigned char* c_data = m_data;

    foreach(const cTerrain::mapentry item, m_items) {
        TerrainType* c_item = reinterpret_cast<TerrainType*>(c_data);
		c_data += sizeof(TerrainType);

        item.second.Fill(c_item, GetStringTable(), GetRelocationManager());

		cLoader& loader = GetLSRManager()->assignIndexElement(OVLT_UNIQUE, item.second.name, ovlTERManager::TAG, c_item);
		loader.assignSymbolReference(item.second.texture, ovlTEXManager::TAG, &c_item->texture_ref);
		
		/*
        SymbolStruct* c_symbol = GetLSRManager()->MakeSymbol(OVLT_UNIQUE, GetStringTable()->FindSymbolString(item.first, TAG), reinterpret_cast<unsigned long*>(c_item));
        GetLSRManager()->OpenLoader(OVLT_UNIQUE, TAG, reinterpret_cast<unsigned long*>(c_item), false, c_symbol);

        SYMREF_MAKE(OVLT_UNIQUE, item.second.texture, ovlTEXManager::TAG, &c_item->texture_ref, true);
        GetLSRManager()->CloseLoader(OVLT_UNIQUE);
		*/
    }

}
