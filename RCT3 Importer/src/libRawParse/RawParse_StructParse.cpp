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

void cRawParser::ParseBAN(wxXmlNode* node) {
    USE_PREFIX(node);
    cBoneAnim ban;
    wxString name = ParseString(node, RAWXML_BAN, wxT("name"), NULL, useprefix);
    ban.name = name.ToAscii();
    OPTION_PARSE(float, ban.totaltime, ParseFloat(node, RAWXML_BAN, wxT("totaltime")));
    if (ban.totaltime != 0.0)
        ban.calc_time = false;
    wxLogVerbose(wxString::Format(_("Adding ban %s to %s."), name.c_str(), m_output.GetFullPath().c_str()));

    wxXmlNode *child = node->GetChildren();
    while (child) {
        DO_CONDITION_COMMENT(child);

        if (child->GetName() == RAWXML_BAN_BONE) {
            cBoneAnimBone banim;
            wxString bonename = ParseString(child, RAWXML_BAN_BONE, wxT("name"), NULL);
            banim.name = bonename.ToAscii();
            wxXmlNode *subchild = child->GetChildren();
            while (subchild) {
                DO_CONDITION_COMMENT(subchild);

                if (subchild->GetName() == RAWXML_BAN_BONE_TRANSLATION) {
                    txyz frame;
                    frame.Time = ParseFloat(subchild, RAWXML_BAN_BONE_TRANSLATION, wxT("time"));
                    frame.X = ParseFloat(subchild, RAWXML_BAN_BONE_TRANSLATION, wxT("x"));
                    frame.Y = ParseFloat(subchild, RAWXML_BAN_BONE_TRANSLATION, wxT("y"));
                    frame.Z = ParseFloat(subchild, RAWXML_BAN_BONE_TRANSLATION, wxT("z"));
                    banim.translations.insert(frame);
                } else if (subchild->GetName() == RAWXML_BAN_BONE_ROTATION) {
                    txyz frame;
                    frame.Time = ParseFloat(subchild, RAWXML_BAN_BONE_ROTATION, wxT("time"));
                    frame.X = ParseFloat(subchild, RAWXML_BAN_BONE_ROTATION, wxT("x"));
                    frame.Y = ParseFloat(subchild, RAWXML_BAN_BONE_ROTATION, wxT("y"));
                    frame.Z = ParseFloat(subchild, RAWXML_BAN_BONE_ROTATION, wxT("z"));
                    banim.rotations.insert(frame);
                } else if COMPILER_WRONGTAG(subchild) {
                    throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in ban(%s)/banbone(%s)."), subchild->GetName().c_str(), name.c_str(), bonename.c_str()));
                }

                subchild = subchild->GetNext();
            }

            ban.bones.push_back(banim);
        } else if COMPILER_WRONGTAG(child) {
            throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in ban tag '%s'."), child->GetName().c_str(), name.c_str()));
        }

        child = child->GetNext();
    }

    ovlBANManager* c_ban = m_ovl.GetManager<ovlBANManager>();
    c_ban->AddAnimation(ban);
}

void cRawParser::ParseBSH(wxXmlNode* node) {
    USE_PREFIX(node);
    cBoneShape1 bsh1;
    wxString name = ParseString(node, RAWXML_BSH, wxT("name"), NULL, useprefix);
    bsh1.name = name.ToAscii();
    boundsInit(&bsh1.bbox1, &bsh1.bbox2);
    wxXmlNode* bb1 = NULL;
    wxXmlNode* bb2 = NULL;
    wxLogVerbose(wxString::Format(_("Adding bsh %s to %s."), name.c_str(), m_output.GetFullPath().c_str()));

    wxXmlNode *child = node->GetChildren();
    while (child) {
        DO_CONDITION_COMMENT(child);

        if (child->GetName() == RAWXML_BSH_BBOX1) {
            bb1 = child;
            ParseVector(child, bsh1.bbox1, RAWXML_BSH_BBOX1);
        } else if (child->GetName() == RAWXML_BSH_BBOX2) {
            bb2 = child;
            ParseVector(child, bsh1.bbox2, RAWXML_BSH_BBOX2);
        } else if (child->GetName() == RAWXML_BSH_MESH) {
            USE_PREFIX(child);
            cBoneShape2 bsh2;
            bsh2.fts = ParseString(child, RAWXML_BSH_MESH, wxT("ftx"), NULL, useprefix).ToAscii();
            bsh2.texturestyle = ParseString(child, RAWXML_BSH_MESH, wxT("txs"), NULL).ToAscii();
            OPTION_PARSE(unsigned long, bsh2.placetexturing, ParseUnsigned(child, RAWXML_BSH_MESH, wxT("placing")));
            OPTION_PARSE(unsigned long, bsh2.textureflags, ParseUnsigned(child, RAWXML_BSH_MESH, wxT("flags")));
            OPTION_PARSE(unsigned long, bsh2.sides, ParseUnsigned(child, RAWXML_BSH_MESH, wxT("sides")));

            if (child->HasProp(wxT("meshname"))) {
                wxString meshname = ParseString(child, RAWXML_BSH_MESH, wxT("meshname"), NULL);
                bool modelfilevar = false;
                wxFSFileName modelfile = ParseString(child, RAWXML_BSH_MESH, wxT("modelfile"), &modelfilevar);
                wxString hand = child->GetPropVal(wxT("handedness"), wxT("left"));
                wxString up = child->GetPropVal(wxT("up"), wxT("y"));
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
                    if (m_bake.Index(RAWXML_BSH) == wxNOT_FOUND) {
                        if (!modelfilevar) {
                            if (m_bake.Index(RAWBAKE_ABSOLUTE) == wxNOT_FOUND) {
                                modelfile.MakeRelativeTo(m_bakeroot.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME));
                            }
                            child->DeleteProperty(wxT("modelfile"));
                            child->AddProperty(wxT("modelfile"), modelfile.GetFullPath());
                        }
                        child = child->GetNext();
                        continue;
                    }
                }
/*
                if (!modelfile.IsFileReadable())
                    throw RCT3Exception(_("bsh tag: Model file not readable: ") + modelfile.GetFullPath());
*/
                counted_ptr<c3DLoader> model(c3DLoader::LoadFile(modelfile.GetFullPath().fn_str()));
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
                OPTION_PARSE(unsigned long, fudgenormals, ParseUnsigned(child, RAWXML_BSH_MESH, wxT("fudge")));

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

                wxXmlNode *subchild = child->GetChildren();
                while (subchild) {
                    if (subchild->GetName() == RAWXML_BSH_MESH_TRANSFORM) {
                        if ((ori != ORIENTATION_LEFT_YUP) && (m_mode != MODE_INSTALL))
                            wxLogWarning(_("You've set handedness and up as well as giving a transformation matrix. The handedness/up is ignored for transforming the mesh."));
                        ParseMatrix(subchild, m, RAWXML_BSH_MESH_TRANSFORM);
                        do_matrix = true;
                    } else if COMPILER_WRONGTAG(subchild) {
                        throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in bsh(%s)/bshmesh."), subchild->GetName().c_str(), name.c_str()));
                    }
                    subchild = subchild->GetNext();
                }

                boundsInit(&temp_min, &temp_max);
                if (child->HasProp(wxT("bone"))) {
                    char bone = ParseSigned(child, RAWXML_BSH_MESH, wxT("bone"));
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
                    child->DeleteProperty(wxT("meshname"));
                    child->DeleteProperty(wxT("modelfile"));
                    child->DeleteProperty(wxT("handedness"));
                    child->DeleteProperty(wxT("up"));
                    child->DeleteProperty(wxT("bone"));
                    child->DeleteProperty(wxT("fudge"));
                    delete child->GetChildren();
                    child->SetChildren(NULL);

                    wxXmlNode* lastchild = NULL;
                    for(std::vector<VERTEX2>::iterator it = bsh2.vertices.begin(); it != bsh2.vertices.end(); ++it) {
                        wxXmlNode* vert = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, RAWXML_BSH_MESH_VERTEX2);
                        vert->AddProperty(wxT("u"), wxString::Format(wxT("%8f"), it->tu));
                        vert->AddProperty(wxT("v"), wxString::Format(wxT("%8f"), it->tv));
                        vert->AddProperty(wxT("colour"), wxString::Format(wxT("%08lX"), it->color));
                        wxXmlNode* pos = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, RAWXML_BSH_MESH_VERTEX2_P);
                        vert->SetChildren(pos);
                        pos->AddProperty(wxT("x"), wxString::Format(wxT("%f"), it->position.x));
                        pos->AddProperty(wxT("y"), wxString::Format(wxT("%f"), it->position.y));
                        pos->AddProperty(wxT("z"), wxString::Format(wxT("%f"), it->position.z));
                        wxXmlNode* normal = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, RAWXML_BSH_MESH_VERTEX2_N);
                        pos->SetNext(normal);
                        normal->AddProperty(wxT("x"), wxString::Format(wxT("%8f"), it->normal.x));
                        normal->AddProperty(wxT("y"), wxString::Format(wxT("%8f"), it->normal.y));
                        normal->AddProperty(wxT("z"), wxString::Format(wxT("%8f"), it->normal.z));
                        wxXmlNode* lastvertchild = normal;
                        for (int i = 0; i < 4; ++i) {
                            if (it->boneweight[i] > 0) {
                                wxXmlNode* bonenode = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, RAWXML_BSH_MESH_VERTEX2_B);
                                bonenode->AddProperty(wxT("bone"), wxString::Format(wxT("%hhd"), it->bone[i]));
                                bonenode->AddProperty(wxT("weight"), wxString::Format(wxT("%hhu"), it->boneweight[i]));
                                lastvertchild->SetNext(bonenode);
                                lastvertchild = bonenode;
                            }
                        }
                        if (lastchild) {
                            lastchild->SetNext(vert);
                        } else {
                            child->SetChildren(vert);
                        }
                        lastchild = vert;
                    }
                    for(int l = 0; l < bsh2.indices.size(); l+=3) {
                        wxXmlNode* tri = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, RAWXML_BSH_MESH_TRIANGLE);
                        tri->AddProperty(wxT("a"), wxString::Format(wxT("%hu"), bsh2.indices[l]));
                        tri->AddProperty(wxT("b"), wxString::Format(wxT("%hu"), bsh2.indices[l+1]));
                        tri->AddProperty(wxT("c"), wxString::Format(wxT("%hu"), bsh2.indices[l+2]));
                        lastchild->SetNext(tri);
                        lastchild = tri;
                    }
                }
            } else {
                BAKE_SKIP(child);
                wxXmlNode *subchild = child->GetChildren();
                while (subchild) {
                    DO_CONDITION_COMMENT(subchild);
                    if (subchild->GetName() == RAWXML_BSH_MESH_VERTEX2) {
                        VERTEX2 v;
                        vertex2init(v);
                        v.tu = ParseFloat(subchild, RAWXML_BSH_MESH_VERTEX2, wxT("u"));
                        v.tv = ParseFloat(subchild, RAWXML_BSH_MESH_VERTEX2, wxT("v"));
                        if (subchild->HasProp(wxT("colour"))) {
                            wxString scol = subchild->GetPropVal(wxT("colour"), wxT(""));
                            unsigned long lcol = 0;
                            if (sscanf(scol.ToAscii(), "%lx", &lcol))
                                v.color = lcol;
                        }

                        bool nopos = true;
                        bool nonorm = true;
                        int bone = 0;

                        wxXmlNode *thirdchild = subchild->GetChildren();
                        while (thirdchild) {
                            if (thirdchild->GetName() == RAWXML_BSH_MESH_VERTEX2_P) {
                                nopos = false;
                                ParseVector(thirdchild, v.position, RAWXML_BSH_MESH_VERTEX2_P);
                            } else if (thirdchild->GetName() == RAWXML_BSH_MESH_VERTEX2_N) {
                                nonorm = false;
                                ParseVector(thirdchild, v.normal, RAWXML_BSH_MESH_VERTEX2_N);
                            } else if (thirdchild->GetName() == RAWXML_BSH_MESH_VERTEX2_B) {
                                if (bone == 3)
                                    throw RCT3Exception(wxString::Format(_("Too many boneassignments in bsh(%s)/bshmesh/vertex2."), name.c_str()));
                                v.bone[bone] = ParseSigned(thirdchild, RAWXML_BSH_MESH_VERTEX2_B, wxT("bone"));
                                v.boneweight[bone] = ParseUnsigned(thirdchild, RAWXML_BSH_MESH_VERTEX2_B, wxT("weight"));
                                bone++;
                            } else if COMPILER_WRONGTAG(thirdchild) {
                                throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in bsh(%s)/bshmesh/vertex2."), thirdchild->GetName().c_str(), name.c_str()));
                            }
                            thirdchild = thirdchild->GetNext();
                        }

                        if (nopos || nonorm)
                            throw RCT3Exception(wxString::Format(_("Position or normal missing in bsh(%s)/bshmesh/vertex2."), name.c_str()));

                        if (!(bb1 || bb2))
                            boundsContain(&v.position, &bsh1.bbox1, &bsh1.bbox2);

                        bsh2.vertices.push_back(v);
                    } else if (subchild->GetName() == RAWXML_BSH_MESH_TRIANGLE) {
                        unsigned short index = ParseUnsigned(subchild, RAWXML_BSH_MESH_TRIANGLE, wxT("a"));
                        bsh2.indices.push_back(index);
                        index = ParseUnsigned(subchild, RAWXML_BSH_MESH_TRIANGLE, wxT("b"));
                        bsh2.indices.push_back(index);
                        index = ParseUnsigned(subchild, RAWXML_BSH_MESH_TRIANGLE, wxT("c"));
                        bsh2.indices.push_back(index);
                    } else if COMPILER_WRONGTAG(subchild) {
                        throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in bsh(%s)/bshmesh."), subchild->GetName().c_str(), name.c_str()));
                    }
                    subchild = subchild->GetNext();
                }
            }
            bsh1.meshes.push_back(bsh2);
        } else if (child->GetName() == RAWXML_BSH_BONE) {
            BAKE_SKIP(child);
            cBoneStruct bones;
            wxString bonename = ParseString(child, RAWXML_BSH_BONE, wxT("name"), NULL);
            if (bonename == wxT("Scene Root")) {
                bones = cBoneStruct(true);
            } else {
                bones.name = bonename.ToAscii();
                bones.parentbonenumber = ParseSigned(child, RAWXML_BSH_BONE, wxT("parent"));
                bool missp1 = true;
                bool missp2 = true;
                wxXmlNode *subchild = child->GetChildren();
                while (subchild) {
                    DO_CONDITION_COMMENT(subchild);
                    if (subchild->GetName() == RAWXML_BSH_BONE_POS1) {
                        missp1 = false;
                        ParseMatrix(subchild, bones.pos1, RAWXML_BSH_BONE_POS1);
                    } else if (subchild->GetName() == RAWXML_BSH_BONE_POS2) {
                        missp2 = false;
                        ParseMatrix(subchild, bones.pos2, RAWXML_BSH_BONE_POS2);
                    } else if COMPILER_WRONGTAG(subchild) {
                        throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in bsh(%s)/bshbone(%s)."), subchild->GetName().c_str(), name.c_str(), bonename.c_str()));
                    }
                    subchild = subchild->GetNext();
                }
                if (missp1)
                    throw RCT3Exception(wxString::Format(_("Missing position1 in bsh(%s)/bshbone(%s)."), name.c_str(), bonename.c_str()));
                if (missp2)
                    bones.pos2 = bones.pos1;
            }
            bsh1.bones.push_back(bones);
        } else if COMPILER_WRONGTAG(child) {
            throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in bsh tag '%s'."), child->GetName().c_str(), name.c_str()));
        }
        child = child->GetNext();
    }

    if (m_mode != MODE_BAKE) {
        ovlBSHManager* c_bsh = m_ovl.GetManager<ovlBSHManager>();
        c_bsh->AddModel(bsh1);
    } else if ((!bb1) && (!bb2)) {
        if (m_bake.Index(RAWXML_BSH) != wxNOT_FOUND) {
            wxXmlNode* bb = makeElementNode(RAWXML_BSH_BBOX2);
            bb->AddProperty(wxT("x"), wxString::Format(wxT("%f"), bsh1.bbox2.x));
            bb->AddProperty(wxT("y"), wxString::Format(wxT("%f"), bsh1.bbox2.y));
            bb->AddProperty(wxT("z"), wxString::Format(wxT("%f"), bsh1.bbox2.z));
            node->InsertChild(bb, NULL);

            bb = makeElementNode(RAWXML_BSH_BBOX1);
            bb->AddProperty(wxT("x"), wxString::Format(wxT("%f"), bsh1.bbox1.x));
            bb->AddProperty(wxT("y"), wxString::Format(wxT("%f"), bsh1.bbox1.y));
            bb->AddProperty(wxT("z"), wxString::Format(wxT("%f"), bsh1.bbox1.z));
            node->InsertChild(bb, NULL);
        }
    }
}

