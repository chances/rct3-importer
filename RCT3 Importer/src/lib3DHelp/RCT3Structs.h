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
#include <wx/xml/xml.h>
#include <vector>

#include "sceneryvisual.h"

#include "3DLoader.h"
#include "RCT3Macros.h"

enum {
    RCT3_TEXTURE_ERROR_OUT = 0,
    RCT3_TEXTURE_SCALE_DOWN = 1,
    RCT3_TEXTURE_SCALE_UP = 2
};

WX_DECLARE_STRING_HASH_MAP(int, cIntMap);

#define RCT3XML_REFERENCE wxT("reference")

#define RCT3XML_MATRIX wxT("matrix")
bool XmlParseMatrixNode(wxXmlNode* node, D3DMATRIX* marix, wxString* name, unsigned long version);
wxXmlNode* XmlMakeMatrixNode(const D3DMATRIX& matrix, const wxString& name);

//bool XmlParseTXYZNode(wxXmlNode* node, txyz* v, unsigned long version);
//wxXmlNode* XmlMakeTXYZNode(const txyz& v);
//typedef vector<txyz>::iterator cTXYZIterator;

typedef wxArrayString::iterator cStringIterator;

struct cText {
	wxString name;
	wxString text;
};

class cRCT3Xml {
public:
    virtual bool FromNode(wxXmlNode* node, const wxString& path, unsigned long version) = 0;
    virtual wxXmlNode* GetNode(const wxString& path) = 0;
    virtual const wxString GetTagName() const = 0;
};

#define RCT3XML_CTXYZ wxT("txyz")
class cTXYZ: public cRCT3Xml {
DEF_RCT3_VECIT(cTXYZ)
public:
    txyz v;

    cTXYZ() {
        v.Time = 0.0;
        v.X = 0.0;
        v.Y = 0.0;
        v.Z = 0.0;
    }
    cTXYZ(const txyz& nv) {
        v = nv;
    }
    operator txyz () {
        return v;
    }

    txyz GetFixed(c3DLoaderOrientation ori);

    virtual bool FromNode(wxXmlNode* node, const wxString& path, unsigned long version);
    virtual wxXmlNode* GetNode(const wxString& path);
    virtual const wxString GetTagName() const {return RCT3XML_CTXYZ;};
};

//txyz operator= (cTXYZ& v);

enum {
    CMS_FUDGE_NONE     = 0,
    CMS_FUDGE_X        = 1,
    CMS_FUDGE_Y        = 2,
    CMS_FUDGE_Z        = 3,
    CMS_FUDGE_XM       = 4,
    CMS_FUDGE_YM       = 5,
    CMS_FUDGE_ZM       = 6,
    CMS_FUDGE_RIM      = 7
};

#define RCT3XML_CMESHSTRUCT wxT("mesh")
class cMeshStruct: public cRCT3Xml {
public:
    typedef std::vector<cMeshStruct>::iterator iterator;
    typedef std::vector<cMeshStruct> vec;

    bool disabled;
    wxString TXS;
    wxString FTX;
    unsigned long place;
    unsigned long flags;
    unsigned long unknown;
    wxString Name;
    unsigned long fudgenormals; // 0 No
                                // 1 X
                                // 2 Y (Up)
                                // 3 Z

    // Helper variables for the UI
    bool valid;
    unsigned long faces;
    bool effectpoint;
    D3DVECTOR effectpoint_vert;

    // Helper for bone assignment
    unsigned int bone;
    // For compiler compatibility
    wxString bonename;

    cMeshStruct() {
        Init();
    };
    cMeshStruct(wxXmlNode* node, const wxString& path, unsigned long version) {
        Init();
        FromNode(node, path, version);
    }
    void CopySettingsFrom(const cMeshStruct& from);

    bool FromCompilerXml(wxXmlNode* node, const wxString& path);

    virtual bool FromNode(wxXmlNode* node, const wxString& path, unsigned long version);
    virtual wxXmlNode* GetNode(const wxString& path);
    virtual const wxString GetTagName() const {return RCT3XML_CMESHSTRUCT;};
private:
    void Init();
};
WX_DECLARE_STRING_HASH_MAP(cMeshStruct *, cMeshStructMap);

#define CFTF_ALPHA_NONE     0
#define CFTF_ALPHA_INTERNAL 1
#define CFTF_ALPHA_EXTERNAL 2

