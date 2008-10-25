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

#include "ManagerSHS.h"

#include "3DLoader.h"
#include "matrix.h"
#include "RCT3Structs.h"


void cRawParser::ParseSHS(cXmlNode& node) {
    USE_PREFIX(node);
    cStaticShape1 sh1;
    wxString name = ParseString(node, wxT(RAWXML_SHS), wxT("name"), NULL, useprefix);
    sh1.name = name.ToAscii();
    boundsInit(&sh1.bbox1, &sh1.bbox2);
    cXmlNode bb1;
    cXmlNode bb2;
    wxLogVerbose(wxString::Format(_("Adding shs %s to %s."), name.c_str(), m_output.GetFullPath().c_str()));

    cXmlNode child(node.children());
    while (child) {
        DO_CONDITION_COMMENT(child);

        if (child(RAWXML_SHS_BBOX1)) {
            bb1 = child;
            ParseVector(child, sh1.bbox1, wxT(RAWXML_SHS_BBOX1));
        } else if (child(RAWXML_SHS_BBOX2)) {
            bb2 = child;
            ParseVector(child, sh1.bbox2, wxT(RAWXML_SHS_BBOX2));
        } else if (child(RAWXML_SHS_MESH)) {
            USE_PREFIX(child);
            cStaticShape2 sh2;
            sh2.fts = ParseString(child, wxT(RAWXML_SHS_MESH), wxT("ftx"), NULL, useprefix).ToAscii();
            sh2.texturestyle = ParseString(child, wxT(RAWXML_SHS_MESH), wxT("txs"), NULL).ToAscii();
            OPTION_PARSE(unsigned long, sh2.placetexturing, ParseUnsigned(child, wxT(RAWXML_SHS_MESH), wxT("transparency")));
            OPTION_PARSE(unsigned long, sh2.textureflags, ParseUnsigned(child, wxT(RAWXML_SHS_MESH), wxT("flags")));
            OPTION_PARSE(unsigned long, sh2.sides, ParseUnsigned(child, wxT(RAWXML_SHS_MESH), wxT("sides")));
            OPTION_PARSE(long, sh2.support, ParseSigned(child, wxT(RAWXML_SHS_MESH), wxT("support")));

            if (child.hasProp("meshname")) {
                wxString meshname = ParseString(child, wxT(RAWXML_SHS_MESH), wxT("meshname"), NULL);
                bool modelfilevar = false;
                wxFSFileName modelfile = ParseString(child, wxT(RAWXML_SHS_MESH), wxT("modelfile"), &modelfilevar);
                wxString hand = child.wxgetPropVal("handedness", "left");
                wxString up = child.wxgetPropVal("up", "y");
                c3DLoaderOrientation ori = ORIENTATION_LEFT_YUP;
                if (hand == wxT("left")) {
                    if (up == wxT("x")) {
                        ori = ORIENTATION_LEFT_XUP;
                    } else if (up == wxT("y")) {
                        ori = ORIENTATION_LEFT_YUP;
                    } else if (up == wxT("z")) {
                        ori = ORIENTATION_LEFT_ZUP;
                    } else {
                        throw MakeNodeException<RCT3Exception>(wxString::Format(_("Unknown value '%s' for up attribute in shs(%s)/shsmesh tag."), up.c_str(), name.c_str()), child);
                    }
                } else if (hand == wxT("right")) {
                    if (up == wxT("x")) {
                        ori = ORIENTATION_RIGHT_XUP;
                    } else if (up == wxT("y")) {
                        ori = ORIENTATION_RIGHT_YUP;
                    } else if (up == wxT("z")) {
                        ori = ORIENTATION_RIGHT_ZUP;
                    } else {
                        throw MakeNodeException<RCT3Exception>(wxString::Format(_("Unknown value '%s' for up attribute in shs(%s)/shsmesh tag."), up.c_str(), name.c_str()), child);
                    }
                } else {
                    throw MakeNodeException<RCT3Exception>(wxString::Format(_("Unknown value '%s' for handedness attribute in shs(%s)/shsmesh tag."), up.c_str(), name.c_str()), child);
                }

                if (!modelfile.IsAbsolute())
                    modelfile.MakeAbsolute(m_input.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME));

                if (m_mode == MODE_BAKE) {
                    if (m_bake.Index(wxT(RAWXML_SHS)) == wxNOT_FOUND) {
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

//                if (!modelfile.IsFileReadable())
//                    throw RCT3Exception(_("shs tag: Model file not readable: ") + modelfile.GetFullPath());

                boost::shared_ptr<c3DLoader> model(c3DLoader::LoadFile(modelfile.GetFullPath().c_str()));
                if (!model.get())
                    throw MakeNodeException<RCT3Exception>(_("shs tag: Model file not readable or has unknown format: ") + modelfile.GetFullPath(), child);

                int meshnr = -1;
                for (unsigned int i = 0; i < model->getObjectCount(); ++i) {
                    if (meshname == model->getObjectName(i)) {
                        meshnr = i;
                        break;
                    }
                }
                if (meshnr == -1)
                    throw MakeNodeException<RCT3Exception>(wxString::Format(_("shs tag: Model file '%s' does not contain a mesh called '%s'."), modelfile.GetFullPath().c_str(), meshname.c_str()), child);

                unsigned long fudgenormals = CMS_FUDGE_NONE;
                OPTION_PARSE(unsigned long, fudgenormals, ParseUnsigned(child, wxT(RAWXML_SHS_MESH), wxT("fudge")));

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
                    if (subchild(RAWXML_SHS_MESH_TRANSFORM)) {
                        if ((ori != ORIENTATION_LEFT_YUP) && (m_mode != MODE_INSTALL))
                            wxLogWarning(_("You've set handedness and up as well as giving a transformation matrix. The handedness/up is ignored for transforming the mesh."));
                        ParseMatrix(subchild, m, wxT(RAWXML_SHS_MESH_TRANSFORM));
                        do_matrix = true;
                    } else if (subchild.element()) {
                        throw MakeNodeException<RCT3Exception>(wxString::Format(_("Unknown tag '%s' in shs(%s)/shsmesh."), subchild.wxname().c_str(), name.c_str()), subchild);
                    }
                    subchild.go_next();
                }

                boundsInit(&temp_min, &temp_max);
                model->fetchObject(meshnr, &sh2, &temp_min, &temp_max,
                                          do_matrix?&m:NULL,
                                          c_pfudge_normal);
                if (!(bb1 || bb2))
                    boundsContain(&temp_min, &temp_max, &sh1.bbox1, &sh1.bbox2);
                if (fudgenormals == CMS_FUDGE_RIM) {
                    c3DLoader::FlattenNormals(&sh2, temp_min, temp_max);
                }
                if (m_mode == MODE_BAKE) {
                    child.delProp("meshname");
                    child.delProp("modelfile");
                    child.delProp("handedness");
                    child.delProp("up");
                    child.delProp("fudge");
                    if (child.children())
                        child.children().detach();

                    for(std::vector<VERTEX>::iterator it = sh2.vertices.begin(); it != sh2.vertices.end(); ++it) {
                        cXmlNode vert(child.newChild(RAWXML_SHS_MESH_VERTEX));
                        vert.addProp("u", wxString::Format(wxT("%8f"), it->tu).mb_str(wxConvUTF8));
                        vert.addProp("v", wxString::Format(wxT("%8f"), it->tv).mb_str(wxConvUTF8));
                        vert.addProp("colour", wxString::Format(wxT("%08lX"), it->color).mb_str(wxConvUTF8));
                        cXmlNode pos(vert.newChild(RAWXML_SHS_MESH_VERTEX_P));
                        pos.addProp("x", wxString::Format(wxT("%f"), it->position.x).mb_str(wxConvUTF8));
                        pos.addProp("y", wxString::Format(wxT("%f"), it->position.y).mb_str(wxConvUTF8));
                        pos.addProp("z", wxString::Format(wxT("%f"), it->position.z).mb_str(wxConvUTF8));
                        cXmlNode normal(vert.newChild(RAWXML_SHS_MESH_VERTEX_N));
                        normal.addProp("x", wxString::Format(wxT("%8f"), it->normal.x).mb_str(wxConvUTF8));
                        normal.addProp("y", wxString::Format(wxT("%8f"), it->normal.y).mb_str(wxConvUTF8));
                        normal.addProp("z", wxString::Format(wxT("%8f"), it->normal.z).mb_str(wxConvUTF8));
                    }
                    for(size_t l = 0; l < sh2.indices.size(); l+=3) {
                        cXmlNode tri(child.newChild(RAWXML_SHS_MESH_TRIANGLE));
                        tri.addProp("a", wxString::Format(wxT("%lu"), sh2.indices[l]).mb_str(wxConvUTF8));
                        tri.addProp("b", wxString::Format(wxT("%lu"), sh2.indices[l+1]).mb_str(wxConvUTF8));
                        tri.addProp("c", wxString::Format(wxT("%lu"), sh2.indices[l+2]).mb_str(wxConvUTF8));
                    }
                }
            } else {
                BAKE_SKIP(child);
                cXmlNode subchild(child.children());
                while (subchild) {
                    DO_CONDITION_COMMENT(subchild);
                    if (subchild(RAWXML_SHS_MESH_VERTEX)) {
                        VERTEX v;
                        v.tu = ParseFloat(subchild, wxT(RAWXML_SHS_MESH_VERTEX), wxT("u"));
                        v.tv = ParseFloat(subchild, wxT(RAWXML_SHS_MESH_VERTEX), wxT("v"));
                        if (subchild.hasProp("colour")) {
                            wxString scol = subchild.wxgetPropVal("colour");
                            unsigned long lcol = 0;
                            if (sscanf(scol.ToAscii(), "%lx", &lcol))
                                v.color = lcol;
                        }

                        bool nopos = true;
                        bool nonorm = true;

                        cXmlNode thirdchild(subchild.children());
                        while (thirdchild) {
                            if (thirdchild(RAWXML_SHS_MESH_VERTEX_P)) {
                                nopos = false;
                                ParseVector(thirdchild, v.position, wxT(RAWXML_SHS_MESH_VERTEX_P));
                            } else if (thirdchild(RAWXML_SHS_MESH_VERTEX_N)) {
                                nonorm = false;
                                ParseVector(thirdchild, v.normal, wxT(RAWXML_SHS_MESH_VERTEX_N));
                            } else if (thirdchild.element()) {
                                throw MakeNodeException<RCT3Exception>(wxString::Format(_("Unknown tag '%s' in shs(%s)/shsmesh/vertex."), thirdchild.wxname().c_str(), name.c_str()), thirdchild);
                            }
                            thirdchild.go_next();
                        }

                        if (nopos || nonorm)
                            throw MakeNodeException<RCT3Exception>(wxString::Format(_("Position or normal missing in shs(%s)/shsmesh/vertex."), name.c_str()), subchild);

                        if (!(bb1 || bb2))
                            boundsContain(&v.position, &sh1.bbox1, &sh1.bbox2);

                        sh2.vertices.push_back(v);
                    } else if (subchild(RAWXML_SHS_MESH_TRIANGLE)) {
                        unsigned short index = ParseUnsigned(subchild, wxT(RAWXML_SHS_MESH_TRIANGLE), wxT("a"));
                        sh2.indices.push_back(index);
                        index = ParseUnsigned(subchild, wxT(RAWXML_SHS_MESH_TRIANGLE), wxT("b"));
                        sh2.indices.push_back(index);
                        index = ParseUnsigned(subchild, wxT(RAWXML_SHS_MESH_TRIANGLE), wxT("c"));
                        sh2.indices.push_back(index);
                    } else if (subchild.element()) {
                        throw MakeNodeException<RCT3Exception>(wxString::Format(_("Unknown tag '%s' in shs(%s)/shsmesh."), subchild.wxname().c_str(), name.c_str()), subchild);
                    }
                    subchild.go_next();
                }
            }
            sh1.meshes.push_back(sh2);
        } else if (child(RAWXML_SHS_EFFECT)) {
            BAKE_SKIP(child);
            cEffectStruct effect;
            wxString effectname = ParseString(child, wxT(RAWXML_SHS_EFFECT), wxT("name"), NULL);

            effect.name = effectname.ToAscii();
            bool missp = true;
            cXmlNode subchild(child.children());
            while (subchild) {
                DO_CONDITION_COMMENT(subchild);
                if (subchild(RAWXML_SHS_EFFECT_POS)) {
                    missp = false;
                    ParseMatrix(subchild, effect.pos, wxT(RAWXML_SHS_EFFECT_POS));
                } else if (subchild.element()) {
                    throw MakeNodeException<RCT3Exception>(wxString::Format(_("Unknown tag '%s' in shs(%s)/shseffect(%s)."), subchild.wxname().c_str(), name.c_str(), effectname.c_str()), subchild);
                }
                subchild.go_next();
            }
            if (missp)
                throw MakeNodeException<RCT3Exception>(wxString::Format(_("Missing position in shs(%s)/shseffect(%s)."), name.c_str(), effectname.c_str()), child);

            sh1.effects.push_back(effect);
        } else if (child.element()) {
            throw MakeNodeException<RCT3Exception>(wxString::Format(_("Unknown tag '%s' in shs tag '%s'."), child.wxname().c_str(), name.c_str()), child);
        }
        child.go_next();
    }

    if (m_mode != MODE_BAKE) {
        ovlSHSManager* c_shs = m_ovl.GetManager<ovlSHSManager>();
        c_shs->AddModel(sh1);
    } else if ((!bb1) && (!bb2)) {
        if (m_bake.Index(wxT(RAWXML_SHS)) != wxNOT_FOUND) {
            cXmlNode bb(RAWXML_SHS_BBOX2);
            bb.addProp("x", wxString::Format(wxT("%f"), sh1.bbox2.x).mb_str(wxConvUTF8));
            bb.addProp("y", wxString::Format(wxT("%f"), sh1.bbox2.y).mb_str(wxConvUTF8));
            bb.addProp("z", wxString::Format(wxT("%f"), sh1.bbox2.z).mb_str(wxConvUTF8));
            node.insertNodeAsFirstChild(bb);

            bb = cXmlNode(RAWXML_SHS_BBOX1);
            bb.addProp("x", wxString::Format(wxT("%f"), sh1.bbox1.x).mb_str(wxConvUTF8));
            bb.addProp("y", wxString::Format(wxT("%f"), sh1.bbox1.y).mb_str(wxConvUTF8));
            bb.addProp("z", wxString::Format(wxT("%f"), sh1.bbox1.z).mb_str(wxConvUTF8));
            node.insertNodeAsFirstChild(bb);
        }
    }
}

