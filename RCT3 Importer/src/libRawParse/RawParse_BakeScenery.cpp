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

#include <boost/format.hpp>

#include "SCNFile.h"
#include "matrix.h"

inline void makeTXYZ(cXmlNode& parent, const wxString& name, const txyz& t) {
    cXmlNode ret = parent.newChild(name.utf8_str());
    ret.prop("time", boost::str(boost::format("%8f") % t.Time));
    ret.prop("x", boost::str(boost::format("%8f") % t.X));
    ret.prop("y", boost::str(boost::format("%8f") % t.Y));
    ret.prop("z", boost::str(boost::format("%8f") % t.Z));
}

cXmlNode XmlMakeMatrixNode(cXmlNode& parent, const MATRIX& matrix, const wxString& name, const wxString& matrixname = wxT("")) {
    cXmlNode node(parent.newChild(name.utf8_str()));
    if (name != wxT(""))
        node.wxprop("name", matrixname);

    cXmlNode row1("row1", boost::str(boost::format("%8f %8f %8f %8f") % matrix._11 % matrix._12 % matrix._13 % matrix._14).c_str());
    node.appendChildren(row1);

    cXmlNode row2("row2", boost::str(boost::format("%8f %8f %8f %8f") % matrix._21 % matrix._22 % matrix._23 % matrix._24).c_str());
    node.appendChildren(row2);

    cXmlNode row3("row3", boost::str(boost::format("%8f %8f %8f %8f") % matrix._31 % matrix._32 % matrix._33 % matrix._34).c_str());
    node.appendChildren(row3);

    cXmlNode row4("row4", boost::str(boost::format("%8f %8f %8f %8f") % matrix._41 % matrix._42 % matrix._43 % matrix._44).c_str());
    node.appendChildren(row4);

    return node;
}

