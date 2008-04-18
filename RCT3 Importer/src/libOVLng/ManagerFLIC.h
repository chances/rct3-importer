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


#ifndef MANAGERFLIC_H_INCLUDED
#define MANAGERFLIC_H_INCLUDED

#include <boost/shared_array.hpp>
#include <map>
#include <set>
#include <string>

#include "ManagerBTBL.h"
#include "ManagerOVL.h"

#include "icontexture.h"

/*
Here is how simple flics work:

Main Data:
Stores a relocated pointer to the following FlicStruct. This is eased by the Filc struct.
The FlicStruct is a data pointer which is 0 on disk and two unknowns. Unknown 1 is 1 and
Unknown 2 is 1.0.

Extra Data:
Stores a FlicHeader followed by FlicMipHeader/Data Blocks. The mipcount is either 0 or
9 (the latter for 256x256 fully mipped textures). Those have 7 mips followed by a zeroed
FlicMipHeader.


*/

/*
struct FlicInternal {
    FlicHeader header;
    FlicStruct flic;
    unsigned long datasize;
    FlicStruct** madep1;
    FlicStruct* madep2;
};
*/

class ovlFLICManager: public ovlOVLManager {
public:
    static const char* LOADER;
    static const char* NAME;
    static const char* TAG;
    unsigned long TYPE; // Not const, neither static!
private:
    std::map<unsigned long, cTexture> m_items;
    std::map<std::string, unsigned long> m_itemmap;
    ovlBTBLManager* m_btbl;
public:
    ovlFLICManager(): ovlOVLManager(), m_btbl(NULL) {
        m_deferable = true;
    };

    virtual void Init(cOvl* ovl);

    void AddTexture(const cTexture& item);

    FlicStruct** GetPointer1(const std::string& name);
    FlicStruct* GetPointer2(const std::string& name);

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
/*
class ovlFLICManager: public ovlOVLManager {
public:
    static const char* LOADER;
    static const char* NAME;
    static const char* TAG;
    static const unsigned long TYPE;
private:
    map<string, FlicInternal> m_flics;
public:
    ovlFLICManager(): ovlOVLManager() {
        m_deferable = true;
    };
    virtual ~ovlFLICManager();

    void AddTexture(const string& name, unsigned long dimension, unsigned long size, unsigned long* data);
    void SetParameters(const string& name, unsigned long format = 0x13, unsigned long unk1 = 1, float unk2 = 1.0);

    FlicStruct** GetPointer1(const string& name);
    FlicStruct* GetPointer2(const string& name);

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
*/
#endif
