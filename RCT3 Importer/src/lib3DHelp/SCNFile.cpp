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
// Parts of this code are based on code written for rct3tool (Copyright 2005
// Jonathan Wilson).
//
///////////////////////////////////////////////////////////////////////////////

#include "SCNFile.h"

#include <boost/algorithm/string/classification.hpp>
#include <boost/format.hpp>
#include <sstream>
#include <stdio.h>

#include <wx/log.h>

#include "confhelp.h"
#include "gximage.h"
#include "lib3Dconfig.h"
#include "matrix.h"
#include "OVLException.h"
#include "OVLManagers.h"
#include "OVLng.h"
#include "pretty.h"
#include "RCT3Exception.h"
#include "rct3log.h"
#include "texcheck.h"
#include "xmldefs.h"
#include "xmlhelper.h"

#include "wxexception_libxmlcpp.h"

#include "cXmlInputOutputCallbackString.h"
#include "cXmlValidatorRNVRelaxNG.h"

#include "rng/rct3xml-ovlcompiler-v1.rnc.gz.h"
#include "rng/rct3xml-scenery-v1.rnc.gz.h"

#ifndef MAX_PATH
#define MAX_PATH 256
#endif

#define ASCIIALNUM "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"

using namespace r3;
using namespace std;
using namespace xmlcpp;

bool cSCNFile::CheckForModelNameDuplicates() {
    for (cModel::iterator moda = models.begin(); moda != models.end(); moda++) {
        // Internal dups
        for (cModel::iterator modi = models.begin(); modi != models.end(); modi++) {
            // Skip same
            if (modi == moda)
                continue;
            if (modi->name == moda->name) {
                return false;
            }
        }

        // Animated dups
        for (cAnimatedModel::iterator amod = animatedmodels.begin(); amod != animatedmodels.end(); amod++) {
            if (moda->name == amod->name) {
                return false;
            }
        }
    }
    // Animated internal dups
    for (cAnimatedModel::iterator amoda = animatedmodels.begin(); amoda != animatedmodels.end(); amoda++) {
        for (cAnimatedModel::iterator amodi = animatedmodels.begin(); amodi != animatedmodels.end(); amodi++) {
            // Skip same
            if (amodi == amoda)
                continue;
            if (amodi->name == amoda->name) {
                return false;
            }
        }
    }
    return true;
}

void cSCNFile::Init() {
    ovlpath = wxT("");
    name = wxT("");
    imported = false;
    flexitextures.clear();
    models.clear();
    animatedmodels.clear();
    lods.clear();
    animations.clear();
    splines.clear();
    references.clear();
    sivsettings = cSIVSettings();

//    error = CSCNFILE_NO_ERROR;
}

bool cSCNFile::Load() {
    bool ret = true;
    Init();

    if (filename == wxT("")) {
        throw RCT3Exception(_("No scenery file name given"));
    }

    try {
        boost::shared_ptr<c3DLoader> obj = c3DLoader::LoadFile(filename.GetFullPath().c_str());
        if (obj) {
            try {
                filename = "";
                return FromModelFile(obj);
            } catch (E3DLoader& e) {
				if (!boost::get_error_info<wxe_file>(e))
					e << wxe_file(filename.GetFullPath());
                throw RCT3Exception(_("Model conversion error: ") + e.wxwhat(), e);
            }
        }
    } catch (E3DLoader& e) {
		if (!boost::get_error_info<wxe_file>(e))
			e << wxe_file(filename.GetFullPath());
        throw RCT3Exception(_("Model loader error: ") + e.wxwhat(), e);
    }

    if (filename.GetExt().Lower() == wxT("xml")) {
		try {
			cXmlDoc doc;
			if (doc.read(filename.GetFullPath(), wxString(), XML_PARSE_DTDLOAD)) {
				ret = LoadXML(doc);
				if (!ret)
					wxLogWarning(_("There were warnings or errors loading the xml file.\nIf they concern models, they will be shown to you when you open the respective settings."));
				return ret;
			} else {
				wxe_xml_error_infos einfos;
				int eline = transferXmlErrors(doc, einfos);
				/*
				int eline = 0;
				foreach(const cXmlStructuredError& se, doc.getStructuredErrors()) {
					einfos.push_back(makeXmlErrorInfo(se));
					if (!eline) 
						eline = se.getLine();
					//wxLogError(UTF8STRINGWRAP(it->message).Trim(true));
				}
				foreach(const string& ge, doc.getGenericErrors())
					einfos.push_back(wxString::FromUTF8(ge.c_str()));
					//wxLogError(UTF8STRINGWRAP((*it)));
				*/
				throw RCT3Exception(_("XML error(s) while loading file")) << wxe_file(filename.GetFullPath()) << wxe_xml_errors(einfos) << wxe_xml_node_line(eline);
			}
		} catch(WXException& e) {
			if (!boost::get_error_info<wxe_file>(e))
				e << wxe_file(filename.GetFullPath());
			throw;
		}
    }

    if (filename.GetExt().Lower() != wxT("scn")) {
        wxLogWarning(_("Unrecognized file extension. Assuming it's a SCN scenery file."));
    }

	try {
		FILE *f = fopen(filename.GetFullPath().mb_str(wxConvFile), "rb");
		if (!f) {
			throw RCT3Exception(_("Failed to open scenery file"));
		}

		unsigned long objlen;
		fread(&objlen, sizeof(objlen), 1, f);

		// Check for old file format and call legacy loading function
		if (objlen < ((unsigned long) -1)) {
			ret = LoadLegacy(objlen, f);
			if (!ret)
				wxLogWarning(_("There were warnings or errors loading the model files.\nThey will be shown to you when you open the respective model settings."));
			filename.SetExt(wxT("xml"));
			return ret;
		}

		// Read magic
		char scnmagic[8];
		fread(&scnmagic, 8, 1, f);
		if (strncmp(CSCNFILE_MAGIC, scnmagic, 7) != 0) {
			fclose(f);
			throw RCT3Exception(_("Invalid scenery file"));
		}

		// Read version
		fread(&version, sizeof(long), 1, f);
		if (version > VERSION_CSCNFILE) {
			fclose(f);
			throw RCT3Exception(_("Scenery file version not supported"));
		}

		// Read ovl name
		unsigned long count;
		fread(&count, sizeof(count), 1, f);
		if (count) {
			char *tmp = new char[count];
			fread(tmp, count, 1, f);
			wxFileName ovlname = wxString(tmp, wxConvLocal);
			if (strlen(tmp)) {
				if (!ovlname.IsAbsolute()) {
					ovlname.MakeAbsolute(filename.GetPath());
				}
			}
			delete[] tmp;
			ovlpath = ovlname.GetPathWithSep();
			name = ovlname.GetName();
		}
		wxLogVerbose(_("Loading %s from SCN file."), name.c_str());

		prefix = "";

		// Read sivsettings
		fread(&sivsettings.sivflags, sizeof(unsigned int), 1, f);
		fread(&sivsettings.sway, sizeof(float), 1, f);
		fread(&sivsettings.brightness, sizeof(float), 1, f);
		fread(&sivsettings.unknown, sizeof(float), 1, f);
		fread(&sivsettings.scale, sizeof(float), 1, f);
		fread(&sivsettings.unk6, sizeof(unsigned long), 1, f);
		fread(&sivsettings.unk7, sizeof(unsigned long), 1, f);
		fread(&sivsettings.unk8, sizeof(unsigned long), 1, f);
		fread(&sivsettings.unk9, sizeof(unsigned long), 1, f);
		fread(&sivsettings.unk10, sizeof(unsigned long), 1, f);
		fread(&sivsettings.unk11, sizeof(unsigned long), 1, f);

		LoadTextures(f);
		ret = LoadModels(f);
		if (!ret)
			wxLogWarning(_("There were warnings or errors loading the model files.\nThey will be shown to you when you open the respective model settings."));
		LoadLODs(f);
		LoadReferences(f);

		fclose(f);
		filename.SetExt(wxT("xml"));
		return ret;
	} catch (WXException& e) {
		e << wxe_file(filename.GetFullPath());
		throw;
	} catch (exception& e) {
		throw RCT3Exception(_("Unknown exception while loading file: ") + wxString(e.what(), wxConvLocal)) << wxe_file(filename.GetFullPath());
	}
}

void cSCNFile::LoadTextures(FILE *f) {
    char* tmp;
    unsigned long texcount = 0;
    fread(&texcount, sizeof(texcount), 1, f);

    for (unsigned long i = 0; i < texcount; i++) {
        cFlexiTexture ft;
        unsigned long namelen = 0;
        fread(&namelen, sizeof(namelen), 1, f);
        if (namelen) {
            tmp = new char[namelen];
            fread(tmp, namelen, 1, f);
            ft.Name = wxString(tmp, wxConvLocal);
            delete[] tmp;
        }
        wxLogVerbose(_("  Loading texture %s from SCN file."), ft.Name.c_str());

        fread(&ft.Recolorable, sizeof(ft.Recolorable), 1, f);
        fread(&ft.FPS, sizeof(ft.FPS), 1, f);

        unsigned long anim_count = 0;
        fread(&anim_count, sizeof(anim_count), 1, f);
        unsigned long old_anim = 0;
        unsigned long x = 0;
        for(unsigned long i = 0; i < anim_count; i++) {
            unsigned long l;
            fread(&l, sizeof(unsigned long), 1, f);
            if (i && (l == old_anim)) {
                ft.Animation[x].count(ft.Animation[x].count()+1);
            } else {
                cFlexiTextureAnim an(l);
                ft.Animation.push_back(an);
                old_anim = l;
                if (i)
                    x++;
            }
        }

        unsigned long frame_count = 0;
        fread(&frame_count, sizeof(frame_count), 1, f);
        for(unsigned long i = 0; i < frame_count; i++) {
            cFlexiTextureFrame ftfr;

            ftfr.recolorable_read(f);

            unsigned long texturelen = 0;
            fread(&texturelen, sizeof(texturelen), 1, f);
            if (texturelen) {
                tmp = new char[texturelen];
                fread(tmp, texturelen, 1, f);
                ftfr.texture(wxString(tmp, wxConvLocal));
                if (strlen(tmp)) {
                    if (!ftfr.texture().IsAbsolute()) {
                        ftfr.texture_nc().MakeAbsolute(filename.GetPath());
                    }
                    wxLogVerbose(_("    Frame: %s"), ftfr.texture().GetFullPath().c_str());
                }
                delete[] tmp;
            }

            unsigned long alphalen = 0;
            fread(&alphalen, sizeof(alphalen), 1, f);
            if (alphalen) {
                tmp = new char[alphalen];
                fread(tmp, alphalen, 1, f);
                ftfr.alpha(wxString(tmp, wxConvFile));
                if (strlen(tmp)) {
                    if (!ftfr.alpha().IsAbsolute()) {
                        ftfr.alpha_nc().MakeAbsolute(filename.GetPath());
                    }
                    wxLogVerbose(_("      Alpha: %s"), ftfr.alpha().GetFullPath().c_str());
                }
                delete[] tmp;
            }
            if (version >= 5) {
                ftfr.alphacutoff_read(f);
                ftfr.alphasource_read(f);
            } else {
                if (ftfr.alpha() != wxT(""))
                    ftfr.alphasource(CFTF_ALPHA_EXTERNAL);
                else
                    ftfr.alphasource(CFTF_ALPHA_NONE);
            }
            ft.Frames.push_back(ftfr);
        }
        flexitextures.push_back(ft);
    }
}

