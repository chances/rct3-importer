///////////////////////////////////////////////////////////////////////////////
//
// New OVL creation library
// Manager class for FLIC structures
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


#include "ManagerFLIC.h"

#include "OVLException.h"
#include "OVLng.h"

using namespace std;

const char* ovlFLICManager::LOADER = "FGDK";
const char* ovlFLICManager::NAME = "Flic";
const char* ovlFLICManager::TAG = "flic";
//const unsigned long ovlFLICManager::TYPE = 2;

void ovlFLICManager::Init(cOvl* ovl) {
    ovlOVLManager::Init(ovl);
    if (ovl->HasManager<ovlBTBLManager>()) {
        TYPE = 3;
        m_btbl = ovl->GetManager<ovlBTBLManager>();
    } else {
        TYPE = 2;
    }
}

void ovlFLICManager::AddTexture(const cTexture& item) {
    Check("ovlFLICManager::AddTexture");
    item.Check();

    unsigned long index;
    if (m_btbl) {
        index = m_btbl->AddTexture(item);
    } else {
        index = m_items.size();
    }
    m_items[index] = item;
    m_itemmap[item.name] = index;

    // FlicStruct and a pointer. Rest goes to extradata
    m_size += sizeof(Flic); //sizeof(FlicStruct) + 4;

    GetLSRManager()->AddLoader(OVLT_COMMON);
}

FlicStruct** ovlFLICManager::GetPointer1(const string& name) {
    if (!IsMade())
        throw EOvl("ovlFLICManager::GetPointer1 called in unmade state");

    map<string, unsigned long>::iterator it = m_itemmap.find(name);
    if (it == m_itemmap.end())
        throw EOvl("ovlFLICManager::GetPointer1 called with unknown texture name");
    map<unsigned long, cTexture>::iterator it2 = m_items.find(it->second);

    return it2->second.madep1;
}

FlicStruct* ovlFLICManager::GetPointer2(const string& name) {
    if (!IsMade())
        throw EOvl("ovlFLICManager::GetPointer2 called in unmade state");

    map<string, unsigned long>::iterator it = m_itemmap.find(name);
    if (it == m_itemmap.end())
        throw EOvl("ovlFLICManager::GetPointer2 called with unknown texture name");
    map<unsigned long, cTexture>::iterator it2 = m_items.find(it->second);

    return it2->second.madep2;
}

void ovlFLICManager::Make(cOvlInfo* info) {
    Check("ovlFLICManager::Make");
    if (!info)
        throw EOvl("ovlFLICManager::Make called without valid info");

    m_blobs[""] = cOvlMemBlob(OVLT_COMMON, 2, m_size);
    ovlOVLManager::Make(info);
    unsigned char* c_data = m_blobs[""].data;

    for (map<unsigned long, cTexture>::iterator it = m_items.begin(); it != m_items.end(); ++it) {
        Flic* c_flic = reinterpret_cast<Flic*>(c_data);
        c_data += sizeof(Flic);
        c_flic->fl = &c_flic->fl2;
        GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long*>(&c_flic->fl));

        // Fill FlicStruct
        c_flic->fl2.FlicDataPtr = 0;
        c_flic->fl2.unk1 = it->second.flic.unk1;
        c_flic->fl2.unk2 = it->second.flic.unk2;

        // Strore pointers for texture reference
        it->second.madep1 = &c_flic->fl;
        it->second.madep2 = &c_flic->fl2;

        GetLSRManager()->OpenLoader(OVLT_COMMON, TAG, reinterpret_cast<unsigned long*>(&c_flic->fl2), 1, NULL);

        // Extra Data
        if (m_btbl) {
            // easy
            unsigned long* index = new unsigned long[1];
            *index = it->first;
            GetLSRManager()->AddExtraData(OVLT_COMMON, 4, reinterpret_cast<unsigned char*>(index));
        } else {
            unsigned long c_extrasize = sizeof(FlicHeader) + (it->second.mips.size() * sizeof(FlicMipHeader)) + it->second.CalcSize() + sizeof(FlicMipHeader);
            unsigned char* c_extradata = new unsigned char[c_extrasize];
            unsigned char* c_extra = c_extradata;
            FlicHeader* c_header = reinterpret_cast<FlicHeader*>(c_extra);
            c_extra += sizeof(FlicHeader);
            it->second.FillHeader(c_header);

            FlicMipHeader* c_mip;
            for (set<cTextureMIP>::iterator i_mip = it->second.mips.begin(); i_mip != it->second.mips.end(); ++i_mip) {
                c_mip = reinterpret_cast<FlicMipHeader*>(c_extra);
                c_extra += sizeof(FlicMipHeader);
                i_mip->FillHeader(cTexture::GetBlockSize(it->second.format), c_mip, it->second.format);

                c_extra += i_mip->FillRawData(cTexture::GetBlockSize(it->second.format), c_extra);
            }

            c_mip = reinterpret_cast<FlicMipHeader*>(c_extra);
            c_mip->MHeight = 0;
            c_mip->MWidth = 0;
            c_mip->Pitch = 0;
            c_mip->Blocks = 0;

            GetLSRManager()->AddExtraData(OVLT_COMMON, c_extrasize, c_extradata);
        }
        GetLSRManager()->CloseLoader(OVLT_COMMON);
    }

    if (m_defermake) {
        m_defermake->Make(info);
    }
}


