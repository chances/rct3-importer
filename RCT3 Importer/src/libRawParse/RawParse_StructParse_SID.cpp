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

#include "ManagerSID.h"

#include "pretty.h"

//#include <boost/iostreams/device/array.hpp>
//#include <boost/iostreams/stream.hpp>

#define RAWXML_SID_GROUP            "group"
#define RAWXML_SID_TYPE             "type"
#define RAWXML_SID_POSITION         "position"
#define RAWXML_SID_COLOURS          "colours"
//#define RAWXML_SID_SVD              "sidvisual"
#define RAWXML_SID_IMPORTERUNKNOWNS "importerUnknowns"
#define RAWXML_SID_EXTRA            "extra"
#define RAWXML_SID_FLATRIDE         "flatRide"
#define RAWXML_SID_FLATRIDE_ANIM    "animation"
#define RAWXML_SID_SOUND            "sound"
#define RAWXML_SID_SOUND_REF        "soundRef"
#define RAWXML_SID_SOUND_ANIM       "animation"
#define RAWXML_SID_SOUND_ANIM_COMMAND "command"
#define RAWXML_SID_SQUAREUNKNOWNS   "squareUnknowns"
#define RAWXML_SID_STALLUNKNOWNS    "stallUnknowns"
#define RAWXML_SID_PARAMETER        "parameter"

