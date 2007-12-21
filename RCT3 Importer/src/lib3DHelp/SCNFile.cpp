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

#include <sstream>
#include <stdio.h>

#include "confhelp.h"
#include "gximage.h"
#include "lib3Dconfig.h"
#include "matrix.h"
#include "OVLEXception.h"
#include "OVLManagers.h"
#include "OVLng.h"
#include "RCT3Exception.h"
#include "rct3log.h"
#include "texcheck.h"

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
    flexitextures.clear();
    models.clear();
    animatedmodels.clear();
    lods.clear();
    animations.clear();
    references.clear();
    sivsettings = cSIVSettings();

//    error = CSCNFILE_NO_ERROR;
}

bool cSCNFile::Load() {
    bool ret = true;
    Init();

    if (filename == "") {
        throw RCT3Exception(_("No scenery file name given."));
    }

    if (filename.GetExt().Lower() == wxT("xml")) {
        wxXmlDocument doc;
        if (doc.Load(filename.GetFullPath())) {
            ret = LoadXML(&doc);
            if (!ret)
                wxLogWarning(_("There were warnings or errors loading the xml file.\nIf they concern models, they will be shown to you when you open the respective settings."));
            return ret;
        } else {
            // wxXmlDocument reports errors
            return false;
        }
    }

    if (filename.GetExt().Lower() != wxT("scn")) {
        wxLogWarning(_("Unrecognized file extension. Assuming it's a SCN scenery file."));
    }

    FILE *f = fopen(filename.GetFullPath().fn_str(), "rb");
    if (!f) {
        throw RCT3Exception(_("Failed to open scenery file."));
    }

    unsigned long objlen;
    fread(&objlen, sizeof(objlen), 1, f);

/*
    // Check for XML file
    if (objlen == CSCNFILE_XMLFILE) {
        fclose(f);
        return LoadXML();
    }
*/

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
        throw RCT3Exception(_("Invalid scenery file."));
    }

    // Read version
    fread(&version, sizeof(long), 1, f);
    if (version > VERSION_CSCNFILE) {
        fclose(f);
        throw RCT3Exception(_("Scenery file version not supported."));
    }

    // Read ovl name
    unsigned long count;
    fread(&count, sizeof(count), 1, f);
    if (count) {
        char *tmp = new char[count];
        fread(tmp, count, 1, f);
        wxFileName ovlname = wxString(tmp);
        if (strlen(tmp)) {
            if (!ovlname.IsAbsolute()) {
                ovlname.MakeAbsolute(filename.GetPath());
            }
        }
        delete[] tmp;
        ovlpath = ovlname.GetPathWithSep();
        name = ovlname.GetName();
    }

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
            ft.Name = tmp;
            delete[] tmp;
        }

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
                ftfr.texture(wxString(tmp));
                if (strlen(tmp)) {
                    if (!ftfr.texture().IsAbsolute()) {
                        ftfr.texture_nc().MakeAbsolute(filename.GetPath());
                    }
                }
                delete[] tmp;
            }

            unsigned long alphalen = 0;
            fread(&alphalen, sizeof(alphalen), 1, f);
            if (alphalen) {
                tmp = new char[alphalen];
                fread(tmp, alphalen, 1, f);
                ftfr.alpha(wxString(tmp));
                if (strlen(tmp)) {
                    if (!ftfr.alpha().IsAbsolute()) {
                        ftfr.alpha_nc().MakeAbsolute(filename.GetPath());
                    }
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
            mod.name = tmp;
            delete[] tmp;
        }

        // Read model filename
        count = 0;
        fread(&count, sizeof(count), 1, f);
        if (count) {
            tmp = new char[count];
            fread(tmp, count, 1, f);
            mod.file = wxString(tmp);
            if (strlen(tmp)) {
                if (!mod.file.IsAbsolute()) {
                    mod.file.MakeAbsolute(filename.GetPath());
                }
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
                mod.transformnames.push_back(wxString(tmp));
                delete[] tmp;
            } else {
                mod.transformnames.push_back(_("<empty name>"));
            }

            // Matrix
            D3DMATRIX m;

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
                ms.Name = tmp;
                delete[] tmp;
            }

            // Enabled or disabled
            BOOL dis;
            fread(&dis, sizeof(BOOL), 1, f);
            ms.disabled = dis;

            fread(&ms.flags, sizeof(ms.flags), 1, f);
            fread(&ms.place, sizeof(ms.place), 1, f);
            fread(&ms.unknown, sizeof(ms.unknown), 1, f);

            count = 0;
            fread(&count, sizeof(count), 1, f);
            if (count) {
                tmp = new char[count];
                fread(tmp, count, 1, f);
                ms.FTX = tmp;
                delete[] tmp;
            }

            count = 0;
            fread(&count, sizeof(count), 1, f);
            if (count) {
                tmp = new char[count];
                fread(tmp, count, 1, f);
                ms.TXS = tmp;
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
                e.name = tmp;
                delete[] tmp;
            }

            // Matrices
            matrices = 0;
            fread(&matrices, sizeof(matrices), 1, f);
            for (unsigned long j = 0; j < matrices; j++) {
                D3DMATRIX m;

                // Name
                count = 0;
                fread(&count, sizeof(count), 1, f);
                if (count) {
                    tmp = new char[count];
                    fread(tmp, count, 1, f);
                    e.transformnames.push_back(wxString(tmp));
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
            lod.modelname = tmp;
            delete[] tmp;
        }

        fread(&lod.distance, sizeof(lod.distance), 1, f);
        fread(&lod.unk2, sizeof(lod.unk2), 1, f);
        fread(&lod.unk4, sizeof(lod.unk4), 1, f);
        fread(&lod.unk14, sizeof(lod.unk14), 1, f);

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
            references.push_back(wxString(tmp));
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
        models[0].file = tmp;
        wxFileName t = models[0].file;
        models[0].name = t.GetName();
    }
    delete[] tmp;

    unsigned long meshcount;
    fread(&meshcount, sizeof(meshcount), 1, f);
    models[0].meshstructs.clear();
    for (i = 0; i < meshcount; i++) {
        cMeshStruct ms;
        BOOL dis;
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
                ms.FTX = tmp;
                delete[] tmp;
            } else
                ms.FTX = "";

            unsigned long txslen;
            fread(&txslen, sizeof(txslen), 1, f);
            if (txslen) {
                tmp = new char[txslen];
                fread(tmp, txslen, 1, f);
                ms.TXS = tmp;
                delete[] tmp;
            } else
                ms.TXS = "";
        }
        ms.effectpoint = false;
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
            ft.Name = tmp;
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
            ftfr.texture(wxString(tmp));
            delete[] tmp;
        } else
            ftfr.texture(wxString(wxT("")));

        unsigned long alphalen;
        fread(&alphalen, sizeof(alphalen), 1, f);
        if (alphalen) {
            tmp = new char[alphalen];
            fread(tmp, alphalen, 1, f);
            ftfr.alpha(wxString(tmp));
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
            ref = tmp;
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
            e.name = tmp;
            delete[] tmp;
        } else
            e.name = "";

        D3DMATRIX m;
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
        e.transformnames.push_back("Custom Matrix (old scn)");

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
                models[0].transformnames.push_back("Fix Orientation (old scn)");
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
                    mname = tmp;
                    delete[] tmp;
                } else
                    mname = "";
                //tmpmeshlist.push_back(mname);
                models[0].meshstructs[i].Name = mname;
            }

            // Read transformations
            unsigned long matrices;
            fread(&matrices, sizeof(matrices), 1, f);
            for (unsigned long j = 0; j < matrices; j++) {
                D3DMATRIX m;
                wxString nam;

                unsigned long namelen;
                fread(&namelen, sizeof(namelen), 1, f);
                if (namelen) {
                    tmp = new char[namelen];
                    fread(tmp, namelen, 1, f);
                    nam = tmp;
                    delete[] tmp;
                } else
                    nam = "";
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
                    e.name = tmp;
                    delete[] tmp;
                } else
                    e.name = "";

                fread(&matrices, sizeof(matrices), 1, f);
                for (unsigned long j = 0; j < matrices; j++) {
                    D3DMATRIX m;
                    wxString nam;

                    fread(&namelen, sizeof(namelen), 1, f);
                    if (namelen) {
                        tmp = new char[namelen];
                        fread(tmp, namelen, 1, f);
                        nam = tmp;
                        delete[] tmp;
                    } else
                        nam = "";
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
/*
cMeshStruct cSCNFile::MakeMeshStruct(c3DLoader *obj, unsigned int j) {
    cMeshStruct ms;
    if ((strcmp(obj->GetObjectName(j).c_str(), "root") == 0)
            || (obj->GetObjectVertexCount(j) < 3)
            || (!obj->IsObjectValid(j))) {
        ms.disabled = true;
    } else {
        ms.disabled = false;
    }
    if (obj->GetObjectVertexCount(j) == 1) {
        ms.effectpoint = true;
        ms.effectpoint_vert = obj->GetObjectVertex(j, 0).position;
    } else {
        ms.effectpoint = false;
    }
    ms.Name = obj->GetObjectName(j);
    ms.flags = 0;
    ms.place = 0;
    ms.unknown = 3;
    ms.FTX = "";
    ms.TXS = "";
    return ms;
}

void cSCNFile::ResetAndReload(c3DLoader *obj) {
    models[0].meshstructs.clear();
    // Just load the new object into new mesh structs
    for (long j = 0; j < obj->GetObjectCount(); j++) {
        models[0].meshstructs.push_back(MakeMeshStruct(obj, j));
    }

}


void cSCNFile::Fixup(c3DLoader *obj) {
    std::vector<cMeshStruct> fixup;
    int msinfile = models[0].meshstructs.size();
    unsigned int i;

    error = CSCNFILE_ERROR_MESH_FIX;

    // First round of fixups
    int nrfound = 0;
    for (i = 0; i < obj->GetObjectCount(); i++) {
        bool found = false;
        for (cMeshStructIterator ms = models[0].meshstructs.begin(); ms != models[0].meshstructs.end(); ms++) {
            if (ms->Name == obj->GetObjectName(i)) {
                // Found!
                fixup.push_back(*ms);
                found = true;
                nrfound++;
                models[0].meshstructs.erase(ms);
                break;
            }
        }
        if (!found)
            fixup.push_back(MakeMeshStruct(obj, i));
    }

    // If the fixups were successful, meshstructs is empty
    if (models[0].meshstructs.size() == 0) {
        if (msinfile != obj->GetObjectCount())
            error |= CSCNFILE_ERROR_MESH_MISMATCH;
        models[0].meshstructs = fixup;
        return;
    }

    // Didn't work. Yay! -.-
    if (obj->GetObjectCount() == models[0].meshstructs.size() == msinfile) {
        // Special case: Nothing worked, but we have the same number of meshes/objects
        error |= CSCNFILE_ERROR_MESH_CONFUSION;
        for (i = 0; i < obj->GetObjectCount(); i++) {
            models[0].meshstructs[i].Name = obj->GetObjectName(i);
        }
        return;
    }
    if (nrfound == obj->GetObjectCount()) {
        // Special case: Meshes/Objects disappeared
        error |= CSCNFILE_ERROR_MESH_POOF;
        models[0].meshstructs.clear();
        models[0].meshstructs = fixup;
        return;
    }

    // Well, we can't call this a fix, but it should be the best
    error = CSCNFILE_ERROR_MESH_CONFUSION;
    models[0].meshstructs.clear();
    models[0].meshstructs = fixup;
    return;
}

bool cSCNFile::LoadObject(bool reset) {
    unsigned int i;
    c3DLoader *obj = c3DLoader::LoadFile(const_cast<char *>(models[0].file.GetFullPath().fn_str()));
    if (reset)
        models[0].meshstructs.clear();

    if (!obj) {
        error = CSCNFILE_ERROR_OBJNOTFOUND;
        if (reset)
            return false;
        if (tmpmeshlist.size()) {
            for (i = 0; i < models[0].meshstructs.size(); i++)
                models[0].meshstructs[i].Name = tmpmeshlist[i];
        } else {
            for (i = 0; i < models[0].meshstructs.size(); i++) {
                models[0].meshstructs[i].Name = wxString::Format("Mesh #%02d", i+1);
            }
        }
        return true;
    }

    if (reset) {
        ResetAndReload(obj);
    } else {
        if (obj->GetObjectCount() == models[0].meshstructs.size()) {
            if (tmpmeshlist.size()) {
                bool needsfix = false;
                for (i = 0; i < models[0].meshstructs.size(); i++) {
                    if (tmpmeshlist[i] != obj->GetObjectName(i))
                        needsfix = true;
                    models[0].meshstructs[i].Name = tmpmeshlist[i];
                    if (obj->GetObjectVertexCount(i) == 1) {
                        models[0].meshstructs[i].effectpoint = true;
                        models[0].meshstructs[i].effectpoint_vert = obj->GetObjectVertex(i, 0).position;
                    } else {
                        models[0].meshstructs[i].effectpoint = false;
                    }
                }
                if (needsfix) {
                    Fixup(obj);
                }
            } else {
                // This is the classic case
                for (i = 0; i < models[0].meshstructs.size(); i++) {
                    models[0].meshstructs[i].Name = obj->GetObjectName(i);
                    if (obj->GetObjectVertexCount(i) == 1) {
                        models[0].meshstructs[i].effectpoint = true;
                        models[0].meshstructs[i].effectpoint_vert = obj->GetObjectVertex(i, 0).position;
                    } else {
                        models[0].meshstructs[i].effectpoint = false;
                    }
                }
            }
        } else
            Fixup(obj);
    }

    delete obj;

    return true;
}
*/

/*
void cSCNFile::SaveTextures(FILE *f) {
    unsigned long texcount = flexitextures.size();
    fwrite(&texcount, sizeof(texcount), 1, f);
    wxFileName temp;
    for (unsigned long i = 0; i < texcount; i++) {
        cFlexiTexture ft = flexitextures[i];
        unsigned long namelen = ft.Name.length()+1;
        fwrite(&namelen, sizeof(namelen), 1, f);
        if (namelen) {
            fwrite(ft.Name.c_str(), namelen, 1, f);
        }

        fwrite(&ft.Recolorable, sizeof(ft.Recolorable), 1, f);
        fwrite(&ft.FPS, sizeof(ft.FPS), 1, f);

        unsigned long anim_count = 0;
        for(cFlexiTextureAnim::iterator it = ft.Animation.begin(); it != ft.Animation.end(); it++) {
            anim_count += it->count();
        }
        fwrite(&anim_count, sizeof(anim_count), 1, f);
        for(unsigned long j = 0; j < ft.Animation.size(); j++) {
            for(unsigned long k = 0; k < ft.Animation[j].count(); k++)
                fwrite(&ft.Animation[j].frame(), sizeof(unsigned long), 1, f);
        }

        unsigned long frame_count = ft.Frames.size();
        fwrite(&frame_count, sizeof(frame_count), 1, f);
        for(cFlexiTextureFrame::iterator it = ft.Frames.begin(); it != ft.Frames.end(); it++) {
            it->recolorable_write(f);

            temp = it->texture();
            if (temp != wxT("")) {
                if (save_relative_paths)
                    temp.MakeRelativeTo(filename.GetPath());
            }
            unsigned long texturelen = temp.GetFullPath().length()+1;
            fwrite(&texturelen, sizeof(texturelen), 1, f);
            if (texturelen) {
                fwrite(temp.GetFullPath().fn_str(), texturelen, 1, f);
            }

            temp = it->alpha();
            if (temp != wxT("")) {
                if (save_relative_paths)
                    temp.MakeRelativeTo(filename.GetPath());
            }
            unsigned long alphalen = temp.GetFullPath().length()+1;
            fwrite(&alphalen, sizeof(alphalen), 1, f);
            if (alphalen) {
                fwrite(temp.GetFullPath().fn_str(), alphalen, 1, f);
            }

            it->alphacutoff_write(f);
            it->alphasource_write(f);
        }
    }
}

void cSCNFile::SaveModels(FILE *f) {
    unsigned long modcount = models.size();
    fwrite(&modcount, sizeof(modcount), 1, f);
    wxFileName temp;
    unsigned long count;
    for (unsigned long m = 0; m < modcount; m++) {
        cModel *mod = &models[m];

        // Write model name
        count = mod->name.length()+1;
        fwrite(&count, sizeof(count), 1, f);
        fwrite(mod->name.c_str(), count, 1, f);

        // Write model filename
        temp = mod->file;
        if (temp != wxT("")) {
            if (save_relative_paths)
                temp.MakeRelativeTo(filename.GetPath());
        }
        count = temp.GetFullPath().length()+1;
        fwrite(&count, sizeof(count), 1, f);
        fwrite(temp.GetFullPath().fn_str(), count, 1, f);

        // Write transformations
        unsigned long matrices = mod->transforms.size();
        fwrite(&matrices, sizeof(matrices), 1, f);
        for (unsigned long j = 0; j < matrices; j++) {
            // Name
            unsigned long namelen = mod->transformnames[j].length()+1;
            fwrite(&namelen, sizeof(namelen), 1, f);
            if (namelen) {
                fwrite(mod->transformnames[j].c_str(), namelen, 1, f);
            }

            // Matrix
            D3DMATRIX m = mod->transforms[j];

            fwrite(&m._11, sizeof(m._11), 1, f);
            fwrite(&m._12, sizeof(m._12), 1, f);
            fwrite(&m._13, sizeof(m._13), 1, f);
            fwrite(&m._14, sizeof(m._14), 1, f);
            fwrite(&m._21, sizeof(m._21), 1, f);
            fwrite(&m._22, sizeof(m._22), 1, f);
            fwrite(&m._23, sizeof(m._23), 1, f);
            fwrite(&m._24, sizeof(m._24), 1, f);
            fwrite(&m._31, sizeof(m._31), 1, f);
            fwrite(&m._32, sizeof(m._32), 1, f);
            fwrite(&m._33, sizeof(m._33), 1, f);
            fwrite(&m._34, sizeof(m._34), 1, f);
            fwrite(&m._41, sizeof(m._41), 1, f);
            fwrite(&m._42, sizeof(m._42), 1, f);
            fwrite(&m._43, sizeof(m._43), 1, f);
            fwrite(&m._44, sizeof(m._44), 1, f);
        }

        // Write Meshes
        unsigned long meshcount = mod->meshstructs.size();
        fwrite(&meshcount, sizeof(meshcount), 1, f);
        for (unsigned long i = 0; i < meshcount; i++) {
            cMeshStruct ms = mod->meshstructs[i];

            // Name
            count = ms.Name.length()+1;
            fwrite(&count, sizeof(count), 1, f);
            fwrite(ms.Name.c_str(), count, 1, f);

            // Enabled or disabled
            BOOL dis = ms.disabled;
            fwrite(&dis, sizeof(BOOL), 1, f);

            fwrite(&ms.flags, sizeof(ms.flags), 1, f);
            fwrite(&ms.place, sizeof(ms.place), 1, f);
            fwrite(&ms.unknown, sizeof(ms.unknown), 1, f);

            unsigned long ftxlen = ms.FTX.length()+1;
            fwrite(&ftxlen, sizeof(ftxlen), 1, f);
            if (ftxlen) {
                fwrite(ms.FTX.c_str(), ftxlen, 1, f);
            }

            unsigned long txslen = ms.TXS.length()+1;
            fwrite(&txslen, sizeof(txslen), 1, f);
            if (txslen) {
                fwrite(ms.TXS.c_str(), txslen, 1, f);
            }
        }

        // Write effect point structures
        unsigned long ptcount = mod->effectpoints.size();
        fwrite(&ptcount, sizeof(ptcount), 1, f);
        for (unsigned long i = 0; i < ptcount; i++) {
            cEffectPoint e = mod->effectpoints[i];

            // Name
            unsigned long namelen = e.name.length()+1;
            fwrite(&namelen, sizeof(namelen), 1, f);
            if (namelen) {
                fwrite(e.name.c_str(), namelen, 1, f);
            }

            // Matrices
            matrices = e.transforms.size();
            fwrite(&matrices, sizeof(matrices), 1, f);
            for (unsigned long j = 0; j < matrices; j++) {
                D3DMATRIX m = e.transforms[j];

                // Name
                namelen = e.transformnames[j].length()+1;
                fwrite(&namelen, sizeof(namelen), 1, f);
                if (namelen) {
                    fwrite(e.transformnames[j].c_str(), namelen, 1, f);
                }

                // Matrix
                fwrite(&m._11, sizeof(m._11), 1, f);
                fwrite(&m._12, sizeof(m._12), 1, f);
                fwrite(&m._13, sizeof(m._13), 1, f);
                fwrite(&m._14, sizeof(m._14), 1, f);
                fwrite(&m._21, sizeof(m._21), 1, f);
                fwrite(&m._22, sizeof(m._22), 1, f);
                fwrite(&m._23, sizeof(m._23), 1, f);
                fwrite(&m._24, sizeof(m._24), 1, f);
                fwrite(&m._31, sizeof(m._31), 1, f);
                fwrite(&m._32, sizeof(m._32), 1, f);
                fwrite(&m._33, sizeof(m._33), 1, f);
                fwrite(&m._34, sizeof(m._34), 1, f);
                fwrite(&m._41, sizeof(m._41), 1, f);
                fwrite(&m._42, sizeof(m._42), 1, f);
                fwrite(&m._43, sizeof(m._43), 1, f);
                fwrite(&m._44, sizeof(m._44), 1, f);
            }

        }

    }
}

void cSCNFile::SaveLODs(FILE *f) {
    unsigned long lodcount = lods.size();
    fwrite(&lodcount, sizeof(lodcount), 1, f);
    for (unsigned long m = 0; m < lodcount; m++) {
        cLOD *lod = &lods[m];

        // Write model name
        unsigned long count = lod->modelname.length()+1;
        fwrite(&count, sizeof(count), 1, f);
        fwrite(lod->modelname.c_str(), count, 1, f);

        fwrite(&lod->distance, sizeof(lod->distance), 1, f);
        fwrite(&lod->unk2, sizeof(lod->unk2), 1, f);
        fwrite(&lod->unk4, sizeof(lod->unk4), 1, f);
        fwrite(&lod->unk14, sizeof(lod->unk14), 1, f);
    }
}

void cSCNFile::SaveReferences(FILE *f) {
    unsigned long refcount = references.size();
    fwrite(&refcount, sizeof(refcount), 1, f);
    for (unsigned long i = 0; i < refcount; i++) {
        unsigned long reflen = references[i].length()+1;
        fwrite(&reflen, sizeof(reflen), 1, f);
        if (reflen) {
            fwrite(references[i].c_str(), reflen, 1, f);
        }
    }
}
*/

bool cSCNFile::Save() {
//    unsigned long i;
//    error = CSCNFILE_NO_ERROR;

    if (filename == "") {
        throw RCT3Exception(_("Saving failed, file name not set."));
    }
/*
    FILE *f = fopen(filename.GetFullPath().fn_str(), "wb");
    if (!f) {
        error = CSCNFILE_ERROR_FILENAME;
        return false;
    }

    version = VERSION_CSCNFILE;

    // Write unsigned int max to allow discrimination between old and new files
    long newtag = -1;
    fwrite(&newtag, sizeof(newtag), 1, f);

    // Write magic
    char scnmagic[8] = CSCNFILE_MAGIC;
    fwrite(&scnmagic, 8, 1, f);

    // Write version
    fwrite(&version, sizeof(long), 1, f);

    // Write ovl name
    wxFileName temp = ovlpath;
    temp.SetName(name);
    if (save_relative_paths)
        temp.MakeRelativeTo(filename.GetPath());
    unsigned long count = temp.GetFullPath().length()+1;
    fwrite(&count, sizeof(count), 1, f);
    fwrite(temp.GetFullPath().fn_str(), count, 1, f);

    // Write sivsettings
    fwrite(&sivsettings.sivflags, sizeof(unsigned int), 1, f);
    fwrite(&sivsettings.sway, sizeof(float), 1, f);
    fwrite(&sivsettings.brightness, sizeof(float), 1, f);
    fwrite(&sivsettings.unknown, sizeof(float), 1, f);
    fwrite(&sivsettings.scale, sizeof(float), 1, f);
    fwrite(&sivsettings.unk6, sizeof(unsigned long), 1, f);
    fwrite(&sivsettings.unk7, sizeof(unsigned long), 1, f);
    fwrite(&sivsettings.unk8, sizeof(unsigned long), 1, f);
    fwrite(&sivsettings.unk9, sizeof(unsigned long), 1, f);
    fwrite(&sivsettings.unk10, sizeof(unsigned long), 1, f);
    fwrite(&sivsettings.unk11, sizeof(unsigned long), 1, f);

    SaveTextures(f);
    SaveModels(f);
    SaveLODs(f);
    SaveReferences(f);

    fclose(f);
*/
    SaveXML();

    return true;

    /*
        unsigned long i;
        error = CSCNFILE_NO_ERROR;

        if (filename == "") {
            error = CSCNFILE_ERROR_FILENAME;
            return false;
        }

        FILE *f = fopen(filename.GetFullPath().fn_str(), "wb");
        if (!f) {
            error = CSCNFILE_ERROR_FILENAME;
            return false;
        }

        version = VERSION_CSCNFILE;

        unsigned long objlen = objfile.length()+1;
        fwrite(&objlen, sizeof(objlen), 1, f);
        fwrite(objfile.c_str(), objlen, 1, f);

        unsigned long meshcount = meshstructs.size();
        fwrite(&meshcount, sizeof(meshcount), 1, f);
        for (i = 0; i < meshcount; i++) {
            cMeshStruct ms = meshstructs[i];
            BOOL dis = ms.disabled;
            fwrite(&dis, sizeof(BOOL), 1, f);
            if (ms.disabled == false) {
                fwrite(&ms.flags, sizeof(ms.flags), 1, f);
                fwrite(&ms.place, sizeof(ms.place), 1, f);
                fwrite(&ms.unknown, sizeof(ms.unknown), 1, f);

                unsigned long ftxlen = ms.FTX.length()+1;
                fwrite(&ftxlen, sizeof(ftxlen), 1, f);
                if (ftxlen) {
                    fwrite(ms.FTX.c_str(), ftxlen, 1, f);
                }

                unsigned long txslen = ms.TXS.length()+1;
                fwrite(&txslen, sizeof(txslen), 1, f);
                if (txslen) {
                    fwrite(ms.TXS.c_str(), txslen, 1, f);
                }
            }
        }

        unsigned long texcount = flexitextures.size();
        fwrite(&texcount, sizeof(texcount), 1, f);
        for (i = 0; i < texcount; i++) {
            cFlexiTexture ft = flexitextures[i];
            fwrite(&ft.Recolorable, sizeof(ft.Recolorable), 1, f);

            unsigned long namelen = ft.Name.length()+1;
            fwrite(&namelen, sizeof(namelen), 1, f);
            if (namelen) {
                fwrite(ft.Name.c_str(), namelen, 1, f);
            }

            unsigned long texturelen = ft.Texture.length()+1;
            fwrite(&texturelen, sizeof(texturelen), 1, f);
            if (texturelen) {
                fwrite(ft.Texture.c_str(), texturelen, 1, f);
            }

            unsigned long alphalen = ft.Alpha.length()+1;
            fwrite(&alphalen, sizeof(alphalen), 1, f);
            if (alphalen) {
                fwrite(ft.Alpha.c_str(), alphalen, 1, f);
            }
        }

        unsigned long refcount = references.size();
        fwrite(&refcount, sizeof(refcount), 1, f);
        for (i = 0; i < refcount; i++) {
            unsigned long reflen = references[i].length()+1;
            fwrite(&reflen, sizeof(reflen), 1, f);
            if (reflen) {
                fwrite(references[i].c_str(), reflen, 1, f);
            }
        }

        // Old effectpoints store
        unsigned long ptcount = 0;
        fwrite(&ptcount, sizeof(ptcount), 1, f);

        fwrite(&version, sizeof(long), 1, f);

        // SCN Verison 1 Stuff
        long fixorient = 0;
        fwrite(&fixorient, sizeof(long), 1, f);

        // SCN Version 2 Stuff
        fwrite(&sivsettings.sivflags, sizeof(unsigned int), 1, f);
        fwrite(&sivsettings.sway, sizeof(float), 1, f);
        fwrite(&sivsettings.brightness, sizeof(float), 1, f);
        fwrite(&sivsettings.unknown, sizeof(float), 1, f);
        fwrite(&sivsettings.scale, sizeof(float), 1, f);

        // SCN Version 3 Stuff
        // Write Mesh names
        for (i = 0; i < meshcount; i++) {
            unsigned long namelen = meshstructs[i].Name.length()+1;
            fwrite(&namelen, sizeof(namelen), 1, f);
            if (namelen) {
                fwrite(meshstructs[i].Name.c_str(), namelen, 1, f);
            }
        }

        // Write transformations
        unsigned long matrices = transforms.size();
        fwrite(&matrices, sizeof(matrices), 1, f);
        for (unsigned long j = 0; j < matrices; j++) {
            unsigned long namelen = transformnames[j].length()+1;
            fwrite(&namelen, sizeof(namelen), 1, f);
            if (namelen) {
                fwrite(transformnames[j].c_str(), namelen, 1, f);
            }

            D3DMATRIX m = transforms[j];

            fwrite(&m._11, sizeof(m._11), 1, f);
            fwrite(&m._12, sizeof(m._12), 1, f);
            fwrite(&m._13, sizeof(m._13), 1, f);
            fwrite(&m._14, sizeof(m._14), 1, f);
            fwrite(&m._21, sizeof(m._21), 1, f);
            fwrite(&m._22, sizeof(m._22), 1, f);
            fwrite(&m._23, sizeof(m._23), 1, f);
            fwrite(&m._24, sizeof(m._24), 1, f);
            fwrite(&m._31, sizeof(m._31), 1, f);
            fwrite(&m._32, sizeof(m._32), 1, f);
            fwrite(&m._33, sizeof(m._33), 1, f);
            fwrite(&m._34, sizeof(m._34), 1, f);
            fwrite(&m._41, sizeof(m._41), 1, f);
            fwrite(&m._42, sizeof(m._42), 1, f);
            fwrite(&m._43, sizeof(m._43), 1, f);
            fwrite(&m._44, sizeof(m._44), 1, f);
        }

        // Write new effect point structures
        ptcount = effectpoints.size();
        fwrite(&ptcount, sizeof(ptcount), 1, f);
        for (i = 0; i < ptcount; i++) {
            cEffectPoint e = effectpoints[i];

            unsigned long namelen = e.Name.length()+1;
            fwrite(&namelen, sizeof(namelen), 1, f);
            if (namelen) {
                fwrite(e.Name.c_str(), namelen, 1, f);
            }

            matrices = e.Transform.size();
            fwrite(&matrices, sizeof(matrices), 1, f);
            for (unsigned long j = 0; j < matrices; j++) {
                D3DMATRIX m = e.Transform[j];

                namelen = e.TransformNames[j].length()+1;
                fwrite(&namelen, sizeof(namelen), 1, f);
                if (namelen) {
                    fwrite(e.TransformNames[j].c_str(), namelen, 1, f);
                }

                fwrite(&m._11, sizeof(m._11), 1, f);
                fwrite(&m._12, sizeof(m._12), 1, f);
                fwrite(&m._13, sizeof(m._13), 1, f);
                fwrite(&m._14, sizeof(m._14), 1, f);
                fwrite(&m._21, sizeof(m._21), 1, f);
                fwrite(&m._22, sizeof(m._22), 1, f);
                fwrite(&m._23, sizeof(m._23), 1, f);
                fwrite(&m._24, sizeof(m._24), 1, f);
                fwrite(&m._31, sizeof(m._31), 1, f);
                fwrite(&m._32, sizeof(m._32), 1, f);
                fwrite(&m._33, sizeof(m._33), 1, f);
                fwrite(&m._34, sizeof(m._34), 1, f);
                fwrite(&m._41, sizeof(m._41), 1, f);
                fwrite(&m._42, sizeof(m._42), 1, f);
                fwrite(&m._43, sizeof(m._43), 1, f);
                fwrite(&m._44, sizeof(m._44), 1, f);
            }

        }

        fclose(f);
        return true;
    */
}

void cSCNFile::SaveXML() {
    wxString xmlfile = filename.GetFullPath();
    //xmlfile += wxT(".xml");
    wxXmlDocument doc;
    doc.SetFileEncoding(wxT("UTF-8"));
    doc.SetRoot(GetNode(filename.GetPath()));
    doc.Save(xmlfile, 2);
}

bool cSCNFile::LoadXML(wxXmlDocument* doc) {
    wxString temp;
/*
    wxXmlDocument doc;
    if (!doc.Load(filename.GetFullPath()))
        throw RCT3Exception(wxString::Format(_("Error loading xml file '%s'."), filename.GetFullPath().fn_str()));
*/
    // start processing the XML file
    wxXmlNode* root = doc->GetRoot();
    if (root->GetName() != RCT3XML_CSCNFILE) {
        throw RCT3Exception(wxString::Format(_("Error loading xml file '%s'. Wrong root tag. Probably you tried to load a xml file made for a different purpose."), filename.GetFullPath().fn_str()));
    }

    wxString path = filename.GetPath();

    // Peak at version to detect ovlcompiler xml
    if (root->GetPropVal(wxT("version"), &temp)) {
        return FromNode(root, path, 0);
    } else {
        version = VERSION_CSCNFILE_FIRSTXML;
        name = filename.GetName();
        return FromCompilerXml(root, path);
    }

}

#define COMPILER_BSH wxT("bsh")
#define COMPILER_BAN wxT("ban")
#define COMPILER_FTX wxT("ftx")
#define COMPILER_FIX wxT("fix")
#define COMPILER_REFERENCE wxT("reference")
#define COMPILER_OPTIONS wxT("options")

bool cSCNFile::FromCompilerXml(wxXmlNode* node, const wxString& path) {
    wxXmlNode *child = node->GetChildren();
    c3DLoaderOrientation ori = ORIENTATION_UNKNOWN;
    bool ret = true;
    int option_lods = 1;

    name = node->GetPropVal(wxT("name"), name);

    // Read ovl file
    ovlpath = node->GetPropVal(wxT("file"), wxT(".\\"));
    if (!ovlpath.IsAbsolute()) {
        ovlpath.MakeAbsolute(path);
    }

    while (child) {

        if (child->GetName() == COMPILER_BSH) {
            cAnimatedModel nmodel;
            if (!nmodel.FromCompilerXml(child, path))
                ret = false;
            animatedmodels.push_back(nmodel);
        } else if (child->GetName() == COMPILER_BAN) {
            cAnimation anim;
            if (!anim.FromCompilerXml(child, path))
                ret = false;
            animations.push_back(anim);
        } else if (child->GetName() == COMPILER_FTX) {
            cFlexiTexture ftx;
            if (!ftx.FromCompilerXml(child, path))
                ret = false;
            flexitextures.push_back(ftx);
        } else if (child->GetName() == COMPILER_FIX) {
            if (ori != ORIENTATION_UNKNOWN) {
                throw RCT3Exception(_("Second fix tag found."));
            }
            wxString hand = child->GetPropVal(wxT("handedness"), wxT("left"));
            hand.MakeLower();
            wxString up = child->GetPropVal(wxT("up"), wxT("y"));
            up.MakeLower();
            if (hand == wxT("left")) {
                if (up == wxT("x")) {
                    ori = ORIENTATION_LEFT_XUP;
                } else if (up == wxT("y")) {
                    ori = ORIENTATION_LEFT_YUP;
                } else if (up == wxT("z")) {
                    ori = ORIENTATION_LEFT_ZUP;
                } else {
                    throw RCT3Exception(wxString::Format(_("Unknown value '%s' for up attribute in fix tag."), up.c_str()));
                }
            } else if (hand == wxT("right")) {
                if (up == wxT("x")) {
                    ori = ORIENTATION_RIGHT_XUP;
                } else if (up == wxT("y")) {
                    ori = ORIENTATION_RIGHT_YUP;
                } else if (up == wxT("z")) {
                    ori = ORIENTATION_RIGHT_ZUP;
                } else {
                    throw RCT3Exception(wxString::Format(_("Unknown value '%s' for up attribute in fix tag."), up.c_str()));
                }
            } else {
                throw RCT3Exception(wxString::Format(_("Unknown value '%s' for handedness attribute in fix tag."), hand.c_str()));
            }
            wxLogDebug(wxT("Fix tag found: %s %s (%d)"), hand.c_str(), up.c_str(), static_cast<unsigned int>(ori));
        } else if (child->GetName() == COMPILER_REFERENCE) {
            wxString ref = child->GetPropVal(wxT("path"), wxT(""));
            if (ref.IsEmpty())
                throw RCT3Exception(_("REFERENCE tag misses path attribute."));
            references.Add(ref);
        } else if (child->GetName() == COMPILER_OPTIONS) {
            wxString te = child->GetPropVal(wxT("lods"), wxT("1"));
            if (te == wxT("1")) {
                option_lods = 1;
            } else if (te == wxT("3")) {
                option_lods = 3;
            } else if (te == wxT("4")) {
                option_lods = 4;
            } else {
                throw RCT3Exception(wxString::Format(_("Unknown lod option '%s'."), te.c_str()));
            }
        } else if COMPILER_WRONGTAG(child) {
            throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in ovl tag."), child->GetName().c_str()));
        }

        child = child->GetNext();
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
        if (animations.size())
            lod.animations.Add(animations[0].name);
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
                            cModel sm = cModel(animatedmodels[2]);
                            sm.name += wxT("ULLOD");
                        } else {
                            cModel sm = cModel(animatedmodels[3]);
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
    } else {
        throw RCT3Exception(_("No bsh tags found."));
    }

    filename = wxT(""); // Destroy filename to prevent overwriting

    return ret;
}

#define CSCNFILE_READVECTOR(vec, itype, itag, tag) \
    } else if (child->GetName() == tag) { \
        wxXmlNode* subchild = child->GetChildren(); \
        while (subchild) { \
            if (subchild->GetName() == itag) { \
                itype item; \
                if (!item.FromNode(subchild, path, version)) \
                    ret = false; \
                vec.push_back(item); \
            } \
            subchild = subchild->GetNext(); \
        }

bool cSCNFile::FromNode(wxXmlNode* node, const wxString& path, unsigned long version) {
    bool ret = true;
    wxString temp;
    if (!node)
        return false;
    if (!node->GetName().IsSameAs(RCT3XML_CSCNFILE))
        return false;

    name = node->GetPropVal(wxT("name"), wxT(""));

    // Read ovl file
    ovlpath = node->GetPropVal(wxT("file"), wxT(".\\"));
    if (!ovlpath.IsAbsolute()) {
        ovlpath.MakeAbsolute(path);
    }

    // Read version
    if (node->GetPropVal(wxT("version"), &temp)) {
        if (!temp.ToULong(&version)) {
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


    wxXmlNode *child = node->GetChildren();
    while (child) {

        if (child->GetName() == RCT3XML_CSIVSETTINGS) {
            if (!sivsettings.FromNode(child, path, version))
                ret = false;
        CSCNFILE_READVECTOR(flexitextures, cFlexiTexture, RCT3XML_CFLEXITEXTURE, RCT3XML_CSCNFILE_FLEXITEXTURES)
        CSCNFILE_READVECTOR(models, cModel, RCT3XML_CMODEL, RCT3XML_CSCNFILE_MODELS)
        CSCNFILE_READVECTOR(animatedmodels, cAnimatedModel, RCT3XML_CANIMATEDMODEL, RCT3XML_CSCNFILE_ANIMATEDMODELS)
        CSCNFILE_READVECTOR(animations, cAnimation, RCT3XML_CANIMATION, RCT3XML_CSCNFILE_ANIMATIONS)
        CSCNFILE_READVECTOR(lods, cLOD, RCT3XML_CLOD, RCT3XML_CSCNFILE_LODS)
        } else if (child->GetName() == RCT3XML_CSCNFILE_REFERENCES) {
            wxXmlNode* subchild = child->GetChildren();
            while (subchild) {
                if (subchild->GetName() == RCT3XML_REFERENCE) {
                    wxString ref = subchild->GetPropVal(wxT("path"), wxT(""));
                    if (!ref.IsEmpty())
                        references.push_back(ref);
                }
                subchild = subchild->GetNext();
            }
        }

        child = child->GetNext();
    }
    return ret;
}

//    parent = new wxXmlNode(node, wxXML_ELEMENT_NODE, tag);
//        newchild->SetParent(parent);

#define CSCNFILE_WRITEVECTOR(vec, iter, tag) \
    parent = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, tag); \
    for (iter it = vec.begin(); it != vec.end(); it++) { \
        wxXmlNode* newchild = it->GetNode(path); \
        if (lastchild) \
            lastchild->SetNext(newchild); \
        else \
            parent->SetChildren(newchild); \
        lastchild = newchild; \
    } \
    lastparent->SetNext(parent); \
    lastparent = parent; \
    lastchild = NULL;

wxXmlNode* cSCNFile::GetNode(const wxString& path) {
    wxXmlNode* node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, RCT3XML_CSCNFILE);
    wxXmlNode* lastchild = NULL;
    wxXmlNode* lastparent = NULL;

    node->AddProperty(wxT("name"), name);
    wxFileName temp = ovlpath;
    temp.MakeRelativeTo(path);
    node->AddProperty(wxT("file"), temp.GetPathWithSep());
    node->AddProperty(wxT("version"), wxString::Format("%lu", version));

    lastparent = sivsettings.GetNode(path);
    //lastparent->SetParent(node);
    node->SetChildren(lastparent);

    wxXmlNode* parent;

    CSCNFILE_WRITEVECTOR(flexitextures, cFlexiTexture::iterator, RCT3XML_CSCNFILE_FLEXITEXTURES)
    CSCNFILE_WRITEVECTOR(models, cModel::iterator, RCT3XML_CSCNFILE_MODELS)
    CSCNFILE_WRITEVECTOR(animatedmodels, cAnimatedModel::iterator, RCT3XML_CSCNFILE_ANIMATEDMODELS)
    CSCNFILE_WRITEVECTOR(animations, cAnimation::iterator, RCT3XML_CSCNFILE_ANIMATIONS)
    CSCNFILE_WRITEVECTOR(lods, cLOD::iterator, RCT3XML_CSCNFILE_LODS)

    //parent = new wxXmlNode(node, wxXML_ELEMENT_NODE, RCT3XML_CSCNFILE_REFERENCES);
    parent = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, RCT3XML_CSCNFILE_REFERENCES);
    for (cStringIterator it = references.begin(); it != references.end(); it++) {
        //wxXmlNode* newchild = new wxXmlNode(parent, wxXML_ELEMENT_NODE, RCT3XML_REFERENCE);
        wxXmlNode* newchild = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, RCT3XML_REFERENCE);
        newchild->AddProperty(wxT("path"), *it);
        if (lastchild)
            lastchild->SetNext(newchild);
        else
            parent->SetChildren(newchild);
        lastchild = newchild;
    }
    lastparent->SetNext(parent);

    return node;
}

