///////////////////////////////////////////////////////////////////////////////
//
// raw ovl xml interpreter
// Command line ovl creation utility
// Copyright (C) 2010 Tobias Minch
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

#include "ManagerMAM.h"

#include "3DLoader.h"
#include "matrix.h"
#include "RCT3Structs.h"


void cRawParser::ParseMAM(cXmlNode& node) {
    USE_PREFIX(node);
    cManifoldMesh mam;
    wxString name = ParseString(node, wxT(RAWXML_MAM), wxT("name"), NULL, useprefix);
    mam.name = name.ToAscii();
    boundsInit(&mam.bbox1, &mam.bbox2);
    cXmlNode bb1;
    cXmlNode bb2;
    wxLogVerbose(wxString::Format(_("Adding mam %s to %s."), name.c_str(), m_output.GetFullPath().c_str()));

    cXmlNode child(node.children());
    while (child) {
        DO_CONDITION_COMMENT(child);

        if (child(RAWXML_MAM_BBOX1)) {
            bb1 = child;
            ParseVector(child, mam.bbox1, wxT(RAWXML_SHS_BBOX1));
        } else if (child(RAWXML_MAM_BBOX2)) {
            bb2 = child;
            ParseVector(child, mam.bbox2, wxT(RAWXML_SHS_BBOX2));
        } else if (child(RAWXML_MAM_MESH)) {
            USE_PREFIX(child);

            if (child.hasProp("meshname")) {
                wxString meshname = ParseString(child, wxT(RAWXML_MAM_MESH), wxT("meshname"), NULL);
                bool modelfilevar = false;
                wxFSFileName modelfile = ParseString(child, wxT(RAWXML_MAM_MESH), wxT("modelfile"), &modelfilevar);
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
                        throw MakeNodeException<RCT3Exception>(wxString::Format(_("Unknown value '%s' for up attribute in shs(%s)/shsmesh tag"), up.c_str(), name.c_str()), child);
                    }
                } else if (hand == wxT("right")) {
                    if (up == wxT("x")) {
                        ori = ORIENTATION_RIGHT_XUP;
                    } else if (up == wxT("y")) {
                        ori = ORIENTATION_RIGHT_YUP;
                    } else if (up == wxT("z")) {
                        ori = ORIENTATION_RIGHT_ZUP;
                    } else {
                        throw MakeNodeException<RCT3Exception>(wxString::Format(_("Unknown value '%s' for up attribute in shs(%s)/shsmesh tag"), up.c_str(), name.c_str()), child);
                    }
                } else {
                    throw MakeNodeException<RCT3Exception>(wxString::Format(_("Unknown value '%s' for handedness attribute in shs(%s)/shsmesh tag"), up.c_str(), name.c_str()), child);
                }

                if (!modelfile.IsAbsolute())
                    modelfile.MakeAbsolute(m_input.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME));

                if (m_mode == MODE_BAKE) {
                    if (m_bake.Index(wxT(RAWXML_MAM)) == wxNOT_FOUND) {
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
                    throw MakeNodeException<RCT3Exception>(_("mam tag: Model file not readable or has unknown format: ") + modelfile.GetFullPath(), child);

                int meshnr = -1;
                for (unsigned int i = 0; i < model->getObjectCount(); ++i) {
                    if (meshname == model->getObjectName(i)) {
                        meshnr = i;
                        break;
                    }
                }
                if (meshnr == -1)
                    throw MakeNodeException<RCT3Exception>(wxString::Format(_("mam tag: Model file '%s' does not contain a mesh called '%s'"), modelfile.GetFullPath().c_str(), meshname.c_str()), child);

                VECTOR temp_min;
                VECTOR temp_max;

                MATRIX m = matrixGetFixOrientation(ori);
                bool do_matrix = ori != ORIENTATION_LEFT_YUP;

                cXmlNode subchild(child.children());
                while (subchild) {
                    if (subchild(RAWXML_MAM_MESH_TRANSFORM)) {
                        if ((ori != ORIENTATION_LEFT_YUP) && (m_mode != MODE_INSTALL))
                            wxLogWarning(_("You've set handedness and up as well as giving a transformation matrix. The handedness/up is ignored for transforming the mesh."));
                        ParseMatrix(subchild, m, wxT(RAWXML_MAM_MESH_TRANSFORM));
                        do_matrix = true;
                    } else if (subchild.element()) {
                        throw MakeNodeException<RCT3Exception>(wxString::Format(_("Unknown tag '%s' in mam(%s)/mesh"), subchild.wxname().c_str(), name.c_str()), subchild);
                    }
                    subchild.go_next();
                }

                boundsInit(&temp_min, &temp_max);
                model->fetchObject(meshnr, &mam, &temp_min, &temp_max,
                                          do_matrix?&m:NULL);
                if (!(bb1 || bb2))
                    boundsContain(&temp_min, &temp_max, &mam.bbox1, &mam.bbox2);

                if (m_mode == MODE_BAKE) {
                    child.delProp("meshname");
                    child.delProp("modelfile");
                    child.delProp("handedness");
                    child.delProp("up");
                    child.delProp("fudge");
                    if (child.children())
                        child.children().detach();

                    for(std::vector<ManifoldMeshVertex>::iterator it = mam.vertices.begin(); it != mam.vertices.end(); ++it) {
                        cXmlNode vert(child.newChild(RAWXML_MAM_MESH_VERTEX));
                        vert.addProp("unk04", wxString::Format(wxT("%lu"), it->unk04).mb_str(wxConvUTF8));
                        cXmlNode pos(vert.newChild(RAWXML_MAM_MESH_VERTEX_P));
                        pos.addProp("x", wxString::Format(wxT("%f"), it->position.x).mb_str(wxConvUTF8));
                        pos.addProp("y", wxString::Format(wxT("%f"), it->position.y).mb_str(wxConvUTF8));
                        pos.addProp("z", wxString::Format(wxT("%f"), it->position.z).mb_str(wxConvUTF8));
                    }
                    for(size_t l = 0; l < mam.indices.size(); l+=3) {
                        cXmlNode tri(child.newChild(RAWXML_MAM_MESH_TRIANGLE));
                        tri.addProp("a", wxString::Format(wxT("%u"), mam.indices[l]).mb_str(wxConvUTF8));
                        tri.addProp("b", wxString::Format(wxT("%u"), mam.indices[l+1]).mb_str(wxConvUTF8));
                        tri.addProp("c", wxString::Format(wxT("%u"), mam.indices[l+2]).mb_str(wxConvUTF8));
                    }
                }
            } else {
                BAKE_SKIP(child);
                cXmlNode subchild(child.children());
                while (subchild) {
                    DO_CONDITION_COMMENT(subchild);
                    if (subchild(RAWXML_MAM_MESH_VERTEX)) {
                        ManifoldMeshVertex v;
						v.unk04 = ParseUnsigned(subchild, wxT(RAWXML_SHS_MESH_TRIANGLE), wxT("b"));

                        bool nopos = true;

                        cXmlNode thirdchild(subchild.children());
                        while (thirdchild) {
                            if (thirdchild(RAWXML_MAM_MESH_VERTEX_P)) {
                                nopos = false;
                                ParseVector(thirdchild, v.position, wxT(RAWXML_SHS_MESH_VERTEX_P));
                            } else if (thirdchild.element()) {
                                throw MakeNodeException<RCT3Exception>(wxString::Format(_("Unknown tag '%s' in mam(%s)/mesh/vertex"), thirdchild.wxname().c_str(), name.c_str()), thirdchild);
                            }
                            thirdchild.go_next();
                        }

                        if (nopos)
                            throw MakeNodeException<RCT3Exception>(wxString::Format(_("Position missing in mam(%s)/mesh/vertex"), name.c_str()), subchild);

                        if (!(bb1 || bb2))
                            boundsContain(&v.position, &mam.bbox1, &mam.bbox2);

                        mam.vertices.push_back(v);
                    } else if (subchild(RAWXML_MAM_MESH_TRIANGLE)) {
                        unsigned short index = ParseUnsigned(subchild, wxT(RAWXML_MAM_MESH_TRIANGLE), wxT("a"));
                        mam.indices.push_back(index);
                        index = ParseUnsigned(subchild, wxT(RAWXML_MAM_MESH_TRIANGLE), wxT("b"));
                        mam.indices.push_back(index);
                        index = ParseUnsigned(subchild, wxT(RAWXML_MAM_MESH_TRIANGLE), wxT("c"));
                        mam.indices.push_back(index);
                    } else if (subchild.element()) {
                        throw MakeNodeException<RCT3Exception>(wxString::Format(_("Unknown tag '%s' in mam(%s)/mesh"), subchild.wxname().c_str(), name.c_str()), subchild);
                    }
                    subchild.go_next();
                }
            }
        } else if (child.element()) {
            throw MakeNodeException<RCT3Exception>(wxString::Format(_("Unknown tag '%s' in mam tag '%s'"), child.wxname().c_str(), name.c_str()), child);
        }
        child.go_next();
    }

    if (m_mode != MODE_BAKE) {
        ovlMAMManager* c_mam = m_ovl.GetManager<ovlMAMManager>();
        c_mam->AddMesh(mam);
    } else if ((!bb1) && (!bb2)) {
        if (m_bake.Index(wxT(RAWXML_MAM)) != wxNOT_FOUND) {
            cXmlNode bb(RAWXML_MAM_BBOX2);
            bb.addProp("x", wxString::Format(wxT("%f"), mam.bbox2.x).mb_str(wxConvUTF8));
            bb.addProp("y", wxString::Format(wxT("%f"), mam.bbox2.y).mb_str(wxConvUTF8));
            bb.addProp("z", wxString::Format(wxT("%f"), mam.bbox2.z).mb_str(wxConvUTF8));
            node.insertNodeAsFirstChild(bb);

            bb = cXmlNode(RAWXML_MAM_BBOX1);
            bb.addProp("x", wxString::Format(wxT("%f"), mam.bbox1.x).mb_str(wxConvUTF8));
            bb.addProp("y", wxString::Format(wxT("%f"), mam.bbox1.y).mb_str(wxConvUTF8));
            bb.addProp("z", wxString::Format(wxT("%f"), mam.bbox1.z).mb_str(wxConvUTF8));
            node.insertNodeAsFirstChild(bb);
        }
    }
}

