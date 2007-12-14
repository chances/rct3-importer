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


#ifndef _RAWOVLMAKE_H_
#define _RAWOVLMAKE_H_

#include "wx_pch.h"

#include <wx/filename.h>
#include <wx/xml/xml.h>

#include "OVLng.h"

#define RAWXML_ROOT wxT("rawovl")
#define RAWXML_SUBROOT wxT("subovl")

class cRawOvl {
private:
    wxFileName m_output;
    wxFileName m_outputbasedir;
    wxFileName m_input;  // For path relativity
    cOvl m_ovl;

    void Load(wxXmlNode* root);
    cOvlType ParseType(wxXmlNode* node, const wxString& nodes, const wxString& attribute = wxT("target"));
    wxString ParseString(wxXmlNode* node, const wxString& nodes, const wxString& attribute);
    unsigned long ParseUnsigned(wxXmlNode* node, const wxString& nodes, const wxString& attribute);
    long ParseSigned(wxXmlNode* node, const wxString& nodes, const wxString& attribute);
    double ParseFloat(wxXmlNode* node, const wxString& nodes, const wxString& attribute);

    void ParseSID(wxXmlNode* node);
    void ParseTEX(wxXmlNode* node);
public:
    cRawOvl(const wxFileName& file, const wxFileName& outputdir = wxString(wxT("")), const wxFileName& output = wxString(wxT("")));
    cRawOvl(wxXmlNode* root, const wxFileName& file, const wxFileName& outputdir = wxString(wxT("")), const wxFileName& output = wxString(wxT("")));
};

#endif