bool cSCNFile::LoadModels(FILE *f) {
    bool model_ok = true;
    unsigned long count;
    char* tmp;

    unsigned long modcount = 0;
    fread(&modcount, sizeof(modcount), 1, f);

    for (unsigned long m = 0; m < modcount; m++) {
        cModel mod;

        // Read model name
        count = 0;
        fread(&count, sizeof(count), 1, f);
        if (count) {
            tmp = new char[count];
            fread(tmp, count, 1, f);
            mod.name = wxString(tmp, wxConvLocal);
            delete[] tmp;
        }
        wxLogVerbose(_("  Loading model %s from SCN file."), mod.name.c_str());

        // Read model filename
        count = 0;
        fread(&count, sizeof(count), 1, f);
        if (count) {
            tmp = new char[count];
            fread(tmp, count, 1, f);
            mod.file = wxString(tmp, wxConvFile);
            if (strlen(tmp)) {
                if (!mod.file.IsAbsolute()) {
                    mod.file.MakeAbsolute(filename.GetPath());
                }
                wxLogVerbose(_("    Model file: %s"), mod.file.GetFullPath().c_str());
            }
            delete[] tmp;
        }

        // Read transformations
        unsigned long matrices = 0;
        fread(&matrices, sizeof(matrices), 1, f);
        for (unsigned long j = 0; j < matrices; j++) {
            // Name
            count = 0;
            fread(&count, sizeof(count), 1, f);
            if (count) {
                tmp = new char[count];
                fread(tmp, count, 1, f);
                mod.transformnames.push_back(wxString(tmp, wxConvLocal));
                delete[] tmp;
            } else {
                mod.transformnames.push_back(_("<empty name>"));
            }

            // Matrix
            MATRIX m;

            fread(&m._11, sizeof(m._11), 1, f);
            fread(&m._12, sizeof(m._12), 1, f);
            fread(&m._13, sizeof(m._13), 1, f);
            fread(&m._14, sizeof(m._14), 1, f);
            fread(&m._21, sizeof(m._21), 1, f);
            fread(&m._22, sizeof(m._22), 1, f);
            fread(&m._23, sizeof(m._23), 1, f);
            fread(&m._24, sizeof(m._24), 1, f);
            fread(&m._31, sizeof(m._31), 1, f);
            fread(&m._32, sizeof(m._32), 1, f);
            fread(&m._33, sizeof(m._33), 1, f);
            fread(&m._34, sizeof(m._34), 1, f);
            fread(&m._41, sizeof(m._41), 1, f);
            fread(&m._42, sizeof(m._42), 1, f);
            fread(&m._43, sizeof(m._43), 1, f);
            fread(&m._44, sizeof(m._44), 1, f);
            mod.transforms.push_back(m);
        }

        // Read Meshes
        unsigned long meshcount = 0;
        fread(&meshcount, sizeof(meshcount), 1, f);
        for (unsigned long i = 0; i < meshcount; i++) {
            cMeshStruct ms;

            // Name
            count = 0;
            fread(&count, sizeof(count), 1, f);
            if (count) {
                tmp = new char[count];
                fread(tmp, count, 1, f);
                ms.Name = wxString(tmp, wxConvLocal);
                delete[] tmp;
            }

            // Enabled or disabled
            unsigned long dis;
            fread(&dis, sizeof(unsigned long), 1, f);
            ms.disabled = dis;
            wxLogVerbose(_("    Mesh: %s, %s"), ms.Name.c_str(), dis?wxT("disabled"):wxT("enabled"));

            fread(&ms.flags, sizeof(ms.flags), 1, f);
            fread(&ms.place, sizeof(ms.place), 1, f);
            fread(&ms.unknown, sizeof(ms.unknown), 1, f);

            count = 0;
            fread(&count, sizeof(count), 1, f);
            if (count) {
                tmp = new char[count];
                fread(tmp, count, 1, f);
                ms.FTX = wxString(tmp, wxConvLocal);
                delete[] tmp;
            }

            count = 0;
            fread(&count, sizeof(count), 1, f);
            if (count) {
                tmp = new char[count];
                fread(tmp, count, 1, f);
                ms.TXS = wxString(tmp, wxConvLocal);
                delete[] tmp;
            }

            mod.meshstructs.push_back(ms);
        }

        // Read effect point structures
        unsigned long ptcount = 0;
        fread(&ptcount, sizeof(ptcount), 1, f);
        for (unsigned long i = 0; i < ptcount; i++) {
            cEffectPoint e;

            // Name
            count = 0;
            fread(&count, sizeof(count), 1, f);
            if (count) {
                tmp = new char[count];
                fread(tmp, count, 1, f);
                e.name = wxString(tmp, wxConvLocal);
                delete[] tmp;
            }

            // Matrices
            matrices = 0;
            fread(&matrices, sizeof(matrices), 1, f);
            for (unsigned long j = 0; j < matrices; j++) {
                MATRIX m;

                // Name
                count = 0;
                fread(&count, sizeof(count), 1, f);
                if (count) {
                    tmp = new char[count];
                    fread(tmp, count, 1, f);
                    e.transformnames.push_back(wxString(tmp, wxConvLocal));
                    delete[] tmp;
                } else {
                    e.transformnames.push_back(_("<empty name>"));
                }

                // Matrix
                fread(&m._11, sizeof(m._11), 1, f);
                fread(&m._12, sizeof(m._12), 1, f);
                fread(&m._13, sizeof(m._13), 1, f);
                fread(&m._14, sizeof(m._14), 1, f);
                fread(&m._21, sizeof(m._21), 1, f);
                fread(&m._22, sizeof(m._22), 1, f);
                fread(&m._23, sizeof(m._23), 1, f);
                fread(&m._24, sizeof(m._24), 1, f);
                fread(&m._31, sizeof(m._31), 1, f);
                fread(&m._32, sizeof(m._32), 1, f);
                fread(&m._33, sizeof(m._33), 1, f);
                fread(&m._34, sizeof(m._34), 1, f);
                fread(&m._41, sizeof(m._41), 1, f);
                fread(&m._42, sizeof(m._42), 1, f);
                fread(&m._43, sizeof(m._43), 1, f);
                fread(&m._44, sizeof(m._44), 1, f);

                e.transforms.push_back(m);
            }
            mod.effectpoints.push_back(e);
        }
        if (!mod.Sync())
            model_ok = false;
        models.push_back(mod);
    }
    return model_ok;
}

void cSCNFile::LoadLODs(FILE *f) {
    unsigned long count;
    char* tmp;

    unsigned long lodcount = 0;
    fread(&lodcount, sizeof(lodcount), 1, f);
    for (unsigned long m = 0; m < lodcount; m++) {
        cLOD lod;

        // Write model name
        count = 0;
        fread(&count, sizeof(count), 1, f);
        if (count) {
            tmp = new char[count];
            fread(tmp, count, 1, f);
            lod.modelname = wxString(tmp, wxConvLocal);
            delete[] tmp;
        }

        fread(&lod.distance, sizeof(lod.distance), 1, f);
        fread(&lod.unk2, sizeof(lod.unk2), 1, f);
        fread(&lod.unk4, sizeof(lod.unk4), 1, f);
        fread(&lod.unk14, sizeof(lod.unk14), 1, f);
        wxLogVerbose(_("Loaded LOD %s, %.1f from SCN file."), lod.modelname.c_str(), lod.distance);
        lods.push_back(lod);
    }
}

void cSCNFile::LoadReferences(FILE *f) {
    unsigned long count;
    char* tmp;

    unsigned long refcount = references.size();
    fread(&refcount, sizeof(refcount), 1, f);
    for (unsigned long i = 0; i < refcount; i++) {
        count = 0;
        fread(&count, sizeof(count), 1, f);
        if (count) {
            tmp = new char[count];
            fread(tmp, count, 1, f);
            wxString ref = wxString(tmp, wxConvLocal);
            wxLogVerbose(_("  Loading reference '%s' from SCN file."), ref.c_str());
            references.push_back(ref);
            delete[] tmp;
        }
    }
}


bool cSCNFile::LoadLegacy(unsigned long objlen, FILE *f) {
    char *tmp;
    unsigned long i;

    models.resize(1);

    tmp = new char[MAX_PATH];
    if (objlen)
        fread(tmp, objlen, 1, f);

    if (objlen) {
        models[0].file = wxString(tmp, wxConvFile);
        wxFileName t = models[0].file;
        models[0].name = t.GetName();
    }
    delete[] tmp;

    unsigned long meshcount;
    fread(&meshcount, sizeof(meshcount), 1, f);
    models[0].meshstructs.clear();
    for (i = 0; i < meshcount; i++) {
        cMeshStruct ms;
        unsigned long dis;
        fread(&dis, sizeof(dis), 1, f);
        ms.disabled = dis;
        if (ms.disabled == false) {
            fread(&ms.flags, sizeof(ms.flags), 1, f);
            fread(&ms.place, sizeof(ms.place), 1, f);
            fread(&ms.unknown, sizeof(ms.unknown), 1, f);

            unsigned long ftxlen;
            fread(&ftxlen, sizeof(ftxlen), 1, f);
            if (ftxlen) {
                tmp = new char[ftxlen];
                fread(tmp, ftxlen, 1, f);
                ms.FTX = wxString(tmp, wxConvLocal);
                delete[] tmp;
            } else
                ms.FTX = wxT("");

            unsigned long txslen;
            fread(&txslen, sizeof(txslen), 1, f);
            if (txslen) {
                tmp = new char[txslen];
                fread(tmp, txslen, 1, f);
                ms.TXS = wxString(tmp, wxConvLocal);
                delete[] tmp;
            } else
                ms.TXS = wxT("");
        }
//        ms.effectpoint = false;
        models[0].meshstructs.push_back(ms);
    }

    unsigned long texcount;
    fread(&texcount, sizeof(texcount), 1, f);
    for (i = 0; i < texcount; i++) {
        cFlexiTexture ft;
        ft.FPS = 0;
        fread(&ft.Recolorable, sizeof(ft.Recolorable), 1, f);

        unsigned long namelen;
        fread(&namelen, sizeof(namelen), 1, f);
        if (namelen) {
            tmp = new char[namelen];
            fread(tmp, namelen, 1, f);
            ft.Name = wxString(tmp, wxConvLocal);
            delete[] tmp;
        } else
            ft.Name = wxT("");

        cFlexiTextureFrame ftfr;
        ftfr.recolorable(ft.Recolorable);
        unsigned long texturelen;
        fread(&texturelen, sizeof(texturelen), 1, f);
        if (texturelen) {
            tmp = new char[texturelen];
            fread(tmp, texturelen, 1, f);
            ftfr.texture(wxString(tmp, wxConvFile));
            delete[] tmp;
        } else
            ftfr.texture(wxString(wxT("")));

        unsigned long alphalen;
        fread(&alphalen, sizeof(alphalen), 1, f);
        if (alphalen) {
            tmp = new char[alphalen];
            fread(tmp, alphalen, 1, f);
            ftfr.alpha(wxString(tmp, wxConvFile));
            delete[] tmp;
        } else
            ftfr.alpha(wxString(wxT("")));

        if (ftfr.alpha() != wxT(""))
            ftfr.alphasource(CFTF_ALPHA_EXTERNAL);
        else
            ftfr.alphasource(CFTF_ALPHA_NONE);

        if ((ftfr.texture() != wxT("")) || (ftfr.alpha() != wxT("")))
            ft.Frames.push_back(ftfr);

        flexitextures.push_back(ft);
    }

    unsigned long refcount;
    fread(&refcount, sizeof(refcount), 1, f);
    for (i = 0; i < refcount; i++) {
        unsigned long reflen;
        fread(&reflen, sizeof(reflen), 1, f);
        if (reflen) {
            wxString ref;
            tmp = new char[reflen];
            fread(tmp, reflen, 1, f);
            ref = wxString(tmp, wxConvFile);
            delete[] tmp;
            references.push_back(ref);
        }
    }

    models[0].effectpoints.clear();
    unsigned long ptcount;
    fread(&ptcount, sizeof(ptcount), 1, f);
    for (i = 0; i < ptcount; i++) {
        cEffectPoint e;

        unsigned long namelen;
        fread(&namelen, sizeof(namelen), 1, f);
        if (namelen) {
            tmp = new char[namelen];
            fread(tmp, namelen, 1, f);
            e.name = wxString(tmp, wxConvLocal);
            delete[] tmp;
        } else
            e.name = wxT("");

        MATRIX m;
        fread(&m._11, sizeof(m._11), 1, f);
        fread(&m._12, sizeof(m._12), 1, f);
        fread(&m._13, sizeof(m._13), 1, f);
        fread(&m._14, sizeof(m._14), 1, f);
        fread(&m._21, sizeof(m._21), 1, f);
        fread(&m._22, sizeof(m._22), 1, f);
        fread(&m._23, sizeof(m._23), 1, f);
        fread(&m._24, sizeof(m._24), 1, f);
        fread(&m._31, sizeof(m._31), 1, f);
        fread(&m._32, sizeof(m._32), 1, f);
        fread(&m._33, sizeof(m._33), 1, f);
        fread(&m._34, sizeof(m._34), 1, f);
        fread(&m._41, sizeof(m._41), 1, f);
        fread(&m._42, sizeof(m._42), 1, f);
        fread(&m._43, sizeof(m._43), 1, f);
        fread(&m._44, sizeof(m._44), 1, f);
        e.transforms.push_back(m);
        e.transformnames.push_back(_("Custom Matrix (old scn)"));

        models[0].effectpoints.push_back(e);
    }

//    tmpmeshlist.clear();
    models[0].transforms.clear();
    models[0].transformnames.clear();

    if (!feof(f)) {
        version = 0;
        fread(&version, sizeof(long), 1, f);

        if (version >= 1) {
            long fixorient = 0;
            fread(&fixorient, sizeof(long), 1, f);
            if ((version <=2) && fixorient) {
                models[0].transforms.push_back(matrixGetFixOrientation());
                models[0].transformnames.push_back(_("Fix Orientation (old scn)"));
            }
        }
        // SCN Version 2 Stuff
        if (version >= 2) {
            fread(&sivsettings.sivflags, sizeof(unsigned int), 1, f);
            fread(&sivsettings.sway, sizeof(float), 1, f);
            fread(&sivsettings.brightness, sizeof(float), 1, f);
            fread(&sivsettings.unknown, sizeof(float), 1, f);
            fread(&sivsettings.scale, sizeof(float), 1, f);
        }
        // SCN Version 3 Stuff
        if (version >= 3) {
            // Read Mesh names
            for (i = 0; i < meshcount; i++) {
                wxString mname;

                unsigned long namelen;
                fread(&namelen, sizeof(namelen), 1, f);
                if (namelen) {
                    tmp = new char[namelen];
                    fread(tmp, namelen, 1, f);
                    mname = wxString(tmp, wxConvLocal);
                    delete[] tmp;
                } else
                    mname = wxT("");
                //tmpmeshlist.push_back(mname);
                models[0].meshstructs[i].Name = mname;
            }

            // Read transformations
            unsigned long matrices;
            fread(&matrices, sizeof(matrices), 1, f);
            for (unsigned long j = 0; j < matrices; j++) {
                MATRIX m;
                wxString nam;

                unsigned long namelen;
                fread(&namelen, sizeof(namelen), 1, f);
                if (namelen) {
                    tmp = new char[namelen];
                    fread(tmp, namelen, 1, f);
                    nam = wxString(tmp, wxConvLocal);
                    delete[] tmp;
                } else
                    nam = wxT("");
                models[0].transformnames.push_back(nam);

                fread(&m._11, sizeof(m._11), 1, f);
                fread(&m._12, sizeof(m._12), 1, f);
                fread(&m._13, sizeof(m._13), 1, f);
                fread(&m._14, sizeof(m._14), 1, f);
                fread(&m._21, sizeof(m._21), 1, f);
                fread(&m._22, sizeof(m._22), 1, f);
                fread(&m._23, sizeof(m._23), 1, f);
                fread(&m._24, sizeof(m._24), 1, f);
                fread(&m._31, sizeof(m._31), 1, f);
                fread(&m._32, sizeof(m._32), 1, f);
                fread(&m._33, sizeof(m._33), 1, f);
                fread(&m._34, sizeof(m._34), 1, f);
                fread(&m._41, sizeof(m._41), 1, f);
                fread(&m._42, sizeof(m._42), 1, f);
                fread(&m._43, sizeof(m._43), 1, f);
                fread(&m._44, sizeof(m._44), 1, f);
                models[0].transforms.push_back(m);
            }

            // Read new effect point structures
            fread(&ptcount, sizeof(ptcount), 1, f);
            for (i = 0; i < ptcount; i++) {
                cEffectPoint e;

                unsigned long namelen;
                fread(&namelen, sizeof(namelen), 1, f);
                if (namelen) {
                    tmp = new char[namelen];
                    fread(tmp, namelen, 1, f);
                    e.name = wxString(tmp, wxConvLocal);
                    delete[] tmp;
                } else
                    e.name = wxT("");

                fread(&matrices, sizeof(matrices), 1, f);
                for (unsigned long j = 0; j < matrices; j++) {
                    MATRIX m;
                    wxString nam;

                    fread(&namelen, sizeof(namelen), 1, f);
                    if (namelen) {
                        tmp = new char[namelen];
                        fread(tmp, namelen, 1, f);
                        nam = wxString(tmp, wxConvLocal);
                        delete[] tmp;
                    } else
                        nam = wxT("");
                    e.transformnames.push_back(nam);

                    fread(&m._11, sizeof(m._11), 1, f);
                    fread(&m._12, sizeof(m._12), 1, f);
                    fread(&m._13, sizeof(m._13), 1, f);
                    fread(&m._14, sizeof(m._14), 1, f);
                    fread(&m._21, sizeof(m._21), 1, f);
                    fread(&m._22, sizeof(m._22), 1, f);
                    fread(&m._23, sizeof(m._23), 1, f);
                    fread(&m._24, sizeof(m._24), 1, f);
                    fread(&m._31, sizeof(m._31), 1, f);
                    fread(&m._32, sizeof(m._32), 1, f);
                    fread(&m._33, sizeof(m._33), 1, f);
                    fread(&m._34, sizeof(m._34), 1, f);
                    fread(&m._41, sizeof(m._41), 1, f);
                    fread(&m._42, sizeof(m._42), 1, f);
                    fread(&m._43, sizeof(m._43), 1, f);
                    fread(&m._44, sizeof(m._44), 1, f);
                    e.transforms.push_back(m);
                }

                models[0].effectpoints.push_back(e);
            }
        }
    }
    fclose(f);

    if (models[0].file == wxT("")) {
        models.clear();
        lods.clear();
        return true;
    }

    cLOD lod;
    lod.modelname = models[0].name;
    lod.distance = 40.0;
    lods.push_back(lod);
    lod.distance = 100.0;
    lods.push_back(lod);
    lod.distance = 4000.0;
    lods.push_back(lod);

    return models[0].Sync();
    //return LoadObject(false);
}