void cRawParser::ParseCED(wxXmlNode* node) {
    USE_PREFIX(node);
    cCarriedItemExtra ced;
    ced.name = ParseString(node, RAWXML_CED, wxT("name"), NULL, useprefix).ToAscii();
    ced.nametxt = ParseString(node, RAWXML_CED, wxT("nametxt"), NULL, useprefix).ToAscii();
    ced.icon = ParseString(node, RAWXML_CED, wxT("icon"), NULL, useprefix).ToAscii();
    wxLogVerbose(wxString::Format(_("Adding ced %s to %s."), wxString(ced.name.c_str(), wxConvLocal).c_str(), m_output.GetFullPath().c_str()));

    wxXmlNode *child = node->GetChildren();
    while (child) {
        DO_CONDITION_COMMENT(child);

        if (child->GetName() == RAWXML_CED_MORE) {
            OPTION_PARSE(float, ced.hunger, ParseFloat(child, RAWXML_CED_MORE, wxT("hunger")));
            OPTION_PARSE(float, ced.thirst, ParseFloat(child, RAWXML_CED_MORE, wxT("thirst")));
            OPTION_PARSE(unsigned long, ced.unk1, ParseUnsigned(child, RAWXML_CED_MORE, wxT("u1")));
            OPTION_PARSE(unsigned long, ced.unk4, ParseUnsigned(child, RAWXML_CED_MORE, wxT("u4")));
            OPTION_PARSE(float, ced.unk7, ParseFloat(child, RAWXML_CED_MORE, wxT("u7")));
        } else if COMPILER_WRONGTAG(child) {
            throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in ced tag."), child->GetName().c_str()));
        }
        child = child->GetNext();
    }

    ovlCEDManager* c_ced = m_ovl.GetManager<ovlCEDManager>();
    c_ced->AddExtra(ced);
}

void cRawParser::ParseCHG(wxXmlNode* node) {
    USE_PREFIX(node);
    cChangingRoom room;
    room.name = ParseString(node, RAWXML_CHG, wxT("name"), NULL, useprefix).ToAscii();
    room.attraction.name = ParseString(node, RAWXML_CHG, wxT("nametxt"), NULL, useprefix).ToAscii();
    room.attraction.description = ParseString(node, RAWXML_CHG, wxT("description"), NULL, useprefix).ToAscii();
    room.sid = ParseString(node, RAWXML_CHG, wxT("sid"), NULL, useprefix).ToAscii();
    room.spline = ParseString(node, RAWXML_CHG, wxT("roomspline"), NULL, useprefix).ToAscii();
    wxLogVerbose(wxString::Format(_("Adding chg %s to %s."), wxString(room.name.c_str(), wxConvLocal).c_str(), m_output.GetFullPath().c_str()));

    wxXmlNode *child = node->GetChildren();
    while (child) {
        DO_CONDITION_COMMENT(child);

        if (child->GetName() == RAWXML_ATTRACTION_BASE) {
            USE_PREFIX(child);
            OPTION_PARSE(unsigned long, room.attraction.type, ParseUnsigned(child, RAWXML_ATTRACTION_BASE, wxT("type")));
            ParseStringOption(room.attraction.icon, child, wxT("icon"), NULL, useprefix);
            ParseStringOption(room.spline, child, wxT("attractionspline"), NULL, useprefix);
        } else if (child->GetName() == RAWXML_ATTRACTION_UNKNOWNS) {
            OPTION_PARSE(unsigned long, room.attraction.unk2, ParseUnsigned(child, RAWXML_ATTRACTION_UNKNOWNS, wxT("u2")));
            OPTION_PARSE(long, room.attraction.unk3, ParseSigned(child, RAWXML_ATTRACTION_UNKNOWNS, wxT("u3")));
            OPTION_PARSE(unsigned long, room.attraction.unk4, ParseUnsigned(child, RAWXML_ATTRACTION_UNKNOWNS, wxT("u4")));
            OPTION_PARSE(unsigned long, room.attraction.unk5, ParseUnsigned(child, RAWXML_ATTRACTION_UNKNOWNS, wxT("u5")));
            OPTION_PARSE(unsigned long, room.attraction.unk6, ParseUnsigned(child, RAWXML_ATTRACTION_UNKNOWNS, wxT("u6")));
            OPTION_PARSE(unsigned long, room.attraction.unk9, ParseUnsigned(child, RAWXML_ATTRACTION_UNKNOWNS, wxT("u9")));
            OPTION_PARSE(long, room.attraction.unk10, ParseSigned(child, RAWXML_ATTRACTION_UNKNOWNS, wxT("u10")));
            OPTION_PARSE(unsigned long, room.attraction.unk11, ParseUnsigned(child, RAWXML_ATTRACTION_UNKNOWNS, wxT("u11")));
            OPTION_PARSE(unsigned long, room.attraction.unk12, ParseUnsigned(child, RAWXML_ATTRACTION_UNKNOWNS, wxT("u12")));
            OPTION_PARSE(unsigned long, room.attraction.unk13, ParseUnsigned(child, RAWXML_ATTRACTION_UNKNOWNS, wxT("u13")));
        } else if COMPILER_WRONGTAG(child) {
            throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in sta tag."), child->GetName().c_str()));
        }

        child = child->GetNext();
    }

    ovlCHGManager* c_chg = m_ovl.GetManager<ovlCHGManager>();
    c_chg->AddRoom(room);
}

void cRawParser::ParseCID(wxXmlNode* node) {
    USE_PREFIX(node);
    cCarriedItem cid;
    cid.name = ParseString(node, RAWXML_CID, wxT("name"), NULL, useprefix).ToAscii();
    cid.nametxt = ParseString(node, RAWXML_CID, wxT("nametxt"), NULL, useprefix).ToAscii();
    cid.pluralnametxt = ParseString(node, RAWXML_CID, wxT("pluralnametxt"), NULL, useprefix).ToAscii();
    wxLogVerbose(wxString::Format(_("Adding cid %s to %s."), wxString(cid.name.c_str(), wxConvLocal).c_str(), m_output.GetFullPath().c_str()));

    wxXmlNode *child = node->GetChildren();
    while (child) {
        DO_CONDITION_COMMENT(child);

        if (child->GetName() == RAWXML_CID_SHAPE) {
            USE_PREFIX(child);
            cid.shape.shaped = ParseUnsigned(child, RAWXML_CID_SHAPE, wxT("shaped"));
            if (cid.shape.shaped) {
                cid.shape.shape1 = ParseString(child, RAWXML_CID_SHAPE, wxT("shape1"), NULL, useprefix).ToAscii();
                cid.shape.shape2 = ParseString(child, RAWXML_CID_SHAPE, wxT("shape1"), NULL, useprefix).ToAscii();
            } else {
                cid.shape.MakeBillboard();
                cid.shape.fts = ParseString(child, RAWXML_CID_SHAPE, wxT("ftx"), NULL, useprefix).ToAscii();
            }
            OPTION_PARSE(float, cid.shape.unk9, ParseFloat(child, RAWXML_CID_SHAPE, wxT("distance")));
            OPTION_PARSE(unsigned long, cid.shape.defaultcolour, ParseUnsigned(child, RAWXML_CID_SHAPE, wxT("defaultcolour")));
            OPTION_PARSE(float, cid.shape.scalex, ParseFloat(child, RAWXML_CID_SHAPE, wxT("scalex")));
            OPTION_PARSE(float, cid.shape.scaley, ParseFloat(child, RAWXML_CID_SHAPE, wxT("scaley")));
        } else if (child->GetName() == RAWXML_CID_MORE) {
            USE_PREFIX(child);
            OPTION_PARSE(unsigned long, cid.addonpack, ParseUnsigned(child, RAWXML_CID_MORE, wxT("addonpack")));
            ParseStringOption(cid.icon, child, wxT("icon"), NULL, useprefix);
        } else if (child->GetName() == RAWXML_CID_EXTRA) {
            USE_PREFIX(child);
            wxString extra = ParseString(child, RAWXML_CID_EXTRA, wxT("name"), NULL, useprefix);
            cid.extras.push_back(std::string(extra.ToAscii()));
        } else if (child->GetName() == RAWXML_CID_SETTINGS) {
            OPTION_PARSE(unsigned long, cid.settings.flags, ParseUnsigned(child, RAWXML_CID_SETTINGS, wxT("flags")));
            OPTION_PARSE(long, cid.settings.ageclass, ParseSigned(child, RAWXML_CID_SETTINGS, wxT("ageclass")));
            OPTION_PARSE(unsigned long, cid.settings.type, ParseUnsigned(child, RAWXML_CID_SETTINGS, wxT("type")));
            OPTION_PARSE(float, cid.settings.hunger, ParseFloat(child, RAWXML_CID_SETTINGS, wxT("hunger")));
            OPTION_PARSE(float, cid.settings.thirst, ParseFloat(child, RAWXML_CID_SETTINGS, wxT("thirst")));
            OPTION_PARSE(float, cid.settings.lightprotectionfactor, ParseFloat(child, RAWXML_CID_SETTINGS, wxT("lightprotectionfactor")));
        } else if (child->GetName() == RAWXML_CID_TRASH) {
            USE_PREFIX(child);
            ParseStringOption(cid.trash.wrapper, child, wxT("wrapper"), NULL, useprefix);
            OPTION_PARSE(float, cid.trash.trash1, ParseFloat(child, RAWXML_CID_TRASH, wxT("trash1")));
            OPTION_PARSE(long, cid.trash.trash2, ParseSigned(child, RAWXML_CID_TRASH, wxT("trash2")));
        } else if (child->GetName() == RAWXML_CID_SOAKED) {
            ParseStringOption(cid.soaked.male_morphmesh_body, child, wxT("malebody"), NULL, false);
            ParseStringOption(cid.soaked.male_morphmesh_legs, child, wxT("malelegs"), NULL, false);
            ParseStringOption(cid.soaked.female_morphmesh_body, child, wxT("femalebody"), NULL, false);
            ParseStringOption(cid.soaked.female_morphmesh_legs, child, wxT("femalelegs"), NULL, false);
            OPTION_PARSE(unsigned long, cid.soaked.unk38, ParseUnsigned(child, RAWXML_CID_SOAKED, wxT("unknown")));
            ParseStringOption(cid.soaked.textureoption, child, wxT("textureoption"), NULL, false);
        } else if (child->GetName() == RAWXML_CID_SIZEUNK) {
            OPTION_PARSE(unsigned long, cid.size.unk17, ParseUnsigned(child, RAWXML_CID_SIZEUNK, wxT("u1")));
            OPTION_PARSE(unsigned long, cid.size.unk18, ParseUnsigned(child, RAWXML_CID_SIZEUNK, wxT("u2")));
            OPTION_PARSE(unsigned long, cid.size.unk19, ParseUnsigned(child, RAWXML_CID_SIZEUNK, wxT("u3")));
            OPTION_PARSE(unsigned long, cid.size.unk20, ParseUnsigned(child, RAWXML_CID_SIZEUNK, wxT("u4")));
        } else if (child->GetName() == RAWXML_CID_UNK) {
            OPTION_PARSE(unsigned long, cid.unknowns.unk1, ParseUnsigned(child, RAWXML_CID_UNK, wxT("u1")));
            OPTION_PARSE(unsigned long, cid.unknowns.unk3, ParseUnsigned(child, RAWXML_CID_UNK, wxT("u3")));
            OPTION_PARSE(unsigned long, cid.unknowns.unk26, ParseUnsigned(child, RAWXML_CID_UNK, wxT("u26")));
            OPTION_PARSE(unsigned long, cid.unknowns.unk27, ParseUnsigned(child, RAWXML_CID_UNK, wxT("u27")));
            OPTION_PARSE(unsigned long, cid.unknowns.unk28, ParseUnsigned(child, RAWXML_CID_UNK, wxT("u28")));
            OPTION_PARSE(unsigned long, cid.unknowns.unk33, ParseUnsigned(child, RAWXML_CID_UNK, wxT("u33")));
        } else if COMPILER_WRONGTAG(child) {
            throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in cid tag."), child->GetName().c_str()));
        }
        child = child->GetNext();
    }

    ovlCIDManager* c_cid = m_ovl.GetManager<ovlCIDManager>();
    c_cid->AddItem(cid);
}

