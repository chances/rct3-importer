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

#include <wx/filename.h>
#include <string>
#include <vector>

#include "boost_unordered_extension.h"

#include "cryptoguid.h"
#include "rct3constants.h"
#include "sceneryvisual.h"

#include "cXmlNode.h"

#include "3DLoader.h"
#include "RCT3Macros.h"

enum {
    RCT3_TEXTURE_ERROR_OUT = 0,
    RCT3_TEXTURE_SCALE_DOWN = 1,
    RCT3_TEXTURE_SCALE_UP = 2
};

//WX_DECLARE_STRING_HASH_MAP(int, cIntMap);
typedef std::map<wxString, int> cIntMap;


#define RCT3XML_MATRIX "matrix"
bool XmlParseMatrixNode(xmlcpp::cXmlNode& node, r3::MATRIX* marix, wxString* name, unsigned long version);
xmlcpp::cXmlNode XmlMakeMatrixNode(const r3::MATRIX& matrix, const wxString& name);

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
	cryptoGUID guid;

    virtual bool FromNode(xmlcpp::cXmlNode& node, const wxString& path, unsigned long version) = 0;
    virtual xmlcpp::cXmlNode GetNode(const wxString& path) = 0;
    virtual const std::string GetTagName() const = 0;

    static wxString getSingular() { return wxT("UNIMPLEMENTED"); }
    static wxString getPlural() { return wxT("UNIMPLEMENTEDs"); }

	cRCT3Xml(): guid(true) {}
    virtual ~cRCT3Xml() {}
};

#define RCT3XML_CTXYZ "txyz"
class cTXYZ: public cRCT3Xml {
DEF_RCT3_VECIT(cTXYZ)
public:
    r3::txyz v;

    cTXYZ() {
        v.Time = 0.0;
        v.X = 0.0;
        v.Y = 0.0;
        v.Z = 0.0;
    }
    cTXYZ(const r3::txyz& nv) {
        v = nv;
    }
    operator r3::txyz () {
        return v;
    }

    r3::txyz GetFixed(c3DLoaderOrientation ori, bool rotate, bool axis = false) const;

    virtual bool FromNode(xmlcpp::cXmlNode& node, const wxString& path, unsigned long version);
    virtual xmlcpp::cXmlNode GetNode(const wxString& path);
    virtual const std::string GetTagName() const {return RCT3XML_CTXYZ;};
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
    CMS_FUDGE_RIM      = 7,
    CMS_FUDGE_ENUMSIZE
};

#define RCT3XML_CMESHSTRUCT "mesh"
class cMeshStruct: public cRCT3Xml {
DEF_RCT3_VECIT(cMeshStruct)
public:
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
    bool multibone;

    // Helper variables for the UI
    bool valid;
    unsigned long faces;
    //bool effectpoint;
    r3::VECTOR effectpoint_vert;
    std::set<wxString> boneassignment;

    // Helper for bone assignment
    unsigned int bone;
    // For compiler compatibility
    wxString bonename;

	wxString algo_x;
	wxString algo_y;
	wxString algo_z;

    cMeshStruct() {
        Init();
    };
    cMeshStruct(xmlcpp::cXmlNode& node, const wxString& path, unsigned long version) {
        Init();
        FromNode(node, path, version);
    }
    void CopySettingsFrom(const cMeshStruct& from);
    void autoMeshStyle(const wxString& style);
    bool hasIdenticalSettingsAs(const cMeshStruct& to) {
        return (TXS == to.TXS) &&
               (FTX == to.FTX) &&
               (place == to.place) &&
               (flags == to.flags) &&
               (unknown == to.unknown) &&
               (fudgenormals == to.fudgenormals);
    }

    bool FromCompilerXml(xmlcpp::cXmlNode& node, const wxString& path);

    void Check(const wxString& modelname);

    virtual bool FromNode(xmlcpp::cXmlNode& node, const wxString& path, unsigned long version);
    virtual xmlcpp::cXmlNode GetNode(const wxString& path);
    virtual const std::string GetTagName() const {return RCT3XML_CMESHSTRUCT;};

    static unsigned long getRightTransparencyValue(const wxString& txs);
private:
    void Init();
};
//WX_DECLARE_STRING_HASH_MAP(cMeshStruct *, cMeshStructMap);
typedef std::map<wxString, cMeshStruct*> cMeshStructMap;

