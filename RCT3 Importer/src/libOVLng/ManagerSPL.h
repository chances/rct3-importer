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


#ifndef MANAGERSPL_H_INCLUDED
#define MANAGERSPL_H_INCLUDED

#include <map>
#include <string>
#include <vector>

#include "spline.h"
#include "ManagerOVL.h"
#include "ManagerCommon.h"

using namespace std;

class cSplineData {
public:
    unsigned char data[14];

    cSplineData() {
        for (int i = 0; i < 14; ++i) {
            data[i] = 0;
        }
    }
    void MakeStrange() {
        for (unsigned char i = 1; i < 15; ++i) {
            data[i-1] = (i * 16) + i;
        }
    }
};

class cSpline {
public:
    string name;
    vector<SplineNode> nodes;
    unsigned long cyclic;
    float unk3;
    vector<float> lengths;
    vector<cSplineData> unknowndata;
    float unk6;

    cSpline() {
        cyclic = 0;
        unk3 = 0.0;
        unk6 = 0.0;
    };
    void Fill(Spline* spl) {
        spl->count = nodes.size();
        spl->cyclic = cyclic;
        spl->length = 0.0;
        spl->unk3 = unk3;
        spl->unk6 = unk6;
        for (unsigned int i = 0; i < nodes.size(); ++i) {
            spl->nodes[i] = nodes[i];
        }
        for (unsigned int i = 0; i < lengths.size(); ++i) {
            spl->lengths[i] = lengths[i];
            spl->length += lengths[i];
        }
        unsigned long c = 0;
        for (vector<cSplineData>::iterator it = unknowndata.begin(); it != unknowndata.end(); ++it) {
            for (int i = 0; i < 14; ++i) {
                spl->unk5[c] = it->data[i];
                c++;
            }
        }
    }
};

class ovlSPLManager: public ovlOVLManager {
public:
    static const char* LOADER;
    static const char* NAME;
    static const char* TAG;
private:
    map<string, cSpline>  m_items;

public:
    ovlSPLManager(): ovlOVLManager() {
    };

    void AddSpline(const cSpline& item);

    virtual unsigned char* Make(cOvlInfo* info);

    virtual const char* Name() const {
        return NAME;
    };
    virtual const char* Tag() const {
        return TAG;
    };
    virtual const char* Loader() const {
        return LOADER;
    };
};



#endif