void cRawParser::ParseFTX(wxXmlNode* node) {
    USE_PREFIX(node);
    cFlexiTextureInfoStruct c_fti;

    wxString name = ParseString(node, RAWXML_FTX, wxT("name"), NULL, useprefix);
    c_fti.name = name.ToAscii();
    OPTION_PARSE(unsigned long, c_fti.dimension, ParseUnsigned(node, RAWXML_FTX, wxT("dimension")));
    OPTION_PARSE(unsigned long, c_fti.recolourable, ParseUnsigned(node, RAWXML_FTX, wxT("recolourable")));
    OPTION_PARSE(unsigned long, c_fti.fps, ParseUnsigned(node, RAWXML_FTX, wxT("fps")));
//    wxFileName texture = ParseString(node, RAWXML_TEX, wxT("texture"));
//    if (!texture.IsAbsolute())
//        texture.MakeAbsolute(m_input.GetPathWithSep());
    wxLogVerbose(wxString::Format(_("Adding ftx %s to %s."), name.c_str(), m_output.GetFullPath().c_str()));
    counted_array_ptr<unsigned char> data;

    wxXmlNode* child = node->GetChildren();
    while (child) {
        DO_CONDITION_COMMENT(child);

        if (child->GetName() == RAWXML_FTX_ANIMATION) {
            if (m_mode == MODE_BAKE) {
                child = child->GetNext();
                continue;
            }
            unsigned long frame = ParseUnsigned(child, RAWXML_FTX_ANIMATION, wxT("index"));
            unsigned long rep = 1;
            OPTION_PARSE(unsigned long, rep, ParseUnsigned(child, RAWXML_FTX_ANIMATION, wxT("repeat")));
            for (unsigned long i = 0; i < rep; ++i)
                c_fti.animation.push_back(frame);
        } else if (child->GetName() == RAWXML_FTX_FRAME) {
            cFlexiTextureStruct c_fts;
            c_fts.recolourable = -1;
            OPTION_PARSE(unsigned long, c_fts.dimension, ParseUnsigned(child, RAWXML_FTX_FRAME, wxT("dimension")));
            OPTION_PARSE(unsigned long, c_fts.recolourable, ParseUnsigned(child, RAWXML_FTX_FRAME, wxT("recolourable")));
            unsigned char alphacutoff = 0;
            OPTION_PARSE(unsigned long, alphacutoff, ParseUnsigned(child, RAWXML_FTX_FRAME_IMAGE, wxT("alphacutoff")));
            if (c_fts.recolourable == -1)
                c_fts.recolourable = c_fti.recolourable;
            if (c_fts.dimension == 0)
                c_fts.dimension = c_fti.dimension;

            wxXmlNode* subchild = child->GetChildren();
            while (subchild) {
                DO_CONDITION_COMMENT(subchild);

                if (subchild->GetName() == RAWXML_FTX_FRAME_IMAGE) {
                    if ((c_fts.palette.get() || c_fts.texture.get()) && (m_mode != MODE_BAKE))
                        throw RCT3Exception(wxString(wxT("ftx tag '"))+name+wxT("'/ftxframe/image: multiple datasources."));

                    bool usealpha = false;
                    OPTION_PARSE(unsigned long, usealpha, ParseUnsigned(subchild, RAWXML_FTX_FRAME_IMAGE, wxT("usealpha")));

                    wxString t = subchild->GetNodeContent();
                    bool filevar = MakeVariable(t);
                    wxFSFileName texture = t;
                     if (m_mode == MODE_BAKE) {
                        if (m_bake.Index(RAWXML_FTX) == wxNOT_FOUND) {
                            if (!filevar) {
                                if (m_bake.Index(RAWBAKE_ABSOLUTE) == wxNOT_FOUND) {
                                    texture.MakeRelativeTo(m_bakeroot.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME));
                                }
                                delete subchild->GetChildren();
                                subchild->SetChildren(new wxXmlNode(NULL, wxXML_TEXT_NODE, wxT(""), texture.GetFullPath()));
                            }
                            subchild = subchild->GetNext();
                            continue;
                        }
                    }
//                    if (!texture.IsFileReadable())
//                        throw RCT3Exception(_("ftx/ftxframe tag: File not readable: ") + texture.GetFullPath());
                    try {
                        wxGXImage img(texture.GetFullPath(), false);
                        if (!img.Ok())
                            throw RCT3Exception(_("ftx/ftxframe tag: Error reading image file: ") + texture.GetFullPath());

                        if (c_fts.dimension) {
                            if ((img.GetWidth() != c_fts.dimension) || (img.GetHeight() != c_fts.dimension)) {
                                img.Rescale(c_fts.dimension, c_fts.dimension);
                            }
                        } else {
                            int width = img.GetWidth();
                            int height = img.GetHeight();
                            if (width != height) {
                                throw Magick::Exception("No dimension set and texture is not square");
                            }
                            if ((1 << local_log2(width)) != width) {
                                throw Magick::Exception("Texture's width/height is not a power of 2");
                            }
                            c_fts.dimension = width;
                        }

                        if (!c_fti.dimension)
                            c_fti.dimension = c_fts.dimension;

                        img.flip();
                        c_fts.palette = counted_array_ptr<unsigned char>(new unsigned char[256 * sizeof(RGBQUAD)]);
                        memset(c_fts.palette.get(), 0, 256 * sizeof(RGBQUAD));
                        c_fts.texture = counted_array_ptr<unsigned char>(new unsigned char[c_fts.dimension * c_fts.dimension]);
                        if (usealpha)
                            c_fts.alpha = counted_array_ptr<unsigned char>(new unsigned char[c_fts.dimension * c_fts.dimension]);

                        if (c_fts.recolourable && (img.type() != Magick::PaletteType)) {
                            memcpy(c_fts.palette.get(), cFlexiTexture::GetRGBPalette(), 256 * sizeof(RGBQUAD));
                            img.GetAs8bitForced(c_fts.texture.get(), c_fts.palette.get(), true);
                        } else {
                            img.GetAs8bit(c_fts.texture.get(), c_fts.palette.get());
                        }

                        if (usealpha) {
                            img.GetAlpha(c_fts.alpha.get());
                        }

                        for (unsigned int j = 0; j < 256; j++)
                            reinterpret_cast<RGBQUAD*>(c_fts.palette.get())[j].rgbReserved = alphacutoff;

                        if (m_mode == MODE_BAKE) {
                            unsigned long outsize = 4 * ((c_fts.dimension * c_fts.dimension > 256 * sizeof(RGBQUAD))?c_fts.dimension * c_fts.dimension:256 * sizeof(RGBQUAD));
                            unsigned long outlen = outsize;
                            counted_array_ptr<char> outbuf(new char[outsize]) ;
                            int bret = base64_encode(c_fts.palette.get(), 256 * sizeof(RGBQUAD), reinterpret_cast<unsigned char*>(outbuf.get()), &outlen);
                            switch (bret) {
                                case CRYPT_OK:
                                    break;
                                case CRYPT_BUFFER_OVERFLOW:
                                    throw RCT3Exception(wxString(_("Buffer overflow baking palette '")) + texture.GetFullPath() + _("' in tex tag ") + name);
                                default:
                                    throw RCT3Exception(wxString(_("Unknown base64 error baking palette '")) + texture.GetFullPath() + _("' in tex tag ") + name);
                            }
                            subchild->SetName(RAWXML_FTX_FRAME_PDATA);
                            subchild->DeleteProperty(wxT("usealpha"));
                            delete subchild->GetChildren();
                            subchild->SetChildren(new wxXmlNode(NULL, wxXML_TEXT_NODE, wxT(""), wxString(outbuf.get(), wxConvLocal)));

                            outlen = outsize;
                            bret = base64_encode(c_fts.texture.get(), c_fts.dimension * c_fts.dimension, reinterpret_cast<unsigned char*>(outbuf.get()), &outlen);
                            switch (bret) {
                                case CRYPT_OK:
                                    break;
                                case CRYPT_BUFFER_OVERFLOW:
                                    throw RCT3Exception(wxString(_("Buffer overflow baking texture '")) + texture.GetFullPath() + _("' in tex tag ") + name);
                                default:
                                    throw RCT3Exception(wxString(_("Unknown base64 error baking texture '")) + texture.GetFullPath() + _("' in tex tag ") + name);
                            }
                            wxXmlNode* newnode = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, RAWXML_FTX_FRAME_TDATA);
                            CopyBaseAttributes(subchild, newnode);
                            newnode->SetChildren(new wxXmlNode(NULL, wxXML_TEXT_NODE, wxT(""), wxString(outbuf.get(), wxConvLocal)));
                            newnode->SetNext(subchild->GetNext());
                            subchild->SetNext(newnode);
                            subchild = newnode;

                            if (usealpha) {
                                outlen = outsize;
                                bret = base64_encode(c_fts.alpha.get(), c_fts.dimension * c_fts.dimension, reinterpret_cast<unsigned char*>(outbuf.get()), &outlen);
                                switch (bret) {
                                    case CRYPT_OK:
                                        break;
                                    case CRYPT_BUFFER_OVERFLOW:
                                        throw RCT3Exception(wxString(_("Buffer overflow baking alpha '")) + texture.GetFullPath() + _("' in tex tag ") + name);
                                    default:
                                        throw RCT3Exception(wxString(_("Unknown base64 error baking alpha '")) + texture.GetFullPath() + _("' in tex tag ") + name);
                                }
                                wxXmlNode* newnode = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, RAWXML_FTX_FRAME_ADATA);
                                CopyBaseAttributes(subchild, newnode);
                                newnode->SetChildren(new wxXmlNode(NULL, wxXML_TEXT_NODE, wxT(""), wxString(outbuf.get(), wxConvLocal)));
                                newnode->SetNext(subchild->GetNext());
                                subchild->SetNext(newnode);
                                subchild = newnode;
                            }
                            child->DeleteProperty(wxT("dimension"));
                            child->AddProperty(wxT("dimension"), wxString::Format(wxT("%ld"), c_fts.dimension));
                        }

                    } catch (Magick::Exception& e) {
                        throw RCT3Exception(wxString::Format(_("ftx/ftxframe tag: GarphicsMagik exception: %s"), e.what()));
                    }

                } else if (subchild->GetName() == RAWXML_FTX_FRAME_ALPHA) {
                    if (c_fts.alpha.get() && (m_mode != MODE_BAKE))
                        throw RCT3Exception(wxString(wxT("ftx tag '"))+name+wxT("'/ftxframe/alpha: multiple datasources."));

                    wxString t = subchild->GetNodeContent();
                    bool filevar = MakeVariable(t);
                    wxFSFileName texture = t;
                    if (!texture.IsAbsolute())
                        texture.MakeAbsolute(m_input.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME));
                    if (m_mode == MODE_BAKE) {
                        if (m_bake.Index(RAWXML_FTX) == wxNOT_FOUND) {
                            if (!filevar) {
                                if (m_bake.Index(RAWBAKE_ABSOLUTE) == wxNOT_FOUND) {
                                    texture.MakeRelativeTo(m_bakeroot.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME));
                                }
                                delete subchild->GetChildren();
                                subchild->SetChildren(new wxXmlNode(NULL, wxXML_TEXT_NODE, wxT(""), texture.GetFullPath()));
                            }
                            subchild = subchild->GetNext();
                            continue;
                        }
                    }
