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

//#include <boost/algorithm/string.hpp>

#include "ManagerBAN.h"

#include "3DLoader.h"
#include "matrix.h"
#include "RCT3Structs.h"


void cRawParser::ParseBAN(cXmlNode& node) {
    USE_PREFIX(node);
    cBoneAnim ban;
    wxString name = ParseString(node, RAWXML_BAN, "name", NULL, useprefix);
    ban.name = name.ToAscii();
    wxLogVerbose(wxString::Format(_("Adding ban %s to %s."), name.c_str(), m_output.GetFullPath().c_str()));

    if (node.hasProp("modelFile")) {
        wxString animname = ParseString(node, RAWXML_BAN, "name", NULL);
        ParseStringOption(animname, node, "modelAnimation", NULL);
        wxFSFileName modelfile = ParseString(node, RAWXML_BAN, "modelFile", NULL);
        if (!modelfile.IsAbsolute())
            modelfile.MakeAbsolute(m_input.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME));

        // TODO: baking

        boost::shared_ptr<c3DLoader> model(c3DLoader::LoadFile(modelfile.GetFullPath().c_str()));
        if (!model.get())
            throw MakeNodeException<RCT3Exception>(_("ban tag: Model file not readable or has unknown format: ") + modelfile.GetFullPath(), node);

        if (!model->getAnimations().size()) {
            throw MakeNodeException<RCT3Exception>(_("ban tag: Model file does not contain animations: ") + modelfile.GetFullPath(), node);
        }

        std::map<wxString, c3DAnimation>::const_iterator sp = model->getAnimations().find(animname);
        if (sp == model->getAnimations().end()) {
            throw MakeNodeException<RCT3Exception>(wxString::Format(_("ban tag: Model file '%s' does not contain animation '%s'"), modelfile.GetFullPath().c_str(), animname.c_str()), node);
        }

        c3DLoaderOrientation ori = ParseOrientation(node, wxString::Format(wxT("ban(%s) tag"), name.c_str()), model->getOrientation());
        ban.calc_time = true;

        foreach(const c3DAnimBone::pair& bone, sp->second.m_bones) {
            cBoneAnimBone cbone;
            cbone.name = bone.second.m_name.ToAscii();
            foreach(const txyz& fr, bone.second.m_translations) {
                cbone.translations.insert(cTXYZ(fr).GetFixed(ori, false));
            }
            foreach(const txyz& fr, bone.second.m_rotations) {
                cbone.rotations.insert(cTXYZ(fr).GetFixed(ori, true, bone.second.m_axis));
            }
            ban.bones.push_back(cbone);
        }

    } else {
        c3DLoaderOrientation ori = ParseOrientation(node, wxString::Format(wxT("ban(%s) tag"), name.c_str()));
        OPTION_PARSE(float, ban.totaltime, ParseFloat(node, wxT(RAWXML_BAN), wxT("totaltime")));
        if (ban.totaltime != 0.0)
            ban.calc_time = false;
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
                        throw MakeNodeException<RCT3Exception>(wxString::Format(_("Unknown tag '%s' in ban(%s)/banbone(%s)"), subchild.wxname().c_str(), name.c_str(), bonename.c_str()), subchild);
                    }

                    subchild.go_next();
                }

                ban.bones.push_back(banim);
            } else if (child.element()) {
                throw MakeNodeException<RCT3Exception>(wxString::Format(_("Unknown tag '%s' in ban tag '%s'"), child.wxname().c_str(), name.c_str()), child);
            }

            child.go_next();
        }
    }

    ovlBANManager* c_ban = m_ovl.GetManager<ovlBANManager>();
    c_ban->AddAnimation(ban);
}

