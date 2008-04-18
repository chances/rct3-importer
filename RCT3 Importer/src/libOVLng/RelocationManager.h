///////////////////////////////////////////////////////////////////////////////
//
// New OVL creation library
// Manager class for relocations
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


#ifndef RELOCATIONMANAGER_H_INCLUDED
#define RELOCATIONMANAGER_H_INCLUDED

#include <queue>

#include "OVLClasses.h"

class ovlRelocationManager {
private:
    std::queue<unsigned long *> m_relocations;
    cOvlInfo* m_info;

    unsigned long DoRelocationForSaving(unsigned long *reloc, bool& common);
public:
    ovlRelocationManager() {};
    virtual ~ovlRelocationManager() {};

    void Init(cOvlInfo* info);

    void AddRelocation(unsigned long *reloc);
    void Make();
    //void WriteFixups(FILE* f);
};

#endif