//                    if (!texture.IsFileReadable())
//                        throw RCT3Exception(_("ftx/ftxframe tag: File not readable: ") + texture.GetFullPath());

                    try {
                        wxGXImage img(texture.GetFullPath(), false);
                        if (!img.Ok())
                            throw RCT3Exception(_("ftx/ftxframe tag: Error reading image file: ") + texture.GetFullPath());

                        if (c_fts.dimension) {
                            if ((img.GetWidth() != c_fts.dimension) || (img.GetHeight() != c_fts.dimension)) {
                                img.Rescale(c_fts.dimension, c_fts.dimension);
                            }
                        } else {
                            int width = img.GetWidth();
                            int height = img.GetHeight();
                            if (width != height) {
                                throw Magick::Exception("No dimension set and texture is not square");
                            }
                            if ((1 << local_log2(width)) != width) {
                                throw Magick::Exception("Texture's width/height is not a power of 2");
                            }
                            c_fts.dimension = width;
                        }

                        if (!c_fti.dimension)
                            c_fti.dimension = c_fts.dimension;

                        img.flip();
                        c_fts.alpha = counted_array_ptr<unsigned char>(new unsigned char[c_fts.dimension * c_fts.dimension]);
                        img.GetGrayscale(c_fts.alpha.get());

                        if (m_mode == MODE_BAKE) {
                            unsigned long outsize = 4 * c_fts.dimension * c_fts.dimension;
                            unsigned long outlen = outsize;
                            counted_array_ptr<char> outbuf(new char[outsize]) ;
                            int bret = base64_encode(c_fts.alpha.get(), c_fts.dimension * c_fts.dimension, reinterpret_cast<unsigned char*>(outbuf.get()), &outlen);
                            switch (bret) {
                                case CRYPT_OK:
                                    break;
                                case CRYPT_BUFFER_OVERFLOW:
                                    throw RCT3Exception(wxString(_("Buffer overflow baking alpha '")) + texture.GetFullPath() + _("' in tex tag ") + name);
                                default:
                                    throw RCT3Exception(wxString(_("Unknown base64 error baking alpha '")) + texture.GetFullPath() + _("' in tex tag ") + name);
                            }
                            subchild->SetName(RAWXML_FTX_FRAME_ADATA);
                            delete subchild->GetChildren();
                            subchild->SetChildren(new wxXmlNode(NULL, wxXML_TEXT_NODE, wxT(""), wxString(outbuf.get(), wxConvLocal)));
                            child->DeleteProperty(wxT("dimension"));
                            child->AddProperty(wxT("dimension"), wxString::Format(wxT("%ld"), c_fts.dimension));
                        }

                    } catch (Magick::Exception& e) {
                        throw RCT3Exception(wxString::Format(_("ftx/ftxframe tag: GarphicsMagik exception: %s"), e.what()));
                    }
                } else if (subchild->GetName() == RAWXML_FTX_FRAME_PDATA) {
                    if (c_fts.palette.get() && (m_mode != MODE_BAKE))
                        throw RCT3Exception(wxString(wxT("ftx tag '"))+name+wxT("'/ftxframe/palettedata: multiple datasources."));

                    BAKE_SKIP(subchild);

                    wxString tex = subchild->GetNodeContent();
                    MakeVariable(tex);
                    c_fts.palette = counted_array_ptr<unsigned char>(new unsigned char[256 * sizeof(RGBQUAD)]);
                    unsigned long outlen = 256 * sizeof(RGBQUAD);
#ifdef UNICODE
                    int bret = base64_decode(reinterpret_cast<const unsigned char*>(tex.ToAscii().data()), tex.Length(), c_fts.palette.get(), &outlen);
#else
                    int bret = base64_decode(reinterpret_cast<const unsigned char*>(tex.ToAscii()), tex.Length(), c_fts.palette.get(), &outlen);
#endif
                    switch (bret) {
                        case CRYPT_OK:
                            break;
                        case CRYPT_INVALID_PACKET:
                            throw RCT3Exception(wxString(_("Decoding error in ftx/ftxframe/palettedata tag ")) + name);
                        case CRYPT_BUFFER_OVERFLOW:
                            throw RCT3Exception(wxString(_("Buffer overflow decoding ftx/ftxframe/palettedata tag ")) + name);
                        default:
                            throw RCT3Exception(wxString(_("Unknown base64 error decoding ftx/ftxframe/palettedata tag ")) + name);
                    }
                    if (outlen != 256 * sizeof(RGBQUAD))
                        throw RCT3Exception(wxString::Format(_("Datasize mismatch (%ld/%d) error in ftx('%s')/ftxframe/palettedata tag."), outlen, 256 * sizeof(RGBQUAD),name.c_str()));
                } else if (subchild->GetName() == RAWXML_FTX_FRAME_TDATA) {
                    if (c_fts.texture.get() && (m_mode != MODE_BAKE))
                        throw RCT3Exception(wxString(wxT("ftx tag '"))+name+wxT("'/ftxframe/texturedata: multiple datasources."));
                    if (m_mode == MODE_BAKE) {
                        subchild = subchild->GetNext();
                        continue;
                    }

                    if (!c_fts.dimension)
                        throw RCT3Exception(wxString(wxT("ftx tag '"))+name+wxT("'/ftxframe: dimension missing for decoding texturedata."));

                    if (!c_fti.dimension)
                        c_fti.dimension = c_fts.dimension;

                    wxString tex = subchild->GetNodeContent();
                    MakeVariable(tex);
                    c_fts.texture = counted_array_ptr<unsigned char>(new unsigned char[c_fts.dimension * c_fts.dimension]);
                    unsigned long outlen = c_fts.dimension * c_fts.dimension;
#ifdef UNICODE
                    int bret = base64_decode(reinterpret_cast<const unsigned char*>(tex.ToAscii().data()), tex.Length(), c_fts.texture.get(), &outlen);
#else
                    int bret = base64_decode(reinterpret_cast<const unsigned char*>(tex.ToAscii()), tex.Length(), c_fts.texture.get(), &outlen);
#endif
                    switch (bret) {
                        case CRYPT_OK:
                            break;
                        case CRYPT_INVALID_PACKET:
                            throw RCT3Exception(wxString(_("Decoding error in ftx/ftxframe/texturedata tag ")) + name);
                        case CRYPT_BUFFER_OVERFLOW:
                            throw RCT3Exception(wxString(_("Buffer overflow decoding ftx/ftxframe/texturedata tag ")) + name);
                        default:
                            throw RCT3Exception(wxString(_("Unknown base64 error decoding ftx/ftxframe/texturedata tag ")) + name);
                    }
                    if (outlen != c_fts.dimension * c_fts.dimension)
                        throw RCT3Exception(wxString(_("Datasize mismatch error in ftx/ftxframe/texturedata tag ")) + name);
                } else if (subchild->GetName() == RAWXML_FTX_FRAME_ADATA) {
                    if (c_fts.alpha.get() && (m_mode != MODE_BAKE))
                        throw RCT3Exception(wxString(wxT("ftx tag '"))+name+wxT("'/ftxframe/alphadata: multiple datasources."));

                    if (m_mode == MODE_BAKE) {
                        subchild = subchild->GetNext();
                        continue;
                    }

                    if (!c_fts.dimension)
                        throw RCT3Exception(wxString(wxT("ftx tag '"))+name+wxT("'/ftxframe: dimension missing for decoding alphadata."));

                    wxString tex = subchild->GetNodeContent();
                    MakeVariable(tex);
                    c_fts.alpha = counted_array_ptr<unsigned char>(new unsigned char[c_fts.dimension * c_fts.dimension]);
                    unsigned long outlen = c_fts.dimension * c_fts.dimension;
#ifdef UNICODE
                    int bret = base64_decode(reinterpret_cast<const unsigned char*>(tex.ToAscii().data()), tex.Length(), c_fts.alpha.get(), &outlen);
#else
                    int bret = base64_decode(reinterpret_cast<const unsigned char*>(tex.ToAscii()), tex.Length(), c_fts.alpha.get(), &outlen);
#endif
                    switch (bret) {
                        case CRYPT_OK:
                            break;
                        case CRYPT_INVALID_PACKET:
                            throw RCT3Exception(wxString(_("Decoding error in ftx/ftxframe/alphadata tag ")) + name);
                        case CRYPT_BUFFER_OVERFLOW:
                            throw RCT3Exception(wxString(_("Buffer overflow decoding ftx/ftxframe/alphadata tag ")) + name);
                        default:
                            throw RCT3Exception(wxString(_("Unknown base64 error decoding ftx/ftxframe/alphadata tag ")) + name);
                    }
                    if (outlen != c_fts.dimension * c_fts.dimension)
                        throw RCT3Exception(wxString(_("Datasize mismatch error in ftx/ftxframe/alphadata tag ")) + name);
                } else if COMPILER_WRONGTAG(subchild) {
                    throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in ftx/ftxframe tag."), child->GetName().c_str()));
                }

                subchild = subchild->GetNext();
            }

            if (m_mode != MODE_BAKE) {
                if (!c_fts.palette.get())
                    throw RCT3Exception(wxString(wxT("ftx tag '"))+name+wxT("'/ftxframe: palette data missing."));
                if (!c_fts.texture.get())
                    throw RCT3Exception(wxString(wxT("ftx tag '"))+name+wxT("'/ftxframe: texture data missing."));
                c_fti.frames.push_back(c_fts);
            }
        } else if COMPILER_WRONGTAG(child) {
            throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in ftx tag."), child->GetName().c_str()));
        }
        child = child->GetNext();

    }

    if (m_mode != MODE_BAKE) {
        ovlFTXManager* c_ftx = m_ovl.GetManager<ovlFTXManager>();
        c_ftx->AddTexture(c_fti);
    }
}

void cRawParser::ParseSAT(wxXmlNode* node) {
    USE_PREFIX(node);
    cSpecialAttraction spa;
    wxString name = ParseString(node, RAWXML_SAT, wxT("name"), NULL, useprefix);
    spa.name = name.ToAscii();
    spa.attraction.name = ParseString(node, RAWXML_SAT, wxT("nametxt"), NULL, useprefix).ToAscii();
    spa.attraction.description = ParseString(node, RAWXML_SAT, wxT("description"), NULL, useprefix).ToAscii();
    spa.sid = ParseString(node, RAWXML_SAT, wxT("sid"), NULL, useprefix).ToAscii();
    wxLogVerbose(wxString::Format(_("Adding sat %s to %s."), name.c_str(), m_output.GetFullPath().c_str()));

    wxXmlNode *child = node->GetChildren();
    while (child) {
        DO_CONDITION_COMMENT(child);

        if (child->GetName() == RAWXML_ATTRACTION_BASE) {
            USE_PREFIX(child);
            OPTION_PARSE(unsigned long, spa.attraction.type, ParseUnsigned(child, RAWXML_ATTRACTION_BASE, wxT("type")));
            ParseStringOption(spa.attraction.icon, child, wxT("icon"), NULL, useprefix);
            ParseStringOption(spa.attraction.spline, child, wxT("spline"), NULL, useprefix);
        } else if (child->GetName() == RAWXML_ATTRACTION_UNKNOWNS) {
            OPTION_PARSE(unsigned long, spa.attraction.unk2, ParseUnsigned(child, RAWXML_ATTRACTION_UNKNOWNS, wxT("u2")));
            OPTION_PARSE(long, spa.attraction.unk3, ParseSigned(child, RAWXML_ATTRACTION_UNKNOWNS, wxT("u3")));
            OPTION_PARSE(unsigned long, spa.attraction.unk4, ParseUnsigned(child, RAWXML_ATTRACTION_UNKNOWNS, wxT("u4")));
            OPTION_PARSE(unsigned long, spa.attraction.unk5, ParseUnsigned(child, RAWXML_ATTRACTION_UNKNOWNS, wxT("u5")));
            OPTION_PARSE(unsigned long, spa.attraction.unk6, ParseUnsigned(child, RAWXML_ATTRACTION_UNKNOWNS, wxT("u6")));
            OPTION_PARSE(unsigned long, spa.attraction.unk9, ParseUnsigned(child, RAWXML_ATTRACTION_UNKNOWNS, wxT("u9")));
            OPTION_PARSE(long, spa.attraction.unk10, ParseSigned(child, RAWXML_ATTRACTION_UNKNOWNS, wxT("u10")));
            OPTION_PARSE(unsigned long, spa.attraction.unk11, ParseUnsigned(child, RAWXML_ATTRACTION_UNKNOWNS, wxT("u11")));
            OPTION_PARSE(unsigned long, spa.attraction.unk12, ParseUnsigned(child, RAWXML_ATTRACTION_UNKNOWNS, wxT("u12")));
            OPTION_PARSE(unsigned long, spa.attraction.unk13, ParseUnsigned(child, RAWXML_ATTRACTION_UNKNOWNS, wxT("u13")));
        } else if COMPILER_WRONGTAG(child) {
            throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in sat tag '%s'."), child->GetName().c_str(), name.c_str()));
        }

        child = child->GetNext();
    }

    ovlSATManager* c_sat = m_ovl.GetManager<ovlSATManager>();
    c_sat->AddAttraction(spa);
}

