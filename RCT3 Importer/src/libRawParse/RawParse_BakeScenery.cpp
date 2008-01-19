///////////////////////////////////////////////////////////////////////////////
//
// raw ovl xml interpreter
// Command line ovl creation utility
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
///////////////////////////////////////////////////////////////////////////////



#include "RawParse_cpp.h"

#define XML_ADD(base, node) \
    if (base ## lastchild) \
        base ## lastchild->SetNext(node); \
    else \
        base->SetChildren(node); \
    base ## lastchild = node;

inline wxXmlNode* makeTXYZ(const wxString& name, const txyz& t) {
    wxXmlNode* ret = makeElementNode(name);
    ret->AddProperty(wxT("time"), wxString::Format(wxT("%f"), t.Time));
    ret->AddProperty(wxT("x"), wxString::Format(wxT("%f"), t.X));
    ret->AddProperty(wxT("y"), wxString::Format(wxT("%f"), t.Y));
    ret->AddProperty(wxT("z"), wxString::Format(wxT("%f"), t.Z));
    return ret;
}

void BakeScenery(wxXmlNode* root, const cSCNFile& scn) {
    wxXmlNode* rootlastchild = NULL;

    for (wxArrayString::const_iterator its = scn.references.begin(); its != scn.references.end(); ++its) {
        XML_ADD(root, makeContentNode(RAWXML_REFERENCE, *its));
    }

    if (scn.lods.size()) {
        wxXmlNode* svd = makeElementNode(RAWXML_SVD);
        svd->AddProperty(wxT("name"), scn.name);
        svd->AddProperty(wxT("flags"), wxString::Format(wxT("%lu"), scn.sivsettings.sivflags));
        svd->AddProperty(wxT("sway"), wxString::Format(wxT("%f"), scn.sivsettings.sway));
        svd->AddProperty(wxT("brightness"), wxString::Format(wxT("%f"), scn.sivsettings.brightness));
        svd->AddProperty(wxT("scale"), wxString::Format(wxT("%f"), scn.sivsettings.scale));
        wxXmlNode* svdunk = makeElementNode(RAWXML_SVD_UNK);
        svd->SetChildren(svdunk);
        svdunk->AddProperty(wxT("u4"), wxString::Format(wxT("%f"), scn.sivsettings.unknown));
        svdunk->AddProperty(wxT("u6"), wxString::Format(wxT("%lu"), scn.sivsettings.unk6));
        svdunk->AddProperty(wxT("u7"), wxString::Format(wxT("%lu"), scn.sivsettings.unk7));
        svdunk->AddProperty(wxT("u8"), wxString::Format(wxT("%lu"), scn.sivsettings.unk8));
        svdunk->AddProperty(wxT("u9"), wxString::Format(wxT("%lu"), scn.sivsettings.unk9));
        svdunk->AddProperty(wxT("u10"), wxString::Format(wxT("%lu"), scn.sivsettings.unk10));
        svdunk->AddProperty(wxT("u11"), wxString::Format(wxT("%lu"), scn.sivsettings.unk11));
        wxXmlNode* svdlastchild = svdunk;
        for (cLOD::const_iterator it = scn.lods.begin(); it != scn.lods.end(); ++it) {
            wxXmlNode* lod = makeElementNode(RAWXML_SVD_LOD);
            lod->AddProperty(wxT("name"), it->modelname);
            lod->AddProperty(wxT("distance"), wxString::Format(wxT("%f"), it->distance));
            wxXmlNode* lodunk = makeElementNode(RAWXML_SVD_LOD_UNK);
            lod->SetChildren(lodunk);
            lodunk->AddProperty(wxT("u2"), wxString::Format(wxT("%lu"), it->unk2));
            lodunk->AddProperty(wxT("u4"), wxString::Format(wxT("%lu"), it->unk4));
            lodunk->AddProperty(wxT("u14"), wxString::Format(wxT("%lu"), it->unk14));
            wxXmlNode* lodlastchild = lodunk;
            if (it->animated) {
                lod->AddProperty(wxT("type"), wxString::Format(wxT("%u"), SVDLOD_MESHTYPE_ANIMATED));
                lod->AddProperty(wxT("boneshape"), it->modelname);
                for (wxArrayString::const_iterator its = it->animations.begin(); its != it->animations.end(); ++its) {
                    XML_ADD(lod, makeContentNode(RAWXML_SVD_LOD_ANIMATION, *its));
                }
            } else {
                lod->AddProperty(wxT("type"), wxString::Format(wxT("%u"), SVDLOD_MESHTYPE_STATIC));
                lod->AddProperty(wxT("staticshape"), it->modelname);
            }
            XML_ADD(svd, lod);
        }
        XML_ADD(root, svd);
    }

    for (cModel::const_iterator it = scn.models.begin(); it != scn.models.end(); ++it) {
        wxXmlNode* shs = makeElementNode(RAWXML_SHS);
        wxXmlNode* shslastchild = NULL;
        wxString hand = wxT("left");
        wxString up = wxT("y");
        switch (it->usedorientation) {
            case ORIENTATION_LEFT_XUP:
                up = wxT("x");
                break;
            case ORIENTATION_LEFT_ZUP:
                up = wxT("x");
                break;
            case ORIENTATION_RIGHT_XUP:
                hand = wxT("right");
                up = wxT("x");
                break;
            case ORIENTATION_RIGHT_YUP:
                hand = wxT("right");
                break;
            case ORIENTATION_RIGHT_ZUP:
                hand = wxT("right");
                up = wxT("z");
                break;
        }
        MATRIX transformMatrix;
        MATRIX undoDamage;
        bool do_transform = it->GetTransformationMatrices(transformMatrix, undoDamage);

        shs->AddProperty(wxT("name"), it->name);
        for (cMeshStruct::const_iterator itm = it->meshstructs.begin(); itm != it->meshstructs.end(); ++itm) {
            if (itm->disabled)
                continue;
            wxXmlNode* mesh = makeElementNode(RAWXML_SHS_MESH);
            mesh->AddProperty(wxT("ftx"), itm->FTX);
            mesh->AddProperty(wxT("txs"), itm->TXS);
            mesh->AddProperty(wxT("placing"), wxString::Format(wxT("%lu"), itm->place));
            mesh->AddProperty(wxT("flags"), wxString::Format(wxT("%lu"), itm->flags));
            mesh->AddProperty(wxT("doublesided"), wxString::Format(wxT("%lu"), itm->unknown));
            if ((!do_transform) || (itm->fudgenormals != CMS_FUDGE_NONE)) {
                mesh->AddProperty(wxT("handedness"), hand);
                mesh->AddProperty(wxT("up"), up);
            }
            mesh->AddProperty(wxT("meshname"), itm->Name);
            mesh->AddProperty(wxT("modelfile"), it->file.GetFullPath());
            mesh->AddProperty(wxT("fudge"), wxString::Format(wxT("%lu"), itm->fudgenormals));
            if (do_transform) {
                wxXmlNode* pos = XmlMakeMatrixNode(transformMatrix, wxT(""));
                pos->SetName(RAWXML_SHS_MESH_TRANSFORM);
                mesh->SetChildren(pos);
            }
            XML_ADD(shs, mesh);
        }
        for (cEffectPoint::const_iterator itb = it->effectpoints.begin(); itb != it->effectpoints.end(); ++itb) {
            wxXmlNode* effect = makeElementNode(RAWXML_SHS_EFFECT);
            effect->AddProperty(wxT("name"), itb->name);
            wxXmlNode* pos = XmlMakeMatrixNode(matrixThereAndBackAgain(itb->transforms, transformMatrix, undoDamage), wxT(""));
            pos->SetName(RAWXML_SHS_EFFECT_POS);
            effect->SetChildren(pos);
            XML_ADD(shs, effect);
        }
        XML_ADD(root, shs);
    }

    for (cAnimatedModel::const_iterator it = scn.animatedmodels.begin(); it != scn.animatedmodels.end(); ++it) {
        wxXmlNode* bsh = makeElementNode(RAWXML_BSH);
        wxXmlNode* bshlastchild = NULL;
        wxString hand = wxT("left");
        wxString up = wxT("y");
        switch (it->usedorientation) {
            case ORIENTATION_LEFT_XUP:
                up = wxT("x");
                break;
            case ORIENTATION_LEFT_ZUP:
                up = wxT("x");
                break;
            case ORIENTATION_RIGHT_XUP:
                hand = wxT("right");
                up = wxT("x");
                break;
            case ORIENTATION_RIGHT_YUP:
                hand = wxT("right");
                break;
            case ORIENTATION_RIGHT_ZUP:
                hand = wxT("right");
                up = wxT("z");
                break;
        }
        MATRIX transformMatrix;
        MATRIX undoDamage;
        bool do_transform = it->GetTransformationMatrices(transformMatrix, undoDamage);

        bsh->AddProperty(wxT("name"), it->name);
        for (cMeshStruct::const_iterator itm = it->meshstructs.begin(); itm != it->meshstructs.end(); ++itm) {
            if (itm->disabled)
                continue;
            wxXmlNode* mesh = makeElementNode(RAWXML_BSH_MESH);
            mesh->AddProperty(wxT("ftx"), itm->FTX);
            mesh->AddProperty(wxT("txs"), itm->TXS);
            mesh->AddProperty(wxT("placing"), wxString::Format(wxT("%lu"), itm->place));
            mesh->AddProperty(wxT("flags"), wxString::Format(wxT("%lu"), itm->flags));
            mesh->AddProperty(wxT("doublesided"), wxString::Format(wxT("%lu"), itm->unknown));
            if ((!do_transform) || (itm->fudgenormals != CMS_FUDGE_NONE)) {
                mesh->AddProperty(wxT("handedness"), hand);
                mesh->AddProperty(wxT("up"), up);
            }
            mesh->AddProperty(wxT("meshname"), itm->Name);
            mesh->AddProperty(wxT("modelfile"), it->file.GetFullPath());
            mesh->AddProperty(wxT("bone"), wxString::Format(wxT("%ld"), itm->bone));
            mesh->AddProperty(wxT("fudge"), wxString::Format(wxT("%lu"), itm->fudgenormals));
            if (do_transform) {
                wxXmlNode* pos = XmlMakeMatrixNode(transformMatrix, wxT(""));
                pos->SetName(RAWXML_BSH_MESH_TRANSFORM);
                mesh->SetChildren(pos);
            }
            XML_ADD(bsh, mesh);
        }
        wxXmlNode* rootbone = makeElementNode(RAWXML_BSH_BONE);
        rootbone->AddProperty(wxT("name"), wxT("Scene Root"));
        XML_ADD(bsh, rootbone);
        for (cModelBone::const_iterator itb = it->modelbones.begin(); itb != it->modelbones.end(); ++itb) {
            wxXmlNode* bone = makeElementNode(RAWXML_BSH_BONE);
            bone->AddProperty(wxT("name"), itb->name);
            bone->AddProperty(wxT("parent"), wxString::Format(wxT("%ld"), itb->nparent));
            wxXmlNode* pos1 = XmlMakeMatrixNode(matrixThereAndBackAgain(itb->positions1, transformMatrix, undoDamage), wxT(""));
            pos1->SetName(RAWXML_BSH_BONE_POS1);
            bone->SetChildren(pos1);
            if (itb->usepos2) {
                wxXmlNode* pos2 = XmlMakeMatrixNode(matrixThereAndBackAgain(itb->positions2, transformMatrix, undoDamage), wxT(""));
                pos2->SetName(RAWXML_BSH_BONE_POS2);
                pos1->SetNext(pos2);
            }
            XML_ADD(bsh, bone);
        }
        XML_ADD(root, bsh);
    }

    for (cAnimation::const_iterator it = scn.animations.begin(); it != scn.animations.end(); ++it) {
        wxXmlNode* ban = makeElementNode(RAWXML_BAN);
        wxXmlNode* banlastchild = NULL;
        ban->AddProperty(wxT("name"), it->name);
        for (cBoneAnimation::const_iterator itb = it->boneanimations.begin(); itb != it->boneanimations.end(); ++itb) {
            wxXmlNode* bone = makeElementNode(RAWXML_BAN_BONE);
            wxXmlNode* bonelastchild = NULL;
            bone->AddProperty(wxT("name"), itb->name);
            for (cTXYZ::const_iterator itt = itb->translations.begin(); itt != itb->translations.end(); ++itt) {
                txyz t = itt->GetFixed(it->usedorientation);
                wxXmlNode* frame = makeTXYZ(RAWXML_BAN_BONE_TRANSLATION, t);
                XML_ADD(bone, frame);
            }
            for (cTXYZ::const_iterator itt = itb->rotations.begin(); itt != itb->rotations.end(); ++itt) {
                txyz t = itt->GetFixed(it->usedorientation);
                wxXmlNode* frame = makeTXYZ(RAWXML_BAN_BONE_ROTATION, t);
                XML_ADD(bone, frame);
            }
            XML_ADD(ban, bone);
        }
        XML_ADD(root, ban);
    }

    for (cFlexiTexture::const_iterator it = scn.flexitextures.begin(); it != scn.flexitextures.end(); ++it) {
        wxXmlNode* ftx = makeElementNode(RAWXML_FTX);
        wxXmlNode* ftxlastchild = NULL;

        ftx->AddProperty(wxT("name"), it->Name);
        ftx->AddProperty(wxT("recolourable"), wxString::Format(wxT("%ld"), it->Recolorable));
        if (it->FPS) {
            ftx->AddProperty(wxT("fps"), wxString::Format(wxT("%ld"), it->FPS));
        }
        for(cFlexiTextureAnim::const_iterator ita = it->Animation.begin(); ita != it->Animation.end(); ++ita) {
            wxXmlNode* ftxanim = makeElementNode(RAWXML_FTX_ANIMATION);
            ftxanim->AddProperty(wxT("index"), wxString::Format(wxT("%lu"), ita->frame()));
            ftxanim->AddProperty(wxT("repeat"), wxString::Format(wxT("%lu"), ita->count()));
            XML_ADD(ftx, ftxanim);
        }
        for(cFlexiTextureFrame::const_iterator itf = it->Frames.begin(); itf != it->Frames.end(); ++itf) {
            wxXmlNode* ftxframe = makeElementNode(RAWXML_FTX_FRAME);
            ftxframe->AddProperty(wxT("recolourable"), wxString::Format(wxT("%lu"), itf->recolorable()));
            ftxframe->AddProperty(wxT("alphacutoff"), wxString::Format(wxT("%hhu"), itf->alphacutoff()));
            wxXmlNode* ftximage = makeContentNode(RAWXML_FTX_FRAME_IMAGE, itf->texture().GetFullPath());
            if (itf->alphasource() == CFTF_ALPHA_INTERNAL) {
                ftximage->AddProperty(wxT("usealpha"), wxT("1"));
            } else if (itf->alphasource() == CFTF_ALPHA_EXTERNAL) {
                ftximage->SetNext(makeContentNode(RAWXML_FTX_FRAME_ALPHA, itf->alpha().GetFullPath()));
            }
            ftxframe->SetChildren(ftximage);
            XML_ADD(ftx, ftxframe);
        }
        XML_ADD(root, ftx);
    }

}

