///////////////////////////////////////////////////////////////////////////////
//
// New OVL creation library
// Manager class for CID structures
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


#ifndef MANAGERCID_H_INCLUDED
#define MANAGERCID_H_INCLUDED

#include <map>
#include <string>
#include <vector>

#include "carrieditems.h"
#include "ManagerOVL.h"
#include "ManagerCommon.h"

class cCarriedItemUnknowns {
public:
    unsigned long unk1;             // Always 1
    unsigned long unk3;             // 0 usually, 2 for MoreCarriedItems
    unsigned long unk26;
    unsigned long unk27;
    unsigned long unk28;
    unsigned long unk33;            // 0 usually, 16 for inflatables and swimsuits, 32 for sunglasses

    cCarriedItemUnknowns() {
        unk1 = 1;
        unk3 = 0;
        unk26 = 0;
        unk27 = 0;
        unk28 = 0;
        unk33 = 0;
    }
    void Fill(r3::CarriedItem* cid);
};

class cCarriedItemSizeUnknowns {
public:
    unsigned long unk17;           // unk17-20 are somehow related to package size and/or desirability
    unsigned long unk18;           // unk19 >= unk18 >= unk17, unk20 can be smaller than unk19
    unsigned long unk19;           // The values of a bigger version of an item are larger
    unsigned long unk20;           // Could be a price

    cCarriedItemSizeUnknowns() {
        unk17 = 50;
        unk18 = 100;
        unk19 = 100;
        unk20 = 100;
    }
    void Fill(r3::CarriedItem* cid);
};

class cCarriedItemSoaked {
public:
    std::string male_morphmesh_body;      // All char*s point to empty strings if not set
    std::string male_morphmesh_legs;
    std::string female_morphmesh_body;
    std::string female_morphmesh_legs;
    unsigned long unk38;            // 0 usually, 1 for SwimSuitV1
    std::string textureoption;            // "Inflatable", "DefaultFlexiColours" (Sunglasses), "SwimSuitV1" or "SwimSuitV2"

    cCarriedItemSoaked() {
        unk38 = 0;
    }
    void Fill(r3::CarriedItem* cid) {
        cid->unk38 = unk38;
    }
};

class cCarriedItemTrash {
public:
    std::string wrapper;           // Used up version or open umbrella, :cid for none
    float trash1;                   // 0.0 usually, 0.01 for trash
    long trash2;                    // -1 usually, 1 for FoodWrapper and Stick, 2 for boxes and bottles

    cCarriedItemTrash() {
        trash1 = 0.0;
        trash2 = -1;
    }
    void Fill(r3::CarriedItem* cid);
};

class cCarriedItemSettings {
public:
    unsigned long flags;            // see above
    long ageclass;                  // -1 Usually, for Soaked complex items: 3 kids, 12 teens and 48 adults
    unsigned long type;             // see above
    float hunger;                   // Hunger mod, positive quences, negative icreases
    float thirst;                   // Thirst mod
    float lightprotectionfactor;    // 0.0 usually, 4.0, 15.0 and 50.0 for the respective sunblockers

    cCarriedItemSettings() {
        flags = r3::Constants::CarriedItem::Flag::Souvenir;
        ageclass = -1;
        type = r3::Constants::CarriedItem::Type::Hand | r3::Constants::CarriedItem::Type::Body;
        hunger = 0.0;
        thirst = 0.0;
        lightprotectionfactor = 0.0;
    }
    void Fill(r3::CarriedItem* cid);
};

class cCarriedItemShape {
public:
    std::string shape1;                  // So far shape2 always matches shape1
    std::string shape2;
    float unk9;                     // 0.0 for hidden items, 40.0 usually for shape items, 200.0 for billboard items. Probably a LOD distance
    unsigned long defaultcolour;    // 0 usually, 7 for Baloon
    unsigned long shaped;           // Usually 1, 0 for billboard objects
    std::string fts;                     // is the FlexiTexture for a billboard object
    float scalex;                   // Scalefactor for billboards
    float scaley;

    cCarriedItemShape() {
        unk9 = 40.0;
        defaultcolour = 0;
        shaped = 1;
        scalex = 1.0;
        scaley = 1.0;
    }
    void MakeBillboard();
    void Fill(r3::CarriedItem* cid);
};

class cCarriedItem {
public:
    std::string name;
    unsigned long addonpack;        // 0 vanilla, 1 soaked, 2 wild
    std::string nametxt;
    std::string pluralnametxt;
    std::string icon;                    // :gsi for none 40x40
    cCarriedItemShape shape;
    cCarriedItemSettings settings;
    cCarriedItemTrash trash;
    cCarriedItemSoaked soaked;
    cCarriedItemSizeUnknowns size;
    cCarriedItemUnknowns unknowns;
    std::vector<std::string> extras;

    cCarriedItem() {
        addonpack = 0;
    }
    void Fill(r3::CarriedItem* cid);
};


class ovlCIDManager: public ovlOVLManager {
public:
    static const char* LOADER;
    static const char* NAME;
    static const char* TAG;
private:
    std::map<std::string, cCarriedItem>  m_items;

public:
    ovlCIDManager(): ovlOVLManager() {
    };

    void AddItem(const cCarriedItem& item);

    virtual void Make(cOvlInfo* info);

	virtual int GetCount(r3::cOvlType type) const {
		if (type == r3::OVLT_COMMON)
			return 0;
		else
			return m_items.size();
	}

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
