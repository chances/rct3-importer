///////////////////////////////////////////////////////////////////////////////
//
// Structure for wai/WildAnimalItem
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
///////////////////////////////////////////////////////////////////////////////

#ifndef __wildanimalitem_h__
#define __wildanimalitem_h__

#include "guiicon.h"
#include "staticshape.h"

struct WildAnimalItem {
    unsigned long unk1;         // 0
    unsigned long unk2;         // 2
    unsigned long unk3;         // 2
    unsigned long unk4;         // 2
    char* shortname;            // Short name of the item
    StaticShape1* staticshape;
    float distance;             // Probably a lod distance. Always 200.0
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
    GUISkinItem* icon;
    wchar_t* name;
    float unk17;                // Don't know what these are.
    float unk18;                // They are around 1.0, between 0.8 and 1.5
    float unk19;
    float unk20;
    unsigned long flags;        // Flags. Probably:
                                // 1 Plant eaters
                                // 2 Meat eaters
                                // 4 Big animals
    wchar_t* description;
    float weight;               // Guess. Between 0.2 and 1.0.
};

#endif
