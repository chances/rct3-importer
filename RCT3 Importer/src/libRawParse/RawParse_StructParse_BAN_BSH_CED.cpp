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

#include "ManagerBAN.h"
#include "ManagerBSH.h"
#include "ManagerCED.h"

void cRawParser::ParseBAN(cXmlNode& node) {
    USE_PREFIX(node);
    cBoneAnim ban;
    wxString name = ParseString(node, wxT(RAWXML_BAN), wxT("name"), NULL, useprefix);
    ban.name = name.ToAscii();
    OPTION_PARSE(float, ban.totaltime, ParseFloat(node, wxT(RAWXML_BAN), wxT("totaltime")));
    if (ban.totaltime != 0.0)
        ban.calc_time = false;
    c3DLoaderOrientation ori = ParseOrientation(node, wxString::Format(wxT("ban(%s) tag"), name.c_str()));
    wxLogVerbose(wxString::Format(_("Adding ban %s to %s."), name.c_str(), m_output.GetFullPath().c_str()));

    cXmlNode child(node.children());
    while (child) {
        DO_CONDITION_COMMENT(child);

        if (child(RAWXML_BAN_BONE)) {
            cBoneAnimBone banim;
            wxString bonename = ParseString(child, wxT(RAWXML_BAN_BONE), wxT("name"), NULL);
            banim.name = bonename.ToAscii();
            cXmlNode subchild(child.children());
            while (subchild) {
                DO_CONDITION_COMMENT(subchild);

                if (subchild(RAWXML_BAN_BONE_TRANSLATION)) {
                    txyz frame;
                    frame.Time = ParseFloat(subchild, wxT(RAWXML_BAN_BONE_TRANSLATION), wxT("time"));
                    frame.X = ParseFloat(subchild, wxT(RAWXML_BAN_BONE_TRANSLATION), wxT("x"));
                    frame.Y = ParseFloat(subchild, wxT(RAWXML_BAN_BONE_TRANSLATION), wxT("y"));
                    frame.Z = ParseFloat(subchild, wxT(RAWXML_BAN_BONE_TRANSLATION), wxT("z"));
                    if (ori != ORIENTATION_LEFT_YUP)
                        doFixOrientation(frame, ori);
                    banim.translations.insert(frame);
                } else if (subchild(RAWXML_BAN_BONE_ROTATION)) {
                    txyz frame;
                    frame.Time = ParseFloat(subchild, wxT(RAWXML_BAN_BONE_ROTATION), wxT("time"));
                    frame.X = ParseFloat(subchild, wxT(RAWXML_BAN_BONE_ROTATION), wxT("x"));
                    frame.Y = ParseFloat(subchild, wxT(RAWXML_BAN_BONE_ROTATION), wxT("y"));
                    frame.Z = ParseFloat(subchild, wxT(RAWXML_BAN_BONE_ROTATION), wxT("z"));
                    if (ori != ORIENTATION_LEFT_YUP)
                        doFixOrientation(frame, ori);
                    banim.rotations.insert(frame);
                } else if (subchild.element()) {
                    throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in ban(%s)/banbone(%s)."), STRING_FOR_FORMAT(subchild.name()), name.c_str(), bonename.c_str()));
                }

                subchild.go_next();
            }

            ban.bones.push_back(banim);
        } else if (child.element()) {
            throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in ban tag '%s'."), STRING_FOR_FORMAT(child.name()), name.c_str()));
        }

        child.go_next();
    }

    ovlBANManager* c_ban = m_ovl.GetManager<ovlBANManager>();
    c_ban->AddAnimation(ban);
}

