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

#define RAWXML_ROOT      wxT("rawovl")
#define RAWXML_SUBROOT   wxT("subovl")
#define RAWXML_VARIABLE  wxT("variable")
#define RAWXML_VARIABLES wxT("variables")

WX_DECLARE_STRING_HASH_MAP(wxString, cRawOvlVars);

enum eRawOvlMode {
    MODE_COMPILE = 0,
    MODE_INSTALL = 1,
    MODE_BAKE = 2
};

class cRawOvl {
private:
    wxFileName m_output;
    wxFileName m_outputbasedir;
    wxFileName m_input;  // For path relativity
//    wxSortedArrayString m_options;
    wxSortedArrayString m_bake;
    wxFileName m_bakeroot;
    cRawOvlVars m_commandvariables;
    cRawOvlVars m_variables;

    eRawOvlMode m_mode;
    bool m_dryrun;
    std::vector<wxFileName> m_modifiedfiles;
    std::vector<wxFileName> m_newfiles;

    cOvl m_ovl;

    void Load(wxXmlNode* root);
    cOvlType ParseType(wxXmlNode* node, const wxString& nodes, const wxString& attribute = wxT("target"));
    wxString ParseString(wxXmlNode* node, const wxString& nodes, const wxString& attribute);
    unsigned long ParseUnsigned(wxXmlNode* node, const wxString& nodes, const wxString& attribute);
    long ParseSigned(wxXmlNode* node, const wxString& nodes, const wxString& attribute);
    double ParseFloat(wxXmlNode* node, const wxString& nodes, const wxString& attribute);

    bool MakeVariable(wxString& var);
    void PassBakeStructures(const wxSortedArrayString& bake) {
        m_bake = bake;
    }
    void PassBakeRoot(const wxFileName& br) {
        m_bakeroot = br;
    }
    void PassVariables(const cRawOvlVars& c, const cRawOvlVars& v) {
        m_commandvariables = c;
        m_variables = v;
    }

    void ParseCED(wxXmlNode* node);
    void ParseCHG(wxXmlNode* node);
    void ParseCID(wxXmlNode* node);
    void ParseSID(wxXmlNode* node);
    void ParseSPL(wxXmlNode* node);
    void ParseSTA(wxXmlNode* node);
    void ParseTEX(wxXmlNode* node);
    void ParseVariable(wxXmlNode* node, bool command = false);
    void ParseVariables(wxXmlNode* node, bool command = false, const wxString& path = wxT(""));
    void Parse(wxXmlNode* node);

    void Init() {
        m_mode = MODE_COMPILE;
        m_dryrun = false;
    }
public:
    cRawOvl(){
        Init();
    }
    cRawOvl(const wxFileName& file, const wxFileName& outputdir = wxString(wxT("")), const wxFileName& output = wxString(wxT(""))) {
        Init();
        Process(file, outputdir, output);
    }
    cRawOvl(wxXmlNode* root, const wxFileName& file, const wxFileName& outputdir = wxString(wxT("")), const wxFileName& output = wxString(wxT(""))) {
        Init();
        Process(root, file, outputdir, output);
    }

    void SetOptions(eRawOvlMode mode, bool dryrun) {
        m_mode = mode;
        m_dryrun = dryrun;
    }

    const std::vector<wxFileName>& GetModifiedFiles() const {
        return m_modifiedfiles;
    }
    const std::vector<wxFileName>& GetNewFiles() const {
        return m_newfiles;
    }

    void Process(const wxFileName& file, const wxFileName& outputdir = wxString(wxT("")), const wxFileName& output = wxString(wxT("")));
    void Process(wxXmlNode* root, const wxFileName& file, const wxFileName& outputdir = wxString(wxT("")), const wxFileName& output = wxString(wxT("")));

    void AddConditions(const wxString& options);
    void AddConditions(const wxArrayString& options);
    void AddConditions(const wxSortedArrayString& options);
    void RemoveConditions(const wxString& options);
    void ParseConditions(const wxString& options);

    void LoadVariables(const wxFileName& fn, bool command = false, wxXmlNode* target = NULL);

    void AddBakeStructures(const wxString& structs);
};

#endif
