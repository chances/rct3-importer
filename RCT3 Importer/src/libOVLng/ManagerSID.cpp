///////////////////////////////////////////////////////////////////////////////
//
// New OVL creation library
// Manager class for SVD structures
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

#include "ManagerSID.h"

#include "ManagerGSI.h"
#include "ManagerSVD.h"
#include "ManagerTXT.h"
#include "OVLException.h"

using namespace r3;

const char* ovlSIDManager::NAME = "SceneryItem";
const char* ovlSIDManager::TAG = "sid";

void ovlSIDManager::AddSID(const cSid& sid) {
    Check("ovlSIDManager::AddSID");
    if (sid.svds.size() == 0)
        throw EOvl("ovlSIDManager::AddSID called without svds");
    if (sid.name == "")
        throw EOvl("ovlSIDManager::AddSID called without name");
    if (m_sids.find(sid.name) != m_sids.end())
        throw EOvl("ovlSIDManager::AddSID: Item with name '"+sid.name+"' already exists");
    if (sid.ovlpath == "")
        throw EOvl("ovlSIDManager::AddSID called without ovlpath");
    if (sid.ui.name == "")
        throw EOvl("ovlSIDManager::AddSID called without menu name");
    if (sid.ui.icon == "")
        throw EOvl("ovlSIDManager::AddSID called without menu icon");
    if ((sid.squareunknowns.size()>1) && (sid.squareunknowns.size() != (sid.position.xsquares * sid.position.ysquares)))
        throw EOvl("ovlSIDManager::AddSID called with illegal numer of square unknowns");

    m_sids[sid.name] = sid;

    // Sizes
    // Main struct
    m_size += sizeof(SceneryItem);
    if (sid.extra.version == 1)
        m_size += sizeof(SceneryItemExtra1);
    else if (sid.extra.version == 2)
        m_size += sizeof(SceneryItemExtra2);
    // SVD pointers
    m_size += sid.svds.size() * 4;
    // Data
    m_commonsize += sid.position.xsquares * sid.position.ysquares * sizeof(SceneryItemData);
    // Unknown 8
    if (sid.squareunknowns.size()) {
        if (sid.squareunknowns.size()>1) {
            for (unsigned long x = 0; x < sid.position.xsquares * sid.position.ysquares; ++x) {
                if (sid.squareunknowns[x].use_unk8) {
                    m_commonsize += sizeof(unsigned long);
                }
            }
        } else {
            if (sid.squareunknowns[0].use_unk8) {
                m_commonsize += sid.position.xsquares * sid.position.ysquares * sizeof(unsigned long);
            }
        }
    }
    // Params
    m_commonsize += sid.parameters.size() * sizeof(SceneryParams);

    GetLSRManager()->AddSymbol(OVLT_UNIQUE);
    GetLSRManager()->AddLoader(OVLT_UNIQUE);
    GetStringTable()->AddSymbolString(sid.name.c_str(), ovlSIDManager::TAG);
    GetStringTable()->AddString(sid.ovlpath.c_str());

    GetLSRManager()->AddSymRef(OVLT_UNIQUE);
    GetStringTable()->AddSymbolString(sid.ui.name.c_str(), ovlTXTManager::TAG);
    GetLSRManager()->AddSymRef(OVLT_UNIQUE);
    GetStringTable()->AddSymbolString(sid.ui.icon.c_str(), ovlGSIManager::TAG);
    GetStringTable()->AddString(sid.position.supports.c_str()); // Assign even if empty
    if ((sid.ui.group != "") && (sid.ui.groupicon != "")) {
        GetLSRManager()->AddSymRef(OVLT_UNIQUE);
        GetStringTable()->AddSymbolString(sid.ui.group.c_str(), ovlTXTManager::TAG);
        GetLSRManager()->AddSymRef(OVLT_UNIQUE);
        GetStringTable()->AddSymbolString(sid.ui.groupicon.c_str(), ovlGSIManager::TAG);
    }
    for (vector<string>::const_iterator it = sid.svds.begin(); it != sid.svds.end(); ++it) {
        GetLSRManager()->AddSymRef(OVLT_UNIQUE);
        GetStringTable()->AddSymbolString(it->c_str(), ovlSVDManager::TAG);
    }
    for (vector<cSidParam>::const_iterator it = sid.parameters.begin(); it != sid.parameters.end(); ++it) {
        GetStringTable()->AddString(it->name.c_str());
        GetStringTable()->AddString(it->param.c_str());
    }

}

