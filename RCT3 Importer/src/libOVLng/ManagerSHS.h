///////////////////////////////////////////////////////////////////////////////
//
// New OVL creation library
// Manager class for SHS structures
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


#ifndef MANAGERSHS_H_INCLUDED
#define MANAGERSHS_H_INCLUDED

#include <map>
#include <string>
#include <vector>

#include "staticshape.h"
#include "ManagerOVL.h"

using namespace std;

class ovlSHSManager: public ovlOVLManager {
public:
    static const char* TAG;
private:
    vector<StaticShape1*> m_modellist;
    vector<string> m_modelnames;
    map<StaticShape2*, string> m_ftxmap;
    map<StaticShape2*, string> m_txsmap;

    StaticShape1* m_cmodel;
    unsigned long m_nmesh;
    long m_meshcount;
    unsigned long m_neffect;
    long m_effectcount;
public:
    ovlSHSManager(): ovlOVLManager() {
        m_cmodel = NULL;
        m_nmesh = 0;
        m_neffect = 0;
        m_meshcount = 0;
        m_effectcount = 0;
    };
    virtual ~ovlSHSManager();

    void AddModel(const char* name, unsigned long meshes, unsigned long effects);
    void SetBoundingBox(const D3DVECTOR& bbox1, const D3DVECTOR& bbox2);
    void AddEffectPoint(const char* name, const D3DMATRIX& matrix);
    void AddMesh(const char* ftx, const char* txs, unsigned long place, unsigned long flags, unsigned long sides,
                 unsigned long vertexcount, VERTEX* vertices, unsigned long indexcount, unsigned long* indices);

    virtual unsigned char* Make();

    virtual const char* Tag() const {
        return TAG;
    };
};


#endif
