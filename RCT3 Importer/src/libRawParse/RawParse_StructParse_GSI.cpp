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

#include "ManagerGSI.h"
#include "pretty.h"

#define RAWXML_GSI_POSITION "value"

void cRawParser::ParseGSI(cXmlNode& node) {
    USE_PREFIX(node);
	wxString name = ParseString(node, wxT(RAWXML_GSI), wxT("name"), NULL, useprefix);
    wxLogVerbose(wxString::Format(_("Adding gsi %s to %s."), name.c_str(), m_output.GetFullPath().c_str()));
	
	cGUISkinItem gsi;
		
	gsi.name = name.ToAscii();
	wxString tex = ParseString(node, wxT(RAWXML_GSI), wxT("tex"), NULL, useprefix);
	gsi.texture = tex.ToAscii();
	OPTION_PARSE(unsigned long, gsi.unk2, ParseUnsigned(node, RAWXML_GSI, "unk"));
	
	if (node.hasProp("type")) {
		gsi.gsi_type = ParseUnsigned(node, RAWXML_GSI, "type");
		
		foreach(const cXmlNode& child, node.children()) {
			USE_PREFIX(child);
			DO_CONDITION_COMMENT_FOR(child);
			
			if (child(RAWXML_GSI_POSITION)) {
				unsigned long p = HandleUnsignedContent(child.wxcontent(), child, RAWXML_GSI_POSITION);
				gsi.values.push_back(p);
			} else if (child.element()) {
				throw MakeNodeException<RCT3Exception>(wxString::Format(_("Unknown tag '%s' in gsi(%s) tag"), child.wxname().c_str(), name.c_str()), child);
			}
		}
	} else {
		unsigned long p;
		p = ParseUnsigned(node, RAWXML_GSI, "left");
		gsi.values.push_back(p);
		p = ParseUnsigned(node, RAWXML_GSI, "top");
		gsi.values.push_back(p);
		p = ParseUnsigned(node, RAWXML_GSI, "right");
		gsi.values.push_back(p);
		p = ParseUnsigned(node, RAWXML_GSI, "bottom");
		gsi.values.push_back(p);
	}

	ovlGSIManager* c_gsi = m_ovl.GetManager<ovlGSIManager>();
	c_gsi->AddItem(gsi);
}

