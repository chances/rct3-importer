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

#include "cXmlInputOutputCallbackString.h"
#include "cXmlValidatorIsoSchematron.h"
#include "cXmlValidatorMulti.h"
#include "cXmlValidatorRNVRelaxNG.h"

#include "rng/rct3xml-raw-v1.rnc.gz.h"
#include "rng/rct3xml-raw-v1.sch.gz.h"

void cRawParser::FillAllBakes(wxSortedArrayString& tofill) {
    tofill.push_back(wxT(RAWXML_VARIABLES));
    tofill.push_back(wxT(RAWXML_IMPORT));
    tofill.push_back(wxT(RAWXML_WRITE));
    tofill.push_back(wxT(RAWXML_BSH));
    tofill.push_back(wxT(RAWXML_FTX));
    tofill.push_back(wxT(RAWXML_SHS));
    tofill.push_back(wxT(RAWXML_TEX));
    tofill.push_back(wxT(RAWXML_TXT));
//    tofill.push_back(wxT(RAWBAKE_XML));
}

boost::shared_ptr<cXmlValidator> cRawParser::Validator() {
	wxLogDebug("cRawParser::Validator()");
    cXmlInputOutputCallbackString::Init();
    XMLCPP_RES_ADD_ONCE(rct3xml_raw_v1, rnc);
    XMLCPP_RES_ADD_ONCE(rct3xml_raw_v1, sch);

    boost::shared_ptr<cXmlValidatorMulti> val(new cXmlValidatorMulti());
	wxLogDebug("cRawParser::Validator(), primary");
    val->primary(boost::shared_ptr<cXmlValidator>(new cXmlValidatorRNVRelaxNG(XMLCPP_RES_USE(rct3xml_raw_v1, rnc).c_str())));
	wxLogDebug("cRawParser::Validator(), secondary");
    val->secondary(boost::shared_ptr<cXmlValidator>(new cXmlValidatorIsoSchematron(XMLCPP_RES_USE(rct3xml_raw_v1, sch).c_str())));
    if (!val->primary()->ok()) {
        wxString error(_("Internal Error: could not load raw RelaxNG schema:\n"));
        error += val->primary()->wxgetErrorList();
        throw RCT3Exception(error);
    }
    if (!val->secondary()->ok()) {
        wxString error(_("Internal Error: could not load raw Schematron schema:\n"));
        error += val->secondary()->wxgetErrorList();
        throw RCT3Exception(error);
    }
    return val;
}

void cRawParser::Process(const wxFSFileName& file, const wxFileName& outputdir, const wxFileName& output) {
    m_input = file;
    m_output = output;
    if (outputdir == wxT("")) {
        m_outputbasedir.SetPath(file.GetBasePath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME));
    } else {
        m_outputbasedir.SetPath(outputdir.GetPathWithSep());
    }
    cXmlDoc doc;
/*
    wxFileSystem fs;
    std::auto_ptr<wxFSFile> inputfsfile(fs.OpenFile(file.GetFullPath(), wxFS_READ | wxFS_SEEKABLE));
    if (!inputfsfile.get())
        throw RCT3Exception(_("Cannot read input file ")+file.GetFullPath());

    if (!doc.Load(*inputfsfile->GetStream()))
        throw RCT3Exception(wxT("Error loading xml file ")+file.GetFullPath());
*/
    if (!doc.read(file.GetFullPath().mb_str(wxConvUTF8), NULL, XML_PARSE_DTDLOAD))
        throw RCT3Exception(wxT("Error loading xml file ")+file.GetFullPath());

    cXmlNode root(doc.root());
    Load(root);
}

void cRawParser::Process(cXmlNode& root, const wxFSFileName& file, const wxFileName& outputdir, const wxFileName& output) {
    m_input = file;
    if (!m_input.IsAbsolute())
        m_input.MakeAbsolute();
    m_output = output;
    if (outputdir == wxT("")) {
        m_outputbasedir.SetPath(file.GetBasePath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME));
    } else {
        m_outputbasedir.SetPath(outputdir.GetPathWithSep());
    }
    Load(root);
}

void cRawParser::AddConditions(const wxString& options) {
    wxStringTokenizer tok(options, wxT(" "));

    while (tok.HasMoreTokens()) {
        wxString t = tok.GetNextToken();
        if ((m_variables.find(t) == m_variables.end()) && (t != wxT("inherit")))
            m_variables[t] = wxT("1");
    }
}

void cRawParser::AddConditions(const wxArrayString& options) {
    for (wxArrayString::const_iterator it = options.begin(); it != options.end(); ++it) {
        if ((m_variables.find(*it) == m_variables.end()) && (*it != wxT("inherit")))
            m_variables[*it] = wxT("1");
    }
}

