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

#include "ManagerCHG.h"

void cRawParser::ParseCHG(cXmlNode& node) {
    USE_PREFIX(node);
    cChangingRoom room;
    room.name = ParseString(node, wxT(RAWXML_CHG), wxT("name"), NULL, useprefix).ToAscii();
    room.attraction.name = ParseString(node, wxT(RAWXML_CHG), wxT("nametxt"), NULL, useprefix).ToAscii();
    room.attraction.description = ParseString(node, wxT(RAWXML_CHG), wxT("description"), NULL, useprefix).ToAscii();
    room.sid = ParseString(node, wxT(RAWXML_CHG), wxT("sid"), NULL, useprefix).ToAscii();
    room.spline = ParseString(node, wxT(RAWXML_CHG), wxT("roomspline"), NULL, useprefix).ToAscii();
    wxLogVerbose(wxString::Format(_("Adding chg %s to %s."), wxString(room.name.c_str(), wxConvLocal).c_str(), m_output.GetFullPath().c_str()));

    cXmlNode child(node.children());
    while (child) {
        DO_CONDITION_COMMENT(child);

        if (child(RAWXML_ATTRACTION)) {
            ParseAttraction(child, room.attraction);
        } else if (child.element()) {
            throw MakeNodeException<RCT3Exception>(wxString::Format(_("Unknown tag '%s' in chg tag"), child.wxname().c_str()),child);
        }

        child.go_next();
    }

    ovlCHGManager* c_chg = m_ovl.GetManager<ovlCHGManager>();
    c_chg->AddRoom(room);
}
