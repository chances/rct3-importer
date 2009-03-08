///////////////////////////////////////////////////////////////////////////////
//
// New OVL creation library
// Manager class for CID structures
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

#include "ManagerCID.h"

#include "ManagerCED.h"
#include "ManagerFTX.h"
#include "ManagerGSI.h"
#include "ManagerSHS.h"
#include "ManagerSID.h"
#include "ManagerTXT.h"
#include "OVLException.h"

using namespace r3;
using namespace std;

const char* ovlCIDManager::LOADER = "FGDK";
const char* ovlCIDManager::NAME = "CarriedItem";
const char* ovlCIDManager::TAG = "cid";

void cCarriedItemUnknowns::Fill(r3::CarriedItem* cid) {
    cid->unk1 = unk1;
    cid->unk3 = unk3;
    cid->unk26 = unk26;
    cid->unk27 = unk27;
    cid->unk28 = unk28;
    cid->unk33 = unk33;
}

void cCarriedItemSizeUnknowns::Fill(r3::CarriedItem* cid) {
    cid->unk17 = unk17;
    cid->unk18 = unk18;
    cid->unk19 = unk19;
    cid->unk20 = unk20;
}

void cCarriedItemTrash::Fill(r3::CarriedItem* cid) {
    cid->wrapper = NULL;
    cid->trash1 = trash1;
    cid->trash2 = trash2;
}

void cCarriedItemSettings::Fill(r3::CarriedItem* cid) {
    cid->flags = flags;
    cid->ageclass = ageclass;
    cid->type = type;
    cid->hunger = hunger;
    cid->thirst = thirst;
    cid->lightprotectionfactor = lightprotectionfactor;
}

void cCarriedItemShape::MakeBillboard() {
    unk9 = 200.0;
    defaultcolour = 7;
    shaped = 0;
    scalex = 0.75;
    scaley = 0.75;
}
void cCarriedItemShape::Fill(r3::CarriedItem* cid) {
    cid->unk9 = unk9;
    cid->defaultcolour = defaultcolour;
    cid->shaped = shaped;
    cid->scalex = scalex;
    cid->scaley = scaley;
    cid->shape1 = NULL;
    cid->shape2 = NULL;
    cid->fts = NULL;
}

void cCarriedItem::Fill(r3::CarriedItem* cid) {
    cid->addonpack = addonpack;
    cid->name = NULL;
    cid->pluralname = NULL;
    shape.Fill(cid);
    settings.Fill(cid);
    trash.Fill(cid);
    soaked.Fill(cid);
    size.Fill(cid);
    unknowns.Fill(cid);
}

