///////////////////////////////////////////////////////////////////////////////
//
// New OVL creation library
// Manager class for CHG structures
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


#ifndef MANAGERCHG_H_INCLUDED
#define MANAGERCHG_H_INCLUDED

#include <map>
#include <string>
#include <vector>

#include "stall.h"
#include "ManagerOVL.h"
#include "ManagerCommon.h"

class cChangingRoom {
public:
    string name;
    cAttraction attraction;
    string sid;
    string spline;

    cChangingRoom() {
        attraction.type = r3::Constants::Attraction::Type::Changing_Room | r3::Constants::Addon::Soaked_Hi;
	    attraction.unk2 = 0;
	    attraction.unk3 = 0;
	    attraction.unk6 = r3::Constants::Attraction::BaseUpkeep::Other;
	    attraction.unk10 = -1;
	    attraction.addonascn = r3::Constants::Addon::Soaked;
	    attraction.unk12 = r3::Constants::Attraction::Unknown12::Default;

    };
    void Fill(r3::ChangingRoom* chg) {
        chg->SID = NULL;
        chg->spline = NULL;
        attraction.Fill(chg->att);
    }
};

class ovlCHGManager: public ovlOVLManager {
public:
    static const char* LOADER;
    static const char* NAME;
    static const char* TAG;
private:
    map<string, cChangingRoom>  m_items;

public:
    ovlCHGManager(): ovlOVLManager() {
    };

    void AddRoom(const cChangingRoom& item);

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