#define RCT3XML_CFLEXITEXTUREANIM wxT("textureanim")
class cFlexiTextureAnim: public cRCT3Xml {
DEF_RCT3_VECIT(cFlexiTextureAnim)
private:
    unsigned long m_frame;
    unsigned long m_count;
public:

    cFlexiTextureAnim(): m_frame(-1), m_count(0) {};
    cFlexiTextureAnim(unsigned long fr, unsigned long co = 1) {
        frame(fr);
        count(co);
    };
    virtual bool FromNode(wxXmlNode* node, const wxString& path, unsigned long version);
    virtual wxXmlNode* GetNode(const wxString& path);
    virtual const wxString GetTagName() const {return RCT3XML_CFLEXITEXTUREANIM;};
DEF_RCT3_D_PROPERTY(unsigned long, m_frame, frame)
DEF_RCT3_D_PROPERTY(unsigned long, m_count, count)
};

class cFlexiTexture;
#define RCT3XML_CFLEXITEXTUREFRAME wxT("textureframe")
struct sFlexiTextureFrame {
    wxFileName Texture;
    wxFileName Alpha;

    unsigned long Recolorable; // We don't expose this yet.
    unsigned char AlphaCutoff;
    unsigned long AlphaSource;
};

class cFlexiTextureFrame: public cRCT3Xml {
DEF_RCT3_VECIT(cFlexiTextureFrame)
DEF_RCT3_RAW(sFlexiTextureFrame, cFlexiTextureFrame)
protected:
    void Init() {
	    texture(wxString(wxT("")));
	    alpha(wxString(wxT("")));
	    recolorable(0);
	    alphacutoff(0);
	    alphasource(CFTF_ALPHA_NONE);
	    used = false;
    };
public:
	bool used;          // Used during ovl generation to see if the texture frame is actually used.

	cFlexiTextureFrame(): used(false) {
	    Init();
	}

	bool Check(cFlexiTexture* parent);

    virtual bool FromNode(wxXmlNode* node, const wxString& path, unsigned long version);
    virtual wxXmlNode* GetNode(const wxString& path);
    virtual const wxString GetTagName() const {return RCT3XML_CFLEXITEXTUREFRAME;};
DEF_RCT3_PROPERTY(wxFileName, Texture, texture)
DEF_RCT3_PROPERTY_NC(wxFileName, Texture, texture)
DEF_RCT3_PROPERTY(wxFileName, Alpha, alpha)
DEF_RCT3_PROPERTY_NC(wxFileName, Alpha, alpha)
DEF_RCT3_PROPERTY(unsigned long, Recolorable, recolorable)
DEF_RCT3_PROPERTY_F(unsigned long, Recolorable, recolorable)
DEF_RCT3_PROPERTY(unsigned char, AlphaCutoff, alphacutoff)
DEF_RCT3_PROPERTY_F(unsigned char, AlphaCutoff, alphacutoff)
DEF_RCT3_PROPERTY(unsigned long, AlphaSource, alphasource)
DEF_RCT3_PROPERTY_F(unsigned long, AlphaSource, alphasource)
};

#define RCT3XML_CFLEXITEXTURE wxT("texture")
class cFlexiTexture: public cRCT3Xml {
private:
    static RGBQUAD g_rgbPalette[256];
    static bool g_rgbPaletteCreated;
public:
    typedef std::vector<cFlexiTexture>::iterator iterator;
    typedef std::vector<cFlexiTexture> vec;

    wxString Name;
    unsigned long FPS;
    cFlexiTextureAnim::vec Animation;
    cFlexiTextureFrame::vec Frames;
    unsigned long Recolorable;
	bool used;          // Used during ovl generation to see if the texture is actually used.

	cFlexiTexture(): Name(wxT("")), FPS(0), Recolorable(0), used(false) {};

	bool Check();

    bool FromCompilerXml(wxXmlNode* node, const wxString& path);

    virtual bool FromNode(wxXmlNode* node, const wxString& path, unsigned long version);
    virtual wxXmlNode* GetNode(const wxString& path);
    virtual const wxString GetTagName() const {return RCT3XML_CFLEXITEXTURE;};

    static RGBQUAD* GetRGBPalette();
};

class cModelBone;
//typedef std::vector<cModelBone>::iterator cModelBoneIterator;
#define RCT3XML_CEFFECTPOINT wxT("effectpoint")
class cEffectPoint: public cRCT3Xml {
public:
    typedef std::vector<cEffectPoint>::iterator iterator;
    typedef std::vector<cEffectPoint> vec;