void cRawParser::ParseSHS(wxXmlNode* node) {
    USE_PREFIX(node);
    cStaticShape1 sh1;
    wxString name = ParseString(node, RAWXML_SHS, wxT("name"), NULL, useprefix);
    sh1.name = name.ToAscii();
    boundsInit(&sh1.bbox1, &sh1.bbox2);
    wxXmlNode* bb1 = NULL;
    wxXmlNode* bb2 = NULL;
    wxLogVerbose(wxString::Format(_("Adding shs %s to %s."), name.c_str(), m_output.GetFullPath().c_str()));

    wxXmlNode *child = node->GetChildren();
    while (child) {
        DO_CONDITION_COMMENT(child);

        if (child->GetName() == RAWXML_SHS_BBOX1) {
            bb1 = child;
            ParseVector(child, sh1.bbox1, RAWXML_SHS_BBOX1);
        } else if (child->GetName() == RAWXML_SHS_BBOX2) {
            bb2 = child;
            ParseVector(child, sh1.bbox2, RAWXML_SHS_BBOX2);
        } else if (child->GetName() == RAWXML_SHS_MESH) {
            USE_PREFIX(child);
            cStaticShape2 sh2;
            sh2.fts = ParseString(child, RAWXML_SHS_MESH, wxT("ftx"), NULL, useprefix).ToAscii();
            sh2.texturestyle = ParseString(child, RAWXML_SHS_MESH, wxT("txs"), NULL).ToAscii();
            OPTION_PARSE(unsigned long, sh2.placetexturing, ParseUnsigned(child, RAWXML_SHS_MESH, wxT("placing")));
            OPTION_PARSE(unsigned long, sh2.textureflags, ParseUnsigned(child, RAWXML_SHS_MESH, wxT("flags")));
            OPTION_PARSE(unsigned long, sh2.sides, ParseUnsigned(child, RAWXML_SHS_MESH, wxT("sides")));

            if (child->HasProp(wxT("meshname"))) {
                wxString meshname = ParseString(child, RAWXML_SHS_MESH, wxT("meshname"), NULL);
                bool modelfilevar = false;
                wxFSFileName modelfile = ParseString(child, RAWXML_SHS_MESH, wxT("modelfile"), &modelfilevar);
                wxString hand = child->GetPropVal(wxT("handedness"), wxT("left"));
                wxString up = child->GetPropVal(wxT("up"), wxT("y"));
                c3DLoaderOrientation ori = ORIENTATION_LEFT_YUP;
                if (hand == wxT("left")) {
                    if (up == wxT("x")) {
                        ori = ORIENTATION_LEFT_XUP;
                    } else if (up == wxT("y")) {
                        ori = ORIENTATION_LEFT_YUP;
                    } else if (up == wxT("z")) {
                        ori = ORIENTATION_LEFT_ZUP;
                    } else {
                        throw RCT3Exception(wxString::Format(_("Unknown value '%s' for up attribute in shs(%s)/shsmesh tag."), up.c_str(), name.c_str()));
                    }
                } else if (hand == wxT("right")) {
                    if (up == wxT("x")) {
                        ori = ORIENTATION_RIGHT_XUP;
                    } else if (up == wxT("y")) {
                        ori = ORIENTATION_RIGHT_YUP;
                    } else if (up == wxT("z")) {
                        ori = ORIENTATION_RIGHT_ZUP;
                    } else {
                        throw RCT3Exception(wxString::Format(_("Unknown value '%s' for up attribute in shs(%s)/shsmesh tag."), up.c_str(), name.c_str()));
                    }
                } else {
                    throw RCT3Exception(wxString::Format(_("Unknown value '%s' for handedness attribute in shs(%s)/shsmesh tag."), up.c_str(), name.c_str()));
                }

                if (!modelfile.IsAbsolute())
                    modelfile.MakeAbsolute(m_input.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME));

                if (m_mode == MODE_BAKE) {
                    if (m_bake.Index(RAWXML_SHS) == wxNOT_FOUND) {
                        if (!modelfilevar) {
                            if (m_bake.Index(RAWBAKE_ABSOLUTE) == wxNOT_FOUND) {
                                modelfile.MakeRelativeTo(m_bakeroot.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME));
                            }
                            child->DeleteProperty(wxT("modelfile"));
                            child->AddProperty(wxT("modelfile"), modelfile.GetFullPath());
                        }
                        child = child->GetNext();
                        continue;
                    }
                }

//                if (!modelfile.IsFileReadable())
//                    throw RCT3Exception(_("shs tag: Model file not readable: ") + modelfile.GetFullPath());

                counted_ptr<c3DLoader> model(c3DLoader::LoadFile(modelfile.GetFullPath().fn_str()));
                if (!model.get())
                    throw RCT3Exception(_("shs tag: Model file not readable or has unknown format: ") + modelfile.GetFullPath());

                int meshnr = -1;
                for (int i = 0; i < model->GetObjectCount(); ++i) {
                    if (meshname == model->GetObjectName(i)) {
                        meshnr = i;
                        break;
                    }
                }
                if (meshnr == -1)
                    throw RCT3Exception(wxString::Format(_("shs tag: Model file '%s' does not contain a mesh called '%s'."), modelfile.GetFullPath().c_str(), meshname.c_str()));

                unsigned long fudgenormals = CMS_FUDGE_NONE;
                OPTION_PARSE(unsigned long, fudgenormals, ParseUnsigned(child, RAWXML_SHS_MESH, wxT("fudge")));

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

                wxXmlNode *subchild = child->GetChildren();
                while (subchild) {
                    if (subchild->GetName() == RAWXML_SHS_MESH_TRANSFORM) {
                        if ((ori != ORIENTATION_LEFT_YUP) && (m_mode != MODE_INSTALL))
                            wxLogWarning(_("You've set handedness and up as well as giving a transformation matrix. The handedness/up is ignored for transforming the mesh."));
                        ParseMatrix(subchild, m, RAWXML_SHS_MESH_TRANSFORM);
                        do_matrix = true;
                    } else if COMPILER_WRONGTAG(subchild) {
                        throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in shs(%s)/shsmesh."), subchild->GetName().c_str(), name.c_str()));
                    }
                    subchild = subchild->GetNext();
                }

                boundsInit(&temp_min, &temp_max);
                model->FetchObject(meshnr, &sh2, &temp_min, &temp_max,
                                          do_matrix?&m:NULL,
                                          c_pfudge_normal);
                if (!(bb1 || bb2))
                    boundsContain(&temp_min, &temp_max, &sh1.bbox1, &sh1.bbox2);
                if (fudgenormals == CMS_FUDGE_RIM) {
                    c3DLoader::FlattenNormals(&sh2, temp_min, temp_max);
                }
                if (m_mode == MODE_BAKE) {
                    child->DeleteProperty(wxT("meshname"));
                    child->DeleteProperty(wxT("modelfile"));
                    child->DeleteProperty(wxT("handedness"));
                    child->DeleteProperty(wxT("up"));
                    child->DeleteProperty(wxT("fudge"));
                    delete child->GetChildren();
                    child->SetChildren(NULL);

                    wxXmlNode* lastchild = NULL;
                    for(std::vector<VERTEX>::iterator it = sh2.vertices.begin(); it != sh2.vertices.end(); ++it) {
                        wxXmlNode* vert = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, RAWXML_SHS_MESH_VERTEX);
                        vert->AddProperty(wxT("u"), wxString::Format(wxT("%8f"), it->tu));
                        vert->AddProperty(wxT("v"), wxString::Format(wxT("%8f"), it->tv));
                        vert->AddProperty(wxT("colour"), wxString::Format(wxT("%08lX"), it->color));
                        wxXmlNode* pos = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, RAWXML_SHS_MESH_VERTEX_P);
                        vert->SetChildren(pos);
                        pos->AddProperty(wxT("x"), wxString::Format(wxT("%f"), it->position.x));
                        pos->AddProperty(wxT("y"), wxString::Format(wxT("%f"), it->position.y));
                        pos->AddProperty(wxT("z"), wxString::Format(wxT("%f"), it->position.z));
                        wxXmlNode* normal = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, RAWXML_SHS_MESH_VERTEX_N);
                        pos->SetNext(normal);
                        normal->AddProperty(wxT("x"), wxString::Format(wxT("%8f"), it->normal.x));
                        normal->AddProperty(wxT("y"), wxString::Format(wxT("%8f"), it->normal.y));
                        normal->AddProperty(wxT("z"), wxString::Format(wxT("%8f"), it->normal.z));
                        if (lastchild) {
                            lastchild->SetNext(vert);
                        } else {
                            child->SetChildren(vert);
                        }
                        lastchild = vert;
                    }
                    for(int l = 0; l < sh2.indices.size(); l+=3) {
                        wxXmlNode* tri = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, RAWXML_SHS_MESH_TRIANGLE);
                        tri->AddProperty(wxT("a"), wxString::Format(wxT("%lu"), sh2.indices[l]));
                        tri->AddProperty(wxT("b"), wxString::Format(wxT("%lu"), sh2.indices[l+1]));
                        tri->AddProperty(wxT("c"), wxString::Format(wxT("%lu"), sh2.indices[l+2]));
                        lastchild->SetNext(tri);
                        lastchild = tri;
                    }
                }
            } else {
                BAKE_SKIP(child);
                wxXmlNode *subchild = child->GetChildren();
                while (subchild) {
                    DO_CONDITION_COMMENT(subchild);
                    if (subchild->GetName() == RAWXML_SHS_MESH_VERTEX) {
                        VERTEX v;
                        v.tu = ParseFloat(subchild, RAWXML_SHS_MESH_VERTEX, wxT("u"));
                        v.tv = ParseFloat(subchild, RAWXML_SHS_MESH_VERTEX, wxT("v"));
                        if (subchild->HasProp(wxT("colour"))) {
                            wxString scol = subchild->GetPropVal(wxT("colour"), wxT(""));
                            unsigned long lcol = 0;
                            if (sscanf(scol.ToAscii(), "%lx", &lcol))
                                v.color = lcol;
                        }

                        bool nopos = true;
                        bool nonorm = true;

                        wxXmlNode *thirdchild = subchild->GetChildren();
                        while (thirdchild) {
                            if (thirdchild->GetName() == RAWXML_SHS_MESH_VERTEX_P) {
                                nopos = false;
                                ParseVector(thirdchild, v.position, RAWXML_SHS_MESH_VERTEX_P);
                            } else if (thirdchild->GetName() == RAWXML_SHS_MESH_VERTEX_N) {
                                nonorm = false;
                                ParseVector(thirdchild, v.normal, RAWXML_SHS_MESH_VERTEX_N);
                            } else if COMPILER_WRONGTAG(thirdchild) {
                                throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in shs(%s)/shsmesh/vertex."), thirdchild->GetName().c_str(), name.c_str()));
                            }
                            thirdchild = thirdchild->GetNext();
                        }

                        if (nopos || nonorm)
                            throw RCT3Exception(wxString::Format(_("Position or normal missing in shs(%s)/shsmesh/vertex."), name.c_str()));

                        if (!(bb1 || bb2))
                            boundsContain(&v.position, &sh1.bbox1, &sh1.bbox2);

                        sh2.vertices.push_back(v);
                    } else if (subchild->GetName() == RAWXML_SHS_MESH_TRIANGLE) {
                        unsigned short index = ParseUnsigned(subchild, RAWXML_SHS_MESH_TRIANGLE, wxT("a"));
                        sh2.indices.push_back(index);
                        index = ParseUnsigned(subchild, RAWXML_SHS_MESH_TRIANGLE, wxT("b"));
                        sh2.indices.push_back(index);
                        index = ParseUnsigned(subchild, RAWXML_SHS_MESH_TRIANGLE, wxT("c"));
                        sh2.indices.push_back(index);
                    } else if COMPILER_WRONGTAG(subchild) {
                        throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in shs(%s)/shsmesh."), subchild->GetName().c_str(), name.c_str()));
                    }
                    subchild = subchild->GetNext();
                }
            }
            sh1.meshes.push_back(sh2);
        } else if (child->GetName() == RAWXML_SHS_EFFECT) {
            BAKE_SKIP(child);
            cEffectStruct effect;
            wxString effectname = ParseString(child, RAWXML_SHS_EFFECT, wxT("name"), NULL);

            effect.name = effectname.ToAscii();
            bool missp = true;
            wxXmlNode *subchild = child->GetChildren();
            while (subchild) {
                DO_CONDITION_COMMENT(subchild);
                if (subchild->GetName() == RAWXML_SHS_EFFECT_POS) {
                    missp = false;
                    ParseMatrix(subchild, effect.pos, RAWXML_SHS_EFFECT_POS);
                } else if COMPILER_WRONGTAG(subchild) {
                    throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in shs(%s)/shseffect(%s)."), subchild->GetName().c_str(), name.c_str(), effectname.c_str()));
                }
                subchild = subchild->GetNext();
            }
            if (missp)
                throw RCT3Exception(wxString::Format(_("Missing position in shs(%s)/shseffect(%s)."), name.c_str(), effectname.c_str()));

            sh1.effects.push_back(effect);
        } else if COMPILER_WRONGTAG(child) {
            throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in shs tag '%s'."), child->GetName().c_str(), name.c_str()));
        }
        child = child->GetNext();
    }

    if (m_mode != MODE_BAKE) {
        ovlSHSManager* c_shs = m_ovl.GetManager<ovlSHSManager>();
        c_shs->AddModel(sh1);
    } else if ((!bb1) && (!bb2)) {
        if (m_bake.Index(RAWXML_SHS) != wxNOT_FOUND) {
            wxXmlNode* bb = makeElementNode(RAWXML_SHS_BBOX2);
            bb->AddProperty(wxT("x"), wxString::Format(wxT("%f"), sh1.bbox2.x));
            bb->AddProperty(wxT("y"), wxString::Format(wxT("%f"), sh1.bbox2.y));
            bb->AddProperty(wxT("z"), wxString::Format(wxT("%f"), sh1.bbox2.z));
            node->InsertChild(bb, NULL);

            bb = makeElementNode(RAWXML_SHS_BBOX1);
            bb->AddProperty(wxT("x"), wxString::Format(wxT("%f"), sh1.bbox1.x));
            bb->AddProperty(wxT("y"), wxString::Format(wxT("%f"), sh1.bbox1.y));
            bb->AddProperty(wxT("z"), wxString::Format(wxT("%f"), sh1.bbox1.z));
            node->InsertChild(bb, NULL);
        }
    }
}

