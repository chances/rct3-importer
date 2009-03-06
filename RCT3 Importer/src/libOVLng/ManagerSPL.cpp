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

using namespace r3;
using namespace std;

#ifdef __BORLANDC__
inline float roundf (float f) {
	float t = floor(f);
	return ((t-f)>=0.5)?t+1:t;
}
#endif


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
inline float calcSplineLength(const VECTOR& p1, const VECTOR& p2, const VECTOR& p3, const VECTOR& p4, cSplineData& cdata) {
    float len = 0.0;
    VECTOR p12;
    VECTOR p23;
    VECTOR p34;
    VECTOR p123;
    VECTOR p234;
    VECTOR p1234;
    VECTOR o1234 = p1;
    vector<float> datadist;
    for (unsigned long  k = 1; k <= 15; ++k) {
        float p = static_cast<float>(k) / static_cast<float>(15);
        calcExtraVec(p, p1, p2, p12);
        calcExtraVec(p, p2, p3, p23);
        calcExtraVec(p, p3, p4, p34);
        calcExtraVec(p, p12, p23, p123);
        calcExtraVec(p, p23, p34, p234);
        calcExtraVec(p, p123, p234, p1234);
        len += calcVecDist(o1234, p1234);
        datadist.push_back(len);
        o1234 = p1234;
    }
    for (unsigned long k = 1; k < 15; ++k) {
        float c = (static_cast<float>(k) * 16.0) + static_cast<float>(k);
        cdata.data[k-1] = roundf((2*c) - (255.0 * datadist.at(k-1) / len));
    }
    return len;
}

void cSpline::CalcLengths() {
    if (!lengths.size()) {
        lengths.clear();
        datas.clear();
        max_y = 0;
        for (int i = 1; i < nodes.size(); ++i) {
            cSplineData cdata;
            lengths.push_back(calcSplineLengthAdd(nodes[i - 1].pos, nodes[i - 1].cp2, nodes[i].cp1, nodes[i].pos, cdata));
            datas.push_back(cdata);
            if (nodes[i - 1].pos.y > max_y)
                max_y = nodes[i - 1].pos.y;
        }
        if (cyclic) {
            cSplineData cdata;
            lengths.push_back(calcSplineLengthAdd(nodes[nodes.size() - 1].pos, nodes[nodes.size() - 1].cp2, nodes[0].cp1, nodes[0].pos, cdata));
            datas.push_back(cdata);
        }
        if (nodes[nodes.size() - 1].pos.y > max_y)
            max_y = nodes[nodes.size() - 1].pos.y;
    }
}

void cSpline::AssignData() {
    unsigned long datacount = nodes.size() - (cyclic?0:1);
    if (!datas.size()) {
        cSplineData sd;
        sd.MakeStrange();
        datas.push_back(sd);
    }
    if (datas.size() < datacount) {
        for (unsigned long i = datas.size(); i < datacount; ++i) {
            datas.push_back(datas[0]);
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
    spl->inv_totallength = inv_totallength;
    spl->max_y = max_y;
    for (unsigned int i = 0; i < nodes.size(); ++i) {
        spl->nodes[i] = nodes[i];
    }
    for (unsigned int i = 0; i < lengths.size(); ++i) {
        spl->lengths[i] = lengths[i];
        if (calc_length)
            spl->totallength += lengths[i];
    }
    if (calc_length)
        spl->inv_totallength = 1.0 / spl->totallength;
    unsigned long c = 0;
    for (vector<cSplineData>::iterator it = datas.begin(); it != datas.end(); ++it) {
        for (int i = 0; i < 14; ++i) {
            spl->datas[c].data[i] = it->data[i];
        }
        c++;
    }
}


void ovlSPLManager::AddSpline(const cSpline& item) {
    Check("ovlSPLManager::AddSpline");
    if (item.name == "")
        BOOST_THROW_EXCEPTION(EOvl("ovlSPLManager::AddSpline called without name"));
    if (m_items.find(item.name) != m_items.end())
        BOOST_THROW_EXCEPTION(EOvl("ovlSPLManager::AddSpline: Item with name '"+item.name+"' already exists"));
    if (item.nodes.size() == 0)
        BOOST_THROW_EXCEPTION(EOvl("ovlSPLManager::AddSpline called without nodes"));


    m_items[item.name] = item;

    m_items[item.name].CalcLengths();
    m_items[item.name].AssignData();

    if (m_items[item.name].lengths.size() != item.nodes.size() - (item.cyclic?0:1))
        BOOST_THROW_EXCEPTION(EOvl("ovlSPLManager::AddSpline called with wrong number of lengths"));
    if (m_items[item.name].datas.size() != item.nodes.size() - (item.cyclic?0:1))
        BOOST_THROW_EXCEPTION(EOvl("ovlSPLManager::AddSpline called with wrong number of data items"));

    m_size += sizeof(Spline);
    m_size += m_items[item.name].nodes.size() * sizeof(SplineNode);
    m_size += m_items[item.name].lengths.size() * sizeof(float);
    m_size += m_items[item.name].datas.size() * 14;

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
        c_data += it->second.datas.size() * 14;
        GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long*>(&c_item->datas));

        it->second.Fill(c_item);

        SymbolStruct* c_symbol = GetLSRManager()->MakeSymbol(OVLT_COMMON, GetStringTable()->FindSymbolString(it->first.c_str(), TAG), reinterpret_cast<unsigned long*>(c_item));
        GetLSRManager()->OpenLoader(OVLT_COMMON, TAG, reinterpret_cast<unsigned long*>(c_item), false, c_symbol);
        GetLSRManager()->CloseLoader(OVLT_COMMON);

    }

}
