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
#include "SCNFile.h"

void cRawParser::ParseImport(cXmlNode& node) {
	USE_PREFIX(node);
	// <import file="scenery file" (name="internal svd name") />
	bool filenamevar;
	wxFSFileName filename = ParseString(node, RAWXML_IMPORT, "file", &filenamevar);
	wxString name;
	ParseStringOption(name, node, "name", NULL);
	wxString use;
	ParseStringOption(use, node, "use", NULL);
	if (!filename.IsAbsolute()) {
		filename.MakeAbsolute(m_input.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME));
	}

	if (m_mode == MODE_BAKE) {
		if (m_bake.Index(RAWXML_IMPORT) == wxNOT_FOUND) {
			if (!filenamevar) {
				if (m_bake.Index(RAWBAKE_ABSOLUTE) == wxNOT_FOUND) {
					filename.MakeRelativeTo(m_bakeroot.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME));
				}
				node.prop("file", filename.GetFullPath());
			}
			node.go_next();
			return;
		}
	}

	wxLogVerbose(wxString::Format(_("Importing from %s..."), filename.GetFullPath().c_str()));
	cSCNFile c_scn(filename.GetFullPath());
	if (!name.IsEmpty()) {
		c_scn.name = name;
	}
	c_scn.prefix = "";
	if (useprefix && (m_prefix != "")) {
		c_scn.prefix = wxString(m_prefix.c_str(), wxConvLocal);
	}

	if (m_mode == MODE_BAKE) {
		c_scn.Check();
		node.name(RAWXML_SECTION);
		node.delProp("name");
		node.delProp("file");
		if (node.children())
			node.children().detach();
// TODO (belgabor#1#): bake use attribute
		BakeScenery(node, *c_scn.m_work);
		Parse(node); // Bake contained stuff
	} else {
		wxLogVerbose(wxString::Format(_("Importing %s (%s) to %s."), filename.GetFullPath().c_str(), c_scn.name.c_str(), m_output.GetFullPath().c_str()));
		if (use.IsEmpty()) {
			bool hasSomething = false;
			
			foreach (const cXmlNode& child, node.children()) {
				DO_CONDITION_COMMENT_FOR(child);
				
				if (child("main")) {
					hasSomething = true;
					c_scn.MakeToOvlMain(m_ovl);
				} else if (child("animations")) {
					hasSomething = true;
					c_scn.MakeToOvlAnimations(m_ovl);
				} else if (child("animation")) {
					hasSomething = true;
					wxString anim = HandleStringContent(child.wxcontent(), NULL, false);
					c_scn.MakeToOvlAnimations(m_ovl, anim);
				} else if (child("splines")) {
					hasSomething = true;
					c_scn.MakeToOvlSplines(m_ovl);
				} else if (child("spline")) {
					hasSomething = true;
					wxString spl = HandleStringContent(child.wxcontent(), NULL, false);
					c_scn.MakeToOvlSplines(m_ovl, spl);
				} else if (child.element()) {
					throw MakeNodeException<RCT3Exception>(wxString::Format(_("Unknown tag '%s' in import tag"), child.wxname().c_str()), child);
				}
			}
			
			if (!hasSomething)
				c_scn.MakeToOvl(m_ovl);
		} else if (use == "main") {
			c_scn.MakeToOvlMain(m_ovl);
		} else if (use == "animations") {
			c_scn.MakeToOvlAnimations(m_ovl);
		} else if (use == "splines") {
			c_scn.MakeToOvlSplines(m_ovl);
		} else  {
			throw MakeNodeException<RCT3Exception>(wxString::Format(_("Unknown value '%s' for use attribute in import tag"), use.c_str()), node);
		}
	}

}

