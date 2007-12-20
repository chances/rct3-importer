///////////////////////////////////////////////////////////////////////////////
//
// New OVL creation library
// Shared tranfer classes for managers
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

#include "ManagerCommon.h"

#include "OVLException.h"

const char* ovlTXSManager::TAG = "txs";


void cAttraction::Fill(Attraction* att) {
    if ((type & ATTRACTION_TYPE_Wild) == ATTRACTION_TYPE_Wild) {
        Attraction2* att2 = reinterpret_cast<Attraction2*>(att);
        att2->type = type;
        att2->Name = NULL;
        att2->Description = NULL;
        att2->GSI = NULL;
        att2->unk2 = unk2;
        att2->unk3 = unk3;
        att2->unk4 = unk4;
        att2->unk5 = unk5;
        att2->unk6 = unk6;
        att2->spline = NULL;
        att2->unk7 = unk7;
        att2->unk8 = unk8;
        att2->unk9 = unk9;
        att2->unk10 = unk10;
        att2->unk11 = unk11;
        att2->unk12 = unk12;
        att2->unk13 = unk13;
    } else if ((type & ATTRACTION_TYPE_Soaked) == ATTRACTION_TYPE_Soaked) {
        att->type = type;
        att->Name = NULL;
        att->Description = NULL;
        att->GSI = NULL;
        att->unk2 = unk2;
        att->unk3 = unk3;
        att->unk4 = unk4;
        att->unk5 = unk5;
        att->unk6 = unk6;
        att->spline = NULL;
        att->unk7 = unk7;
        att->unk8 = unk8;
        att->unk9 = unk9;
        att->unk10 = unk10;
        att->unk11 = unk11;
        att->unk12 = unk12;
    } else {
        throw EOvl("cAttractionType::Fill, cannot determine structure type.");
    }
}

void cAttraction::Fill(Stall* sta) {
    sta->type = type & 0xFF; // low byte only
    sta->unk2 = unk2;
    sta->unk3 = unk3;
    sta->unk4 = unk4;
    sta->unk5 = unk5;
    sta->unk6 = unk6;
    sta->unk7 = unk7;
    sta->unk8 = unk8;
    sta->unk9 = unk9;
    sta->unk10 = unk10;
}


