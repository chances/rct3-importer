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

#include "ManagerANR.h"

#include "pretty.h"

#define RAWXML_ANR_SHOWITEM         "showItem"

void cRawParser::ParseANR(cXmlNode& node) {
    USE_PREFIX(node);
    cAnimatedRide animride;
    animride.name = ParseString(node, wxT(RAWXML_ANR), wxT("name"), NULL, useprefix).ToAscii();
    animride.attraction.name = ParseString(node, wxT(RAWXML_ANR), wxT("nametxt"), NULL, useprefix).ToAscii();
    animride.attraction.description = ParseString(node, wxT(RAWXML_ANR), wxT("description"), NULL, useprefix).ToAscii();
    animride.sid = ParseString(node, wxT(RAWXML_ANR), wxT("sid"), NULL, useprefix).ToAscii();
    wxLogVerbose(wxString::Format(_("Adding anr %s to %s."), wxString(animride.name.c_str(), wxConvLocal).c_str(), m_output.GetFullPath().c_str()));

    foreach(const cXmlNode& child, node.children()) {
        DO_CONDITION_COMMENT_FOR(child);

        if (child(RAWXML_ATTRACTION)) {
            ParseAttraction(child, animride.attraction);
        } else if (child(RAWXML_RIDE)) {
            ParseRide(child, animride.ride);
        } else if (child(RAWXML_UNKNOWNS)) {
            OPTION_PARSE(unsigned long, animride.unk22, ParseUnsigned(child, RAWXML_ANR "/" RAWXML_UNKNOWNS, wxT("u22")));
            OPTION_PARSE(unsigned long, animride.unk23, ParseUnsigned(child, RAWXML_ANR "/" RAWXML_UNKNOWNS, wxT("u23")));
            OPTION_PARSE(unsigned long, animride.unk24, ParseUnsigned(child, RAWXML_ANR "/" RAWXML_UNKNOWNS, wxT("u24")));
            OPTION_PARSE(unsigned long, animride.unk25, ParseUnsigned(child, RAWXML_ANR "/" RAWXML_UNKNOWNS, wxT("u25")));
            OPTION_PARSE(unsigned long, animride.unk8, ParseUnsigned(child, RAWXML_ANR "/" RAWXML_UNKNOWNS, wxT("u8")));
            OPTION_PARSE(unsigned long, animride.unk9, ParseUnsigned(child, RAWXML_ANR "/" RAWXML_UNKNOWNS, wxT("u9")));
        } else if (child(RAWXML_ANR_SHOWITEM)) {
            cAnimatedRideShowItem si;
            si.animation = ParseUnsigned(child, RAWXML_ANR_SHOWITEM, wxT("animationIndex"));
            si.name = child();
            boost::trim(si.name);
            animride.showitems.push_back(si);
        } else if (child.element()) {
            throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in anr tag"), child.wxname().c_str()));
        }
    }

    ovlANRManager* c_anr = m_ovl.GetManager<ovlANRManager>();
    c_anr->AddRide(animride);
    wxLogVerbose(wxString::Format(_("Adding anr %s to %s. -- Done"), wxString(animride.name.c_str(), wxConvLocal).c_str(), m_output.GetFullPath().c_str()));
}
