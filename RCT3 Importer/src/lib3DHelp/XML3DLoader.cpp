///////////////////////////////////////////////////////////////////////////////
//
// 3D Utility Library
// XML Loader
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

#include "XML3DLoader.h"

#include <map>

#include "matrix.h"
#include "xmldefs.h"
#include "xmlhelper.h"

#include "cXmlDoc.h"
#include "cXmlNode.h"
#include "cXmlXPath.h"
#include "cXmlXPathResult.h"

using namespace r3;
using namespace std;
using namespace xmlcpp;

template<class V>
inline void parseVector(V& v, const cXmlNode& n) {
    parseFloatC(n.getPropVal("x"), v.x);
    parseFloatC(n.getPropVal("y"), v.y);
    parseFloatC(n.getPropVal("z"), v.z);
}

inline bool parseTXYZ(txyz& v, const cXmlNode& n) {
    parseVector(v, n);
    return parseFloatC(n.getPropVal("time"), v.time);
}

void cXML3DLoader::parseVertex(VERTEX2& v, cXmlNode n, c3DMesh& mesh) {
    vertex2init(v);
    int bone = 0;
    cXmlNode vert = n.children();

    while (vert) {
        if (vert("position")) {
            parseVector(v.position, vert);
        } else if (vert("normal")) {
            parseVector(v.normal, vert);
        } else if (vert("uv")) {
            parseFloatC(vert.getPropVal("u"), v.tu);
            parseFloatC(vert.getPropVal("v"), v.tv);
        } else if (vert("bone")) {
            if (bone < 4) {
                wxString bname = vert.wxgetPropVal("name");
                mesh.m_bones.insert(bname);
                map<wxString, c3DBone>::iterator it = m_bones.find(bname);
                if (it != m_bones.end()) {
                    float weight = 100.0;
                    parseFloatC(vert.getPropVal("weight"), weight);
                    weight = (weight * 255.0) / 100.0;
                    v.bone[bone] = it->second.m_id;
                    v.boneweight[bone] = weight;
                    ++bone;
                }
            }
        }
        ++vert;
    }
}

void cXML3DLoader::parseAnimation(c3DAnimation& a, xmlcpp::cXmlNode n, float _max) {
    foreach(const cXmlNode& bonen, n.children()) {
        if (bonen("bone")) {
            c3DAnimBone cbone;
            cbone.m_name = bonen.getPropVal("name");
            foreach(const cXmlNode& frame, bonen.children()) {
                if (frame("translate")) {
                    txyz pt;
                    if (!parseTXYZ(pt, frame)) {
                        if (frame.getPropVal("time") == "max") {
                            if (_max > 0.0) {
                                pt.time = _max;
                            } else {
                                wxLogWarning(_("Model XML loader: max keyframe time requested but no maximum time available (skipped)"));
                                continue;
                            }
                        } else {
                            wxLogWarning(_("Model XML loader: failed to parse keyframe time (skipped)"));
                            continue;
                        }
                    }
                    cbone.m_translations.push_back(pt);
                } else if (frame("rotate")) {
                    txyz pt;
                    if (!parseTXYZ(pt, frame)) {
                        if (frame.getPropVal("time") == "max") {
                            if (_max > 0.0) {
                                pt.time = _max;
                            } else {
                                wxLogWarning(_("Model XML loader: max keyframe time requested but no maximum time available (skipped)"));
                                continue;
                            }
                        } else {
                            wxLogWarning(_("Model XML loader: failed to parse keyframe time (skipped)"));
                            continue;
                        }
                    }
                    cbone.m_rotations.push_back(pt);
                }
            }
            a.m_bones[cbone.m_name] = cbone;
        }
    }
}


