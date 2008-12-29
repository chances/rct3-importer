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
#include <boost/tokenizer.hpp>
#include "pretty.h"

#include "ManagerTRR.h"

#define RAWXML_TRR_SECTION          "trackSection"
#define RAWXML_TRR_SECTION_UNIQUE   "trackSectionRemoveDuplicates"
#define RAWXML_TRR_TRAIN            "train"
#define RAWXML_TRR_PATH             "trackPath"
#define RAWXML_TRR_SPECIALS         "specials"
#define RAWXML_TRR_OPTIONS          "options"
#define RAWXML_TRR_STATION          "station"
#define RAWXML_TRR_LAUNCHED         "launched"
#define RAWXML_TRR_CHAIN            "chain"
#define RAWXML_TRR_CONSTANT         "constant"
#define RAWXML_TRR_COST             "cost"
#define RAWXML_TRR_SPLINES          "splines"
#define RAWXML_TRR_TOWER            "tower"
#define RAWXML_TRR_SOAKED           "soaked"
#define RAWXML_TRR_WILD             "wild"

bool TrackSectionSort(const cTrackedRideSection& a, const cTrackedRideSection& b) {
    return stricmp(a.name.c_str(), b.name.c_str())<0;
}

bool TrackSectionSame(const cTrackedRideSection& a, const cTrackedRideSection& b) {
    return !stricmp(a.name.c_str(), b.name.c_str());
}