	wxString name;
	std::vector<D3DMATRIX> transforms;
	wxArrayString transformnames;

	cEffectPoint(): name(wxT("")) {};
	cEffectPoint(const cModelBone& bone);

    virtual bool FromNode(wxXmlNode* node, const wxString& path, unsigned long version);
    virtual wxXmlNode* GetNode(const wxString& path);
    virtual const wxString GetTagName() const {return RCT3XML_CEFFECTPOINT;};
};

class cAnimatedModel;
class cModelMap;
#define RCT3XML_CMODEL wxT("model")
class cModel: public cRCT3Xml {
DEF_RCT3_VECIT(cModel)
public:
    wxString name;
    wxFileName file;
	std::vector<D3DMATRIX> transforms;
	wxArrayString transformnames;
    cMeshStruct::vec meshstructs;
    cEffectPoint::vec effectpoints;
    c3DLoaderOrientation usedorientation;

    c3DLoaderOrientation fileorientation; // This is the orientation reported by the 3DLoader
	wxArrayString error;
	bool fatal_error;
	bool used;          // Used during ovl generation to see if the model is actually used.

	cModel(): name(wxT("")), file(wxT("")), usedorientation(ORIENTATION_UNKNOWN), fileorientation(ORIENTATION_UNKNOWN), fatal_error(false) {};
	cModel(const cAnimatedModel& model);
	cModel(D3DMATRIX def, c3DLoaderOrientation ori);
	cModel(wxString filen): name(wxT("")), file(filen), usedorientation(ORIENTATION_UNKNOWN), fileorientation(ORIENTATION_UNKNOWN), fatal_error(false) {
        Load();
    };
	virtual bool Load();
	bool Load(wxFileName filen) {
	    file = filen;
	    return Load();
	}
	virtual bool Sync();        // Tries to adjust the meshstructs to the contents of the model file.
                                // Usually called after loading from a SCN file
	bool Sync(wxFileName filen) {
	    file = filen;
	    return Sync();
	}

	virtual bool Check(cModelMap& modnames);

    virtual bool FromNode(wxXmlNode* node, const wxString& path, unsigned long version);
    virtual wxXmlNode* AddNodeContent(wxXmlNode* node, const wxString& path, bool do_local);
    virtual wxXmlNode* GetNode(const wxString& path) {
        wxXmlNode* node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, GetTagName());
        AddNodeContent(node, path, true);
        return node;
    }
    virtual const wxString GetTagName() const {return RCT3XML_CMODEL;};

protected:
    void SetupFileProperties(cMeshStruct* ms, c3DLoader* obj, unsigned int n);
    cMeshStruct MakeMesh(c3DLoader* obj, unsigned int n);

    bool CheckMeshes(bool animated = false);
};

WX_DECLARE_STRING_HASH_MAP(cModel *, cModelMap);

#define RCT3XML_CMODELBONE wxT("modelbone")
#define RCT3XML_CMODELBONE_P1 wxT("positions1")
#define RCT3XML_CMODELBONE_P2 wxT("positions2")
#define RCT3XML_CMODELBONE_MESH wxT("bmesh")
class cModelBone: public cRCT3Xml {
public:
    typedef std::vector<cModelBone>::iterator iterator;
    typedef std::vector<cModelBone> vec;

	wxString name;
	wxString parent;
	bool usepos2;
	wxArrayString meshes;
	std::vector<D3DMATRIX> positions1;
	wxArrayString position1names;
	std::vector<D3DMATRIX> positions2;
	wxArrayString position2names;

	// Parent by number
	int nparent;

	cModelBone(): name(wxT("")), parent(wxT("")), usepos2(false) {};
	cModelBone(const cEffectPoint& p): parent(wxT("")), usepos2(false) {
	    name = p.name;
	    positions1 = p.transforms;
	    position1names = p.transformnames;
	};

    bool FromCompilerXml(wxXmlNode* node, const wxString& path);

    virtual bool FromNode(wxXmlNode* node, const wxString& path, unsigned long version);
    virtual wxXmlNode* GetNode(const wxString& path);
    virtual const wxString GetTagName() const {return RCT3XML_CMODELBONE;};
};

// typedef std::vector<cModelBone>::iterator cModelBoneIterator; // Advance above

