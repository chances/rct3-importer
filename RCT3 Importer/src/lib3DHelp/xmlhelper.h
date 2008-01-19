///////////////////////////////////////////////////////////////////////////////
//
// 3D Utility Library
// Xml helper functions
// Copyright (C) 2006 Tobias Minch
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
// Parts of this code are based on code written for rct3tool (Copyright 2005
// Jonathan Wilson) by DragonsIOA.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _XMLHELPER_H_
#define _XMLHELPER_H_

inline wxXmlNode* makeElementNode(const wxString& name) {
    return new wxXmlNode(NULL, wxXML_ELEMENT_NODE, name);
}

inline wxXmlNode* makeTextNode(const wxString& text) {
    return new wxXmlNode(NULL, wxXML_TEXT_NODE, wxT(""), text);
}

inline wxXmlNode* makeContentNode(const wxString& name, const wxString& text) {
    wxXmlNode* t = makeElementNode(name);
    t->SetChildren(makeTextNode(text));
    return t;
}

#endif