void cRawParser::ParseTRR(cXmlNode& node) {
    USE_PREFIX(node);
    cTrackedRide ride;
    ride.name = ParseString(node, wxT(RAWXML_TRR), wxT("name"), NULL, useprefix).ToAscii();
    ride.attraction.name = ParseString(node, wxT(RAWXML_TRR), wxT("nametxt"), NULL, useprefix).ToAscii();
    ride.attraction.description = ParseString(node, wxT(RAWXML_TRR), wxT("description"), NULL, useprefix).ToAscii();
    wxLogVerbose(wxString::Format(_("Adding trr %s to %s."), wxString(ride.name.c_str(), wxConvLocal).c_str(), m_output.GetFullPath().c_str()));

    foreach (const cXmlNode& child, node.children()) {
        DO_CONDITION_COMMENT_FOR(child);

        if (child(RAWXML_ATTRACTION)) {
            ParseAttraction(child, ride.attraction);
        } else if (child(RAWXML_RIDE)) {
            ParseRide(child, ride.ride);
        } else if (child(RAWXML_TRR_SECTION)) {
            cTrackedRideSection section;
            unsigned long split = 1;
            string sect(child());
            boost::trim(sect);
            OPTION_PARSE(unsigned long, section.cost, ParseUnsigned(child, RAWXML_TRR_SECTION, wxT("cost")));
            OPTION_PARSE(unsigned long, split, ParseUnsigned(child, RAWXML_TRR_SECTION, wxT("split")));

            if (split) {
                boost::char_separator<char> sep(" ,");
                typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
                tokenizer tok(sect, sep);
                foreach(const string& token, tok) {
                    section.name = token;
                    ride.sections.push_back(section);
                }
            } else {
                section.name = sect;
                ride.sections.push_back(section);
            }
        } else if (child(RAWXML_TRR_SECTION_UNIQUE)) {
            sort(ride.sections.begin(), ride.sections.end(), TrackSectionSort);
            unique(ride.sections.begin(), ride.sections.end(), TrackSectionSame);
        } else if (child(RAWXML_TRR_TRAIN)) {
            ride.trains.push_back(boost::trim_copy(child()));
        } else if (child(RAWXML_TRR_PATH)) {
            ride.trackpaths.push_back(boost::trim_copy(child()));
        } else if (child(RAWXML_TRR_SPECIALS)) {
            ParseStringOption(ride.specials.cable_lift, child, wxT("cableLift"), NULL);
            OPTION_PARSE(float, ride.specials.cable_lift_unk1, ParseFloat(child, RAWXML_TRR_SPECIALS, wxT("cableLift_u1")));
            OPTION_PARSE(float, ride.specials.cable_lift_unk2, ParseFloat(child, RAWXML_TRR_SPECIALS, wxT("cableLift_u2")));
            ParseStringOption(ride.specials.lift_car, child, wxT("liftCar"), NULL);
            OPTION_PARSE(float, ride.specials.unk28, ParseFloat(child, RAWXML_TRR_SPECIALS, wxT("u28")));
            OPTION_PARSE(float, ride.specials.unk29, ParseFloat(child, RAWXML_TRR_SPECIALS, wxT("u29")));
        } else if (child(RAWXML_TRR_OPTIONS)) {
            OPTION_PARSE(unsigned long, ride.options.only_on_water, ParseUnsigned(child, RAWXML_TRR_OPTIONS, wxT("onlyOnWater")));
            OPTION_PARSE(unsigned long, ride.options.blocks_possible, ParseUnsigned(child, RAWXML_TRR_OPTIONS, wxT("blocksPossible")));
            OPTION_PARSE(unsigned long, ride.options.car_rotation, ParseUnsigned(child, RAWXML_TRR_OPTIONS, wxT("carRotation")));
            OPTION_PARSE(unsigned long, ride.options.loop_not_allowed, ParseUnsigned(child, RAWXML_TRR_OPTIONS, wxT("loopNotAllowed")));
            OPTION_PARSE(unsigned long, ride.options.deconstruct_everywhere, ParseUnsigned(child, RAWXML_TRR_OPTIONS, wxT("deconstructEverywhere")));
        } else if (child(RAWXML_TRR_STATION)) {
            ParseStringOption(ride.station.name, child, wxT("name"), NULL);
            OPTION_PARSE(unsigned long, ride.station.platform_height_over_track, ParseUnsigned(child, RAWXML_TRR_STATION, wxT("platformHeightOverTrack")));
            OPTION_PARSE(unsigned long, ride.station.start_preset, ParseUnsigned(child, RAWXML_TRR_STATION, wxT("startPreset")));
            OPTION_PARSE(unsigned long, ride.station.start_possibilities, ParseUnsigned(child, RAWXML_TRR_STATION, wxT("startPossibilities")));
            OPTION_PARSE(float, ride.station.station_roll_speed, ParseFloat(child, RAWXML_TRR_STATION, wxT("stationRollSpeed")));
            OPTION_PARSE(unsigned long, ride.station.modus_flags, ParseUnsigned(child, RAWXML_TRR_STATION, wxT("modusFlags")));
        } else if (child(RAWXML_TRR_LAUNCHED)) {
            OPTION_PARSE(float, ride.launched.preset, ParseFloat(child, RAWXML_TRR_LAUNCHED, wxT("preset")));
            OPTION_PARSE(float, ride.launched.min, ParseFloat(child, RAWXML_TRR_LAUNCHED, wxT("min")));
            OPTION_PARSE(float, ride.launched.max, ParseFloat(child, RAWXML_TRR_LAUNCHED, wxT("max")));
            OPTION_PARSE(float, ride.launched.step, ParseFloat(child, RAWXML_TRR_LAUNCHED, wxT("step")));
        } else if (child(RAWXML_TRR_CHAIN)) {
            OPTION_PARSE(float, ride.chain.preset, ParseFloat(child, RAWXML_TRR_CHAIN, wxT("preset")));
            OPTION_PARSE(float, ride.chain.min, ParseFloat(child, RAWXML_TRR_CHAIN, wxT("min")));
            OPTION_PARSE(float, ride.chain.max, ParseFloat(child, RAWXML_TRR_CHAIN, wxT("max")));
            OPTION_PARSE(float, ride.chain.step, ParseFloat(child, RAWXML_TRR_CHAIN, wxT("step")));
            OPTION_PARSE(float, ride.chain.lock, ParseFloat(child, RAWXML_TRR_CHAIN, wxT("lock")));
        } else if (child(RAWXML_TRR_CONSTANT)) {
            OPTION_PARSE(float, ride.constant.preset, ParseFloat(child, RAWXML_TRR_CONSTANT, wxT("preset")));
            OPTION_PARSE(float, ride.constant.min, ParseFloat(child, RAWXML_TRR_CONSTANT, wxT("min")));
            OPTION_PARSE(float, ride.constant.max, ParseFloat(child, RAWXML_TRR_CONSTANT, wxT("max")));
            OPTION_PARSE(float, ride.constant.step, ParseFloat(child, RAWXML_TRR_CONSTANT, wxT("step")));
            OPTION_PARSE(float, ride.constant.up_down_variation, ParseFloat(child, RAWXML_TRR_CONSTANT, wxT("upDownVariation")));
        } else if (child(RAWXML_TRR_COST)) {
            OPTION_PARSE(unsigned long, ride.cost.upkeep_per_train, ParseUnsigned(child, RAWXML_TRR_COST, wxT("upkeepPerTrain")));
            OPTION_PARSE(unsigned long, ride.cost.upkeep_per_car, ParseUnsigned(child, RAWXML_TRR_COST, wxT("upkeepPerCar")));
            OPTION_PARSE(unsigned long, ride.cost.upkeep_per_station, ParseUnsigned(child, RAWXML_TRR_COST, wxT("upkeepPerStation")));
            OPTION_PARSE(unsigned long, ride.cost.ride_cost_preview1, ParseUnsigned(child, RAWXML_TRR_COST, wxT("costPreview1")));
            OPTION_PARSE(unsigned long, ride.cost.ride_cost_preview2, ParseUnsigned(child, RAWXML_TRR_COST, wxT("costPreview2")));
            OPTION_PARSE(unsigned long, ride.cost.ride_cost_preview3, ParseUnsigned(child, RAWXML_TRR_COST, wxT("costPreview3")));
            OPTION_PARSE(unsigned long, ride.cost.cost_per_4h_height, ParseUnsigned(child, RAWXML_TRR_COST, wxT("costPer4h")));
        } else if (child(RAWXML_TRR_SPLINES)) {
            OPTION_PARSE(unsigned long, ride.splines.auto_complete, ParseUnsigned(child, RAWXML_TRR_SPLINES, wxT("autoComplete")));
            ParseStringOption(ride.splines.track, child, wxT("track"), NULL);
            ParseStringOption(ride.splines.track_big, child, wxT("trackBig"), NULL);
            ParseStringOption(ride.splines.car, child, wxT("car"), NULL);
            ParseStringOption(ride.splines.car_swing, child, wxT("carSwing"), NULL);
            OPTION_PARSE(float, ride.splines.free_space_profile_height, ParseFloat(child, RAWXML_TRR_SPLINES, wxT("profileHight")));
        } else if (child(RAWXML_TRR_TOWER)) {
            OPTION_PARSE(float, ride.tower.top_duration, ParseFloat(child, RAWXML_TRR_TOWER, wxT("topDuration")));
            OPTION_PARSE(float, ride.tower.top_distance, ParseFloat(child, RAWXML_TRR_TOWER, wxT("topDistance")));
            ParseStringOption(ride.tower.top, child, wxT("top"), NULL);
            ParseStringOption(ride.tower.mid, child, wxT("mid"), NULL);
            ParseStringOption(ride.tower.other_top, child, wxT("otherTop"), NULL);
            ParseStringOption(ride.tower.other_mid, child, wxT("otherMid"), NULL);
            ParseStringOption(ride.tower.other_top_flipped, child, wxT("otherTopFlipped"), NULL);
            ParseStringOption(ride.tower.other_mid_flipped, child, wxT("otherMidFlipped"), NULL);
        } else if (child(RAWXML_UNKNOWNS)) {
            OPTION_PARSE(float, ride.unknowns.unk31, ParseFloat(child, RAWXML_TRR "/" RAWXML_UNKNOWNS, wxT("u31")));
            OPTION_PARSE(float, ride.unknowns.unk32, ParseFloat(child, RAWXML_TRR "/" RAWXML_UNKNOWNS, wxT("u32")));
            OPTION_PARSE(float, ride.unknowns.unk33, ParseFloat(child, RAWXML_TRR "/" RAWXML_UNKNOWNS, wxT("u33")));
            OPTION_PARSE(float, ride.unknowns.unk34, ParseFloat(child, RAWXML_TRR "/" RAWXML_UNKNOWNS, wxT("u34")));
            OPTION_PARSE(unsigned long, ride.unknowns.unk38, ParseUnsigned(child, RAWXML_TRR "/" RAWXML_UNKNOWNS, wxT("u38")));
            OPTION_PARSE(float, ride.unknowns.unk43, ParseFloat(child, RAWXML_TRR "/" RAWXML_UNKNOWNS, wxT("u43")));
            OPTION_PARSE(float, ride.unknowns.unk45, ParseFloat(child, RAWXML_TRR "/" RAWXML_UNKNOWNS, wxT("u45")));
            OPTION_PARSE(float, ride.unknowns.unk48, ParseFloat(child, RAWXML_TRR "/" RAWXML_UNKNOWNS, wxT("u48")));
            OPTION_PARSE(float, ride.unknowns.unk49, ParseFloat(child, RAWXML_TRR "/" RAWXML_UNKNOWNS, wxT("u49")));
            OPTION_PARSE(float, ride.unknowns.unk50, ParseFloat(child, RAWXML_TRR "/" RAWXML_UNKNOWNS, wxT("u50")));
            OPTION_PARSE(float, ride.unknowns.unk51, ParseFloat(child, RAWXML_TRR "/" RAWXML_UNKNOWNS, wxT("u51")));
            OPTION_PARSE(unsigned long, ride.unknowns.unk57, ParseUnsigned(child, RAWXML_TRR "/" RAWXML_UNKNOWNS, wxT("u57")));
            OPTION_PARSE(unsigned long, ride.unknowns.unk58, ParseUnsigned(child, RAWXML_TRR "/" RAWXML_UNKNOWNS, wxT("u58")));
            OPTION_PARSE(unsigned long, ride.unknowns.unk59, ParseUnsigned(child, RAWXML_TRR "/" RAWXML_UNKNOWNS, wxT("u59")));
            OPTION_PARSE(unsigned long, ride.unknowns.unk60, ParseUnsigned(child, RAWXML_TRR "/" RAWXML_UNKNOWNS, wxT("u60")));
            OPTION_PARSE(unsigned long, ride.unknowns.unk61, ParseUnsigned(child, RAWXML_TRR "/" RAWXML_UNKNOWNS, wxT("u61")));
            OPTION_PARSE(unsigned long, ride.unknowns.unk69, ParseUnsigned(child, RAWXML_TRR "/" RAWXML_UNKNOWNS, wxT("u69")));
            OPTION_PARSE(float, ride.unknowns.unk95, ParseFloat(child, RAWXML_TRR "/" RAWXML_UNKNOWNS, wxT("u95")));
        } else if (child(RAWXML_TRR_SOAKED)) {
            OPTION_PARSE(unsigned long, ride.soaked.unk80, ParseUnsigned(child, RAWXML_TRR_SOAKED, wxT("u80")));
            OPTION_PARSE(unsigned long, ride.soaked.unk81, ParseUnsigned(child, RAWXML_TRR_SOAKED, wxT("u81")));
            OPTION_PARSE(float, ride.soaked.unk82, ParseFloat(child, RAWXML_TRR_SOAKED, wxT("u82")));
            OPTION_PARSE(unsigned long, ride.soaked.unk85, ParseUnsigned(child, RAWXML_TRR_SOAKED, wxT("u85")));
            OPTION_PARSE(unsigned long, ride.soaked.unk86, ParseUnsigned(child, RAWXML_TRR_SOAKED, wxT("u86")));
            OPTION_PARSE(unsigned long, ride.soaked.unk87, ParseUnsigned(child, RAWXML_TRR_SOAKED, wxT("u87")));
            OPTION_PARSE(long, ride.soaked.unk88, ParseSigned(child, RAWXML_TRR_SOAKED, wxT("u88")));
            OPTION_PARSE(float, ride.soaked.unk89, ParseFloat(child, RAWXML_TRR_SOAKED, wxT("u89")));
            OPTION_PARSE(float, ride.soaked.unk90, ParseFloat(child, RAWXML_TRR_SOAKED, wxT("u90")));
            OPTION_PARSE(float, ride.soaked.unk93, ParseFloat(child, RAWXML_TRR_SOAKED, wxT("u93")));
            OPTION_PARSE(float, ride.soaked.unk94, ParseFloat(child, RAWXML_TRR_SOAKED, wxT("u94")));
            OPTION_PARSE(unsigned long, ride.soaked.unk95, ParseUnsigned(child, RAWXML_TRR_SOAKED, wxT("u95")));
            OPTION_PARSE(long, ride.soaked.unk96, ParseSigned(child, RAWXML_TRR_SOAKED, wxT("u96")));
            OPTION_PARSE(unsigned long, ride.soaked.unk97, ParseUnsigned(child, RAWXML_TRR_SOAKED, wxT("u97")));
            OPTION_PARSE(unsigned long, ride.soaked.short_struct, ParseUnsigned(child, RAWXML_TRR_SOAKED, wxT("shortStruct")));
            OPTION_PARSE(float, ride.soaked.unk99, ParseFloat(child, RAWXML_TRR_SOAKED, wxT("u99")));
            OPTION_PARSE(float, ride.soaked.unk100, ParseFloat(child, RAWXML_TRR_SOAKED, wxT("u100")));
            OPTION_PARSE(float, ride.soaked.unk101, ParseFloat(child, RAWXML_TRR_SOAKED, wxT("u101")));
            OPTION_PARSE(float, ride.soaked.unk102, ParseFloat(child, RAWXML_TRR_SOAKED, wxT("u102")));
            OPTION_PARSE(float, ride.soaked.unk103, ParseFloat(child, RAWXML_TRR_SOAKED, wxT("u103")));
        } else if (child(RAWXML_TRR_WILD)) {
            ParseStringOption(ride.wild.splitter, child, wxT("splitter"), NULL);
            OPTION_PARSE(unsigned long, ride.wild.robo_flag, ParseUnsigned(child, RAWXML_TRR_WILD, wxT("boneControl")));
            OPTION_PARSE(unsigned long, ride.wild.spinner_control, ParseUnsigned(child, RAWXML_TRR_WILD, wxT("spinnerControl")));
            OPTION_PARSE(unsigned long, ride.wild.unk107, ParseUnsigned(child, RAWXML_TRR_WILD, wxT("u107")));
            OPTION_PARSE(unsigned long, ride.wild.unk107, ParseUnsigned(child, RAWXML_TRR_WILD, wxT("equivalentInversion")));
            OPTION_PARSE(unsigned long, ride.wild.unk108, ParseUnsigned(child, RAWXML_TRR_WILD, wxT("u108")));
            OPTION_PARSE(unsigned long, ride.wild.split_flag, ParseUnsigned(child, RAWXML_TRR_WILD, wxT("splitFlag")));
            ParseStringOption(ride.wild.internalname, child, wxT("name"), NULL);
        } else if (child.element()) {
            throw MakeNodeException<RCT3Exception>(wxString::Format(_("Unknown tag '%s' in trr tag"), child.wxname().c_str()), child);
        }
    }

    ovlTRRManager* c_trr = m_ovl.GetManager<ovlTRRManager>();
    c_trr->AddRide(ride);
    wxLogVerbose(wxString::Format(_("Adding trr %s to %s. -- Done"), wxString(ride.name.c_str(), wxConvLocal).c_str(), m_output.GetFullPath().c_str()));
}
