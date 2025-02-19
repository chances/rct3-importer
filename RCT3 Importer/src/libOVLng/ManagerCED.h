///////////////////////////////////////////////////////////////////////////////
//
// New OVL creation library
// Manager class for CED structures
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


#ifndef MANAGERCED_H_INCLUDED
#define MANAGERCED_H_INCLUDED

#include <map>
#include <string>
#include <vector>

#include "carrieditems.h"
#include "ManagerOVL.h"
#include "ManagerCommon.h"

class cCarriedItemExtra {
public:
    std::string name;
    unsigned long unk1; // 0
    std::string nametxt;
	std::string icon;        // 20x20
	unsigned long unk4; // 0 usually, 4 for Mustard
	float hunger;       // >= 0.0, <= 0.3   Probably influences hunger
	float thirst;       // >= -0.3, <=0.3   Probably influences thirst, negative increases, positive quenches
	float unk7;         // 0.0 usually, 0.3 for Parmesan, Chilli, ChilliSauce and Marshmellow

    cCarriedItemExtra() {
        unk1 = 0;
        unk4 = 0;
        unk7 = 0.0;
        hunger = 0.0;
        thirst = 0.0;
    }
    void Fill(r3::CarriedItemExtra* ced);
};


class ovlCEDManager: public ovlOVLManager {
public:
    static const char* LOADER;
    static const char* NAME;
    static const char* TAG;
private:
    std::map<std::string, cCarriedItemExtra>  m_extras;

public:
    ovlCEDManager(): ovlOVLManager() {
    };

    void AddExtra(const cCarriedItemExtra& item);

    virtual void Make(cOvlInfo* info);

	virtual int GetCount(r3::cOvlType type) const {
		if (type == r3::OVLT_COMMON)
			return 0;
		else
			return m_extras.size();
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
