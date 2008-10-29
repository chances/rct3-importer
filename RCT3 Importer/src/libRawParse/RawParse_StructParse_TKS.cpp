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

#include <boost/algorithm/string.hpp>
//#include <boost/tokenizer.hpp>
#include "pretty.h"

#include "ManagerTKS.h"

#define RAWXML_TKS_SPEED            "speed"
#define RAWXML_TKS_BASIC            "basic"
#define RAWXML_TKS_BASIC_ENTRY      "entry"
#define RAWXML_TKS_BASIC_EXIT       "exit"
#define RAWXML_TKS_SPEED            "speed"
#define RAWXML_TKS_SPLINES          "splines"
#define RAWXML_TKS_SPLINES_CAR      "car"
#define RAWXML_TKS_SPLINES_JOIN     "join"
#define RAWXML_TKS_SPLINES_EXTRA    "extra"
#define RAWXML_TKS_SPLINES_WATER    "water"
#define RAWXML_TKS_ANIMATIONS       "animations"
#define RAWXML_TKS_OPTIONS          "options"
//#define RAWXML_UNKNOWNS "unknowns"
#define RAWXML_TKS_SOAKED           "soaked"
#define RAWXML_TKS_SOAKED_PATH      "path"
//#define RAWXML_RIDE_STATION_LIMIT   "rideStationLimit"
#define RAWXML_TKS_SOAKED_SPEED     "speedSplines"
//#define RAWXML_TKS_SPLINES          "splines"
#define RAWXML_TKS_WILD             "wild"


#define ParseEntryExit(node, strct, tag) \
    OPTION_PARSE(unsigned long, strct.curve, ParseUnsigned(node, tag, wxT("curve"))); \
    OPTION_PARSE(unsigned long, strct.flags, ParseUnsigned(node, tag, wxT("flags"))); \
    OPTION_PARSE(unsigned long, strct.unknown, ParseUnsigned(node, tag, wxT("unknown"))); \
    OPTION_PARSE(unsigned long, strct.slope, ParseUnsigned(node, tag, wxT("slope"))); \
    OPTION_PARSE(unsigned long, strct.bank, ParseUnsigned(node, tag, wxT("bank"))); \
    ParseStringOption(strct.group, child, wxT("group"), NULL); \

#define ParseSplinePair(node, strct, tag) \
    strct.left = ParseString(node, tag, wxT("left"), NULL, useprefix).ToAscii(); \
    strct.right = ParseString(node, tag, wxT("right"), NULL, useprefix).ToAscii();

