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

#include "ManagerCED.h"

#define RAWXML_CED_MORE             "cedmore"

void cRawParser::ParseCED(cXmlNode& node) {
    USE_PREFIX(node);
    cCarriedItemExtra ced;
    ced.name = ParseString(node, wxT(RAWXML_CED), wxT("name"), NULL, useprefix).ToAscii();
    ced.nametxt = ParseString(node, wxT(RAWXML_CED), wxT("nametxt"), NULL, useprefix).ToAscii();
    ced.icon = ParseString(node, wxT(RAWXML_CED), wxT("icon"), NULL, useprefix).ToAscii();
    wxLogVerbose(wxString::Format(_("Adding ced %s to %s."), wxString(ced.name.c_str(), wxConvLocal).c_str(), m_output.GetFullPath().c_str()));

    cXmlNode child(node.children());
    while (child) {
        DO_CONDITION_COMMENT(child);

        if (child(RAWXML_CED_MORE)) {
            OPTION_PARSE(float, ced.hunger, ParseFloat(child, wxT(RAWXML_CED_MORE), wxT("hunger")));
            OPTION_PARSE(float, ced.thirst, ParseFloat(child, wxT(RAWXML_CED_MORE), wxT("thirst")));
            OPTION_PARSE(unsigned long, ced.unk1, ParseUnsigned(child, wxT(RAWXML_CED_MORE), wxT("u1")));
            OPTION_PARSE(unsigned long, ced.unk4, ParseUnsigned(child, wxT(RAWXML_CED_MORE), wxT("u4")));
            OPTION_PARSE(float, ced.unk7, ParseFloat(child, wxT(RAWXML_CED_MORE), wxT("u7")));
        } else if (child.element()) {
            throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in ced tag."), child.wxname().c_str()));
        }
        child.go_next();
    }

    ovlCEDManager* c_ced = m_ovl.GetManager<ovlCEDManager>();
    c_ced->AddExtra(ced);
}