class cTextureStyle {
private:
    static std::vector<wxString> m_texturestyles;
    static void makeStyles();
public:
    static inline const std::vector<wxString>& getTextureStyles() {
        if (!m_texturestyles.size())
            makeStyles();
        return m_texturestyles;
    }
    static bool isValid(const wxString& style);
};

#define CFTF_ALPHA_NONE     0
#define CFTF_ALPHA_INTERNAL 1
#define CFTF_ALPHA_EXTERNAL 2

#define RCT3XML_CFLEXITEXTUREANIM "textureanim"
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
    virtual bool FromNode(xmlcpp::cXmlNode& node, const wxString& path, unsigned long version);
    virtual xmlcpp::cXmlNode GetNode(const wxString& path);
    virtual const std::string GetTagName() const {return RCT3XML_CFLEXITEXTUREANIM;};
DEF_RCT3_D_PROPERTY(unsigned long, m_frame, frame)
DEF_RCT3_D_PROPERTY(unsigned long, m_count, count)
};

class cFlexiTexture;
#define RCT3XML_CFLEXITEXTUREFRAME "textureframe"
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
	cFlexiTextureFrame(const cFlexiTextureFrame& o_fr) {
	    m_raw = o_fr.m_raw;
	    used = o_fr.used;
	}
	cFlexiTextureFrame& operator =(const cFlexiTextureFrame& o_fr) {
	    m_raw = o_fr.m_raw;
	    used = o_fr.used;
	    return *this;
    }

	bool Check(cFlexiTexture* parent);

    virtual bool FromNode(xmlcpp::cXmlNode& node, const wxString& path, unsigned long version);
    virtual xmlcpp::cXmlNode GetNode(const wxString& path);
    virtual const std::string GetTagName() const {return RCT3XML_CFLEXITEXTUREFRAME;};
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

#define RCT3XML_CFLEXITEXTURE "texture"
class cFlexiTexture: public cRCT3Xml {
DEF_RCT3_VECIT(cFlexiTexture)
private:
    static r3::COLOURQUAD g_rgbPalette[256];
    static bool g_rgbPaletteCreated;
    static r3::COLOURQUAD g_bmyPalette[256];
    static bool g_bmyPaletteCreated;
    static boost::caseless_set m_reserved;
    static void makeReserved();	
public:
    wxString Name;
    unsigned long FPS;
    cFlexiTextureAnim::vec Animation;
    cFlexiTextureFrame::vec Frames;
    unsigned long Recolorable;
	bool used;          // Used during ovl generation to see if the texture is actually used.

	cFlexiTexture(): Name(wxT("")), FPS(0), Recolorable(0), used(false) {};

	bool Check();

    bool FromCompilerXml(xmlcpp::cXmlNode& node, const wxString& path);

    virtual bool FromNode(xmlcpp::cXmlNode& node, const wxString& path, unsigned long version);
    virtual xmlcpp::cXmlNode GetNode(const wxString& path);
    virtual const std::string GetTagName() const {return RCT3XML_CFLEXITEXTURE;};

    static wxString getSingular() { return wxT("texture"); }
    static wxString getPlural() { return wxT("textures"); }

    static const r3::COLOURQUAD* GetRGBPalette();
    static const r3::COLOURQUAD* GetBMYPalette();
    static inline const boost::caseless_set& getReserved() {
        if (!m_reserved.size())
            makeReserved();
        return m_reserved;
    }
    static bool isReserved(const wxString& t_ftx);
};

class cModelBone;
//typedef std::vector<cModelBone>::iterator cModelBoneIterator;
#define RCT3XML_CEFFECTPOINT "effectpoint"
class cEffectPoint: public cRCT3Xml {
DEF_RCT3_VECIT(cEffectPoint)
public:

	wxString name;
	std::vector<r3::MATRIX> transforms;
	wxArrayString transformnames;

	cEffectPoint(): name(wxT("")) {};
	cEffectPoint(const cModelBone& bone);

    virtual bool FromNode(xmlcpp::cXmlNode& node, const wxString& path, unsigned long version);
    virtual xmlcpp::cXmlNode GetNode(const wxString& path);
    virtual const std::string GetTagName() const {return RCT3XML_CEFFECTPOINT;};
};

class cAnimatedModel;
class cModel;
//WX_DECLARE_STRING_HASH_MAP(cModel *, cModelMap);
typedef std::map<wxString, cModel*> cModelMap;
#define RCT3XML_CMODEL "model"
class cModel: public cRCT3Xml {
DEF_RCT3_VECIT(cModel)
public:
    wxString name;
    wxFileName file;
	std::vector<r3::MATRIX> transforms;
	wxArrayString transformnames;
    cMeshStruct::vec meshstructs;
    cEffectPoint::vec effectpoints;
    c3DLoaderOrientation usedorientation;
    wxString deducedname;
    bool auto_bones;
    bool auto_sort;

