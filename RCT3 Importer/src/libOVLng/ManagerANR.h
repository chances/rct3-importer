///////////////////////////////////////////////////////////////////////////////
//
// New OVL creation library
// Manager class for ANR structures
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


#ifndef MANAGERANR_H_INCLUDED
#define MANAGERANR_H_INCLUDED

#include <map>
#include <string>
#include <vector>

#include "animatedride.h"
#include "ManagerOVL.h"
#include "ManagerCommon.h"

struct cAnimatedRideShowItem {
    unsigned long animation;
    std::string name;
    cAnimatedRideShowItem(): animation(0) {}
};

class cAnimatedRide {
public:
    std::string name;
    cAttraction attraction;
    cRide ride;
    std::string sid;
	unsigned long unk22;        ///< Seen 0
	unsigned long unk23;        ///< Seen 0
	unsigned long unk24;        ///< Seen 0
	unsigned long unk25;        ///< Seen 10
    unsigned long unk8;           ///< Seen 0
    unsigned long unk9;           ///< Seen 0
    std::vector<cAnimatedRideShowItem> showitems;

    cAnimatedRide() {
        attraction.type = r3::Constants::Attraction::Type::Ride_Thrill;
	    attraction.unk6 = r3::Constants::Attraction::BaseUpkeep::Other;
	    attraction.unk10 = 16;
	    attraction.unk12 = r3::Constants::Attraction::Unknown12::Default;
	    // ride defaults are fine
	    unk22 = 0;
	    unk23 = 0;
	    unk24 = 0;
	    unk25 = 10;
	    unk8 = 0;
	    unk9 = 0;
    };
    void Fill(r3::AnimatedRide_V* anr) const;
    void Fill(r3::AnimatedRide_SW* anr) const;
};

class ovlANRManager: public ovlOVLManager {
public:
    static const char* LOADER;
    static const char* NAME;
    static const char* TAG;
private:
    std::map<std::string, cAnimatedRide>  m_items;
    unsigned long m_commonsize;

public:
    ovlANRManager(): ovlOVLManager() {
        m_commonsize = 0;
    };

    void AddRide(const cAnimatedRide& item);

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
