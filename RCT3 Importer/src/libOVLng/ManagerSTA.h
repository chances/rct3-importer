///////////////////////////////////////////////////////////////////////////////
//
// New OVL creation library
// Manager class for STA structures
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


#ifndef MANAGERSTA_H_INCLUDED
#define MANAGERSTA_H_INCLUDED

#include <map>
#include <string>
#include <vector>

#include "stall.h"
#include "ManagerOVL.h"
#include "ManagerCommon.h"

class cStallItem {
public:
    std::string item;
    unsigned long cost;
    cStallItem() {
        cost = 0;
    }
};

class cStallUnknowns {
public:
    unsigned long unk1;
    unsigned long unk2;
    unsigned long unk3;
    unsigned long unk4;
    unsigned long unk5;
    unsigned long unk6;

    cStallUnknowns() {
        unk1 = 6;
        unk2 = 7;
        unk3 = 5;
        unk4 = 7;
        unk5 = 8;
        unk6 = 9;
    }
    void Fill(r3::StallA* sta);
    void Fill(r3::StallB* sta);
};

class cStall {
public:
    std::string name;
    cAttraction attraction;
    cStallUnknowns unknowns;
    std::vector<cStallItem> items;
    std::string sid;

    cStall() {};
    void Fill(r3::StallA* sta);
    void Fill(r3::StallB* sta);
};

class ovlSTAManager: public ovlOVLManager {
public:
    static const char* LOADER;
    static const char* NAME;
    static const char* TAG;
private:
    std::map<std::string, cStall>  m_stalls;

public:
    ovlSTAManager(): ovlOVLManager() {
    };

    void AddStall(const cStall& stall);

    virtual void Make(cOvlInfo* info);

    virtual const char* Name() const {
        return NAME;
    };
    virtual const char* Tag() const {
        return TAG;
    };
    virtual const char* Loader() const {
        return LOADER;
    };
};



#endif
