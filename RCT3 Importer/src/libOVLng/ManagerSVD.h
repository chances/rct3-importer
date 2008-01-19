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

class cSceneryItemVisualLOD {
public:
	unsigned long meshtype; //seems to be flag for what type of mesh this is, 0 = StaticShape, 3 = BoneShape, 4 = Billboard
	string name;
	string staticshape; //Set to 0 in disk files, is the StaticStape for static meshes in ram
	unsigned long unk2; //seen 0 so far
	string boneshape; //Set to 0 in disk files, is the BoneStape for animated meshes in ram
	unsigned long unk4; //seen 0 so far
	string fts; //Set to 0 in disk files, is the FlexiTexture for a billboard object
	string txs; //Set to 0 in disk files, is the Texture Style for a billboard object, is always BillboardStandard.
	float unk7; //seen 1.0 so far
	float unk8; //seen 1.0 so far
	float unk9; //seen 0 so far
	float unk10; //seen 1.0 so far
	float unk11; //seen 0 so far
	float unk12; //seen 1.0 so far
	float distance; //has something to do with the distance this LOD model kicks in at or stops being used at
	unsigned long unk14; //seen 0 so far
	vector<string> animations; //points to one or more BoneAnim pointers which are 0 in disk files

    cSceneryItemVisualLOD() {
        meshtype = 0;
        unk2 = 0;
        unk4 = 0;
        unk7 = 1.0;
        unk8 = 1.0;
        unk9 = 0.0;
        unk10 = 1.0;
        unk11 = 0.0;
        unk12 = 1.0;
        distance = 0.0;
        unk14 = 0;
    }
    void Fill(SceneryItemVisualLOD* lod) {
        lod->MeshType = meshtype;
        lod->StaticShape = NULL;
        lod->unk2 = unk2;
        lod->BoneShape = NULL;
        lod->unk4 = unk4;
        lod->fts = NULL;
        lod->TextureData = NULL;
        lod->unk7 = unk7;
        lod->unk8 = unk8;
        lod->unk9 = unk9;
        lod->unk10 = unk10;
        lod->unk11 = unk11;
        lod->unk12 = unk12;
        lod->distance = distance;
        lod->AnimationCount = animations.size();
        lod->unk14 = unk14;
    }
};

class cSceneryItemVisual {
public:
    string name;
	unsigned long sivflags; // Flags
	                    // 0x00000001 Set on Trees, Shrubs & Fern
	                    // 0x00000002 Flowers
	                    // 0x00000004 Set on Trees & Shrubs, Rotational Variation
	                    // 0x00000070 Unknown
	                    // 0x01000000 Soaked!
	                    // 0x02000000 Wild!
	float sway;         //seen 0.0 so far, 0.2 on trees & shrubs that move. 0.0 for safari theme trees. Amount of Swaying
	float brightness;   //seen 1.0 so far, 0.8 on trees & shrubs that move. Brightness Variation
	float unk4;         //seen 1.0 so far
	float scale;        //seen 0.0 so far, 0.4 on trees & shrubs that move. Scale Variation
	vector<cSceneryItemVisualLOD> lods;
	unsigned long unk6; //seen 0 so far
	unsigned long unk7; //seen 0 so far
	unsigned long unk8; //seen 0 so far
	unsigned long unk9; //seen 0 so far
	unsigned long unk10; //seen 0 so far
	unsigned long unk11; //seen 0 so far

	cSceneryItemVisual() {
	    sivflags = 0;
	    sway = 0.0;
	    brightness = 1.0;
	    unk4 = 1.0;
	    scale = 0.0;
	    unk6 = 0;
	    unk7 = 0;
	    unk8 = 0;
	    unk9 = 0;
	    unk10 = 0;
	    unk11 = 0;
	}
	void Fill(SceneryItemVisual* siv) {
	    siv->sivflags = sivflags;
	    siv->sway = sway;
	    siv->brightness = brightness;
	    siv->unk4 = unk4;
	    siv->scale = scale;
	    siv->LODCount = lods.size();
	    siv->unk6 = unk6;
	    siv->unk7 = unk7;
	    siv->unk8 = unk8;
	    siv->unk9 = unk9;
	    siv->unk10 = unk10;
	    siv->unk11 = unk11;
        for (unsigned long i = 0; i < lods.size(); ++i) {
            lods[i].Fill(siv->LODMeshes[i]);
        }
	}
};

class ovlSVDManager: public ovlOVLManager {
public:
    static const char* NAME;
    static const char* TAG;
private:
    map<string, cSceneryItemVisual> m_items;
public:
    ovlSVDManager(): ovlOVLManager() {};

    void AddSVD(const cSceneryItemVisual& item);

    virtual void Make(cOvlInfo* info);

    virtual const char* Name() const {
        return NAME;
    };
    virtual const char* Tag() const {
        return TAG;
    };
};

/*
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
*/

#endif
