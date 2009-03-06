///////////////////////////////////////////////////////////////////////////////
//
// New OVL creation library
// Manager class for TER structures
// Copyright (C) 2009 Tobias Minch
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


#ifndef MANAGERTER_H_INCLUDED
#define MANAGERTER_H_INCLUDED

#include "terraintype.h"

#include <map>
#include <string>
#include <vector>

#include "ManagerOVL.h"
#include "ManagerCommon.h"

class cTerrain {
public:
    typedef std::pair<std::string, cTerrain> mapentry;

    std::string name;
    std::string description_name;
    std::string icon_name;
    std::string texture;
    unsigned long   version;
	unsigned long	addon;
	unsigned long	number;
	unsigned long	type;
	struct Parameters {
		unsigned long colour01;
		unsigned long colour02;
		float inv_width;
		float inv_height;
		
		Parameters(): colour01(0xFFFF007F), colour02(0xFFFF007F), inv_width(0.1), inv_height(0.1) {}

		void Fill(r3::TerrainType& rc, ovlStringTable* tab, ovlRelocationManager* rel) const;
	} parameters;
	struct Unknowns {
		unsigned long     unk02;
		float             unk13;
		float             unk14;
		float             unk15;
		Unknowns(): unk02(0), unk13(0.3), unk14(0.0), unk15(0.5) {}

		void Fill(r3::TerrainType& rc, ovlStringTable* tab, ovlRelocationManager* rel) const;

	} unknowns;
	
	cTerrain(): name(""), description_name(""), icon_name(""), version(1), addon(0), number(0), type(0) {}

    void Fill(r3::TerrainType* rc, ovlStringTable* tab, ovlRelocationManager* rel) const;

    unsigned long GetUniqueSize() const;
};

class ovlTERManager: public ovlOVLManager {
public:
    static const char* NAME;
    static const char* TAG;
private:
    std::map<std::string, cTerrain>  m_items;

public:
    ovlTERManager(): ovlOVLManager() {
    };

    void AddItem(const cTerrain& item);

    virtual void Make(cOvlInfo* info);

    virtual const char* Name() const {
        return NAME;
    };
    virtual const char* Tag() const {
        return TAG;
    };
};

#endif

