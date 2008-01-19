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

#include "RCT3Structs.h"
//#include "3DLoader.h"

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

#define RCT3XML_CSCNFILE wxT("ovl")
#define RCT3XML_CSCNFILE_FLEXITEXTURES wxT("textures")
#define RCT3XML_CSCNFILE_MODELS wxT("models")
#define RCT3XML_CSCNFILE_ANIMATEDMODELS wxT("animatedmodels")
#define RCT3XML_CSCNFILE_ANIMATIONS wxT("animations")
#define RCT3XML_CSCNFILE_LODS wxT("lods")
#define RCT3XML_CSCNFILE_REFERENCES wxT("references")

class cOvl;
class cRawParser;
class cSCNFile: public cRCT3Xml {
friend class cRawParser;
public:
    wxFileName filename;
    wxFileName ovlpath;
    wxString name;
    unsigned long version;
//    long error;
//    wxString objfile;
    bool save_relative_paths;

	cSIVSettings sivsettings;
    cFlexiTexture::vec flexitextures;
    cModel::vec models;
    cAnimatedModel::vec animatedmodels;
    cAnimation::vec animations;
    cLOD::vec lods;
    wxArrayString references;

//    wxArrayString tmpmeshlist;

//    std::vector<cMeshStruct> meshstructs;
//    std::vector<cEffectPoint> effectpoints;
//	std::vector<D3DMATRIX> transforms;
//	wxArrayString transformnames;


	cSCNFile(): filename(wxT("")),version(VERSION_CSCNFILE),save_relative_paths(true),m_work(NULL) {};
	cSCNFile(wxString l_filename, wxXmlDocument* doc = NULL): filename(l_filename),save_relative_paths(true),m_work(NULL) {
	    if (doc) {
	        LoadXML(doc);
	    } else {
            Load();
	    }
    };
	void Init();
	bool Load();
	void LoadTextures(FILE *f);
	bool LoadModels(FILE *f);
	void LoadLODs(FILE *f);
	void LoadReferences(FILE *f);
	bool LoadLegacy(unsigned long objlen, FILE *f);
	bool LoadXML(wxXmlDocument* doc);
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
	void CleanWork();

    virtual bool FromNode(wxXmlNode* node, const wxString& path, unsigned long version);
    virtual wxXmlNode* GetNode(const wxString& path);
    virtual const wxString GetTagName() const {return RCT3XML_CSCNFILE;};
private:
    cSCNFile* m_work;
    bool m_textureovl;
    int m_meshes;
    int m_textures;
//    cMeshStruct MakeMeshStruct(c3DLoader *obj, unsigned int j);
//    void ResetAndReload(c3DLoader *obj);
//    void Fixup(c3DLoader *obj);
    bool FromCompilerXml(wxXmlNode* node, const wxString& path);
};

#endif // SCNFILE_H_INCLUDED