bool cSCNFile::Save() {

    if (filename == wxT("")) {
        throw RCT3Exception(_("Saving failed, file name not set"));
    }
    SaveXML();
    imported = false;
    return true;
}

void cSCNFile::SaveXML() {
    wxString xmlfile = filename.GetFullPath();
    //xmlfile += wxT(".xml");
    cXmlDoc doc(true);
    //doc.SetFileEncoding(wxT("UTF-8"));
    cXmlNode root = GetNode(filename.GetPath());
    doc.root(root);
    doc.write(xmlfile, true, wxString(wxT("UTF-8")));
}

bool cSCNFile::LoadXML(cXmlDoc& doc) {
    // start processing the XML file
    cXmlNode root = doc.root();
    if (!root(RCT3XML_CSCNFILE)) {
        throw RCT3Exception(wxString::Format(_("Error loading xml file '%s'. Wrong root element '%s'. Probably you tried to load a xml file made for a different purpose"), filename.GetFullPath().c_str(), STRING_FOR_FORMAT(root.name()))) << wxe_xml_node_line(root.line());
    }

    if (READ_RCT3_VALIDATE()) {
        cXmlInputOutputCallbackString::Init();
        XMLCPP_RES_ADD_ONCE(rct3xml_ovlcompiler_v1, rnc);
        XMLCPP_RES_ADD_ONCE(rct3xml_scenery_v1, rnc);
    }

    wxString path = filename.GetPath();

    // Peak at version to detect ovlcompiler xml
    if (root.ns() == XML_NAMESPACE_SCENERY) {
        if (READ_RCT3_VALIDATE()) {
            if (READ_RCT3_REPORTVALIDATION()) {
                wxLogMessage(_("Validating..."));
            }
            cXmlValidatorRNVRelaxNG val(XMLCPP_RES_USE(rct3xml_scenery_v1, rnc).c_str());
            if (!val) {
                //wxString error(_("Internal Error: could not load scenery schema:\n"));
                //error += val.wxgetErrorList();
				wxe_xml_error_infos einfos;
				int eline = transferXmlErrors(val, einfos);
                throw RCT3Exception(_("Internal Error: could not load scenery schema")) << wxe_xml_errors(einfos) << wxe_xml_node_line(eline);
            }
            if (doc.validate(val)) {
                //wxString error(_("Invalid scenery xml file:\n"));
                //error += val.wxgetErrorList();
				wxe_xml_error_infos einfos;
				int eline = transferXmlErrors(val, einfos);
                throw RCT3Exception(_("Invalid scenery xml file")) << wxe_xml_errors(einfos) << wxe_xml_node_line(eline);
            }
            if (READ_RCT3_REPORTVALIDATION()) {
                wxLogMessage(_("...Ok!"));
            }
        }
        return FromNode(root, path, 0);
    } else if (root.ns() == XML_NAMESPACE_COMPILER) {
        if (READ_RCT3_VALIDATE()) {
            if (READ_RCT3_REPORTVALIDATION()) {
                wxLogMessage(_("Validating..."));
            }
            cXmlValidatorRNVRelaxNG val(XMLCPP_RES_USE(rct3xml_ovlcompiler_v1, rnc).c_str());
            if (!val) {
                //wxString error(_("Internal Error: could not load ovlcompiler schema:\n"));
                //error += val.wxgetErrorList();
				wxe_xml_error_infos einfos;
				int eline = transferXmlErrors(val, einfos);
                throw RCT3Exception(_("Internal Error: could not load ovlcompiler schema")) << wxe_xml_errors(einfos) << wxe_xml_node_line(eline);
            }
            if (doc.validate(val)) {
                //wxString error(_("Invalid ovlcompiler xml file:\n"));
                //error += val.wxgetErrorList();
				wxe_xml_error_infos einfos;
				int eline = transferXmlErrors(val, einfos);
                throw RCT3Exception(_("Invalid ovlcompiler xml file")) << wxe_xml_errors(einfos) << wxe_xml_node_line(eline);
            }
            if (READ_RCT3_REPORTVALIDATION()) {
                wxLogMessage(_("...Ok!"));
            }
        }

        version = VERSION_CSCNFILE_FIRSTXML;
        name = filename.GetName();
        return FromCompilerXml(root, path);
    } else {
        throw RCT3Exception(wxString::Format(_("Error loading xml file '%s'. Root element has no or unsupported namespace. Probably you forgot or mistyped the namespace declaration"), filename.GetFullPath().c_str())) << wxe_xml_node_line(root.line());
    }

}

#define COMPILER_BSH "bsh"
#define COMPILER_BAN "ban"
#define COMPILER_FTX "ftx"
#define COMPILER_FIX "fix"
#define COMPILER_REFERENCE "reference"
#define COMPILER_OPTIONS "options"

bool cSCNFile::FromCompilerXml(cXmlNode& node, const wxString& path) {
    cXmlNode child = node.children();
    c3DLoaderOrientation ori = ORIENTATION_UNKNOWN;
    bool ret = true;
    int option_lods = 1;

    name = node.wxgetPropVal("name", name);
    prefix = node.wxgetPropVal("prefix", prefix);

    // Read ovl file
    ovlpath = node.wxgetPropVal("file", ".\\");
    if (!ovlpath.IsAbsolute()) {
        ovlpath.MakeAbsolute(path);
    }

    while (child) {

        if (child(COMPILER_BSH)) {
            cAnimatedModel nmodel;
            if (!nmodel.FromCompilerXml(child, path))
                ret = false;
            animatedmodels.push_back(nmodel);
        } else if (child(COMPILER_BAN)) {
            cAnimation anim;
            if (!anim.FromCompilerXml(child, path))
                ret = false;
            animations.push_back(anim);
        } else if (child(COMPILER_FTX)) {
            cFlexiTexture ftx;
            if (!ftx.FromCompilerXml(child, path))
                ret = false;
            flexitextures.push_back(ftx);
        } else if (child(COMPILER_FIX)) {
            if (ori != ORIENTATION_UNKNOWN) {
                throw RCT3Exception(_("Second fix tag found")) << wxe_xml_node_line(child.line());
            }
            wxString hand = child.wxgetPropVal("handedness", "left");
            hand.MakeLower();
            wxString up = child.wxgetPropVal("up", "y");
            up.MakeLower();
            if (hand == wxT("left")) {
                if (up == wxT("x")) {
                    ori = ORIENTATION_LEFT_XUP;
                } else if (up == wxT("y")) {
                    ori = ORIENTATION_LEFT_YUP;
                } else if (up == wxT("z")) {
                    ori = ORIENTATION_LEFT_ZUP;
                } else {
                    throw RCT3Exception(wxString::Format(_("Unknown value '%s' for up attribute in fix tag"), up.c_str())) << wxe_xml_node_line(child.line());
                }
            } else if (hand == wxT("right")) {
                if (up == wxT("x")) {
                    ori = ORIENTATION_RIGHT_XUP;
                } else if (up == wxT("y")) {
                    ori = ORIENTATION_RIGHT_YUP;
                } else if (up == wxT("z")) {
                    ori = ORIENTATION_RIGHT_ZUP;
                } else {
                    throw RCT3Exception(wxString::Format(_("Unknown value '%s' for up attribute in fix tag"), up.c_str())) << wxe_xml_node_line(child.line());
                }
            } else {
                throw RCT3Exception(wxString::Format(_("Unknown value '%s' for handedness attribute in fix tag"), hand.c_str())) << wxe_xml_node_line(child.line());
            }
            wxLogDebug(wxT("Fix tag found: %s %s (%d)"), hand.c_str(), up.c_str(), static_cast<unsigned int>(ori));
        } else if (child(COMPILER_REFERENCE)) {
            wxString ref = child.wxcontent();
            if (ref.IsEmpty())
                throw RCT3Exception(_("REFERENCE tag misses content"));
            references.push_back(ref);
        } else if (child(COMPILER_OPTIONS)) {
            wxString te = child.wxgetPropVal("lods", "1");
            if (te == wxT("1")) {
                option_lods = 1;
            } else if (te == wxT("3")) {
                option_lods = 3;
            } else if (te == wxT("4")) {
                option_lods = 4;
            } else {
                throw RCT3Exception(wxString::Format(_("Unknown lod option '%s'"), te.c_str())) << wxe_xml_node_line(child.line());
            }
        } else if (child.element()) {
            throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in ovl tag"), STRING_FOR_FORMAT(child.name()))) << wxe_xml_node_line(child.line());
        }

        ++child;
    }

    if (ori == ORIENTATION_UNKNOWN)
        ori = ORIENTATION_LEFT_YUP;

    for (cAnimatedModel::iterator it = animatedmodels.begin(); it != animatedmodels.end(); ++it)
        it->usedorientation = ori;
    for (cAnimation::iterator it = animations.begin(); it != animations.end(); ++it)
        it->usedorientation = ori;

    if (animatedmodels.size()) {
        cLOD lod;
        lod.animated = true;
        lod.modelname = animatedmodels[0].name;
        foreach(const cAnimation& an, animations)
            lod.animations.push_back(an.name);
        switch (option_lods) {
            case 1: {
                    lod.distance = 4000;
                    lods.push_back(lod);
                }
                break;
            case 3: {
                    lod.distance = 40;
                    lods.push_back(lod);
                    if (animatedmodels.size() == 3) {
                        lod.modelname = animatedmodels[1].name;
                        lod.distance = 100;
                        lods.push_back(lod);
                        lod.modelname = animatedmodels[2].name;
                        lod.distance = 4000;
                        lods.push_back(lod);
                    } else {
                        cAnimatedModel m = animatedmodels[0];
                        m.name += wxT("MLOD");
                        animatedmodels.push_back(m);
                        lod.modelname = m.name;
                        lod.distance = 100;
                        lods.push_back(lod);

                        m = animatedmodels[0];
                        m.name += wxT("LLOD");
                        animatedmodels.push_back(m);
                        lod.modelname = m.name;
                        lod.distance = 4000;
                        lods.push_back(lod);
                    }
                }
                break;
            case 4: {
                    lod.distance = 40;
                    lods.push_back(lod);
                    if (animatedmodels.size() >= 3) {
                        lod.modelname = animatedmodels[1].name;
                        lod.distance = 100;
                        lods.push_back(lod);
                        lod.modelname = animatedmodels[2].name;
                        lod.distance = 300;
                        lods.push_back(lod);
                        cModel sm;
                        if (animatedmodels.size() == 3) {
                            sm = cModel(animatedmodels[2]);
                            sm.name += wxT("ULLOD");
                        } else {
                            sm = cModel(animatedmodels[3]);
                            animatedmodels.erase(animatedmodels.begin()+3);
                        }
                        sm.effectpoints.clear();
                        models.push_back(sm);
                        lod.modelname = sm.name;
                        lod.distance = 4000;
                        lod.animated = false;
                        lods.push_back(lod);
                    } else {
                        cAnimatedModel m = animatedmodels[0];
                        m.name += wxT("MLOD");
                        animatedmodels.push_back(m);
                        lod.modelname = m.name;
                        lod.distance = 100;
                        lods.push_back(lod);

                        m = animatedmodels[0];
                        m.name += wxT("LLOD");
                        animatedmodels.push_back(m);
                        lod.modelname = m.name;
                        lod.distance = 300;
                        lods.push_back(lod);

                        cModel sm = cModel(animatedmodels[0]);
                        sm.name += wxT("ULLOD");
                        sm.effectpoints.clear();
                        models.push_back(sm);
                        lod.modelname = sm.name;
                        lod.distance = 4000;
                        lod.animated = false;
                        lods.push_back(lod);
                    }
                }
                break;
        }
//    } else {
//        throw RCT3Exception(_("No bsh tags found"));
    }

    filename = wxT(""); // Destroy filename to prevent overwriting

    return ret;
}

