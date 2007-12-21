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


#ifndef MANAGERSVD_H_INCLUDED
#define MANAGERSVD_H_INCLUDED

#include <map>
#include <string>
#include <vector>

#include "sceneryvisual.h"
#include "ManagerOVL.h"

using namespace std;

class ovlSVDManager: public ovlOVLManager {
public:
    static const char* NAME;
    static const char* TAG;
private:
    vector<SceneryItemVisual*> m_svdlist;
    vector<string> m_svdnames;
    map<SceneryItemVisualLOD*, string> m_modelmap;
    map<SceneryItemVisualLOD*, vector<string> > m_animationmap;

    SceneryItemVisual* m_csvd;
    //SceneryItemVisualLOD* m_clod;
    long m_lodcount;
    unsigned long m_nlod;
    long m_animcount;
    //unsigned long m_nanim;
public:
    ovlSVDManager(): ovlOVLManager() {
        m_csvd = NULL;
        //m_clod = NULL;
        m_lodcount = 0;
        m_nlod = 0;
        m_animcount = 0;
        //m_nanim = 0;
    };
    virtual ~ovlSVDManager();

    void AddSVD(const char* name, unsigned long lods, unsigned long flags = 0, float sway = 0.0, float brightness = 1.0, float scale = 0.0);
    void SetSVDUnknowns(float unk4, unsigned long unk6, unsigned long unk7, unsigned long unk8, unsigned long unk9, unsigned long unk10, unsigned long unk11);
    void AddStaticLOD(const char* name, const char* modelname, float distance, unsigned long unk2 = 0, unsigned long unk4 = 0, unsigned long unk14 = 0);
    void OpenAnimatedLOD(const char* name, const char* modelname, unsigned long animations, float distance, unsigned long unk2 = 0, unsigned long unk4 = 0, unsigned long unk14 = 0);
    void AddAnimation(const char* name);
    void CloseAnimatedLOD();

    virtual void Make(cOvlInfo* info);

    virtual const char* Name() const {
        return NAME;
    };
    virtual const char* Tag() const {
        return TAG;
    };
};



#endif