void cRawParser::ParseSID(wxXmlNode* node) {
    USE_PREFIX(node);
    cSid sid;
    sid.name = ParseString(node, RAWXML_SID, wxT("name"), NULL, useprefix).ToAscii();
    sid.ovlpath = ParseString(node, RAWXML_SID, wxT("ovlpath"), NULL).ToAscii();
    sid.ui.name = ParseString(node, RAWXML_SID, wxT("nametxt"), NULL, useprefix).ToAscii();
    sid.ui.icon = ParseString(node, RAWXML_SID, wxT("icon"), NULL, useprefix).ToAscii();
    wxString svd = ParseString(node, RAWXML_SID, wxT("svd"), NULL, useprefix);
    sid.svds.push_back(std::string(svd.ToAscii()));
    wxLogVerbose(wxString::Format(_("Adding sid %s to %s."), wxString(sid.name.c_str(), wxConvLocal).c_str(), m_output.GetFullPath().c_str()));

    wxXmlNode *child = node->GetChildren();
    while (child) {
        DO_CONDITION_COMMENT(child);

        if (child->GetName() == RAWXML_SID_GROUP) {
            USE_PREFIX(child);
            wxString name = ParseString(child, RAWXML_SID_GROUP, wxT("name"), NULL, useprefix);
            wxString icon = ParseString(child, RAWXML_SID_GROUP, wxT("icon"), NULL, useprefix);
            sid.ui.group = name.ToAscii();
            sid.ui.groupicon = icon.ToAscii();
        } else if (child->GetName() == RAWXML_SID_TYPE) {
            OPTION_PARSE(unsigned long, sid.ui.type, ParseUnsigned(child, RAWXML_SID_TYPE, wxT("scenerytype")));
            OPTION_PARSE(long, sid.ui.cost, ParseSigned(child, RAWXML_SID_TYPE, wxT("cost")));
            OPTION_PARSE(long, sid.ui.removal_cost, ParseSigned(child, RAWXML_SID_TYPE, wxT("removalcost")));
        } else if (child->GetName() == RAWXML_SID_POSITION) {
            OPTION_PARSE(unsigned long, sid.position.positioningtype, ParseUnsigned(child, RAWXML_SID_POSITION, wxT("type")));
            OPTION_PARSE(unsigned long, sid.position.xsquares, ParseUnsigned(child, RAWXML_SID_POSITION, wxT("xsquares")));
            OPTION_PARSE(unsigned long, sid.position.ysquares, ParseUnsigned(child, RAWXML_SID_POSITION, wxT("ysquares")));
            OPTION_PARSE(float, sid.position.xpos, ParseFloat(child, RAWXML_SID_POSITION, wxT("xpos")));
            OPTION_PARSE(float, sid.position.ypos, ParseFloat(child, RAWXML_SID_POSITION, wxT("ypos")));
            OPTION_PARSE(float, sid.position.zpos, ParseFloat(child, RAWXML_SID_POSITION, wxT("zpos")));
            OPTION_PARSE(float, sid.position.xsize, ParseFloat(child, RAWXML_SID_POSITION, wxT("xsize")));
            OPTION_PARSE(float, sid.position.ysize, ParseFloat(child, RAWXML_SID_POSITION, wxT("ysize")));
            OPTION_PARSE(float, sid.position.zsize, ParseFloat(child, RAWXML_SID_POSITION, wxT("zsize")));
            ParseStringOption(sid.position.supports, child, wxT("supports"), NULL, false);
        } else if (child->GetName() == RAWXML_SID_COLOURS) {
            OPTION_PARSE(unsigned long, sid.colours.defaultcol[0], ParseUnsigned(child, RAWXML_SID_COLOURS, wxT("choice1")));
            OPTION_PARSE(unsigned long, sid.colours.defaultcol[1], ParseUnsigned(child, RAWXML_SID_COLOURS, wxT("choice2")));
            OPTION_PARSE(unsigned long, sid.colours.defaultcol[2], ParseUnsigned(child, RAWXML_SID_COLOURS, wxT("choice3")));
        } else if (child->GetName() == RAWXML_SID_SQUAREUNKNOWNS) {
            cSidSquareUnknowns squ;
            OPTION_PARSE(unsigned long, squ.unk6, ParseUnsigned(child, RAWXML_SID_SQUAREUNKNOWNS, wxT("u6")));
            OPTION_PARSE(unsigned long, squ.unk7, ParseUnsigned(child, RAWXML_SID_SQUAREUNKNOWNS, wxT("u7")));
            if (child->HasProp(wxT("u8"))) {
                OPTION_PARSE(unsigned long, squ.unk8, ParseUnsigned(child, RAWXML_SID_SQUAREUNKNOWNS, wxT("u8")));
                squ.use_unk8 = true;
            }
            OPTION_PARSE(unsigned long, squ.unk9, ParseUnsigned(child, RAWXML_SID_SQUAREUNKNOWNS, wxT("u9")));

            if (child->HasProp(wxT("flags"))) {
                wxString flags = ParseString(child, RAWXML_SID_SQUAREUNKNOWNS, wxT("flags"), NULL);
                if (flags.Length() != 32)
                    throw RCT3InvalidValueException(_("The flags attribute of a sidsquareunknowns tag is of invalid length"));
                const char* fl = flags.ToAscii();
                for (int i = 0; i < 32; ++i) {
                    if (fl[i] == '0') {
                        squ.flag[i] = false;
                    } else if (fl[i] == '1') {
                        squ.flag[i] = true;
                    } else {
                        throw RCT3InvalidValueException(_("The flags attribute of a sidsquareunknowns tag has an invalid character"));
                    }
                }
            }

            sid.squareunknowns.push_back(squ);
        } else if (child->GetName() == RAWXML_SID_IMPORTERUNKNOWNS) {
            OPTION_PARSE(unsigned long, sid.importerunknowns.unk1, ParseUnsigned(child, RAWXML_SID_IMPORTERUNKNOWNS, wxT("u1")));
            OPTION_PARSE(unsigned long, sid.importerunknowns.unk2, ParseUnsigned(child, RAWXML_SID_IMPORTERUNKNOWNS, wxT("u2")));
            if (child->HasProp(wxT("flags"))) {
                wxString flags = ParseString(child, RAWXML_SID_IMPORTERUNKNOWNS, wxT("flags"), NULL);
                if (flags.Length() != 32)
                    throw RCT3InvalidValueException(_("The flags attribute of a sidimporterunknowns tag is of invalid length"));
                const char* fl = flags.ToAscii();
                for (int i = 0; i < 32; ++i) {
                    if (fl[i] == '0') {
                        sid.importerunknowns.flag[i] = false;
                    } else if (fl[i] == '1') {
                        sid.importerunknowns.flag[i] = true;
                    } else {
                        throw RCT3InvalidValueException(_("The flags attribute of a sidimporterunknowns tag has an invalid character"));
                    }
                }
            }

        } else if (child->GetName() == RAWXML_SID_STALLUNKNOWNS) {
            sid.stallunknowns.MakeStall();
            OPTION_PARSE(unsigned long, sid.stallunknowns.unk1, ParseUnsigned(child, RAWXML_SID_STALLUNKNOWNS, wxT("u1")));
            OPTION_PARSE(unsigned long, sid.stallunknowns.unk2, ParseUnsigned(child, RAWXML_SID_STALLUNKNOWNS, wxT("u2")));
        } else if (child->GetName() == RAWXML_SID_SVD) {
            USE_PREFIX(child);
            svd = ParseString(node, RAWXML_SVD, wxT("name"), NULL, useprefix);
            sid.svds.push_back(std::string(svd.ToAscii()));
        } else if (child->GetName() == RAWXML_SID_PARAMETER) {
            cSidParam param;
            param.name = ParseString(child, RAWXML_SID_GROUP, wxT("name"), NULL).ToAscii();
            ParseStringOption(param.param, child, wxT("param"), NULL, false);
            if (param.param != "") {
                if (param.param[0] != ' ')
                    param.param = " " + param.param;
            }
            sid.parameters.push_back(param);
        } else if COMPILER_WRONGTAG(child) {
            throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in sid tag."), child->GetName().c_str()));
        }

        child = child->GetNext();
    }

    ovlSIDManager* c_sid = m_ovl.GetManager<ovlSIDManager>();
    c_sid->AddSID(sid);
}

void cRawParser::ParseSPL(wxXmlNode* node) {
    USE_PREFIX(node);
    cSpline spl;
    spl.name = ParseString(node, RAWXML_SPL, wxT("name"), NULL, useprefix).ToAscii();
    spl.unk3 = ParseFloat(node, RAWXML_SPL, wxT("u3"));
    OPTION_PARSE(float, spl.unk6, ParseFloat(node, RAWXML_SPL, wxT("u6")));
    OPTION_PARSE(unsigned long, spl.cyclic, ParseUnsigned(node, RAWXML_SPL, wxT("cyclic")));
    OPTION_PARSE(unsigned long, spl.lengthcalcres, ParseUnsigned(node, RAWXML_SPL, wxT("autolengthres")));
    wxLogVerbose(wxString::Format(_("Adding spl %s to %s."), wxString(spl.name.c_str(), wxConvLocal).c_str(), m_output.GetFullPath().c_str()));

    wxXmlNode *child = node->GetChildren();
    while (child) {
        DO_CONDITION_COMMENT(child);

        if (child->GetName() == RAWXML_SPL_NODE) {
            SplineNode sp;
            sp.pos.x = ParseFloat(child, RAWXML_SPL_NODE, wxT("x"));
            sp.pos.y = ParseFloat(child, RAWXML_SPL_NODE, wxT("y"));
            sp.pos.z = ParseFloat(child, RAWXML_SPL_NODE, wxT("z"));
            sp.cp1.x = ParseFloat(child, RAWXML_SPL_NODE, wxT("cp1x"));
            sp.cp1.y = ParseFloat(child, RAWXML_SPL_NODE, wxT("cp1y"));
            sp.cp1.z = ParseFloat(child, RAWXML_SPL_NODE, wxT("cp1z"));
            sp.cp2.x = ParseFloat(child, RAWXML_SPL_NODE, wxT("cp2x"));
            sp.cp2.y = ParseFloat(child, RAWXML_SPL_NODE, wxT("cp2y"));
            sp.cp2.z = ParseFloat(child, RAWXML_SPL_NODE, wxT("cp2z"));
            spl.nodes.push_back(sp);
        } else if (child->GetName() == RAWXML_SPL_LENGTH) {
            float l = ParseFloat(child, RAWXML_SPL_LENGTH, wxT("length"));
            spl.lengths.push_back(l);
        } else if (child->GetName() == RAWXML_SPL_DATA) {
            cSplineData dt;
            wxString datastr = ParseString(child, RAWXML_SPL_DATA, wxT("data"), NULL);
            if (datastr.Length() != 28)
                throw RCT3InvalidValueException(_("The data attribute of a spldata tag is of invalid length"));
            const char* d = datastr.ToAscii();
            for (int i = 0; i < 28; i+=2) {
                unsigned char dta = 0;
                unsigned char t = ParseDigit(d[i]);
                if (t >= 16)
                    throw RCT3InvalidValueException(_("The data attribute of a spldata tag has an invalid character"));
                dta += t * 16;
                t = ParseDigit(d[i+1]);
                if (t >= 16)
                    throw RCT3InvalidValueException(_("The data attribute of a spldata tag has an invalid character"));
                dta += t;
                dt.data[i/2] = dta;
            }
            spl.unknowndata.push_back(dt);
        } else if COMPILER_WRONGTAG(child) {
            throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in ced tag."), child->GetName().c_str()));
        }
        child = child->GetNext();
    }

    ovlSPLManager* c_spl = m_ovl.GetManager<ovlSPLManager>();
    c_spl->AddSpline(spl);
}

