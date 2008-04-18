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

#include "ManagerPTD.h"
#include "ManagerQTD.h"
#include "ManagerSAT.h"
#include "ManagerSHS.h"

#define DO_PTD(a,b) \
        } else if (cname.IsSameAs(wxT(RAWXML_PTD_ ## a))) { \
            wxString c = UTF8STRINGWRAP(child.content()); \
            MakeVariable(c); \
            ptd.b[0] = c.ToAscii();

#define DO_PTD_TURN(a, b) DO_PTD(TURN_ ## a, turn_ ## b)
#define DO_PTD_CORNER(a, b) DO_PTD(CORNER_ ## a, corner_ ## b)
#define DO_PTD_COMPLEX(a, c) \
    DO_PTD(a, c.b) \
    DO_PTD(a ## _FC, c.fc) \
    DO_PTD(a ## _BC, c.bc) \
    DO_PTD(a ## _TC, c.tc)

void cRawParser::ParsePTD(cXmlNode& node) {
    USE_PREFIX(node);

    cPath ptd;
    wxString name = ParseString(node, wxT(RAWXML_PTD), wxT("name"), NULL, useprefix);
    ptd.name = name.ToAscii();
    ParseStringOption(ptd.internalname, node, wxT("internalname"), NULL, useprefix);
    ptd.nametxt = ParseString(node, wxT(RAWXML_PTD), wxT("nametxt"), NULL, useprefix).ToAscii();
    ptd.icon = ParseString(node, wxT(RAWXML_PTD), wxT("icon"), NULL, useprefix).ToAscii();
    unsigned long underwater = 0;
    unsigned long extended = 0;
    OPTION_PARSE(unsigned long, underwater, ParseUnsigned(node, wxT(RAWXML_PTD), wxT("underwater")));
    OPTION_PARSE(unsigned long, extended, ParseUnsigned(node, wxT(RAWXML_PTD), wxT("extended")));
    if (underwater)
        ptd.unk1 |= cPath::FLAG_UNDERWATER;
    if (extended)
        ptd.unk1 |= cPath::FLAG_EXTENDED;
    wxLogVerbose(wxString::Format(_("Adding ptd %s to %s."), name.c_str(), m_output.GetFullPath().c_str()));

    cXmlNode child(node.children());
    while (child) {
        wxString cname = UTF8STRINGWRAP(child.name());

        if (cname == wxT(RAWXML_PTD_TEXTURE_A)) {
            wxString c = UTF8STRINGWRAP(child.content());
            MakeVariable(c);
            ptd.texture_a = c.ToAscii();
        } else if (cname == wxT(RAWXML_PTD_TEXTURE_B)) {
            wxString c = UTF8STRINGWRAP(child.content());
            MakeVariable(c);
            ptd.texture_b = c.ToAscii();
        DO_PTD(FLAT, flat)
        DO_PTD(STRAIGHT_A, straight_a)
        DO_PTD(STRAIGHT_B, straight_b)
        DO_PTD_TURN(U, u)
        DO_PTD_TURN(LA, la)
        DO_PTD_TURN(LB, lb)
        DO_PTD_TURN(TA, ta)
        DO_PTD_TURN(TB, tb)
        DO_PTD_TURN(TC, tc)
        DO_PTD_TURN(X, x)
        DO_PTD_CORNER(A, a)
        DO_PTD_CORNER(B, b)
        DO_PTD_CORNER(C, c)
        DO_PTD_CORNER(D, d)
        DO_PTD_COMPLEX(SLOPE, slope)
        DO_PTD_COMPLEX(SLOPESTRAIGHT, slopestraight)
        DO_PTD_COMPLEX(SLOPESTRAIGHTLEFT, slopestraightleft)
        DO_PTD_COMPLEX(SLOPESTRAIGHTRIGHT, slopestraightright)
        DO_PTD_COMPLEX(SLOPEMID, slopemid)
        DO_PTD(FLAT_FC, flat_fc)
        } else if (cname == wxT(RAWXML_PTD_PAVING)) {
            wxString c = UTF8STRINGWRAP(child.content());
            MakeVariable(c);
            ptd.paving = c.ToAscii();
        } else if (child.element()) {
            throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in ptd tag '%s'."), STRING_FOR_FORMAT(child.name()), name.c_str()));
        }

        child.go_next();
    }

    ovlPTDManager* c_ptd = m_ovl.GetManager<ovlPTDManager>();
    c_ptd->AddPath(ptd);

}

void cRawParser::ParseQTD(cXmlNode& node) {
    USE_PREFIX(node);

    cQueue qtd;
    wxString name = ParseString(node, wxT(RAWXML_QTD), wxT("name"), NULL, useprefix);
    qtd.name = name.ToAscii();
    ParseStringOption(qtd.internalname, node, wxT("internalname"), NULL, useprefix);
    qtd.nametxt = ParseString(node, wxT(RAWXML_QTD), wxT("nametxt"), NULL, useprefix).ToAscii();
    qtd.icon = ParseString(node, wxT(RAWXML_QTD), wxT("icon"), NULL, useprefix).ToAscii();
    qtd.texture = ParseString(node, wxT(RAWXML_QTD), wxT("texture"), NULL, useprefix).ToAscii();
    wxLogVerbose(wxString::Format(_("Adding qtd %s to %s."), name.c_str(), m_output.GetFullPath().c_str()));

    cXmlNode child(node.children());
    while (child) {
        wxString cname = child.wxname();

        if (cname == wxT(RAWXML_QTD_STRAIGHT)) {
            wxString c = child.wxcontent();
            MakeVariable(c);
            qtd.straight = c.ToAscii();
        } else if (cname == wxT(RAWXML_QTD_TURN_L)) {
            wxString c = child.wxcontent();
            MakeVariable(c);
            qtd.turn_l = c.ToAscii();
        } else if (cname == wxT(RAWXML_QTD_TURN_R)) {
            wxString c = child.wxcontent();
            MakeVariable(c);
            qtd.turn_r = c.ToAscii();
        } else if (cname == wxT(RAWXML_QTD_SLOPEUP)) {
            wxString c = child.wxcontent();
            MakeVariable(c);
            qtd.slopeup = c.ToAscii();
        } else if (cname == wxT(RAWXML_QTD_SLOPEDOWN)) {
            wxString c = child.wxcontent();
            MakeVariable(c);
            qtd.slopedown = c.ToAscii();
        } else if (cname == wxT(RAWXML_QTD_SLOPESTRAIGHT)) {
            wxString c = child.wxcontent();
            MakeVariable(c);
            if (qtd.slopestraight[0] == "")
                qtd.slopestraight[0] = c.ToAscii();
            else
                qtd.slopestraight[1] = c.ToAscii();
        } else if (child.element()) {
            throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in qtd tag '%s'."), cname.c_str(), name.c_str()));
        }

        child.go_next();
    }

    ovlQTDManager* c_qtd = m_ovl.GetManager<ovlQTDManager>();
    c_qtd->AddQueue(qtd);

}

void cRawParser::ParseSAT(cXmlNode& node) {
    USE_PREFIX(node);
    cSpecialAttraction spa;
    wxString name = ParseString(node, wxT(RAWXML_SAT), wxT("name"), NULL, useprefix);
    spa.name = name.ToAscii();
    spa.attraction.name = ParseString(node, wxT(RAWXML_SAT), wxT("nametxt"), NULL, useprefix).ToAscii();
    spa.attraction.description = ParseString(node, wxT(RAWXML_SAT), wxT("description"), NULL, useprefix).ToAscii();
    spa.sid = ParseString(node, wxT(RAWXML_SAT), wxT("sid"), NULL, useprefix).ToAscii();
    wxLogVerbose(wxString::Format(_("Adding sat %s to %s."), name.c_str(), m_output.GetFullPath().c_str()));

    cXmlNode child(node.children());
    while (child) {
        DO_CONDITION_COMMENT(child);

        if (child(RAWXML_ATTRACTION)) {
            ParseAttraction(child, spa.attraction);
        } else if (child.element()) {
            throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in sat tag '%s'."), UTF8STRINGWRAP(child.name()), name.c_str()));
        }

        child.go_next();
    }

    ovlSATManager* c_sat = m_ovl.GetManager<ovlSATManager>();
    c_sat->AddAttraction(spa);
}

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
                        throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in shs(%s)/shsmesh."), STRING_FOR_FORMAT(subchild.name()), name.c_str()));
                    }
                    subchild.go_next();
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
                    for(int l = 0; l < sh2.indices.size(); l+=3) {
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
                            wxString scol = UTF8STRINGWRAP(subchild.getPropVal("colour"));
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
                                throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in shs(%s)/shsmesh/vertex."), STRING_FOR_FORMAT(thirdchild.name()), name.c_str()));
                            }
                            thirdchild.go_next();
                        }

                        if (nopos || nonorm)
                            throw RCT3Exception(wxString::Format(_("Position or normal missing in shs(%s)/shsmesh/vertex."), name.c_str()));

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
                        throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in shs(%s)/shsmesh."), STRING_FOR_FORMAT(subchild.name()), name.c_str()));
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
                    throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in shs(%s)/shseffect(%s)."), STRING_FOR_FORMAT(subchild.name()), name.c_str(), effectname.c_str()));
                }
                subchild.go_next();
            }
            if (missp)
                throw RCT3Exception(wxString::Format(_("Missing position in shs(%s)/shseffect(%s)."), name.c_str(), effectname.c_str()));

            sh1.effects.push_back(effect);
        } else if (child.element()) {
            throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in shs tag '%s'."), STRING_FOR_FORMAT(child.name()), name.c_str()));
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

