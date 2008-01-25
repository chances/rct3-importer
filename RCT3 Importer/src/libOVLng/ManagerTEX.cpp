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

const char* ovlTEXManager::LOADER = "FGDK";
const char* ovlTEXManager::NAME = "Texture";
const char* ovlTEXManager::TAG = "tex";
const unsigned long ovlTEXManager::TYPE = 2;

//#define GUIICON "GUIIcon"

void ovlTEXManager::Init(cOvl* ovl) {
    ovlOVLManager::Init(ovl);
    m_flicman = ovl->GetManager<ovlFLICManager>();
}

void ovlTEXManager::AddTexture(const cTextureStruct& item) {
    DUMP_LOG("Trace: ovlTEXManager::AddTexture(%s)", UNISTR(item.texture.name.c_str()));
    Check("ovlTEXManager::AddTexture");
    if (item.texture.name == "")
        throw EOvl("ovlTEXManager::AddTexture called without name");
    if (m_items.find(item.texture.name) != m_items.end())
        throw EOvl("ovlTEXManager::AddTexture: Item with name '"+item.texture.name+"' already exists");

    m_items[item.texture.name] = item;
    m_flicman->AddTexture(item.texture);

    // TextureStruct + TextureStruct2
    m_size += sizeof(Tex);

    GetLSRManager()->AddLoader(OVLT_UNIQUE);
    GetLSRManager()->AddSymbol(OVLT_UNIQUE);
    GetLSRManager()->AddSymRef(OVLT_UNIQUE);
    GetStringTable()->AddSymbolString(item.texture.name.c_str(), Tag());
    GetStringTable()->AddSymbolString(item.texturestyle.c_str(), ovlTXSManager::TAG);
}

void ovlTEXManager::Make(cOvlInfo* info) {
    Check("ovlTEXManager::Make");
    if (!info)
        throw EOvl("ovlTEXManager::Make called without valid info");

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

        SymbolStruct* s_tex = GetLSRManager()->MakeSymbol(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->first.c_str(), Tag()), reinterpret_cast<unsigned long*>(c_tex));
        GetLSRManager()->OpenLoader(OVLT_UNIQUE, TAG, reinterpret_cast<unsigned long*>(c_tex), 0, s_tex);
        GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.texturestyle.c_str(), ovlTXSManager::TAG), reinterpret_cast<unsigned long*>(&c_tex->t1.TextureData));
        GetLSRManager()->CloseLoader(OVLT_UNIQUE);
    }
}

/*
void ovlTEXManager::Init(cOvl* ovl) {
    ovlOVLManager::Init(ovl);
    GetStringTable()->AddSymbolString(GUIICON, ovlTXSManager::TAG);
    m_flicman = ovl->GetManager<ovlFLICManager>();
}

void ovlTEXManager::AddTexture(const string& name, unsigned long dimension, unsigned long size, unsigned long* data) {
    Check("ovlTEXManager::AddTexture");

    m_flicman->AddTexture(name, dimension, size, data);
    TextureStruct ts;
    ts.unk1 = 0x70007;
    ts.unk2 = 0x70007;
    ts.unk3 = 0x70007;
    ts.unk4 = 0x70007;
    ts.unk5 = 0x70007;
    ts.unk6 = 0x70007;
    ts.unk7 = 0x70007;
    ts.unk8 = 0x70007;
    ts.unk9 = 1;
    ts.unk10 = 8;
    ts.unk11 = 0x10;
    ts.TextureData = NULL;
    ts.unk12 = 1;
    ts.Flic = NULL;
    ts.ts2 = NULL;

    m_textures[name] = ts;

    // TextureStruct + TextureStruct2
    m_size += sizeof(Tex);

    GetLSRManager()->AddLoader(OVLT_UNIQUE);
    GetLSRManager()->AddSymbol(OVLT_UNIQUE);
    GetLSRManager()->AddSymRef(OVLT_UNIQUE);
    GetStringTable()->AddSymbolString(name.c_str(), Tag());
}

void ovlTEXManager::SetUnknowns18(const string& name, unsigned long unk1, unsigned long unk2, unsigned long unk3, unsigned long unk4, unsigned long unk5, unsigned long unk6, unsigned long unk7, unsigned long unk8) {
    Check("ovlTEXManager::SetUnknowns18");

    map<string, TextureStruct>::iterator it = m_textures.find(name);
    if (it == m_textures.end())
        throw EOvl("ovlTEXManager::SetUnknowns18 called with unknown texture name");

    it->second.unk1 = unk1;
    it->second.unk2 = unk2;
    it->second.unk3 = unk3;
    it->second.unk4 = unk4;
    it->second.unk5 = unk5;
    it->second.unk6 = unk6;
    it->second.unk7 = unk7;
    it->second.unk8 = unk8;
}

void ovlTEXManager::SetUnknowns912(const string& name, unsigned long unk9, unsigned long unk10, unsigned long unk11, unsigned long unk12) {
    Check("ovlTEXManager::SetUnknowns912");

    map<string, TextureStruct>::iterator it = m_textures.find(name);
    if (it == m_textures.end())
        throw EOvl("ovlTEXManager::SetUnknowns912 called with unknown texture name");

    it->second.unk9 = unk9;
    it->second.unk10 = unk10;
    it->second.unk11 = unk11;
    it->second.unk12 = unk12;
}


void ovlTEXManager::Make(cOvlInfo* info) {
    Check("ovlTEXManager::Make");
    if (!info)
        throw EOvl("ovlFTXManager::Make called without valid info");

    if (!m_flicman->IsMade()) {
        m_flicman->DeferMake(this);
        return;
    }

    m_blobs[""] = cOvlMemBlob(OVLT_UNIQUE, 2, m_size);
    ovlOVLManager::Make(info);
    unsigned char* c_data = m_blobs[""].data;

    for (map<string, TextureStruct>::iterator it = m_textures.begin(); it != m_textures.end(); ++it) {
        Tex* c_tex = reinterpret_cast<Tex*>(c_data);
        c_data += sizeof(Tex);
        c_tex->t1 = it->second;
        c_tex->t1.ts2 = &c_tex->t2;
        GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long*>(&c_tex->t1.ts2));
        c_tex->t1.Flic = m_flicman->GetPointer1(it->first);
        GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long*>(&c_tex->t1.Flic));

        c_tex->t2.Texture = &c_tex->t1;
        GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long*>(&c_tex->t2.Texture));
        c_tex->t2.Flic = m_flicman->GetPointer2(it->first);
        GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long*>(&c_tex->t2.Flic));

        SymbolStruct* s_tex = GetLSRManager()->MakeSymbol(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->first.c_str(), Tag()), reinterpret_cast<unsigned long*>(c_tex));
        GetLSRManager()->OpenLoader(OVLT_UNIQUE, TAG, reinterpret_cast<unsigned long*>(c_tex), 0, s_tex);
        GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(GUIICON, ovlTXSManager::TAG), reinterpret_cast<unsigned long*>(&c_tex->t1.TextureData));
        GetLSRManager()->CloseLoader(OVLT_UNIQUE);
    }
}
*/
