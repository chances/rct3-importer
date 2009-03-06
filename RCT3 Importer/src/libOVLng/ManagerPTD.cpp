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

#include "OVLDebug.h"

#include "ManagerPTD.h"

#include "ManagerGSI.h"
#include "ManagerTXT.h"
#include "OVLException.h"

using namespace std;
using namespace r3;

const char* ovlPTDManager::NAME = "PathType";
const char* ovlPTDManager::TAG = "ptd";

inline void MakeCopy(string* s) {
    if (s[1] == "")
        s[1] = s[0];
    if (s[2] == "")
        s[2] = s[0];
    if (s[3] == "")
        s[3] = s[0];
}

inline void AddStrings(string* s, ovlStringTable* tab) {
    tab->AddString(s[0]);
    tab->AddString(s[1]);
    tab->AddString(s[2]);
    tab->AddString(s[3]);
}

void cPathExt::DoCopy(bool ext, ovlStringTable* tab) {
    MakeCopy(&b[0]);
    AddStrings(&b[0], tab);
    if (ext) {
        MakeCopy(&fc[0]);
        AddStrings(&fc[0], tab);
        MakeCopy(&bc[0]);
        AddStrings(&bc[0], tab);
        MakeCopy(&tc[0]);
        AddStrings(&tc[0], tab);
    }
}

void cPath::DoCopy(ovlStringTable* tab) {
    bool ext = unk1 & FLAG_EXTENDED;
    if (internalname == "")
        internalname = name;
    tab->AddString(internalname);
    tab->AddString(texture_a);
    tab->AddString(texture_b);
    MakeCopy(&flat[0]);
    AddStrings(&flat[0], tab);
    MakeCopy(&straight_a[0]);
    AddStrings(&straight_a[0], tab);
    MakeCopy(&straight_b[0]);
    AddStrings(&straight_b[0], tab);
    MakeCopy(&turn_u[0]);
    AddStrings(&turn_u[0], tab);
    MakeCopy(&turn_la[0]);
    AddStrings(&turn_la[0], tab);
    MakeCopy(&turn_lb[0]);
    AddStrings(&turn_lb[0], tab);
    MakeCopy(&turn_ta[0]);
    AddStrings(&turn_ta[0], tab);
    MakeCopy(&turn_tb[0]);
    AddStrings(&turn_tb[0], tab);
    MakeCopy(&turn_tc[0]);
    AddStrings(&turn_tc[0], tab);
    MakeCopy(&turn_x[0]);
    AddStrings(&turn_x[0], tab);
    MakeCopy(&corner_a[0]);
    AddStrings(&corner_a[0], tab);
    MakeCopy(&corner_b[0]);
    AddStrings(&corner_b[0], tab);
    MakeCopy(&corner_c[0]);
    AddStrings(&corner_c[0], tab);
    MakeCopy(&corner_d[0]);
    AddStrings(&corner_d[0], tab);
    slope.DoCopy(ext, tab);
    slopestraight.DoCopy(ext, tab);
    slopestraightleft.DoCopy(ext, tab);
    slopestraightright.DoCopy(ext, tab);
    slopemid.DoCopy(ext, tab);
    if (ext) {
        MakeCopy(&flat_fc[0]);
        AddStrings(&flat_fc[0], tab);
        if (paving != "")
            tab->AddString(paving);
    }
}

inline void DoFill(char** path, string* s, ovlStringTable* tab, ovlRelocationManager* rel) {
    for (int i = 0; i < 4; ++i) {
        path[i] = tab->FindString(s[i]);
        rel->AddRelocation(reinterpret_cast<unsigned long*>(&path[i]));
    }
}

