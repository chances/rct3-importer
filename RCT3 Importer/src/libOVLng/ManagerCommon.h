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

#include "rct3constants.h"
#include "sharedride.h"
#include "stall.h"
#include "vertex.h"

#include "ManagerOVL.h"

inline float TriMin(float a, float b, float c) {
    return (a<b)?((a<c)?a:c):((b<c)?b:c);
}

inline float TriMax(float a, float b, float c) {
    return (a>b)?((a>c)?a:c):((b>c)?b:c);
}

struct cTriangleSortAlgorithm {
    enum Algorithm {
        MIN = 0,
        MAX,
        MEAN,
        MINMAX,
        MAXMIN,
        ANGLE45,
        MINXZBYY,
        MINXZBYOTHER,
        MINYBYX,
        MINYBYZ,
        MININV,
        MINMAXINV,
        NONE,
        EnumSize
    };
    enum Axis {
        BY_X,
        BY_Y,
        BY_Z
    };
    static const Algorithm DEFAULT;

    Algorithm algo;
    Axis dir;
    cTriangleSortAlgorithm(Algorithm algorithm, Axis direction): algo(algorithm), dir(direction) {};
    bool operator() (const r3::VECTOR& a1, const r3::VECTOR& a2, const r3::VECTOR& a3, const r3::VECTOR& b1, const r3::VECTOR& b2, const r3::VECTOR& b3) const;

    static Algorithm GetAlgo(const char* algoname);
    static inline Algorithm GetDefault() { return DEFAULT; }
    static const char* GetDefaultName();
    static const char* GetAlgorithmName(Algorithm algo);
};
/*
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
*/

class cAttraction {
public:
    union {
        unsigned long type; // Second byte is 02 for Soaked and 03 for Wild/Vanilla (structure is new since soaked)
                            // This gives the structure version. 03 has one long more
        struct {
            unsigned char type_b;
            unsigned char version;
            unsigned short unused;
        } __attribute__((__packed__));
    };
	std::string name;
	std::string description;
	std::string icon;
	unsigned long unk2;
    long unk3;
	unsigned long unk4;
	unsigned long unk5;
	union {
        unsigned long unk6;
        unsigned long baseUpkeep;
	};
	std::string spline;
	std::vector<std::string> splines;
	union {
        unsigned long unk9;
        unsigned long flags;
	};
	union {
        long unk10;
        long maxHeight;
	};
	unsigned long addonascn;
	unsigned long unk12;
	unsigned long unk13;

	cAttraction() {
	    // default to generic stall
	    type = r3::Constants::Attraction::Type::Stall_Misc | r3::Constants::Addon::Wild_Hi;
	    unk2 = 10000;
	    unk3 = -7500;
	    unk4 = 0;
	    unk5 = 0;
	    unk6 = r3::Constants::Attraction::BaseUpkeep::Stall;
	    unk9 = 0;
	    unk10 = 12;
	    addonascn = r3::Constants::Addon::Vanilla;
	    unk12 = r3::Constants::Attraction::Unknown12::Generic_Stall;
	    unk13 = r3::Constants::Attraction::Unknown13::Default;
	}
    void Fill(r3::Attraction_V* attv) const; // Automatically casts to Attraction_S/W depending on type
    inline void Fill(r3::Attraction_S* att) const { Fill(reinterpret_cast<r3::Attraction_V*>(att)); }
    void Fill(r3::StallA* sta) const;
    void Fill(r3::SpecialAttractionA* sp) const;
};

class cRideOption {
public:
    unsigned long type;
    r3::RideOptionOption option;
    float param[4];

    cRideOption() {
        type = 0;
        option.suboption = 0;
        option.group = 0;
        param[0] = 0.0;
        param[1] = 0.0;
        param[2] = 0.0;
        param[3] = 0.0;
    }
    unsigned long GetSize() const;
    void SetType(unsigned long t);
    void Fill(r3::RideOption* r) const;
};

class cRide {
public:
    unsigned long attractivity;
    unsigned long seating;
    std::vector<cRideOption> options;
    unsigned long entry_fee;
    unsigned long min_circuits;           ///< Seen 1
    long max_circuits;           ///< Seen -1, 1 (Spiral Slide, FunHouse, LionShow, TigerShow, DolphinShow, KillerWhaleShow)
    unsigned long unk11;          ///< Seen 3
    unsigned long unk12;          ///< Seen 3
    long unk13;          ///< Seen -2
    long unk14;          ///< Seen -2
    long unk15;          ///< Seen -2
    unsigned long unk16;          ///< Seen 1
    unsigned long unk17;          ///< Seen 1

    cRide() {
        attractivity = 55;
        seating = 0;
        entry_fee = 100;
        min_circuits = 1;
        max_circuits = -1;
        unk11 = 3;
        unk12 = 3;
        unk13 = -2;
        unk14 = -2;
        unk15 = -2;
        unk16 = 1;
        unk17 = 1;
    }
    unsigned long GetOptionsSize() const;
    void Fill(r3::Ride_V* r) const;
    void Fill(r3::Ride_S* r) const;
    void Fill(r3::Ride_W* r) const;

};

// "Fake" classes for consistent access to file tags
class ovlTXSManager: public ovlOVLManager {
public:
    static const char* TAG;
};


#endif
