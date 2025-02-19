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

#include <wx/log.h>

#include <map>

#include "confhelp.h"
#include "gximage.h"
#include "lib3Dconfig.h"
#include "matrix.h"
#include "rct3constants.h"
#include "xmldefs.h"
#include "xmlhelper.h"
#include "wxexception_libxmlcpp.h"

#include "cXmlDoc.h"
#include "cXmlInputOutputCallbackString.h"
#include "cXmlNode.h"
#include "cXmlValidatorIsoSchematron.h"
#include "cXmlValidatorRNVRelaxNG.h"
#include "cXmlXPath.h"
#include "cXmlXPathResult.h"

#include "rng/model.rnc.gz.h"
#include "rng/model.sch.gz.h"

using namespace r3;
using namespace std;
using namespace xmlcpp;

template<class V>
inline void parseVector(V& v, const cXmlNode& n) {
    parseFloatC(n.getPropVal("x"), v.x);
    parseFloatC(n.getPropVal("y"), v.y);
    parseFloatC(n.getPropVal("z"), v.z);
}

inline bool parseTXYZ(txyz& v, float& w, const cXmlNode& n) {
    parseVector(v, n);
    parseFloatC(n.getPropVal("w"), w);
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
                    weight = roundf(weight * 255.0) / 100.0;
                    if (static_cast<uint8_t>(weight)>0) {
                        v.bone[bone] = it->second.m_id;
                        v.boneweight[bone] = weight;
                        ++bone;
                    }
                }
            }
        }
        ++vert;
    }
    vertex2boneFix(v);
}

