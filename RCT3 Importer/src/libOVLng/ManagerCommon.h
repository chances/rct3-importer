///////////////////////////////////////////////////////////////////////////////
//
// New OVL creation library
// Shared tranfer classes for managers and "fake" managers
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


#ifndef MANAGERCOMMON_H_INCLUDED
#define MANAGERCOMMON_H_INCLUDED

#include <string>

#include "stall.h"

#include "ManagerOVL.h"

using namespace std;

enum cAttractionType {
    ATTRACTION_TYPE_Ride_Water          = 0x00,
    ATTRACTION_TYPE_Ride_Gentle         = 0x01,
    ATTRACTION_TYPE_RollerCoaster       = 0x03,
    ATTRACTION_TYPE_Ride_Thrill         = 0x04,
    ATTRACTION_TYPE_Ride_Other          = 0x05,
    ATTRACTION_TYPE_Ride_Junior         = 0x06,
    ATTRACTION_TYPE_Stall_Food          = 0x07,
    ATTRACTION_TYPE_Stall_Drink         = 0x08,
    ATTRACTION_TYPE_Stall_Misc          = 0x09,
    ATTRACTION_TYPE_Special_Toilet      = 0x0B,
    ATTRACTION_TYPE_Special_Cash        = 0x0C,
    ATTRACTION_TYPE_Special_FirstAid    = 0x0D,
    ATTRACTION_TYPE_Stall_Information   = 0x0E,
    ATTRACTION_TYPE_Changing_Room       = 0x12,
    ATTRACTION_TYPE_Ride_Pool           = 0x13,
    ATTRACTION_TYPE_Ride_Safari         = 0x15,

    ATTRACTION_TYPE_Soaked              = 0x200,
    ATTRACTION_TYPE_Wild                = 0x300
};

class cAttraction {
public:
	unsigned long type; // Second byte is 02 for Soaked and 03 for Wild/Vanilla (structure is new since soaked)
                        // This gives the structure version. 03 has one long more
	string name;
	string description;
	string icon;
	unsigned long unk2;
    long unk3;
	unsigned long unk4;
	unsigned long unk5;
	unsigned long unk6;
	string spline;
	unsigned long unk7;
	unsigned long unk8;
	unsigned long unk9;
	long unk10;
	unsigned long unk11;
	unsigned long unk12;
	unsigned long unk13;

	cAttraction() {
	    // default to generic stall
	    type = ATTRACTION_TYPE_Stall_Misc | ATTRACTION_TYPE_Wild;
	    unk2 = 10000;
	    unk3 = -7500;
	    unk4 = 0;
	    unk5 = 0;
	    unk6 = 1500;
	    unk7 = 0;
	    unk8 = 0;
	    unk9 = 0;
	    unk10 = 12;
	    unk11 = 0;
	    unk12 = 2;
	    unk13 = 0;
	}
    void Fill(Attraction* att); // Automatically casts to Attraction2 depending on type
    void Fill(Stall* sta);
};

// "Fake" classes for consistent access to file tags
class ovlTXSManager: public ovlOVLManager {
public:
    static const char* TAG;
};


#endif
