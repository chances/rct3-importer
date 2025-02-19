///////////////////////////////////////////////////////////////////////////////
//
// New OVL creation library
// Manager class for BAN structures
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


#ifndef MANAGERBAN_H_INCLUDED
#define MANAGERBAN_H_INCLUDED

#include <map>
#include <set>
#include <string>
#include <vector>

#include "boneanim.h"
#include "ManagerOVL.h"

struct cTXYZComp {
    bool operator() (const r3::txyz& lhs, const r3::txyz& rhs) const {
        return lhs.Time<rhs.Time;
    }
};

class cBoneAnimBone {
public:
    std::string name;
    std::set<r3::txyz, cTXYZComp> translations;
    std::set<r3::txyz, cTXYZComp> rotations;

    cBoneAnimBone(){};
    float Fill(r3::BoneAnimBone* bone);
};

class cBoneAnim {
public:
    std::string name;
    float totaltime;
    bool calc_time;
    std::vector<cBoneAnimBone> bones;

    cBoneAnim() {
        totaltime = 0.0;
        calc_time = true;
    }
    void Fill(r3::BoneAnim* ban);
};

class ovlBANManager: public ovlOVLManager {
public:
    static const char* NAME;
    static const char* TAG;
private:
    std::map<std::string, cBoneAnim> m_items;

public:
    ovlBANManager(): ovlOVLManager() {};

    void AddAnimation(const cBoneAnim& item);

    virtual void Make(cOvlInfo* info);

	virtual int GetCount(r3::cOvlType type) const {
		if (type == r3::OVLT_COMMON)
			return m_items.size();
		else
			return 0;
	}

    virtual const char* Name() const {
        return NAME;
    };
    virtual const char* Tag() const {
        return TAG;
    };
};

#endif
