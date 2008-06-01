///////////////////////////////////////////////////////////////////////////////
//
// 3D Utility Library
// MS3D Loader
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
// Jonathan Wilson) by DragonsIOA.
//
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>

#include <boost/tokenizer.hpp>

#include "MS3DLoader.h"
#include "MS3DFile.h"
#include "wxLocalLog.h"
#include "gximage.h"
#include "matrix.h"
#include "pretty.h"
#include "rct3constants.h"
#include "xmlhelper.h"

#include "cXmlDoc.h"
#include "cXmlInputOutputCallbackString.h"
#include "cXmlNode.h"
#include "cXmlValidatorRNVRelaxNG.h"

#include "rng/ms3d_comment.rnc.gz.h"

using namespace r3;
using namespace std;
using namespace xmlcpp;

//#define DUMP_ANIDATA

inline void fixMS3DRotation(txyz& v) {
    float ax = fabs(v.x);
    float ay = fabs(v.y);
    float az = fabs(v.z);

    if (((ax < 0.000001) && (az < 0.000001)) || ((ay < 0.000001) && (az < 0.000001)) || ((ax < 0.000001) && (ay < 0.000001)))
        return; // Only one axis affected, bail out

    MATRIX m = matrixMultiply(matrixGetRotationX(v.x), matrixGetRotationY(v.y));
    matrixMultiplyIP(m, matrixGetRotationZ(v.z));

    VECTOR va;
    matrixExtractRotation(m, va);
//    va.x = atan2(-m.m[1][2], m.m[2][2]);
//    va.y = asin(m.m[0][2]);
//    va.z = atan2(-m.m[0][1], m.m[0][0]);

    VECTOR vb;
    matrixExtractRotation(m, vb, true);
//    vb.x = atan2(m.m[1][2], -m.m[2][2]);
//    vb.y = asin(-m.m[0][2])+M_PI;
//    vb.z = atan2(m.m[0][1], -m.m[0][0]);

    if ((fabs(va.x) + fabs(va.y) + fabs(va.z)) > (fabs(vb.x) + fabs(vb.y) + fabs(vb.z)))
        v.v = vb;
    else
        v.v = va;
}