void ovlSIDManager::Make(cOvlInfo* info) {
    DUMP_LOG("Trace: ovlSIDManager::Make()");
    Check("ovlSIDManager::Make");

    m_blobs["0"] = cOvlMemBlob(OVLT_UNIQUE, 2, m_size);
    m_blobs["1"] = cOvlMemBlob(OVLT_COMMON, 2, m_commonsize);
    ovlOVLManager::Make(info);
    unsigned char* c_data = m_blobs["0"].data;
    unsigned char* c_commondata = m_blobs["1"].data;

    for (map<string, cSid>::iterator it = m_sids.begin(); it != m_sids.end(); ++it) {
        // Assign structs
        SceneryItem* c_sid = reinterpret_cast<SceneryItem*>(c_data);
        c_data += sizeof(SceneryItem);

        // Assign extra if necessary
        if (it->second.extra.version == 1) {
            SceneryItemExtra1* c_ex = reinterpret_cast<SceneryItemExtra1*>(c_data);
            c_data += sizeof(SceneryItemExtra1);
            it->second.extra.FillExtra1(c_ex);
        } else if (it->second.extra.version == 2) {
            SceneryItemExtra2* c_ex = reinterpret_cast<SceneryItemExtra2*>(c_data);
            c_data += sizeof(SceneryItemExtra2);
            it->second.extra.FillExtra2(c_ex);
        }

        // Assign svd pointers
        c_sid->svd = reinterpret_cast<SceneryItemVisual**>(c_data);
        c_data += it->second.svds.size() * 4;
        memset(c_sid->svd, 0, it->second.svds.size() * 4); // Set 0, symbolref targets
        GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long*>(&c_sid->svd));

        // Assign common
        c_sid->data = reinterpret_cast<SceneryItemData*>(c_commondata);
        c_commondata += it->second.position.xsquares * it->second.position.ysquares * sizeof(SceneryItemData);
        GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long*>(&c_sid->data));
        if (it->second.squareunknowns.size()) {
            for (unsigned long x = 0; x < it->second.position.xsquares * it->second.position.ysquares; ++x) {
                if (it->second.squareunknowns.size() >= (it->second.position.xsquares * it->second.position.ysquares)) {
                    if (it->second.squareunknowns[x].use_unk8) {
                        c_sid->data[x].unk4 = reinterpret_cast<unsigned long*>(c_commondata);
                        c_commondata += sizeof(unsigned long);
                        GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long*>(&c_sid->data[x].unk4));
                    }
                } else {
                    if (it->second.squareunknowns[0].use_unk8) {
                        c_sid->data[x].unk4 = reinterpret_cast<unsigned long*>(c_commondata);
                        c_commondata += sizeof(unsigned long);
                        GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long*>(&c_sid->data[x].unk4));
                    }
                }
            }
        }
        if (it->second.parameters.size()) {
            c_sid->params = reinterpret_cast<SceneryParams*>(c_commondata);
            c_commondata += it->second.parameters.size() * sizeof(SceneryParams);
            GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long*>(&c_sid->params));
            c_sid->count = it->second.parameters.size();
        } else {
            c_sid->params = NULL;
            c_sid->count = 0;
        }

        it->second.Fill(c_sid);
        // 'Simple' strings
        c_sid->supports = GetStringTable()->FindString(it->second.position.supports.c_str());
        GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long*>(&c_sid->supports));
        c_sid->OvlName = GetStringTable()->FindString(it->second.ovlpath.c_str());
        GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long*>(&c_sid->OvlName));
        for (unsigned long c = 0; c < it->second.parameters.size(); ++c) {
            c_sid->params[c].type = GetStringTable()->FindString(it->second.parameters[c].name.c_str());
            GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long*>(&c_sid->params[c].type));
            c_sid->params[c].params = GetStringTable()->FindString(it->second.parameters[c].param.c_str());
            GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long*>(&c_sid->params[c].params));
        }

        // Symbol and Loader
        SymbolStruct* c_symbol = GetLSRManager()->MakeSymbol(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->first.c_str(), TAG), reinterpret_cast<unsigned long*>(c_sid));
        GetLSRManager()->OpenLoader(OVLT_UNIQUE, TAG, reinterpret_cast<unsigned long*>(c_sid), false, c_symbol);

        GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.ui.name.c_str(), ovlTXTManager::TAG),
                             reinterpret_cast<unsigned long*>(&c_sid->Name));
        GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.ui.icon.c_str(), ovlGSIManager::TAG),
                             reinterpret_cast<unsigned long*>(&c_sid->icon));
        if ((it->second.ui.group != "") && (it->second.ui.groupicon != "")) {
            GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.ui.group.c_str(), ovlTXTManager::TAG),
                                 reinterpret_cast<unsigned long*>(&c_sid->groupname));
            GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.ui.groupicon.c_str(), ovlGSIManager::TAG),
                                 reinterpret_cast<unsigned long*>(&c_sid->groupicon));
        }
        c_sid->svdcount = it->second.svds.size();
        for (unsigned long c = 0; c < it->second.svds.size(); ++c) {
            GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.svds[c].c_str(), ovlSVDManager::TAG),
                                 reinterpret_cast<unsigned long*>(&c_sid->svd[c]));
        }


        GetLSRManager()->CloseLoader(OVLT_UNIQUE);
    }

}