bool cSCNFile::Check() {
    wxLogDebug(wxT("Trace, cSCNFile::Check"));
    bool warning = false;
    CleanWork();

    m_work = new cSCNFile(*this);


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
                    wxLogWarning(_("You didn't set any levels of detail."));
                    warning = true;
                } else {
                    throw RCT3Exception(_("You didn't set any levels of detail!"));
                }
            }

            // Check the models
            if ((!m_work->models.size()) && (!m_work->animatedmodels.size())) {
                if (READ_RCT3_EXPERTMODE()) {
                    wxLogWarning(_("You didn't add any models."));
                    warning = true;
                } else {
                    throw RCT3Exception(_("You didn't add any models!"));
                }
            }

            // Check for duplicate names
            if (!CheckForModelNameDuplicates()) {
                throw RCT3Exception(_("You have a duplicate model name. Model names have to be unique, even between static and animated models."));
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
                    if (mod->fatal_error) {
                        throw RCT3Exception(wxString::Format(_("Level of Detail '%s' (%.1f): Error in corresponding model."), i_lod->modelname.c_str(), i_lod->distance));
                    }
                    mod->used = true;
                    if (i_lod->animated) {
                        i_lod->animated = false;
                        i_lod->animations.clear();
                        warning = true;
                        wxLogWarning(_("Level of Detail '%s' (%.1f): Static model was marked as animated."), i_lod->modelname.c_str(), i_lod->distance);
                    }
                } else if (amod) {
                    if (amod->fatal_error) {
                        throw RCT3Exception(wxString::Format(_("Level of Detail '%s' (%.1f): Error in corresponding animated model."), i_lod->modelname.c_str(), i_lod->distance));
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
                                wxLogWarning(_("Level of Detail '%s' (%.1f): Assigned animation '%s'missing."), i_lod->modelname.c_str(), i_lod->distance, i_lod->animations[s].c_str());
                            } else {
                                wxLogWarning(_("Level of Detail '%s' (%.1f): Assigned animation '%s'missing. Removed."), i_lod->modelname.c_str(), i_lod->distance, i_lod->animations[s].c_str());
                                i_lod->animations.erase(i_lod->animations.begin()+s);
                            }
                        }
                    }
                    if (!i_lod->animations.size()) {
                        warning = true;
                        wxLogWarning(_("Level of Detail '%s' (%.1f): Animated model used but no animations assigned."), i_lod->modelname.c_str(), i_lod->distance);
                    }
                } else {
                    if (READ_RCT3_EXPERTMODE()) {
                        wxLogWarning(_("Level of Detail '%s' (%.1f): Corresponding model missing."), i_lod->modelname.c_str(), i_lod->distance);
                    } else {
                        throw RCT3Exception(wxString::Format(_("Level of Detail '%s' (%.1f): Corresponding model missing."), i_lod->modelname.c_str(), i_lod->distance));
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

//            if (!i_ftx->Frames.size()) {
//                // No texture frames added.
//                throw RCT3Exception(wxString::Format(_("Texture '%s': No texture frames defined."), i_ftx->Name.c_str()));
//            }
//
//            // The validators should make these checks unnecessary, but better safe than sorry
//            for (cFlexiTextureFrame::iterator i_ftxfr = i_ftx->Frames.begin(); i_ftxfr != i_ftx->Frames.end(); i_ftxfr++) {
//                try {
//                    checkRCT3Texture(i_ftxfr->texture().GetFullPath());
//                } catch (RCT3TextureException& e) {
//                    throw RCT3Exception(wxString::Format(_("Texture '%s', file '%s': %s"), i_ftx->Name.c_str(), i_ftxfr->texture().GetFullPath().fn_str(), e.what()));
//                }
//
//                if (i_ftxfr->alphasource() == CFTF_ALPHA_EXTERNAL) {
//                    try {
//                        checkRCT3Texture(i_ftxfr->alpha().GetFullPath());
//                    } catch (RCT3TextureException& e) {
//                        i_ftxfr->alphasource(CFTF_ALPHA_NONE);
//                        warning = true;
//                        wxLogWarning(_("Texture '%s', file '%s': Problem with alpha channel file. Alpha deactivated. (Problem was %s)"), i_ftx->Name.c_str(), i_ftxfr->alpha().GetFullPath().fn_str(), e.what());
//                    }
//                } else if (i_ftxfr->alphasource() == CFTF_ALPHA_INTERNAL) {
//                    wxGXImage img(i_ftxfr->texture().GetFullPath());
//                    if (!img.HasAlpha()) {
//                        i_ftxfr->alphasource(CFTF_ALPHA_NONE);
//                        warning = true;
//                        wxLogWarning(_("Texture '%s', file '%s': No alpha channel in file. Alpha deactivated."), i_ftx->Name.c_str(), i_ftxfr->texture().GetFullPath().fn_str());
//                    }
//                }
//
//                // Make sure recolorable flag matches
//                i_ftxfr->recolorable(i_ftx->Recolorable);
//                // Init for next step
//                i_ftxfr->used = false;
//            }
//
//            // Check the animation
//            if (i_ftx->Animation.size()) {
//                for (unsigned long i = 0; i < i_ftx->Animation.size(); i++) {
//                    if (i_ftx->Animation[i].frame() >= i_ftx->Frames.size()) {
//                        // Illegal reference
//                        throw RCT3Exception(wxString::Format(_("Texture '%s': Animation step %d references non-existing frame."), i_ftx->Name.c_str(), i+1));
//                    }
//                    i_ftx->Frames[i_ftx->Animation[i].frame()].used = true;
//                }
//            } else {
//                // We need to add one refering to the first frame
//                cFlexiTextureAnim an(0);
//                i_ftx->Animation.push_back(an);
//                i_ftx->Frames[0].used = true;
//            }
//
//
//            // Now we go through the frames again and fix unreferenced ones
//            // Doing this inverse to make removing easier
//            for (long i = i_ftx->Frames.size() - 1; i >=0 ; i--) {
//                if (!i_ftx->Frames[i].used) {
//                    // Great -.-
//                    i_ftx->Frames.erase(i_ftx->Frames.begin() + i);
//                    // Fix animation
//                    for (cFlexiTextureAnim::iterator i_anim = i_ftx->Animation.begin(); i_anim != i_ftx->Animation.end(); i_anim++) {
//                        if (i_anim->frame() > i)
//                            i_anim->frame(i_anim->frame() - 1);
//                    }
//                    warning = true;
//                    wxLogWarning(_("Texture '%s', frame %d: Unused. It will not be written to the ovl file."), i_ftx->Name.c_str(), i);
//                }
//            }
//
//            // Zero FPS if there is only one animation step
//            if (i_ftx->Animation.size()<=1) {
//                i_ftx->FPS = 0;
//            }
        }

    }
    return !warning;
}