void BakeScenery(cXmlNode& root, const cSCNFile& scn) {

    foreach(const cReference& ref, scn.references) {
        //XML_ADD(root, makeContentNode(RAWXML_REFERENCE, *its));
        root.newTextChild(RAWXML_REFERENCE, ref.name.utf8_str());
    }

    if (scn.lods.size()) {
        cXmlNode svd(root.newChild(RAWXML_SVD));
        svd.wxprop("name", scn.name);
        svd.prop("flags", boost::str(boost::format("0x%08x") % scn.sivsettings.sivflags));
        svd.prop("sway", boost::str(boost::format("%8f") % scn.sivsettings.sway));
        svd.prop("brightness", boost::str(boost::format("%8f") % scn.sivsettings.brightness));
        svd.prop("scale", boost::str(boost::format("%8f") % scn.sivsettings.scale));
        cXmlNode svdunk(svd.newChild(RAWXML_UNKNOWNS));
        svdunk.prop("u4", boost::str(boost::format("%8f") % scn.sivsettings.unknown));
        svdunk.addProp("u6", wxString::Format(wxT("%lu"), scn.sivsettings.unk6).mb_str(wxConvUTF8));
        svdunk.addProp("u7", wxString::Format(wxT("%lu"), scn.sivsettings.unk7).mb_str(wxConvUTF8));
        svdunk.addProp("u8", wxString::Format(wxT("%lu"), scn.sivsettings.unk8).mb_str(wxConvUTF8));
        svdunk.addProp("u9", wxString::Format(wxT("%lu"), scn.sivsettings.unk9).mb_str(wxConvUTF8));
        svdunk.addProp("u10", wxString::Format(wxT("%lu"), scn.sivsettings.unk10).mb_str(wxConvUTF8));
        svdunk.addProp("u11", wxString::Format(wxT("%lu"), scn.sivsettings.unk11).mb_str(wxConvUTF8));
        for (cLOD::const_iterator it = scn.lods.begin(); it != scn.lods.end(); ++it) {
            cXmlNode lod(svd.newChild(RAWXML_SVD_LOD));
            lod.addProp("name", it->modelname.mb_str(wxConvUTF8));
            lod.prop("distance", boost::str(boost::format("%8f") % it->distance));
            cXmlNode lodunk(lod.newChild(RAWXML_UNKNOWNS));
            lodunk.addProp("u2", wxString::Format(wxT("%lu"), it->unk2).mb_str(wxConvUTF8));
            lodunk.addProp("u4", wxString::Format(wxT("%lu"), it->unk4).mb_str(wxConvUTF8));
            lodunk.addProp("u14", wxString::Format(wxT("%lu"), it->unk14).mb_str(wxConvUTF8));
            if (it->animated) {
                lod.addProp("type", wxString::Format(wxT("%u"), static_cast<int>(r3::Constants::SVD::LOD_Type::Animated)).mb_str(wxConvUTF8));
                lod.addProp("boneshape", it->modelname.mb_str(wxConvUTF8));
                for (wxArrayString::const_iterator its = it->animations.begin(); its != it->animations.end(); ++its) {
                    //XML_ADD(lod, makeContentNode(RAWXML_SVD_LOD_ANIMATION, *its));
                    lod.newTextChild(RAWXML_SVD_LOD_ANIMATION, its->mb_str(wxConvUTF8));
                }
            } else {
                lod.addProp("type", wxString::Format(wxT("%u"), static_cast<int>(r3::Constants::SVD::LOD_Type::Static)).mb_str(wxConvUTF8));
                lod.addProp("staticshape", it->modelname.mb_str(wxConvUTF8));
            }
        }
    }

    for (cModel::const_iterator it = scn.models.begin(); it != scn.models.end(); ++it) {
        cXmlNode shs(root.newChild(RAWXML_SHS));
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

        shs.addProp("name", it->name.mb_str(wxConvUTF8));
        for (cMeshStruct::const_iterator itm = it->meshstructs.begin(); itm != it->meshstructs.end(); ++itm) {
            if (itm->disabled)
                continue;
            cXmlNode mesh(shs.newChild(RAWXML_SHS_MESH));
            mesh.addProp("ftx", itm->FTX.mb_str(wxConvUTF8));
            mesh.addProp("txs", itm->TXS.mb_str(wxConvUTF8));
            mesh.addProp("placing", wxString::Format(wxT("%lu"), itm->place).mb_str(wxConvUTF8));
            mesh.addProp("flags", wxString::Format(wxT("%lu"), itm->flags).mb_str(wxConvUTF8));
            mesh.addProp("doublesided", wxString::Format(wxT("%lu"), itm->unknown).mb_str(wxConvUTF8));
            if ((!do_transform) || (itm->fudgenormals != CMS_FUDGE_NONE)) {
                mesh.addProp("handedness", hand.mb_str(wxConvUTF8));
                mesh.addProp("up", up.mb_str(wxConvUTF8));
            }
            mesh.addProp("meshname", itm->Name.mb_str(wxConvUTF8));
            mesh.addProp("modelfile", it->file.GetFullPath().mb_str(wxConvUTF8));
            mesh.addProp("fudge", wxString::Format(wxT("%lu"), itm->fudgenormals).mb_str(wxConvUTF8));
            if (do_transform) {
                XmlMakeMatrixNode(mesh, transformMatrix, wxT(RAWXML_SHS_MESH_TRANSFORM));
            }
        }
        for (cEffectPoint::const_iterator itb = it->effectpoints.begin(); itb != it->effectpoints.end(); ++itb) {
            cXmlNode effect(shs.newChild(RAWXML_SHS_EFFECT));
            effect.addProp("name", itb->name.mb_str(wxConvUTF8));
            XmlMakeMatrixNode(effect, matrixThereAndBackAgain(itb->transforms, transformMatrix, undoDamage), wxT(RAWXML_SHS_EFFECT_POS));
        }
    }

    for (cAnimatedModel::const_iterator it = scn.animatedmodels.begin(); it != scn.animatedmodels.end(); ++it) {
        cXmlNode bsh(root.newChild(RAWXML_BSH));
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

        bsh.addProp("name", it->name.mb_str(wxConvUTF8));
        for (cMeshStruct::const_iterator itm = it->meshstructs.begin(); itm != it->meshstructs.end(); ++itm) {
            if (itm->disabled)
                continue;
            cXmlNode mesh(bsh.newChild(RAWXML_BSH_MESH));
            mesh.addProp("ftx", itm->FTX.mb_str(wxConvUTF8));
            mesh.addProp("txs", itm->TXS.mb_str(wxConvUTF8));
            mesh.addProp("placing", wxString::Format(wxT("%lu"), itm->place).mb_str(wxConvUTF8));
            mesh.addProp("flags", wxString::Format(wxT("%lu"), itm->flags).mb_str(wxConvUTF8));
            mesh.addProp("doublesided", wxString::Format(wxT("%lu"), itm->unknown).mb_str(wxConvUTF8));
            if ((!do_transform) || (itm->fudgenormals != CMS_FUDGE_NONE)) {
                mesh.addProp("handedness", hand.mb_str(wxConvUTF8));
                mesh.addProp("up", up.mb_str(wxConvUTF8));
            }
            mesh.addProp("meshname", itm->Name.mb_str(wxConvUTF8));
            mesh.addProp("modelfile", it->file.GetFullPath().mb_str(wxConvUTF8));
            mesh.addProp("bone", wxString::Format(wxT("%ld"), itm->bone).mb_str(wxConvUTF8));
            mesh.addProp("fudge", wxString::Format(wxT("%lu"), itm->fudgenormals).mb_str(wxConvUTF8));
            if (do_transform) {
                XmlMakeMatrixNode(mesh, transformMatrix, wxT(RAWXML_SHS_MESH_TRANSFORM));
            }
        }
        cXmlNode rootbone(bsh.newChild(RAWXML_BSH_BONE));
        rootbone.addProp("name", "Scene Root");
        for (cModelBone::const_iterator itb = it->modelbones.begin(); itb != it->modelbones.end(); ++itb) {
            cXmlNode bone(bsh.newChild(RAWXML_BSH_BONE));
            bone.addProp("name", itb->name.mb_str(wxConvUTF8));
            bone.addProp("parent", wxString::Format(wxT("%ld"), itb->nparent).mb_str(wxConvUTF8));
            XmlMakeMatrixNode(bone, matrixThereAndBackAgain(itb->positions1, transformMatrix, undoDamage), wxT(RAWXML_BSH_BONE_POS1));
            if (itb->usepos2) {
                XmlMakeMatrixNode(bone, matrixThereAndBackAgain(itb->positions2, transformMatrix, undoDamage), wxT(RAWXML_BSH_BONE_POS2));
            }
        }
    }

    for (cAnimation::const_iterator it = scn.animations.begin(); it != scn.animations.end(); ++it) {
        cXmlNode ban(root.newChild(RAWXML_BAN));
        ban.addProp("name", it->name.mb_str(wxConvUTF8));
        for (cBoneAnimation::const_iterator itb = it->boneanimations.begin(); itb != it->boneanimations.end(); ++itb) {
            cXmlNode bone(ban.newChild(RAWXML_BAN_BONE));
            bone.addProp("name", itb->name.mb_str(wxConvUTF8));
            for (cTXYZ::const_iterator itt = itb->translations.begin(); itt != itb->translations.end(); ++itt) {
                txyz t = itt->GetFixed(it->usedorientation, false);
                makeTXYZ(bone, wxT(RAWXML_BAN_BONE_TRANSLATION), t);
            }
            for (cTXYZ::const_iterator itt = itb->rotations.begin(); itt != itb->rotations.end(); ++itt) {
                txyz t = itt->GetFixed(it->usedorientation, true);
                makeTXYZ(bone, wxT(RAWXML_BAN_BONE_ROTATION), t);
            }
        }
    }

    for (cFlexiTexture::const_iterator it = scn.flexitextures.begin(); it != scn.flexitextures.end(); ++it) {
        cXmlNode ftx(root.newChild(RAWXML_FTX));

        ftx.addProp("name", it->Name.mb_str(wxConvUTF8));
        ftx.addProp("recolourable", wxString::Format(wxT("%ld"), it->Recolorable).mb_str(wxConvUTF8));
        if (it->FPS) {
            ftx.addProp("fps", wxString::Format(wxT("%ld"), it->FPS).mb_str(wxConvUTF8));
        }
        for(cFlexiTextureAnim::const_iterator ita = it->Animation.begin(); ita != it->Animation.end(); ++ita) {
            cXmlNode ftxanim(ftx.newChild(RAWXML_FTX_ANIMATION));
            ftxanim.addProp("index", wxString::Format(wxT("%lu"), ita->frame()).mb_str(wxConvUTF8));
            ftxanim.addProp("repeat", wxString::Format(wxT("%lu"), ita->count()).mb_str(wxConvUTF8));
        }
        for(cFlexiTextureFrame::const_iterator itf = it->Frames.begin(); itf != it->Frames.end(); ++itf) {
            cXmlNode ftxframe(ftx.newChild(RAWXML_FTX_FRAME));
            ftxframe.addProp("recolourable", wxString::Format(wxT("%lu"), itf->recolorable()).mb_str(wxConvUTF8));
            ftxframe.addProp("alphacutoff", wxString::Format(wxT("%hhu"), itf->alphacutoff()).mb_str(wxConvUTF8));
            cXmlNode ftximage(ftxframe.newTextChild(RAWXML_FTX_FRAME_IMAGE, itf->texture().GetFullPath().mb_str(wxConvUTF8)));
            if (itf->alphasource() == CFTF_ALPHA_INTERNAL) {
                ftximage.addProp("usealpha", "1");
            } else if (itf->alphasource() == CFTF_ALPHA_EXTERNAL) {
                ftxframe.newTextChild(RAWXML_FTX_FRAME_ALPHA, itf->alpha().GetFullPath().mb_str(wxConvUTF8));
            }
        }
    }
// TODO (belgabor#1#): Spline baking

}

