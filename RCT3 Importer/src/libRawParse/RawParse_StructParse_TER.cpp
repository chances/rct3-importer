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

#include "ManagerTER.h"

#define RAWXML_TER_PARAMETERS       "parameters"

void cRawParser::ParseTER(cXmlNode& node) {
    USE_PREFIX(node);
    cTerrain terrain;
    terrain.name = ParseString(node, RAWXML_TER, "name", NULL, useprefix).ToAscii();
    terrain.description_name = ParseString(node, RAWXML_TER, "descriptionName", NULL, useprefix).ToAscii();
    terrain.icon_name = ParseString(node, RAWXML_TER, "iconName", NULL, useprefix).ToAscii();
    terrain.texture = ParseString(node, RAWXML_TER, "texture", NULL, useprefix).ToAscii();
    terrain.number = ParseUnsigned(node, RAWXML_TER, "number");
    OPTION_PARSE(unsigned long, terrain.version, ParseUnsigned(node, RAWXML_TER, "version"));
    OPTION_PARSE(unsigned long, terrain.addon, ParseUnsigned(node, RAWXML_TER, "addon"));
    OPTION_PARSE(unsigned long, terrain.type, ParseUnsigned(node, RAWXML_TER, "type"));
    wxLogVerbose(wxString::Format(_("Adding ter %s to %s."), wxString(terrain.name.c_str()).c_str(), m_output.GetFullPath().c_str()));

    foreach (const cXmlNode& child, node.children()) {
        USE_PREFIX(child);
        DO_CONDITION_COMMENT_FOR(child);

        if (child(RAWXML_TER_PARAMETERS)) {
            OPTION_PARSE(unsigned long, terrain.parameters.colour01, ParseUnsigned(child, RAWXML_TER_PARAMETERS, "colourSimple"));
            OPTION_PARSE(unsigned long, terrain.parameters.colour02, ParseUnsigned(child, RAWXML_TER_PARAMETERS, "colourMap"));
            OPTION_PARSE(float, terrain.parameters.inv_width, ParseFloat(child, RAWXML_TER_PARAMETERS, "invWidth"));
            OPTION_PARSE(float, terrain.parameters.inv_height, ParseFloat(child, RAWXML_TER_PARAMETERS, "invHeight"));
        } else if (child(RAWXML_UNKNOWNS)) {
            OPTION_PARSE(unsigned long, terrain.unknowns.unk02, ParseUnsigned(child, RAWXML_UNKNOWNS, "u02"));
            OPTION_PARSE(float, terrain.unknowns.unk13, ParseFloat(child, RAWXML_UNKNOWNS, "u13"));
            OPTION_PARSE(float, terrain.unknowns.unk14, ParseFloat(child, RAWXML_UNKNOWNS, "u14"));
            OPTION_PARSE(float, terrain.unknowns.unk15, ParseFloat(child, RAWXML_UNKNOWNS, "u15"));
        } else if (child.element()) {
            throw MakeNodeException<RCT3Exception>(wxString::Format(_("Unknown tag '%s' in ter tag"), child.wxname().c_str()), child);
        }
    }

    ovlTERManager* c_ter = m_ovl.GetManager<ovlTERManager>();
    c_ter->AddItem(terrain);
    wxLogVerbose(wxString::Format(_("Adding ter %s to %s. -- Done"), wxString(terrain.name.c_str()).c_str(), m_output.GetFullPath().c_str()));
}
