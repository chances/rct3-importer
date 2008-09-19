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
//#include <wx/xml/xml.h>
#include <boost/shared_array.hpp>
#include <boost/exception/error_info.hpp>

#include "vertex.h"

#include "fsfilename.h"

#include "3DLoaderTypes.h"

#include "OVLng.h"

#include "cXmlNode.h"

#define RAWXML_ROOT      "rawovl"
#define RAWXML_SUBROOT   "rawovl"
#define RAWXML_SET       "set"
#define RAWXML_UNSET     "unset"
#define RAWXML_VARIABLES "variables"
#define RAWXML_DATAREF   "dataref"

typedef boost::error_info<struct tag_node_line, int> node_line_info;

WX_DECLARE_STRING_HASH_MAP(wxString, cRawParserVars);

enum eRawOvlMode {
    MODE_COMPILE = 0,
    MODE_INSTALL = 1,
    MODE_BAKE = 2
};

class cAttraction;
class cRide;
class cRideStationLimit;

class cRawDatablock {
private:
    boost::shared_array<unsigned char> m_data;
    wxString m_datatype;
    unsigned long m_datasize;
    xmlcpp::cXmlNode m_node;
    xmlcpp::cXmlNode m_refnode;
    xmlcpp::cXmlNode m_insertbefore;
public:
    cRawDatablock(const xmlcpp::cXmlNode& node, const xmlcpp::cXmlNode& refnode, const xmlcpp::cXmlNode& insertbefore):
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
    xmlcpp::cXmlNode m_firstchild; ///< For making new data references.
    std::map<wxString, xmlcpp::cXmlNode> m_datareferences;

    cOvl m_ovl;

    void Load(xmlcpp::cXmlNode& root);
    cOvlType ParseType(const xmlcpp::cXmlNode& node, const wxString& nodes, const wxString& attribute = wxT("target"));
    wxString ParseString(const xmlcpp::cXmlNode& node, const wxString& nodes, const wxString& attribute, bool* variable, bool addprefix = false);
    wxString HandleStringContent(const wxString& content, bool* variable, bool addprefix = false);
    void ParseStringOption(std::string& str, const xmlcpp::cXmlNode& node, const wxString& attribute, bool* variable, bool addprefix = false);
    void ParseStringOption(wxString& str, const xmlcpp::cXmlNode& node, const wxString& attribute, bool* variable, bool addprefix = false);
    unsigned long ParseUnsigned(const xmlcpp::cXmlNode& node, const wxString& nodes, const wxString& attribute);
    long ParseSigned(const xmlcpp::cXmlNode& node, const wxString& nodes, const wxString& attribute);
    double ParseFloat(const xmlcpp::cXmlNode& node, const wxString& nodes, const wxString& attribute);
    void ParseVector(const xmlcpp::cXmlNode& node, r3::VECTOR& v, const wxString& nodes);
    void ParseMatrix(const xmlcpp::cXmlNode& node, r3::MATRIX& m, const wxString& nodes);

    void ParseAttraction(const xmlcpp::cXmlNode& node, cAttraction& attraction);
    void ParseRide(const xmlcpp::cXmlNode& node, cRide& ride);
    void ParseRideStationLimit(const xmlcpp::cXmlNode& node, cRideStationLimit& limit);

    c3DLoaderOrientation ParseOrientation(const xmlcpp::cXmlNode& node, const wxString& nodes, c3DLoaderOrientation defori = ORIENTATION_LEFT_YUP);

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

    void CopyBaseAttributes(const xmlcpp::cXmlNode& from, xmlcpp::cXmlNode& to);

    void ParseANR(xmlcpp::cXmlNode& node);
    void ParseBAN(xmlcpp::cXmlNode& node);
    void ParseBSH(xmlcpp::cXmlNode& node);
    void ParseCED(xmlcpp::cXmlNode& node);
    void ParseCHG(xmlcpp::cXmlNode& node);
    void ParseCID(xmlcpp::cXmlNode& node);
    void ParseFTX(xmlcpp::cXmlNode& node);
    void ParsePTD(xmlcpp::cXmlNode& node);
    void ParseQTD(xmlcpp::cXmlNode& node);
    void ParseSAT(xmlcpp::cXmlNode& node);
    void ParseSHS(xmlcpp::cXmlNode& node);
    void ParseSID(xmlcpp::cXmlNode& node);
    void ParseSND(xmlcpp::cXmlNode& node);
    void ParseSPL(xmlcpp::cXmlNode& node);
    void ParseSTA(xmlcpp::cXmlNode& node);
    void ParseSVD(xmlcpp::cXmlNode& node);
    void ParseTEX(xmlcpp::cXmlNode& node);
    void ParseTKS(xmlcpp::cXmlNode& node);
    void ParseTRR(xmlcpp::cXmlNode& node);
    void ParseWAI(xmlcpp::cXmlNode& node);

    void ParseSet(const xmlcpp::cXmlNode& node, bool command = false, const wxString& tag = wxT(RAWXML_SET));
    void ParseUnset(const xmlcpp::cXmlNode& node, bool command = false);
    void ParseVariables(xmlcpp::cXmlNode& node, bool command = false, const wxString& path = wxT(""));
    void Parse(xmlcpp::cXmlNode& node);

    xmlcpp::cXmlNode* FindDataReference(const wxString& guid, wxFSFileName& input);
    cRawDatablock MakeDataBlock(const wxString& ref, const wxFSFileName& input, const xmlcpp::cXmlNode& node, const xmlcpp::cXmlNode& refnode = xmlcpp::cXmlNode());
    cRawDatablock GetDataBlock(const wxString& ref, const xmlcpp::cXmlNode& node);

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
    cRawParser(xmlcpp::cXmlNode& root, const wxFSFileName& file, const wxFileName& outputdir = wxString(wxT("")), const wxFileName& output = wxString(wxT(""))) {
        Init();
        Process(root, file, outputdir, output);
    }

    void SetOptions(eRawOvlMode mode, bool dryrun) {
        m_mode = mode;
        m_dryrun = dryrun;
        if (mode == MODE_INSTALL) {
            m_commandvariables[wxT("INSTALLING")] = wxT("1");
        }
    }

    const std::vector<wxFileName>& GetModifiedFiles() const {
        return m_modifiedfiles;
    }
    const std::vector<wxFileName>& GetNewFiles() const {
        return m_newfiles;
    }

    void Process(const wxFSFileName& file, const wxFileName& outputdir = wxString(wxT("")), const wxFileName& output = wxString(wxT("")));
    void Process(xmlcpp::cXmlNode& root, const wxFSFileName& file, const wxFileName& outputdir = wxString(wxT("")), const wxFileName& output = wxString(wxT("")));

    void AddConditions(const wxString& options);
    void AddConditions(const wxArrayString& options);
    void AddConditions(const wxSortedArrayString& options);
    void RemoveConditions(const wxString& options);
    void ParseConditions(const wxString& options);

    void LoadVariables(const wxFSFileName& fn, bool command = false, xmlcpp::cXmlNode* target = NULL);

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
    static boost::shared_ptr<xmlcpp::cXmlValidator> Validator();
};

#endif
