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
#include "matrix.h"
//#include "3DLoader.h"
#include <sstream>
#include <stdio.h>


bool cSCNFile::Load() {

    ovlname = wxT("");
    flexitextures.clear();
    models.clear();
    lods.clear();
    references.clear();
    sivsettings = cSIVSettings();

    error = CSCNFILE_NO_ERROR;

    if (filename == "") {
        error = CSCNFILE_ERROR_FILENAME;
        return false;
    }

    FILE *f = fopen(filename.GetFullPath().fn_str(), "rb");
    if (!f) {
        error = CSCNFILE_ERROR_FILENAME;
        return false;
    }

    unsigned long objlen;
    fread(&objlen, sizeof(objlen), 1, f);

    // Check for old file format and call legacy loading function
    if (objlen < ((unsigned long) -1))
        return LoadLegacy(objlen, f);

    // Read magic
    char scnmagic[8];
    fread(&scnmagic, 8, 1, f);
    if (strncmp(CSCNFILE_MAGIC, scnmagic, 7) != 0) {
        error = CSCNFILE_ERROR_WRONG_MAGIC;
        fclose(f);
        return false;
    }

    // Read version
    fread(&version, sizeof(long), 1, f);
    if (version > VERSION_CSCNFILE) {
        error = CSCNFILE_ERROR_VERSION;
        fclose(f);
        return false;
    }

    // Read ovl name
    unsigned long count;
    fread(&count, sizeof(count), 1, f);
    if (count) {
        char *tmp = new char[count];
        fread(tmp, count, 1, f);
        ovlname = wxString(tmp);
        if (strlen(tmp)) {
            if (!ovlname.IsAbsolute()) {
                ovlname.MakeAbsolute(filename.GetPath());
            }
        }
        delete[] tmp;
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
    bool model_ok = LoadModels(f);
    LoadLODs(f);
    LoadReferences(f);

    fclose(f);
    return model_ok;
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
        for(unsigned long i = 0; i < anim_count; i++) {
            unsigned long l;
            fread(&l, sizeof(unsigned long), 1, f);
            ft.Animation.push_back(l);
        }

        unsigned long frame_count = 0;
        fread(&frame_count, sizeof(frame_count), 1, f);
        for(unsigned long i = 0; i < frame_count; i++) {
            cFlexiTextureFrame ftfr;

            fread(&ftfr.Recolorable, sizeof(ftfr.Recolorable), 1, f);

            unsigned long texturelen = 0;
            fread(&texturelen, sizeof(texturelen), 1, f);
            if (texturelen) {
                tmp = new char[texturelen];
                fread(tmp, texturelen, 1, f);
                ftfr.Texture = wxString(tmp);
                if (strlen(tmp)) {
                    if (!ftfr.Texture.IsAbsolute()) {
                        ftfr.Texture.MakeAbsolute(filename.GetPath());
                    }
                }
                delete[] tmp;
            }

            unsigned long alphalen = 0;
            fread(&alphalen, sizeof(alphalen), 1, f);
            if (alphalen) {
                tmp = new char[alphalen];
                fread(tmp, alphalen, 1, f);
                ftfr.Alpha = wxString(tmp);
                if (strlen(tmp)) {
                    if (!ftfr.Alpha.IsAbsolute()) {
                        ftfr.Alpha.MakeAbsolute(filename.GetPath());
                    }
                }
                delete[] tmp;
            }
            if (version >= 5) {
                fread(&ftfr.AlphaCutoff, sizeof(ftfr.AlphaCutoff), 1, f);
                fread(&ftfr.AlphaSource, sizeof(ftfr.AlphaSource), 1, f);
            } else {
                if (ftfr.Alpha != wxT(""))
                    ftfr.AlphaSource = CFTF_ALPHA_EXTERNAL;
                else
                    ftfr.AlphaSource = CFTF_ALPHA_NONE;
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
                e.Name = tmp;
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
                    e.TransformNames.push_back(wxString(tmp));
                    delete[] tmp;
                } else {
                    e.TransformNames.push_back(_("<empty name>"));
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

                e.Transform.push_back(m);
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
        ftfr.Recolorable = ft.Recolorable;
        unsigned long texturelen;
        fread(&texturelen, sizeof(texturelen), 1, f);
        if (texturelen) {
            tmp = new char[texturelen];
            fread(tmp, texturelen, 1, f);
            ftfr.Texture = tmp;
            delete[] tmp;
        } else
            ftfr.Texture = wxT("");

        unsigned long alphalen;
        fread(&alphalen, sizeof(alphalen), 1, f);
        if (alphalen) {
            tmp = new char[alphalen];
            fread(tmp, alphalen, 1, f);
            ftfr.Alpha = tmp;
            delete[] tmp;
        } else
            ftfr.Alpha = wxT("");

        if (ftfr.Alpha != wxT(""))
            ftfr.AlphaSource = CFTF_ALPHA_EXTERNAL;
        else
            ftfr.AlphaSource = CFTF_ALPHA_NONE;

        if ((ftfr.Texture != wxT("")) || (ftfr.Alpha != wxT("")))
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
            e.Name = tmp;
            delete[] tmp;
        } else
            e.Name = "";

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
        e.Transform.push_back(m);
        e.TransformNames.push_back("Custom Matrix (old scn)");

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
                    e.Name = tmp;
                    delete[] tmp;
                } else
                    e.Name = "";

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
                    e.TransformNames.push_back(nam);

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
                    e.Transform.push_back(m);
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

        unsigned long anim_count = ft.Animation.size();
        fwrite(&anim_count, sizeof(anim_count), 1, f);
        for(std::vector<unsigned long>::iterator it = ft.Animation.begin(); it != ft.Animation.end(); it++) {
            fwrite(&(*it), sizeof(unsigned long), 1, f);
        }

        unsigned long frame_count = ft.Frames.size();
        fwrite(&frame_count, sizeof(frame_count), 1, f);
        for(cFlexiTextureFrameIterator it = ft.Frames.begin(); it != ft.Frames.end(); it++) {
            fwrite(&it->Recolorable, sizeof(it->Recolorable), 1, f);

            temp = it->Texture;
            if (temp != wxT("")) {
                if (save_relative_paths)
                    temp.MakeRelativeTo(filename.GetPath());
            }
            unsigned long texturelen = temp.GetFullPath().length()+1;
            fwrite(&texturelen, sizeof(texturelen), 1, f);
            if (texturelen) {
                fwrite(temp.GetFullPath().fn_str(), texturelen, 1, f);
            }

            temp = it->Alpha;
            if (temp != wxT("")) {
                if (save_relative_paths)
                    temp.MakeRelativeTo(filename.GetPath());
            }
            unsigned long alphalen = temp.GetFullPath().length()+1;
            fwrite(&alphalen, sizeof(alphalen), 1, f);
            if (alphalen) {
                fwrite(temp.GetFullPath().fn_str(), alphalen, 1, f);
            }

            fwrite(&it->AlphaCutoff, sizeof(it->AlphaCutoff), 1, f);
            fwrite(&it->AlphaSource, sizeof(it->AlphaSource), 1, f);
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
            unsigned long namelen = e.Name.length()+1;
            fwrite(&namelen, sizeof(namelen), 1, f);
            if (namelen) {
                fwrite(e.Name.c_str(), namelen, 1, f);
            }

            // Matrices
            matrices = e.Transform.size();
            fwrite(&matrices, sizeof(matrices), 1, f);
            for (unsigned long j = 0; j < matrices; j++) {
                D3DMATRIX m = e.Transform[j];

                // Name
                namelen = e.TransformNames[j].length()+1;
                fwrite(&namelen, sizeof(namelen), 1, f);
                if (namelen) {
                    fwrite(e.TransformNames[j].c_str(), namelen, 1, f);
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

bool cSCNFile::Save() {
//    unsigned long i;
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

    // Write unsigned int max to allow discrimination between old and new files
    long newtag = -1;
    fwrite(&newtag, sizeof(newtag), 1, f);

    // Write magic
    char scnmagic[8] = CSCNFILE_MAGIC;
    fwrite(&scnmagic, 8, 1, f);

    // Write version
    fwrite(&version, sizeof(long), 1, f);

    // Write ovl name
    wxFileName temp = ovlname;
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
