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

#define CSCNFILE_NO_ERROR               0x00000000
#define CSCNFILE_ERROR_FILENAME         0x00000001
#define CSCNFILE_ERROR_WRONG_MAGIC      0x00000002
#define CSCNFILE_ERROR_VERSION          0x00000004

#define CSCNFILE_MAGIC                  "RCT3SCN"

class cSCNFile {
public:
    wxFileName filename;
    wxFileName ovlname;
    long version;
    long error;
//    wxString objfile;
    bool save_relative_paths;

	cSIVSettings sivsettings;
    std::vector<cFlexiTexture> flexitextures;
    std::vector<cModel> models;
    std::vector<cLOD> lods;
    wxArrayString references;

//    wxArrayString tmpmeshlist;

//    std::vector<cMeshStruct> meshstructs;
//    std::vector<cEffectPoint> effectpoints;
//	std::vector<D3DMATRIX> transforms;
//	wxArrayString transformnames;


	cSCNFile(): filename(wxT("")),version(VERSION_CSCNFILE),error(CSCNFILE_NO_ERROR),save_relative_paths(true) {};
	cSCNFile(wxString l_filename): filename(l_filename),error(CSCNFILE_NO_ERROR),save_relative_paths(true) {Load();};
	bool Load();
	void LoadTextures(FILE *f);
	bool LoadModels(FILE *f);
	void LoadLODs(FILE *f);
	void LoadReferences(FILE *f);
	bool LoadLegacy(unsigned long objlen, FILE *f);
//	bool LoadObject(bool reset = true);
	bool Save();
	void SaveTextures(FILE *f);
	void SaveModels(FILE *f);
	void SaveLODs(FILE *f);
	void SaveReferences(FILE *f);
	bool Ok() {return version <= VERSION_CSCNFILE;};
	bool Error() {return error;};
private:
//    cMeshStruct MakeMeshStruct(c3DLoader *obj, unsigned int j);
//    void ResetAndReload(c3DLoader *obj);
//    void Fixup(c3DLoader *obj);
};

#endif // SCNFILE_H_INCLUDED
