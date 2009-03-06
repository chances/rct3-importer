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

#include "ManagerRIT.h"

#define RAWXML_RIT_CARS          	"cars"
#define RAWXML_RIT_SPEED            "speed"
#define RAWXML_RIT_CAMERA           "camera"
#define RAWXML_RIT_WATER          	"water"
#define RAWXML_RIT_LIFT       		"liftSplines"
#define RAWXML_RIT_SOAKED           "soaked"
#define RAWXML_RIT_WILD             "wild"


void cRawParser::ParseRIT(cXmlNode& node) {
    USE_PREFIX(node);
    cRideTrain train;
    train.name = ParseString(node, RAWXML_RIT, "name", NULL, useprefix).ToAscii();
    train.internalname = ParseString(node, RAWXML_RIT, "internalName", NULL, false).ToAscii();
    train.internaldescription = ParseString(node, RAWXML_RIT, "internalDescription", NULL, false).ToAscii();
    OPTION_PARSE(unsigned long, train.version, ParseUnsigned(node, RAWXML_RIC, "version"));
    OPTION_PARSE(unsigned long, train.overtake, ParseUnsigned(node, RAWXML_RIC, "overtake"));
	ParseStringOption(train.station, node, "station", NULL, false);
    wxLogVerbose(wxString::Format(_("Adding rit %s to %s."), wxString(train.name.c_str()).c_str(), m_output.GetFullPath().c_str()));

    foreach (const cXmlNode& child, node.children()) {
        USE_PREFIX(child);
        DO_CONDITION_COMMENT_FOR(child);

        if (child(RAWXML_RIT_CARS)) {
			train.cars.front = ParseString(child, RAWXML_RIT_CARS, "front", NULL, useprefix).ToAscii();
            ParseStringOption(train.cars.second, child, "second", NULL, useprefix);
            ParseStringOption(train.cars.mid, child, "mid", NULL, useprefix);
            ParseStringOption(train.cars.penultimate, child, "penultimate", NULL, useprefix);
            ParseStringOption(train.cars.rear, child, "rear", NULL, useprefix);
            ParseStringOption(train.cars.link, child, "link", NULL, useprefix);
            ParseStringOption(train.cars.unknown, child, "unknownCar", NULL, useprefix);
            OPTION_PARSE(unsigned long, train.cars.count_min, ParseUnsigned(child, RAWXML_RIT_CARS, "countMin"));
            OPTION_PARSE(unsigned long, train.cars.count_max, ParseUnsigned(child, RAWXML_RIT_CARS, "countMax"));
            OPTION_PARSE(unsigned long, train.cars.count_def, ParseUnsigned(child, RAWXML_RIT_CARS, "countDefault"));
        } else if (child(RAWXML_RIT_SPEED)) {
            OPTION_PARSE(float, train.speed.unk01, ParseFloat(child, RAWXML_RIT_SPEED, "u01"));
            OPTION_PARSE(float, train.speed.unk02, ParseFloat(child, RAWXML_RIT_SPEED, "u02"));
            OPTION_PARSE(float, train.speed.unk03, ParseFloat(child, RAWXML_RIT_SPEED, "u03"));
        } else if (child(RAWXML_RIT_CAMERA)) {
            OPTION_PARSE(float, train.camera.lookahead, ParseFloat(child, RAWXML_RIT_CAMERA, "lookAhead"));
            OPTION_PARSE(float, train.camera.g_displacement_mult, ParseFloat(child, RAWXML_RIT_CAMERA, "gDispMult"));
            OPTION_PARSE(float, train.camera.g_displacement_max, ParseFloat(child, RAWXML_RIT_CAMERA, "gDispMax"));
            OPTION_PARSE(float, train.camera.g_displacement_smoothing, ParseFloat(child, RAWXML_RIT_CAMERA, "gDispSmooth"));
            OPTION_PARSE(float, train.camera.shake_min_speed, ParseFloat(child, RAWXML_RIT_CAMERA, "shakeMinSpeed"));
            OPTION_PARSE(float, train.camera.shake_factor, ParseFloat(child, RAWXML_RIT_CAMERA, "shakeFactor"));
            OPTION_PARSE(float, train.camera.shake_h_max, ParseFloat(child, RAWXML_RIT_CAMERA, "shakeHMax"));
            OPTION_PARSE(float, train.camera.shake_v_max, ParseFloat(child, RAWXML_RIT_CAMERA, "shakeVMax"));
            OPTION_PARSE(float, train.camera.chain_shake_factor, ParseFloat(child, RAWXML_RIT_CAMERA, "chainShakeFactor"));
            OPTION_PARSE(float, train.camera.chain_shake_h_max, ParseFloat(child, RAWXML_RIT_CAMERA, "chainShakeHMax"));
            OPTION_PARSE(float, train.camera.chain_shake_v_max, ParseFloat(child, RAWXML_RIT_CAMERA, "chainShakeVMax"));
            OPTION_PARSE(float, train.camera.smoothing, ParseFloat(child, RAWXML_RIT_CAMERA, "smoothing"));
            OPTION_PARSE(float, train.camera.la_tilt_factor, ParseFloat(child, RAWXML_RIT_CAMERA, "tiltFactor"));
        } else if (child(RAWXML_RIT_WATER)) {
            OPTION_PARSE(float, train.water.unk01, ParseFloat(child, RAWXML_RIT_WATER, "u01"));
            OPTION_PARSE(float, train.water.unk02, ParseFloat(child, RAWXML_RIT_WATER, "u02"));
            OPTION_PARSE(float, train.water.unk03, ParseFloat(child, RAWXML_RIT_WATER, "u03"));
            OPTION_PARSE(float, train.water.unk04, ParseFloat(child, RAWXML_RIT_WATER, "u04"));
            OPTION_PARSE(float, train.water.unk05, ParseFloat(child, RAWXML_RIT_WATER, "u05"));
            OPTION_PARSE(float, train.water.unk06, ParseFloat(child, RAWXML_RIT_WATER, "u06"));
            OPTION_PARSE(float, train.water.free_roam_curve_angle, ParseFloat(child, RAWXML_RIT_WATER, "curveAngle"));
            OPTION_PARSE(float, train.water.unk07, ParseFloat(child, RAWXML_RIT_WATER, "u07"));
        } else if (child(RAWXML_RIT_LIFT)) {
            ParseStringOption(train.liftsplines.left, child, "left", NULL, useprefix);
            ParseStringOption(train.liftsplines.right, child, "right", NULL, useprefix);
        } else if (child(RAWXML_UNKNOWNS)) {
            OPTION_PARSE(float, train.unknowns.unk37, ParseFloat(child, RAWXML_UNKNOWNS, "u37"));
            OPTION_PARSE(float, train.unknowns.unk38, ParseFloat(child, RAWXML_UNKNOWNS, "u38"));
            OPTION_PARSE(float, train.unknowns.unk39, ParseFloat(child, RAWXML_UNKNOWNS, "u39"));
            OPTION_PARSE(float, train.unknowns.unk40, ParseFloat(child, RAWXML_UNKNOWNS, "u40"));
            OPTION_PARSE(float, train.unknowns.unk41, ParseFloat(child, RAWXML_UNKNOWNS, "u41"));
            OPTION_PARSE(float, train.unknowns.unk42, ParseFloat(child, RAWXML_UNKNOWNS, "u42"));
            OPTION_PARSE(float, train.unknowns.unk43, ParseFloat(child, RAWXML_UNKNOWNS, "u43"));
            OPTION_PARSE(float, train.unknowns.unk44, ParseFloat(child, RAWXML_UNKNOWNS, "u44"));
        } else if (child(RAWXML_RIT_SOAKED)) {
            OPTION_PARSE(unsigned long, train.soaked.icon, ParseUnsigned(child, RAWXML_RIT_SOAKED, "icon"));
            OPTION_PARSE(unsigned long, train.soaked.unk51, ParseUnsigned(child, RAWXML_RIT_SOAKED, "u51"));
        } else if (child(RAWXML_RIT_WILD)) {
            OPTION_PARSE(float, train.wild.airboat_unk01, ParseFloat(child, RAWXML_RIT_WILD, "airboatU01"));
            OPTION_PARSE(float, train.wild.airboat_unk02, ParseFloat(child, RAWXML_RIT_WILD, "airboatU02"));
            OPTION_PARSE(float, train.wild.airboat_unk03, ParseFloat(child, RAWXML_RIT_WILD, "airboatU03"));
            OPTION_PARSE(float, train.wild.airboat_unk04, ParseFloat(child, RAWXML_RIT_WILD, "airboatU04"));
            OPTION_PARSE(float, train.wild.airboat_unk05, ParseFloat(child, RAWXML_RIT_WILD, "airboatU05"));
            OPTION_PARSE(float, train.wild.airboat_unk06, ParseFloat(child, RAWXML_RIT_WILD, "airboatU06"));
            OPTION_PARSE(unsigned long, train.wild.unk59, ParseUnsigned(child, RAWXML_RIT_WILD, "u59"));
            OPTION_PARSE(unsigned long, train.wild.unk60, ParseUnsigned(child, RAWXML_RIT_WILD, "u60"));
            OPTION_PARSE(float, train.wild.ffsiez_unk, ParseFloat(child, RAWXML_RIT_WILD, "ffSiezU"));
        } else if (child.element()) {
            throw MakeNodeException<RCT3Exception>(wxString::Format(_("Unknown tag '%s' in rit tag"), child.wxname().c_str()), child);
        }
    }

    ovlRITManager* c_rit = m_ovl.GetManager<ovlRITManager>();
    c_rit->AddItem(train);
    wxLogVerbose(wxString::Format(_("Adding rit %s to %s. -- Done"), wxString(train.name.c_str()).c_str(), m_output.GetFullPath().c_str()));
}
