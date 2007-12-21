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


#include "ManagerCID.h"

#include "ManagerCED.h"
#include "ManagerFTX.h"
#include "ManagerGSI.h"
#include "ManagerSHS.h"
#include "ManagerSID.h"
#include "ManagerTXT.h"
#include "OVLDebug.h"
#include "OVLException.h"

const char* ovlCIDManager::LOADER = "FGDK";
const char* ovlCIDManager::NAME = "CarriedItem";
const char* ovlCIDManager::TAG = "cid";

void ovlCIDManager::AddItem(const cCarriedItem& item) {
    Check("ovlCIDManager::AddItem");
    if (item.name == "")
        throw EOvl("ovlCIDManager::AddItem called without name");
    if (m_items.find(item.name) != m_items.end())
        throw EOvl("ovlCIDManager::AddItem: Item with name '"+item.name+"' already exists");
    if (item.nametxt == "")
        throw EOvl("ovlCIDManager::AddItem called without name text");
    if (item.pluralnametxt == "")
        throw EOvl("ovlCIDManager::AddItem called without plural name text");
    if ((item.shape.shape1 == "") && (item.shape.shape2 == "") && (item.shape.fts == ""))
        throw EOvl("ovlCIDManager::AddItem called without shape info");
    if (((item.shape.shape1 != "") || (item.shape.shape2 != "")) && (item.shape.fts != ""))
        throw EOvl("ovlCIDManager::AddItem, cannot be both 3D and 2D");

    m_items[item.name] = item;

    // Main
    m_size += sizeof(CarriedItem);
    // Extras
    m_size += item.extras.size() * 4;


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

    }
}
