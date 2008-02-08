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

#define DO_PTD(a,b) \
        } else if (cname.IsSameAs(RAWXML_PTD_ ## a)) { \
            wxString c = child->GetNodeContent(); \
            MakeVariable(c); \
            ptd.b[0] = c.ToAscii();

#define DO_PTD_TURN(a, b) DO_PTD(TURN_ ## a, turn_ ## b)
#define DO_PTD_CORNER(a, b) DO_PTD(CORNER_ ## a, corner_ ## b)
#define DO_PTD_COMPLEX(a, c) \
    DO_PTD(a, c.b) \
    DO_PTD(a ## _FC, c.fc) \
    DO_PTD(a ## _BC, c.bc) \
    DO_PTD(a ## _TC, c.tc)

void cRawParser::ParsePTD(wxXmlNode* node) {
    USE_PREFIX(node);

    cPath ptd;
    wxString name = ParseString(node, RAWXML_PTD, wxT("name"), NULL, useprefix);
    ptd.name = name.ToAscii();
    ParseStringOption(ptd.internalname, node, wxT("internalname"), NULL, useprefix);
    ptd.nametxt = ParseString(node, RAWXML_PTD, wxT("nametxt"), NULL, useprefix).ToAscii();
    ptd.icon = ParseString(node, RAWXML_PTD, wxT("icon"), NULL, useprefix).ToAscii();
    unsigned long underwater = 0;
    unsigned long extended = 0;
    OPTION_PARSE(unsigned long, underwater, ParseUnsigned(node, RAWXML_PTD, wxT("underwater")));
    OPTION_PARSE(unsigned long, extended, ParseUnsigned(node, RAWXML_PTD, wxT("extended")));
    if (underwater)
        ptd.unk1 |= cPath::FLAG_UNDERWATER;
    if (extended)
        ptd.unk1 |= cPath::FLAG_EXTENDED;
    wxLogVerbose(wxString::Format(_("Adding ptd %s to %s."), name.c_str(), m_output.GetFullPath().c_str()));

    wxXmlNode *child = node->GetChildren();
    while (child) {
        wxString cname = child->GetName();

        if (cname == RAWXML_PTD_TEXTURE_A) {
            wxString c = child->GetNodeContent();
            MakeVariable(c);
            ptd.texture_a = c.ToAscii();
        } else if (cname == RAWXML_PTD_TEXTURE_B) {
            wxString c = child->GetNodeContent();
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
        } else if (child->GetName() == RAWXML_PTD_PAVING) {
            wxString c = child->GetNodeContent();
            MakeVariable(c);
            ptd.paving = c.ToAscii();
        } else if COMPILER_WRONGTAG(child) {
            throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in ptd tag '%s'."), child->GetName().c_str(), name.c_str()));
        }

        child = child->GetNext();
    }

    ovlPTDManager* c_ptd = m_ovl.GetManager<ovlPTDManager>();
    c_ptd->AddPath(ptd);

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
            OPTION_PARSE(long, sh2.support, ParseSigned(child, RAWXML_SHS_MESH, wxT("support")));

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