void cRawParser::ParseSID(cXmlNode& node) {
    USE_PREFIX(node);
    cSid sid;
    wxString name = ParseString(node, wxT(RAWXML_SID), wxT("name"), NULL, useprefix);
    sid.name = name.ToAscii();
    sid.ovlpath = ParseString(node, wxT(RAWXML_SID), wxT("ovlpath"), NULL).ToAscii();
    sid.ui.name = ParseString(node, wxT(RAWXML_SID), wxT("nametxt"), NULL, useprefix).ToAscii();
    ParseStringOption(sid.ui.icon, node, wxT("icon"), NULL, useprefix);
    wxString svd = ParseString(node, wxT(RAWXML_SID), wxT("svd"), NULL, useprefix);
    sid.svds.push_back(std::string(svd.ToAscii()));
    wxLogVerbose(wxString::Format(_("Adding sid %s to %s."), wxString(sid.name.c_str(), wxConvLocal).c_str(), m_output.GetFullPath().c_str()));

    cXmlNode child(node.children());
    while (child) {
        DO_CONDITION_COMMENT(child);

        if (child(RAWXML_SID_GROUP)) {
            USE_PREFIX(child);
            wxString name = ParseString(child, wxT(RAWXML_SID_GROUP), wxT("name"), NULL, useprefix);
            wxString icon = ParseString(child, wxT(RAWXML_SID_GROUP), wxT("icon"), NULL, useprefix);
            sid.ui.group = name.ToAscii();
            sid.ui.groupicon = icon.ToAscii();
        } else if (child(RAWXML_SID_TYPE)) {
            OPTION_PARSE(unsigned long, sid.ui.type, ParseUnsigned(child, wxT(RAWXML_SID_TYPE), wxT("scenerytype")));
            OPTION_PARSE(long, sid.ui.cost, ParseSigned(child, wxT(RAWXML_SID_TYPE), wxT("cost")));
            OPTION_PARSE(long, sid.ui.removal_cost, ParseSigned(child, wxT(RAWXML_SID_TYPE), wxT("removalcost")));
        } else if (child(RAWXML_SID_POSITION)) {
            OPTION_PARSE(unsigned long, sid.position.positioningtype, ParseUnsigned(child, wxT(RAWXML_SID_POSITION), wxT("type")));
            OPTION_PARSE(unsigned long, sid.position.xsquares, ParseUnsigned(child, wxT(RAWXML_SID_POSITION), wxT("xsquares")));
            OPTION_PARSE(unsigned long, sid.position.zsquares, ParseUnsigned(child, wxT(RAWXML_SID_POSITION), wxT("zsquares")));
            OPTION_PARSE(float, sid.position.xpos, ParseFloat(child, wxT(RAWXML_SID_POSITION), wxT("xpos")));
            OPTION_PARSE(float, sid.position.ypos, ParseFloat(child, wxT(RAWXML_SID_POSITION), wxT("ypos")));
            OPTION_PARSE(float, sid.position.zpos, ParseFloat(child, wxT(RAWXML_SID_POSITION), wxT("zpos")));
            OPTION_PARSE(float, sid.position.xsize, ParseFloat(child, wxT(RAWXML_SID_POSITION), wxT("xsize")));
            OPTION_PARSE(float, sid.position.ysize, ParseFloat(child, wxT(RAWXML_SID_POSITION), wxT("ysize")));
            OPTION_PARSE(float, sid.position.zsize, ParseFloat(child, wxT(RAWXML_SID_POSITION), wxT("zsize")));
            ParseStringOption(sid.position.supports, child, wxT("supports"), NULL, false);
        } else if (child(RAWXML_SID_COLOURS)) {
            OPTION_PARSE(unsigned long, sid.colours.defaultcol[0], ParseUnsigned(child, wxT(RAWXML_SID_COLOURS), wxT("choice1")));
            OPTION_PARSE(unsigned long, sid.colours.defaultcol[1], ParseUnsigned(child, wxT(RAWXML_SID_COLOURS), wxT("choice2")));
            OPTION_PARSE(unsigned long, sid.colours.defaultcol[2], ParseUnsigned(child, wxT(RAWXML_SID_COLOURS), wxT("choice3")));
        } else if (child(RAWXML_SID_SQUAREUNKNOWNS)) {
            cSidSquareUnknowns squ;
            OPTION_PARSE(long, squ.min_height, ParseSigned(child, wxT(RAWXML_SID_SQUAREUNKNOWNS), wxT("minHeight")));
            OPTION_PARSE(long, squ.max_height, ParseSigned(child, wxT(RAWXML_SID_SQUAREUNKNOWNS), wxT("maxHeight")));
            OPTION_PARSE(unsigned long, squ.supports, ParseUnsigned(child, wxT(RAWXML_SID_SQUAREUNKNOWNS), wxT("supports")));
            OPTION_PARSE(unsigned long, squ.flags, ParseUnsigned(child, wxT(RAWXML_SID_SQUAREUNKNOWNS), wxT("flags")));

/*
            if (child.hasProp("flags")) {
                wxString flags = ParseString(child, wxT(RAWXML_SID_SQUAREUNKNOWNS), wxT("flags"), NULL);
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
*/
            sid.squareunknowns.push_back(squ);
        } else if (child(RAWXML_SID_IMPORTERUNKNOWNS)) {
            OPTION_PARSE(unsigned long, sid.importerunknowns.unk1, ParseUnsigned(child, wxT(RAWXML_SID_IMPORTERUNKNOWNS), wxT("u1")));
            OPTION_PARSE(unsigned long, sid.importerunknowns.unk2, ParseUnsigned(child, wxT(RAWXML_SID_IMPORTERUNKNOWNS), wxT("u2")));
            OPTION_PARSE(unsigned long, sid.importerunknowns.flags, ParseUnsigned(child, wxT(RAWXML_SID_IMPORTERUNKNOWNS), wxT("flags")));
            /*
            if (child.hasProp("flags")) {
                wxString flags = ParseString(child, wxT(RAWXML_SID_IMPORTERUNKNOWNS), wxT("flags"), NULL);
                if (flags.Length() != 32)
                    throw RCT3InvalidValueException(_("The flags attribute of a sidimporterunknowns tag is of invalid length"));
                //const char* fl = flags.ToAscii();
                for (int i = 0; i < 32; ++i) {
                    if (flags[i] == wxT('0')) {
                        sid.importerunknowns.flag[i] = false;
                    } else if (flags[i] == wxT('1')) {
                        sid.importerunknowns.flag[i] = true;
                    } else {
                        throw RCT3InvalidValueException(_("The flags attribute of a sidimporterunknowns tag has an invalid character"));
                    }
                }
            }
            */
        } else if (child(RAWXML_SID_STALLUNKNOWNS)) {
            sid.stallunknowns.MakeStall();
            OPTION_PARSE(unsigned long, sid.stallunknowns.unk1, ParseUnsigned(child, wxT(RAWXML_SID_STALLUNKNOWNS), wxT("u1")));
            OPTION_PARSE(unsigned long, sid.stallunknowns.unk2, ParseUnsigned(child, wxT(RAWXML_SID_STALLUNKNOWNS), wxT("u2")));
        } else if (child(RAWXML_SID_EXTRA)) {
            OPTION_PARSE(unsigned long, sid.extra.version, ParseUnsigned(child, wxT(RAWXML_SID_EXTRA), wxT("version")));
            OPTION_PARSE(unsigned long, sid.extra.AddonPack, ParseUnsigned(child, wxT(RAWXML_SID_EXTRA), wxT("addonpack")));
            OPTION_PARSE(long, sid.extra.billboardaspect, ParseSigned(child, wxT(RAWXML_SID_EXTRA), wxT("billboardaspect")));
        } else if (child(RAWXML_SID_FLATRIDE)) {
            OPTION_PARSE(unsigned long, sid.flatride.chunked_anr_animation_chunks, ParseUnsigned(child, wxT(RAWXML_SID_FLATRIDE), wxT("chunkCount")));
            if (sid.flatride.chunked_anr_animation_chunks)
                sid.flatride.chunked_anr_unk2 = 1;
            OPTION_PARSE(unsigned long, sid.flatride.chunked_anr_unk1, ParseUnsigned(child, wxT(RAWXML_SID_FLATRIDE), wxT("ageAlternatives")));
            OPTION_PARSE(unsigned long, sid.flatride.chunked_anr_unk2, ParseUnsigned(child, wxT(RAWXML_SID_FLATRIDE), wxT("runAnimations")));
            OPTION_PARSE(unsigned long, sid.flatride.chunked_anr_unk3, ParseUnsigned(child, wxT(RAWXML_SID_FLATRIDE), wxT("cyclesPerCircuit")));
            foreach(const cXmlNode& subchild, child.children()) {
                USE_PREFIX(subchild);
                DO_CONDITION_COMMENT_FOR(subchild);

                if (subchild(RAWXML_SID_FLATRIDE_ANIM)) {
                    wxString c = HandleStringContent(subchild(), NULL, useprefix);
                    if (!c.IsEmpty())
                        sid.flatride.individual_animations.push_back(static_cast<const char*>(c.ToAscii()));
                } else if (subchild.element()) {
                    throw MakeNodeException<RCT3Exception>(wxString::Format(_("Unknown tag '%s' in sid/flatRide tag"), subchild.wxname().c_str()), subchild);
                }
            }
        } else if (child(RAWXML_SID_SOUND)) {
            cSidSound sound;
            OPTION_PARSE(float, sound.extra_unkf[0], ParseFloat(child, wxT(RAWXML_SID_SOUND), wxT("extraU1")));
            OPTION_PARSE(float, sound.extra_unkf[1], ParseFloat(child, wxT(RAWXML_SID_SOUND), wxT("extraU2")));
            OPTION_PARSE(unsigned long, sound.extra_unk, ParseUnsigned(child, wxT(RAWXML_SID_SOUND), wxT("extraU3")));

            foreach(const cXmlNode& subchild, child.children()) {
                USE_PREFIX(subchild);
                DO_CONDITION_COMMENT_FOR(subchild);

                if (subchild(RAWXML_SID_SOUND_REF)) {
                    wxString c = HandleStringContent(subchild(), NULL, useprefix);
                    if (!c.IsEmpty())
                        sound.sounds.push_back(static_cast<const char*>(c.ToAscii()));
                } else if (subchild(RAWXML_SID_SOUND_ANIM)) {
                    vector<cSidSoundScript> commands;
                    float fps = 30.0;
                    float last_time = 0;
                    unsigned long last_command = 0;
                    OPTION_PARSE(float, fps, ParseFloat(subchild, wxT(RAWXML_SID_SOUND_ANIM), wxT("fps")));

                    foreach(const cXmlNode& grandchild, subchild.children()) {
                        USE_PREFIX(grandchild);
                        DO_CONDITION_COMMENT_FOR(grandchild);

                        if (grandchild(RAWXML_SID_SOUND_ANIM_COMMAND)) {
                            cSidSoundScript scr;
                            bool is_frame = false;
                            if (grandchild.hasProp("frame")) {
                                scr.time = (ParseFloat(grandchild, wxT(RAWXML_SID_SOUND_ANIM_COMMAND), wxT("frame"))-1.0) / fps;
                                is_frame = true;
                            } else
                                scr.time = ParseFloat(grandchild, wxT(RAWXML_SID_SOUND_ANIM_COMMAND), wxT("time"));
                            scr.command = ParseUnsigned(grandchild, wxT(RAWXML_SID_SOUND_ANIM_COMMAND), wxT("type"));
                            last_time = scr.time;
                            last_command = scr.command;
                            OPTION_PARSE(float, scr.parameter[0], ParseFloat(grandchild, wxT(RAWXML_SID_SOUND_ANIM_COMMAND), wxT("value")));
                            OPTION_PARSE(float, scr.parameter[1], ParseFloat(grandchild, wxT(RAWXML_SID_SOUND_ANIM_COMMAND), wxT("end")));
                            if (is_frame)
                                scr.parameter[1] = (scr.parameter[1]-1.0)/fps;
                            commands.push_back(scr);
                        } else if (grandchild.element()) {
                            throw MakeNodeException<RCT3Exception>(wxString::Format(_("Unknown tag '%s' in sid/sound/animation tag"), subchild.wxname().c_str()), subchild);
                        }
                    }
                    if (last_command)
                        commands.push_back(cSidSoundScript(last_time));
                    sound.animationscripts.push_back(commands);
                } else if (subchild.element()) {
                    throw MakeNodeException<RCT3Exception>(wxString::Format(_("Unknown tag '%s' in sid/sound tag"), subchild.wxname().c_str()), subchild);
                }
            }

            sid.sounds.push_back(sound);
        } else if (child(RAWXML_SVD)) {
            USE_PREFIX(child);
            svd = HandleStringContent(child(), NULL, useprefix);
            if (svd.IsEmpty())
                throw MakeNodeException<RCT3Exception>(wxString::Format(_("An svd in sid tag '%s' is empty"), name.c_str()), child);
            sid.svds.push_back(std::string(svd.ToAscii()));
        } else if (child(RAWXML_SID_PARAMETER)) {
            cSidParam param;
            param.name = ParseString(child, wxT(RAWXML_SID_GROUP), wxT("name"), NULL).ToAscii();
            ParseStringOption(param.param, child, wxT("param"), NULL, false);
            if (param.param != "") {
                if (param.param[0] != ' ')
                    param.param = " " + param.param;
            }
            sid.parameters.push_back(param);
        } else if (child.element()) {
            throw MakeNodeException<RCT3Exception>(wxString::Format(_("Unknown tag '%s' in sid tag '%s'"), child.wxname().c_str(), name.c_str()), child);
        }

        child.go_next();
    }

    ovlSIDManager* c_sid = m_ovl.GetManager<ovlSIDManager>();
    c_sid->AddSID(sid);
}