void ovlCIDManager::AddItem(const cCarriedItem& item) {
    Check("ovlCIDManager::AddItem");
    if (item.name == "")
        BOOST_THROW_EXCEPTION(EOvl("ovlCIDManager::AddItem called without name"));
    if (m_items.find(item.name) != m_items.end())
        BOOST_THROW_EXCEPTION(EOvl("ovlCIDManager::AddItem: Item with name '"+item.name+"' already exists"));
    if (item.nametxt == "")
        BOOST_THROW_EXCEPTION(EOvl("ovlCIDManager::AddItem called without name text"));
    if (item.pluralnametxt == "")
        BOOST_THROW_EXCEPTION(EOvl("ovlCIDManager::AddItem called without plural name text"));
    if ((item.shape.shape1 == "") && (item.shape.shape2 == "") && (item.shape.fts == ""))
        BOOST_THROW_EXCEPTION(EOvl("ovlCIDManager::AddItem called without shape info"));
    if (((item.shape.shape1 != "") || (item.shape.shape2 != "")) && (item.shape.fts != ""))
        BOOST_THROW_EXCEPTION(EOvl("ovlCIDManager::AddItem, cannot be both 3D and 2D"));

    m_items[item.name] = item;

    // Main
    m_size += sizeof(CarriedItem);
    // Extras
    m_size += item.extras.size() * 4;


	cLoader& loader = GetLSRManager()->reserveIndexElement(OVLT_UNIQUE, item.name, ovlCIDManager::TAG);
	
	loader.reserveSymbolReference(item.nametxt, ovlTXTManager::TAG);
	loader.reserveSymbolReference(item.pluralnametxt, ovlTXTManager::TAG);
    // The following is not an error, if not set it's symref'd to :gsi
	loader.reserveSymbolReference(item.icon, ovlGSIManager::TAG);

	loader.reserveSymbolReference(item.shape.shape1, ovlSHSManager::TAG, false);
	loader.reserveSymbolReference(item.shape.shape2, ovlSHSManager::TAG, false);
	
    // The following is not an error, if not set it's symref'd to :cid
	loader.reserveSymbolReference(item.trash.wrapper, ovlCIDManager::TAG);
	
	foreach(const string& extra, item.extras)
		loader.reserveSymbolReference(extra, ovlCEDManager::TAG);

	loader.reserveSymbolReference(item.shape.fts, ovlFTXManager::TAG, false);
	
	STRINGLIST_ADD(item.soaked.male_morphmesh_body, true);
	STRINGLIST_ADD(item.soaked.male_morphmesh_legs, true);
	STRINGLIST_ADD(item.soaked.female_morphmesh_body, true);
	STRINGLIST_ADD(item.soaked.female_morphmesh_legs, true);
	STRINGLIST_ADD(item.soaked.textureoption, true);

/*
    GetLSRManager()->AddSymbol(OVLT_UNIQUE);
    GetLSRManager()->AddLoader(OVLT_UNIQUE);
    GetStringTable()->AddSymbolString(item.name.c_str(), ovlCIDManager::TAG);

    GetLSRManager()->AddSymRef(OVLT_UNIQUE);
    GetStringTable()->AddSymbolString(item.nametxt.c_str(), ovlTXTManager::TAG);
    GetLSRManager()->AddSymRef(OVLT_UNIQUE);
    GetStringTable()->AddSymbolString(item.pluralnametxt.c_str(), ovlTXTManager::TAG);
    // The following is not an error, if not set it's symref'd to :gsi
    GetLSRManager()->AddSymRef(OVLT_UNIQUE);
    GetStringTable()->AddSymbolString(item.icon.c_str(), ovlGSIManager::TAG);
    if (item.shape.shape1 != "") {
        GetLSRManager()->AddSymRef(OVLT_UNIQUE);
        GetStringTable()->AddSymbolString(item.shape.shape1.c_str(), ovlSHSManager::TAG);
    }
    if (item.shape.shape2 != "") {
        GetLSRManager()->AddSymRef(OVLT_UNIQUE);
        GetStringTable()->AddSymbolString(item.shape.shape2.c_str(), ovlSHSManager::TAG);
    }
    // The following is not an error, if not set it's symref'd to :cid
    GetLSRManager()->AddSymRef(OVLT_UNIQUE);
    GetStringTable()->AddSymbolString(item.trash.wrapper.c_str(), ovlCIDManager::TAG);
    for (vector<string>::const_iterator it = item.extras.begin(); it != item.extras.end(); ++it) {
        GetLSRManager()->AddSymRef(OVLT_UNIQUE);
        GetStringTable()->AddSymbolString(it->c_str(), ovlCEDManager::TAG);
    }
    if (item.shape.fts != "") {
        GetLSRManager()->AddSymRef(OVLT_UNIQUE);
        GetStringTable()->AddSymbolString(item.shape.fts.c_str(), ovlFTXManager::TAG);
    }
    GetStringTable()->AddString(item.soaked.male_morphmesh_body.c_str());
    GetStringTable()->AddString(item.soaked.male_morphmesh_legs.c_str());
    GetStringTable()->AddString(item.soaked.female_morphmesh_body.c_str());
    GetStringTable()->AddString(item.soaked.female_morphmesh_legs.c_str());
    GetStringTable()->AddString(item.soaked.textureoption.c_str());
	 */
}

