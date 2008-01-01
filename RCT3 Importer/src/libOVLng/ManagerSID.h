///////////////////////////////////////////////////////////////////////////////
//
// New OVL creation library
// Manager class for SID structures
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


#ifndef MANAGERSID_H_INCLUDED
#define MANAGERSID_H_INCLUDED

#include <map>
#include <string>
#include <vector>

#include "sceneryrevised.h"
#include "ManagerOVL.h"

using namespace std;

class cSidSquareUnknowns {
public:
    bool flag[32];
    unsigned long unk6; // SIDData unk2
    unsigned long unk7; // SIDData unk3
    bool use_unk8;
    unsigned long unk8; // SIDData unk4
    unsigned long unk9; // SIDData unk5

    cSidSquareUnknowns() {
        for (int i = 0; i < 32; ++i)
            flag[i] = false;
        unk6 = 0;
        unk7 = 0;
        use_unk8 = false;
        unk8 = 0;
        unk9 = 0;
    }
    void Fill(SceneryItemData* i) {
        i->flags2 = 0;
        for (unsigned long x = 0; x<32; ++x) {
            if (flag[x])
                i->flags2 += (1 << x);
        }
        i->unk2 = unk6;
        i->unk3 = unk7;
        if (use_unk8)
            *(i->unk4) = unk8;
        else
            i->unk4 = NULL;
        i->unk5 = unk9;
    }
};

class cSidImporterUnknowns {
public:
    bool flag[32];
    unsigned long unk1; // unk4
    unsigned long unk2; // unk17

    cSidImporterUnknowns() {
        for (int i = 0; i < 32; ++i)
            flag[i] = false;
        unk1 = 0;
        unk2 = 0;
    }
    void Fill(SceneryItem* i) {
        i->flags1 = 0;
        for (unsigned long x = 0; x<32; ++x) {
            if (flag[x])
                i->flags1 += (1 << x);
        }
        i->unk4 = unk1;
        i->unk17 = unk2;
    }
};

class cSidStallUnknowns {
public:
    unsigned long unk1;
    unsigned long unk2;

    cSidStallUnknowns() {
        unk1 = 0;
        unk2 = 0;
    }
    void MakeStall() {
        unk1 = 1440;
        unk2 = 2880;
    }
    void Fill(SceneryItem* i) {
        i->stallunknown1 = unk1;
        i->stallunknown2 = unk2;
    }
};

class cSidUnknowns {
public:
	unsigned long unk27; //is 0
	unsigned long unk28; //is 0
	unsigned long unk34; //is 0
	unsigned long unk35; //is 0
	unsigned long unk36; //is 0
	unsigned long unk37; //is 0
	unsigned long unk38; //is 0
	unsigned long unk39; //is 0
	unsigned long unk40; //is 0
	unsigned long unk41; //is 0
	unsigned long unk44; //is 0
	unsigned long unk45; //is 0 except for trampoline
	unsigned long unk46; //is 0 except for trampoline
	unsigned long unk47; //is 0 except for trampoline
	unsigned long unk48; //is 0 except for trampoline

    cSidUnknowns() {
        unk27 = 0;
        unk28 = 0;
        unk34 = 0;
        unk35 = 0;
        unk36 = 0;
        unk37 = 0;
        unk38 = 0;
        unk39 = 0;
        unk40 = 0;
        unk41 = 0;
        unk44 = 0;
        unk45 = 0;
        unk46 = 0;
        unk47 = 0;
        unk48 = 0;
    }
    void Fill(SceneryItem* i) {
        unk27 = unk27;
        unk28 = unk28;
        unk34 = unk34;
        unk35 = unk35;
        unk36 = unk36;
        unk37 = unk37;
        unk38 = unk38;
        unk39 = unk39;
        unk40 = unk40;
        unk41 = unk41;
        unk44 = unk44;
        unk45 = unk45;
        unk46 = unk46;
        unk47 = unk47;
        unk48 = unk48;
    }
};

class cSidDefaultColours {
public:
    unsigned long defaultcol[3];

    cSidDefaultColours() {
        defaultcol[0] = 0;
        defaultcol[1] = 0;
        defaultcol[2] = 0;
    }
    void Fill(SceneryItem* i) {
        i->defaultcol1 = defaultcol[0];
        i->defaultcol2 = defaultcol[1];
        i->defaultcol3 = defaultcol[2];
    }
};