void cSCNFile::Make() {
    if (name.IsEmpty()) {
        throw RCT3Exception(_("OVL Name not set."));
    }

    if (!m_work)
        Check();

    try {
        wxFileName ovlfile = ovlpath;
        ovlfile.SetName(name);
        cOvl c_ovl(ovlfile.GetFullPath().fn_str());
        MakeToOvl(c_ovl);
        c_ovl.Save();
    } catch (EOvl& e) {
        throw RCT3Exception(wxString::Format(_("Error during OVL creation: %s"), e.what()));
    } catch (Magick::Exception& e) {
        throw RCT3Exception(wxString::Format(_("Error from image library: %s"), e.what()));
    }
}

void cSCNFile::MakeToOvl(cOvl& c_ovl) {
    if (!m_work)
        Check();

        // References
        for (cStringIterator it = references.begin(); it != references.end(); ++it) {
            c_ovl.AddReference(it->c_str());
        }

        // SVD, shapes & animations
        if (!m_textureovl) {
            if (m_work->lods.size()) {
                // SVD
                wxLogDebug(wxT("TRACE cSCNFile::MakeToOvl SVD"));
                ovlSVDManager* c_svd = c_ovl.GetManager<ovlSVDManager>();
                c_svd->AddSVD(name.c_str(), m_work->lods.size(), sivsettings.sivflags, sivsettings.sway, sivsettings.brightness, sivsettings.scale);
                c_svd->SetSVDUnknowns(sivsettings.unknown, sivsettings.unk6, sivsettings.unk7, sivsettings.unk8, sivsettings.unk9, sivsettings.unk10, sivsettings.unk11);
                for (cLOD::iterator it = m_work->lods.begin(); it != m_work->lods.end(); ++it) {
                    if (it->animated) {
                        c_svd->OpenAnimatedLOD(it->modelname.c_str(), it->modelname.c_str(), it->animations.size(), it->distance, it->unk2, it->unk4, it->unk14);
                        if (it->animations.size()) {
                            for (cStringIterator its = it->animations.begin(); its != it->animations.end(); ++its) {
                                c_svd->AddAnimation(its->c_str());
                            }
                        }
                        c_svd->CloseAnimatedLOD();
                    } else {
                        c_svd->AddStaticLOD(it->modelname.c_str(), it->modelname.c_str(), it->distance, it->unk2, it->unk4, it->unk14);
                    }
                }
            }

            // Static shapes
            if (m_work->models.size()) {
                wxLogDebug(wxT("TRACE cSCNFile::MakeToOvl SHS"));
                ovlSHSManager* c_shs = c_ovl.GetManager<ovlSHSManager>();
                for (cModel::iterator i_mod = m_work->models.begin(); i_mod != m_work->models.end(); ++i_mod) {
                    if (!i_mod->used)
                        continue;

                    cStaticShape1 c_mod;

                    boundsInit(&c_mod.bbox1, &c_mod.bbox2);

                    // Find active mesh count
//                    int mesh_count = 0;
//                    for (cMeshStruct::iterator i_mesh = i_mod->meshstructs.begin(); i_mesh != i_mod->meshstructs.end(); i_mesh++) {
//                        if (!i_mesh->disabled)
//                            mesh_count++;
//                    }
                    c_mod.name = i_mod->name.c_str();

                    // Determine transformation matrices
                    D3DMATRIX transformMatrix = matrixMultiply(matrixMultiply(i_mod->transforms), matrixGetFixOrientation(i_mod->usedorientation));
                    bool do_transform = !matrixIsEqual(transformMatrix, matrixGetUnity());
                    D3DMATRIX undoDamage;
                    int m = 0;
                    for(m = i_mod->transformnames.size()-1; m>=0; m--) {
                        if (i_mod->transformnames[m] == wxT("-"))
                            break;
                    }
                    if (m>0) {
                        // Note: if m=0, the first is the separator, so this can fall through to no separator
                        std::vector<D3DMATRIX> undostack;
                        for (int n = m; n < i_mod->transforms.size(); n++)
                            undostack.push_back(i_mod->transforms[n]);
                        undostack.push_back(matrixGetFixOrientation(i_mod->usedorientation));
                        undoDamage = matrixInverse(matrixMultiply(undostack));
                    } else {
                        undoDamage = matrixInverse(transformMatrix);
                    }
                    // We need to reevaluate do_transform
                    if (!do_transform)
                        do_transform = !matrixIsEqual(undoDamage, matrixGetUnity());

                    // Do effect points
                    if (i_mod->effectpoints.size() != 0) {
                        for (unsigned int e = 0; e < i_mod->effectpoints.size(); e++) {
                            cEffectStruct c_es;
                            c_es.name = i_mod->effectpoints[e].name.c_str();
                            if (do_transform) {
                                std::vector<D3DMATRIX> tempstack = i_mod->effectpoints[e].transforms;
                                // to correctely apply the model transformation matrix to effect points we have to
                                // transform in-game objects into modeler-space first by applying the inverse
                                // of the model transformation matrix
                                // (now replaced by the undoDamage matrix to allow separation of the coordinate
                                // system fix from other model transformations)
                                tempstack.insert(tempstack.begin(), undoDamage);
                                // At the end of the stack, transform back
                                tempstack.push_back(transformMatrix);
                                c_es.pos = matrixMultiply(tempstack);
                            } else {
                                c_es.pos = matrixMultiply(i_mod->effectpoints[e].transforms);
                            }
                            c_mod.effects.push_back(c_es);
                        }
                    }

                    c3DLoader *object = c3DLoader::LoadFile(i_mod->file.GetFullPath().fn_str());
                    if (!object) {
                        // Poof went the model!
                        throw RCT3Exception(wxString::Format(_("Something happened to the file of model '%s'."), i_mod->name.c_str()));
                    }

                    unsigned long CurrentObj = 0;
                    D3DVECTOR temp_min, temp_max;
                    for (cMeshStruct::iterator i_mesh = i_mod->meshstructs.begin(); i_mesh != i_mod->meshstructs.end(); i_mesh++) {
                        if (i_mesh->disabled == false) {
                            //progress.Update(++progress_count);
                            cStaticShape2 c_ss2;
                            c_ss2.fts = i_mesh->FTX.c_str();
                            c_ss2.texturestyle = i_mesh->TXS.c_str();
                            c_ss2.placetexturing = i_mesh->place;
                            c_ss2.textureflags = i_mesh->flags;
                            c_ss2.sides = i_mesh->unknown;

                            /*
                            unsigned long c_vertexcount;
                            VERTEX* c_vertices;
                            unsigned long c_indexcount;
                            unsigned long* c_indices;
                            */
                            D3DVECTOR c_fudge_normal;
                            D3DVECTOR* c_pfudge_normal = NULL;
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
                                                const_cast<D3DMATRIX *> ((do_transform)?(&transformMatrix):NULL),
                                                c_pfudge_normal);
                            boundsContain(&temp_min, &temp_max, &c_mod.bbox1, &c_mod.bbox2);
                            if (i_mesh->fudgenormals == CMS_FUDGE_RIM) {
                                c3DLoader::FlattenNormals(&c_ss2, temp_min, temp_max);
                            }
                            //c_shs->AddMesh(i_mesh->FTX.c_str(), i_mesh->TXS.c_str(), i_mesh->place, i_mesh->flags,
                            //               i_mesh->unknown, c_vertexcount, c_vertices, c_indexcount, c_indices);
                            //delete[] c_vertices;
                            //delete[] c_indices;
                            c_mod.meshes.push_back(c_ss2);
                        }
                        CurrentObj++;
                    }
                    c_shs->AddModel(c_mod);
                    delete object;
                }
            }

            // Animated shapes
            if (m_work->animatedmodels.size()) {
                wxLogDebug(wxT("TRACE cSCNFile::MakeToOvl BSH"));
                ovlBSHManager* c_bsh = c_ovl.GetManager<ovlBSHManager>();
                for (cAnimatedModel::iterator i_mod = m_work->animatedmodels.begin(); i_mod != m_work->animatedmodels.end(); ++i_mod) {
                    if (!i_mod->used)
                        continue;

                    cBoneShape1 c_bs1;

                    boundsInit(&c_bs1.bbox1, &c_bs1.bbox2);

                    // Find active mesh count
//                    int mesh_count = 0;
//                    for (cMeshStruct::iterator i_mesh = i_mod->meshstructs.begin(); i_mesh != i_mod->meshstructs.end(); i_mesh++) {
//                        if (!i_mesh->disabled)
//                            mesh_count++;
//                    }
                    c_bs1.name = i_mod->name.c_str();
                    c_bs1.bones.push_back(cBoneStruct(true));

                    // Determine transformation matrices
                    D3DMATRIX transformMatrix = matrixMultiply(matrixMultiply(i_mod->transforms), matrixGetFixOrientation(i_mod->usedorientation));
                    bool do_transform = !matrixIsEqual(transformMatrix, matrixGetUnity());
                    D3DMATRIX undoDamage;
                    int m = 0;
                    for(m = i_mod->transformnames.size()-1; m>=0; m--) {
                        if (i_mod->transformnames[m] == wxT("-"))
                            break;
                    }
                    if (m>0) {
                        // Note: if m=0, the first is the separator, so this can fall through to no separator
                        std::vector<D3DMATRIX> undostack;
                        for (int n = m; n < i_mod->transforms.size(); n++)
                            undostack.push_back(i_mod->transforms[n]);
                        undostack.push_back(matrixGetFixOrientation(i_mod->usedorientation));
                        undoDamage = matrixInverse(matrixMultiply(undostack));
                    } else {
                        undoDamage = matrixInverse(transformMatrix);
                    }
                    // We need to reevaluate do_transform
                    if (!do_transform)
                        do_transform = !matrixIsEqual(undoDamage, matrixGetUnity());

                    // Do effect points
                    if (i_mod->modelbones.size() != 0) {
                        for (unsigned int e = 0; e < i_mod->modelbones.size(); e++) {
                            cBoneStruct c_bone;
                            c_bone.name = i_mod->modelbones[e].name.c_str();
                            c_bone.parentbonenumber = i_mod->modelbones[e].nparent;
                            if (do_transform) {
                                std::vector<D3DMATRIX> tempstack = i_mod->modelbones[e].positions1;
                                // to correctely apply the model transformation matrix to effect points we have to
                                // transform in-game objects into modeler-space first by applying the inverse
                                // of the model transformation matrix
                                // (now replaced by the undoDamage matrix to allow separation of the coordinate
                                // system fix from other model transformations)
                                tempstack.insert(tempstack.begin(), undoDamage);
                                // At the end of the stack, transform back
                                tempstack.push_back(transformMatrix);
                                c_bone.pos1 = matrixMultiply(tempstack);

                                // Same for pos2
                                tempstack = i_mod->modelbones[e].positions2;
                                tempstack.insert(tempstack.begin(), undoDamage);
                                tempstack.push_back(transformMatrix);
                                c_bone.pos2 = matrixMultiply(tempstack);
                            } else {
                                c_bone.pos1 = matrixMultiply(i_mod->modelbones[e].positions1);
                                c_bone.pos2 = matrixMultiply(i_mod->modelbones[e].positions2);
                            }
                            c_bs1.bones.push_back(c_bone);
                        }
                    }

                    c3DLoader *object = c3DLoader::LoadFile(i_mod->file.GetFullPath().fn_str());
                    if (!object) {
                        // Poof went the model!
                        throw RCT3Exception(wxString::Format(_("Something happened to the file of model '%s'."), i_mod->name.c_str()));
                    }

                    unsigned long CurrentObj = 0;
                    D3DVECTOR temp_min, temp_max;
                    for (cMeshStruct::iterator i_mesh = i_mod->meshstructs.begin(); i_mesh != i_mod->meshstructs.end(); i_mesh++) {
                        if (i_mesh->disabled == false) {
                            //progress.Update(++progress_count);
                            cBoneShape2 c_bs2;
                            c_bs2.fts = i_mesh->FTX.c_str();
                            c_bs2.texturestyle = i_mesh->TXS.c_str();
                            c_bs2.placetexturing = i_mesh->place;
                            c_bs2.textureflags = i_mesh->flags;
                            c_bs2.sides = i_mesh->unknown;

                            D3DVECTOR c_fudge_normal;
                            D3DVECTOR* c_pfudge_normal = NULL;
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
                            object->FetchObject(CurrentObj, i_mesh->bone, 0xff, &c_bs2, &temp_min, &temp_max,
                                                      const_cast<D3DMATRIX *> ((do_transform)?(&transformMatrix):NULL),
                                                      c_pfudge_normal);
//                            object->FetchAsAnimObject(CurrentObj, i_mesh->bone, 0xff, &c_vertexcount, &c_vertices,
//                                                      &c_indexcount, &c_indices, &temp_min, &temp_max,
//                                                      const_cast<D3DMATRIX *> ((do_transform)?(&transformMatrix):NULL),
//                                                      c_pfudge_normal);
                            boundsContain(&temp_min, &temp_max, &c_bs1.bbox1, &c_bs1.bbox2);
                            if (i_mesh->fudgenormals == CMS_FUDGE_RIM) {
                                c3DLoader::FlattenNormals(&c_bs2, temp_min, temp_max);
                            }
                            c_bs1.meshes.push_back(c_bs2);
//                            c_bsh->AddMesh(i_mesh->FTX.c_str(), i_mesh->TXS.c_str(), i_mesh->place, i_mesh->flags,
//                                           i_mesh->unknown, c_vertexcount, c_vertices, c_indexcount, c_indices);
//                            delete[] c_vertices;
//                            delete[] c_indices;
                        }
                        CurrentObj++;
                    }
                    //c_bsh->SetBoundingBox(box_min, box_max);
                    c_bsh->AddModel(c_bs1);
                    delete object;
                }
            }

            // Animations
            if (m_work->animations.size()) {
                wxLogDebug(wxT("TRACE cSCNFile::MakeToOvl BAN"));
                ovlBANManager* c_ban = c_ovl.GetManager<ovlBANManager>();
                for (cAnimation::iterator i_anim = m_work->animations.begin(); i_anim != m_work->animations.end(); ++i_anim) {
                    cBoneAnim c_item;
                    c_item.name = i_anim->name.c_str();
                    for (cBoneAnimation::iterator i_bone = i_anim->boneanimations.begin(); i_bone != i_anim->boneanimations.end(); ++i_bone) {
                        cBoneAnimBone c_bone;
                        c_bone.name = i_bone->name.c_str();
                        for (cTXYZ::iterator i_txyz = i_bone->translations.begin(); i_txyz != i_bone->translations.end(); ++i_txyz) {
                            c_bone.translations.insert(i_txyz->GetFixed(i_anim->usedorientation));
                        }
                        for (cTXYZ::iterator i_txyz = i_bone->rotations.begin(); i_txyz != i_bone->rotations.end(); ++i_txyz) {
                            c_bone.rotations.insert(i_txyz->GetFixed(i_anim->usedorientation));
                        }
                        c_item.bones.push_back(c_bone);
                    }
                    c_ban->AddAnimation(c_item);
                }
            }

        }

        // Textures
        if (m_work->flexitextures.size()) {
            wxLogDebug(wxT("TRACE cSCNFile::MakeToOvl FTX"));
            ovlFTXManager* c_ftx = c_ovl.GetManager<ovlFTXManager>();
            for (cFlexiTexture::iterator i_ftx = m_work->flexitextures.begin(); i_ftx != m_work->flexitextures.end(); i_ftx++) {
                if (!i_ftx->used)
                    continue;

                cFlexiTextureInfoStruct c_ftis;
                c_ftis.name = i_ftx->Name.c_str();
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
                unsigned long dimension = checkRCT3Texture(i_ftx->Frames[0].texture().GetFullPath());
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
                    c_fts.palette = counted_array_ptr<unsigned char>(new unsigned char[256 * sizeof(RGBQUAD)]);
                    memset(c_fts.palette.get(), 0, 256 * sizeof(RGBQUAD));

                    if ((tex.GetWidth() != dimension) || (tex.GetHeight() != dimension))
                        tex.Rescale(dimension, dimension);
                    c_fts.texture = counted_array_ptr<unsigned char>(new unsigned char[dimension * dimension]);
                    if (i_ftxfr->alphasource() != CFTF_ALPHA_NONE)
                        c_fts.alpha = counted_array_ptr<unsigned char>(new unsigned char[dimension * dimension]);

                    tex.flip();

                    if (i_ftxfr->recolorable()) {
                        memcpy(c_fts.palette.get(), cFlexiTexture::GetRGBPalette(), 256 * sizeof(RGBQUAD));
                        tex.GetAs8bitForced(c_fts.texture.get(), c_fts.palette.get(), true);
                    } else {
                        tex.GetAs8bit(c_fts.texture.get(), c_fts.palette.get());
                    }

                    if (i_ftxfr->alphasource() == CFTF_ALPHA_INTERNAL) {
                        tex.GetAlpha(c_fts.alpha.get());
                    } else if (i_ftxfr->alphasource() == CFTF_ALPHA_EXTERNAL) {
                        wxGXImage alp(i_ftxfr->alpha().GetFullPath());
                        if ((alp.GetWidth() != dimension) || (alp.GetHeight() != dimension))
                            alp.Rescale(dimension, dimension);
                        alp.GetGrayscale(c_fts.alpha.get());
                    }

                    for (unsigned int j = 0; j < 256; j++)
                        reinterpret_cast<RGBQUAD*>(c_fts.palette.get())[j].rgbReserved = i_ftxfr->alphacutoff();

                    c_ftis.frames.push_back(c_fts);
//                    c_ftx->AddTextureFrame(dimension, i_ftxfr->recolorable(),
//                                           reinterpret_cast<unsigned char*>(&palette),
//                                           reinterpret_cast<unsigned char*>(&data),
//                                           (i_ftxfr->alphasource() == CFTF_ALPHA_NONE)?NULL:reinterpret_cast<unsigned char*>(&alpha));
                }

                c_ftx->AddTexture(c_ftis);
            }
        }

}

void cSCNFile::CleanWork() {
    if (m_work)
        delete m_work;
    m_work = NULL;
    m_textureovl = false;
    m_meshes = 0;
    m_textures = 0;
}