void cRawParser::AddConditions(const wxSortedArrayString& options) {
    for (wxSortedArrayString::const_iterator it = options.begin(); it != options.end(); ++it) {
        if ((m_variables.find(*it) == m_variables.end()) && (*it != wxT("inherit")))
            m_variables[*it] = wxT("1");
    }
}

void cRawParser::RemoveConditions(const wxString& options) {
    wxStringTokenizer tok(options, wxT(" "));

    while (tok.HasMoreTokens()) {
        wxString t = tok.GetNextToken();
        if (m_variables.find(t) != m_variables.end())
            m_variables.erase(t);
    }
}

void cRawParser::ParseConditions(const wxString& options) {
    wxStringTokenizer tok(options, wxT(" "));

    while (tok.HasMoreTokens()) {
        wxString t = tok.GetNextToken();
        if (t[0] == '-') {
            if (m_variables.find(t.Mid(1)) != m_variables.end())
                m_variables.erase(t.Mid(1));
        } else if (t[0] == '+') {
            if ((m_variables.find(t.Mid(1)) == m_variables.end()) && (t.Mid(1) != wxT("inherit")))
                m_variables[t.Mid(1)] = wxT("1");
        } else {
            if ((m_variables.find(t) == m_variables.end()) && (t != wxT("inherit")))
                m_variables[t] = wxT("1");
        }
    }
}

void cRawParser::AddBakeStructures(const wxString& structs) {
    if (structs == wxT("all")) {
        cRawParser::FillAllBakes(m_bake);
        return;
    }

    wxStringTokenizer tok(structs, wxT(" "));

    while (tok.HasMoreTokens()) {
        wxString t = tok.GetNextToken();
        if (m_bake.Index(t) == wxNOT_FOUND)
            m_bake.Add(t);
    }
}

bool cRawParser::MakeVariable(wxString& var) {
    var.Trim();
    var.Trim(false);
    wxString varname = var.AfterFirst('$').BeforeLast('$');
    if (varname != wxT("")) {
        wxString repl;
        if (varname == wxT("PREFIX")) {
            repl = wxString(m_prefix.c_str(), wxConvLocal);
        } else {
            cRawParserVars::iterator it = m_commandvariables.find(varname);
            if (it == m_commandvariables.end())
                it = m_variables.find(varname);
            if (it == m_variables.end()) {
                wxLogWarning(wxString::Format(_("Varible '%s' undefined."), varname.c_str()));
                return false;
            }
            repl = it->second;
        }
        wxLogVerbose(_("Found variable: ") + varname + wxT(" --> ") + repl);
        var.Replace(wxT("$")+varname+wxT("$"), repl, false);
        return true;
    }
    return false;
}

void cRawParser::CopyBaseAttributes(const cXmlNode& from, cXmlNode& to) {
    if (from.hasProp("comment")) {
        to.addProp("comment", from.getPropVal("comment", ""));
    }
    if (from.hasProp("if")) {
        to.addProp("if", from.getPropVal("if", ""));
    }
    if (from.hasProp("ifnot")) {
        to.addProp("ifnot", from.getPropVal("ifnot", ""));
    }
    if (from.hasProp("useprefix")) {
        to.addProp("useprefix", from.getPropVal("useprefix", ""));
    }
}