cXML3DLoader::cXML3DLoader(const wxChar *filename): c3DLoader(filename), m_ori(ORIENTATION_RIGHT_XUP) {
    wxString fn(filename);
    cXmlDoc doc(fn.utf8_str(), NULL, XML_PARSE_DTDLOAD);
    if (!doc)
        throw E3DLoaderNotMyBeer();

    if (doc.root().ns() != XML_NAMESPACE_MODEL)
        throw E3DLoaderNotMyBeer();

    cXmlXPath path(doc, "mod", XML_NAMESPACE_MODEL);

    // Fix
    cXmlNode fix = path("//mod:system")[0];
    if (fix) {
        wxString hand = fix.wxgetPropVal("handedness", "right");
        hand.MakeLower();
        wxString up = fix.wxgetPropVal("up", "z");
        up.MakeLower();
        if (hand == wxT("left")) {
            if (up == wxT("x")) {
                m_ori = ORIENTATION_LEFT_XUP;
            } else if (up == wxT("y")) {
                m_ori = ORIENTATION_LEFT_YUP;
            } else if (up == wxT("z")) {
                m_ori = ORIENTATION_LEFT_ZUP;
            }
        } else if (hand == wxT("right")) {
            if (up == wxT("x")) {
                m_ori = ORIENTATION_RIGHT_XUP;
            } else if (up == wxT("y")) {
                m_ori = ORIENTATION_RIGHT_YUP;
            } else if (up == wxT("z")) {
                m_ori = ORIENTATION_RIGHT_ZUP;
            }
        }
    }

    // Bones
    // Do this first so we can resolve bone assignments in vertices
    cXmlXPathResult bones = path("/*/mod:bone");
    for (int i = 0; i < bones.size(); ++i) {
        cXmlNode xbone = bones[i];
        c3DBone bone;
        bone.m_name = xbone.wxgetPropVal("name");
        bone.m_parent = xbone.wxgetPropVal("parent");
        wxString temp = xbone.wxgetPropVal("role");
        if (temp.IsEmpty()) {
            temp = wxT("Bone");
        } else {
            wxString a = temp[0];
            wxString b = temp.Mid(1);
            temp = a.MakeUpper() + b;
        }
        bone.m_type = temp;

        cXmlNode mat = xbone.children();
        while (mat) {
            if (mat("matrix")) {
                int nrow = 0;
                cXmlNode row = mat.children();
                string temp;
                while (row) {
                    if (row("row")) {
                        temp = row();
                        parseMatrixRow(temp, bone.m_pos[1].m[nrow][0], bone.m_pos[1].m[nrow][1], bone.m_pos[1].m[nrow][2], bone.m_pos[1].m[nrow][3]);
                        ++nrow;
                    }
                    ++row;
                }
            }
            ++mat;
        }

        bone.m_id = m_bones.size();
        m_bones[bone.m_name] = bone;
        m_boneId.push_back(bone.m_name);
    }
    calculateBonePos1();


    // Meshes
    cXmlXPathResult meshes = path("/*/mod:mesh");
    for (int m = 0; m < meshes.size(); ++m) {
        cXmlNode xmesh = meshes[m];
        c3DMesh cmesh;
        cmesh.m_name = xmesh.wxgetPropVal("name");
        cmesh.m_flag = C3DMESH_VALID;
        path.setNodeContext(xmesh);

        VERTEX2 tv;

        cXmlXPathResult vertices = path("mod:vertex");
        cXmlXPathResult triangles = path("mod:triangle");
        for (int i = 0; i < triangles.size(); ++i) {
            bool add;
            int j;

            unsigned long a, b, c;
            parseULongC(triangles[i].getPropVal("a"), a);
            parseULongC(triangles[i].getPropVal("b"), b);
            parseULongC(triangles[i].getPropVal("c"), c);

            parseVertex(tv, vertices[a], cmesh);

            // now see if we have already added this point
            add = true;
            for (j = 0; j < cmesh.m_vertices.size(); ++j) {
                VERTEX2 *pv = &cmesh.m_vertices[j];
                if (memcmp(pv, &tv, sizeof(VERTEX2)) == 0) {
                    // we have a match so exit
                    add = false;
                    break;
                }
            }
            if (add) {
                cmesh.m_vertices.push_back(tv);
            }
            // j should have our real index value now as well
            cmesh.m_indices.push_back(j);

            // now for the second one
            parseVertex(tv, vertices[b], cmesh);

            // now see if we have already added this point
            add = true;
            for (j = 0; j < (unsigned long) cmesh.m_vertices.size(); ++j) {
                VERTEX2 *pv = &cmesh.m_vertices[j];
                if (memcmp(pv, &tv, sizeof(VERTEX2)) == 0) {
                    // we have a match so exit
                    add = false;
                    break;
                }
            }
            if (add) {
                cmesh.m_vertices.push_back(tv);
            }
            // j should have our real index value now as well
            cmesh.m_indices.push_back(j);

            // now for the third one
            parseVertex(tv, vertices[c], cmesh);

            // now see if we have already added this point
            add = true;
            for (j = 0; j < (unsigned long) cmesh.m_vertices.size(); ++j) {
                VERTEX2 *pv = &cmesh.m_vertices[j];
                if (memcmp(pv, &tv, sizeof(VERTEX2)) == 0) {
                    // we have a match so exit
                    add = false;
                    break;
                }
            }
            if (add) {
                cmesh.m_vertices.push_back(tv);
            }
            // j should have our real index value now as well
            cmesh.m_indices.push_back(j);
        }
        m_meshes[cmesh.m_name] = cmesh;
        m_meshId.push_back(cmesh.m_name);
    }

    // Splines
    cXmlXPathResult splines = path("/*/mod:spline");
    for (int s = 0; s < splines.size(); ++s) {
        cXmlNode xspline = splines[s];
        c3DSpline cspline;
        cspline.m_name = xspline.wxgetPropVal("name");
        if (xspline.hasProp("cyclic"))
            parseULongC(xspline.getPropVal("cyclic"), cspline.m_cyclic);

        cXmlNode nodes = xspline.children();
        while (nodes) {
            if (nodes("node")) {
                SplineNode rawnode;
                memset(&rawnode, 0, sizeof(SplineNode));

                cXmlNode vecs = nodes.children();
                while (vecs) {
                    if (vecs("position")) {
                        parseVector(rawnode.pos, vecs);
                    } else if (vecs("controlPrev")) {
                        parseVector(rawnode.cp1, vecs);
                    } else if (vecs("controlNext")) {
                        parseVector(rawnode.cp2, vecs);
                    }
                    ++vecs;
                }

                rawnode.cp1.x -= rawnode.pos.x;
                rawnode.cp1.y -= rawnode.pos.y;
                rawnode.cp1.z -= rawnode.pos.z;
                rawnode.cp2.x -= rawnode.pos.x;
                rawnode.cp2.y -= rawnode.pos.y;
                rawnode.cp2.z -= rawnode.pos.z;

                cspline.m_nodes.push_back(rawnode);
            }

            ++nodes;
        }

        m_splines[cspline.m_name] = cspline;
    }

    // Animations
    cXmlXPathResult animations = path("/*/mod:animation");
    cXmlXPath maxpath(doc, "mod", XML_NAMESPACE_MODEL);
    for (int s = 0; s < animations.size(); ++s) {
        cXmlNode xanim = animations[s];
        maxpath.setNodeContext(xanim);
        float f = 0.0;
        cXmlXPathResult times = maxpath("mod:bone/*/@time");
        for(int t = 0; t < times.size(); ++t) {
            float x = 0.0;
            if (parseFloatC(times[t](), x)) {
                if (x>f)
                    f = x;
            }
        }

        c3DAnimation canim;
        canim.m_name = xanim.wxgetPropVal("name");
        if (xanim.hasProp("inherit")) {
            cXmlXPath ianimpath(doc, "mod", XML_NAMESPACE_MODEL);
            wxString ipath = wxString::Format("/*/mod:animation[@name=\"%s\"]", xanim.wxgetPropVal("inherit").c_str());
            cXmlXPathResult ianim = ianimpath(ipath.utf8_str());
            if (ianim.size() != 1)
                throw E3DLoader(_("Model XML loader: animation tried to inherit from undefined or multi-defined animation."));
            parseAnimation(canim, ianim[0], f);
        }

        parseAnimation(canim, xanim, f);
        m_animations[canim.m_name] = canim;
    }

    // Groups
    cXmlXPathResult groups = path("/*/mod:group");
    for (int s = 0; s < groups.size(); ++s) {
        cXmlNode xgroup = groups[s];
        c3DGroup cgroup;
        cgroup.m_name = xgroup.wxgetPropVal("name");

        foreach(const cXmlNode& entries, xgroup.children()) {
            if (entries("mesh")) {
                cgroup.m_meshes.insert(entries());
            } else if (entries("bone")) {
                cgroup.m_bones.insert(entries());
            } else if (entries("animation")) {
                cgroup.m_animations.insert(entries());
            } else if (entries("rct3")) {
                parseFloatC(entries.getPropVal("lodDistance"), cgroup.m_loddistance);
            }
        }

        m_groups[cgroup.m_name] = cgroup;
    }

    if (!m_groups.size())
        makeDefaultGroup(wxFileName(filename).GetName());

}
