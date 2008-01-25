///////////////////////////////////////////////////////////////////////////////
//
// New OVL creation library
// Manager class for SPL structures
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

#include "ManagerSPL.h"

#include <math.h>

#include "OVLException.h"

const char* ovlSPLManager::LOADER = "FGDK";
const char* ovlSPLManager::NAME = "Spline";
const char* ovlSPLManager::TAG = "spl";

inline void calcExtraVec(float p, const VECTOR& f, const VECTOR& t, VECTOR& e) {
    e.x = f.x + ((t.x - f.x) * p);
    e.y = f.y + ((t.y - f.y) * p);
    e.z = f.z + ((t.z - f.z) * p);
}

inline float calcVecDist(const VECTOR& f, const VECTOR& t) {
    return sqrt(pow(t.x - f.x, 2)+pow(t.y - f.y, 2)+pow(t.z - f.z, 2));
}

inline VECTOR addVec(const VECTOR& f, const VECTOR& t) {
    VECTOR v;
    v.x = f.x + t.x;
    v.y = f.y + t.y;
    v.z = f.z + t.z;
    return v;
}

#define calcSplineLengthAdd(a, b, c, d, l) \
    calcSplineLength(a, addVec(a, b), addVec(d, c), d, l)
/// Calculate spline segment length
/**
 * http://en.wikipedia.org/wiki/B%C3%A9zier_curve
 */
inline float calcSplineLength(const VECTOR& p1, const VECTOR& p2, const VECTOR& p3, const VECTOR& p4, unsigned long segments) {
    float len = 0.0;
    VECTOR p12;
    VECTOR p23;
    VECTOR p34;
    VECTOR p123;
    VECTOR p234;
    VECTOR p1234;
    VECTOR o1234 = p1;
    for (unsigned long  k = 1; k <= segments; ++k) {
        float p = static_cast<float>(k) / static_cast<float>(segments);
        calcExtraVec(p, p1, p2, p12);
        calcExtraVec(p, p2, p3, p23);
        calcExtraVec(p, p3, p4, p34);
        calcExtraVec(p, p12, p23, p123);
        calcExtraVec(p, p23, p34, p234);
        calcExtraVec(p, p123, p234, p1234);
        len += calcVecDist(o1234, p1234);
        o1234 = p1234;
    }
    return len;
}

void cSpline::CalcLengths() {
    if (!lengths.size()) {
        lengths.clear();
        for (int i = 1; i < nodes.size(); ++i) {
            lengths.push_back(calcSplineLengthAdd(nodes[i - 1].pos, nodes[i - 1].cp2, nodes[i].cp1, nodes[i].pos, lengthcalcres));
        }
        if (cyclic)
            lengths.push_back(calcSplineLengthAdd(nodes[nodes.size() - 1].pos, nodes[nodes.size() - 1].cp2, nodes[0].cp1, nodes[0].pos, lengthcalcres));
    }
}

void cSpline::AssignData() {
    unsigned long datas = nodes.size() - (cyclic?0:1);
    if (!unknowndata.size()) {
        cSplineData sd;
        sd.MakeStrange();
        unknowndata.push_back(sd);
    }
    if (unknowndata.size() < datas) {
        for (unsigned long i = unknowndata.size(); i < datas; ++i) {
            unknowndata.push_back(unknowndata[0]);
        }
    }
}

void cSpline::Fill(Spline* spl) {
    spl->nodecount = nodes.size();
    spl->cyclic = cyclic;
    if (calc_length)
        spl->totallength = 0.0;
    else
        spl->totallength = totallength;
    spl->unk3 = unk3;
    spl->unk6 = unk6;
    for (unsigned int i = 0; i < nodes.size(); ++i) {
        spl->nodes[i] = nodes[i];
    }
    for (unsigned int i = 0; i < lengths.size(); ++i) {
        spl->lengths[i] = lengths[i];
        if (calc_length)
            spl->totallength += lengths[i];
    }
    unsigned long c = 0;
    for (vector<cSplineData>::iterator it = unknowndata.begin(); it != unknowndata.end(); ++it) {
        for (int i = 0; i < 14; ++i) {
            spl->datas[c].data[i] = it->data[i];
        }
        c++;
    }
}


void ovlSPLManager::AddSpline(const cSpline& item) {
    Check("ovlSPLManager::AddSpline");
    if (item.name == "")
        throw EOvl("ovlSPLManager::AddSpline called without name");
    if (m_items.find(item.name) != m_items.end())
        throw EOvl("ovlSPLManager::AddSpline: Item with name '"+item.name+"' already exists");
    if (item.nodes.size() == 0)
        throw EOvl("ovlSPLManager::AddSpline called without nodes");


    m_items[item.name] = item;

    m_items[item.name].CalcLengths();
    m_items[item.name].AssignData();

    if (m_items[item.name].lengths.size() != item.nodes.size() - (item.cyclic?0:1))
        throw EOvl("ovlSPLManager::AddSpline called with wrong number of lengths");
    if (m_items[item.name].unknowndata.size() != item.nodes.size() - (item.cyclic?0:1))
        throw EOvl("ovlSPLManager::AddSpline called with wrong number of data items");

    m_size += sizeof(Spline);
    m_size += m_items[item.name].nodes.size() * sizeof(SplineNode);
    m_size += m_items[item.name].lengths.size() * sizeof(float);
    m_size += m_items[item.name].unknowndata.size() * 14;

    GetLSRManager()->AddSymbol(OVLT_COMMON);
    GetLSRManager()->AddLoader(OVLT_COMMON);
    GetStringTable()->AddSymbolString(item.name.c_str(), ovlSPLManager::TAG);

}

void ovlSPLManager::Make(cOvlInfo* info) {
    DUMP_LOG("Trace: ovlSPLManager::Make()");
    Check("ovlSPLManager::Make");

    m_blobs[""] = cOvlMemBlob(OVLT_COMMON, 2, m_size);
    ovlOVLManager::Make(info);
    unsigned char* c_data = m_blobs[""].data;

    for (map<string, cSpline>::iterator it = m_items.begin(); it != m_items.end(); ++it) {
        // Assign structs
        Spline* c_item = reinterpret_cast<Spline*>(c_data);
        c_data += sizeof(Spline);

        c_item->nodes = reinterpret_cast<SplineNode*>(c_data);
        c_data += it->second.nodes.size() * sizeof(SplineNode);
        GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long*>(&c_item->nodes));

        c_item->lengths = reinterpret_cast<float*>(c_data);
        c_data += it->second.lengths.size() * sizeof(float);
        GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long*>(&c_item->lengths));

        c_item->datas = reinterpret_cast<SplineData*>(c_data);
        c_data += it->second.unknowndata.size() * 14;
        GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long*>(&c_item->datas));

        it->second.Fill(c_item);

        SymbolStruct* c_symbol = GetLSRManager()->MakeSymbol(OVLT_COMMON, GetStringTable()->FindSymbolString(it->first.c_str(), TAG), reinterpret_cast<unsigned long*>(c_item));
        GetLSRManager()->OpenLoader(OVLT_COMMON, TAG, reinterpret_cast<unsigned long*>(c_item), false, c_symbol);
        GetLSRManager()->CloseLoader(OVLT_COMMON);

    }

}