void cRawParser::ParseSTA(wxXmlNode* node) {
    USE_PREFIX(node);
    cStall stall;
    stall.name = ParseString(node, RAWXML_STA, wxT("name"), NULL, useprefix).ToAscii();
    stall.attraction.name = ParseString(node, RAWXML_STA, wxT("nametxt"), NULL, useprefix).ToAscii();
    stall.attraction.description = ParseString(node, RAWXML_STA, wxT("description"), NULL, useprefix).ToAscii();
    stall.sid = ParseString(node, RAWXML_STA, wxT("sid"), NULL, useprefix).ToAscii();
    wxLogVerbose(wxString::Format(_("Adding sta %s to %s."), wxString(stall.name.c_str(), wxConvLocal).c_str(), m_output.GetFullPath().c_str()));

    wxXmlNode *child = node->GetChildren();
    while (child) {
        DO_CONDITION_COMMENT(child);

        if (child->GetName() == RAWXML_ATTRACTION_BASE) {
            USE_PREFIX(child);
            OPTION_PARSE(unsigned long, stall.attraction.type, ParseUnsigned(child, RAWXML_ATTRACTION_BASE, wxT("type")));
            ParseStringOption(stall.attraction.icon, child, wxT("icon"), NULL, useprefix);
            ParseStringOption(stall.attraction.spline, child, wxT("spline"), NULL, useprefix);
        } else if (child->GetName() == RAWXML_STA_ITEM) {
            USE_PREFIX(child);
            cStallItem item;
            item.item = ParseString(child, RAWXML_STA_ITEM, wxT("cid"), NULL, useprefix).ToAscii();
            item.cost = ParseUnsigned(child, RAWXML_STA_ITEM, wxT("cost"));
            stall.items.push_back(item);
        } else if (child->GetName() == RAWXML_STA_STALLUNKNOWNS) {
            OPTION_PARSE(unsigned long, stall.unknowns.unk1, ParseUnsigned(child, RAWXML_STA_STALLUNKNOWNS, wxT("u1")));
            OPTION_PARSE(unsigned long, stall.unknowns.unk2, ParseUnsigned(child, RAWXML_STA_STALLUNKNOWNS, wxT("u2")));
            OPTION_PARSE(unsigned long, stall.unknowns.unk3, ParseUnsigned(child, RAWXML_STA_STALLUNKNOWNS, wxT("u3")));
            OPTION_PARSE(unsigned long, stall.unknowns.unk4, ParseUnsigned(child, RAWXML_STA_STALLUNKNOWNS, wxT("u4")));
            OPTION_PARSE(unsigned long, stall.unknowns.unk5, ParseUnsigned(child, RAWXML_STA_STALLUNKNOWNS, wxT("u5")));
            OPTION_PARSE(unsigned long, stall.unknowns.unk6, ParseUnsigned(child, RAWXML_STA_STALLUNKNOWNS, wxT("u6")));
        } else if (child->GetName() == RAWXML_ATTRACTION_UNKNOWNS) {
            OPTION_PARSE(unsigned long, stall.attraction.unk2, ParseUnsigned(child, RAWXML_ATTRACTION_UNKNOWNS, wxT("u2")));
            OPTION_PARSE(long, stall.attraction.unk3, ParseSigned(child, RAWXML_ATTRACTION_UNKNOWNS, wxT("u3")));
            OPTION_PARSE(unsigned long, stall.attraction.unk4, ParseUnsigned(child, RAWXML_ATTRACTION_UNKNOWNS, wxT("u4")));
            OPTION_PARSE(unsigned long, stall.attraction.unk5, ParseUnsigned(child, RAWXML_ATTRACTION_UNKNOWNS, wxT("u5")));
            OPTION_PARSE(unsigned long, stall.attraction.unk6, ParseUnsigned(child, RAWXML_ATTRACTION_UNKNOWNS, wxT("u6")));
            OPTION_PARSE(unsigned long, stall.attraction.unk9, ParseUnsigned(child, RAWXML_ATTRACTION_UNKNOWNS, wxT("u9")));
            OPTION_PARSE(long, stall.attraction.unk10, ParseSigned(child, RAWXML_ATTRACTION_UNKNOWNS, wxT("u10")));
            OPTION_PARSE(unsigned long, stall.attraction.unk11, ParseUnsigned(child, RAWXML_ATTRACTION_UNKNOWNS, wxT("u11")));
            OPTION_PARSE(unsigned long, stall.attraction.unk12, ParseUnsigned(child, RAWXML_ATTRACTION_UNKNOWNS, wxT("u12")));
            OPTION_PARSE(unsigned long, stall.attraction.unk13, ParseUnsigned(child, RAWXML_ATTRACTION_UNKNOWNS, wxT("u13")));
        } else if COMPILER_WRONGTAG(child) {
            throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in sta tag."), child->GetName().c_str()));
        }

        child = child->GetNext();
    }

    ovlSTAManager* c_sta = m_ovl.GetManager<ovlSTAManager>();
    c_sta->AddStall(stall);
}

void cRawParser::ParseSVD(wxXmlNode* node) {
    USE_PREFIX(node);
    cSceneryItemVisual svd;
    wxString name = ParseString(node, RAWXML_SVD, wxT("name"), NULL, useprefix);
    svd.name = name.ToAscii();
    OPTION_PARSE(unsigned long, svd.sivflags, ParseFloat(node, RAWXML_SVD, wxT("flags")));
    OPTION_PARSE(float, svd.sway, ParseFloat(node, RAWXML_SVD, wxT("sway")));
    OPTION_PARSE(float, svd.brightness, ParseFloat(node, RAWXML_SVD, wxT("brightness")));
    OPTION_PARSE(float, svd.scale, ParseFloat(node, RAWXML_SVD, wxT("scale")));
    wxLogVerbose(wxString::Format(_("Adding svd %s to %s."), name.c_str(), m_output.GetFullPath().c_str()));

    wxXmlNode *child = node->GetChildren();
    while (child) {
        DO_CONDITION_COMMENT(child);

        if (child->GetName() == RAWXML_SVD_UNK) {
            OPTION_PARSE(float, svd.unk4, ParseFloat(child, RAWXML_SVD_UNK, wxT("u4")));
            OPTION_PARSE(unsigned long, svd.unk6, ParseUnsigned(child, RAWXML_SVD_UNK, wxT("u6")));
            OPTION_PARSE(unsigned long, svd.unk7, ParseUnsigned(child, RAWXML_SVD_UNK, wxT("u7")));
            OPTION_PARSE(unsigned long, svd.unk8, ParseUnsigned(child, RAWXML_SVD_UNK, wxT("u8")));
            OPTION_PARSE(unsigned long, svd.unk9, ParseUnsigned(child, RAWXML_SVD_UNK, wxT("u9")));
            OPTION_PARSE(unsigned long, svd.unk10, ParseUnsigned(child, RAWXML_SVD_UNK, wxT("u10")));
            OPTION_PARSE(unsigned long, svd.unk11, ParseUnsigned(child, RAWXML_SVD_UNK, wxT("u11")));
        } else if (child->GetName() == RAWXML_SVD_LOD) {
            USE_PREFIX(child);
            cSceneryItemVisualLOD lod;
            wxString lodname = ParseString(child, RAWXML_SVD_LOD, wxT("name"), NULL);
            lod.name = lodname.ToAscii();
            lod.meshtype = ParseUnsigned(child, RAWXML_SVD_LOD, wxT("type"));
            lod.distance = ParseFloat(child, RAWXML_SVD_LOD, wxT("distance"));
            ParseStringOption(lod.staticshape, child, wxT("staticshape"), NULL, useprefix);
            ParseStringOption(lod.boneshape, child, wxT("boneshape"), NULL, useprefix);
            ParseStringOption(lod.fts, child, wxT("ftx"), NULL, useprefix);
            ParseStringOption(lod.txs, child, wxT("txs"), NULL);

            wxXmlNode *subchild = child->GetChildren();
            while (subchild) {
                DO_CONDITION_COMMENT(subchild);

                if (subchild->GetName() == RAWXML_SVD_LOD_UNK) {
                    OPTION_PARSE(unsigned long, lod.unk2, ParseUnsigned(child, RAWXML_SVD_LOD_UNK, wxT("u2")));
                    OPTION_PARSE(unsigned long, lod.unk4, ParseUnsigned(child, RAWXML_SVD_LOD_UNK, wxT("u4")));
                    OPTION_PARSE(float, lod.unk7, ParseFloat(child, RAWXML_SVD_LOD_UNK, wxT("u7")));
                    OPTION_PARSE(float, lod.unk8, ParseFloat(child, RAWXML_SVD_LOD_UNK, wxT("u8")));
                    OPTION_PARSE(float, lod.unk9, ParseFloat(child, RAWXML_SVD_LOD_UNK, wxT("u9")));
                    OPTION_PARSE(float, lod.unk10, ParseFloat(child, RAWXML_SVD_LOD_UNK, wxT("u10")));
                    OPTION_PARSE(float, lod.unk11, ParseFloat(child, RAWXML_SVD_LOD_UNK, wxT("u11")));
                    OPTION_PARSE(float, lod.unk12, ParseFloat(child, RAWXML_SVD_LOD_UNK, wxT("u12")));
                    OPTION_PARSE(unsigned long, lod.unk14, ParseUnsigned(child, RAWXML_SVD_LOD_UNK, wxT("u14")));
                } else if (subchild->GetName() == RAWXML_SVD_LOD_ANIMATION) {
                    USE_PREFIX(subchild);
                    wxString anim = subchild->GetNodeContent();
                    MakeVariable(anim);
                    if (useprefix)
                        lod.animations.push_back(m_prefix + std::string(anim.ToAscii()));
                    else
                        lod.animations.push_back(std::string(anim.ToAscii()));
                } else if COMPILER_WRONGTAG(subchild) {
                    throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in svd(%s)/svdlod(%s)."), subchild->GetName().c_str(), name.c_str(), lodname.c_str()));
                }

                subchild = subchild->GetNext();
            }

            svd.lods.push_back(lod);
        } else if COMPILER_WRONGTAG(child) {
            throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in svd tag '%s'."), child->GetName().c_str(), name.c_str()));
        }

        child = child->GetNext();
    }

    ovlSVDManager* c_svd = m_ovl.GetManager<ovlSVDManager>();
    c_svd->AddSVD(svd);
}