void cXML3DLoader::parseAnimation(c3DAnimation& a, xmlcpp::cXmlNode n, float _max) {
    foreach(const cXmlNode& bonen, n.children()) {
        if (bonen("bone")) {
            c3DAnimBone cbone;
            cbone.m_name = bonen.wxgetPropVal("name");
            string rottypes = bonen.getPropVal("rotationformat");
            int rottype = 0;
            if (rottypes == "axis") {
                rottype = 1;
                cbone.m_axis = true;
            } else if (rottypes == "quaternion") {
                rottype = 2;
                cbone.m_axis = true;
            }
            float w;
            foreach(const cXmlNode& frame, bonen.children()) {
                if (frame("translate")) {
                    txyz pt;
                    if (!parseTXYZ(pt, w, frame)) {
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
                    if (!parseTXYZ(pt, w, frame)) {
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
                    switch (rottype) {
                        case 1: {
                            // Axis angle
                            pt.x *= w;
                            pt.y *= w;
                            pt.z *= w;
                            break;
                        }
                        case 2: {
                            // Quaternion
                            if (fabs(w) > 0.99999) {
                                pt.x = 0;
                                pt.y = 0;
                                pt.z = 0;
                                break;
                            }
                            float a = 2*acos(w);
                            float s = a/sqrt(1-(w*w));
                            pt.x *= s;
                            pt.y *= s;
                            pt.z *= s;
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
	
    if (!doc) {
        if (!doc.getStructuredErrors().size())
            throw E3DLoaderNotMyBeer();
        int err = doc.getStructuredErrors()[0].code;
        if ((err == XML_ERR_DOCUMENT_EMPTY) || (err == XML_ERR_GT_REQUIRED))
            throw E3DLoaderNotMyBeer(); // Not an xml file
        // Broken xml file
        //wxString error(_("Invalid xml file:\n"));
        //error += doc.wxgetErrorList();
		wxe_xml_error_infos einfos;
		int eline = transferXmlErrors(doc, einfos);
        throw E3DLoader(_("Invalid xml file")) << wxe_xml_errors(einfos) << wxe_xml_node_line(eline) << wxe_file(filename);
    }

    if (doc.root().ns() != XML_NAMESPACE_MODEL)
        throw E3DLoaderNotMyBeer();

    if (READ_RCT3_VALIDATE()) {
        cXmlInputOutputCallbackString::Init();
        XMLCPP_RES_ADD_ONCE(model, rnc);

        if (READ_RCT3_REPORTVALIDATION()) {
            wxLogMessage(wxString::Format(_("Validating %s..."), filename));
        }
        cXmlValidatorRNVRelaxNG val(XMLCPP_RES_USE(model, rnc).c_str());
        if (!val) {
            //wxString error(_("Internal Error: could not load modxml schema:\n"));
            //error += val.wxgetErrorList();
			wxe_xml_error_infos einfos;
			int eline = transferXmlErrors(val, einfos);
			throw E3DLoader("Internal Error: could not load modxml schema") << wxe_xml_errors(einfos) << wxe_xml_node_line(eline);
        }
        if (doc.validate(val)) {
            //wxString error(_("Invalid modxml file:\n"));
            //error += val.wxgetErrorList();
			wxe_xml_error_infos einfos;
			int eline = transferXmlErrors(val, einfos);
			throw E3DLoader(_("Invalid modxml file")) << wxe_xml_errors(einfos) << wxe_xml_node_line(eline) << wxe_file(filename);
        }

        if (READ_RCT3_DEEPVALIDATE()) {
            XMLCPP_RES_ADD_ONCE(model, sch);
            if (READ_RCT3_REPORTVALIDATION()) {
                wxLogMessage(wxString::Format(_("Deep validating %s..."), filename));
            }
            cXmlValidatorIsoSchematron val(XMLCPP_RES_USE(model, sch).c_str());
            if (!val) {
                //wxString error(_("Internal Error: could not load modxml schema:\n"));
                //error += val.wxgetErrorList();
				wxe_xml_error_infos einfos;
				int eline = transferXmlErrors(val, einfos);
				throw E3DLoader("Internal Error: could not load modxml schematron schema") << wxe_xml_errors(einfos) << wxe_xml_node_line(eline);
            }
            if (doc.validate(val)) {
                //wxString error(_("Invalid modxml file:\n"));
                //error += val.wxgetErrorList();
				wxe_xml_error_infos einfos;
				int eline = transferXmlErrors(val, einfos);
				throw E3DLoader(_("Invalid modxml file")) << wxe_xml_errors(einfos) << wxe_xml_node_line(eline) << wxe_file(filename);
            }

        }

        if (READ_RCT3_REPORTVALIDATION()) {
            wxLogMessage(_("...Ok!"));
        }
    }


    cXmlXPath path(doc, "mod", XML_NAMESPACE_MODEL);

    cXmlXPathResult rmeta = path("/mod:model/mod:metadata[@role='rct3']");
    if (rmeta.size()) {
        foreach(const cXmlNode& n, rmeta[0].children()) {
            if (n("version")) {
                unsigned long v = 0;
                parseULongC(n.content(), v);
                if (v == 1) {
                    m_flags |= r3::Constants::SVD::Flags::Soaked;
                } else if (v==2) {
                    m_flags |= r3::Constants::SVD::Flags::Wild;
                }
            } else if (n("flag")) {
                if (n.content() == "greenery") {
                    m_flags |= r3::Constants::SVD::Flags::Greenery;
                } else if (n.content() == "noshadow") {
                    m_flags |= r3::Constants::SVD::Flags::No_Shadow;
                } else if (n.content() == "rotation") {
                    m_flags |= r3::Constants::SVD::Flags::Rotation;
                } else if (n.content() == "unk01") {
                    m_flags |= r3::Constants::SVD::Flags::Unknown01;
                } else if (n.content() == "unk02") {
                    m_flags |= r3::Constants::SVD::Flags::Unknown02;
                } else if (n.content() == "unk03") {
                    m_flags |= r3::Constants::SVD::Flags::Unknown03;
                } else if (n.content() == "transformedPreview") {
                    m_flags |= r3::Constants::SVD::Flags::Animated_Preview;
                } else if (n.content() == "unkFerris") {
                    m_flags |= r3::Constants::SVD::Flags::Unknown_Giant_Ferris;
                }
            } else if (n("sway")) {
                parseFloatC(n.content(), m_sway);
            } else if (n("brightness")) {
                parseFloatC(n.content(), m_brightness);
            } else if (n("scale")) {
                parseFloatC(n.content(), m_scale);
            } else if (n("unk")) {
                parseFloatC(n.content(), m_unk);
            } else if (n("name")) {
                m_name = n.wxcontent();
            } else if (n("prefix")) {
                m_prefix = n.wxcontent();
            } else if (n("path")) {
                m_path = n.wxcontent();
            }
        }
    }

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


    // Meshes
    cXmlXPathResult meshes = path("/*/mod:mesh");
    for (int m = 0; m < meshes.size(); ++m) {
        cXmlNode xmesh = meshes[m];
        c3DMesh cmesh;
        cmesh.m_name = xmesh.wxgetPropVal("name");
        cmesh.m_texture = xmesh.wxgetPropVal("texture");
        cmesh.m_flag = C3DMESH_VALID;
        path.setNodeContext(xmesh);

        VERTEX2 tv;
        vertex2init(tv);

        cXmlXPathResult meta = path("mod:metadata[@role='rct3']");
        if (meta.size()) {
            foreach(const cXmlNode& n, meta[0].children()) {
                if (n("doubleSided")) {
                    cmesh.m_meshOptions += "ds ";
                } else if (n("textureStyle")) {
                    cmesh.m_meshOptions += n.wxcontent() + " ";
                } else if (n("flags")) {
                    cmesh.m_meshOptions += n.wxcontent() + " ";
                } else if (n("fudgeNormals")) {
                    cmesh.m_meshOptions += wxString("fudge:") + n.wxcontent() + " ";
                }
            }
        }

        cXmlXPathResult vertices = path("mod:vertex");
        cXmlXPathResult triangles = path("mod:triangle");
        vector<VERTEX2> vertices_read(vertices.size(), tv);
        for (int i = 0; i < vertices.size(); ++i) {
            if (vertices[i].hasProp("index")) {
                unsigned long ind = 0;
                if (!parseULongC(vertices[i].getPropVal("index"), ind))
                    throw E3DLoader("Vertex with illegal index attribute") << wxe_xml_node_line(vertices[i].line()) << wxe_file(filename);
                if (ind >= static_cast<unsigned long>(vertices.size()))
                    throw E3DLoader("Vertex with out-of-range index attribute") << wxe_xml_node_line(vertices[i].line()) << wxe_file(filename);
                parseVertex(vertices_read[ind], vertices[i], cmesh);
            } else {
                parseVertex(vertices_read[i], vertices[i], cmesh);
            }
        }


        for (int i = 0; i < triangles.size(); ++i) {
            bool add;
            int j;

            unsigned long a, b, c;
            parseULongC(triangles[i].getPropVal("a"), a);
            parseULongC(triangles[i].getPropVal("b"), b);
            parseULongC(triangles[i].getPropVal("c"), c);

            //parseVertex(tv, vertices[a], cmesh);
            if (a >= static_cast<unsigned long>(vertices.size()))
                throw E3DLoader("Triangle references index out-of-range") << wxe_xml_node_line(triangles[i].line()) << wxe_file(filename);
            tv = vertices_read[a];

            // now see if we have already added this point
            add = true;
            for (j = 0; j < static_cast<int>(cmesh.m_vertices.size()); ++j) {
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
            //parseVertex(tv, vertices[b], cmesh);
            if (b >= static_cast<unsigned long>(vertices.size()))
                throw E3DLoader("Triangle references index out-of-range") << wxe_xml_node_line(triangles[i].line()) << wxe_file(filename);;
            tv = vertices_read[b];

            // now see if we have already added this point
            add = true;
            for (j = 0; j < static_cast<int>(cmesh.m_vertices.size()); ++j) {
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
            //parseVertex(tv, vertices[c], cmesh);
            if (c >= static_cast<unsigned long>(vertices.size()))
                throw E3DLoader("Triangle references index out-of-range") << wxe_xml_node_line(triangles[i].line()) << wxe_file(filename);;
            tv = vertices_read[c];

            // now see if we have already added this point
            add = true;
            for (j = 0; j < static_cast<int>(cmesh.m_vertices.size()); ++j) {
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
        if ((xanim.getPropVal("abstract") == "1") || (xanim.getPropVal("abstract") == "true"))
            continue;

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
                throw E3DLoader(_("Model XML loader: animation tried to inherit from undefined or multi-defined animation")) << wxe_xml_node_line(xanim.line()) << wxe_file(filename);;
            parseAnimation(canim, ianim[0], f);
        }

        parseAnimation(canim, xanim, f);
        m_animations[canim.m_name] = canim;
    }

    // Textures
    cXmlXPathResult textures = path("/*/mod:texture");
    for (int s = 0; s < textures.size(); ++s) {
        cXmlNode xtex = textures[s];
        c3DTexture ctex;
        ctex.m_name = xtex.wxgetPropVal("name");
        ctex.m_file = xtex.wxgetPropVal("file");
        parseULongC(xtex.getPropVal("fps"), ctex.m_fps);
        if (ctex.m_file != "") {
            wxFileName temp(ctex.m_file);
            temp.MakeAbsolute(wxFileName(filename).GetPathWithSep());
            ctex.m_file = temp.GetFullPath();
        }
        ctex.m_alphafile = xtex.wxgetPropVal("alphaFile");
        if (ctex.m_alphafile != "") {
            wxFileName temp(ctex.m_alphafile);
            temp.MakeAbsolute(wxFileName(filename).GetPathWithSep());
            ctex.m_alphafile = temp.GetFullPath();
        }

        ctex.m_alphaType = 0;

        path.setNodeContext(xtex);

        cXmlXPathResult meta = path("mod:metadata[@role='rct3']");
        if (meta.size()) {
            foreach(const cXmlNode& met, meta[0].children()) {
                if (met("recol")) {
                    parseULongC(met.content(), ctex.m_recol);
                } else if (met("useAlpha")) {
                    if (ctex.m_alphafile.IsEmpty()) {
                        wxGXImage i(ctex.m_file);
                        ctex.m_alphaType = i.HasAlpha()?1:0;
                    } else {
                        ctex.m_alphaType = 2;
                    }
                } else if (met("referenced")) {
                    ctex.m_referenced = true;
                    ctex.m_file = met.wxcontent();
                    if (ctex.m_file.IsEmpty())
                        ctex.m_file = "-";
                }
            }
        }

        cXmlXPathResult frames = path("mod:frame");
        cXmlXPathResult sequence = path("mod:sequence");
        if (frames.size()) {
            ctex.m_frames.resize(frames.size());
            for(int i = 0; i < frames.size(); ++i) {
                pair<wxString, wxString> fr;
                fr.first = frames[i].wxgetPropVal("file");
                fr.second = frames[i].wxgetPropVal("alphaFile");
                if (frames[i].hasProp("index")) {
                    unsigned long ind = 0;
                    if (!parseULongC(frames[i].getPropVal("index"), ind))
                        throw E3DLoader("Texture frame with illegal index attribute") << wxe_xml_node_line(frames[i].line()) << wxe_file(filename);
                    if (ind >= static_cast<unsigned long>(frames.size()))
                        throw E3DLoader("Texture frame with out-of-range index attribute") << wxe_xml_node_line(frames[i].line()) << wxe_file(filename);
                    ctex.m_frames[ind] = fr;
                } else {
                    ctex.m_frames[i] = fr;
                }
            }
            for(int i = 0; i < sequence.size(); ++i) {
                unsigned long ind = 0;
                if (!parseULongC(sequence[i].content(), ind))
                    throw E3DLoader("Texture sequence with illegal content") << wxe_xml_node_line(sequence[i].line()) << wxe_file(filename);
                if (ind >= static_cast<unsigned long>(frames.size()))
                    throw E3DLoader("Texture sequence with out-of-range content") << wxe_xml_node_line(sequence[i].line()) << wxe_file(filename);
                ctex.m_sequence.push_back(ind);
            }
        }

        m_textures[ctex.m_name] = ctex;
    }

    // Groups
    cXmlXPathResult groups = path("/*/mod:group");
    for (int s = 0; s < groups.size(); ++s) {
        cXmlNode xgroup = groups[s];
        c3DGroup cgroup;
        cgroup.m_name = xgroup.wxgetPropVal("name");
		if (xgroup.hasProp("animated")) {
			unsigned long a = 0;
			if (parseULongC(xgroup.getPropVal("animated"), a)) {
				if (a)
					cgroup.m_forceanim = c3DGroup::ANIMATED;
				else
					cgroup.m_forceanim = c3DGroup::NONANIMATED;
			} else {
				throw E3DLoader("Unknown content of 'animated' attribute") << wxe_xml_node_line(xgroup.line()) << wxe_file(filename);				
			}
		}

        foreach(const cXmlNode& entries, xgroup.children()) {
            if (entries("mesh")) {
                cgroup.m_meshes.insert(entries.wxcontent());
            } else if (entries("bone")) {
                cgroup.m_bones.insert(entries.wxcontent());
            } else if (entries("animation")) {
				wxString ani(entries.wxcontent());
				if (!ani.IsEmpty())
					cgroup.m_animations.push_back(ani);
            } else if (entries("metadata")) {
                if (entries.getPropVal("role") == "rct3") {
                    foreach(const cXmlNode& met, entries.children()) {
                        if (met("lodDistance")) {
                            parseFloatC(met.content(), cgroup.m_loddistance);
                        }
                    }
                }
            }
        }

        m_groups[cgroup.m_name] = cgroup;
    }

    if (!m_groups.size())
        makeDefaultGroup(wxFileName(filename).GetName());

    calculateBonePos1();
}