class cSidPosition {
public:
	unsigned short positioningtype;
	unsigned long xsquares;
	unsigned long ysquares;
	float xpos;
	float ypos;
	float zpos;
	float xsize;
	float ysize;
	float zsize;
	string supports;

    cSidPosition() {
        positioningtype = SID_POSITION_FULLTILE;
        xsquares = 1;
        ysquares = 1;
        xpos = 0.0;
        ypos = 0.0;
        zpos = 0.0;
        xsize = 4.0;
        ysize = 4.0;
        zsize = 4.0;
    }
    void Fill(SceneryItem* i) {
        i->positioningtype = positioningtype;
        i->xsquares = xsquares;
        i->ysquares = ysquares;
        i->xpos = xpos;
        i->ypos = ypos;
        i->zpos = zpos;
        i->xsize = xsize;
        i->ysize = ysize;
        i->zsize = zsize;
    }
};

class cSidUI {
public:
    string name;
    string icon;
    string group;
    string groupicon;
    unsigned long type;
    long cost;
    long removal_cost;

    cSidUI() {
        type = SID_TYPE_SCENERY_SMALL;
        cost = 0;
        removal_cost = 0;
    }
    void Fill(SceneryItem* i) {
        i->type = type;
        i->cost = cost;
        i->removal_cost = removal_cost;
    }
};

class cSidExtra {
public:
    unsigned short version;
	SceneryExtraSound* SoundsUnk;
	unsigned long unk2;
	unsigned long AddonPack;       // 0 = Vanilla, 1 = Soaked, 2 = Wild
	unsigned long GenericAddon;
	float unkf;                    // So far seen 1.0
	unsigned long unk3;            // So far seen 0xFFFFFFFF

    cSidExtra() {
        version = 0;
        SoundsUnk = 0;
        unk2 = 0;
        AddonPack = 0;
        GenericAddon = 0;
        unkf = 1.0;
        unk3 = 0xFFFFFFFF;
    }
    void Fill(SceneryItem* i) {
        i->extraversion = version;
    }
    void FillExtra1(SceneryItemExtra1* e) {
        e->SoundsUnk = SoundsUnk;
        e->unk2 = unk2;
        e->AddonPack = AddonPack;
        e->GenericAddon = GenericAddon;
    }
    void FillExtra2(SceneryItemExtra2* e) {
        FillExtra1(reinterpret_cast<SceneryItemExtra1*>(e));
        e->unkf = unkf;
        e->unk3 = unk3;
    }
};

class cSidParam {
public:
    string name;
    string param;

    cSidParam(){};
    cSidParam(string n, string p) {
        name = n;
        param = p;
    }
};

class cSid {
public:
    string name;
    string ovlpath;
    cSidUI ui;
    cSidPosition position;
    cSidDefaultColours colours;
    cSidImporterUnknowns importerunknowns;
    vector<cSidSquareUnknowns> squareunknowns;
    cSidStallUnknowns stallunknowns;
    cSidUnknowns unknowns;
    cSidExtra extra;
    vector<string> svds;
    vector<cSidParam> parameters;

    cSid(){};
    void Fill(SceneryItem* i) {
        ui.Fill(i);
        position.Fill(i);
        colours.Fill(i);
        importerunknowns.Fill(i);
        stallunknowns.Fill(i);
        unknowns.Fill(i);
        extra.Fill(i);

        for (unsigned long x = 0; x < position.xsquares * position.ysquares; ++x) {
            if (squareunknowns.size() < position.xsquares * position.ysquares) {
                if (squareunknowns.size() == 0) {
                    cSidSquareUnknowns uk;
                    uk.Fill(&i->data[x]);
                } else {
                    squareunknowns[0].Fill(&i->data[x]);
                }
            } else {
                squareunknowns[x].Fill(&i->data[x]);
            }
        }

        // Unsupported stuff
        i->unk13count = 0;
        i->unk14 = NULL;
        i->SoundsCount = 0;
        i->Sounds = NULL;
        i->carcount = 0;
        i->cars = NULL;
    }
};

class ovlSIDManager: public ovlOVLManager {
public:
    static const char* NAME;
    static const char* TAG;
private:
    map<string, cSid>  m_sids;

    unsigned long m_commonsize;
public:
    ovlSIDManager(): ovlOVLManager() {
        m_commonsize = 0;
    };

    void AddSID(const cSid& sid);

    virtual void Make(cOvlInfo* info);

    virtual const char* Name() const {
        return NAME;
    };
    virtual const char* Tag() const {
        return TAG;
    };
};



#endif
