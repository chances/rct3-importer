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

#include "ManagerRIC.h"

#define RAWXML_RIC_SEATING          "seating"
#define RAWXML_RIC_MOVING           "movingPart"
#define RAWXML_RIC_AXIS             "axis"
#define RAWXML_RIC_BOBBING          "bobbing"
#define RAWXML_RIC_ANIMATIONS       "animations"
#define RAWXML_RIC_WHEELS           "wheels"
#define RAWXML_RIC_AXELS            "axels"
#define RAWXML_RIC_SLIDING          "sliding"
#define RAWXML_RIC_SOAKED           "soaked"
#define RAWXML_RIC_WILD             "wild"


void cRawParser::ParseRIC(cXmlNode& node) {
    USE_PREFIX(node);
    cRideCar car;
    car.name = ParseString(node, RAWXML_RIC, "name", NULL, useprefix).ToAscii();
    car.internalname = ParseString(node, RAWXML_RIC, "internalName", NULL, false).ToAscii();
    car.otherinternalname = ParseString(node, RAWXML_RIC, "otherInternalName", NULL, false).ToAscii();
    OPTION_PARSE(unsigned long, car.version, ParseUnsigned(node, RAWXML_RIC, "version"));
	if (car.version) {
		car.unknowns.initSoaked();
		car.sliding.initSoaked();
	}
    car.svd_ref = ParseString(node, RAWXML_RIC, "svd", NULL, useprefix).ToAscii();
	car.inertia = ParseFloat(node, RAWXML_RIC, "inertia");
    wxLogVerbose(wxString::Format(_("Adding ric %s to %s."), wxString(car.name.c_str()).c_str(), m_output.GetFullPath().c_str()));

    foreach (const cXmlNode& child, node.children()) {
        USE_PREFIX(child);
        DO_CONDITION_COMMENT_FOR(child);

        if (child(RAWXML_RIC_SEATING)) {
			unsigned long l = HandleUnsignedContent(child.wxcontent(), child, RAWXML_RIC_SEATING);
			car.seatings.push_back(l);
        } else if (child(RAWXML_RIC_MOVING)) {
			car.moving.svd_ref = HandleStringContent(child.wxcontent(), NULL, useprefix).ToAscii();
			car.moving.inertia = ParseFloat(child, RAWXML_RIC_MOVING, "inertia");
        } else if (child(RAWXML_RIC_AXIS)) {
            OPTION_PARSE(unsigned long, car.axis.type, ParseUnsigned(child, RAWXML_RIC_AXIS, "type"));
            OPTION_PARSE(float, car.axis.stability, ParseFloat(child, RAWXML_RIC_AXIS, "stability"));
            OPTION_PARSE(float, car.axis.unk01, ParseFloat(child, RAWXML_RIC_AXIS, "u01"));
            OPTION_PARSE(float, car.axis.unk02, ParseFloat(child, RAWXML_RIC_AXIS, "u02"));
            OPTION_PARSE(float, car.axis.momentum, ParseFloat(child, RAWXML_RIC_AXIS, "momentum"));
            OPTION_PARSE(float, car.axis.unk03, ParseFloat(child, RAWXML_RIC_AXIS, "u03"));
            OPTION_PARSE(float, car.axis.unk04, ParseFloat(child, RAWXML_RIC_AXIS, "u04"));
            OPTION_PARSE(float, car.axis.maximum, ParseFloat(child, RAWXML_RIC_AXIS, "maximum"));
        } else if (child(RAWXML_RIC_BOBBING)) {
            OPTION_PARSE(unsigned long, car.bobbing.flag, ParseUnsigned(child, RAWXML_RIC_BOBBING, "flag"));
            OPTION_PARSE(float, car.bobbing.x, ParseFloat(child, RAWXML_RIC_BOBBING, "x"));
            OPTION_PARSE(float, car.bobbing.y, ParseFloat(child, RAWXML_RIC_BOBBING, "y"));
            OPTION_PARSE(float, car.bobbing.z, ParseFloat(child, RAWXML_RIC_BOBBING, "z"));
        } else if (child(RAWXML_RIC_ANIMATIONS)) {
            OPTION_PARSE(long, car.animations.start, ParseSigned(child, RAWXML_RIC_ANIMATIONS, "start"));
            OPTION_PARSE(long, car.animations.started_idle, ParseSigned(child, RAWXML_RIC_ANIMATIONS, "started"));
            OPTION_PARSE(long, car.animations.stop, ParseSigned(child, RAWXML_RIC_ANIMATIONS, "stop"));
            OPTION_PARSE(long, car.animations.stopped_idle, ParseSigned(child, RAWXML_RIC_ANIMATIONS, "stopped"));
            OPTION_PARSE(long, car.animations.belt_open, ParseSigned(child, RAWXML_RIC_ANIMATIONS, "beltOpen"));
            OPTION_PARSE(long, car.animations.belt_open_idle, ParseSigned(child, RAWXML_RIC_ANIMATIONS, "beltOpened"));
            OPTION_PARSE(long, car.animations.belt_close, ParseSigned(child, RAWXML_RIC_ANIMATIONS, "beltClose"));
            OPTION_PARSE(long, car.animations.belt_closed_idle, ParseSigned(child, RAWXML_RIC_ANIMATIONS, "beltClosed"));
            OPTION_PARSE(long, car.animations.doors_open, ParseSigned(child, RAWXML_RIC_ANIMATIONS, "doorsOpen"));
            OPTION_PARSE(long, car.animations.doors_open_idle, ParseSigned(child, RAWXML_RIC_ANIMATIONS, "doorsOpened"));
            OPTION_PARSE(long, car.animations.doors_close, ParseSigned(child, RAWXML_RIC_ANIMATIONS, "doorsClose"));
            OPTION_PARSE(long, car.animations.doors_closed_idle, ParseSigned(child, RAWXML_RIC_ANIMATIONS, "doorsClosed"));
            OPTION_PARSE(long, car.animations.row_both, ParseSigned(child, RAWXML_RIC_ANIMATIONS, "rowBoth"));
            OPTION_PARSE(long, car.animations.row_left, ParseSigned(child, RAWXML_RIC_ANIMATIONS, "rowLeft"));
            OPTION_PARSE(long, car.animations.row_right, ParseSigned(child, RAWXML_RIC_ANIMATIONS, "rowRight"));
            OPTION_PARSE(long, car.animations.canoe_station_idle, ParseSigned(child, RAWXML_RIC_ANIMATIONS, "canoeStationIdle"));
            OPTION_PARSE(long, car.animations.canoe_idle_front, ParseSigned(child, RAWXML_RIC_ANIMATIONS, "canoeFrontIdle"));
            OPTION_PARSE(long, car.animations.canoe_idle_back, ParseSigned(child, RAWXML_RIC_ANIMATIONS, "canoeBackIdle"));
            OPTION_PARSE(long, car.animations.canoe_row_idle, ParseSigned(child, RAWXML_RIC_ANIMATIONS, "canoeRowIdle"));
            OPTION_PARSE(long, car.animations.canoe_row, ParseSigned(child, RAWXML_RIC_ANIMATIONS, "canoeIdle"));
            OPTION_PARSE(long, car.animations.rudder, ParseSigned(child, RAWXML_RIC_ANIMATIONS, "rudder"));
        } else if (child(RAWXML_RIC_WHEELS)) {
            foreach(const cXmlNode& subchild, child.children()) {
                USE_PREFIX(subchild);
                DO_CONDITION_COMMENT_FOR(subchild);

                if (subchild("frontRight")) {
					car.wheels.front_right.svd = HandleStringContent(subchild.wxcontent(), NULL, useprefix).ToAscii();
					OPTION_PARSE(unsigned long, car.wheels.front_right.type, ParseUnsigned(child, "frontRight", "type"));
                } else if (subchild("frontLeft")) {
					car.wheels.front_left.svd = HandleStringContent(subchild.wxcontent(), NULL, useprefix).ToAscii();
					OPTION_PARSE(unsigned long, car.wheels.front_left.type, ParseUnsigned(child, "frontLeft", "type"));
                } else if (subchild("backRight")) {
					car.wheels.back_right.svd = HandleStringContent(subchild.wxcontent(), NULL, useprefix).ToAscii();
					OPTION_PARSE(unsigned long, car.wheels.back_right.type, ParseUnsigned(child, "backRight", "type"));
                } else if (subchild("backLeft")) {
					car.wheels.back_left.svd = HandleStringContent(subchild.wxcontent(), NULL, useprefix).ToAscii();
					OPTION_PARSE(unsigned long, car.wheels.back_left.type, ParseUnsigned(child, "backLeft", "type"));
                } else if (subchild.element()) {
                    throw MakeNodeException<RCT3Exception>(wxString::Format(_("Unknown tag '%s' in ric/wheels tag"), subchild.wxname().c_str()), subchild);
                }
			}
        } else if (child(RAWXML_RIC_AXELS)) {
            foreach(const cXmlNode& subchild, child.children()) {
                USE_PREFIX(subchild);
                DO_CONDITION_COMMENT_FOR(subchild);

                if (subchild("front")) {
					car.axels.front.svd = HandleStringContent(subchild.wxcontent(), NULL, useprefix).ToAscii();
					OPTION_PARSE(unsigned long, car.axels.front.type, ParseUnsigned(child, "front", "type"));
                } else if (subchild("rear")) {
					car.axels.rear.svd = HandleStringContent(subchild.wxcontent(), NULL, useprefix).ToAscii();
					OPTION_PARSE(unsigned long, car.axels.rear.type, ParseUnsigned(child, "rear", "type"));
                } else if (subchild.element()) {
                    throw MakeNodeException<RCT3Exception>(wxString::Format(_("Unknown tag '%s' in ric/axels tag"), subchild.wxname().c_str()), subchild);
                }
			}
        } else if (child(RAWXML_UNKNOWNS)) {
            OPTION_PARSE(unsigned long, car.unknowns.unk54, ParseUnsigned(child, RAWXML_UNKNOWNS, "u54"));
            OPTION_PARSE(float, car.unknowns.vs_unk01, ParseFloat(child, RAWXML_UNKNOWNS, "vsU01"));
            OPTION_PARSE(float, car.unknowns.vs_unk02, ParseFloat(child, RAWXML_UNKNOWNS, "vsU02"));
            OPTION_PARSE(float, car.unknowns.unk60, ParseFloat(child, RAWXML_UNKNOWNS, "u60"));
        } else if (child(RAWXML_RIC_SLIDING)) {
            OPTION_PARSE(long, car.sliding.skyslide_unk01, ParseSigned(child, RAWXML_UNKNOWNS, "skyslideU01"));
            OPTION_PARSE(float, car.sliding.skyslide_unk02, ParseFloat(child, RAWXML_UNKNOWNS, "skyslideU02"));
            OPTION_PARSE(float, car.sliding.skyslide_unk03, ParseFloat(child, RAWXML_UNKNOWNS, "skyslideU03"));
            OPTION_PARSE(float, car.sliding.skyslide_unk04, ParseFloat(child, RAWXML_UNKNOWNS, "skyslideU04"));
            OPTION_PARSE(unsigned long, car.sliding.slide_unk01, ParseUnsigned(child, RAWXML_UNKNOWNS, "slideU01"));
            OPTION_PARSE(unsigned long, car.sliding.skyslide_unk05, ParseUnsigned(child, RAWXML_UNKNOWNS, "skyslideU05"));
            OPTION_PARSE(unsigned long, car.sliding.unstable_unk01, ParseUnsigned(child, RAWXML_UNKNOWNS, "unstableU01"));
            OPTION_PARSE(float, car.sliding.slide_unk02, ParseFloat(child, RAWXML_UNKNOWNS, "slideU02"));
            OPTION_PARSE(float, car.sliding.slide_unk03, ParseFloat(child, RAWXML_UNKNOWNS, "slideU03"));
        } else if (child(RAWXML_RIC_SOAKED)) {
            OPTION_PARSE(unsigned long, car.soaked.unk62, ParseUnsigned(child, RAWXML_RIC_SOAKED, "u62"));
            OPTION_PARSE(unsigned long, car.soaked.unk63, ParseUnsigned(child, RAWXML_RIC_SOAKED, "u63"));
            OPTION_PARSE(float, car.soaked.unk64, ParseFloat(child, RAWXML_RIC_SOAKED, "u64"));
            OPTION_PARSE(float, car.soaked.supersoaker_unk01, ParseFloat(child, RAWXML_RIC_SOAKED, "supersoakerU01"));
            OPTION_PARSE(unsigned long, car.soaked.supersoaker_unk02, ParseUnsigned(child, RAWXML_RIC_SOAKED, "supersoakerU02"));
            OPTION_PARSE(unsigned long, car.soaked.unk70, ParseUnsigned(child, RAWXML_RIC_SOAKED, "u70"));
            OPTION_PARSE(unsigned long, car.soaked.unk72, ParseUnsigned(child, RAWXML_RIC_SOAKED, "u72"));
        } else if (child(RAWXML_RIC_WILD)) {
            OPTION_PARSE(unsigned long, car.wild.flip_unk, ParseUnsigned(child, RAWXML_RIC_WILD, "flipU"));
            ParseStringOption(car.wild.svd_flipped_ref, child, "svdFlipped", NULL, useprefix);
            ParseStringOption(car.wild.moving_svd_flipped_ref, child, "svdMovingFlipped", NULL, useprefix);
            OPTION_PARSE(unsigned long, car.wild.unk77, ParseUnsigned(child, RAWXML_RIC_WILD, "u77"));
            OPTION_PARSE(float, car.wild.drifting_unk, ParseFloat(child, RAWXML_RIC_WILD, "driftingU"));
            OPTION_PARSE(float, car.wild.unk79, ParseFloat(child, RAWXML_RIC_WILD, "u79"));
            ParseStringOption(car.wild.was_ref, child, "was", NULL, useprefix);
            OPTION_PARSE(float, car.wild.paddle_steamer_unk01, ParseFloat(child, RAWXML_RIC_WILD, "paddleSteamerU01"));
            OPTION_PARSE(unsigned long, car.wild.paddle_steamer_unk02, ParseUnsigned(child, RAWXML_RIC_WILD, "paddleSteamerU02"));
            OPTION_PARSE(unsigned long, car.wild.ball_coaster_unk, ParseUnsigned(child, RAWXML_RIC_WILD, "ballCoasterU"));
        } else if (child.element()) {
            throw MakeNodeException<RCT3Exception>(wxString::Format(_("Unknown tag '%s' in ric tag"), child.wxname().c_str()), child);
        }
    }

    ovlRICManager* c_ric = m_ovl.GetManager<ovlRICManager>();
    c_ric->AddItem(car);
    wxLogVerbose(wxString::Format(_("Adding ric %s to %s. -- Done"), wxString(car.name.c_str()).c_str(), m_output.GetFullPath().c_str()));
}
