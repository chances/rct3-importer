///////////////////////////////////////////////////////////////////////////////
//
// New OVL creation library
// Manager class for SAT structures
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

#include "ManagerSAT.h"

#include "ManagerGSI.h"
#include "ManagerSID.h"
#include "ManagerSPL.h"
#include "ManagerTXT.h"
#include "OVLException.h"

using namespace r3;
using namespace std;

const char* ovlSATManager::LOADER = "FGDK";
const char* ovlSATManager::NAME = "SpecialAttraction";
const char* ovlSATManager::TAG = "sat";

void cSpecialAttraction::Fill(r3old::SpecialAttractionA* sp) {
    sp->Name = NULL;
    sp->Description = NULL;
    sp->GSI = NULL;
    sp->spline = NULL;
    attraction.Fill(sp);
}
void cSpecialAttraction::Fill(r3old::SpecialAttractionB* sp) {
    if ((attraction.type & r3::Constants::Addon::Wild_Hi) == r3::Constants::Addon::Wild_Hi) {
        sp->unk = r3::Constants::Addon::Wild;
    } else {
        sp->unk = r3::Constants::Addon::Soaked;
    }
    attraction.Fill(sp->att);
}

void ovlSATManager::AddAttraction(const cSpecialAttraction& item) {
    Check("ovlSATManager::AddAttraction");
    if (item.name == "")
        BOOST_THROW_EXCEPTION(EOvl("ovlSATManager::AddAttraction called without name"));
    if (m_items.find(item.name) != m_items.end())
        BOOST_THROW_EXCEPTION(EOvl("ovlSATManager::AddAttraction: Item with name '"+item.name+"' already exists"));
    if (item.sid == "")
        BOOST_THROW_EXCEPTION(EOvl("ovlSATManager::AddAttraction called without sid"));
    if (item.attraction.name == "")
        BOOST_THROW_EXCEPTION(EOvl("ovlSATManager::AddAttraction called without name text"));
    if (item.attraction.description == "")
        BOOST_THROW_EXCEPTION(EOvl("ovlSATManager::AddAttraction called without description text"));

    m_items[item.name] = item;

    // The following depends on the type of stall structure we want to write
    if (item.attraction.type & r3::Constants::Addon::Soaked_Hi) {
        // SpecialAttraction2
        m_size += sizeof(r3old::SpecialAttractionB);
        if ((item.attraction.type & r3::Constants::Addon::Wild_Hi) == r3::Constants::Addon::Wild_Hi) {
            m_size += sizeof(Attraction_W);
        } else {
            m_size += sizeof(Attraction_S);
        }
    } else {
        // SpecialAttraction
        m_size += sizeof(r3old::SpecialAttractionA);
    }

    GetLSRManager()->AddSymbol(OVLT_UNIQUE);
    GetLSRManager()->AddLoader(OVLT_UNIQUE);
    GetStringTable()->AddSymbolString(item.name.c_str(), ovlSATManager::TAG);

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

}

void ovlSATManager::Make(cOvlInfo* info) {
    DUMP_LOG("Trace: ovlSATManager::Make()");
    Check("ovlSATManager::Make");

    ovlOVLManager::Make(info);
    unsigned char* c_data = m_data;

    for (map<string, cSpecialAttraction>::iterator it = m_items.begin(); it != m_items.end(); ++it) {
        if (it->second.attraction.type & r3::Constants::Addon::Soaked_Hi) {
            // SpecialAttraction2
            // Assign structs
            r3old::SpecialAttractionB* c_att = reinterpret_cast<r3old::SpecialAttractionB*>(c_data);
            c_data += sizeof(r3old::SpecialAttractionB);

            c_att->att = reinterpret_cast<Attraction_S*>(c_data);
            if ((it->second.attraction.type & r3::Constants::Addon::Wild_Hi) == r3::Constants::Addon::Wild_Hi) {
                c_data += sizeof(Attraction_W);
            } else {
                c_data += sizeof(Attraction_S);
            }
            GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long*>(&c_att->att));

            it->second.Fill(c_att);

            SymbolStruct* c_symbol = GetLSRManager()->MakeSymbol(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->first.c_str(), TAG), reinterpret_cast<unsigned long*>(c_att));
            GetLSRManager()->OpenLoader(OVLT_UNIQUE, TAG, reinterpret_cast<unsigned long*>(c_att), false, c_symbol);

            GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.attraction.name.c_str(), ovlTXTManager::TAG),
                                 reinterpret_cast<unsigned long*>(&c_att->att->v.name_ref));
            GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.attraction.description.c_str(), ovlTXTManager::TAG),
                                 reinterpret_cast<unsigned long*>(&c_att->att->v.description_ref));
            GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.sid.c_str(), ovlSIDManager::TAG),
                                 reinterpret_cast<unsigned long*>(&c_att->SID));
            GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.attraction.icon.c_str(), ovlGSIManager::TAG),
                                 reinterpret_cast<unsigned long*>(&c_att->att->v.icon_ref));
            GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.attraction.spline.c_str(), ovlSPLManager::TAG),
                                 reinterpret_cast<unsigned long*>(&c_att->att->v.spline_ref));

            GetLSRManager()->CloseLoader(OVLT_UNIQUE);
        } else {
            // Stall
            // Assign structs
            r3old::SpecialAttractionA* c_att = reinterpret_cast<r3old::SpecialAttractionA*>(c_data);
            c_data += sizeof(r3old::SpecialAttractionA);

            it->second.Fill(c_att);

            SymbolStruct* c_symbol = GetLSRManager()->MakeSymbol(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->first.c_str(), TAG), reinterpret_cast<unsigned long*>(c_att));
            GetLSRManager()->OpenLoader(OVLT_UNIQUE, TAG, reinterpret_cast<unsigned long*>(c_att), false, c_symbol);

            GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.attraction.name.c_str(), ovlTXTManager::TAG),
                                 reinterpret_cast<unsigned long*>(&c_att->Name));
            GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.attraction.description.c_str(), ovlTXTManager::TAG),
                                 reinterpret_cast<unsigned long*>(&c_att->Description));
            GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.sid.c_str(), ovlSIDManager::TAG),
                                 reinterpret_cast<unsigned long*>(&c_att->SID));
            GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.attraction.icon.c_str(), ovlGSIManager::TAG),
                                 reinterpret_cast<unsigned long*>(&c_att->GSI));
            GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.attraction.spline.c_str(), ovlSPLManager::TAG),
                                 reinterpret_cast<unsigned long*>(&c_att->spline));

            GetLSRManager()->CloseLoader(OVLT_UNIQUE);
        }

    }
}
