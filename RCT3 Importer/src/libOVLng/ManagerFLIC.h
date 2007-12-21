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

#include <string>
#include <map>

#include "ManagerOVL.h"

#include "icontexture.h"

using namespace std;

struct FlicInternal {
    FlicHeader header;
    FlicStruct flic;
    unsigned long datasize;
    FlicStruct** madep1;
    FlicStruct* madep2;
};

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

#endif