    c3DLoaderOrientation fileorientation; // This is the orientation reported by the 3DLoader
	wxArrayString error;
	bool fatal_error;
	bool used;          // Used during ovl generation to see if the model is actually used.
	std::map<wxString, c3DBone> model_bones;
	std::vector<std::vector<int> > mesh_compaction;

	cModel(): name(wxT("")), file(wxT("")), usedorientation(ORIENTATION_UNKNOWN), auto_bones(false), auto_sort(false), fileorientation(ORIENTATION_UNKNOWN), fatal_error(false) {};
	cModel(const cAnimatedModel& model);
	cModel(r3::MATRIX def, c3DLoaderOrientation ori);
	cModel(wxString filen): name(wxT("")), file(filen), usedorientation(ORIENTATION_UNKNOWN), auto_bones(false), auto_sort(false), fileorientation(ORIENTATION_UNKNOWN), fatal_error(false) {
        Load(false);
    };
	virtual bool Load(bool asoverlay);
	bool Load(wxFileName filen, bool asoverlay = false) {
	    file = filen;
	    return Load(asoverlay);
	}
	virtual bool Sync();        // Tries to adjust the meshstructs to the contents of the model file.
                                // Usually called after loading from a SCN file
	bool Sync(wxFileName filen) {
	    file = filen;
	    return Sync();
	}

	virtual bool Check(cModelMap& modnames);
	virtual bool GetTransformationMatrices(r3::MATRIX& transform, r3::MATRIX& undodamage) const;
	virtual void addBone(const c3DBone& bone);
	virtual void autoBones(const std::set<wxString>* onlyaddfrom = NULL);
	virtual void syncBones();
	virtual void sortBones();

    virtual bool FromNode(xmlcpp::cXmlNode& node, const wxString& path, unsigned long version);
    virtual void AddNodeContent(xmlcpp::cXmlNode& node, const wxString& path, bool do_local);
    virtual xmlcpp::cXmlNode GetNode(const wxString& path) {
        xmlcpp::cXmlNode node(GetTagName());
        AddNodeContent(node, path, true);
        return node;
    }
    virtual const std::string GetTagName() const {return RCT3XML_CMODEL;};

    static wxString getSingular() { return _("model"); }
    static wxString getPlural() { return _("models"); }

protected:
    void SetupFileProperties(cMeshStruct* ms, const boost::shared_ptr<c3DLoader>& obj, unsigned int n);
    cMeshStruct MakeMesh(const boost::shared_ptr<c3DLoader>& obj, unsigned int n);

    bool CheckMeshes(bool animated = false);
};


#define RCT3XML_CMODELBONE "modelbone"
#define RCT3XML_CMODELBONE_P1 "positions1"
#define RCT3XML_CMODELBONE_P2 "positions2"
#define RCT3XML_CMODELBONE_MESH "bmesh"
class cModelBone: public cRCT3Xml {
DEF_RCT3_VECIT(cModelBone)
public:
	wxString name;
	wxString parent;
	bool usepos2;
	wxArrayString meshes;
	std::vector<r3::MATRIX> positions1;
	wxArrayString position1names;
	std::vector<r3::MATRIX> positions2;
	wxArrayString position2names;

	// Parent by number
	int nparent;

	cModelBone(): name(wxT("")), parent(wxT("")), usepos2(false) {};
	cModelBone(const cEffectPoint& p): parent(wxT("")), usepos2(false) {
	    name = p.name;
	    positions1 = p.transforms;
	    position1names = p.transformnames;
	};

    bool FromCompilerXml(xmlcpp::cXmlNode& node, const wxString& path);

    virtual bool FromNode(xmlcpp::cXmlNode& node, const wxString& path, unsigned long version);
    virtual xmlcpp::cXmlNode GetNode(const wxString& path);
    virtual const std::string GetTagName() const {return RCT3XML_CMODELBONE;};
};

// typedef std::vector<cModelBone>::iterator cModelBoneIterator; // Advance above

