///////////////////////////////////////////////////////////////////////////////
//
// New OVL creation library
// Manager class for GSI structures
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


#include "ManagerGSI.h"

#include <boost/format.hpp>

#include "OVLException.h"
#include "pretty.h"
#include "rct3constants.h"

#include "ManagerTEX.h"

using namespace std;
using namespace r3;

const char* ovlGSIManager::LOADER = "FGDK";
const char* ovlGSIManager::NAME = "GUISkinItem";
const char* ovlGSIManager::TAG = "gsi";

void cGUISkinItem::Check() const {
	size_t len = 4;
	switch(gsi_type) {
		case 0:
			break;
		case 1:
			len = 0;
			break;
		case 2:
		case 3:
			len = 8;
			break;
		case 4:
		case 5:
			len = 6;
		default:
			BOOST_THROW_EXCEPTION(EOvl(boost::str(boost::format("cGUISkinType '%s' has unknown type '%d'") % name % gsi_type)));
	}
	if (len != values.size()) {
		BOOST_THROW_EXCEPTION(EOvl(boost::str(boost::format("cGUISkinType '%s' has %d values, but according to it's type, it should have %d.") % name % values.size() % len)));
	}
}

unsigned long cGUISkinItem::GetCommonSize() const {
	switch(gsi_type) {
		case 0:
			return 4 * sizeof(uint32_t);
		case 1:
			return 0;
		case 2:
		case 3:
			return 8 * sizeof(uint32_t);
		case 4:
		case 5:
			return 6 * sizeof(uint32_t);
		default:
			BOOST_THROW_EXCEPTION(EOvl(boost::str(boost::format("cGUISkinType '%s' has unknown type '%d'") % name % gsi_type)));
	}
}

unsigned long cGUISkinItem::GetUniqueSize() const {
	return sizeof(GUISkinItem);
}


void ovlGSIManager::AddItem(const cGUISkinItem& item) {
    Check("ovlGSIManager::AddItem");
    if (item.name == "")
        BOOST_THROW_EXCEPTION(EOvl("ovlGSIManager::AddItem called without name"));
    if (m_items.find(item.name) != m_items.end())
        BOOST_THROW_EXCEPTION(EOvl("ovlGSIManager::AddItem: Item with name '"+item.name+"' already exists"));
	item.Check();

    m_items[item.name] = item;

    // Base structures
    m_size += item.GetUniqueSize();
    m_commonsize += item.GetCommonSize();

	cLoader& loader = GetLSRManager()->reserveIndexElement(OVLT_UNIQUE, item.name, ovlGSIManager::TAG);	
	loader.reserveSymbolReference(item.texture, ovlTEXManager::TAG);
	
	/*
    GetLSRManager()->AddLoader(OVLT_UNIQUE);
    GetLSRManager()->AddSymbol(OVLT_UNIQUE);
    GetLSRManager()->AddSymRef(OVLT_UNIQUE);
    GetStringTable()->AddSymbolString(item.name, Tag());
    GetStringTable()->AddSymbolString(item.texture, ovlTEXManager::TAG);
	 */
}

void ovlGSIManager::Make(cOvlInfo* info) {
    Check("ovlGSIManager::Make");
    if (!info)
        BOOST_THROW_EXCEPTION(EOvl("ovlGSIManager::Make called without valid info"));

    m_blobs["0"] = cOvlMemBlob(OVLT_UNIQUE, 2, m_size);
	if (m_commonsize)
		m_blobs["1"] = cOvlMemBlob(OVLT_COMMON, 2, m_commonsize);
    ovlOVLManager::Make(info);
    unsigned char* c_data = m_blobs["0"].data;
    unsigned char* c_commondata = NULL;
	if (m_commonsize)
		c_commondata = m_blobs["1"].data;

    foreach(const cGUISkinItem::mapentry item, m_items) {
        // Data setup, GUISkinItem
        GUISkinItem* c_gsi = reinterpret_cast<GUISkinItem*>(c_data);
        c_data += sizeof(GUISkinItem);
        c_gsi->tex_ref = NULL;
        c_gsi->gsi_type = item.second.gsi_type;
        c_gsi->unk2 = item.second.unk2;

		if (item.second.gsi_type != r3::Constants::GSI::Type::Special) {
			c_gsi->pos = reinterpret_cast<uint32_t*>(c_commondata);
			c_commondata += item.second.GetCommonSize();
			uint32_t* c_gsipos = c_gsi->pos;
			foreach(unsigned long p, item.second.values) {
				*c_gsipos = p;
				c_gsipos++;
			}
			GetRelocationManager()->AddRelocation((unsigned long*)&c_gsi->pos);
		} else {
			c_gsi->pos = NULL;
		}
		

		cLoader& loader = GetLSRManager()->assignIndexElement(OVLT_UNIQUE, item.first, ovlGSIManager::TAG, c_gsi);
		loader.assignSymbolReference(item.second.texture, ovlTEXManager::TAG, &c_gsi->tex_ref);
		/*
        SymbolStruct* s_gsi = GetLSRManager()->MakeSymbol(OVLT_UNIQUE, GetStringTable()->FindSymbolString(item.first, Tag()), reinterpret_cast<unsigned long*>(c_gsi));
        GetLSRManager()->OpenLoader(OVLT_UNIQUE, TAG, reinterpret_cast<unsigned long*>(c_gsi), 0, s_gsi);
        GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(item.second.texture, ovlTEXManager::TAG), reinterpret_cast<unsigned long*>(&c_gsi->tex_ref));
        GetLSRManager()->CloseLoader(OVLT_UNIQUE);
		 */
    }

}