void cRawParser::ParseBSH(cXmlNode& node) {
    USE_PREFIX(node);
    cBoneShape1 bsh1;
    wxString name = ParseString(node, wxT(RAWXML_BSH), wxT("name"), NULL, useprefix);
    bsh1.name = name.ToAscii();
    boundsInit(&bsh1.bbox1, &bsh1.bbox2);
    cXmlNode bb1;
    cXmlNode bb2;
    wxLogVerbose(wxString::Format(_("Adding bsh %s to %s."), name.c_str(), m_output.GetFullPath().c_str()));

    cXmlNode child(node.children());
    while (child) {
        DO_CONDITION_COMMENT(child);

        if (child(RAWXML_BSH_BBOX1)) {
            bb1 = child;
            ParseVector(child, bsh1.bbox1, wxT(RAWXML_BSH_BBOX1));
        } else if (child(RAWXML_BSH_BBOX2)) {
            bb2 = child;
            ParseVector(child, bsh1.bbox2, wxT(RAWXML_BSH_BBOX2));
        } else if (child(RAWXML_BSH_MESH)) {
            USE_PREFIX(child);
            cBoneShape2 bsh2;
            bsh2.fts = ParseString(child, wxT(RAWXML_BSH_MESH), wxT("ftx"), NULL, useprefix).ToAscii();
            bsh2.texturestyle = ParseString(child, wxT(RAWXML_BSH_MESH), wxT("txs"), NULL).ToAscii();
            OPTION_PARSE(unsigned long, bsh2.placetexturing, ParseUnsigned(child, wxT(RAWXML_BSH_MESH), wxT("transparency")));
            OPTION_PARSE(unsigned long, bsh2.textureflags, ParseUnsigned(child, wxT(RAWXML_BSH_MESH), wxT("flags")));
            OPTION_PARSE(unsigned long, bsh2.sides, ParseUnsigned(child, wxT(RAWXML_BSH_MESH), wxT("sides")));
            OPTION_PARSE(long, bsh2.support, ParseSigned(child, wxT(RAWXML_BSH_MESH), wxT("support")));

            if (child.hasProp("meshname")) {
                wxString meshname = ParseString(child, wxT(RAWXML_BSH_MESH), wxT("meshname"), NULL);
                bool modelfilevar = false;
                wxFSFileName modelfile = ParseString(child, wxT(RAWXML_BSH_MESH), wxT("modelfile"), &modelfilevar);
                wxString hand = UTF8STRINGWRAP(child.getPropVal("handedness", "left"));
                wxString up = UTF8STRINGWRAP(child.getPropVal("up", "y"));
                c3DLoaderOrientation ori = ORIENTATION_LEFT_YUP;
                if (hand == wxT("left")) {
                    if (up == wxT("x")) {
                        ori = ORIENTATION_LEFT_XUP;
                    } else if (up == wxT("y")) {
                        ori = ORIENTATION_LEFT_YUP;
                    } else if (up == wxT("z")) {
                        ori = ORIENTATION_LEFT_ZUP;
                    } else {
                        throw RCT3Exception(wxString::Format(_("Unknown value '%s' for up attribute in bsh(%s)/bshmesh tag."), up.c_str(), name.c_str()));
                    }
                } else if (hand == wxT("right")) {
                    if (up == wxT("x")) {
                        ori = ORIENTATION_RIGHT_XUP;
                    } else if (up == wxT("y")) {
                        ori = ORIENTATION_RIGHT_YUP;
                    } else if (up == wxT("z")) {
                        ori = ORIENTATION_RIGHT_ZUP;
                    } else {
                        throw RCT3Exception(wxString::Format(_("Unknown value '%s' for up attribute in bsh(%s)/bshmesh tag."), up.c_str(), name.c_str()));
                    }
                } else {
                    throw RCT3Exception(wxString::Format(_("Unknown value '%s' for handedness attribute in bsh(%s)/bshmesh tag."), up.c_str(), name.c_str()));
                }

                if (!modelfile.IsAbsolute())
                    modelfile.MakeAbsolute(m_input.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME));

                if (m_mode == MODE_BAKE) {
                    if (m_bake.Index(wxT(RAWXML_BSH)) == wxNOT_FOUND) {
                        if (!modelfilevar) {
                            if (m_bake.Index(wxT(RAWBAKE_ABSOLUTE)) == wxNOT_FOUND) {
                                modelfile.MakeRelativeTo(m_bakeroot.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME));
                            }
                            child.prop("modelfile", modelfile.GetFullPath());
                        }
                        child.go_next();
                        continue;
                    }
                }
/*
                if (!modelfile.IsFileReadable())
                    throw RCT3Exception(_("bsh tag: Model file not readable: ") + modelfile.GetFullPath());
*/
                counted_ptr<c3DLoader> model(c3DLoader::LoadFile(modelfile.GetFullPath().c_str()));
                if (!model.get())
                    throw RCT3Exception(_("bsh tag: Model file cannot be read or has unknown format: ") + modelfile.GetFullPath());

                int meshnr = -1;
                for (int i = 0; i < model->GetObjectCount(); ++i) {
                    if (meshname == model->GetObjectName(i)) {
                        meshnr = i;
                        break;
                    }
                }
                if (meshnr == -1)
                    throw RCT3Exception(wxString::Format(_("bsh tag: Model file '%s' does not contain a mesh called '%s'."), modelfile.GetFullPath().c_str(), meshname.c_str()));

                unsigned long fudgenormals = CMS_FUDGE_NONE;
                OPTION_PARSE(unsigned long, fudgenormals, ParseUnsigned(child, wxT(RAWXML_BSH_MESH), wxT("fudge")));

                VECTOR temp_min;
                VECTOR temp_max;
                VECTOR c_fudge_normal;
                VECTOR* c_pfudge_normal = NULL;
                switch (fudgenormals) {
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
                    matrixApplyIP(c_pfudge_normal, matrixGetFixOrientation(ori));

                MATRIX m = matrixGetFixOrientation(ori);
                bool do_matrix = ori != ORIENTATION_LEFT_YUP;

                cXmlNode subchild(child.children());
                while (subchild) {
                    if (subchild(RAWXML_BSH_MESH_TRANSFORM)) {
                        if ((ori != ORIENTATION_LEFT_YUP) && (m_mode != MODE_INSTALL))
                            wxLogWarning(_("You've set handedness and up as well as giving a transformation matrix. The handedness/up is ignored for transforming the mesh."));
                        ParseMatrix(subchild, m, wxT(RAWXML_BSH_MESH_TRANSFORM));
                        do_matrix = true;
                    } else if (subchild.element()) {
                        throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in bsh(%s)/bshmesh."), STRING_FOR_FORMAT(subchild.name()), name.c_str()));
                    }
                    subchild.go_next();
                }

                boundsInit(&temp_min, &temp_max);
                if (child.hasProp("bone")) {
                    char bone = ParseSigned(child, wxT(RAWXML_BSH_MESH), wxT("bone"));
                    model->FetchObject(meshnr, bone, &bsh2, &temp_min, &temp_max,
                                              do_matrix?&m:NULL,
                                              c_pfudge_normal);
                } else {
                    model->FetchObject(meshnr, &bsh2, &temp_min, &temp_max,
                                              do_matrix?&m:NULL,
                                              c_pfudge_normal);
                }
                if (!(bb1 || bb2))
                    boundsContain(&temp_min, &temp_max, &bsh1.bbox1, &bsh1.bbox2);
                if (fudgenormals == CMS_FUDGE_RIM) {
                    c3DLoader::FlattenNormals(&bsh2, temp_min, temp_max);
                }
                if (m_mode == MODE_BAKE) {
                    child.delProp("meshname");
                    child.delProp("modelfile");
                    child.delProp("handedness");
                    child.delProp("up");
                    child.delProp("bone");
                    child.delProp("fudge");
                    if (child.children())
                        child.children().detach();

                    //wxXmlNode* lastchild = NULL;
                    for(std::vector<VERTEX2>::iterator it = bsh2.vertices.begin(); it != bsh2.vertices.end(); ++it) {
                        cXmlNode vert(child.newChild(RAWXML_BSH_MESH_VERTEX2));
                        vert.addProp("u", wxString::Format(wxT("%8f"), it->tu).mb_str(wxConvUTF8));
                        vert.addProp("v", wxString::Format(wxT("%8f"), it->tv).mb_str(wxConvUTF8));
                        vert.addProp("colour", wxString::Format(wxT("%08lX"), it->color).mb_str(wxConvUTF8));
                        cXmlNode pos(vert.newChild(RAWXML_BSH_MESH_VERTEX2_P));
                        pos.addProp("x", wxString::Format(wxT("%f"), it->position.x).mb_str(wxConvUTF8));
                        pos.addProp("y", wxString::Format(wxT("%f"), it->position.y).mb_str(wxConvUTF8));
                        pos.addProp("z", wxString::Format(wxT("%f"), it->position.z).mb_str(wxConvUTF8));
                        cXmlNode normal(vert.newChild(RAWXML_BSH_MESH_VERTEX2_N));
                        normal.addProp("x", wxString::Format(wxT("%8f"), it->normal.x).mb_str(wxConvUTF8));
                        normal.addProp("y", wxString::Format(wxT("%8f"), it->normal.y).mb_str(wxConvUTF8));
                        normal.addProp("z", wxString::Format(wxT("%8f"), it->normal.z).mb_str(wxConvUTF8));
                        for (int i = 0; i < 4; ++i) {
                            if (it->boneweight[i] > 0) {
                                cXmlNode bonenode(vert.newChild(RAWXML_BSH_MESH_VERTEX2_B));
                                bonenode.addProp("bone", wxString::Format(wxT("%hhd"), it->bone[i]).mb_str(wxConvUTF8));
                                bonenode.addProp("weight", wxString::Format(wxT("%hhu"), it->boneweight[i]).mb_str(wxConvUTF8));
                            }
                        }
                    }
                    for(int l = 0; l < bsh2.indices.size(); l+=3) {
                        cXmlNode tri(child.newChild(RAWXML_BSH_MESH_TRIANGLE));
                        tri.addProp("a", wxString::Format(wxT("%hu"), bsh2.indices[l]).mb_str(wxConvUTF8));
                        tri.addProp("b", wxString::Format(wxT("%hu"), bsh2.indices[l+1]).mb_str(wxConvUTF8));
                        tri.addProp("c", wxString::Format(wxT("%hu"), bsh2.indices[l+2]).mb_str(wxConvUTF8));
                    }
                }
            } else {
                BAKE_SKIP(child);
                cXmlNode subchild(child.children());
                while (subchild) {
                    DO_CONDITION_COMMENT(subchild);
                    if (subchild(RAWXML_BSH_MESH_VERTEX2)) {
                        VERTEX2 v;
                        vertex2init(v);
                        v.tu = ParseFloat(subchild, wxT(RAWXML_BSH_MESH_VERTEX2), wxT("u"));
                        v.tv = ParseFloat(subchild, wxT(RAWXML_BSH_MESH_VERTEX2), wxT("v"));
                        if (subchild.hasProp("colour")) {
                            wxString scol = UTF8STRINGWRAP(subchild.getPropVal("colour"));
                            unsigned long lcol = 0;
                            if (sscanf(scol.ToAscii(), "%lx", &lcol))
                                v.color = lcol;
                        }

                        bool nopos = true;
                        bool nonorm = true;
                        int bone = 0;

                        cXmlNode thirdchild(subchild.children());
                        while (thirdchild) {
                            if (thirdchild(RAWXML_BSH_MESH_VERTEX2_P)) {
                                nopos = false;
                                ParseVector(thirdchild, v.position, wxT(RAWXML_BSH_MESH_VERTEX2_P));
                            } else if (thirdchild(RAWXML_BSH_MESH_VERTEX2_N)) {
                                nonorm = false;
                                ParseVector(thirdchild, v.normal, wxT(RAWXML_BSH_MESH_VERTEX2_N));
                            } else if (thirdchild(RAWXML_BSH_MESH_VERTEX2_B)) {
                                if (bone == 3)
                                    throw RCT3Exception(wxString::Format(_("Too many boneassignments in bsh(%s)/bshmesh/vertex2."), name.c_str()));
                                v.bone[bone] = ParseSigned(thirdchild, wxT(RAWXML_BSH_MESH_VERTEX2_B), wxT("bone"));
                                v.boneweight[bone] = ParseUnsigned(thirdchild, wxT(RAWXML_BSH_MESH_VERTEX2_B), wxT("weight"));
                                bone++;
                            } else if (thirdchild.element()) {
                                throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in bsh(%s)/bshmesh/vertex2."), STRING_FOR_FORMAT(thirdchild.name()), name.c_str()));
                            }
                            thirdchild.go_next();
                        }

                        if (nopos || nonorm)
                            throw RCT3Exception(wxString::Format(_("Position or normal missing in bsh(%s)/bshmesh/vertex2."), name.c_str()));

                        if (!(bb1 || bb2))
                            boundsContain(&v.position, &bsh1.bbox1, &bsh1.bbox2);

                        bsh2.vertices.push_back(v);
                    } else if (subchild(RAWXML_BSH_MESH_TRIANGLE)) {
                        unsigned short index = ParseUnsigned(subchild, wxT(RAWXML_BSH_MESH_TRIANGLE), wxT("a"));
                        bsh2.indices.push_back(index);
                        index = ParseUnsigned(subchild, wxT(RAWXML_BSH_MESH_TRIANGLE), wxT("b"));
                        bsh2.indices.push_back(index);
                        index = ParseUnsigned(subchild, wxT(RAWXML_BSH_MESH_TRIANGLE), wxT("c"));
                        bsh2.indices.push_back(index);
                    } else if (subchild.element()) {
                        throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in bsh(%s)/bshmesh."), STRING_FOR_FORMAT(subchild.name()), name.c_str()));
                    }
                    subchild.go_next();
                }
            }
            bsh1.meshes.push_back(bsh2);
        } else if (child(RAWXML_BSH_BONE)) {
            BAKE_SKIP(child);
            cBoneStruct bones;
            wxString bonename = ParseString(child, wxT(RAWXML_BSH_BONE), wxT("name"), NULL);
            if (bonename == wxT("Scene Root")) {
                bones = cBoneStruct(true);
            } else {
                bones.name = bonename.ToAscii();
                bones.parentbonenumber = ParseSigned(child, wxT(RAWXML_BSH_BONE), wxT("parent"));
                bool missp1 = true;
                bool missp2 = true;
                cXmlNode subchild(child.children());
                while (subchild) {
                    DO_CONDITION_COMMENT(subchild);
                    if (subchild(RAWXML_BSH_BONE_POS1)) {
                        missp1 = false;
                        ParseMatrix(subchild, bones.pos1, wxT(RAWXML_BSH_BONE_POS1));
                    } else if (subchild(RAWXML_BSH_BONE_POS2)) {
                        missp2 = false;
                        ParseMatrix(subchild, bones.pos2, wxT(RAWXML_BSH_BONE_POS2));
                    } else if (subchild.element()) {
                        throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in bsh(%s)/bshbone(%s)."), STRING_FOR_FORMAT(subchild.name()), name.c_str(), bonename.c_str()));
                    }
                    subchild.go_next();
                }
                if (missp1)
                    throw RCT3Exception(wxString::Format(_("Missing position1 in bsh(%s)/bshbone(%s)."), name.c_str(), bonename.c_str()));
                if (missp2)
                    bones.pos2 = bones.pos1;
            }
            bsh1.bones.push_back(bones);
        } else if (child.element()) {
            throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in bsh tag '%s'."), STRING_FOR_FORMAT(child.name()), name.c_str()));
        }
        child.go_next();
    }

    if (m_mode != MODE_BAKE) {
        ovlBSHManager* c_bsh = m_ovl.GetManager<ovlBSHManager>();
        c_bsh->AddModel(bsh1);
    } else if ((!bb1) && (!bb2)) {
        if (m_bake.Index(wxT(RAWXML_BSH)) != wxNOT_FOUND) {
            cXmlNode bb(RAWXML_BSH_BBOX2);
            bb.addProp("x", wxString::Format(wxT("%f"), bsh1.bbox2.x).mb_str(wxConvUTF8));
            bb.addProp("y", wxString::Format(wxT("%f"), bsh1.bbox2.y).mb_str(wxConvUTF8));
            bb.addProp("z", wxString::Format(wxT("%f"), bsh1.bbox2.z).mb_str(wxConvUTF8));
            node.insertNodeAsFirstChild(bb);

            bb = cXmlNode(RAWXML_BSH_BBOX1);
            bb.addProp("x", wxString::Format(wxT("%f"), bsh1.bbox1.x).mb_str(wxConvUTF8));
            bb.addProp("y", wxString::Format(wxT("%f"), bsh1.bbox1.y).mb_str(wxConvUTF8));
            bb.addProp("z", wxString::Format(wxT("%f"), bsh1.bbox1.z).mb_str(wxConvUTF8));
            node.insertNodeAsFirstChild(bb);
        }
    }
}