class cAnimatedModel;
//WX_DECLARE_STRING_HASH_MAP(cAnimatedModel *, cAnimatedModelMap);
typedef std::map<wxString, cAnimatedModel*> cAnimatedModelMap;
#define RCT3XML_CANIMATEDMODEL "animatedmodel"
class cAnimatedModel: public cModel {
DEF_RCT3_VECIT(cAnimatedModel)
public:
    cModelBone::vec modelbones;

	cAnimatedModel(): cModel() {};
	cAnimatedModel(const cModel& model);
	cAnimatedModel(r3::MATRIX def, c3DLoaderOrientation ori): cModel(def, ori) {};
	cAnimatedModel(wxString filen): cModel() {
	    file = filen;
        Load(false);
    };

	bool Check(cAnimatedModelMap& amodnames);
	virtual void addBone(const c3DBone& bone);
	virtual void autoBones(const std::set<wxString>* onlyaddfrom = NULL);
	virtual void syncBones();
	virtual void sortBones();

    bool FromCompilerXml(xmlcpp::cXmlNode& node, const wxString& path);

    virtual bool FromNode(xmlcpp::cXmlNode& node, const wxString& path, unsigned long version);
    virtual void AddNodeContent(xmlcpp::cXmlNode& node, const wxString& path, bool do_local);
    virtual const std::string GetTagName() const {return RCT3XML_CANIMATEDMODEL;};

    static wxString getSingular() { return _("animated model"); }
    static wxString getPlural() { return _("animated models"); }
};


#define RCT3XML_CBONEANIMATION "boneanimation"
#define RCT3XML_CBONEANIMATION_TRANSLATIONS "translations"
#define RCT3XML_CBONEANIMATION_ROTATIONS "rotations"
class cBoneAnimation: public cRCT3Xml {
DEF_RCT3_VECIT(cBoneAnimation)
public:
    wxString name;
    cTXYZ::vec translations;
    cTXYZ::vec rotations;
    bool axis;

    cBoneAnimation(): name(wxT("")), axis(false) {};

    int decimateTranslations(float threshold);
    int decimateRotations(float threshold, float bailout);

    bool FromCompilerXml(xmlcpp::cXmlNode& node, const wxString& path);

    virtual bool FromNode(xmlcpp::cXmlNode& node, const wxString& path, unsigned long version);
    virtual xmlcpp::cXmlNode GetNode(const wxString& path);
    virtual const std::string GetTagName() const {return RCT3XML_CBONEANIMATION;};
};

#define RCT3XML_CANIMATION "animation"
class cAnimation: public cRCT3Xml {
DEF_RCT3_VECIT(cAnimation)
public:
    wxString name;
    cBoneAnimation::vec boneanimations;
    c3DLoaderOrientation usedorientation;

    // Total time
    float totaltime;

    cAnimation(): name(wxT("")), usedorientation(ORIENTATION_UNKNOWN), totaltime(0.0) {};
    cAnimation(c3DLoaderOrientation ori): name(wxT("")), usedorientation(ori), totaltime(0.0) {};

    bool FromCompilerXml(xmlcpp::cXmlNode& node, const wxString& path);
    bool Check(const wxSortedArrayString& presentbones);
    void CheckTimes();

    virtual bool FromNode(xmlcpp::cXmlNode& node, const wxString& path, unsigned long version);
    virtual xmlcpp::cXmlNode GetNode(const wxString& path);
    virtual const std::string GetTagName() const {return RCT3XML_CANIMATION;};

    static wxString getSingular() { return _("animation"); }
    static wxString getPlural() { return _("animations"); }
};

#define RCT3XML_CLOD "lod"
#define RCT3XML_CLOD_ANIMATION "lodanim"
class cLOD: public cRCT3Xml {
DEF_RCT3_VECIT(cLOD)
public:
    wxString modelname;
    bool animated;
    std::vector<wxString> animations;
    float distance; //has something to do with the distance this LOD model kicks in at or stops being used at
    unsigned long unk2; //seen 0 so far
    unsigned long unk4; //seen 0 so far
    unsigned long unk14; //seen 0 so far

    cLOD() : modelname(wxT("")), animated(false), distance(0.0), unk2(0), unk4(0), unk14(0) {};

    virtual bool FromNode(xmlcpp::cXmlNode& node, const wxString& path, unsigned long version);
    virtual xmlcpp::cXmlNode GetNode(const wxString& path);
    virtual const std::string GetTagName() const {return RCT3XML_CLOD;};
};

#define RCT3XML_CSPLINE "spline"
#define RCT3XML_CSPLINE_NODE "node"
class cImpSpline: public cRCT3Xml {
DEF_RCT3_VECIT(cImpSpline)
public:
    c3DSpline spline;
    c3DLoaderOrientation usedorientation;

