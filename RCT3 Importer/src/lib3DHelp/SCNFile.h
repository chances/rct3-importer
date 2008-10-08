///////////////////////////////////////////////////////////////////////////////
//
// 3D Utility Library
// Scenery Settings class
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

#ifndef SCNFILE_H_INCLUDED
#define SCNFILE_H_INCLUDED

#include "wx_pch.h"

#include <boost/shared_ptr.hpp>

#include "RCT3Structs.h"
//#include "3DLoader.h"
#include "cXmlDoc.h"

#define VERSION_CSCNFILE 5
#define VERSION_CSCNFILE_FIRSTXML 5

#define CSCNFILE_NO_ERROR               0x00000000
#define CSCNFILE_ERROR_FILENAME         0x00000001
#define CSCNFILE_ERROR_WRONG_MAGIC      0x00000002
#define CSCNFILE_ERROR_VERSION          0x00000004
#define CSCNFILE_ERROR_VERSION_XML      0x00000008
#define CSCNFILE_ERROR_WRONG_ROOT       0x00000010

#define CSCNFILE_MAGIC                  "RCT3SCN"
#define CSCNFILE_XMLFILE                0x6D783F3C  // "<?xm"

#define RCT3XML_CSCNFILE "ovl"
#define RCT3XML_CSCNFILE_FLEXITEXTURES "textures"
#define RCT3XML_CSCNFILE_MODELS "models"
#define RCT3XML_CSCNFILE_ANIMATEDMODELS "animatedmodels"
#define RCT3XML_CSCNFILE_ANIMATIONS "animations"
#define RCT3XML_CSCNFILE_SPLINES "splines"
#define RCT3XML_CSCNFILE_LODS "lods"
#define RCT3XML_CSCNFILE_REFERENCES "references"

class cOvl;
class cRawParser;
class cSCNFile: public cRCT3Xml {
friend class cRawParser;
public:
    wxFileName filename;
    wxFileName ovlpath;
    wxString name;
    wxString prefix;
    unsigned long version;
//    long error;
//    wxString objfile;
    bool save_relative_paths;
    bool imported;

	cSIVSettings sivsettings;
    cFlexiTexture::vec flexitextures;
    cModel::vec models;
    cAnimatedModel::vec animatedmodels;
    cAnimation::vec animations;
    cImpSpline::vec splines;
    cLOD::vec lods;
    cReference::vec references;

    template<class T>
    inline typename T::vec& collection() { return T::breakme_scnfile_collection; }

//    wxArrayString tmpmeshlist;

//    std::vector<cMeshStruct> meshstructs;
//    std::vector<cEffectPoint> effectpoints;
//	std::vector<D3DMATRIX> transforms;
//	wxArrayString transformnames;


	cSCNFile(): filename(wxT("")),version(VERSION_CSCNFILE),save_relative_paths(true),imported(false) {};
	cSCNFile(wxString l_filename, xmlcpp::cXmlDoc* doc = NULL): filename(l_filename),save_relative_paths(true),imported(false) {
	    if (doc) {
	        LoadXML(*doc);
	    } else {
            Load();
	    }
    };
	void Init();
	bool Load();
	inline bool Load(const wxString& l_filename) {
	    filename = l_filename;
	    return Load();
	}
	void LoadTextures(FILE *f);
	bool LoadModels(FILE *f);
	void LoadLODs(FILE *f);
	void LoadReferences(FILE *f);
	bool LoadLegacy(unsigned long objlen, FILE *f);
	bool LoadXML(xmlcpp::cXmlDoc& doc);
//	bool LoadObject(bool reset = true);
	bool Save();
//	void SaveTextures(FILE *f);
//	void SaveModels(FILE *f);
//	void SaveLODs(FILE *f);
//	void SaveReferences(FILE *f);
	void SaveXML();
	bool Ok() {return version <= VERSION_CSCNFILE;};
//	bool Error() {return error;};

	bool CheckForModelNameDuplicates();
	bool Check();
	void Make();
	void MakeToOvl(cOvl& c_ovl);
	void MakeToOvlMain(cOvl& c_ovl);
	void MakeToOvlAnimations(cOvl& c_ovl);
	void MakeToOvlSplines(cOvl& c_ovl);
	wxString getPrefixed(const wxString& pref, bool isTexture = false);
	void CleanWork();

	/// Determines if the ovl is a texture ovl.
	/**
	 * Ovly valid after calling Check()!
	 */
	bool IsTextureOVL() {return m_textureovl;}

    virtual bool FromNode(xmlcpp::cXmlNode& node, const wxString& path, unsigned long version);
    virtual xmlcpp::cXmlNode GetNode(const wxString& path);
    virtual const std::string GetTagName() const {return RCT3XML_CSCNFILE;};

    static wxString GetSupportedFilesFilter() {
        return _("All Supported Files|") + GetSupportedFilesFilterExt() + wxT(";") + c3DLoader::GetSupportedFilesFilterExt() +
            _("|Supported Scenery Files|*.scn;*.xml|Old Scenery Files (*.scn)|*.scn|New Scenery Files (*.xml)|*.xml|") +
            c3DLoader::GetSupportedFilesFilter();
    }
    static wxString GetSupportedFilesFilterExt() {
        return _("*.scn;*.xml");
    }
private:
    boost::shared_ptr<cSCNFile> m_work;
    int m_meshes;
    int m_textures;
    bool m_textureovl;
//    cMeshStruct MakeMeshStruct(c3DLoader *obj, unsigned int j);
//    void ResetAndReload(c3DLoader *obj);
//    void Fixup(c3DLoader *obj);
    bool FromCompilerXml(xmlcpp::cXmlNode& node, const wxString& path);
    bool FromModelFile(boost::shared_ptr<c3DLoader>& model);
};


template<>
inline cFlexiTexture::vec& cSCNFile::collection<cFlexiTexture>() { return flexitextures; }
template<>
inline cModel::vec& cSCNFile::collection<cModel>() { return models; }
template<>
inline cAnimatedModel::vec& cSCNFile::collection<cAnimatedModel>() { return animatedmodels; }
template<>
inline cAnimation::vec& cSCNFile::collection<cAnimation>() { return animations; }
template<>
inline cImpSpline::vec& cSCNFile::collection<cImpSpline>() { return splines; }
template<>
inline cLOD::vec& cSCNFile::collection<cLOD>() { return lods; }
template<>
inline cReference::vec& cSCNFile::collection<cReference>() { return references; }


#endif // SCNFILE_H_INCLUDED
