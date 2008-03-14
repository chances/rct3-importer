///////////////////////////////////////////////////////////////////////////////
//
// New OVL creation library
// Manager class for QTD structures
// Copyright (C) 2008 Tobias Minch
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


#ifndef MANAGERQTD_H_INCLUDED
#define MANAGERQTD_H_INCLUDED

#include <map>
#include <string>
#include <vector>

#include "path.h"
#include "ManagerOVL.h"
#include "ManagerCommon.h"

using namespace std;

class cQueue {
public:
    string name;
    unsigned long unk1; ///< flags
    string internalname;
    string nametxt;
	string icon;        ///< 40x40
    string texture;
    string straight;
    string turn_l;
    string turn_r;
    string slopeup;
    string slopedown;
    string slopestraight[2];

    void Fill(QueueType* qtd, ovlStringTable* tab, ovlRelocationManager* rel);
    void DoCopy(ovlStringTable* tab);
};


class ovlQTDManager: public ovlOVLManager {
public:
    static const char* NAME;
    static const char* TAG;
private:
    map<string, cQueue>  m_items;

public:
    ovlQTDManager(): ovlOVLManager() {
    };

    void AddQueue(const cQueue& item);

    virtual void Make(cOvlInfo* info);

    virtual const char* Name() const {
        return NAME;
    };
    virtual const char* Tag() const {
        return TAG;
    };
};



#endif
