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

#include "ManagerSTA.h"

//#include <boost/iostreams/device/array.hpp>
//#include <boost/iostreams/stream.hpp>

void cRawParser::ParseSTA(cXmlNode& node) {
    USE_PREFIX(node);
    cStall stall;
    wxString name = ParseString(node, wxT(RAWXML_STA), wxT("name"), NULL, useprefix);
    stall.name = name.ToAscii();
    stall.attraction.name = ParseString(node, wxT(RAWXML_STA), wxT("nametxt"), NULL, useprefix).ToAscii();
    stall.attraction.description = ParseString(node, wxT(RAWXML_STA), wxT("description"), NULL, useprefix).ToAscii();
    stall.sid = ParseString(node, wxT(RAWXML_STA), wxT("sid"), NULL, useprefix).ToAscii();
    wxLogVerbose(wxString::Format(_("Adding sta %s to %s."), wxString(stall.name.c_str(), wxConvLocal).c_str(), m_output.GetFullPath().c_str()));

    cXmlNode child(node.children());
    while (child) {
        DO_CONDITION_COMMENT(child);

        if (child(RAWXML_ATTRACTION)) {
            ParseAttraction(child, stall.attraction);
        } else if (child(RAWXML_STA_ITEM)) {
            USE_PREFIX(child);
            cStallItem item;
            item.item = ParseString(child, wxT(RAWXML_STA_ITEM), wxT("cid"), NULL, useprefix).ToAscii();
            item.cost = ParseUnsigned(child, wxT(RAWXML_STA_ITEM), wxT("cost"));
            stall.items.push_back(item);
        } else if (child(RAWXML_STA_STALLUNKNOWNS)) {
            OPTION_PARSE(unsigned long, stall.unknowns.unk1, ParseUnsigned(child, wxT(RAWXML_STA_STALLUNKNOWNS), wxT("u1")));
            OPTION_PARSE(unsigned long, stall.unknowns.unk2, ParseUnsigned(child, wxT(RAWXML_STA_STALLUNKNOWNS), wxT("u2")));
            OPTION_PARSE(unsigned long, stall.unknowns.unk3, ParseUnsigned(child, wxT(RAWXML_STA_STALLUNKNOWNS), wxT("u3")));
            OPTION_PARSE(unsigned long, stall.unknowns.unk4, ParseUnsigned(child, wxT(RAWXML_STA_STALLUNKNOWNS), wxT("u4")));
            OPTION_PARSE(unsigned long, stall.unknowns.unk5, ParseUnsigned(child, wxT(RAWXML_STA_STALLUNKNOWNS), wxT("u5")));
            OPTION_PARSE(unsigned long, stall.unknowns.unk6, ParseUnsigned(child, wxT(RAWXML_STA_STALLUNKNOWNS), wxT("u6")));
        } else if (child.element()) {
            throw MakeNodeException<RCT3Exception>(wxString::Format(_("Unknown tag '%s' in sta tag '%s'."), child.wxname().c_str(), name.c_str()), child);
        }

        child.go_next();
    }

    ovlSTAManager* c_sta = m_ovl.GetManager<ovlSTAManager>();
    c_sta->AddStall(stall);
}