void cRawParser::ParseTKS(cXmlNode& node) {
    USE_PREFIX(node);
    cTrackSection section;
    section.name = ParseString(node, wxT(RAWXML_TKS), wxT("name"), NULL, useprefix).ToAscii();
    section.internalname = ParseString(node, wxT(RAWXML_TKS), wxT("internalname"), NULL, false).ToAscii();
    OPTION_PARSE(unsigned long, section.version, ParseUnsigned(node, RAWXML_TKS, wxT("version")));
    section.sid = ParseString(node, wxT(RAWXML_TKS), wxT("sid"), NULL, useprefix).ToAscii();
    wxLogVerbose(wxString::Format(_("Adding tks %s to %s."), wxString(section.name.c_str()).c_str(), m_output.GetFullPath().c_str()));

    foreach (const cXmlNode& child, node.children()) {
        USE_PREFIX(child);
        DO_CONDITION_COMMENT_FOR(child);

        if (child(RAWXML_TKS_SPEED)) {
            cTrackSectionSpeed speed;
            speed.unk01 = ParseFloat(child, RAWXML_TKS_SPEED, wxT("u1"));
            speed.unk02 = ParseFloat(child, RAWXML_TKS_SPEED, wxT("u2"));
            speed.unk03 = ParseFloat(child, RAWXML_TKS_SPEED, wxT("u3"));
            OPTION_PARSE(float, speed.unk04, ParseFloat(child, RAWXML_TKS_SPEED, wxT("u4")));
            OPTION_PARSE(float, speed.unk05, ParseFloat(child, RAWXML_TKS_SPEED, wxT("u5")));
            section.speeds.push_back(speed);
        } else if (child(RAWXML_TKS_BASIC)) {
            OPTION_PARSE(unsigned long, section.basic.special, ParseUnsigned(child, RAWXML_TKS_BASIC, wxT("special")));
            OPTION_PARSE(unsigned long, section.basic.direction, ParseUnsigned(child, RAWXML_TKS_BASIC, wxT("direction")));

            foreach(const cXmlNode& subchild, child.children()) {
                USE_PREFIX(subchild);
                DO_CONDITION_COMMENT_FOR(subchild);

                if (subchild(RAWXML_TKS_BASIC_ENTRY)) {
                    ParseEntryExit(subchild, section.basic.entry, RAWXML_TKS_BASIC_ENTRY);
                } else if (subchild(RAWXML_TKS_BASIC_EXIT)) {
                    ParseEntryExit(subchild, section.basic.exit, RAWXML_TKS_BASIC_EXIT);
                } else if (subchild.element()) {
                    throw MakeNodeException<RCT3Exception>(wxString::Format(_("Unknown tag '%s' in tks/basic tag"), subchild.wxname().c_str()), subchild);
                }
            }
        } else if (child(RAWXML_TKS_SPLINES)) {
            foreach(const cXmlNode& subchild, child.children()) {
                USE_PREFIX(subchild);
                DO_CONDITION_COMMENT_FOR(subchild);

                if (subchild(RAWXML_TKS_SPLINES_CAR)) {
                    ParseSplinePair(subchild, section.splines.car, RAWXML_TKS_SPLINES_CAR);
                } else if (subchild(RAWXML_TKS_SPLINES_JOIN)) {
                    ParseSplinePair(subchild, section.splines.join, RAWXML_TKS_SPLINES_JOIN);
                } else if (subchild(RAWXML_TKS_SPLINES_EXTRA)) {
                    ParseSplinePair(subchild, section.splines.extra, RAWXML_TKS_SPLINES_EXTRA);
                } else if (subchild(RAWXML_TKS_SPLINES_WATER)) {
                    ParseSplinePair(subchild, section.splines.water, RAWXML_TKS_SPLINES_WATER);
                } else if (subchild.element()) {
                    throw MakeNodeException<RCT3Exception>(wxString::Format(_("Unknown tag '%s' in tks/splines tag"), subchild.wxname().c_str()), subchild);
                }
            }
        } else if (child(RAWXML_TKS_ANIMATIONS)) {
            OPTION_PARSE(long, section.animations.stopped, ParseSigned(child, RAWXML_TKS_ANIMATIONS, wxT("stopped")));
            OPTION_PARSE(long, section.animations.starting, ParseSigned(child, RAWXML_TKS_ANIMATIONS, wxT("starting")));
            OPTION_PARSE(long, section.animations.running, ParseSigned(child, RAWXML_TKS_ANIMATIONS, wxT("running")));
            OPTION_PARSE(long, section.animations.stopping, ParseSigned(child, RAWXML_TKS_ANIMATIONS, wxT("stopping")));
            OPTION_PARSE(long, section.animations.hold_after_train_stop, ParseSigned(child, RAWXML_TKS_ANIMATIONS, wxT("holdAfterTrainStop")));
            OPTION_PARSE(long, section.animations.hold_loop, ParseSigned(child, RAWXML_TKS_ANIMATIONS, wxT("holdLoop")));
            OPTION_PARSE(long, section.animations.hold_before_release, ParseSigned(child, RAWXML_TKS_ANIMATIONS, wxT("holdBeforeRelease")));
            OPTION_PARSE(long, section.animations.hold_leaving, ParseSigned(child, RAWXML_TKS_ANIMATIONS, wxT("holdLeaving")));
            OPTION_PARSE(long, section.animations.hold_after_train_left, ParseSigned(child, RAWXML_TKS_ANIMATIONS, wxT("holdAfterTrainLeft")));
            OPTION_PARSE(long, section.animations.pre_station_leave, ParseSigned(child, RAWXML_TKS_ANIMATIONS, wxT("preStationLeave")));
            OPTION_PARSE(long, section.animations.rotating_tower_idle, ParseSigned(child, RAWXML_TKS_ANIMATIONS, wxT("rotatingTowerIdle")));
        } else if (child(RAWXML_UNKNOWNS)) {
            OPTION_PARSE(unsigned long, section.unknowns.unk15, ParseUnsigned(child, RAWXML_UNKNOWNS, wxT("u15")));
            OPTION_PARSE(unsigned long, section.unknowns.unk16, ParseUnsigned(child, RAWXML_UNKNOWNS, wxT("u16")));
            OPTION_PARSE(unsigned long, section.unknowns.unk17, ParseUnsigned(child, RAWXML_UNKNOWNS, wxT("u17")));
            OPTION_PARSE(unsigned long, section.unknowns.unk22, ParseUnsigned(child, RAWXML_UNKNOWNS, wxT("u22")));
            OPTION_PARSE(unsigned long, section.unknowns.unk23, ParseUnsigned(child, RAWXML_UNKNOWNS, wxT("u23")));
            OPTION_PARSE(unsigned long, section.unknowns.unk24, ParseUnsigned(child, RAWXML_UNKNOWNS, wxT("u24")));
            OPTION_PARSE(unsigned long, section.unknowns.unk45, ParseUnsigned(child, RAWXML_UNKNOWNS, wxT("u45")));
            OPTION_PARSE(unsigned long, section.unknowns.unk47, ParseUnsigned(child, RAWXML_UNKNOWNS, wxT("u47")));
            OPTION_PARSE(unsigned long, section.unknowns.unk53, ParseUnsigned(child, RAWXML_UNKNOWNS, wxT("u53")));
            OPTION_PARSE(unsigned long, section.unknowns.unk68, ParseUnsigned(child, RAWXML_UNKNOWNS, wxT("u68")));
            OPTION_PARSE(unsigned long, section.unknowns.unk69, ParseUnsigned(child, RAWXML_UNKNOWNS, wxT("u69")));
            OPTION_PARSE(float, section.unknowns.unk77, ParseFloat(child, RAWXML_UNKNOWNS, wxT("u77")));
            OPTION_PARSE(float, section.unknowns.unk78, ParseFloat(child, RAWXML_UNKNOWNS, wxT("u78")));
            OPTION_PARSE(float, section.unknowns.unk79, ParseFloat(child, RAWXML_UNKNOWNS, wxT("u79")));
        } else if (child(RAWXML_TKS_OPTIONS)) {
            OPTION_PARSE(unsigned long, section.options.tower_ride_base_flag, ParseUnsigned(child, RAWXML_TKS_OPTIONS, wxT("towerRideBase")));
            OPTION_PARSE(float, section.options.tower_unkf01, ParseFloat(child, RAWXML_TKS_OPTIONS, wxT("tower")));
            OPTION_PARSE(float, section.options.water_splash01, ParseFloat(child, RAWXML_TKS_OPTIONS, wxT("waterSplash01")));
            OPTION_PARSE(float, section.options.water_splash02, ParseFloat(child, RAWXML_TKS_OPTIONS, wxT("waterSplash02")));
            OPTION_PARSE(float, section.options.reverser_val, ParseFloat(child, RAWXML_TKS_OPTIONS, wxT("reverser")));
            OPTION_PARSE(float, section.options.elevator_top_val, ParseFloat(child, RAWXML_TKS_OPTIONS, wxT("elevatorTop")));
            OPTION_PARSE(float, section.options.rapids01, ParseFloat(child, RAWXML_TKS_OPTIONS, wxT("rapids01")));
            OPTION_PARSE(float, section.options.rapids02, ParseFloat(child, RAWXML_TKS_OPTIONS, wxT("rapids02")));
            OPTION_PARSE(float, section.options.rapids03, ParseFloat(child, RAWXML_TKS_OPTIONS, wxT("rapids03")));
            OPTION_PARSE(float, section.options.whirlpool01, ParseFloat(child, RAWXML_TKS_OPTIONS, wxT("whirlpool01")));
            OPTION_PARSE(float, section.options.whirlpool02, ParseFloat(child, RAWXML_TKS_OPTIONS, wxT("whirlpool02")));
            ParseStringOption(section.options.chair_lift_station_end, child, wxT("chairLiftStationEnd"), NULL);
        } else if (child(RAWXML_TKS_SOAKED)) {
            ParseStringOption(section.soaked.loop_path, child, wxT("loopSpline"), NULL, useprefix);
            OPTION_PARSE(float, section.soaked.tower_unkf2, ParseFloat(child, RAWXML_TKS_SOAKED, wxT("tower")));
            OPTION_PARSE(float, section.soaked.aquarium_val, ParseFloat(child, RAWXML_TKS_SOAKED, wxT("aquarium")));
            ParseStringOption(section.soaked.auto_group, child, wxT("autoGroup"), NULL);
            OPTION_PARSE(long, section.soaked.giant_flume_val, ParseSigned(child, RAWXML_TKS_SOAKED, wxT("giantFlume")));
            OPTION_PARSE(unsigned long, section.soaked.entry_wide_flag, ParseUnsigned(child, RAWXML_TKS_SOAKED, wxT("entryWide")));
            OPTION_PARSE(unsigned long, section.soaked.exit_wide_flag, ParseUnsigned(child, RAWXML_TKS_SOAKED, wxT("exitWide")));
            OPTION_PARSE(float, section.soaked.speed_spline_val, ParseFloat(child, RAWXML_TKS_SOAKED, wxT("speedSpline")));
            OPTION_PARSE(float, section.soaked.slide_end_to_lifthill_val, ParseFloat(child, RAWXML_TKS_SOAKED, wxT("slideEndToLifthill")));
            OPTION_PARSE(unsigned long, section.soaked.soaked_options, ParseUnsigned(child, RAWXML_TKS_SOAKED, wxT("flags")));

            foreach(const cXmlNode& subchild, child.children()) {
                USE_PREFIX(subchild);
                DO_CONDITION_COMMENT_FOR(subchild);

                if (subchild(RAWXML_TKS_SOAKED_PATH)) {
                    string path;
                    ParseStringOption(path, subchild, wxT("spline"), NULL, useprefix);
                    section.soaked.paths.push_back(path);
                } else if (subchild(RAWXML_RIDE_STATION_LIMIT)) {
                    cRideStationLimit limit;
                    ParseRideStationLimit(subchild, limit);
                    section.soaked.ride_station_limits.push_back(limit);
                } else if (subchild(RAWXML_TKS_SOAKED_SPEED)) {
                    cTrackSectionSpeedSplines speeds;
                    speeds.speed = ParseFloat(subchild, RAWXML_TKS_SOAKED_SPEED, wxT("speed"));

                    foreach(const cXmlNode& speedchild, child.children()) {
                        USE_PREFIX(speedchild);
                        DO_CONDITION_COMMENT_FOR(speedchild);

                        if (speedchild(RAWXML_TKS_SPLINES)) {
                            ParseSplinePair(speedchild, speeds.splines, RAWXML_TKS_SPLINES);
                        } else if (speedchild.element()) {
                            throw MakeNodeException<RCT3Exception>(wxString::Format(_("Unknown tag '%s' in tks/soked/speedSplines tag"), speedchild.wxname().c_str()), speedchild);
                        }
                    }
                    section.soaked.speeds.push_back(speeds);
                } else if (subchild("groupIsAtEntry")) {
                    string gr(subchild());
                    boost::trim(gr);
                    section.soaked.groups_is_at_entry.push_back(gr);
                } else if (subchild("groupIsAtExit")) {
                    string gr(subchild());
                    boost::trim(gr);
                    section.soaked.groups_is_at_exit.push_back(gr);
                } else if (subchild("groupRequireAtEntry")) {
                    string gr(subchild());
                    boost::trim(gr);
                    section.soaked.groups_must_have_at_entry.push_back(gr);
                } else if (subchild("groupRequireAtExit")) {
                    string gr(subchild());
                    boost::trim(gr);
                    section.soaked.groups_must_have_at_exit.push_back(gr);
                } else if (subchild("groupRefuseAtEntry")) {
                    string gr(subchild());
                    boost::trim(gr);
                    section.soaked.groups_must_not_be_at_entry.push_back(gr);
                } else if (subchild("groupRefuseAtExit")) {
                    string gr(subchild());
                    boost::trim(gr);
                    section.soaked.groups_must_not_be_at_exit.push_back(gr);
                } else if (subchild.element()) {
                    throw MakeNodeException<RCT3Exception>(wxString::Format(_("Unknown tag '%s' in tks/soaked tag"), subchild.wxname().c_str()), subchild);
                }
            }

        } else if (child(RAWXML_TKS_WILD)) {
            OPTION_PARSE(unsigned long, section.wild.splitter_half, ParseUnsigned(child, RAWXML_TKS_WILD, wxT("splitterHalf")));
            ParseStringOption(section.wild.splitter_joined_other_ref, child, wxT("splitterJoint"), NULL, useprefix);
            OPTION_PARSE(unsigned long, section.wild.rotator_type, ParseUnsigned(child, RAWXML_TKS_WILD, wxT("rotatorType")));
            OPTION_PARSE(float, section.wild.animal_house, ParseFloat(child, RAWXML_TKS_WILD, wxT("animalHouse")));
            ParseStringOption(section.wild.alternate_text_lookup, child, wxT("alternateTextLookup"), NULL);
            OPTION_PARSE(float, section.wild.tower_cap01, ParseFloat(child, RAWXML_TKS_WILD, wxT("towerCap01")));
            OPTION_PARSE(float, section.wild.tower_cap02, ParseFloat(child, RAWXML_TKS_WILD, wxT("towerCap02")));
        } else if (child.element()) {
            throw MakeNodeException<RCT3Exception>(wxString::Format(_("Unknown tag '%s' in tks tag"), child.wxname().c_str()), child);
        }
    }

    ovlTKSManager* c_tks = m_ovl.GetManager<ovlTKSManager>();
    c_tks->AddSection(section);
    wxLogVerbose(wxString::Format(_("Adding tks %s to %s. -- Done"), wxString(section.name.c_str()).c_str(), m_output.GetFullPath().c_str()));
}