void insertBoneParent(boost::shared_ptr<c3DLoader>& model, const wxString& bone, set<wxString>& usedbones) {
    c3DBone& b = model->getBone(bone);
    if (!b.m_parent.IsEmpty()) {
        usedbones.insert(b.m_parent);
        insertBoneParent(model, b.m_parent, usedbones);
    }
}

/** @brief FromModelFile
  *
  * @todo: document this function
  */
bool cSCNFile::FromModelFile(boost::shared_ptr<c3DLoader>& model) {
    c3DLoaderOrientation ori = model->getOrientation();
    imported = true;
    {
        wxFileName t(model->getFilename());
        ovlpath = model->getSuggestedPath().IsEmpty()?wxFileName::DirName(t.GetPathWithSep()):model->getSuggestedPath();
        name = model->getName().IsEmpty()?t.GetName():model->getName();
    }
    prefix = model->getPrefix();

    sivsettings = cSIVSettings(model.get());

    // Textures
    foreach(const c3DTexture::pair& tex, model->getTextures()) {
        if (tex.second.m_referenced) {
            if (tex.second.m_file == "-")
                continue;
            if (find(references.begin(), references.end(), tex.second.m_file) == references.end()) {
                references.push_back(tex.second.m_file);
                continue;
            }
        }

        cFlexiTexture te;
        te.Name = tex.second.m_name;
        te.Recolorable = tex.second.m_recol;

        if (tex.second.m_frames.size()) {
            typedef pair<wxString, wxString> strpair;
            foreach(const strpair& p, tex.second.m_frames) {
                cFlexiTextureFrame fr;
                fr.texture(p.first);
                fr.alpha(p.second);
                fr.recolorable(tex.second.m_recol);
                fr.alphasource(tex.second.m_alphaType);

                te.Frames.push_back(fr);
            }
            if (tex.second.m_sequence.size()) {
                unsigned long old = tex.second.m_sequence[0] + 1;
                foreach(unsigned long i, tex.second.m_sequence) {
                    if (i != old) {
                        te.Animation.push_back(cFlexiTextureAnim(i));
                        old = i;
                    } else {
                        te.Animation[te.Animation.size()-1].count(te.Animation[te.Animation.size()-1].count() + 1);
                    }
                }
            }
        } else {
            cFlexiTextureFrame fr;
            fr.texture(tex.second.m_file);
            fr.alpha(tex.second.m_alphafile);
            fr.recolorable(tex.second.m_recol);
            fr.alphasource(tex.second.m_alphaType);

            te.Frames.push_back(fr);
        }

        flexitextures.push_back(te);
    }

    // Splines
    foreach(const c3DSpline::pair& spl, model->getSplines())
        splines.push_back(cImpSpline(spl.second, ori));

    // Animations
    foreach(const c3DAnimation::pair& ani, model->getAnimations()) {
        cAnimation cani;
        cani.name = ani.first;
        cani.usedorientation = ori;
        foreach(const c3DAnimBone::pair& anibone, ani.second.m_bones) {
            cBoneAnimation canibone;
            canibone.name = anibone.first;
            canibone.axis = anibone.second.m_axis;
            canibone.translations.resize(anibone.second.m_translations.size());
            canibone.rotations.resize(anibone.second.m_rotations.size());
            copy(anibone.second.m_translations.begin(), anibone.second.m_translations.end(), canibone.translations.begin());
            copy(anibone.second.m_rotations.begin(), anibone.second.m_rotations.end(), canibone.rotations.begin());
            cani.boneanimations.push_back(canibone);
        }
        animations.push_back(cani);
    }

    // Groups
    foreach(const c3DGroup::pair& gr, model->getGroups()) {
        // Find out wheter it is animated or not
        bool animated = gr.second.m_forceanim;
        if (!animated) {
            foreach(const wxString& bn, gr.second.m_bones) {
                if ((model->getBone(bn).m_type == "Bone") || (!model->getBone(bn).m_parent.IsEmpty())) {
                    animated = true;
                    break;
                }
            }
        }

        // Find out which bones we need
        set<wxString> usedbones = gr.second.m_bones;

        // Mesh bones
        foreach(const wxString& me, gr.second.m_meshes) {
            foreach(const wxString& bn, model->getObjectBones(me)) {
                usedbones.insert(bn);
            }
        }

        // Parents
        set<wxString> usedboneparents;
        foreach(const wxString& me, usedbones) {
            insertBoneParent(model, me, usedboneparents);
        }
        if (usedboneparents.size())
            usedbones.insert(usedboneparents.begin(), usedboneparents.end());

        cModel mod;
        cAnimatedModel amod;
        cModel& accmod = animated?amod:mod;

        accmod.name = gr.first;
        if (!accmod.Load(model->getFilename(), false)) {
			foreach(const wxString& err, accmod.errors)
				wxLogWarning(err);
            if (accmod.stored_exception) {
				throw RCT3Exception(accmod.stored_exception->wxwhat(), *accmod.stored_exception);
            }
        }
        accmod.usedorientation = ori;

        accmod.autoBones(&usedbones);

        // Activate and deactivate appropriate meshes, textures and options
        foreach(cMeshStruct& ms, accmod.meshstructs) {
            ms.disabled = (!ms.valid) || (gr.second.m_meshes.find(ms.Name) == gr.second.m_meshes.end());
            const c3DMesh& m = model->getObject(ms.Name);
            if (!m.m_texture.IsEmpty())
                ms.FTX = m.m_texture;
            if (!m.m_meshOptions.IsEmpty())
                ms.autoMeshStyle(m.m_meshOptions);
        }

        accmod.auto_bones = true;
        accmod.auto_sort = true;
        accmod.sortBones();

        if (animated) {
            animatedmodels.push_back(amod);
        } else {
            models.push_back(mod);
        }

        if (gr.second.m_loddistance >= 0.0) {
            cLOD lod;
            lod.modelname = gr.first;
            lod.distance = gr.second.m_loddistance;
            lod.animated = animated;
            if (animated)
                copy(gr.second.m_animations.begin(), gr.second.m_animations.end(), back_inserter(lod.animations));
            lods.push_back(lod);
        }

    }

    return true;
}