void ovlCIDManager::Make(cOvlInfo* info) {
    DUMP_LOG("Trace: ovlCIDManager::Make()");
    Check("ovlCIDManager::Make");

    ovlOVLManager::Make(info);
    unsigned char* c_data = m_data;

    for (map<string, cCarriedItem>::iterator it = m_items.begin(); it != m_items.end(); ++it) {
        // Assign structs
        CarriedItem* c_item = reinterpret_cast<CarriedItem*>(c_data);
        c_data += sizeof(CarriedItem);

        if (it->second.extras.size()) {
            c_item->extracount = it->second.extras.size();
            c_item->extras = reinterpret_cast<CarriedItemExtra**>(c_data);
            c_data += c_item->extracount * 4;
            GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long*>(&c_item->extras));
        } else {
            c_item->extracount = 0;
            c_item->extras = NULL;
        }
        it->second.Fill(c_item);

		cLoader& loader = GetLSRManager()->assignIndexElement(OVLT_UNIQUE, it->first, ovlCIDManager::TAG, c_item);
		loader.assignSymbolReference(it->second.nametxt, ovlTXTManager::TAG, &c_item->name);
		loader.assignSymbolReference(it->second.pluralnametxt, ovlTXTManager::TAG, &c_item->pluralname);
		loader.assignSymbolReference(it->second.icon, ovlGSIManager::TAG, &c_item->GSI);
		
		loader.assignSymbolReference(it->second.shape.shape1, ovlSHSManager::TAG, &c_item->shape1, false);
		loader.assignSymbolReference(it->second.shape.shape2, ovlSHSManager::TAG, &c_item->shape2, false);
		
		loader.assignSymbolReference(it->second.trash.wrapper, ovlCIDManager::TAG, &c_item->wrapper);
		
        unsigned long c = 0;
		foreach(const string& extra, it->second.extras)
			loader.assignSymbolReference(extra, ovlCEDManager::TAG, &c_item->extras[c++]);
			
		loader.assignSymbolReference(it->second.shape.fts, ovlFTXManager::TAG, &c_item->fts, false);
		
		STRINGLIST_ASSIGN(c_item->male_morphmesh_body, it->second.soaked.male_morphmesh_body, true, GetStringTable(), GetRelocationManager());
		STRINGLIST_ASSIGN(c_item->male_morphmesh_legs, it->second.soaked.male_morphmesh_legs, true, GetStringTable(), GetRelocationManager());
		STRINGLIST_ASSIGN(c_item->female_morphmesh_body, it->second.soaked.female_morphmesh_body, true, GetStringTable(), GetRelocationManager());
		STRINGLIST_ASSIGN(c_item->female_morphmesh_legs, it->second.soaked.female_morphmesh_legs, true, GetStringTable(), GetRelocationManager());
		STRINGLIST_ASSIGN(c_item->textureoption, it->second.soaked.textureoption, true, GetStringTable(), GetRelocationManager());
		/*
        SymbolStruct* c_symbol = GetLSRManager()->MakeSymbol(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->first.c_str(), TAG), reinterpret_cast<unsigned long*>(c_item));
        GetLSRManager()->OpenLoader(OVLT_UNIQUE, TAG, reinterpret_cast<unsigned long*>(c_item), false, c_symbol);

        GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.nametxt.c_str(), ovlTXTManager::TAG),
                             reinterpret_cast<unsigned long*>(&c_item->name));
        GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.pluralnametxt.c_str(), ovlTXTManager::TAG),
                             reinterpret_cast<unsigned long*>(&c_item->pluralname));
        GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.icon.c_str(), ovlGSIManager::TAG),
                             reinterpret_cast<unsigned long*>(&c_item->GSI));
        if (it->second.shape.shape1 != "") {
            GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.shape.shape1.c_str(), ovlSHSManager::TAG),
                                 reinterpret_cast<unsigned long*>(&c_item->shape1));
        }
        if (it->second.shape.shape2 != "") {
            GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.shape.shape2.c_str(), ovlSHSManager::TAG),
                                 reinterpret_cast<unsigned long*>(&c_item->shape2));
        }
        GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.trash.wrapper.c_str(), ovlCIDManager::TAG),
                             reinterpret_cast<unsigned long*>(&c_item->wrapper));
        unsigned long c = 0;
        for (vector<string>::iterator iit = it->second.extras.begin(); iit != it->second.extras.end(); ++iit) {
            GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(iit->c_str(), ovlCEDManager::TAG),
                                 reinterpret_cast<unsigned long*>(&c_item->extras[c]));
            c_item->extras[c] = NULL;
            c++;
        }
        if (it->second.shape.fts != "") {
            GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.shape.fts.c_str(), ovlFTXManager::TAG),
                                 reinterpret_cast<unsigned long*>(&c_item->fts));
        }
		 
        c_item->male_morphmesh_body = GetStringTable()->FindString(it->second.soaked.male_morphmesh_body.c_str());
        GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long*>(&c_item->male_morphmesh_body));
        c_item->male_morphmesh_legs = GetStringTable()->FindString(it->second.soaked.male_morphmesh_legs.c_str());
        GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long*>(&c_item->male_morphmesh_legs));
        c_item->female_morphmesh_body = GetStringTable()->FindString(it->second.soaked.female_morphmesh_body.c_str());
        GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long*>(&c_item->female_morphmesh_body));
        c_item->female_morphmesh_legs = GetStringTable()->FindString(it->second.soaked.female_morphmesh_legs.c_str());
        GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long*>(&c_item->female_morphmesh_legs));
        c_item->textureoption = GetStringTable()->FindString(it->second.soaked.textureoption.c_str());
        GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long*>(&c_item->textureoption));

        GetLSRManager()->CloseLoader(OVLT_UNIQUE);
		 */

    }
}
