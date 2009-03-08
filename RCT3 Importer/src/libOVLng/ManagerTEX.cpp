///////////////////////////////////////////////////////////////////////////////
//
// New OVL creation library
// Manager class for TEX structures
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

#include "ManagerTEX.h"

#include "ManagerCommon.h"
#include "ManagerFLIC.h"
#include "OVLException.h"
#include "OVLng.h"

using namespace std;
using namespace r3;

const char* ovlTEXManager::LOADER = "FGDK";
const char* ovlTEXManager::NAME = "Texture";
const char* ovlTEXManager::TAG = "tex";
const unsigned long ovlTEXManager::TYPE = 2;

//#define GUIICON "GUIIcon"

void cTextureStruct::Fill(TextureStruct* ts) {
    ts->unk1 = unk1;
    ts->unk2 = unk2;
    ts->unk3 = unk3;
    ts->unk4 = unk4;
    ts->unk5 = unk5;
    ts->unk6 = unk6;
    ts->unk7 = unk7;
    ts->unk8 = unk8;
    ts->unk9 = unk9;
    ts->unk10 = unk10;
    ts->unk11 = unk11;
    ts->TextureData = NULL;
    ts->unk12 = unk12;
}

void ovlTEXManager::Init(cOvl* ovl) {
    ovlOVLManager::Init(ovl);
    m_flicman = ovl->GetManager<ovlFLICManager>();
}

void ovlTEXManager::AddTexture(const cTextureStruct& item) {
    DUMP_LOG("Trace: ovlTEXManager::AddTexture(%s)", UNISTR(item.texture.name.c_str()));
    Check("ovlTEXManager::AddTexture");
    if (item.texture.name == "")
        BOOST_THROW_EXCEPTION(EOvl("ovlTEXManager::AddTexture called without name"));
    if (m_items.find(item.texture.name) != m_items.end())
        BOOST_THROW_EXCEPTION(EOvl("ovlTEXManager::AddTexture: Item with name '"+item.texture.name+"' already exists"));

    m_items[item.texture.name] = item;
    m_flicman->AddTexture(item.texture);

    // TextureStruct + TextureStruct2
    m_size += sizeof(Tex);

	cLoader& loader = GetLSRManager()->reserveIndexElement(OVLT_UNIQUE, item.texture.name, ovlTEXManager::TAG);
	loader.reserveSymbolReference(item.texturestyle, ovlTXSManager::TAG);

/*
    GetLSRManager()->AddLoader(OVLT_UNIQUE);
    GetLSRManager()->AddSymbol(OVLT_UNIQUE);
    GetLSRManager()->AddSymRef(OVLT_UNIQUE);
    GetStringTable()->AddSymbolString(item.texture.name.c_str(), Tag());
    GetStringTable()->AddSymbolString(item.texturestyle.c_str(), ovlTXSManager::TAG);
	 */
}

void ovlTEXManager::Make(cOvlInfo* info) {
    Check("ovlTEXManager::Make");
    if (!info)
        BOOST_THROW_EXCEPTION(EOvl("ovlTEXManager::Make called without valid info"));

    if (!m_flicman->IsMade()) {
        m_flicman->DeferMake(this);
        return;
    }

    m_blobs[""] = cOvlMemBlob(OVLT_UNIQUE, 2, m_size);
    ovlOVLManager::Make(info);
    unsigned char* c_data = m_blobs[""].data;

    for (map<string, cTextureStruct>::iterator it = m_items.begin(); it != m_items.end(); ++it) {
        Tex* c_tex = reinterpret_cast<Tex*>(c_data);
        c_data += sizeof(Tex);
        it->second.Fill(&c_tex->t1);
        c_tex->t1.ts2 = &c_tex->t2;
        GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long*>(&c_tex->t1.ts2));
        c_tex->t1.Flic = m_flicman->GetPointer1(it->first);
        GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long*>(&c_tex->t1.Flic));

        c_tex->t2.Texture = &c_tex->t1;
        GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long*>(&c_tex->t2.Texture));
        c_tex->t2.Flic = m_flicman->GetPointer2(it->first);
        GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long*>(&c_tex->t2.Flic));
		
		cLoader& loader = GetLSRManager()->assignIndexElement(OVLT_UNIQUE, it->first, ovlTEXManager::TAG, c_tex);
		loader.assignSymbolReference(it->second.texturestyle, ovlTXSManager::TAG, &c_tex->t1.TextureData);
		/*
        SymbolStruct* s_tex = GetLSRManager()->MakeSymbol(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->first.c_str(), Tag()), reinterpret_cast<unsigned long*>(c_tex));
        GetLSRManager()->OpenLoader(OVLT_UNIQUE, TAG, reinterpret_cast<unsigned long*>(c_tex), 0, s_tex);
        GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.texturestyle.c_str(), ovlTXSManager::TAG), reinterpret_cast<unsigned long*>(&c_tex->t1.TextureData));
        GetLSRManager()->CloseLoader(OVLT_UNIQUE);
		 */
    }
}
