///////////////////////////////////////////////////////////////////////////////
//
// OVL dumping library
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
// Based on the ovl dumper by Jonathan Wilson
//
///////////////////////////////////////////////////////////////////////////////


#include "ovldumperstructs.h"

#include "OVLDException.h"

void OvlSymbol::SetData(unsigned long data) {
    if (IsPointer)
        throw EOvlD("Tried to set data for a pointer symbol.");

    orgsymbol->data = reinterpret_cast<unsigned long *>(data);
}

void OvlStringTableEntry::SetString(const char* newstr) const {
    unsigned long len = strlen(newstr);
    if (len > orglength)
        throw EOvlD("Tried to set string: too long.");

    if (len != orglength) {
        for (unsigned long l = 0; l < orglength; ++l)
            orgstring[l] = 0;
    }
    strcpy(orgstring, newstr);
}
