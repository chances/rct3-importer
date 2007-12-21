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

using namespace std;

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
    void Fill(CarriedItem* cid) {
        cid->unk1 = unk1;
        cid->unk3 = unk3;
        cid->unk26 = unk26;
        cid->unk27 = unk27;
        cid->unk28 = unk28;
        cid->unk33 = unk33;
    }
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
    void Fill(CarriedItem* cid) {
        cid->unk17 = unk17;
        cid->unk18 = unk18;
        cid->unk19 = unk19;
        cid->unk20 = unk20;
    }
};

class cCarriedItemSoaked {
public:
    string male_morphmesh_body;      // All char*s point to empty strings if not set
    string male_morphmesh_legs;
    string female_morphmesh_body;
    string female_morphmesh_legs;
    unsigned long unk38;            // 0 usually, 1 for SwimSuitV1
    string textureoption;            // "Inflatable", "DefaultFlexiColours" (Sunglasses), "SwimSuitV1" or "SwimSuitV2"

    cCarriedItemSoaked() {
        unk38 = 0;
    }
    void Fill(CarriedItem* cid) {
        cid->unk38 = unk38;
    }
};

class cCarriedItemTrash {
public:
    string wrapper;           // Used up version or open umbrella, :cid for none
    float trash1;                   // 0.0 usually, 0.01 for trash
    long trash2;                    // -1 usually, 1 for FoodWrapper and Stick, 2 for boxes and bottles

    cCarriedItemTrash() {
        trash1 = 0.0;
        trash2 = -1;
    }
    void Fill(CarriedItem* cid) {
        cid->wrapper = NULL;
        cid->trash1 = trash1;
        cid->trash2 = trash2;
    }
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
        flags = CARRIEDITEM_SOUVENIR;
        ageclass = -1;
        type = CARRIEDITEM_TYPE_HAND | CARRIEDITEM_TYPE_BODY;
        hunger = 0.0;
        thirst = 0.0;
        lightprotectionfactor = 0.0;
    }
    void Fill(CarriedItem* cid) {
        cid->flags = flags;
        cid->ageclass = ageclass;
        cid->type = type;
        cid->hunger = hunger;
        cid->thirst = thirst;
        cid->lightprotectionfactor = lightprotectionfactor;
    }
};

class cCarriedItemShape {
public:
    string shape1;                  // So far shape2 always matches shape1
    string shape2;
    float unk9;                     // 0.0 for hidden items, 40.0 usually for shape items, 200.0 for billboard items. Probably a LOD distance
    unsigned long defaultcolour;    // 0 usually, 7 for Baloon
    unsigned long shaped;           // Usually 1, 0 for billboard objects
    string fts;                     // is the FlexiTexture for a billboard object
    float scalex;                   // Scalefactor for billboards
    float scaley;

    cCarriedItemShape() {
        unk9 = 40.0;
        defaultcolour = 0;
        shaped = 1;
        scalex = 1.0;
        scaley = 1.0;
    }
    void MakeBillboard() {
        unk9 = 200.0;
        defaultcolour = 7;
        shaped = 0;
        scalex = 0.75;
        scaley = 0.75;
    }
    void Fill(CarriedItem* cid) {
        cid->unk9 = unk9;
        cid->defaultcolour = defaultcolour;
        cid->shaped = shaped;
        cid->scalex = scalex;
        cid->scaley = scaley;
        cid->shape1 = NULL;
        cid->shape2 = NULL;
        cid->fts = NULL;
    }
};

class cCarriedItem {
public:
    string name;
    unsigned long addonpack;        // 0 vanilla, 1 soaked, 2 wild
    string nametxt;
    string pluralnametxt;
    string icon;                    // :gsi for none 40x40
    cCarriedItemShape shape;
    cCarriedItemSettings settings;
    cCarriedItemTrash trash;
    cCarriedItemSoaked soaked;
    cCarriedItemSizeUnknowns size;
    cCarriedItemUnknowns unknowns;
    vector<string> extras;

    cCarriedItem() {
        addonpack = 0;
    }
    void Fill(CarriedItem* cid) {
        cid->addonpack = addonpack;
        cid->name = NULL;
        cid->pluralname = NULL;
        shape.Fill(cid);
        settings.Fill(cid);
        trash.Fill(cid);
        soaked.Fill(cid);
        size.Fill(cid);
        unknowns.Fill(cid);
    }
};


class ovlCIDManager: public ovlOVLManager {
public:
    static const char* LOADER;
    static const char* NAME;
    static const char* TAG;
private:
    map<string, cCarriedItem>  m_items;

public:
    ovlCIDManager(): ovlOVLManager() {
    };

    void AddItem(const cCarriedItem& item);

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