#define CSCNFILE_READVECTOR(vec, itype, itag, tag) \
    } else if (child(tag)) { \
        cXmlNode subchild = child.children(); \
        while (subchild) { \
            if (subchild(itag)) { \
                itype item; \
                if (!item.FromNode(subchild, path, version)) \
                    ret = false; \
                vec.push_back(item); \
            } \
            ++subchild; \
        }

bool cSCNFile::FromNode(cXmlNode& node, const wxString& path, unsigned long version) {
    bool ret = true;
    std::string temp;
    if (!node)
        return false;
    if (!node(RCT3XML_CSCNFILE))
        return false;

    name = node.wxgetPropVal("name");
    prefix = node.wxgetPropVal("prefix");

    // Read ovl file
    ovlpath = node.wxgetPropVal("file", ".\\");
    if (!ovlpath.IsAbsolute()) {
        ovlpath.MakeAbsolute(path);
    }

    // Read version
    if (node.getPropVal("version", &temp)) {
        if (!parseULong(temp, version)) {
            version = VERSION_CSCNFILE_FIRSTXML;
            ret = false;
        }
    } else {
        version = VERSION_CSCNFILE_FIRSTXML;
    }

    this->version = version;

    if (version > VERSION_CSCNFILE) {
        //error |= CSCNFILE_ERROR_VERSION_XML;
        wxLogWarning(_("The xml scenery file was created with a newer importer version.\nThere might be some errors, so check whether everything is alright."));
        ret = false;
    }


    cXmlNode child = node.children();
    while (child) {

        if (child(RCT3XML_CSIVSETTINGS)) {
            if (!sivsettings.FromNode(child, path, version))
                ret = false;
        CSCNFILE_READVECTOR(flexitextures, cFlexiTexture, RCT3XML_CFLEXITEXTURE, RCT3XML_CSCNFILE_FLEXITEXTURES)
        CSCNFILE_READVECTOR(models, cModel, RCT3XML_CMODEL, RCT3XML_CSCNFILE_MODELS)
        CSCNFILE_READVECTOR(animatedmodels, cAnimatedModel, RCT3XML_CANIMATEDMODEL, RCT3XML_CSCNFILE_ANIMATEDMODELS)
        CSCNFILE_READVECTOR(animations, cAnimation, RCT3XML_CANIMATION, RCT3XML_CSCNFILE_ANIMATIONS)
        CSCNFILE_READVECTOR(splines, cImpSpline, RCT3XML_CSPLINE, RCT3XML_CSCNFILE_SPLINES)
        CSCNFILE_READVECTOR(lods, cLOD, RCT3XML_CLOD, RCT3XML_CSCNFILE_LODS)
        CSCNFILE_READVECTOR(references, cReference, RCT3XML_REFERENCE, RCT3XML_CSCNFILE_REFERENCES)
/*
        } else if (child(RCT3XML_CSCNFILE_REFERENCES)) {
            cXmlNode subchild = child.children();
            while (subchild) {
                if (subchild(RCT3XML_REFERENCE)) {
                    wxString ref = subchild.wxgetPropVal("path");
                    if (!ref.IsEmpty())
                        references.push_back(ref);
                }
                ++subchild;
            }
*/
        }

        ++child;
    }
    return ret;
}

//    parent = new wxXmlNode(node, wxXML_ELEMENT_NODE, tag);
//        newchild->SetParent(parent);

#define CSCNFILE_WRITEVECTOR(vec, iter, tag) \
    tempnode = cXmlNode(tag); \
    for (iter it = vec.begin(); it != vec.end(); it++) { \
        cXmlNode newchild = it->GetNode(path); \
        tempnode.appendChildren(newchild); \
    } \
    node.appendChildren(tempnode);

cXmlNode cSCNFile::GetNode(const wxString& path) {
    cXmlNode node(RCT3XML_CSCNFILE);
    node.prop("name", name);
    if (!prefix.IsEmpty())
        node.prop("prefix", prefix);
    wxFileName temp = ovlpath;
    temp.MakeRelativeTo(path);
    node.prop("file", temp.GetPathWithSep());
    node.prop("version", boost::str(boost::format("%lu") % version).c_str());
    node.prop("xmlns", XML_NAMESPACE_SCENERY);

    cXmlNode tempnode = sivsettings.GetNode(path);
    node.appendChildren(tempnode);

    CSCNFILE_WRITEVECTOR(flexitextures, cFlexiTexture::iterator, RCT3XML_CSCNFILE_FLEXITEXTURES)
    CSCNFILE_WRITEVECTOR(models, cModel::iterator, RCT3XML_CSCNFILE_MODELS)
    CSCNFILE_WRITEVECTOR(animatedmodels, cAnimatedModel::iterator, RCT3XML_CSCNFILE_ANIMATEDMODELS)
    CSCNFILE_WRITEVECTOR(animations, cAnimation::iterator, RCT3XML_CSCNFILE_ANIMATIONS)
    CSCNFILE_WRITEVECTOR(splines, cImpSpline::iterator, RCT3XML_CSCNFILE_SPLINES)
    CSCNFILE_WRITEVECTOR(lods, cLOD::iterator, RCT3XML_CSCNFILE_LODS)
    CSCNFILE_WRITEVECTOR(references, cReference::iterator, RCT3XML_CSCNFILE_REFERENCES)

    //parent = new wxXmlNode(node, wxXML_ELEMENT_NODE, RCT3XML_CSCNFILE_REFERENCES);
    /*
    tempnode = cXmlNode(RCT3XML_CSCNFILE_REFERENCES);
    for (cStringIterator it = references.begin(); it != references.end(); it++) {
        //wxXmlNode* newchild = new wxXmlNode(parent, wxXML_ELEMENT_NODE, RCT3XML_REFERENCE);
        cXmlNode newchild(RCT3XML_REFERENCE);
        newchild.prop("path", *it);
        tempnode.appendChildren(newchild);
    }
    node.appendChildren(tempnode);
    */


    return node;
}

bool cSCNFile::Check() {
    wxLogDebug(wxT("Trace, cSCNFile::Check"));
	try {
		bool warning = false;
		CleanWork();

		m_work.reset(new cSCNFile(*this));

		if (!READ_RCT3_EXPERTMODE()) {
			if (name.IsEmpty()) {
				throw RCT3Exception(_("No name set!"));
			}
		}

		if (prefix.IsEmpty() && READ_RCT3_WARNPREFIX()) {
			wxLogWarning(_("You should set a prefix!"));
			warning = true;
		}
		
		if (!prefix.IsEmpty() && !boost::all(prefix, boost::is_any_of(ASCIIALNUM))) {
			wxLogWarning(_("Your prefix contains non-alphanumeric or non-AscII characters. This often works, but may lead to trouble if some exported names are not prefixed."));
			warning = true;
		}


		/////////////////////////////////////////////////////
		// Sanity Checks & fix the work copy for processing
		{
			wxSortedArrayString usedtextures;
			wxSortedArrayString presentbones;
			if ((!m_work->models.size()) && (!m_work->animatedmodels.size()) && (!m_work->lods.size()) && (!m_work->animations.size())) {
				m_textureovl = true;
			} else {
				// Sanity check for model related stuff

				// Check for LODs
				if (!m_work->lods.size()) {
					if (READ_RCT3_EXPERTMODE()) {
						if (!READ_RCT3_MOREEXPERTMODE()) {
							wxLogWarning(_("You didn't set any levels of detail."));
							warning = true;
						}
					} else {
						throw RCT3Exception(_("You didn't set any levels of detail!"));
					}
				} else if ((m_work->lods.size()==1) && (!READ_RCT3_EXPERTMODE())) {
					warning = true;
					wxLogWarning(_("You only set one LOD, this can lead to weird effects in RCT3. Please set up at least 3 LODs."));
				}

				// Check the models
				if ((!m_work->models.size()) && (!m_work->animatedmodels.size())) {
					if (READ_RCT3_EXPERTMODE()) {
						if (!READ_RCT3_MOREEXPERTMODE()) {
							wxLogWarning(_("You didn't add any models."));
							warning = true;
						}
					} else {
						throw RCT3Exception(_("You didn't add any models!"));
					}
				}

				// Check for duplicate names
				if (!CheckForModelNameDuplicates()) {
					throw RCT3Exception(_("You have a duplicate model name. Model names have to be unique, even between static and animated models"));
				}

				cModelMap modnames;
				for (cModel::iterator i_mod = m_work->models.begin(); i_mod != m_work->models.end(); ++i_mod) {
					// Note: at this stage we issue only warnings. Errors occur when a LOD references
					//   a broken model
					wxLogDebug(wxT("Trace, cSCNFile::Check model %s"), i_mod->name.c_str());
					i_mod->Check(modnames);
				}
				cAnimatedModelMap amodnames;
				for (cAnimatedModel::iterator i_mod = m_work->animatedmodels.begin(); i_mod != m_work->animatedmodels.end(); ++i_mod) {
					// Note: at this stage we issue only warnings. Errors occur when a LOD references
					//   a broken model
					wxLogDebug(wxT("Trace, cSCNFile::Check animated model %s"), i_mod->name.c_str());
					i_mod->Check(amodnames);
				}

				// Now check the LODs
				for (cLOD::iterator i_lod = m_work->lods.begin(); i_lod != m_work->lods.end(); i_lod++) {
					cModel *mod = modnames[i_lod->modelname];
					cAnimatedModel *amod = amodnames[i_lod->modelname];
					if (mod) {
						if (mod->stored_exception) {
							throw RCT3Exception(wxString::Format(_("Level of Detail '%s' (%.1f): Error in corresponding model"), i_lod->modelname.c_str(), i_lod->distance));
						}
						mod->used = true;
						if (i_lod->animated) {
							i_lod->animated = false;
							i_lod->animations.clear();
							warning = true;
							wxLogWarning(_("Level of Detail '%s' (%.1f): Static model was marked as animated."), i_lod->modelname.c_str(), i_lod->distance);
						}
					} else if (amod) {
						if (amod->stored_exception) {
							throw RCT3Exception(wxString::Format(_("Level of Detail '%s' (%.1f): Error in corresponding animated model"), i_lod->modelname.c_str(), i_lod->distance));
						}
						if (amod->used) {
							wxString mess(wxString::Format(_("Level of Detail '%s' (%.1f): Animated model was already used in a different LOD. This breaks the animation."), i_lod->modelname.c_str(), i_lod->distance));
							if (READ_RCT3_EXPERTMODE()) {
								warning = true;
								wxLogWarning(mess);
							} else {
								throw RCT3Exception(mess);
							}
						}
						amod->used = true;
						if (!i_lod->animated) {
							i_lod->animated = true;
							warning = true;
							wxLogWarning(_("Level of Detail '%s' (%.1f): Animated model was marked as static."), i_lod->modelname.c_str(), i_lod->distance);
						}
						for (int s = i_lod->animations.size()-1; s >= 0; --s) {
							bool a_found = false;
							for (cAnimation::iterator i_anim = m_work->animations.begin(); i_anim != m_work->animations.end(); ++i_anim) {
								if (i_anim->name == i_lod->animations[s]) {
									a_found = true;
									break;
								}
							}
							if (!a_found) {
								warning = true;
								if (READ_RCT3_EXPERTMODE()) {
									if (!READ_RCT3_MOREEXPERTMODE())
										wxLogWarning(_("Level of Detail '%s' (%.1f): Assigned animation '%s' missing."), i_lod->modelname.c_str(), i_lod->distance, i_lod->animations[s].c_str());
								} else {
									wxLogWarning(_("Level of Detail '%s' (%.1f): Assigned animation '%s' missing. Removed."), i_lod->modelname.c_str(), i_lod->distance, i_lod->animations[s].c_str());
									i_lod->animations.erase(i_lod->animations.begin()+s);
								}
							}
						}
						if (!i_lod->animations.size()) {
							warning = true;
							wxLogWarning(_("Level of Detail '%s' (%.1f): Animated model used but no animations assigned."), i_lod->modelname.c_str(), i_lod->distance);
						} else if (i_lod->animations.size()>1) {
							if (!READ_RCT3_EXPERTMODE()) {
								wxLogWarning(_("Level of Detail '%s' (%.1f): More than one animation assigned. A LOD of normal animated Scenery uses only one animation, more than one only apply to special cases."), i_lod->modelname.c_str(), i_lod->distance);
							}
						}
					} else {
						if (READ_RCT3_EXPERTMODE()) {
							wxLogWarning(_("Level of Detail '%s' (%.1f): Corresponding model missing."), i_lod->modelname.c_str(), i_lod->distance);
						} else {
							throw RCT3Exception(wxString::Format(_("Level of Detail '%s' (%.1f): Corresponding model missing"), i_lod->modelname.c_str(), i_lod->distance));
						}
					}
				}

				// Last add warnings about unused models and remember texture names and bones for used ones
				for (cModel::iterator i_mod = m_work->models.begin(); i_mod != m_work->models.end(); ++i_mod) {
					if ((i_mod->used) || READ_RCT3_EXPERTMODE()) {
						for (cMeshStruct::iterator i_mesh = i_mod->meshstructs.begin(); i_mesh != i_mod->meshstructs.end(); ++i_mesh) {
							if (!i_mesh->disabled) {
								usedtextures.Add(i_mesh->FTX);
								m_meshes++;
							}
						}
						if (!i_mod->used) {
							// Expert mode
							warning = true;
							wxLogWarning(_("Model '%s': Unused."), i_mod->name.c_str());
						}
					} else {
						warning = true;
						wxLogWarning(_("Model '%s': Unused. It will not be written to the ovl file."), i_mod->name.c_str());
					}
				}
				for (cAnimatedModel::iterator i_mod = m_work->animatedmodels.begin(); i_mod != m_work->animatedmodels.end(); ++i_mod) {
					if ((i_mod->used) || READ_RCT3_EXPERTMODE()) {
						for (cMeshStruct::iterator i_mesh = i_mod->meshstructs.begin(); i_mesh != i_mod->meshstructs.end(); ++i_mesh) {
							if (!i_mesh->disabled) {
								usedtextures.Add(i_mesh->FTX);
								m_meshes++;
							}
						}
						for (cModelBone::iterator i_bone = i_mod->modelbones.begin(); i_bone != i_mod->modelbones.end(); ++i_bone) {
							presentbones.Add(i_bone->name);
						}
						if (!i_mod->used) {
							// Expert mode
							warning = true;
							wxLogWarning(_("Animated model '%s': Unused."), i_mod->name.c_str());
						}
					} else {
						warning = true;
						wxLogWarning(_("Animated model '%s': Unused. It will not be written to the ovl file."), i_mod->name.c_str());
					}
				}

				// Check animations
				for (cAnimation::iterator i_anim = m_work->animations.begin(); i_anim != m_work->animations.end(); ++i_anim) {
					i_anim->Check(presentbones);
				}
			}

			// Check the textures
			for (cFlexiTexture::iterator i_ftx = m_work->flexitextures.begin(); i_ftx != m_work->flexitextures.end(); i_ftx++) {
				if ((!m_textureovl) && (usedtextures.Index(i_ftx->Name) == wxNOT_FOUND)) {
					if (READ_RCT3_EXPERTMODE()) {
						i_ftx->used = true;
						m_textures++;
						warning = true;
						wxLogWarning(_("Texture '%s': Unused."), i_ftx->Name.c_str());
					} else {
						// We're not writing a texture ovl and the texture is unused
						i_ftx->used = false;
						warning = true;
						wxLogWarning(_("Texture '%s': Unused. It will not be written to the ovl file."), i_ftx->Name.c_str());
					continue;
					}
				} else {
					// Either used or a texture ovl
					i_ftx->used = true;
					m_textures++;
				}

				i_ftx->Check();
			}

			if (splines.size() && (!READ_RCT3_EXPERTMODE())) {
				warning = true;
				wxLogWarning(_("You defined splines, there is currently no way to make use of them with the importer alone!"));
			}

			// Warn if neither textures nor references are given
			if ((!flexitextures.size()) && (!references.size()) && (!READ_RCT3_EXPERTMODE())) {
				warning = true;
				wxLogWarning(_("You defined neither textures nor references, are you sure that's ok?"));
			}

			// Warn if overwriting
			if (READ_RCT3_WARNOVERWRITE()) {
				wxFileName ovlfile = ovlpath;
				ovlfile.SetName(name);
				ovlfile.SetExt(wxT("common.ovl"));
				if (ovlfile.FileExists()) {
					warning = true;
					wxLogWarning(wxString::Format(_("Ovl file '%s' already exists"), ovlfile.GetFullPath().c_str()));
				}
			}

			foreach(cImpSpline& a, splines)
				a.Check();
		}
		return !warning;
	} catch (WXException& e) {
		if ((!boost::get_error_info<wxe_file>(e)) && (filename != ""))
			e << wxe_file(filename.GetFullPath());
		throw;
	}
}

void cSCNFile::Make() {
	try {
		if (name.IsEmpty()) {
			throw RCT3Exception(_("OVL Name not set"));
		}

		if (!m_work)
			Check();

		try {
			wxFileName ovlfile = ovlpath;
			ovlfile.SetName(prefix+name);
			cOvl c_ovl(std::string(ovlfile.GetFullPath().mb_str(wxConvFile)));
			MakeToOvl(c_ovl);
			c_ovl.Save();
		} catch (EOvl& e) {
			throw RCT3Exception(wxString::Format(_("Error during OVL creation: %s"), wxString(e.what(), wxConvLocal).c_str()));
		} catch (Magick::Exception& e) {
			throw RCT3Exception(wxString::Format(_("Error from image library (during OVL creation): %s"), wxString(e.what(), wxConvLocal).c_str()));
		}
	} catch (WXException& e) {
		if ((!boost::get_error_info<wxe_file>(e)) && (filename != ""))
			e << wxe_file(filename.GetFullPath());
		throw;
	}
}

/** @brief getPrefixed
  *
  * @todo: document this function
  */
wxString cSCNFile::getPrefixed(const wxString& pref, bool isTexture) {
	if ((pref[0] == '[') && (pref[pref.size()-1] == ']')) {
		wxString expref = pref.AfterFirst('[').BeforeLast(']');
		if (!boost::all(string(expref.mb_str()), boost::is_any_of(ASCIIALNUM)))
			wxLogWarning(wxString::Format(_("Name '%s' contains non-alphanumeric or non-AscII characters. This can lead to trouble."), expref.c_str()));
        return expref;
    } else {
		if (!boost::all(string(pref.mb_str()), boost::is_any_of(ASCIIALNUM)))
			wxLogWarning(wxString::Format(_("Name '%s' contains non-alphanumeric or non-AscII characters. This can lead to trouble."), pref.c_str()));
		if (isTexture && cFlexiTexture::isReserved(pref))
			return pref;
		else
			return prefix + pref;
	}
}


#define PARSE_ALGO(arg, def) \
    { \
        wxString algo = def; \
        if (algo.IsSameAs(wxT("min"), false)) { \
            sortalgo_ ## arg = cTriangleSortAlgorithm::MIN; \
        } else if (algo.IsSameAs(wxT("max"), false)) { \
            sortalgo_ ## arg = cTriangleSortAlgorithm::MAX; \
        } else if (algo.IsSameAs(wxT("mean"), false)) { \
            sortalgo_ ## arg = cTriangleSortAlgorithm::MEAN; \
        } else if (algo.IsSameAs(wxT("minmax"), false)) { \
            sortalgo_ ## arg = cTriangleSortAlgorithm::MINMAX; \
        } else if (algo.IsSameAs(wxT("maxmin"), false)) { \
            sortalgo_ ## arg = cTriangleSortAlgorithm::MAXMIN; \
        } else if (algo.IsSameAs(wxT("none"), false)) { \
            sortalgo_ ## arg = cTriangleSortAlgorithm::NONE; \
        } else { \
            throw RCT3Exception(wxString::Format(_("Unknown triangle sort algorithm: %s"), algo.c_str())); \
        } \
    }

void cSCNFile::MakeToOvl(cOvl& c_ovl) {
    MakeToOvlMain(c_ovl);
    if (!m_textureovl)
        MakeToOvlAnimations(c_ovl);
    MakeToOvlSplines(c_ovl);
}

void cSCNFile::MakeToOvlMain(cOvl& c_ovl) {
	try {
		if (!m_work)
			Check();

	//    wxString a = READ_RCT3_TRIANGLESORT_X();
	//    cTriangleSortAlgorithm::Algorithm sortalgo_x = cTriangleSortAlgorithm::GetAlgo(a.mb_str(wxConvLocal));
	//    cTriangleSortAlgorithm::Algorithm sortalgo_y = cTriangleSortAlgorithm::GetAlgo("mean");
	//    cTriangleSortAlgorithm::Algorithm sortalgo_z = cTriangleSortAlgorithm::GetAlgo("mean");
		cTriangleSortAlgorithm::Algorithm sortalgo_x = cTriangleSortAlgorithm::GetAlgo(READ_RCT3_TRIANGLESORT_X().mb_str(wxConvLocal));
		if (sortalgo_x == cTriangleSortAlgorithm::EnumSize) {
			throw RCT3Exception(wxString::Format(_("Unknown triangle sort algorithm: %s"), READ_RCT3_TRIANGLESORT_X().c_str()));
		}
		wxLogVerbose(_("Default x-sort algorithm: ") + READ_RCT3_TRIANGLESORT_X());
		cTriangleSortAlgorithm::Algorithm sortalgo_y = cTriangleSortAlgorithm::GetAlgo(READ_RCT3_TRIANGLESORT_Y().mb_str(wxConvLocal));
		if (sortalgo_y == cTriangleSortAlgorithm::EnumSize) {
			throw RCT3Exception(wxString::Format(_("Unknown triangle sort algorithm: %s"), READ_RCT3_TRIANGLESORT_Y().c_str()));
		}
		wxLogVerbose(_("Default y-sort algorithm: ") + READ_RCT3_TRIANGLESORT_Y());
		cTriangleSortAlgorithm::Algorithm sortalgo_z = cTriangleSortAlgorithm::GetAlgo(READ_RCT3_TRIANGLESORT_Z().mb_str(wxConvLocal));
		if (sortalgo_z == cTriangleSortAlgorithm::EnumSize) {
			throw RCT3Exception(wxString::Format(_("Unknown triangle sort algorithm: %s"), READ_RCT3_TRIANGLESORT_Z().c_str()));
		}
		wxLogVerbose(_("Default z-sort algorithm: ") + READ_RCT3_TRIANGLESORT_Z());

	//    PARSE_ALGO(x, READ_RCT3_TRIANGLESORT_X());
	//    PARSE_ALGO(y, READ_RCT3_TRIANGLESORT_Y());
	//    PARSE_ALGO(z, READ_RCT3_TRIANGLESORT_Z());
	//    {
	//        wxString algo = READ_RCT3_TRIANGLESORT();
	//        if (algo.IsSameAs(wxT("min"), false)) {
	//            sortalgo = cTriangleSortAlgorithm::MIN;
	//        } else if (algo.IsSameAs(wxT("max"), false)) {
	//            sortalgo = cTriangleSortAlgorithm::MAX;
	//        } else if (algo.IsSameAs(wxT("mean"), false)) {
	//            sortalgo = cTriangleSortAlgorithm::MEAN;
	//        } else if (algo.IsSameAs(wxT("minmax"), false)) {
	//            sortalgo = cTriangleSortAlgorithm::MINMAX;
	//        } else if (algo.IsSameAs(wxT("maxmin"), false)) {
	//            sortalgo = cTriangleSortAlgorithm::MAXMIN;
	//        } else if (algo.IsSameAs(wxT("none"), false)) {
	//            sortalgo = cTriangleSortAlgorithm::NONE;
	//        } else {
	//            throw RCT3Exception(wxString::Format(_("Unknown triangle sort algorithm: %s"), algo.c_str()));
	//        }
	//    }

		// References
		foreach(const cReference& ref, references) {
			wxLogVerbose(_("Adding Reference: ") + ref.name);
			c_ovl.AddReference(ref.name.ToAscii());
		}

		// SVD, shapes & animations
		if (!m_textureovl) {
			if (m_work->lods.size()) {
				// SVD
				wxLogDebug(wxT("TRACE cSCNFile::MakeToOvl SVD"));
				wxLogVerbose(_("Adding SVD: ") + name);
				ovlSVDManager* c_svd = c_ovl.GetManager<ovlSVDManager>();
				cSceneryItemVisual c_svis;
				c_svis.name = getPrefixed(name).ToAscii();
				c_svis.sivflags = sivsettings.sivflags;
				c_svis.sway = sivsettings.sway;
				c_svis.brightness = sivsettings.brightness;
				c_svis.scale = sivsettings.scale;
				c_svis.unk4 = sivsettings.unknown;
				c_svis.unk6 = sivsettings.unk6;
				c_svis.unk7 = sivsettings.unk7;
				c_svis.unk8 = sivsettings.unk8;
				c_svis.unk9 = sivsettings.unk9;
				c_svis.unk10 = sivsettings.unk10;
				c_svis.unk11 = sivsettings.unk11;
				for (cLOD::iterator it = m_work->lods.begin(); it != m_work->lods.end(); ++it) {
					cSceneryItemVisualLOD c_slod;
					wxLogVerbose(_("  Adding LOD: ") + it->modelname);
					c_slod.name = getPrefixed(it->modelname).ToAscii();
					c_slod.distance = it->distance;
					c_slod.unk2 = it->unk2;
					c_slod.unk4 = it->unk4;
					c_slod.unk14 = it->unk14;
					if (it->animated) {
						c_slod.meshtype = r3::Constants::SVD::LOD_Type::Animated;
						c_slod.boneshape = getPrefixed(it->modelname).ToAscii();
						if (it->animations.size()) {
							for (cStringIterator its = it->animations.begin(); its != it->animations.end(); ++its) {
								c_slod.animations.push_back(std::string(getPrefixed(*its).ToAscii()));
							}
						}
					} else {
						c_slod.meshtype = r3::Constants::SVD::LOD_Type::Static;
						c_slod.staticshape = getPrefixed(it->modelname).ToAscii();
					}
					c_svis.lods.push_back(c_slod);
				}
				c_svd->AddSVD(c_svis);
			}

			// Static shapes
			if (m_work->models.size()) {
				wxLogDebug(wxT("TRACE cSCNFile::MakeToOvl SHS"));
				ovlSHSManager* c_shs = c_ovl.GetManager<ovlSHSManager>();
				for (cModel::iterator i_mod = m_work->models.begin(); i_mod != m_work->models.end(); ++i_mod) {
					if (!i_mod->used)
						continue;
					wxLogVerbose(_("Adding static model: ") + i_mod->name);

					cStaticShape1 c_mod;

					boundsInit(&c_mod.bbox1, &c_mod.bbox2);

					// Find active mesh count
					c_mod.name = getPrefixed(i_mod->name).ToAscii();

					// Determine transformation matrices
					MATRIX transformMatrix;
					MATRIX undoDamage;
					bool do_transform = i_mod->GetTransformationMatrices(transformMatrix, undoDamage);

					// Do effect points
					if (i_mod->effectpoints.size() != 0) {
						for (unsigned int e = 0; e < i_mod->effectpoints.size(); e++) {
							cEffectStruct c_es;
							wxLogVerbose(_("  Adding effect point: ") + i_mod->effectpoints[e].name);
							c_es.name = i_mod->effectpoints[e].name.ToAscii();
							c_es.pos = matrixThereAndBackAgain(i_mod->effectpoints[e].transforms, transformMatrix, undoDamage);
							c_mod.effects.push_back(c_es);
						}
					}

					boost::shared_ptr<c3DLoader> object = c3DLoader::LoadFile(i_mod->file.GetFullPath().c_str());
					if (!object.get()) {
						// Poof went the model!
						throw RCT3Exception(wxString::Format(_("Something happened to the file of model '%s'"), i_mod->name.c_str()));
					}

					//unsigned long CurrentObj = 0;
					VECTOR temp_min, temp_max;

					foreach(vector<int>& meshvec, i_mod->mesh_compaction) {
						cStaticShape2 c_ss2;
						cMeshStruct* i_mesh = &i_mod->meshstructs[meshvec[0]];

						wxLogVerbose(_("  Adding group: ") + i_mesh->Name);
						c_ss2.fts = getPrefixed(i_mesh->FTX, true).ToAscii();
						c_ss2.texturestyle = i_mesh->TXS.ToAscii();
						c_ss2.placetexturing = i_mesh->place;
						c_ss2.textureflags = i_mesh->flags;
						c_ss2.sides = i_mesh->unknown;
						if (i_mesh->algo_x.IsEmpty())
							c_ss2.algo_x = sortalgo_x;
						else
							c_ss2.algo_x = cTriangleSortAlgorithm::GetAlgo(i_mesh->algo_x.mb_str(wxConvLocal));
						if (i_mesh->algo_y.IsEmpty())
							c_ss2.algo_y = sortalgo_y;
						else
							c_ss2.algo_y = cTriangleSortAlgorithm::GetAlgo(i_mesh->algo_y.mb_str(wxConvLocal));
						if (i_mesh->algo_z.IsEmpty())
							c_ss2.algo_z = sortalgo_z;
						else
							c_ss2.algo_z = cTriangleSortAlgorithm::GetAlgo(i_mesh->algo_z.mb_str(wxConvLocal));
						if (i_mesh->place) {
							wxLogVerbose(_("    Using sorting algorithms (x/y/z): ") +
								wxString(cTriangleSortAlgorithm::GetAlgorithmName(c_ss2.algo_x), wxConvLocal) + wxT(" / ") +
								wxString(cTriangleSortAlgorithm::GetAlgorithmName(c_ss2.algo_y), wxConvLocal) + wxT(" / ") +
								wxString(cTriangleSortAlgorithm::GetAlgorithmName(c_ss2.algo_z), wxConvLocal)
							);
						}

						VECTOR c_fudge_normal;
						VECTOR* c_pfudge_normal = NULL;
						boundsInit(&temp_min, &temp_max);
						switch (i_mesh->fudgenormals) {
							case CMS_FUDGE_X:
								c_fudge_normal = vectorMake(1.0, 0.0, 0.0);
								c_pfudge_normal = &c_fudge_normal;
								break;
							case CMS_FUDGE_Y:
								c_fudge_normal = vectorMake(0.0, 1.0, 0.0);
								c_pfudge_normal = &c_fudge_normal;
								break;
							case CMS_FUDGE_Z:
								c_fudge_normal = vectorMake(0.0, 0.0, 1.0);
								c_pfudge_normal = &c_fudge_normal;
								break;
							case CMS_FUDGE_XM:
								c_fudge_normal = vectorMake(-1.0, 0.0, 0.0);
								c_pfudge_normal = &c_fudge_normal;
								break;
							case CMS_FUDGE_YM:
								c_fudge_normal = vectorMake(0.0, -1.0, 0.0);
								c_pfudge_normal = &c_fudge_normal;
								break;
							case CMS_FUDGE_ZM:
								c_fudge_normal = vectorMake(0.0, 0.0, -1.0);
								c_pfudge_normal = &c_fudge_normal;
								break;
						}
						if (c_pfudge_normal)
							matrixApplyIP(c_pfudge_normal, matrixGetFixOrientation(i_mod->usedorientation));

						boundsInit(&temp_min, &temp_max);

						foreach(int mesh, meshvec) {
							wxLogVerbose(_("    Appending group: ") + i_mod->meshstructs[mesh].Name);
							object->fetchObject(mesh, &c_ss2, &temp_min, &temp_max,
												const_cast<MATRIX *> ((do_transform)?(&transformMatrix):NULL),
												c_pfudge_normal);
							boundsContain(&temp_min, &temp_max, &c_mod.bbox1, &c_mod.bbox2);
						}
						if (i_mesh->fudgenormals == CMS_FUDGE_RIM) {
							c3DLoader::FlattenNormals(&c_ss2, temp_min, temp_max);
						}
						c_mod.meshes.push_back(c_ss2);

					}
	/*
					for (cMeshStruct::iterator i_mesh = i_mod->meshstructs.begin(); i_mesh != i_mod->meshstructs.end(); i_mesh++) {
						if (i_mesh->disabled == false) {
							//progress.Update(++progress_count);
							cStaticShape2 c_ss2;
							wxLogVerbose(_("  Adding group: ") + i_mesh->Name);
							c_ss2.fts = getPrefixed(i_mesh->FTX).ToAscii();
							c_ss2.texturestyle = i_mesh->TXS.ToAscii();
							c_ss2.placetexturing = i_mesh->place;
							c_ss2.textureflags = i_mesh->flags;
							c_ss2.sides = i_mesh->unknown;
							if (i_mesh->algo_x.IsEmpty())
								c_ss2.algo_x = sortalgo_x;
							else
								c_ss2.algo_x = cTriangleSortAlgorithm::GetAlgo(i_mesh->algo_x.mb_str(wxConvLocal));
							if (i_mesh->algo_y.IsEmpty())
								c_ss2.algo_y = sortalgo_y;
							else
								c_ss2.algo_y = cTriangleSortAlgorithm::GetAlgo(i_mesh->algo_y.mb_str(wxConvLocal));
							if (i_mesh->algo_z.IsEmpty())
								c_ss2.algo_z = sortalgo_z;
							else
								c_ss2.algo_z = cTriangleSortAlgorithm::GetAlgo(i_mesh->algo_z.mb_str(wxConvLocal));
							if (i_mesh->place) {
								wxLogVerbose(_("    Using sorting algorithms (x/y/z): ") +
									wxString(cTriangleSortAlgorithm::GetAlgorithmName(c_ss2.algo_x), wxConvLocal) + wxT(" / ") +
									wxString(cTriangleSortAlgorithm::GetAlgorithmName(c_ss2.algo_y), wxConvLocal) + wxT(" / ") +
									wxString(cTriangleSortAlgorithm::GetAlgorithmName(c_ss2.algo_z), wxConvLocal)
								);
							}

							VECTOR c_fudge_normal;
							VECTOR* c_pfudge_normal = NULL;
							boundsInit(&temp_min, &temp_max);
							switch (i_mesh->fudgenormals) {
								case CMS_FUDGE_X:
									c_fudge_normal = vectorMake(1.0, 0.0, 0.0);
									c_pfudge_normal = &c_fudge_normal;
									break;
								case CMS_FUDGE_Y:
									c_fudge_normal = vectorMake(0.0, 1.0, 0.0);
									c_pfudge_normal = &c_fudge_normal;
									break;
								case CMS_FUDGE_Z:
									c_fudge_normal = vectorMake(0.0, 0.0, 1.0);
									c_pfudge_normal = &c_fudge_normal;
									break;
								case CMS_FUDGE_XM:
									c_fudge_normal = vectorMake(-1.0, 0.0, 0.0);
									c_pfudge_normal = &c_fudge_normal;
									break;
								case CMS_FUDGE_YM:
									c_fudge_normal = vectorMake(0.0, -1.0, 0.0);
									c_pfudge_normal = &c_fudge_normal;
									break;
								case CMS_FUDGE_ZM:
									c_fudge_normal = vectorMake(0.0, 0.0, -1.0);
									c_pfudge_normal = &c_fudge_normal;
									break;
							}
							if (c_pfudge_normal)
								matrixApplyIP(c_pfudge_normal, matrixGetFixOrientation(i_mod->usedorientation));

							boundsInit(&temp_min, &temp_max);
							object->FetchObject(CurrentObj, &c_ss2, &temp_min, &temp_max,
												const_cast<MATRIX *> ((do_transform)?(&transformMatrix):NULL),
												c_pfudge_normal);
							boundsContain(&temp_min, &temp_max, &c_mod.bbox1, &c_mod.bbox2);
							if (i_mesh->fudgenormals == CMS_FUDGE_RIM) {
								c3DLoader::FlattenNormals(&c_ss2, temp_min, temp_max);
							}
							c_mod.meshes.push_back(c_ss2);
						}
						CurrentObj++;
					}
	*/
					c_shs->AddModel(c_mod);
				}
			}

			// Animated shapes
			if (m_work->animatedmodels.size()) {
				wxLogDebug(wxT("TRACE cSCNFile::MakeToOvl BSH"));
				ovlBSHManager* c_bsh = c_ovl.GetManager<ovlBSHManager>();
				for (cAnimatedModel::iterator i_mod = m_work->animatedmodels.begin(); i_mod != m_work->animatedmodels.end(); ++i_mod) {
					if (!i_mod->used)
						continue;
					wxLogVerbose(_("Adding animated model: ") + i_mod->name);

					cBoneShape1 c_bs1;

					boundsInit(&c_bs1.bbox1, &c_bs1.bbox2);

					// Find active mesh count
					vector<wxString> bonenames;
					c_bs1.name = getPrefixed(i_mod->name).ToAscii();
					c_bs1.bones.push_back(cBoneStruct(true));
					bonenames.push_back("Scene Root");

					// Determine transformation matrices
					MATRIX transformMatrix;
					MATRIX undoDamage;
					bool do_transform = i_mod->GetTransformationMatrices(transformMatrix, undoDamage);

					// Do effect bones
					if (i_mod->modelbones.size() != 0) {
						for (unsigned int e = 0; e < i_mod->modelbones.size(); e++) {
							cBoneStruct c_bone;
							wxLogVerbose(_("  Adding bone: ") + i_mod->modelbones[e].name);
							c_bone.name = i_mod->modelbones[e].name.ToAscii();
							bonenames.push_back(i_mod->modelbones[e].name);
							c_bone.parentbonenumber = i_mod->modelbones[e].nparent;
							c_bone.pos1 = matrixThereAndBackAgain(i_mod->modelbones[e].positions1, transformMatrix, undoDamage);
							c_bone.pos2 = matrixThereAndBackAgain(i_mod->modelbones[e].positions2, transformMatrix, undoDamage);
							c_bs1.bones.push_back(c_bone);
						}
					}

					boost::shared_ptr<c3DLoader> object = c3DLoader::LoadFile(i_mod->file.GetFullPath().c_str());
					if (!object.get()) {
						// Poof went the model!
						throw RCT3Exception(wxString::Format(_("Something happened to the file of model '%s'"), i_mod->name.c_str()));
					}

					//unsigned long CurrentObj = 0;
					VECTOR temp_min, temp_max;
					foreach(vector<int>& meshvec, i_mod->mesh_compaction) {
						cBoneShape2 c_bs2;
						cMeshStruct* i_mesh = &i_mod->meshstructs[meshvec[0]];

						wxLogVerbose(_("  Adding group: ") + i_mesh->Name);
						c_bs2.fts = getPrefixed(i_mesh->FTX, true).ToAscii();
						c_bs2.texturestyle = i_mesh->TXS.ToAscii();
						c_bs2.placetexturing = i_mesh->place;
						c_bs2.textureflags = i_mesh->flags;
						c_bs2.sides = i_mesh->unknown;
						if (i_mesh->algo_x.IsEmpty())
							c_bs2.algo_x = sortalgo_x;
						else
							c_bs2.algo_x = cTriangleSortAlgorithm::GetAlgo(i_mesh->algo_x.mb_str(wxConvLocal));
						if (i_mesh->algo_y.IsEmpty())
							c_bs2.algo_y = sortalgo_y;
						else
							c_bs2.algo_y = cTriangleSortAlgorithm::GetAlgo(i_mesh->algo_y.mb_str(wxConvLocal));
						if (i_mesh->algo_z.IsEmpty())
							c_bs2.algo_z = sortalgo_z;
						else
							c_bs2.algo_z = cTriangleSortAlgorithm::GetAlgo(i_mesh->algo_z.mb_str(wxConvLocal));
						if (i_mesh->place) {
							wxLogVerbose(_("    Using sorting algorithms (x/y/z): ") +
								wxString(cTriangleSortAlgorithm::GetAlgorithmName(c_bs2.algo_x), wxConvLocal) + wxT(" / ") +
								wxString(cTriangleSortAlgorithm::GetAlgorithmName(c_bs2.algo_y), wxConvLocal) + wxT(" / ") +
								wxString(cTriangleSortAlgorithm::GetAlgorithmName(c_bs2.algo_z), wxConvLocal)
							);
						}

						VECTOR c_fudge_normal;
						VECTOR* c_pfudge_normal = NULL;
						switch (i_mesh->fudgenormals) {
							case CMS_FUDGE_X:
								c_fudge_normal = vectorMake(1.0, 0.0, 0.0);
								c_pfudge_normal = &c_fudge_normal;
								break;
							case CMS_FUDGE_Y:
								c_fudge_normal = vectorMake(0.0, 1.0, 0.0);
								c_pfudge_normal = &c_fudge_normal;
								break;
							case CMS_FUDGE_Z:
								c_fudge_normal = vectorMake(0.0, 0.0, 1.0);
								c_pfudge_normal = &c_fudge_normal;
								break;
							case CMS_FUDGE_XM:
								c_fudge_normal = vectorMake(-1.0, 0.0, 0.0);
								c_pfudge_normal = &c_fudge_normal;
								break;
							case CMS_FUDGE_YM:
								c_fudge_normal = vectorMake(0.0, -1.0, 0.0);
								c_pfudge_normal = &c_fudge_normal;
								break;
							case CMS_FUDGE_ZM:
								c_fudge_normal = vectorMake(0.0, 0.0, -1.0);
								c_pfudge_normal = &c_fudge_normal;
								break;
						}
						if (c_pfudge_normal)
							matrixApplyIP(c_pfudge_normal, matrixGetFixOrientation(i_mod->usedorientation));

						boundsInit(&temp_min, &temp_max);
						foreach(int mesh, meshvec) {
							wxLogVerbose(_("    Appending group: ") + i_mod->meshstructs[mesh].Name);
							if (i_mod->meshstructs[mesh].multibone) {
								object->fetchObject(mesh, bonenames, &c_bs2, &temp_min, &temp_max,
														  const_cast<MATRIX *> ((do_transform)?(&transformMatrix):NULL),
														  c_pfudge_normal);
							} else {
								object->fetchObject(mesh, i_mod->meshstructs[mesh].bone, &c_bs2, &temp_min, &temp_max,
														  const_cast<MATRIX *> ((do_transform)?(&transformMatrix):NULL),
														  c_pfudge_normal);
							}
							boundsContain(&temp_min, &temp_max, &c_bs1.bbox1, &c_bs1.bbox2);
						}
						if (i_mesh->fudgenormals == CMS_FUDGE_RIM) {
							c3DLoader::FlattenNormals(&c_bs2, temp_min, temp_max);
						}
						c_bs1.meshes.push_back(c_bs2);

					}
	/*
					for (cMeshStruct::iterator i_mesh = i_mod->meshstructs.begin(); i_mesh != i_mod->meshstructs.end(); i_mesh++) {
						if (i_mesh->disabled == false) {
							//progress.Update(++progress_count);
							cBoneShape2 c_bs2;
							wxLogVerbose(_("  Adding group: ") + i_mesh->Name);
							c_bs2.fts = getPrefixed(i_mesh->FTX).ToAscii();
							c_bs2.texturestyle = i_mesh->TXS.ToAscii();
							c_bs2.placetexturing = i_mesh->place;
							c_bs2.textureflags = i_mesh->flags;
							c_bs2.sides = i_mesh->unknown;
							if (i_mesh->algo_x.IsEmpty())
								c_bs2.algo_x = sortalgo_x;
							else
								c_bs2.algo_x = cTriangleSortAlgorithm::GetAlgo(i_mesh->algo_x.mb_str(wxConvLocal));
							if (i_mesh->algo_y.IsEmpty())
								c_bs2.algo_y = sortalgo_y;
							else
								c_bs2.algo_y = cTriangleSortAlgorithm::GetAlgo(i_mesh->algo_y.mb_str(wxConvLocal));
							if (i_mesh->algo_z.IsEmpty())
								c_bs2.algo_z = sortalgo_z;
							else
								c_bs2.algo_z = cTriangleSortAlgorithm::GetAlgo(i_mesh->algo_z.mb_str(wxConvLocal));
							if (i_mesh->place) {
								wxLogVerbose(_("    Using sorting algorithms (x/y/z): ") +
									wxString(cTriangleSortAlgorithm::GetAlgorithmName(c_bs2.algo_x), wxConvLocal) + wxT(" / ") +
									wxString(cTriangleSortAlgorithm::GetAlgorithmName(c_bs2.algo_y), wxConvLocal) + wxT(" / ") +
									wxString(cTriangleSortAlgorithm::GetAlgorithmName(c_bs2.algo_z), wxConvLocal)
								);
							}

							VECTOR c_fudge_normal;
							VECTOR* c_pfudge_normal = NULL;
							switch (i_mesh->fudgenormals) {
								case CMS_FUDGE_X:
									c_fudge_normal = vectorMake(1.0, 0.0, 0.0);
									c_pfudge_normal = &c_fudge_normal;
									break;
								case CMS_FUDGE_Y:
									c_fudge_normal = vectorMake(0.0, 1.0, 0.0);
									c_pfudge_normal = &c_fudge_normal;
									break;
								case CMS_FUDGE_Z:
									c_fudge_normal = vectorMake(0.0, 0.0, 1.0);
									c_pfudge_normal = &c_fudge_normal;
									break;
								case CMS_FUDGE_XM:
									c_fudge_normal = vectorMake(-1.0, 0.0, 0.0);
									c_pfudge_normal = &c_fudge_normal;
									break;
								case CMS_FUDGE_YM:
									c_fudge_normal = vectorMake(0.0, -1.0, 0.0);
									c_pfudge_normal = &c_fudge_normal;
									break;
								case CMS_FUDGE_ZM:
									c_fudge_normal = vectorMake(0.0, 0.0, -1.0);
									c_pfudge_normal = &c_fudge_normal;
									break;
							}
							if (c_pfudge_normal)
								matrixApplyIP(c_pfudge_normal, matrixGetFixOrientation(i_mod->usedorientation));

							boundsInit(&temp_min, &temp_max);
							object->FetchObject(CurrentObj, i_mesh->bone, &c_bs2, &temp_min, &temp_max,
													  const_cast<MATRIX *> ((do_transform)?(&transformMatrix):NULL),
													  c_pfudge_normal);
							boundsContain(&temp_min, &temp_max, &c_bs1.bbox1, &c_bs1.bbox2);
							if (i_mesh->fudgenormals == CMS_FUDGE_RIM) {
								c3DLoader::FlattenNormals(&c_bs2, temp_min, temp_max);
							}
							c_bs1.meshes.push_back(c_bs2);
						}
						CurrentObj++;
					}
	*/
					c_bs1.bbox1.x -= 1.0;
					c_bs1.bbox1.y -= 1.0;
					c_bs1.bbox1.z -= 1.0;
					c_bs1.bbox2.x += 1.0;
					c_bs1.bbox2.y += 1.0;
					c_bs1.bbox2.z += 1.0;
					c_bsh->AddModel(c_bs1);
				}
			}

			// Animations
			// put to MakeToOvlAnimations
		}

		// Textures
		if (m_work->flexitextures.size()) {
			wxLogDebug(wxT("TRACE cSCNFile::MakeToOvl FTX"));
			ovlFTXManager* c_ftx = c_ovl.GetManager<ovlFTXManager>();
			for (cFlexiTexture::iterator i_ftx = m_work->flexitextures.begin(); i_ftx != m_work->flexitextures.end(); i_ftx++) {
				if (!i_ftx->used)
					continue;

				cFlexiTextureInfoStruct c_ftis;
				wxLogVerbose(_("Adding texture: ") + i_ftx->Name);
				c_ftis.name = getPrefixed(i_ftx->Name).ToAscii();
				c_ftis.fps = i_ftx->FPS;
				c_ftis.recolourable = i_ftx->Recolorable;


				//progress.Update(++progress_count);
	// TODO (tobi#1#): Reimplement textrue cache

	//                if (m_textureovl) {
	//                    ::wxGetApp().g_texturecache.push_back(i_ftx->Name);
	//                }

				// Make the animation
	//                unsigned long animationcount = 0;
	//                for(cFlexiTextureAnim::iterator i_anim = i_ftx->Animation.begin(); i_anim != i_ftx->Animation.end(); i_anim++)
	//                    animationcount += i_anim->count();
	//                unsigned long animation[animationcount];
	//                unsigned long x = 0;
				for(cFlexiTextureAnim::iterator i_anim = i_ftx->Animation.begin(); i_anim != i_ftx->Animation.end(); i_anim++) {
					for (unsigned long i = 0; i < i_anim->count(); i++) {
						c_ftis.animation.push_back(i_anim->frame());
	//                        animation[x] = i_anim->frame();
	//                        x++;
					}
				}

				// Get the size from the first frame
				long dimension = checkRCT3Texture(i_ftx->Frames[0].texture().GetFullPath());
				long texhandling = READ_RCT3_TEXTURE();
				if (dimension > 1024)
					dimension = 1024;
				switch (texhandling) {
					case RCT3_TEXTURE_SCALE_DOWN:
						dimension = 1 << local_log2(dimension);
						break;
					case RCT3_TEXTURE_SCALE_UP: {
							unsigned int l = local_log2(dimension);
							if ((1 << l) != dimension)
								dimension = 1 << (l+1);
						}
						break;
				}

				c_ftis.dimension = dimension;

				// Now loop through the frames
				for (cFlexiTextureFrame::iterator i_ftxfr = i_ftx->Frames.begin(); i_ftxfr != i_ftx->Frames.end(); i_ftxfr++) {
					cFlexiTextureStruct c_fts;
					c_fts.dimension = dimension;
					c_fts.recolourable = i_ftxfr->recolorable();

					wxGXImage tex(i_ftxfr->texture().GetFullPath());
					c_fts.palette = counted_array_ptr<unsigned char>(new unsigned char[256 * sizeof(COLOURQUAD)]);
					memset(c_fts.palette.get(), 0, 256 * sizeof(COLOURQUAD));

					if ((tex.GetWidth() != static_cast<unsigned long>(dimension)) || (tex.GetHeight() != static_cast<unsigned long>(dimension)))
						tex.Rescale(dimension, dimension);
					c_fts.texture = counted_array_ptr<unsigned char>(new unsigned char[dimension * dimension]);
					if (i_ftxfr->alphasource() != CFTF_ALPHA_NONE)
						c_fts.alpha = counted_array_ptr<unsigned char>(new unsigned char[dimension * dimension]);

					tex.flip();

					if (i_ftxfr->recolorable() && ((tex.type() != Magick::PaletteType) || (tex.magick()==wxT("PNG")))) {
						memcpy(c_fts.palette.get(), cFlexiTexture::GetRGBPalette(), 256 * sizeof(COLOURQUAD));
						tex.GetAs8bitForced(c_fts.texture.get(), c_fts.palette.get(), true);
						memcpy(c_fts.palette.get(), cFlexiTexture::GetBMYPalette(), 256 * sizeof(COLOURQUAD));
					} else {
						tex.GetAs8bit(c_fts.texture.get(), c_fts.palette.get());
					}

					if (i_ftxfr->alphasource() == CFTF_ALPHA_INTERNAL) {
						tex.GetAlpha(c_fts.alpha.get());
					} else if (i_ftxfr->alphasource() == CFTF_ALPHA_EXTERNAL) {
						wxGXImage alp(i_ftxfr->alpha().GetFullPath());
						if ((alp.GetWidth() != static_cast<unsigned long>(dimension)) || (alp.GetHeight() != static_cast<unsigned long>(dimension)))
							alp.Rescale(dimension, dimension);
						alp.flip();
						alp.GetGrayscale(c_fts.alpha.get());
					}

					for (unsigned int j = 0; j < 256; j++)
						reinterpret_cast<COLOURQUAD*>(c_fts.palette.get())[j].alpha = i_ftxfr->alphacutoff();

					c_ftis.frames.push_back(c_fts);
				}

				c_ftx->AddTexture(c_ftis);
			}
		}
	} catch (WXException& e) {
		if ((!boost::get_error_info<wxe_file>(e)) && (filename != ""))
			e << wxe_file(filename.GetFullPath());
		throw;
	}

}

/** @brief MakeToOvlAnimations
  *
  * @todo: document this function
  */
void cSCNFile::MakeToOvlAnimations(cOvl& c_ovl) {
	try {
		if (!m_work) {
			// called from ovlmake
			foreach(cAnimation& a, animations)
				a.CheckTimes();
		}
		cSCNFile& use = m_work?(*m_work.get()):(*this);

		if (use.animations.size()) {
			wxLogDebug(wxT("TRACE cSCNFile::MakeToOvl BAN"));
			ovlBANManager* c_ban = c_ovl.GetManager<ovlBANManager>();
			for (cAnimation::iterator i_anim = use.animations.begin(); i_anim != use.animations.end(); ++i_anim) {
				cBoneAnim c_item;
				c_item.name = getPrefixed(i_anim->name).ToAscii();
				wxLogVerbose(_("Adding animation: ") + i_anim->name);
				for (cBoneAnimation::iterator i_bone = i_anim->boneanimations.begin(); i_bone != i_anim->boneanimations.end(); ++i_bone) {
					cBoneAnimBone c_bone;
					c_bone.name = i_bone->name.ToAscii();
					wxLogVerbose(_("  Adding bone: ") + i_bone->name);
					for (cTXYZ::iterator i_txyz = i_bone->translations.begin(); i_txyz != i_bone->translations.end(); ++i_txyz) {
						c_bone.translations.insert(i_txyz->GetFixed(i_anim->usedorientation, false));
					}
					for (cTXYZ::iterator i_txyz = i_bone->rotations.begin(); i_txyz != i_bone->rotations.end(); ++i_txyz) {
						c_bone.rotations.insert(i_txyz->GetFixed(i_anim->usedorientation, true, i_bone->axis));
					}
					c_item.bones.push_back(c_bone);
				}
				c_ban->AddAnimation(c_item);
			}
		}
	} catch (WXException& e) {
		if ((!boost::get_error_info<wxe_file>(e)) && (filename != ""))
			e << wxe_file(filename.GetFullPath());
		throw;
	}
}

/** @brief MakeToOvlSplines
  *
  * @todo: document this function
  */
void cSCNFile::MakeToOvlSplines(cOvl& c_ovl) {
	try {
		if (!m_work) {
			// called from ovlmake
			foreach(cImpSpline& a, splines)
				a.Check();
		}
		// Splines
		if (splines.size()) {
			wxLogDebug(wxT("TRACE cSCNFile::MakeToOvl SPL"));
			ovlSPLManager* c_spl = c_ovl.GetManager<ovlSPLManager>();
			foreach(const cImpSpline& cspl, splines) {
				cSpline spl;
				wxLogVerbose(_("Adding spline: ") + cspl.spline.m_name);
				spl.name = getPrefixed(cspl.spline.m_name).ToAscii();
				spl.cyclic = cspl.spline.m_cyclic;
				spl.nodes = cspl.spline.GetFixed(cspl.usedorientation);
				c_spl->AddSpline(spl);
			}
		}
	} catch (WXException& e) {
		if ((!boost::get_error_info<wxe_file>(e)) && (filename != ""))
			e << wxe_file(filename.GetFullPath());
		throw;
	}
}


void cSCNFile::CleanWork() {
    m_work.reset();
    m_textureovl = false;
    m_meshes = 0;
    m_textures = 0;
}
