///////////////////////////////////////////////////////////////////////////////
//
// New OVL creation library
// Manager class for SAT structures
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


#ifndef MANAGERSAT_H_INCLUDED
#define MANAGERSAT_H_INCLUDED

#include <map>
#include <string>
#include <vector>

#include "stall.h"
#include "ManagerOVL.h"
#include "ManagerCommon.h"

class cSpecialAttraction {
public:
    std::string name;
    cAttraction attraction;
    std::string sid;

    cSpecialAttraction() {
	    attraction.type = r3::Constants::Attraction::Type::Special_Toilet | r3::Constants::Addon::Wild_Hi;
	    attraction.unk6 = r3::Constants::Attraction::BaseUpkeep::Other;
	    attraction.unk12 = r3::Constants::Attraction::Unknown12::Default;
    };
    void Fill(r3::SpecialAttractionA* sp);
    void Fill(r3::SpecialAttractionB* sp);
};

class ovlSATManager: public ovlOVLManager {
public:
    static const char* LOADER;
    static const char* NAME;
    static const char* TAG;
private:
    std::map<std::string, cSpecialAttraction>  m_items;

public:
    ovlSATManager(): ovlOVLManager() {};

    void AddAttraction(const cSpecialAttraction& item);

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
