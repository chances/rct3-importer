///////////////////////////////////////////////////////////////////////////////
//
// New OVL creation library
// Manager class for WAI structures
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


#ifndef MANAGERWAI_H_INCLUDED
#define MANAGERWAI_H_INCLUDED

#include <map>
#include <string>
#include <vector>

#include "wildanimalitem.h"
#include "ManagerOVL.h"
#include "ManagerCommon.h"

class cWildAnimalItem {
public:
    std::string name;
    unsigned long unk1;         // 0
    unsigned long unk2;         // 2
    unsigned long unk3;         // 2
    unsigned long unk4;         // 2
    std::string shortname;           // Short name of the item
    std::string staticshape;
    float distance;             // Probably a lod distance
    long cost;
    long refund;                // In contrast to scenery items, this IS a refund (positive = money back)
    float invdurability;        // Probably reciprocal durability
                                // 0.20 Iced meat and mango
                                // 0.10 Ball and box
                                // 0.05 Dummy and tire
                                // 0.03333 Rock
    unsigned long unk11;        // Unknown enum
                                // 0 Ball and Rock
                                // 1 Box and Dummy
                                // 2 Iced Meat, Mango and Tire
    float x_size;               // These values relate to the size of the item, maybe something about the physics
    float y_size;               // for throwing them around.
    float z_size;
    std::string icon;
    std::string nametxt;
    float unk17;                // Don't know what these are.
    float unk18;                // They are around 1.0, between 0.8 and 1.5
    float unk19;
    float unk20;
    unsigned long flags;        // Flags. Probably:
                                // 1 Plant eaters
                                // 2 Meat eaters
                                // 4 Big animals
    std::string description;
    float weight;               // Guess. Between 0.2 and 1.0.

    cWildAnimalItem() {
        unk1 = 0;
        unk2 = 2;
        unk3 = 2;
        unk4 = 2;
        distance = 200.0;
        cost = 0;
        refund = 0;
        invdurability = 0.05;
        unk11 = 0;
        x_size = 0.0;
        y_size = 0.0;
        z_size = 0.0;
        unk17 = 1.0;
        unk18 = 1.0;
        unk19 = 1.0;
        unk20 = 1.0;
        flags = 0;
        weight = 0.7;
    }
    void Fill(r3::WildAnimalItem* wai);
};


class ovlWAIManager: public ovlOVLManager {
public:
    static const char* LOADER;
    static const char* NAME;
    static const char* TAG;
private:
    std::map<std::string, cWildAnimalItem>  m_items;

public:
    ovlWAIManager(): ovlOVLManager() {
    };

    void AddItem(const cWildAnimalItem& item);

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