void cPath::Fill(PathType2* ptd, ovlStringTable* tab, ovlRelocationManager* rel) {
    if (unk1 & FLAG_EXTENDED) {
        memset(ptd, 0, sizeof(PathType2));
        ptd->extended.unk1 = 0;
        ptd->extended.unk2 = 1;
        DoFill(&ptd->extended.FlatFc[0], &flat_fc[0], tab, rel);
        DoFill(&ptd->extended.SlopeFc[0], &slope.fc[0], tab, rel);
        DoFill(&ptd->extended.SlopeBc[0], &slope.bc[0], tab, rel);
        DoFill(&ptd->extended.SlopeTc[0], &slope.tc[0], tab, rel);
        DoFill(&ptd->extended.SlopeStraightFc[0], &slopestraight.fc[0], tab, rel);
        DoFill(&ptd->extended.SlopeStraightBc[0], &slopestraight.bc[0], tab, rel);
        DoFill(&ptd->extended.SlopeStraightTc[0], &slopestraight.tc[0], tab, rel);
        DoFill(&ptd->extended.SlopeStraightLeftFc[0], &slopestraightleft.fc[0], tab, rel);
        DoFill(&ptd->extended.SlopeStraightLeftBc[0], &slopestraightleft.bc[0], tab, rel);
        DoFill(&ptd->extended.SlopeStraightLeftTc[0], &slopestraightleft.tc[0], tab, rel);
        DoFill(&ptd->extended.SlopeStraightRightFc[0], &slopestraightright.fc[0], tab, rel);
        DoFill(&ptd->extended.SlopeStraightRightBc[0], &slopestraightright.bc[0], tab, rel);
        DoFill(&ptd->extended.SlopeStraightRightTc[0], &slopestraightright.tc[0], tab, rel);
        DoFill(&ptd->extended.SlopeMidFc[0], &slopemid.fc[0], tab, rel);
        DoFill(&ptd->extended.SlopeMidBc[0], &slopemid.bc[0], tab, rel);
        DoFill(&ptd->extended.SlopeMidTc[0], &slopemid.tc[0], tab, rel);
        if (paving != "") {
            ptd->extended.Paving = tab->FindString(paving);
            rel->AddRelocation(reinterpret_cast<unsigned long*>(&ptd->extended.Paving));
        }
    } else {
        memset(ptd, 0, sizeof(PathType));
    }
    ptd->base.unk1 = unk1;
    ptd->base.InternalName = tab->FindString(internalname);
    rel->AddRelocation(reinterpret_cast<unsigned long*>(&ptd->base.InternalName));
    ptd->base.Texture1 = tab->FindString(texture_a);
    rel->AddRelocation(reinterpret_cast<unsigned long*>(&ptd->base.Texture1));
    ptd->base.Texture2 = tab->FindString(texture_b);
    rel->AddRelocation(reinterpret_cast<unsigned long*>(&ptd->base.Texture2));
    DoFill(&ptd->base.Flat[0], &flat[0], tab, rel);
    DoFill(&ptd->base.StraightA[0], &straight_a[0], tab, rel);
    DoFill(&ptd->base.StraightB[0], &straight_b[0], tab, rel);
    DoFill(&ptd->base.TurnU[0], &turn_u[0], tab, rel);
    DoFill(&ptd->base.TurnLA[0], &turn_la[0], tab, rel);
    DoFill(&ptd->base.TurnLB[0], &turn_lb[0], tab, rel);
    DoFill(&ptd->base.TurnTA[0], &turn_ta[0], tab, rel);
    DoFill(&ptd->base.TurnTB[0], &turn_tb[0], tab, rel);
    DoFill(&ptd->base.TurnTC[0], &turn_tc[0], tab, rel);
    DoFill(&ptd->base.TurnX[0], &turn_x[0], tab, rel);
    DoFill(&ptd->base.CornerA[0], &corner_a[0], tab, rel);
    DoFill(&ptd->base.CornerB[0], &corner_b[0], tab, rel);
    DoFill(&ptd->base.CornerC[0], &corner_c[0], tab, rel);
    DoFill(&ptd->base.CornerD[0], &corner_d[0], tab, rel);
    DoFill(&ptd->base.Slope[0], &slope.b[0], tab, rel);
    DoFill(&ptd->base.SlopeStraight[0], &slopestraight.b[0], tab, rel);
    DoFill(&ptd->base.SlopeStraightLeft[0], &slopestraightleft.b[0], tab, rel);
    DoFill(&ptd->base.SlopeStraightRight[0], &slopestraightright.b[0], tab, rel);
    DoFill(&ptd->base.SlopeMid[0], &slopemid.b[0], tab, rel);
}

void ovlPTDManager::AddPath(const cPath& item) {
    Check("ovlPTDManager::AddPath");
    if (item.name == "")
        BOOST_THROW_EXCEPTION(EOvl("ovlPTDManager::AddPath called without name"));
    if (m_items.find(item.name) != m_items.end())
        BOOST_THROW_EXCEPTION(EOvl("ovlPTDManager::AddPath: Item with name '"+item.name+"' already exists"));
    if (item.nametxt == "")
        BOOST_THROW_EXCEPTION(EOvl("ovlPTDManager::AddPath called without name text"));
    if (item.icon == "")
        BOOST_THROW_EXCEPTION(EOvl("ovlPTDManager::AddPath called without icon"));

    m_items[item.name] = item;
    m_items[item.name].DoCopy(GetStringTable());

    // Main
    if (item.unk1 & cPath::FLAG_EXTENDED) {
        m_size += sizeof(PathType2);
    } else {
        m_size += sizeof(PathType);
    }


    GetLSRManager()->AddSymbol(OVLT_UNIQUE);
    GetLSRManager()->AddLoader(OVLT_UNIQUE);
    GetStringTable()->AddSymbolString(item.name.c_str(), ovlPTDManager::TAG);

    GetLSRManager()->AddSymRef(OVLT_UNIQUE);
    GetStringTable()->AddSymbolString(item.nametxt.c_str(), ovlTXTManager::TAG);
    GetLSRManager()->AddSymRef(OVLT_UNIQUE);
    GetStringTable()->AddSymbolString(item.icon.c_str(), ovlGSIManager::TAG);

}

void ovlPTDManager::Make(cOvlInfo* info) {
    DUMP_LOG("Trace: ovlPTDManager::Make()");
    Check("ovlPTDManager::Make");

    ovlOVLManager::Make(info);
    unsigned char* c_data = m_data;

    for (map<string, cPath>::iterator it = m_items.begin(); it != m_items.end(); ++it) {
        // Assign structs
        PathType2* c_item = reinterpret_cast<PathType2*>(c_data);
        if (it->second.unk1 & cPath::FLAG_EXTENDED) {
            c_data += sizeof(PathType2);
        } else {
            c_data += sizeof(PathType);
        }

        it->second.Fill(c_item, GetStringTable(), GetRelocationManager());

        SymbolStruct* c_symbol = GetLSRManager()->MakeSymbol(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->first.c_str(), TAG), reinterpret_cast<unsigned long*>(c_item));
        GetLSRManager()->OpenLoader(OVLT_UNIQUE, TAG, reinterpret_cast<unsigned long*>(c_item), false, c_symbol);

        GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.nametxt.c_str(), ovlTXTManager::TAG),
                             reinterpret_cast<unsigned long*>(&c_item->base.Name));
        GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.icon.c_str(), ovlGSIManager::TAG),
                             reinterpret_cast<unsigned long*>(&c_item->base.gsi));

        GetLSRManager()->CloseLoader(OVLT_UNIQUE);

    }
}
