///////////////////////////////////////////////////////////////////////////////
//
// New OVL creation library
// Manager class for PTD structures
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


#ifndef MANAGERPTD_H_INCLUDED
#define MANAGERPTD_H_INCLUDED

#include <map>
#include <string>
#include <vector>

#include "path.h"
#include "ManagerOVL.h"
#include "ManagerCommon.h"

using namespace std;

struct cPathExt {
    string b[4];
    string fc[4];
    string bc[4];
    string tc[4];

    void DoCopy(bool ext, ovlStringTable* tab);
};

class cPath {
public:
    enum {
        FLAG_BASIC = 1,
        FLAG_UNDERWATER = 2,
        FLAG_EXTENDED = 0x01000000
    };
    string name;
    unsigned long unk1; ///< flags
    string internalname;
    string nametxt;
	string icon;        ///< 40x40
    string texture_a;
    string texture_b;
    string flat[4];
    string straight_a[4];
    string straight_b[4];
    string turn_u[4];
    string turn_la[4];
    string turn_lb[4];
    string turn_ta[4];
    string turn_tb[4];
    string turn_tc[4];
    string turn_x[4];
    string corner_a[4];
    string corner_b[4];
    string corner_c[4];
    string corner_d[4];
    cPathExt slope;
    cPathExt slopestraight;
    cPathExt slopestraightleft;
    cPathExt slopestraightright;
    cPathExt slopemid;
    // Extended follow, if not cPathExt'ed
    string flat_fc[4];
    string paving;

    cPath() {
        unk1 = FLAG_BASIC;
    }
    void Fill(PathType2* ptd, ovlStringTable* tab, ovlRelocationManager* rel);
    void DoCopy(ovlStringTable* tab);
};


class ovlPTDManager: public ovlOVLManager {
public:
    static const char* NAME;
    static const char* TAG;
private:
    map<string, cPath>  m_items;

public:
    ovlPTDManager(): ovlOVLManager() {
    };

    void AddPath(const cPath& item);

    virtual void Make(cOvlInfo* info);

    virtual const char* Name() const {
        return NAME;
    };
    virtual const char* Tag() const {
        return TAG;
    };
};



#endif