    cImpSpline() : usedorientation(ORIENTATION_UNKNOWN) {};
    cImpSpline(const c3DSpline& sp, c3DLoaderOrientation ori) : spline(sp), usedorientation(ori) {};

    bool Check();

    virtual bool FromNode(xmlcpp::cXmlNode& node, const wxString& path, unsigned long version);
    virtual xmlcpp::cXmlNode GetNode(const wxString& path);
    virtual const std::string GetTagName() const {return RCT3XML_CSPLINE;};

    static wxString getSingular() { return _("spline"); }
    static wxString getPlural() { return _("splines"); }
};

#define RCT3XML_CSIVSETTINGS "siv"
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
            sivflags = r3::Constants::SVD::Flags::Greenery | r3::Constants::SVD::Flags::Rotation;
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
    cSIVSettings(const c3DLoader* obj) {
        sivflags = obj->m_flags;
        sway = obj->m_sway;
        brightness = obj->m_brightness;
        scale = obj->m_scale;
        unknown = obj->m_unk;
        unk6 = 0;
        unk7 = 0;
        unk8 = 0;
        unk9 = 0;
        unk10 = 0;
        unk11 = 0;
    }
    virtual bool FromNode(xmlcpp::cXmlNode& node, const wxString& path, unsigned long version);
    virtual xmlcpp::cXmlNode GetNode(const wxString& path);
    virtual const std::string GetTagName() const {return RCT3XML_CSIVSETTINGS;};
};

#define RCT3XML_REFERENCE "reference"
class cReference: public cRCT3Xml {
DEF_RCT3_VECIT(cReference)
public:
    wxString name;

    cReference() {};
    cReference(const wxString& ref): name(ref) {};

    virtual bool FromNode(xmlcpp::cXmlNode& node, const wxString& path, unsigned long version);
    virtual xmlcpp::cXmlNode GetNode(const wxString& path);
    virtual const std::string GetTagName() const {return RCT3XML_REFERENCE;};

    bool operator==(const wxString& c) const { return c == name; }

    static wxString getSingular() { return _("reference"); }
    static wxString getPlural() { return _("references"); }
};


template<class T>
inline const wxString& NameExtractorC(const T& t) { return t.name; }

template<class T>
inline wxString& NameExtractor(T& t) { return t.name; }

template<>
inline const wxString& NameExtractorC(const cImpSpline& t) { return t.spline.m_name; }

template<>
inline wxString& NameExtractor(cImpSpline& t) { return t.spline.m_name; }

template<>
inline const wxString& NameExtractorC(const cFlexiTexture& t) { return t.Name; }

template<>
inline wxString& NameExtractor(cFlexiTexture& t) { return t.Name; }

template<>
inline const wxString& NameExtractorC(const c3DBone& t) { return t.m_name; }

template<>
inline wxString& NameExtractor(c3DBone& t) { return t.m_name; }

template<class T>
struct NamePredicate {
    const wxString& c;
    NamePredicate(const wxString& _c): c(_c) {}
    template<class X>
    NamePredicate(const X& _x): c(NameExtractorC<X>(_x)) {}
    bool operator() (const T& v) {
        return c.IsSameAs(NameExtractorC(v));
    }
};

template<class T, class C>
bool NameCompare (const T& a, const C& b) {
    return (NameExtractorC<T>(a) < NameExtractorC<C>(b));
}
/*
template<class T>
struct EffectCompare {
    static long getPriority(const wxString& n) {
        long prio = 0;
        if (n.IsSameAs("vendor")) {
            prio = READ_RCT3_PRIORITY_VENDOR();
        } else if (n.IsSameAs("nozbl") {
            prio = READ_RCT3_PRIORITY_FOUNTAIN_BL();
        } else if (n.IsSameAs("nozbr") {
            prio = READ_RCT3_PRIORITY_FOUNTAIN_BR();
        } else if (n.IsSameAs("noztl") {
            prio = READ_RCT3_PRIORITY_FOUNTAIN_TL();
        } else if (n.IsSameAs("noztr") {
            prio = READ_RCT3_PRIORITY_FOUNTAIN_TR();
        }
        return prio;
    }
    bool operator() (const T& a, const T& b) {
        return getPriority(NameExtractorC(a)) > getPriority(NameExtractorC(b));
    }
}
*/

#endif // RCT3STRUCTS_H_INCLUDED
