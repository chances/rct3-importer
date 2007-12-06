///////////////////////////////////////////////////////////////////////////////
//
// New OVL creation library
// Manager class for BSH structures
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


#ifndef MANAGERBSH_H_INCLUDED
#define MANAGERBSH_H_INCLUDED

#include <map>
#include <string>
#include <vector>

#include "boneshape.h"
#include "ManagerOVL.h"

using namespace std;

class ovlBSHManager: public ovlOVLManager {
public:
    static const char* NAME;
    static const char* TAG;
private:
    vector<BoneShape1*> m_modellist;
    vector<string> m_modelnames;
    map<BoneShape2*, string> m_ftxmap;
    map<BoneShape2*, string> m_txsmap;

    BoneShape1* m_cmodel;
    unsigned long m_nmesh;
    long m_meshcount;
    unsigned long m_nbone;
    long m_bonecount;
    bool m_rootadded;
public:
    ovlBSHManager(): ovlOVLManager() {
        m_cmodel = NULL;
        m_nmesh = 0;
        m_nbone = 0;
        m_meshcount = 0;
        m_bonecount = 0;
        m_rootadded = false;
    };
    virtual ~ovlBSHManager();

    void AddModel(const char* name, unsigned long meshes, unsigned long bones);
    void SetBoundingBox(const D3DVECTOR& bbox1, const D3DVECTOR& bbox2);
    void AddBone(const char* name, unsigned long parent, const D3DMATRIX& pos1, const D3DMATRIX& pos2, bool isroot = false);
    void AddRootBone();
    void AddMesh(const char* ftx, const char* txs, unsigned long place, unsigned long flags, unsigned long sides,
                 unsigned long vertexcount, VERTEX2* vertices, unsigned long indexcount, unsigned short* indices);

    virtual unsigned char* Make(cOvlInfo* info);

    virtual const char* Name() const {
        return NAME;
    };
    virtual const char* Tag() const {
        return TAG;
    };
};


#endif
