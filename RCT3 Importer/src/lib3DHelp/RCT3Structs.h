///////////////////////////////////////////////////////////////////////////////
//
// 3D Utility Library
// C++ structures for the importer
// Copyright (C) 2006 Tobias Minch
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
///////////////////////////////////////////////////////////////////////////////

#ifndef RCT3STRUCTS_H_INCLUDED
#define RCT3STRUCTS_H_INCLUDED

#include "wx_pch.h"

#include <wx/filename.h>
#include <vector>

#include "sceneryvisual.h"

#include "3DLoader.h"

typedef wxArrayString::iterator cStringIterator;

struct cText {
	wxString name;
	wxString text;
};

class cMeshStruct {
public:
    bool disabled;
    wxString TXS;
    wxString FTX;
    unsigned long place;
    unsigned long flags;
    unsigned long unknown;
    wxString Name;

    // Helper variables for the UI
    bool valid;
    unsigned long faces;
    bool effectpoint;
    D3DVECTOR effectpoint_vert;

    cMeshStruct(): disabled(true), TXS(wxT("")), FTX(wxT("")), place(0), flags(0), unknown(3), Name(wxT("")), valid(false), faces(0), effectpoint(0) {};
    void CopySettingsFrom(const cMeshStruct& from);
};

typedef std::vector<cMeshStruct>::iterator cMeshStructIterator;

class cFlexiTextureFrame {
public:
    wxFileName Texture;
    wxFileName Alpha;
    unsigned long Recolorable; // We don't expose this yet.
/*    unsigned long width; // Temporary for the UI to check that all frames are the same size
    unsigned long height;*/
	bool used;          // Used during ovl generation to see if the texture frame is actually used.

	cFlexiTextureFrame(): Texture(wxT("")), Alpha(wxT("")), Recolorable(0), used(false) {};
};

typedef std::vector<cFlexiTextureFrame>::iterator cFlexiTextureFrameIterator;

class cFlexiTexture {
public:
    wxString Name;
    unsigned long FPS;
    std::vector<unsigned long> Animation;
    std::vector<cFlexiTextureFrame> Frames;
    unsigned long Recolorable;
	bool used;          // Used during ovl generation to see if the texture is actually used.

	cFlexiTexture(): Name(wxT("")), FPS(0), Recolorable(0), used(false) {};
};

typedef std::vector<cFlexiTexture>::iterator cFlexiTextureIterator;

struct cEffectPoint {
	wxString Name;
	std::vector<D3DMATRIX> Transform;
	wxArrayString TransformNames;
};

typedef std::vector<cEffectPoint>::iterator cEffectPointIterator;

class cModel {
public:
    wxString name;
    wxFileName file;
	std::vector<D3DMATRIX> transforms;
	wxArrayString transformnames;
    std::vector<cMeshStruct> meshstructs;
    std::vector<cEffectPoint> effectpoints;
	wxArrayString error;
	bool fatal_error;
	bool used;          // Used during ovl generation to see if the model is actually used.

	cModel(): name(wxT("")), file(wxT("")), fatal_error(false) {};
	cModel(D3DMATRIX def);
	cModel(wxString filen): name(wxT("")), file(filen), fatal_error(false) {
        Load();
    };
	bool Load();
	bool Load(wxFileName filen) {
	    file = filen;
	    return Load();
	}
	bool Sync();        // Tries to adjust the meshstructs to the contents of the model file.
                        // Usually called after loading from a SCN file
	bool Sync(wxFileName filen) {
	    file = filen;
	    return Sync();
	}

protected:
    void SetupFileProperties(cMeshStruct* ms, c3DLoader* obj, unsigned int n);
    cMeshStruct MakeMesh(c3DLoader* obj, unsigned int n);
};

typedef std::vector<cModel>::iterator cModelIterator;
WX_DECLARE_STRING_HASH_MAP(cModel *, cModelMap);

class cLOD {
public:
    wxString modelname;
    float distance; //has something to do with the distance this LOD model kicks in at or stops being used at
    unsigned long unk2; //seen 0 so far
    unsigned long unk4; //seen 0 so far
    unsigned long unk14; //seen 0 so far

    cLOD() : modelname(wxT("")), distance(0.0), unk2(0), unk4(0), unk14(0) {};
};

typedef std::vector<cLOD>::iterator cLODIterator;

class cSIVSettings {
public:
    unsigned int sivflags;
    float sway;
    float brightness;
    float unknown;
    float scale;
	unsigned long unk6; //seen 0 so far
	unsigned long unk7; //seen 0 so far
	unsigned long unk8; //seen 0 so far
	unsigned long unk9; //seen 0 so far
	unsigned long unk10; //seen 0 so far
	unsigned long unk11; //seen 0 so far

    cSIVSettings(bool as_tree = false) {
        if (as_tree) {
            sivflags = OVL_SIV_GREENERY | OVL_SIV_ROTATION;
            sway = 0.2;
            brightness = 0.8;
            scale = 0.4;
        } else {
            sivflags = 0;
            sway = 0.0;
            brightness = 1.0;
            scale = 0.0;
        }
        unknown = 1.0;
        unk6 = 0;
        unk7 = 0;
        unk8 = 0;
        unk9 = 0;
        unk10 = 0;
        unk11 = 0;
    };
};

#endif // RCT3STRUCTS_H_INCLUDED
