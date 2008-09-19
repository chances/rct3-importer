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

#include "rct3constants.h"
#include "sceneryrevised.h"
#include "ManagerOVL.h"

class cSidSquareUnknowns {
public:
    //bool flag[32];
    unsigned long flags;
    long min_height; // SIDData unk2
    long max_height; // SIDData unk3
    //bool use_unk8;
    //unsigned long unk8; // SIDData unk4
    unsigned long supports; // SIDData unk5

    cSidSquareUnknowns() {
//        for (int i = 0; i < 32; ++i)
//            flag[i] = false;
        flags = 0;
        min_height = 0;
        max_height = 0;
        //use_unk8 = false;
        //unk8 = 0;
        supports = 0;
    }
    void Fill(r3::SceneryItemData* i) const;
    int GetHeightSize() const;
    inline long GetHeight() const {
        return max_height - min_height;
    }
};

class cSidImporterUnknowns {
public:
    //bool flag[32];
    unsigned long flags;
    unsigned long unk1; // unk4
    unsigned long unk2; // unk17

    cSidImporterUnknowns() {
//        for (int i = 0; i < 32; ++i)
//            flag[i] = false;
        flags = 0;
        unk1 = 0;
        unk2 = 0;
    }
    void Fill(r3::SceneryItem_V* i);
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
    void Fill(r3::SceneryItem_V* i) {
        i->stall_unknown1 = unk1;
        i->stall_unknown2 = unk2;
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
    }
    void Fill(r3::SceneryItem_V* i);
};

class cSidFlatride {
public:
    std::vector<std::string> individual_animations;
	uint32_t            chunked_anr_unk1; //is 0 except for trampoline (3)
	uint32_t            chunked_anr_animation_chunks; //is 0 except for trampoline (4), parachute drop (12)
	uint32_t            chunked_anr_unk2; //is 0 except for trampoline (2), parachute drop (1)
	uint32_t            chunked_anr_unk3; //is 0 except for trampoline (4), parachute drop (1)

    cSidFlatride() {
        chunked_anr_unk1 = 0;
        chunked_anr_animation_chunks = 0;
        chunked_anr_unk2 = 0;
        chunked_anr_unk3 = 0;
    }
    void Fill(r3::SceneryItem_V* i);
};

class cSidDefaultColours {
public:
    unsigned long defaultcol[3];

    cSidDefaultColours() {
        defaultcol[0] = 0;
        defaultcol[1] = 0;
        defaultcol[2] = 0;
    }
    void Fill(r3::SceneryItem_V* i);
};

class cSidPosition {
public:
	unsigned short positioningtype;
	unsigned long xsquares;
	unsigned long zsquares;
	float xpos;
	float ypos;
	float zpos;
	float xsize;
	float ysize;
	float zsize;
	std::string supports;

    cSidPosition() {
        positioningtype = r3::Constants::SID::Position::Tile_Full;
        xsquares = 1;
        zsquares = 1;
        xpos = 0.0;
        ypos = 0.0;
        zpos = 0.0;
        xsize = 4.0;
        ysize = 4.0;
        zsize = 4.0;
    }
    void Fill(r3::SceneryItem_V* i);
};

class cSidUI {
public:
    std::string name;
    std::string icon;
    std::string group;
    std::string groupicon;
    unsigned long type;
    long cost;
    long removal_cost;

    cSidUI() {
        type = r3::Constants::SID::Type::Scenery_Small;
        cost = 0;
        removal_cost = 0;
    }
    void Fill(r3::SceneryItem_V* i);
};

class cSidSoundScript {
public:
    float time;
    long command;
    float parameter[2];

    cSidSoundScript():time(0.0), command(0) {
        parameter[0] = 0.0;
        parameter[1] = 0.0;
    }
    cSidSoundScript(float _time):time(_time), command(0) {
        parameter[0] = 0.0;
        parameter[1] = 0.0;
    }
    unsigned long GetCommonSize() const;
};

class cSidSound {
public:
    std::vector<std::string> sounds; // unique
    std::vector< std::vector<cSidSoundScript> > animationscripts; // common
    float extra_unkf[2];
    unsigned long extra_unk;

    cSidSound() {
        extra_unkf[0] = -1.0;
        extra_unkf[1] = -1.0;
        extra_unk = 0;
    }
    void FillExtra(r3::SceneryExtraSound& ses) const;
};

class cSidExtra {
public:
    unsigned short version;
	unsigned long unk2;
	unsigned long AddonPack;       // 0 = Vanilla, 1 = Soaked, 2 = Wild
	unsigned long GenericAddon;
	float unkf;                    // So far seen 1.0
	unsigned long billboardaspect;            // So far seen 0xFFFFFFFF

    cSidExtra() {
        version = 0;
        unk2 = 0;
        AddonPack = 0;
        GenericAddon = 0;
        unkf = -1.0;
        billboardaspect = -1;
    }
    void Fill(r3::SceneryItem_V* i);
    void Fill(r3::SceneryItem_S* e);
    void Fill(r3::SceneryItem_W* e);
};

class cSidParam {
public:
    std::string name;
    std::string param;

    cSidParam(){};
    cSidParam(std::string n, std::string p) {
        name = n;
        param = p;
    }
};

class cSid {
public:
    std::string name;
    std::string ovlpath;
    cSidUI ui;
    cSidPosition position;
    cSidDefaultColours colours;
    cSidImporterUnknowns importerunknowns;
    std::vector<cSidSquareUnknowns> squareunknowns;
    cSidStallUnknowns stallunknowns;
    cSidUnknowns unknowns;
    cSidFlatride flatride;
    std::vector<cSidSound> sounds;
    cSidExtra extra;
    std::vector<std::string> svds;
    std::vector<cSidParam> parameters;

    cSid(){};
    void Fill(r3::SceneryItem_V* i);
};

class ovlSIDManager: public ovlOVLManager {
public:
    static const char* NAME;
    static const char* TAG;
private:
    std::map<std::string, cSid>  m_sids;

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
