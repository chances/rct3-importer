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

#include "ManagerSAT.h"

void cRawParser::ParseSAT(cXmlNode& node) {
    USE_PREFIX(node);
    cSpecialAttraction spa;
    wxString name = ParseString(node, wxT(RAWXML_SAT), wxT("name"), NULL, useprefix);
    spa.name = name.ToAscii();
    spa.attraction.name = ParseString(node, wxT(RAWXML_SAT), wxT("nametxt"), NULL, useprefix).ToAscii();
    spa.attraction.description = ParseString(node, wxT(RAWXML_SAT), wxT("description"), NULL, useprefix).ToAscii();
    spa.sid = ParseString(node, wxT(RAWXML_SAT), wxT("sid"), NULL, useprefix).ToAscii();
    wxLogVerbose(wxString::Format(_("Adding sat %s to %s."), name.c_str(), m_output.GetFullPath().c_str()));

    cXmlNode child(node.children());
    while (child) {
        DO_CONDITION_COMMENT(child);

        if (child(RAWXML_ATTRACTION)) {
            ParseAttraction(child, spa.attraction);
        } else if (child.element()) {
            throw MakeNodeException<RCT3Exception>(wxString::Format(_("Unknown tag '%s' in sat tag '%s'"), child.wxname().c_str(), name.c_str()), child);
        }

        child.go_next();
    }

    ovlSATManager* c_sat = m_ovl.GetManager<ovlSATManager>();
    c_sat->AddAttraction(spa);
}
