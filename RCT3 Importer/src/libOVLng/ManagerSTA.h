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

using namespace std;

class cStallItem {
public:
    string item;
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
    void Fill(Stall* sta) {
        sta->unk11 = unk1;
        sta->unk12 = unk2;
        sta->unk13 = unk3;
        sta->unk14 = unk4;
        sta->unk15 = unk5;
        sta->unk16 = unk6;
    }
    void Fill(Stall2* sta) {
        sta->unk2 = unk1;
        sta->unk3 = unk2;
        sta->unk4 = unk3;
        sta->unk5 = unk4;
        sta->unk6 = unk5;
        sta->unk7 = unk6;
    }
};

class cStall {
public:
    string name;
    cAttraction attraction;
    cStallUnknowns unknowns;
    vector<cStallItem> items;
    string sid;

    cStall() {};
    void Fill(Stall* sta) {
        sta->Name = NULL;
        sta->Description = NULL;
        sta->GSI = NULL;
        sta->spline = NULL;
        attraction.Fill(sta);
        unknowns.Fill(sta);
    }
    void Fill(Stall2* sta) {
        attraction.Fill(sta->att);
        unknowns.Fill(sta);
    }
};

class ovlSTAManager: public ovlOVLManager {
public:
    static const char* LOADER;
    static const char* NAME;
    static const char* TAG;
private:
    map<string, cStall>  m_stalls;

public:
    ovlSTAManager(): ovlOVLManager() {
    };

    void AddStall(const cStall& stall);

    virtual unsigned char* Make(cOvlInfo* info);

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