void cRawParser::ParseTEX(wxXmlNode* node) {
    USE_PREFIX(node);
    wxString name = ParseString(node, RAWXML_TEX, wxT("name"), NULL, useprefix);
//    wxFileName texture = ParseString(node, RAWXML_TEX, wxT("texture"));
//    if (!texture.IsAbsolute())
//        texture.MakeAbsolute(m_input.GetPathWithSep());
    wxLogVerbose(wxString::Format(_("Adding tex %s to %s."), name.c_str(), m_output.GetFullPath().c_str()));

    cTextureStruct texture;
    texture.texture.name = name.ToAscii();
    OPTION_PARSE(unsigned long, texture.texture.format, ParseUnsigned(node, RAWXML_TEX, wxT("format")));
    wxGXImage mainimage;
    unsigned long mips = 0;
    OPTION_PARSE(unsigned long, mips, ParseUnsigned(node, RAWXML_TEX, wxT("mips")));
    ParseStringOption(texture.texturestyle, node, wxT("txs"), NULL);

    wxXmlNode* child = node->GetChildren();
    while (child) {
        DO_CONDITION_COMMENT(child);

        if (child->GetName() == RAWXML_TEX_TEXTURE) {
            cTextureMIP mip;
            OPTION_PARSE(unsigned long, mip.dimension, ParseUnsigned(child, RAWXML_TEX, wxT("dimension")));

            wxXmlNode* datanode = child->GetChildren();
            while (datanode && (!datanode->GetName().IsSameAs(RAWXML_DATA)))
                datanode = datanode->GetNext();

            if (!datanode)
                throw RCT3Exception(wxString::Format(_("Tag tex(%s)/texture misses data."), name.c_str()));

            cRawDatablock data = GetDataBlock(wxString::Format(_("tag tex(%s)/texture"), name.c_str()), datanode);
            if (data.datatype().IsEmpty()) {
                throw RCT3Exception(wxString::Format(_("Could not determine data type in tag tex(%s)/texture."), name.c_str()));
            } else if (data.datatype().IsSameAs(wxT("processed"))) {
                unsigned long datadim = cTexture::GetDimension(texture.texture.format, data.datasize());
                if (!mip.dimension)
                    mip.dimension = datadim;
                if (mip.dimension != datadim)
                    throw RCT3Exception(wxString::Format(_("Dimension mismatch in tag tex(%s)/texture."), name.c_str()));
                mip.data = data.data();
            } else {
                wxGXImage img;
                img.read(data.data().get(), data.datasize(), std::string(data.datatype().mb_str(wxConvLocal)));
                if (!img.Ok())
                    throw RCT3Exception(wxString::Format(_("Error decoding image in tag tex(%s)/texture."), name.c_str()));
                img.flip();
                if (!mainimage.Ok())
                    mainimage = img;
                if (mip.dimension) {
                    img.Rescale(mip.dimension, mip.dimension);
                } else {
                    mip.dimension = img.GetWidth();
                }
                mip.data = boost::shared_array<unsigned char>(new unsigned char[mip.CalcSize(cTexture::GetBlockSize(texture.texture.format))]);
                switch (texture.texture.format) {
                    case cTexture::FORMAT_A8R8G8B8:
                        img.GetData("RGBA", mip.data.get(), true);
                        break;
                    case cTexture::FORMAT_DXT1:
                        img.DxtCompress(mip.data.get(), wxDXT1);
                        break;
                    case cTexture::FORMAT_DXT3:
                        img.DxtCompress(mip.data.get(), wxDXT3);
                        break;
                    case cTexture::FORMAT_DXT5:
                        img.DxtCompress(mip.data.get(), wxDXT5);
                        break;
                    default:
                        throw RCT3Exception(wxString::Format(_("Unknown format 0x%02lx in tag tex(%s)/texture."), texture.texture.format, name.c_str()));
                }
            }

            texture.texture.mips.insert(mip);
        } else if COMPILER_WRONGTAG(child) {
            throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in tex(%s) tag."), child->GetName().c_str(), name.c_str()));
        }
        child = child->GetNext();
    }

    if (mips) {
        if ((texture.texture.mips.size() > 1) && (mips != texture.texture.mips.size()))
            throw RCT3Exception(wxString::Format(_("Mip count mismatch in tag tex(%s)."), name.c_str()));
        if ((texture.texture.mips.size() == 1) && (mips > 1)) {
            if (!mainimage.Ok())
                throw RCT3Exception(wxString::Format(_("Cannot auto-generate mips in tag tex(%s)."), name.c_str()));
            for (unsigned long i = 1; i < mips; ++i) {
                mainimage.Rescale(mainimage.GetWidth() / 2, mainimage.GetHeight() / 2);
                if (mainimage.GetWidth() < 4) {
                    wxLogWarning(wxString::Format(_("Too many mips requested in tag tex(%s)."), name.c_str()));
                    break;
                }
                cTextureMIP mip;
                mip.dimension = mainimage.GetWidth();
                mip.data = boost::shared_array<unsigned char>(new unsigned char[mip.CalcSize(cTexture::GetBlockSize(texture.texture.format))]);
                switch (texture.texture.format) {
                    case cTexture::FORMAT_A8R8G8B8:
                        mainimage.GetData("RGBA", mip.data.get(), true);
                        break;
                    case cTexture::FORMAT_DXT1:
                        mainimage.DxtCompress(mip.data.get(), wxDXT1);
                        break;
                    case cTexture::FORMAT_DXT3:
                        mainimage.DxtCompress(mip.data.get(), wxDXT3);
                        break;
                    case cTexture::FORMAT_DXT5:
                        mainimage.DxtCompress(mip.data.get(), wxDXT5);
                        break;
                    default:
                        throw RCT3Exception(wxString::Format(_("Unknown format 0x%02lx in tag tex(%s)."), texture.texture.format, name.c_str()));
                }
                texture.texture.mips.insert(mip);
            }
        }
    }

/*
    counted_array_ptr<unsigned char> data;
    unsigned long dimension = 0;
    unsigned long datasize = 0;

    wxXmlNode* child = node->GetChildren();
    while (child) {
        DO_CONDITION_COMMENT(child);

        if (child->GetName() == RAWXML_TEX_FILE) {
            if (data.get() && (m_mode != MODE_BAKE))
                throw RCT3Exception(wxString(wxT("tex tag '"))+name+wxT("': multiple datasources."));

            wxString t = child->GetNodeContent();
            bool filevar = MakeVariable(t);
            wxFSFileName texture = t;
            if (!texture.IsAbsolute())
                texture.MakeAbsolute(m_input.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME));

            if (m_mode == MODE_BAKE) {
                if (m_bake.Index(RAWXML_TEX) == wxNOT_FOUND) {
                    if (!filevar) {
                        if (m_bake.Index(RAWBAKE_ABSOLUTE) == wxNOT_FOUND) {
                            texture.MakeRelativeTo(m_bakeroot.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME));
                        }
                        delete child->GetChildren();
                        child->SetChildren(new wxXmlNode(NULL, wxXML_TEXT_NODE, wxT(""), texture.GetFullPath()));
                    }
                    child = child->GetNext();
                    continue;
                }
            }

//            if (!texture.IsFileReadable())
//                throw RCT3Exception(_("tex tag: File not readable: ") + texture.GetFullPath());

            try {
                wxGXImage img(texture.GetFullPath(), false);
                if (!img.Ok())
                    throw RCT3Exception(_("tex tag: Error reading image file: ") + texture.GetFullPath());

                int width = img.GetWidth();
                int height = img.GetHeight();
                if (width != height) {
                    throw Magick::Exception("Icon texture is not square");
                }
                if ((1 << local_log2(width)) != width) {
                    throw Magick::Exception("Icon texture's width/height is not a power of 2");
                }
                img.flip();
                img.dxtCompressionMethod(squish::kColourIterativeClusterFit | squish::kWeightColourByAlpha);
                datasize = img.GetDxtBufferSize(wxDXT3);
                dimension = width;
                data = counted_array_ptr<unsigned char>(new unsigned char[datasize]);
                img.DxtCompress(data.get(), wxDXT3);

                if (m_mode == MODE_BAKE) {
                    counted_array_ptr<char> buf(new char[datasize * 4]);
                    unsigned long outlen = datasize * 4;
                    int bret = base64_encode(data.get(), datasize, reinterpret_cast<unsigned char*>(buf.get()), &outlen);
                    switch (bret) {
                        case CRYPT_OK:
                            break;
                        case CRYPT_BUFFER_OVERFLOW:
                            throw RCT3Exception(wxString(_("Buffer overflow baking texture '")) + texture.GetFullPath() + _("' in tex tag ") + name);
                        default:
                            throw RCT3Exception(wxString(_("Unknown base64 error baking texture '")) + texture.GetFullPath() + _("' in tex tag ") + name);
                    }
                    child->SetName(RAWXML_TEX_DATA);
                    child->AddProperty(wxT("dimension"), wxString::Format(wxT("%u"), img.GetWidth()));
                    delete child->GetChildren();
                    child->SetChildren(new wxXmlNode(NULL, wxXML_TEXT_NODE, wxT(""), wxString(buf.get(), wxConvLocal)));
                }

            } catch (Magick::Exception& e) {
                throw RCT3Exception(wxString::Format(_("tex tag: GarphicsMagik exception: %s"), e.what()));
            }
        } else if (child->GetName() == RAWXML_TEX_DATA) {
            if (m_mode == MODE_BAKE) {
                child = child->GetNext();
                continue;
            }
            if (data.get())
                throw RCT3Exception(wxString(wxT("tex tag '"))+name+wxT("': multiple datasources."));
            dimension = ParseUnsigned(child, RAWXML_TEX_DATA, wxT("dimension"));
            datasize = squish::GetStorageRequirements(dimension, dimension, wxDXT3);
            unsigned long outlen = datasize;

            data = counted_array_ptr<unsigned char>(new unsigned char[datasize]);
            wxString tex = child->GetNodeContent();
            MakeVariable(tex);
            int bret = base64_decode(reinterpret_cast<const unsigned char*>(static_cast<const char*>(tex.ToAscii())), tex.Length(), data.get(), &outlen);
            switch (bret) {
                case CRYPT_OK:
                    break;
                case CRYPT_INVALID_PACKET:
                    throw RCT3Exception(wxString(_("Decoding error in tex tag ")) + name);
                case CRYPT_BUFFER_OVERFLOW:
                    throw RCT3Exception(wxString(_("Buffer overflow decoding tex tag ")) + name);
                default:
                    throw RCT3Exception(wxString(_("Unknown base64 error decoding tex tag ")) + name);
            }
            if (outlen != datasize)
                throw RCT3Exception(wxString(_("Datasize mismatch error in tex tag ")) + name);

        } else if COMPILER_WRONGTAG(child) {
            throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in tex tag."), child->GetName().c_str()));
        }
        child = child->GetNext();
    }
*/

    if (m_mode != MODE_BAKE) {
        if (!texture.texture.mips.size())
            throw RCT3Exception(wxString(wxT("tex tag '"))+name+wxT("': no data."));
        ovlTEXManager* c_tex = m_ovl.GetManager<ovlTEXManager>();
        c_tex->AddTexture(texture);
        //c_tex->AddTexture(std::string(name.ToAscii()), dimension, datasize, reinterpret_cast<unsigned long*>(data.get()));
    }
}

void cRawParser::ParseWAI(wxXmlNode* node) {
    USE_PREFIX(node);
    cWildAnimalItem wai;
    wxString name = ParseString(node, RAWXML_WAI, wxT("name"), NULL, useprefix);
    wai.name = name.ToAscii();
    wai.shortname = ParseString(node, RAWXML_WAI, wxT("shortname"), NULL).ToAscii();
    wai.nametxt = ParseString(node, RAWXML_WAI, wxT("nametxt"), NULL, useprefix).ToAscii();
    wai.description = ParseString(node, RAWXML_WAI, wxT("description"), NULL, useprefix).ToAscii();
    wai.icon = ParseString(node, RAWXML_WAI, wxT("icon"), NULL, useprefix).ToAscii();
    wai.staticshape = ParseString(node, RAWXML_WAI, wxT("staticshape"), NULL, useprefix).ToAscii();
    wxLogVerbose(wxString::Format(_("Adding wai %s to %s."), name.c_str(), m_output.GetFullPath().c_str()));

    wxXmlNode *child = node->GetChildren();
    while (child) {
        DO_CONDITION_COMMENT(child);

        if (child->GetName() == RAWXML_WAI_PARAMETERS) {
            OPTION_PARSE(float, wai.distance, ParseFloat(child, RAWXML_WAI_PARAMETERS, wxT("distance")));
            OPTION_PARSE(long, wai.cost, ParseSigned(child, RAWXML_WAI_PARAMETERS, wxT("cost")));
            OPTION_PARSE(long, wai.refund, ParseSigned(child, RAWXML_WAI_PARAMETERS, wxT("refund")));
            OPTION_PARSE(float, wai.invdurability, ParseFloat(child, RAWXML_WAI_PARAMETERS, wxT("inversedurability")));
            OPTION_PARSE(float, wai.x_size, ParseFloat(child, RAWXML_WAI_PARAMETERS, wxT("xsize")));
            OPTION_PARSE(float, wai.y_size, ParseFloat(child, RAWXML_WAI_PARAMETERS, wxT("ysize")));
            OPTION_PARSE(float, wai.z_size, ParseFloat(child, RAWXML_WAI_PARAMETERS, wxT("zsize")));
            OPTION_PARSE(unsigned long, wai.flags, ParseUnsigned(child, RAWXML_WAI_PARAMETERS, wxT("flags")));
            OPTION_PARSE(float, wai.weight, ParseFloat(child, RAWXML_WAI_PARAMETERS, wxT("weight")));
        } else if (child->GetName() == RAWXML_WAI_UNKNOWNS) {
            OPTION_PARSE(unsigned long, wai.unk1, ParseUnsigned(child, RAWXML_WAI_UNKNOWNS, wxT("u1")));
            OPTION_PARSE(unsigned long, wai.unk2, ParseUnsigned(child, RAWXML_WAI_UNKNOWNS, wxT("u2")));
            OPTION_PARSE(unsigned long, wai.unk3, ParseUnsigned(child, RAWXML_WAI_UNKNOWNS, wxT("u3")));
            OPTION_PARSE(unsigned long, wai.unk4, ParseUnsigned(child, RAWXML_WAI_UNKNOWNS, wxT("u4")));
            OPTION_PARSE(unsigned long, wai.unk11, ParseUnsigned(child, RAWXML_WAI_UNKNOWNS, wxT("u11")));
            OPTION_PARSE(float, wai.unk17, ParseFloat(child, RAWXML_WAI_UNKNOWNS, wxT("u17")));
            OPTION_PARSE(float, wai.unk18, ParseFloat(child, RAWXML_WAI_UNKNOWNS, wxT("u18")));
            OPTION_PARSE(float, wai.unk19, ParseFloat(child, RAWXML_WAI_UNKNOWNS, wxT("u19")));
            OPTION_PARSE(float, wai.unk20, ParseFloat(child, RAWXML_WAI_UNKNOWNS, wxT("u20")));
        } else if COMPILER_WRONGTAG(child) {
            throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in wai tag '%s'."), child->GetName().c_str(), name.c_str()));
        }

        child = child->GetNext();
    }

    ovlWAIManager* c_wai = m_ovl.GetManager<ovlWAIManager>();
    c_wai->AddItem(wai);
}