class cAnimatedModelMap;
#define RCT3XML_CANIMATEDMODEL wxT("animatedmodel")
class cAnimatedModel: public cModel {
public:
    typedef std::vector<cAnimatedModel>::iterator iterator;
    typedef std::vector<cAnimatedModel> vec;

    cModelBone::vec modelbones;

	cAnimatedModel(): cModel() {};
	cAnimatedModel(const cModel& model);
	cAnimatedModel(D3DMATRIX def, c3DLoaderOrientation ori): cModel(def, ori) {};
	cAnimatedModel(wxString filen): cModel() {
	    file = filen;
        Load();
    };

	bool Check(cAnimatedModelMap& amodnames);

    bool FromCompilerXml(wxXmlNode* node, const wxString& path);

    virtual bool FromNode(wxXmlNode* node, const wxString& path, unsigned long version);
    virtual wxXmlNode* AddNodeContent(wxXmlNode* node, const wxString& path, bool do_local);
    virtual const wxString GetTagName() const {return RCT3XML_CANIMATEDMODEL;};
};

WX_DECLARE_STRING_HASH_MAP(cAnimatedModel *, cAnimatedModelMap);

#define RCT3XML_CBONEANIMATION wxT("boneanimation")
#define RCT3XML_CBONEANIMATION_TRANSLATIONS wxT("translations")
#define RCT3XML_CBONEANIMATION_ROTATIONS wxT("rotations")
class cBoneAnimation: public cRCT3Xml {
public:
    typedef std::vector<cBoneAnimation>::iterator iterator;
    typedef std::vector<cBoneAnimation> vec;

    wxString name;
    cTXYZ::vec translations;
    cTXYZ::vec rotations;

    cBoneAnimation(): name(wxT("")) {};

    bool FromCompilerXml(wxXmlNode* node, const wxString& path);

    virtual bool FromNode(wxXmlNode* node, const wxString& path, unsigned long version);
    virtual wxXmlNode* GetNode(const wxString& path);
    virtual const wxString GetTagName() const {return RCT3XML_CBONEANIMATION;};
};

#define RCT3XML_CANIMATION wxT("animation")
class cAnimation: public cRCT3Xml {
public:
    typedef std::vector<cAnimation>::iterator iterator;
    typedef std::vector<cAnimation> vec;

    wxString name;
    cBoneAnimation::vec boneanimations;
    c3DLoaderOrientation usedorientation;

    // Total time
    float totaltime;

    cAnimation(): name(wxT("")), usedorientation(ORIENTATION_UNKNOWN), totaltime(0.0) {};
    cAnimation(c3DLoaderOrientation ori): name(wxT("")), usedorientation(ori), totaltime(0.0) {};

    bool FromCompilerXml(wxXmlNode* node, const wxString& path);
    bool Check(const wxSortedArrayString& presentbones);

    virtual bool FromNode(wxXmlNode* node, const wxString& path, unsigned long version);
    virtual wxXmlNode* GetNode(const wxString& path);
    virtual const wxString GetTagName() const {return RCT3XML_CANIMATION;};
};

#define RCT3XML_CLOD wxT("lod")
#define RCT3XML_CLOD_ANIMATION wxT("lodanim")
class cLOD: public cRCT3Xml {
public:
    typedef std::vector<cLOD>::iterator iterator;
    typedef std::vector<cLOD> vec;

    wxString modelname;
    bool animated;
    wxArrayString animations;
    float distance; //has something to do with the distance this LOD model kicks in at or stops being used at
    unsigned long unk2; //seen 0 so far
    unsigned long unk4; //seen 0 so far
    unsigned long unk14; //seen 0 so far

    cLOD() : modelname(wxT("")), animated(false), distance(0.0), unk2(0), unk4(0), unk14(0) {};

    virtual bool FromNode(wxXmlNode* node, const wxString& path, unsigned long version);
    virtual wxXmlNode* GetNode(const wxString& path);
    virtual const wxString GetTagName() const {return RCT3XML_CLOD;};
};

#define RCT3XML_CSIVSETTINGS wxT("siv")
class cSIVSettings: public cRCT3Xml {
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
    virtual bool FromNode(wxXmlNode* node, const wxString& path, unsigned long version);
    virtual wxXmlNode* GetNode(const wxString& path);
    virtual const wxString GetTagName() const {return RCT3XML_CSIVSETTINGS;};
};

#endif // RCT3STRUCTS_H_INCLUDED
