/*
    Originally appeared in
    RCT3 File Dumper
	Copyright 2005 Jonathan Wilson

	This file is part of the RCT3 File Dumper
	The RCT3 File Dumper Program is free software; you can redistribute it and/or modify it under
	the terms of the GNU General Public License as published by the Free
	Software Foundation; either version 2, or (at your option) any later
	version. See the file COPYING for more details.
*/

#ifndef ATTRACTION_H_INCLUDED
#define ATTRACTION_H_INCLUDED

#include "guiicon.h"
#include "spline.h"

namespace r3 {

/// General attraction structure (Vanilla)
/**
 * This structure forms the start of several other structures
 */
struct Attraction_V {
    union {
        uint32_t        type;               ///< second byte 02 siginifies AttractionA
        uint8_t         typech[4];
    };
    wchar_t*            name_ref;
    wchar_t*            description_ref;
    GUISkinItem*        icon_ref;
    uint32_t            unk2;               ///< Probably cost, maybe upkeep? 0 for changing rooms, viewing gallery stub and most tracked rides
    int32_t             unk3;               ///< Probably refund, maybe upkeep? 0 for changing rooms, viewing gallery stub and most tracked rides
    uint32_t            unk4;               ///< Always 0
    uint32_t            unk5;               ///< Always 0
    uint32_t            base_upkeep;        ///< Basal Upkeep
                                /**<
                                 * Seen:
                                 *   - 0 for ZeroG Trampolin, Merry-Go-Round, Fun House and Crooked House
                                 *   - 40 for Spinning Steel Coaster
                                 *   - 125 for Endless and Drifting Coaster
                                 *   - 1500 for Stalls
                                 *   - 4960 for Changing Rooms and all other AnimatedRides and TrackedRides
                                 **/
    Spline*             spline_ref;         ///< A loop on flat rides
    uint32_t            path_count;          ///< Count for the next struct
    Spline**            paths_ref;          ///< splines for peep paths. Last is the mechanic's path
    uint32_t            flags;
    int32_t             max_height;         ///< maximal build height over ground.
};

/// General attraction structure extension (Soaked)
struct Attraction_Sext {
	unsigned long addonascn;    ///< The addon the item is associated with (0 Vanilla, 1 Soaked, 2 Wild)
	unsigned long unk12;        ///< Usually 0, 2 on Generic Stalls, Doughnut Stall and French Fries Stall
};

/// General attraction structure (Soaked)
/**
 * This structure appears as sub-structure in all attractions added or updated in Soaked
 * Used by:
 *   - AnimatedRideB
 *   - ChangingRoom
 *   - SpecialAttractionB
 *   - StallB
 */
struct Attraction_S {
    Attraction_V        v;
    Attraction_Sext     s;
};

/// General attraction structure extension (Wild)
struct Attraction_Wext {
	unsigned long unk13;        ///< Usually 0, 606208/0x94000 for Insect, Reptile and Nocturnal House
};

/// General attraction structure (Wild)
/**
 * This structure appears as sub-structure in all attractions added or updated in Wild
 * Used by:
 *   - AnimatedRideB
 *   - ChangingRoom
 *   - SpecialAttractionB
 *   - StallB
 */
struct Attraction_W {
    Attraction_V        v;
    Attraction_Sext     s;
    Attraction_Wext     w;
};

};

#endif // ATTRACTION_H_INCLUDED
