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
#include <boost/shared_array.hpp>

#include "vertex.h"

#include "fsfilename.h"

#include "3DLoaderTypes.h"

#include "OVLng.h"

#define RAWXML_ROOT      wxT("rawovl")
#define RAWXML_SUBROOT   wxT("subovl")
#define RAWXML_SET       wxT("set")
#define RAWXML_UNSET     wxT("unset")
#define RAWXML_VARIABLES wxT("variables")
#define RAWXML_DATAREF   wxT("dataref")

WX_DECLARE_STRING_HASH_MAP(wxString, cRawParserVars);

enum eRawOvlMode {
    MODE_COMPILE = 0,
    MODE_INSTALL = 1,
    MODE_BAKE = 2
};

class cRawDatablock {
private:
    boost::shared_array<unsigned char> m_data;
    wxString m_datatype;
    unsigned long m_datasize;
    wxXmlNode* m_node;
    wxXmlNode* m_refnode;
    wxXmlNode* m_insertbefore;
public:
    cRawDatablock(wxXmlNode* node, wxXmlNode* refnode, wxXmlNode* insertbefore):
        m_datasize(0), m_node(node), m_refnode(refnode), m_insertbefore(insertbefore) {};

    unsigned long datasize() const { return m_datasize; };
    void datasize(unsigned long _datasize) {
        m_data = boost::shared_array<unsigned char>(new unsigned char[_datasize]);
        m_datasize = _datasize;
    }
    const boost::shared_array<unsigned char>& data() const { return m_data; }
    boost::shared_array<unsigned char>& data() { return m_data; }
    const wxString& datatype() const { return m_datatype; }
    void datatype(const wxString& _datatype) {
        m_datatype = _datatype;
    }
};

class cRawParser {
private:
    wxFileName m_output;
    wxFileName m_outputbasedir;
    wxFSFileName m_input;  ///< For path relativity
    wxFileName m_userdir;
//    wxSortedArrayString m_options;
    wxSortedArrayString m_bake;
    wxFSFileName m_bakeroot;
    cRawParserVars m_commandvariables;
    cRawParserVars m_variables;
    std::string m_prefix;

    eRawOvlMode m_mode;
    bool m_dryrun;
    std::vector<wxFileName> m_modifiedfiles;
    std::vector<wxFileName> m_newfiles;

    cRawParser* m_parent;
    wxXmlNode* m_firstchild; ///< For making new data references.
    std::map<wxString, wxXmlNode*> m_datareferences;

    cOvl m_ovl;

    void Load(wxXmlNode* root);
    cOvlType ParseType(wxXmlNode* node, const wxString& nodes, const wxString& attribute = wxT("target"));
    wxString ParseString(wxXmlNode* node, const wxString& nodes, const wxString& attribute, bool* variable, bool addprefix = false);
    void ParseStringOption(std::string& str, wxXmlNode* node, const wxString& attribute, bool* variable, bool addprefix = false);
    void ParseStringOption(wxString& str, wxXmlNode* node, const wxString& attribute, bool* variable, bool addprefix = false);
    unsigned long ParseUnsigned(wxXmlNode* node, const wxString& nodes, const wxString& attribute);
    long ParseSigned(wxXmlNode* node, const wxString& nodes, const wxString& attribute);
    double ParseFloat(wxXmlNode* node, const wxString& nodes, const wxString& attribute);
    void ParseVector(wxXmlNode* node, VECTOR& v, const wxString& nodes);
    void ParseMatrix(wxXmlNode* node, MATRIX& m, const wxString& nodes);
    c3DLoaderOrientation ParseOrientation(wxXmlNode* node, const wxString& nodes);

    bool MakeVariable(wxString& var);
    void PassBakeStructures(const wxSortedArrayString& bake) {
        m_bake = bake;
    }
    void PassBakeRoot(const wxFSFileName& br) {
        m_bakeroot = br;
    }
    void PassVariables(const cRawParserVars& c, const cRawParserVars& v) {
        m_commandvariables = c;
        m_variables = v;
    }

    void CopyBaseAttributes(const wxXmlNode* from, wxXmlNode* to);

    void ParseBAN(wxXmlNode* node);
    void ParseBSH(wxXmlNode* node);
    void ParseCED(wxXmlNode* node);
    void ParseCHG(wxXmlNode* node);
    void ParseCID(wxXmlNode* node);
    void ParseFTX(wxXmlNode* node);
    void ParseSAT(wxXmlNode* node);
    void ParseSHS(wxXmlNode* node);
    void ParseSID(wxXmlNode* node);
    void ParseSPL(wxXmlNode* node);
    void ParseSTA(wxXmlNode* node);
    void ParseSVD(wxXmlNode* node);
    void ParseTEX(wxXmlNode* node);
    void ParseWAI(wxXmlNode* node);

    void ParseSet(wxXmlNode* node, bool command = false, const wxString& tag = RAWXML_SET);
    void ParseUnset(wxXmlNode* node, bool command = false);
    void ParseVariables(wxXmlNode* node, bool command = false, const wxString& path = wxT(""));
    void Parse(wxXmlNode* node);

    wxXmlNode* FindDataReference(const wxString& guid, wxFSFileName& input) const;
    cRawDatablock MakeDataBlock(const wxString& ref, const wxFSFileName& input, wxXmlNode* node, wxXmlNode* refnode = NULL);
    cRawDatablock GetDataBlock(const wxString& ref, wxXmlNode* node);

    void Init() {
        m_mode = MODE_COMPILE;
        m_dryrun = false;
        m_parent = NULL;
    }
public:
    cRawParser(){
        Init();
    }
    cRawParser(const wxFSFileName& file, const wxFileName& outputdir = wxString(wxT("")), const wxFileName& output = wxString(wxT(""))) {
        Init();
        Process(file, outputdir, output);
    }
    cRawParser(wxXmlNode* root, const wxFSFileName& file, const wxFileName& outputdir = wxString(wxT("")), const wxFileName& output = wxString(wxT(""))) {
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

    void Process(const wxFSFileName& file, const wxFileName& outputdir = wxString(wxT("")), const wxFileName& output = wxString(wxT("")));
    void Process(wxXmlNode* root, const wxFSFileName& file, const wxFileName& outputdir = wxString(wxT("")), const wxFileName& output = wxString(wxT("")));

    void AddConditions(const wxString& options);
    void AddConditions(const wxArrayString& options);
    void AddConditions(const wxSortedArrayString& options);
    void RemoveConditions(const wxString& options);
    void ParseConditions(const wxString& options);

    void LoadVariables(const wxFSFileName& fn, bool command = false, wxXmlNode* target = NULL);

    void AddBakeStructures(const wxString& structs);

    void SetPrefix(const std::string& prefix) {
        m_prefix = prefix;
    }
    void SetUserDir(const wxFileName& userdir) {
        m_userdir = userdir;
    }
    void SetParent(cRawParser* parent) {
        m_parent = parent;
    }

    static void FillAllBakes(wxSortedArrayString& tofill);
};

#endif