/*
ovlFLICManager::~ovlFLICManager() {
    for (map<string, FlicInternal>::iterator it = m_flics.begin(); it != m_flics.end(); ++it) {
        delete[] reinterpret_cast<unsigned char*>(it->second.flic.FlicDataPtr);
    }
}

void ovlFLICManager::AddTexture(const string& name, unsigned long dimension, unsigned long size, unsigned long* data) {
    Check("ovlFLICManager::AddTexture");

    FlicInternal fi;
    fi.header.Format = 0x13;
    fi.header.Width = dimension;
    fi.header.Height = dimension;
    fi.header.Mipcount = 0;
    fi.flic.FlicDataPtr = reinterpret_cast<unsigned long*>(new unsigned char [size]);
    memcpy(fi.flic.FlicDataPtr, data, size);
    fi.flic.unk1 = 1;
    fi.flic.unk2 = 1.0;
    fi.datasize = size;
    fi.madep1 = NULL;
    fi.madep2 = NULL;

    m_flics[name] = fi;

    // FlicStruct and a pointer. Rest goes to extradata
    m_size += sizeof(Flic); //sizeof(FlicStruct) + 4;

    GetLSRManager()->AddLoader(OVLT_COMMON);
}

void ovlFLICManager::SetParameters(const string& name, unsigned long format, unsigned long unk1, float unk2) {
    Check("ovlFLICManager::SetParameters");

    map<string, FlicInternal>::iterator it = m_flics.find(name);
    if (it == m_flics.end())
        throw EOvl("ovlFLICManager::SetParameters called with unknown texture name");

    it->second.header.Format = format;
    it->second.flic.unk1 = unk1;
    it->second.flic.unk2 = unk2;
}

FlicStruct** ovlFLICManager::GetPointer1(const string& name) {
    if (!IsMade())
        throw EOvl("ovlFLICManager::GetPointer1 called in unmade state");

    map<string, FlicInternal>::iterator it = m_flics.find(name);
    if (it == m_flics.end())
        throw EOvl("ovlFLICManager::GetPointer1 called with unknown texture name");

    return it->second.madep1;
}

FlicStruct* ovlFLICManager::GetPointer2(const string& name) {
    if (!IsMade())
        throw EOvl("ovlFLICManager::GetPointer2 called in unmade state");

    map<string, FlicInternal>::iterator it = m_flics.find(name);
    if (it == m_flics.end())
        throw EOvl("ovlFLICManager::GetPointer2 called with unknown texture name");

    return it->second.madep2;
}

void ovlFLICManager::Make(cOvlInfo* info) {
    Check("ovlFLICManager::Make");
    if (!info)
        throw EOvl("ovlFTXManager::Make called without valid info");

    m_blobs[""] = cOvlMemBlob(OVLT_COMMON, 2, m_size);
    ovlOVLManager::Make(info);
    unsigned char* c_data = m_blobs[""].data;

    for (map<string, FlicInternal>::iterator it = m_flics.begin(); it != m_flics.end(); ++it) {
        // Data Transfer etc. We use the Flic helper struct to get the pointer right.
        Flic* c_flic = reinterpret_cast<Flic*>(c_data);
        c_data += sizeof(Flic);
        c_flic->fl = &c_flic->fl2;
        GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long*>(&c_flic->fl));

        // Fill FlicStruct
        c_flic->fl2.FlicDataPtr = 0;
        c_flic->fl2.unk1 = it->second.flic.unk1;
        c_flic->fl2.unk2 = it->second.flic.unk2;

        // Strore pointers for texture reference
        it->second.madep1 = &c_flic->fl;
        it->second.madep2 = &c_flic->fl2;

        GetLSRManager()->OpenLoader(OVLT_COMMON, TAG, reinterpret_cast<unsigned long*>(&c_flic->fl2), 1, NULL);

        // Extra data, in this case most of the data...
        unsigned long c_extrasize = sizeof(FlicHeader) + sizeof(FlicMipHeader) + it->second.datasize + sizeof(FlicMipHeader);
        unsigned char* c_extradata = new unsigned char[c_extrasize];
        unsigned char* c_extra = c_extradata;
        FlicHeader* c_header = reinterpret_cast<FlicHeader*>(c_extra);
        c_extra += sizeof(FlicHeader);
        *c_header = it->second.header;

        FlicMipHeader* c_mip = reinterpret_cast<FlicMipHeader*>(c_extra);
        c_extra += sizeof(FlicMipHeader);
        c_mip->MHeight = c_header->Height;
        c_mip->MWidth = c_header->Width;
        c_mip->Pitch = c_mip->MWidth * 4;
        c_mip->Blocks = it->second.datasize / c_mip->Pitch;

        memcpy(c_extra, it->second.flic.FlicDataPtr, it->second.datasize);
        c_extra += it->second.datasize;

        c_mip = reinterpret_cast<FlicMipHeader*>(c_extra);
        c_mip->MHeight = 0;
        c_mip->MWidth = 0;
        c_mip->Pitch = 0;
        c_mip->Blocks = 0;

        GetLSRManager()->AddExtraData(OVLT_COMMON, c_extrasize, c_extradata);

        GetLSRManager()->CloseLoader(OVLT_COMMON);
    }

    if (m_defermake) {
        m_defermake->Make(info);
    }
}
*/
