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

void cRawParser::Process(const wxFileName& file, const wxFileName& outputdir, const wxFileName& output) {
    m_input = file;
    m_output = output;
    if (outputdir == wxT("")) {
        m_outputbasedir.SetPath(file.GetPathWithSep());
    } else {
        m_outputbasedir.SetPath(outputdir.GetPathWithSep());
    }
    wxXmlDocument doc;
    if (!doc.Load(file.GetFullPath()))
        throw RCT3Exception(wxT("Error loading xml file ")+file.GetFullPath());

    wxXmlNode* root = doc.GetRoot();
    Load(root);
}

void cRawParser::Process(wxXmlNode* root, const wxFileName& file, const wxFileName& outputdir, const wxFileName& output) {
    m_input = file;
    m_output = output;
    if (outputdir == wxT("")) {
        m_outputbasedir.SetPath(file.GetPathWithSep());
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
        var.Replace(wxT("$")+varname+wxT("$"), repl, false);
        return true;
    }
    return false;
}

void cRawParser::CopyBaseAttributes(const wxXmlNode* from, wxXmlNode* to) {
    if (from->HasProp(wxT("comment"))) {
        to->AddProperty(wxT("comment"), from->GetPropVal(wxT("comment"), wxT("")));
    }
    if (from->HasProp(wxT("if"))) {
        to->AddProperty(wxT("if"), from->GetPropVal(wxT("if"), wxT("")));
    }
    if (from->HasProp(wxT("ifnot"))) {
        to->AddProperty(wxT("ifnot"), from->GetPropVal(wxT("ifnot"), wxT("")));
    }
    if (from->HasProp(wxT("useprefix"))) {
        to->AddProperty(wxT("useprefix"), from->GetPropVal(wxT("useprefix"), wxT("")));
    }
}

void cRawParser::Load(wxXmlNode* root) {
    if (!root)
        throw RCT3Exception(wxT("cRawParser::Load, root is null"));
    bool subonly = false;

    if (m_bakeroot == wxT(""))
        m_bakeroot = m_input;

    if (m_mode != MODE_BAKE) {
        if (root->HasProp(wxT("conditions"))) {
            wxString cond = root->GetPropVal(wxT("conditions"), wxT(""));
            ParseConditions(cond);
        }
    }

    if ((root->GetName() == RAWXML_ROOT) || (root->GetName() == RAWXML_SUBROOT)) {
        // <rawovl|subovl file="outputfile" basedir="dir outputfile is relative to">
        {
            wxString basedir = root->GetPropVal(wxT("basedir"), wxT(""));
            if ((m_mode == MODE_INSTALL) && root->HasProp(wxT("installdir"))) {
                basedir = root->GetPropVal(wxT("installdir"), wxT(""));
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
                throw RCT3Exception(wxString::Format(_("Output directory '%s' cannot be written to."), m_outputbasedir.GetFullPath().c_str()));
            }
            if (!EnsureDir(m_outputbasedir))
                throw RCT3Exception(_("Failed to create directory: ")+m_outputbasedir.GetPathWithSep());
        }
        if (m_output == wxT("")) {
            m_output = root->GetPropVal(wxT("file"), wxT(""));
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
                    throw RCT3Exception(wxString::Format(_("File '%s' cannot be written to."), temp.GetFullPath().c_str()));
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
            wxLogMessage(_("Parsing subovl-only raw. Base output dir: ")+m_outputbasedir.GetPathWithSep());
        }

        // Start parsing
        Parse(root);

        if ((!subonly) && (!m_dryrun) && (m_mode != MODE_BAKE)) {
            m_ovl.Save();
            wxLogMessage(wxString::Format(_("OVL '%s' written successfully."), m_output.GetFullPath().c_str()));
        }
    } else {
        throw RCT3Exception(wxT("cRawParser::Load, wrong root"));
    }

}

void cRawParser::LoadVariables(const wxFileName& fn, bool command, wxXmlNode* target) {
    wxXmlDocument doc;
    doc.Load(fn.GetFullPath());

    wxXmlNode* root = doc.GetRoot();

    if (root->GetName() == RAWXML_VARIABLES) {
        ParseVariables(root, command, fn.GetPathWithSep());
        if (target && (m_mode == MODE_BAKE) && (m_bake.Index(RAWXML_VARIABLES) != wxNOT_FOUND)) {
            wxXmlNode* newvars = root->GetChildren();
            if (newvars) {
                root->SetChildren(NULL);
                wxXmlNode* oldvars = target->GetChildren();
                if (oldvars)
                    oldvars->SetParent(NULL);
                target->SetChildren(newvars);
                if (oldvars) {
                    wxXmlNode* child = newvars;
                    wxXmlNode* lastchild = newvars;
                    while (child) {
                        child = child->GetNext();
                        if (child)
                            lastchild = child;
                    }
                    lastchild->SetNext(oldvars);
                }
            }
        }
    } else {
        throw RCT3Exception(wxT("cRawParser::LoadVariables, wrong root"));
    }
}