void cRawParser::ParseCED(cXmlNode& node) {
    USE_PREFIX(node);
    cCarriedItemExtra ced;
    ced.name = ParseString(node, wxT(RAWXML_CED), wxT("name"), NULL, useprefix).ToAscii();
    ced.nametxt = ParseString(node, wxT(RAWXML_CED), wxT("nametxt"), NULL, useprefix).ToAscii();
    ced.icon = ParseString(node, wxT(RAWXML_CED), wxT("icon"), NULL, useprefix).ToAscii();
    wxLogVerbose(wxString::Format(_("Adding ced %s to %s."), wxString(ced.name.c_str(), wxConvLocal).c_str(), m_output.GetFullPath().c_str()));

    cXmlNode child(node.children());
    while (child) {
        DO_CONDITION_COMMENT(child);

        if (child(RAWXML_CED_MORE)) {
            OPTION_PARSE(float, ced.hunger, ParseFloat(child, wxT(RAWXML_CED_MORE), wxT("hunger")));
            OPTION_PARSE(float, ced.thirst, ParseFloat(child, wxT(RAWXML_CED_MORE), wxT("thirst")));
            OPTION_PARSE(unsigned long, ced.unk1, ParseUnsigned(child, wxT(RAWXML_CED_MORE), wxT("u1")));
            OPTION_PARSE(unsigned long, ced.unk4, ParseUnsigned(child, wxT(RAWXML_CED_MORE), wxT("u4")));
            OPTION_PARSE(float, ced.unk7, ParseFloat(child, wxT(RAWXML_CED_MORE), wxT("u7")));
        } else if (child.element()) {
            throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in ced tag."), STRING_FOR_FORMAT(child.name())));
        }
        child.go_next();
    }

    ovlCEDManager* c_ced = m_ovl.GetManager<ovlCEDManager>();
    c_ced->AddExtra(ced);
}

