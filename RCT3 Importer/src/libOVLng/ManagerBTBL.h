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


#ifndef MANAGERBTBL_H_INCLUDED
#define MANAGERBTBL_H_INCLUDED

#include <boost/shared_array.hpp>
#include <vector>
#include <set>
#include <string>
#include <math.h>

#include "ManagerOVL.h"

#include "icontexture.h"

using namespace std;

/*
Here is how complex flics work:

Flic Main Data:
Stores a relocated pointer to the following FlicStruct. This is eased by the Filc struct.
The FlicStruct is a data pointer which is 0 on disk and two unknowns. Unknown 1 is 1 and
Unknown 2 is 1.0.

Flic Extra Data:
Stores a long int, an index into the BitmayTable.

Btbl Main Data:
Stores a BmpTbl struct.

Btbl Extra Data:
First chunk stores two longs which are always 0. Then an array of FlicHeaders for the
textures.
Second chunk strores the raw data without FLicMipHeaders.

*/

class cTextureMIP {
public:
    unsigned long dimension;
    //unsigned long datasize;
    boost::shared_array<unsigned char> data;

    cTextureMIP(): dimension(0)/*, datasize(0)*/ {}
    bool operator< (const cTextureMIP& rhs) const {
        return dimension > rhs.dimension;
    }
    unsigned long CalcSize(unsigned long blocksize) const;
    void FillHeader(unsigned long blocksize, FlicMipHeader* header) const;
    unsigned long FillRawData(unsigned long blocksize, unsigned char* _data) const;
};

class cTexture {
public:
    enum {
        FORMAT_R8G8B8 =     0x01,
        FORMAT_A8R8G8B8 =   0x02,  ///< Seen (32 bpp)
        FORMAT_X8R8G8B8 =   0x03,
        FORMAT_R5G6B5 =     0x04,
        FORMAT_X1R5G5B5 =   0x05,
        FORMAT_P8 =         0x07,
        FORMAT_A1R5G5B5 =   0x08,
        FORMAT_X4R4G4B4 =   0x09,
        FORMAT_A4R4G4B4 =   0x0A,
        FORMAT_L8 =         0x0B,
        FORMAT_A8L8 =       0x0C,
        FORMAT_V8U8 =       0x0E,
        FORMAT_UYVY =       0x10,
        FORMAT_YUY2 =       0x11,
        FORMAT_DXT1 =       0x12,  ///< Seen (4 bpp)
        FORMAT_DXT3 =       0x13,  ///< Seen (8 bpp)
        FORMAT_DXT5 =       0x14,
        FORMAT_R3G3B2 =     0x15,
        FORMAT_A8 =         0x16,
        FORMAT_D16 =        0x100,
        FORMAT_D32 =        0x101,
        FORMAT_D15S1 =      0x102,
        FORMAT_D24S8 =      0x103
    };
    string name;
    unsigned long format;
    set<cTextureMIP> mips;

    // Used by ovlFLICManager
    unsigned long flicindex;
    FlicStruct flic;
    FlicStruct** madep1;
    FlicStruct* madep2;

    cTexture(): format(FORMAT_DXT3), flicindex(-1), madep1(NULL), madep2(NULL) {
        flic.FlicDataPtr = NULL;
        flic.unk1 = 1;
        flic.unk2 = 1.0;
    }

    unsigned long CalcSize() const;
    void Check() const;
    void FillHeader(FlicHeader* header) const;
    unsigned long FillRawData(unsigned char* data) const;

    static unsigned long GetBlockSize(unsigned long _format);
    static unsigned long GetDimension(unsigned long _format, unsigned long _size);
};

class ovlBTBLManager: public ovlOVLManager {
public:
    static const char* LOADER;
    static const char* NAME;
    static const char* TAG;
    static const unsigned long TYPE;
private:
    vector<cTexture> m_items;
public:
    ovlBTBLManager(): ovlOVLManager() {
        //m_deferable = true;
    };

    virtual void Init(cOvl* ovl);

    unsigned long AddTexture(const cTexture& item);

    virtual void Make(cOvlInfo* info);

    virtual const char* Loader() const {
        return LOADER;
    };
    virtual const char* Name() const {
        return NAME;
    };
    virtual const char* Tag() const {
        return TAG;
    };
    virtual const unsigned long Type() const {
        return TYPE;
    };
};

#endif