cMS3DLoader::cMS3DLoader(const wxChar *filename): c3DLoader(filename) {
wxLocalLog(wxT("Trace, cMS3DLoader::cMS3DLoader(%s)"), filename);
//    if (cXmlInputOutputCallbackString::Init())
//        XMLCPP_RES_ADD(ms3d_comment, rng);

    cXmlInputOutputCallbackString::Init();
    XMLCPP_RES_ADD_ONCE(ms3d_comment, rnc);

    auto_ptr<CMS3DFile> ms3df(new CMS3DFile());
    if (!ms3df->LoadFromFile(wxString(filename).mb_str(wxConvFile)))
        throw E3DLoaderNotMyBeer();

    wxFileName f(filename);
    map<unsigned int, wxString> animations;
    animations[0] = _("Default");
    //wxString animationname = animations[0];

wxLocalLog(wxT("Trace, cMS3DLoader::cMS3DLoader(%s) Loaded g %d v %d"), filename, ms3df->GetNumGroups(), ms3df->GetNumVertices());

    // Handle model comment
    foreach(const ms3d_comment_t& c, ms3df->GetModelComments()) {
        cXmlDoc comm(c.comment, NULL, NULL, XML_PARSE_DTDLOAD);
        if (comm) {
            try {
                cXmlValidatorRNVRelaxNG val(XMLCPP_RES_USE(ms3d_comment, rnc).c_str());
                if (!val) {
                    wxString error(_("Internal Error: could not load ms3d comment schema:\n"));
                    foreach(const cXmlStructuredError& se, val.getStructuredErrors()) {
                        error += wxString::Format(wxT("Line %d: %s\n"), se.line,
                            wxString::FromUTF8(se.message.c_str()).c_str());
                    }
                    foreach(const std::string& ge, val.getGenericErrors())
                        error += wxString::FromUTF8(ge.c_str()) + "\n";
                    throw E3DLoader(error);
                }
                if (comm.validate(val)) {
                    wxString error(_("The comment of the MS3D file is not valid:\n"));
                    foreach(const cXmlStructuredError& se, val.getStructuredErrors()) {
                        error += wxString::Format(wxT("Line %d: %s\n"), se.line,
                            wxString::FromUTF8(se.message.c_str()).c_str());
                    }
                    foreach(const std::string& ge, val.getGenericErrors())
                        error += wxString::FromUTF8(ge.c_str()) + "\n";
                    wxLogWarning(error);
                } else {
                    foreach(const cXmlNode& n, comm.root().children()) {
                        if (n("name")) {
                            m_name = n.wxcontent();
                        } else if (n("path")) {
                            m_path = n.wxcontent();
                            wxFileName temp = m_path;
                            temp.MakeAbsolute(f.GetPathWithSep());
                            m_path = temp.GetFullPath();
                        } else if (n("animation")) {
                            unsigned long frame = 0;
                            parseULongC(n.getPropVal("frame"), frame);
                            if (frame) {
                                animations[frame] = n.wxcontent();
                            } else {
                                animations[0] = n.wxcontent();
//                                animationname = animations[0];
                            }
                        } else if (n("noshadow")) {
                            m_noshadow = true;
                        } else if (n("lod")) {
                            c3DGroup gr;
                            unsigned long forceanim = 0;
                            gr.m_name = n.getPropVal("name");
                            parseFloatC(n.getPropVal("distance"), gr.m_loddistance);
                            parseULongC(n.getPropVal("forceanim"), forceanim);
                            gr.m_forceanim = forceanim;
                            foreach(const cXmlNode& ch, n.children()) {
                                if (ch("mesh")) {
                                    gr.m_meshes.insert(ch.wxcontent());
                                } else if (ch("bone")) {
                                    gr.m_bones.insert(ch.wxcontent());
                                } else if (ch("animation")) {
                                    gr.m_animations.push_back(ch.wxcontent());
                                }
                            }
                            if ((!gr.m_animations.size()) && (gr.m_bones.size() || forceanim))
                                gr.m_animations.push_back(animations[0]);
                            m_groups[gr.m_name] = gr;
                        }
                    }
                }
            } catch (exception& e) {
                wxLogError(e.what());
            }
        } else {
            if (c.comment != "")
                wxLogWarning(_("MS3D file had model comment, but it wasn't valid xml."));
        }
    }

    // Handle joint comments
    map<wxString, wxString> jointrenames;
    for (int m = 0; m < ms3df->GetNumJoints(); m++) {
        ms3d_joint_t * joint;
        ms3df->GetJointAt(m, &joint);
        jointrenames[joint->name] = joint->name;
    }
    foreach(const ms3d_comment_t& c, ms3df->GetJointComments()) {
        if (c.index < ms3df->GetNumJoints()) {
            ms3d_joint_t* joint;
            ms3df->GetJointAt(c.index, &joint);
            wxString temp = c.comment;
            temp.Trim().Trim(false);
            jointrenames[joint->name] = temp;
        }
    }


    for (int m = 0; m < ms3df->GetNumGroups(); m++) {
        c3DMesh cmesh;

        ms3d_group_t *group;
        ms3df->GetGroupAt(m, &group);

        cmesh.m_name = wxString(group->name, wxConvLocal);
        cmesh.m_flag = C3DMESH_VALID;

        VERTEX2 tv;
        vertex2init(tv);
        unsigned long i, j;

        if (group->numtriangles == 0) {
            cmesh.m_flag = C3DMESH_INVALID;
        } else
            for (i = 0; i < group->numtriangles * 3; i += 3) {
                ms3d_vertex_t *vertex;
                ms3d_vertex_ex_t *vertexex = NULL;
                ms3d_triangle_t *face;

                unsigned int index = i / 3;
                bool add;

                word i_triangle = group->triangleIndices[index];
                if (i_triangle >= ms3df->GetNumTriangles()) {
                    cmesh.m_flag = C3DMESH_INVALID;
                    continue;
                }
                ms3df->GetTriangleAt(i_triangle, &face);

                vertex2init(tv);
                word i_vertex = face->vertexIndices[0];
                if (i_vertex >= ms3df->GetNumVertices()) {
                    cmesh.m_flag = C3DMESH_INVALID;
                    continue;
                }
                ms3df->GetVertexAt(i_vertex, &vertex);
                ms3df->GetVertexExAt(i_vertex, &vertexex);
                tv.position.x = vertex->vertex[0];
                tv.position.y = vertex->vertex[1];
                tv.position.z = vertex->vertex[2];
                tv.color = 0xffffffff;
                tv.normal.x = face->vertexNormals[0][0];
                tv.normal.y = face->vertexNormals[0][1];
                tv.normal.z = face->vertexNormals[0][2];
                tv.tu = face->s[0];
                tv.tv = 1.0-face->t[0];

                tv.bone[0] = vertex->boneId;
                if ((vertexex) && (tv.bone[0] != -1)) {
//wxLogMessage(wxString::Format(wxT("%s %hhd %hhd %hhd %hhd /  %hhd %hhd %hhd"), cmesh.m_name.c_str(),
//    vertex->boneId, vertexex->boneIds[0], vertexex->boneIds[1], vertexex->boneIds[2], vertexex->weights[0], vertexex->weights[1], vertexex->weights[2]));
                    unsigned char lastweight = 255;
                    bool readnext = true;
                    for (int c = 0; c < 3; ++c) {
                        if (readnext) {
                            tv.boneweight[c] = vertexex->weights[c];
                            lastweight -= vertexex->weights[c];
                        }
                        if (vertexex->boneIds[c] != -1) {
                            tv.bone[c+1] = vertexex->boneIds[c];
                            readnext = true;
                        } else {
                            readnext = false;
                        }
                    }
                    tv.boneweight[3] = lastweight;
                } else {
                    tv.boneweight[0] = 255;
                }

//wxLogMessage(wxString::Format(wxT("%s %hhd %hhd %hhd %hhd /  %hhd %hhd %hhd %hhd"), cmesh.m_name.c_str(),
//    tv.bone[0], tv.bone[1], tv.bone[2], tv.bone[3], tv.boneweight[0], tv.boneweight[1], tv.boneweight[2], tv.boneweight[3]));
                for (int c = 0; c < 4; ++c) {
                    if (tv.bone[c] == -1)
                        continue;
                    if (tv.boneweight[c] > 0) {
                        ms3d_joint_t * joint;
                        ms3df->GetJointAt(tv.bone[c], &joint);
                        cmesh.m_bones.insert(jointrenames[joint->name]);
//wxLogMessage(wxString::Format(wxT("%s %s"), cmesh.m_name.c_str(), jointrenames[joint->name].c_str()));
                    }
                }

                // now see if we have already added this point
                add = TRUE;
                for (j = 0; j < (long) cmesh.m_vertices.size(); ++j) {
                    VERTEX2 *pv = &cmesh.m_vertices[j];
                    if (memcmp(pv, &tv, sizeof(VERTEX2)) == 0) {
                        // we have a match so exit
                        add = FALSE;
                        break;
                    }
                }
                if (add) {
                    cmesh.m_vertices.push_back(tv);
                }
                // j should have our real index value now as well
                cmesh.m_indices.push_back(j);

                // now for the second one
                vertex2init(tv);
                i_vertex = face->vertexIndices[1];
                if (i_vertex >= ms3df->GetNumVertices()) {
                    cmesh.m_flag = C3DMESH_INVALID;
                    continue;
                }
                ms3df->GetVertexAt(i_vertex, &vertex);
                ms3df->GetVertexExAt(i_vertex, &vertexex);
                tv.position.x = vertex->vertex[0];
                tv.position.y = vertex->vertex[1];
                tv.position.z = vertex->vertex[2];
                tv.color = 0xffffffff;
                tv.normal.x = face->vertexNormals[1][0];
                tv.normal.y = face->vertexNormals[1][1];
                tv.normal.z = face->vertexNormals[1][2];
                tv.tu = face->s[1];
                tv.tv = 1.0-face->t[1];

                tv.bone[0] = vertex->boneId;
                if ((vertexex) && (tv.bone[0] != -1)) {
//wxLogMessage(wxString::Format(wxT("%s %hhd %hhd %hhd %hhd /  %hhd %hhd %hhd"), cmesh.m_name.c_str(),
//    vertex->boneId, vertexex->boneIds[0], vertexex->boneIds[1], vertexex->boneIds[2], vertexex->weights[0], vertexex->weights[1], vertexex->weights[2]));
                    unsigned char lastweight = 255;
                    bool readnext = true;
                    for (int c = 0; c < 3; ++c) {
                        if (readnext) {
                            tv.boneweight[c] = vertexex->weights[c];
                            lastweight -= vertexex->weights[c];
                        }
                        if (vertexex->boneIds[c] != -1) {
                            tv.bone[c+1] = vertexex->boneIds[c];
                            readnext = true;
                        } else {
                            readnext = false;
                        }
                    }
                    tv.boneweight[3] = lastweight;
                } else {
                    tv.boneweight[0] = 255;
                }

//wxLogMessage(wxString::Format(wxT("%s %hhd %hhd %hhd %hhd /  %hhd %hhd %hhd %hhd"), cmesh.m_name.c_str(),
//    tv.bone[0], tv.bone[1], tv.bone[2], tv.bone[3], tv.boneweight[0], tv.boneweight[1], tv.boneweight[2], tv.boneweight[3]));
                for (int c = 0; c < 4; ++c) {
                    if (tv.bone[c] == -1)
                        continue;
                    if (tv.boneweight[c] > 0) {
                        ms3d_joint_t * joint;
                        ms3df->GetJointAt(tv.bone[c], &joint);
                        cmesh.m_bones.insert(jointrenames[joint->name]);
//wxLogMessage(wxString::Format(wxT("%s %s"), cmesh.m_name.c_str(), jointrenames[joint->name].c_str()));
                    }
                }

                // now see if we have already added this point
                add = TRUE;
                for (j = 0; j < (long) cmesh.m_vertices.size(); ++j) {
                    VERTEX2 *pv = &cmesh.m_vertices[j];
                    if (memcmp(pv, &tv, sizeof(VERTEX2)) == 0) {
                        // we have a match so exit
                        add = FALSE;
                        break;
                    }
                }
                if (add) {
                    cmesh.m_vertices.push_back(tv);
                }
                // j should have our real index value now as well
                cmesh.m_indices.push_back(j);

                // now for the third one
                vertex2init(tv);
                i_vertex = face->vertexIndices[2];
                if (i_vertex >= ms3df->GetNumVertices()) {
                    cmesh.m_flag = C3DMESH_INVALID;
                    continue;
                }
                ms3df->GetVertexAt(i_vertex, &vertex);
                ms3df->GetVertexExAt(i_vertex, &vertexex);
                tv.position.x = vertex->vertex[0];
                tv.position.y = vertex->vertex[1];
                tv.position.z = vertex->vertex[2];
                tv.color = 0xffffffff;
                tv.normal.x = face->vertexNormals[2][0];
                tv.normal.y = face->vertexNormals[2][1];
                tv.normal.z = face->vertexNormals[2][2];
                tv.tu = face->s[2];
                tv.tv = 1.0-face->t[2];

                tv.bone[0] = vertex->boneId;
                if ((vertexex) && (tv.bone[0] != -1)) {
//wxLogMessage(wxString::Format(wxT("%s %hhd %hhd %hhd %hhd /  %hhd %hhd %hhd"), cmesh.m_name.c_str(),
//    vertex->boneId, vertexex->boneIds[0], vertexex->boneIds[1], vertexex->boneIds[2], vertexex->weights[0], vertexex->weights[1], vertexex->weights[2]));
                    unsigned char lastweight = 255;
                    bool readnext = true;
                    for (int c = 0; c < 3; ++c) {
                        if (readnext) {
                            tv.boneweight[c] = vertexex->weights[c];
                            lastweight -= vertexex->weights[c];
                        }
                        if (vertexex->boneIds[c] != -1) {
                            tv.bone[c+1] = vertexex->boneIds[c];
                            readnext = true;
                        } else {
                            readnext = false;
                        }
                    }
                    tv.boneweight[3] = lastweight;
                } else {
                    tv.boneweight[0] = 255;
                }

//wxLogMessage(wxString::Format(wxT("%s %hhd %hhd %hhd %hhd /  %hhd %hhd %hhd %hhd"), cmesh.m_name.c_str(),
//    tv.bone[0], tv.bone[1], tv.bone[2], tv.bone[3], tv.boneweight[0], tv.boneweight[1], tv.boneweight[2], tv.boneweight[3]));
                for (int c = 0; c < 4; ++c) {
                    if (tv.bone[c] == -1)
                        continue;
                    if (tv.boneweight[c] > 0) {
                        ms3d_joint_t * joint;
                        ms3df->GetJointAt(tv.bone[c], &joint);
                        cmesh.m_bones.insert(jointrenames[joint->name]);
//wxLogMessage(wxString::Format(wxT("%s %s"), cmesh.m_name.c_str(), jointrenames[joint->name].c_str()));
                    }
                }

                // now see if we have already added this point
                add = TRUE;
                for (j = 0; j < (long) cmesh.m_vertices.size(); ++j) {
                    VERTEX2 *pv = &cmesh.m_vertices[j];
                    if (memcmp(pv, &tv, sizeof(VERTEX2)) == 0) {
                        // we have a match so exit
                        add = FALSE;
                        break;
                    }
                }
                if (add) {
                    cmesh.m_vertices.push_back(tv);
                }
                // j should have our real index value now as well
                cmesh.m_indices.push_back(j);
            }
        if (group->materialIndex >= 0) {
            ms3d_material_t* mat;
            ms3df->GetMaterialAt(group->materialIndex, &mat);
            cmesh.m_texture = mat->name;
        }
        m_meshes[cmesh.m_name] = cmesh;
        m_meshId.push_back(cmesh.m_name);
    }

    //c3DAnimation ani;
    //ani.m_name = animationname;
    typedef pair<unsigned int, wxString> animpair;
    foreach(const animpair& an, animations) {
        m_animations[an.second].m_name = an.second;
    }

    for (int m = 0; m < ms3df->GetNumJoints(); m++) {
        ms3d_joint_t * joint;
        ms3df->GetJointAt(m, &joint);
        c3DBone bone;
        bone.m_name = jointrenames[joint->name];
        if (joint->parentName)
            bone.m_parent = jointrenames[joint->parentName];
        bone.m_type = wxT("Bone");

        bone.m_id = m_bones.size();
        m_bones[bone.m_name] = bone;
        m_boneId.push_back(bone.m_name);
    }

    float fps = ms3df->GetAnimationFPS();

    for (int m = 0; m < ms3df->GetNumJoints(); m++) {
        ms3d_joint_t * joint;
        ms3df->GetJointAt(m, &joint);
#ifdef DUMP_ANIDATA
        wxLogMessage("Dumping Bone %s", m_boneId[m].c_str());
#endif
        txyz pos;
        txyz rot;
        memset(&pos, 0, sizeof(txyz));
        memset(&rot, 0, sizeof(txyz));
        findPosAndRot(ms3df.get(), jointrenames, m, pos, rot);
#ifdef DUMP_ANIDATA
        wxLogMessage("  Translation: raw <%f/%f/%f>, written  <%f/%f/%f>",
            joint->position[0],joint->position[1],joint->position[2],
            pos.X, pos.Y, pos.Z
            );
        wxLogMessage("  Rotation: raw <%f/%f/%f>, written  <%f/%f/%f>",
            joint->rotation[0],joint->rotation[1],joint->rotation[2],
            rot.X, rot.Y, rot.Z
            );
#endif

        vector<MATRIX> posm;
        posm.push_back(matrixGetRotationX(rot.X));
        posm.push_back(matrixGetRotationY(rot.Y));
        posm.push_back(matrixGetRotationZ(rot.Z));
        posm.push_back(matrixGetTranslation(pos.X, pos.Y, pos.Z));
        m_bones[m_boneId[m]].m_pos[1] = matrixMultiply(posm);
        //ani.m_bones[m_boneId[m]].m_name = m_boneId[m];
        foreach(const animpair& an, animations) {
            m_animations[an.second].m_bones[m_boneId[m]].m_name = m_boneId[m];
        }

        c3DAnimation* ani = &m_animations[animations[0]];

#ifdef DUMP_ANIDATA
        wxLogMessage("  Translation frames:");
#endif
        for (int i = 0; i < joint->numKeyFramesTrans; ++i) {
            txyz pf;
            unsigned int frame = roundf(joint->keyFramesTrans[i].time * fps);
            pf.Time = static_cast<float>(frame - 1) / fps;
            pf.X = joint->position[0] + joint->keyFramesTrans[i].position[0];
            pf.Y = joint->position[1] + joint->keyFramesTrans[i].position[1];
            pf.Z = joint->position[2] + joint->keyFramesTrans[i].position[2];
            ani->m_bones[m_boneId[m]].m_translations.push_back(pf);
            if (has(animations, frame)) {
                // Need to switch to the next animation
                ani = &m_animations[animations[frame]];
                ani->m_bones[m_boneId[m]].m_translations.push_back(pf);
            }
#ifdef DUMP_ANIDATA
        wxLogMessage("    %f: raw <%f/%f/%f>, written  <%f/%f/%f>", pf.Time,
            joint->keyFramesTrans[i].position[0],joint->keyFramesTrans[i].position[1],joint->keyFramesTrans[i].position[2],
            pf.X, pf.Y, pf.Z
            );
#endif
        }

        ani = &m_animations[animations[0]];

#ifdef DUMP_ANIDATA
        wxLogMessage("  Rotation frames:");
#endif
        for (int i = 0; i < joint->numKeyFramesRot; ++i) {
            txyz pf;
            int frame = roundf(joint->keyFramesRot[i].time * fps);
            pf.Time = static_cast<float>(frame - 1) / fps;
            pf.X = joint->rotation[0] + joint->keyFramesRot[i].rotation[0];
            pf.Y = joint->rotation[1] + joint->keyFramesRot[i].rotation[1];
            pf.Z = joint->rotation[2] + joint->keyFramesRot[i].rotation[2];
            fixMS3DRotation(pf);
            ani->m_bones[m_boneId[m]].m_rotations.push_back(pf);
            if (has(animations, frame)) {
                // Need to switch to the next animation
                ani = &m_animations[animations[frame]];
                ani->m_bones[m_boneId[m]].m_rotations.push_back(pf);
            }
#ifdef DUMP_ANIDATA
        wxLogMessage("    %f: raw <%f/%f/%f>, written  <%f/%f/%f>", pf.Time,
            joint->keyFramesRot[i].rotation[0],joint->keyFramesRot[i].rotation[1],joint->keyFramesRot[i].rotation[2],
            pf.X, pf.Y, pf.Z
            );
#endif
        }
    }

    for (int m = 0; m < ms3df->GetNumVertices(); ++m) {
        ms3d_vertex_t *vertex;
        ms3df->GetVertexAt(m, &vertex);
        if (vertex->referenceCount == 0) {
            c3DBone cbone;
            cbone.m_name = wxString::Format(wxT("Vertex at <%.4f,%.4f,%.4f>"), vertex->vertex[0], vertex->vertex[1], vertex->vertex[2]);
            cbone.m_type = "Effect";
            cbone.m_pos[1] = matrixGetTranslation(vertex->vertex[0], vertex->vertex[1], vertex->vertex[2]);
            cbone.m_id = m_bones.size();
            m_bones[cbone.m_name] = cbone;
            m_boneId.push_back(cbone.m_name);
        }
    }

    calculateBonePos1();

    // Delete animation(s) if they are not animated
    if (m_animations.size()) {
        vector<wxString> todelete;
        foreach(c3DAnimation::pair& an, m_animations) {
            bool deleteme = true;
            foreach(c3DAnimBone::pair& bn, an.second.m_bones) {
                if (bn.second.m_translations.size() || bn.second.m_rotations.size()) {
                    deleteme = false;
                    break;
                }
            }
            if (deleteme)
                todelete.push_back(an.first);
        }
        foreach(const wxString& d, todelete) {
            m_animations.erase(d);
        }
    }
    //if (ani.m_bones.size()) {
    //    m_animations[ani.m_name] = ani;
    //}

    if (!m_groups.size())
        makeDefaultGroup(wxFileName(filename).GetName());

    // Handle group comments
    boost::char_separator<char> sep(" \r\n");
    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
    foreach(const ms3d_comment_t& c, ms3df->GetGroupComments()) {
        if (c.index < m_meshes.size()) {
            m_meshes[m_meshId[c.index]].m_meshOptions = c.comment;
        }
    }
    // Handle textures
    if (ms3df->GetNumMaterials()) {
        wxFileName temp;
        for (int t = 0; t < ms3df->GetNumMaterials(); ++t) {
            string comment;
            foreach(const ms3d_comment_t& c, ms3df->GetMaterialComments()) {
                if (c.index == t) {
                    comment = c.comment;
                    break;
                }
            }
            ms3d_material_t* mat;
            ms3df->GetMaterialAt(t, &mat);
            c3DTexture tex;
            tex.m_name = mat->name;
            tex.m_file = mat->texture;
            if (tex.m_file != "") {
                temp = tex.m_file;
                temp.MakeAbsolute(f.GetPathWithSep());
                tex.m_file = temp.GetFullPath();
            }
            tex.m_alphafile = mat->alphamap;
            if (tex.m_alphafile != "") {
                temp = tex.m_alphafile;
                temp.MakeAbsolute(f.GetPathWithSep());
                tex.m_alphafile = temp.GetFullPath();
            }
            tokenizer tok(comment, sep);
            enum matmode {
                Normal,
                Recol,
                Ref
            } tokenmode = Normal;
            foreach(const string& token, tok) {
                switch (tokenmode) {
                    case Normal: {
                        if (token == "recol") {
                            tokenmode = Recol;
                        } else if (token == "ref") {
                            tex.m_referenced = true;
                            tokenmode = Ref;
                        } else {
                            wxLogWarning(_("Unknown material/texture option token '%s'"), token.c_str());
                        }
                        break;
                    }
                    case Recol: {
                        tex.m_recol = 0;
                        if (token[0] == '1')
                            tex.m_recol |= r3::Constants::FlexiTexture::Recolourable::First;
                        if ((token[0] == '2') || (token[1] == '2'))
                            tex.m_recol |= r3::Constants::FlexiTexture::Recolourable::Second;
                        if ((token[0] == '3') || (token[1] == '3') || (token[2] == '3'))
                            tex.m_recol |= r3::Constants::FlexiTexture::Recolourable::Third;
                        tokenmode = Normal;
                        break;
                    }
                    case Ref: {
                        tex.m_file = token;
                        tokenmode = Normal;
                        break;
                    }
                }
            }
            if (tex.m_file != "") {
                // Figure out alpha mode
                if (tex.m_alphafile != "") {
                    tex.m_alphaType = 2;
                } else {
                    wxGXImage i(tex.m_file);
                    tex.m_alphaType = i.HasAlpha()?1:0;
                }
                m_textures[tex.m_name] = tex;
            }
        }
    }

#ifdef DUMP_ANIDATA
    wxLogMessage("Group Comments:");
    foreach(const ms3d_comment_t& c, ms3df->GetGroupComments())
        wxLogMessage("%d %s", c.index, c.comment.c_str());
    wxLogMessage("Material Comments:");
    foreach(const ms3d_comment_t& c, ms3df->GetMaterialComments())
        wxLogMessage("%d %s", c.index, c.comment.c_str());
    wxLogMessage("Joint Comments:");
    foreach(const ms3d_comment_t& c, ms3df->GetJointComments())
        wxLogMessage("%d %s", c.index, c.comment.c_str());
    wxLogMessage("Model Comments:");
    foreach(const ms3d_comment_t& c, ms3df->GetModelComments())
        wxLogMessage("%d %s", c.index, c.comment.c_str());
#endif

//    for (int m = 0; m < ms3df->GetNumJoints(); m++) {
//        if (!m_bones[m].m_parent.IsEmpty()) {
//            m_bones[m].m_pos[0] = matrixMultiply(m_bones[m].m_pos[1], matrixInverse(m_bones[m_bonemap[m_bones[m].m_parent]].m_pos[1]));
//        } else {
//            m_bones[m].m_pos[0] = m_bones[m].m_pos[1];
//        }
//    }
wxLocalLog(wxT("Trace, cMS3DLoader::cMS3DLoader(%s), End"), filename);
}

void cMS3DLoader::findPosAndRot(void* ms3df, std::map<wxString, wxString>& renames, int i, txyz& pos, txyz& rot) {
    ms3d_joint_t * joint;
    reinterpret_cast<CMS3DFile*>(ms3df)->GetJointAt(i, &joint);
    pos.X += joint->position[0];
    pos.Y += joint->position[1];
    pos.Z += joint->position[2];
    rot.X += joint->rotation[0];
    rot.Y += joint->rotation[1];
    rot.Z += joint->rotation[2];

    if (joint->parentName) {
        wxString parent = joint->parentName;
        if (!parent.IsEmpty()) {
            findPosAndRot(ms3df, renames, m_bones[renames[parent]].m_id, pos, rot);
        }
    }

}

