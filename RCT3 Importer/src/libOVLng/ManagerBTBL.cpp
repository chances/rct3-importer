///////////////////////////////////////////////////////////////////////////////
//
// New OVL creation library
// Manager class for BTBL structures
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


#include "ManagerBTBL.h"

#include "ManagerFLIC.h"
#include "OVLException.h"
#include "OVLng.h"

using namespace std;

const char* ovlBTBLManager::LOADER = "FGDK";
const char* ovlBTBLManager::NAME = "BmpTbl";
const char* ovlBTBLManager::TAG = "btbl";
const unsigned long ovlBTBLManager::TYPE = 1;

unsigned long cTextureMIP::CalcSize(unsigned long blocksize) const {
    return dimension * dimension * blocksize / 16;
}

void cTextureMIP::FillHeader(unsigned long blocksize, FlicMipHeader* header, int format) const {
    header->MHeight = dimension;
    header->MWidth = dimension;
	if (format == cTexture::FORMAT_A8R8G8B8) {
		header->Pitch = header->MWidth * blocksize / 16;
		header->Blocks = header->MHeight;
	} else {
		header->Pitch = header->MWidth * blocksize / 4;
		header->Blocks = header->MHeight / 4;
	}
}

unsigned long cTextureMIP::FillRawData(unsigned long blocksize, unsigned char* _data) const {
    unsigned long size = CalcSize(blocksize);
    memcpy(_data, data.get(), size);
    return size;
}


unsigned long cTexture::CalcSize() const {
    unsigned long size = 0;
	unsigned long blocksize = GetBlockSize(format);
    for (set<cTextureMIP>::const_iterator it = mips.begin(); it != mips.end(); ++it) {
        size += it->CalcSize(blocksize);
    }

    return size;
}

void cTexture::Check() const {
    if (!mips.size())
        throw EOvl("Texture misses mips: "+name);

    GetBlockSize(format); // Throws on wrong format

    unsigned long d = mips.begin()->dimension;
    for (set<cTextureMIP>::const_iterator it = mips.begin(); it != mips.end(); ++it) {
        if (it->dimension != d)
            throw EOvl("Texture mips are inconsistent: "+name);
        d = d / 2;
    }

}

void cTexture::FillHeader(FlicHeader* header) const {
    header->Format = format;
    header->Width = mips.begin()->dimension;
    header->Height = mips.begin()->dimension;
    header->Mipcount = mips.size();
}

unsigned long cTexture::FillRawData(unsigned char* data) const {
    unsigned long size = 0;
	unsigned long blocksize = GetBlockSize(format);
    for (set<cTextureMIP>::const_iterator it = mips.begin(); it != mips.end(); ++it) {
        size += it->FillRawData(blocksize, data + size);
    }
    return size;
}

unsigned long cTexture::GetBlockSize(unsigned long _format) {
    switch (_format) {
        case FORMAT_A8R8G8B8:
            return 16 * 4;
            break;
        case FORMAT_DXT1:
            return 8;
            break;
        case FORMAT_DXT3:
        case FORMAT_DXT5:
            return 16;
            break;
        default:
            throw EOvl("Unknown format in cTexture::GetBlockSize()");
    }
}

unsigned long cTexture::GetDimension(unsigned long _format, unsigned long _size) {
    return sqrt(_size * 16 / cTexture::GetBlockSize(_format));
}

void ovlBTBLManager::Init(cOvl* ovl) {
    ovlOVLManager::Init(ovl);
    if (ovl->HasManager<ovlFLICManager>())
        throw EOvl("Tried to init BmpTbl after adding textures");

    GetLSRManager()->AddLoader(OVLT_COMMON);
}

unsigned long ovlBTBLManager::AddTexture(const cTexture& item) {
    Check("ovlBTBLManager::AddTexture");

    m_items.push_back(item);

    m_size += item.CalcSize();

    return m_items.size() - 1;
}

void ovlBTBLManager::Make(cOvlInfo* info) {
    Check("ovlBTBLManager::Make");
    if (!info)
        throw EOvl("ovlBTBLManager::Make called without valid info");

    m_blobs[""] = cOvlMemBlob(OVLT_COMMON, 2, 8); // BmpTbl
    ovlOVLManager::Make(info);
    BmpTbl* c_tbl = reinterpret_cast<BmpTbl*>(m_blobs[""].data);
    c_tbl->unk = 0;
    c_tbl->count = m_items.size();

    GetLSRManager()->OpenLoader(OVLT_COMMON, TAG, reinterpret_cast<unsigned long*>(&c_tbl), 2, NULL);
    unsigned char* c_extradata1 = new unsigned char[8 + (c_tbl->count * sizeof(FlicHeader))];
    unsigned char* c_extra1 = c_extradata1;
    *reinterpret_cast<unsigned long*>(c_extra1) = 0;
    c_extra1 += 4;
    *reinterpret_cast<unsigned long*>(c_extra1) = 0;
    c_extra1 += 4;
    FlicHeader* c_headers = reinterpret_cast<FlicHeader*>(c_extra1);

    unsigned char* c_extradata2 = new unsigned char[m_size];
    unsigned char* c_extra2 = c_extradata2;

    for (vector<cTexture>::iterator it = m_items.begin(); it != m_items.end(); ++it) {
        it->FillHeader(c_headers);
        c_headers++;
        c_extra2 += it->FillRawData(c_extra2);
    }

    GetLSRManager()->AddExtraData(OVLT_COMMON, 8 + (c_tbl->count * sizeof(FlicHeader)), c_extradata1);
    GetLSRManager()->AddExtraData(OVLT_COMMON, m_size, c_extradata2);

    GetLSRManager()->CloseLoader(OVLT_COMMON);

}