void cRawParser::Load(cXmlNode& root) {
	try {
		if (!root)
			throw MakeNodeException<RCT3Exception>(wxT("cRawParser::Load, root is broken"), root);
		bool subonly = false;

		wxLogMessage("-----");
		wxLogVerbose(wxString::Format(_("Parsing from raw xml %s."), m_input.GetFullPath().c_str()));

		for (cRawParserVars::iterator it = m_variables.begin(); it != m_variables.end(); ++it) {
			wxLogVerbose(wxString::Format(_("  var '%s'->'%s'."), it->first.c_str(), it->second.c_str()));
		}

		if (m_bakeroot == wxT(""))
			m_bakeroot = m_input;

		m_firstchild = root.children();

		if (m_mode != MODE_BAKE) {
			if (root.hasProp("conditions")) {
				wxString cond(root.getPropVal("conditions", "").c_str(), wxConvUTF8);
				MakeVariable(cond);
				ParseConditions(cond);
			}
		}

		if (root(RAWXML_ROOT) || root(RAWXML_SUBROOT)) {
			// <rawovl|subovl file="outputfile" basedir="dir outputfile is relative to">
			{
				wxString basedir(root.getPropVal("basedir").c_str(), wxConvUTF8);
				MakeVariable(basedir);
				if ((m_mode == MODE_INSTALL) && root.hasProp("installdir")) {
					basedir = wxString(root.getPropVal("installdir").c_str(), wxConvUTF8);
				}
				if (!basedir.IsEmpty()) {
					wxFileName temp;
					temp.SetPath(basedir);
					if (!temp.IsAbsolute()) {
						temp.MakeAbsolute(m_outputbasedir.GetPathWithSep());
					}
					m_outputbasedir = temp;
				}
			}

			if ((m_dryrun) || (m_mode == MODE_BAKE)) {
				if (!CanBeWrittenTo(m_outputbasedir)) {
					wxLogWarning(wxString::Format(_("Output directory '%s' cannot be written to."), m_outputbasedir.GetFullPath().c_str()));
				}
			} else {
				if (!CanBeWrittenTo(m_outputbasedir)) {
					throw RCT3Exception(wxString::Format(_("Output directory '%s' cannot be written to"), m_outputbasedir.GetFullPath().c_str()));
				}
				if (!EnsureDir(m_outputbasedir))
					throw RCT3Exception(_("Failed to create directory: ")+m_outputbasedir.GetPathWithSep());
			}
			if (m_output == wxT("")) {
				wxString output(root.getPropVal("file").c_str(), wxConvUTF8);
				MakeVariable(output);
				m_output = output;
			}

			if (m_output != wxT("")) {
				if (!m_output.IsAbsolute())
					m_output.MakeAbsolute(m_outputbasedir.GetPathWithSep());
				m_output.SetExt(wxT(""));
				if (m_output.GetName().EndsWith(wxT(".common"))) {
					wxString temp = m_output.GetName();
					temp.Replace(wxT(".common"), wxT(""), true);
					m_output.SetName(temp);
				}

				wxFileName temp = m_output;
				temp.SetExt(wxT("common.ovl"));
				if ((m_dryrun) || (m_mode == MODE_BAKE)) {
					if (!CanBeWrittenTo(temp)) {
						wxLogWarning(wxString::Format(_("File '%s' cannot be written to."), temp.GetFullPath().c_str()));
					}
				} else {
					if (!CanBeWrittenTo(temp)) {
						throw RCT3Exception(wxString::Format(_("File '%s' cannot be written to"), temp.GetFullPath().c_str()));
					}
					if (!EnsureDir(temp))
						throw RCT3Exception(_("Failed to create directory for file: ")+temp.GetFullPath());
				}

				if (temp.FileExists()) {
					m_modifiedfiles.push_back(m_output);
				} else {
					m_newfiles.push_back(m_output);
				}

				if ((!m_dryrun) && (m_mode != MODE_BAKE)) {
	#ifdef UNICODE
					m_ovl.Init(m_output.GetFullPath().mb_str(wxConvFile).data());
	#else
					m_ovl.Init(m_output.GetFullPath().fn_str());
	#endif
				}
				wxLogMessage(_("Writing ovl from raw: ")+m_output.GetFullPath()+wxT(".common.ovl"));
			} else {
				subonly = true;
				wxLogMessage(_("Parsing rawovl context. Base output dir: ")+m_outputbasedir.GetPathWithSep());
			}

			// Start parsing
			Parse(root);

			if ((!subonly) && (!m_dryrun) && (m_mode != MODE_BAKE)) {
				wxLogVerbose(wxString::Format(_("Saving OVL '%s'..."), m_output.GetFullPath().c_str()));
				m_ovl.Save();
				wxLogMessage(_("...Success!"));
			}
		} else {
			throw MakeNodeException<RCT3Exception>("cRawParser::Load, wrong root", root);
		}
	} catch (WXException& e) {
		if ((!boost::get_error_info<wxe_file>(e)) && (m_input != ""))
			e << wxe_file(m_input.GetFullPath());
		throw;		
	}
}

void cRawParser::LoadVariables(const wxFSFileName& fn, bool command, cXmlNode* target) {
/*
    wxXmlDocument doc;
    wxFileSystem fs;
    std::auto_ptr<wxFSFile> inputfsfile(fs.OpenFile(fn.GetFullPath(), wxFS_READ | wxFS_SEEKABLE));
    if (!inputfsfile.get())
        throw RCT3Exception(_("Cannot read input file ")+fn.GetFullPath());

    doc.Load(*inputfsfile->GetStream());
*/
    cXmlDoc doc(fn.GetFullPath().mb_str(wxConvUTF8), NULL, XML_PARSE_DTDLOAD);
    if (!doc) {
        throw RCT3Exception(_("Cannot read input file ")+fn.GetFullPath());
    }
    cXmlNode root(doc.root());

    if (root(RAWXML_VARIABLES)) {
        ParseVariables(root, command, fn.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME));
        if (target && target->ok() && (m_mode == MODE_BAKE) && (m_bake.Index(wxT(RAWXML_VARIABLES)) != wxNOT_FOUND)) {
            cXmlNode newvars(root.children());
            if (newvars) {
                newvars.detach();
                cXmlNode oldvars(target->children());
                if (oldvars)
                    oldvars.detach();
                target->appendChildren(newvars);
                if (oldvars) {
                    target->appendChildren(oldvars);
                }
            }
        }
    } else {
        throw MakeNodeException<RCT3Exception>(wxT("cRawParser::LoadVariables, wrong root"), root) << wxe_file(fn.GetFullPath());
    }
}

