///////////////////////////////////////////////////////////////////////////////
//
// RCT3 Importer TNG
// Next generation RCT3 Importer
// Copyright (C) 2008 Tobias Minch
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 3
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, <http://www.gnu.org/licenses/>.
//
// Written by
//   Tobias Minich - belgabor@gmx.de
//
///////////////////////////////////////////////////////////////////////////////

#include "validatormain.h"

#include "validatorapp.h"

#include "validator_abbreviation.h"
#include "validator_keyframes.h"

#include <boost/tokenizer.hpp>
#include <wx/aboutdlg.h>
#include <wx/file.h>
#include <wx/textfile.h>

#include "cXmlDoc.h"
#include "cXmlNode.h"
#include "cXmlInputOutputCallbackString.h"
#include "cXmlValidatorIsoSchematron.h"
#include "cXmlValidatorMulti.h"
#include "cXmlValidatorRNVRelaxNG.h"
#include "cXmlValidatorRelaxNG.h"
#include "cXmlValidatorSchematron.h"
#include "wxutilityfunctions.h"

#include "fileselectorcombowrap.h"
#include "pretty.h"
#include "version_validator.h"

#include "rng/rct3xml-ovlcompiler-v1.rngi.gz.h"
#include "rng/rct3xml-raw-v1.rngi.gz.h"
#include "rng/rct3xml-scenery-v1.rngi.gz.h"
#include "rng/model.rngi.gz.h"
#include "rng/ms3d_comment.rngi.gz.h"

#define wxStyledTextEventHandler(func) \
    (wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxStyledTextEventFunction, &func)

using namespace pretty;
using namespace std;
using namespace xmlcpp;

frmMain::frmMain(wxWindow* parent):rcfrmMain(parent) {
    cXmlInputOutputCallbackString::Init();
    XMLCPP_RES_ADD(rct3xml_ovlcompiler_v1, rngi);
    XMLCPP_RES_ADD(rct3xml_raw_v1, rngi);
    XMLCPP_RES_ADD(rct3xml_scenery_v1, rngi);
    XMLCPP_RES_ADD(model, rngi);
    XMLCPP_RES_ADD(ms3d_comment, rngi);

    m_fdlgSchema.reset(new wxFileDialog(this, _("Select schema for validaton"), wxT(""), wxT(""),
        _("All supported schema files|*.xml;*.rnc;*.rng;*.srng;*.shrng;*.sch|RelaxNG|*.rnc;*.rng;*.srng|"
          "Short Hand RelaxNG|*.srng;*.shrng|Schematron|*.sch|Examplotron|*.xml|All files|*.*")), wxWindowDestroyer);
    m_fdlgFile.reset(new wxFileDialog(this, _("Select xml file to validate"), wxT(""), wxT(""),
        _("XML files|*.xml;*.modxml|Plain XML files|*.xml|Model XML files|*.modxml|All files|*.*")), wxWindowDestroyer);
    m_fsSchema->Assign(m_fdlgSchema.get());

    m_kf.reset(new dlgKeyframes(this, m_stcMain), wxWindowDestroyer);

    InitXMLSTC(m_stcMain);
    InitXMLSTC(m_stcSchema);
    InitXMLSTC(m_stcRNG);
    InitCommonSTC(m_stcRNC);
    InitXMLSTC(m_stcSchematron);

    m_stcRNC->SetReadOnly(true);
    m_stcRNG->SetReadOnly(true);
    m_stcSchematron->SetReadOnly(true);

    m_schemachanged = true;
    m_internal = INT_UNDEFINED;

	this->Connect( m_stcMain->GetId(), wxEVT_STC_CHARADDED, wxStyledTextEventHandler(frmMain::OnSTCCharAdded) );
	this->Connect( m_stcMain->GetId(), wxEVT_STC_SAVEPOINTREACHED, wxStyledTextEventHandler(frmMain::OnSTCSavePoint) );
	this->Connect( m_stcMain->GetId(), wxEVT_STC_SAVEPOINTLEFT, wxStyledTextEventHandler(frmMain::OnSTCSavePoint) );
	//this->Connect( m_stcMain->GetId(), wxEVT_KEY_DOWN, wxKeyEventHandler(frmMain::OnSTCKeyDown) );
	this->Connect( m_stcSchema->GetId(), wxEVT_STC_CHARADDED, wxStyledTextEventHandler(frmMain::OnSTCCharAdded) );
	this->Connect( m_stcSchema->GetId(), wxEVT_STC_CHANGE, wxStyledTextEventHandler(frmMain::OnSTCChange) );
    this->Connect( m_fsSchema->GetId(), wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( frmMain::OnSchema ) );

/*
    m_mgr.SetManagedWindow(this);

    m_nb = new wxAuiNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
        wxAUI_NB_TOP | wxAUI_NB_TAB_MOVE | wxAUI_NB_TAB_EXTERNAL_MOVE | wxAUI_NB_SCROLL_BUTTONS | wxAUI_NB_CLOSE_ON_ACTIVE_TAB);
    m_mgr.AddPane(m_nb, wxAuiPaneInfo().Name(wxT("main")).
                                          CenterPane());
    impViewTree* pan = new impViewTree(this);
    impRawovlFileManager::getManager().registerView(*pan);
    m_mgr.AddPane(pan, wxAuiPaneInfo().Name(wxT("tree")).
                                          Caption(_("Project Tree")).
                                          Layer(0).
                                          Left().
                                          CaptionVisible(true).
                                          CloseButton(false).
                                          PaneBorder(true));

    cLogListCtrl* log = new cLogListCtrl(this, wxID_ANY);
    m_mgr.AddPane(log, wxAuiPaneInfo().Name(wxT("log")).
                                          Caption(_("Messages")).
                                          Layer(0).
                                          Bottom().
                                          CaptionVisible(true).
                                          CloseButton(false).
                                          PaneBorder(true));

    SetSize(READ_APP_SIZE("Main", GetSize()));
    wxString view = READ_APP_VIEW();
    if (!view.IsEmpty())
        m_mgr.LoadPerspective(view, false);
    m_mgr.Update();

    this->Connect(wxEVT_MENU_OPEN, wxMenuEventHandler(frmMain::OnMenuOpen));

    impRawovlFileManager::getManager().dirtyChange.connect(sigc::mem_fun(*this, &frmMain::dirtyUpdate));
    impRawovlFileManager::getManager().getNotebook.connect(sigc::mem_fun(*this, &frmMain::getNotebook));
    getProjectFileName.connect(sigc::mem_fun(impRawovlFileManager::getManager(), &impRawovlFileManager::getProjectFileName));
    prepareClose.connect(sigc::mem_fun(impRawovlFileManager::getManager(), &impRawovlFileManager::prepareReset));
    undo.connect(sigc::mem_fun(impRawovlFileManager::getManager(), &impRawovlFileManager::undo));
    canUndo.connect(sigc::mem_fun(impRawovlFileManager::getManager(), &impRawovlFileManager::canUndo));
    redo.connect(sigc::mem_fun(impRawovlFileManager::getManager(), &impRawovlFileManager::redo));
    canRedo.connect(sigc::mem_fun(impRawovlFileManager::getManager(), &impRawovlFileManager::canRedo));
*/
}

void frmMain::InitXMLSTC(wxStyledTextCtrl* ctl) {
    ctl->SetLexer(wxSTC_LEX_XML);
    ctl->SetIndentationGuides(true);
    InitCommonSTC(ctl);

    ctl->MarkerDefine (wxSTC_MARKNUM_FOLDER,        wxSTC_MARK_BOXPLUS, _T("WHITE"), _T("BLACK"));
    ctl->MarkerDefine (wxSTC_MARKNUM_FOLDEROPEN,    wxSTC_MARK_BOXMINUS, _T("WHITE"), _T("BLACK"));
    ctl->MarkerDefine (wxSTC_MARKNUM_FOLDERSUB,     wxSTC_MARK_VLINE,     _T("BLACK"), _T("BLACK"));
    ctl->MarkerDefine (wxSTC_MARKNUM_FOLDEREND,     wxSTC_MARK_BOXPLUSCONNECTED, _T("WHITE"), _T("BLACK"));
    ctl->MarkerDefine (wxSTC_MARKNUM_FOLDEROPENMID, wxSTC_MARK_BOXMINUSCONNECTED, _T("WHITE"), _T("BLACK"));
    ctl->MarkerDefine (wxSTC_MARKNUM_FOLDERMIDTAIL, wxSTC_MARK_TCORNER,     _T("BLACK"), _T("BLACK"));
    ctl->MarkerDefine (wxSTC_MARKNUM_FOLDERTAIL,    wxSTC_MARK_LCORNER,     _T("BLACK"), _T("BLACK"));

    ctl->StyleSetForeground (wxSTC_H_TAG, wxColor(0,0,160));
    ctl->StyleSetBackground (wxSTC_H_TAG, *wxWHITE);
    ctl->StyleSetForeground (wxSTC_H_ATTRIBUTE, wxColor(160,0,0));
    ctl->StyleSetBackground (wxSTC_H_ATTRIBUTE, *wxWHITE);
    ctl->StyleSetForeground (wxSTC_H_DOUBLESTRING, wxColor(0,160,0));
    ctl->StyleSetBackground (wxSTC_H_DOUBLESTRING, *wxWHITE);
    ctl->StyleSetForeground (wxSTC_H_COMMENT, wxColor(128,128,128));
    ctl->StyleSetBackground (wxSTC_H_COMMENT, *wxWHITE);
    ctl->StyleSetItalic (wxSTC_H_COMMENT, true);

    ctl->SetMarginType (2, wxSTC_MARGIN_SYMBOL);
    ctl->SetMarginMask (2, wxSTC_MASK_FOLDERS);
    ctl->StyleSetBackground (2, *wxWHITE);
    ctl->SetMarginWidth (2, 15);
    ctl->SetMarginSensitive (2, true);

    ctl->SetProperty (_T("fold"), _T("1"));
    ctl->SetProperty (_T("fold.comment"), _T("1"));
    ctl->SetProperty (_T("fold.compact"), _T("1"));
    ctl->SetProperty (_T("fold.preprocessor"), _T("1"));
    ctl->SetProperty (_T("fold.html"), _T("1"));
    ctl->SetProperty (_T("fold.html.preprocessor"), _T("1"));
    ctl->SetFoldFlags (wxSTC_FOLDFLAG_LINEBEFORE_CONTRACTED |
                  wxSTC_FOLDFLAG_LINEAFTER_CONTRACTED);

	this->Connect( ctl->GetId(), wxEVT_STC_MARGINCLICK, (wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxStyledTextEventFunction, &frmMain::OnSTCMarginClick) );

}

void frmMain::InitCommonSTC(wxStyledTextCtrl* ctl) {
    ctl->SetTabWidth (4);
    ctl->SetUseTabs (true);
    ctl->SetTabIndents (true);
    ctl->SetBackSpaceUnIndents (true);

    ctl->MarkerDefine (0, wxSTC_MARK_DOTDOTDOT, _T("BLACK"), _T("BLACK"));
    ctl->MarkerDefine (1, wxSTC_MARK_CIRCLE, _T("BLACK"), _T("YELLOW"));
    ctl->MarkerDefine (2, wxSTC_MARK_CIRCLE, _T("BLACK"), _T("RED"));
    ctl->MarkerDefine (3, wxSTC_MARK_CIRCLE, _T("BLACK"), _T("BLACK"));

    wxFont font (8, wxMODERN, wxNORMAL, wxNORMAL);
    ctl->StyleSetFont (wxSTC_STYLE_DEFAULT, font);
    ctl->StyleSetForeground (wxSTC_STYLE_DEFAULT, *wxBLACK);
    ctl->StyleSetBackground (wxSTC_STYLE_DEFAULT, *wxWHITE);
    ctl->StyleSetForeground (wxSTC_STYLE_LINENUMBER, wxColour (_T("DARK GREY")));
    ctl->StyleSetBackground (wxSTC_STYLE_LINENUMBER, *wxWHITE);
    ctl->StyleSetForeground(wxSTC_STYLE_INDENTGUIDE, wxColour (_T("DARK GREY")));

    ctl->SetMarginType (0, wxSTC_MARGIN_NUMBER);
    ctl->StyleSetForeground (wxSTC_STYLE_LINENUMBER, wxColour (_T("DARK GREY")));
    ctl->StyleSetBackground (wxSTC_STYLE_LINENUMBER, *wxWHITE);
    ctl->SetMarginWidth (0, 40); // start out not visible

    ctl->SetMarginType (1, wxSTC_MARGIN_SYMBOL);
    ctl->SetMarginWidth (1, 15);
    ctl->SetMarginSensitive (1, true);

}

frmMain::~frmMain() {
/*
    this->Disconnect(wxEVT_MENU_OPEN, wxMenuEventHandler(frmMain::OnMenuOpen));
    WRITE_APP_SIZE("Main", GetSize());
//    m_mgr->destroy_layout();
    WRITE_APP_VIEW(m_mgr.SavePerspective());
    m_mgr.UnInit();
*/
}

void frmMain::OnSTCMarginClick (wxStyledTextEvent &event) {
    wxStyledTextCtrl* sender = dynamic_cast<wxStyledTextCtrl*>(event.GetEventObject());
    if (sender) {
        if (event.GetMargin() == 2) {
            int lineClick = m_stcMain->LineFromPosition (event.GetPosition());
            int levelClick = m_stcMain->GetFoldLevel (lineClick);
            if ((levelClick & wxSTC_FOLDLEVELHEADERFLAG) > 0) {
                sender->ToggleFold (lineClick);
            }
        }
    }
}

/** @brief OnSTCCharAdded
  *
  * @todo: document this function
  */
void frmMain::OnSTCCharAdded(wxStyledTextEvent &event) {
    wxStyledTextCtrl* sender = dynamic_cast<wxStyledTextCtrl*>(event.GetEventObject());
    if (sender) {
        char chr = (char)event.GetKey();
        int currentLine = sender->GetCurrentLine();
        if (chr == '\n') {
            int lineInd = 0;
            if (currentLine > 0) {
                lineInd = sender->GetLineIndentation(currentLine - 1);
            }
            if (lineInd == 0) return;
            sender->SetLineIndentation (currentLine, lineInd);
            if (sender->GetUseTabs())
                lineInd /= sender->GetTabWidth();
            sender->GotoPos(sender->PositionFromLine (currentLine) + lineInd);
        }
    }
}

/** @brief OnSTCKey
  *
  * @todo: document this function
  *
void frmMain::OnSTCKeyDown(wxKeyEvent &event) {
    m_lbResults->Append(wxString::Format(_("Key %d"), event.GetKeyCode()));

}
*/

/** @brief OnMenuNew
  *
  * @todo: document this function
  */
void frmMain::OnMenuNew(wxCommandEvent& event) {
    m_xmlfile = "";
    m_stcMain->SetText("");
    UpdateUI();
}

void frmMain::OnMenuLoad( wxCommandEvent& event ) {
    if (m_fdlgFile->ShowModal() == wxID_OK) {
        m_xmlfile = m_fdlgFile->GetPath();
        OnReloadXml(event);
    }
}

/** @brief OnMenuSaveAs
  *
  * @todo: document this function
  */
void frmMain::OnMenuSaveAs(wxCommandEvent& event) {
    boost::shared_ptr<wxFileDialog> savedlg( new wxFileDialog(this, _("Save XML file..."), wxT(""), wxT(""),
        _("XML files|*.xml;*.modxml|Plain XML files|*.xml|Model XML files|*.modxml|All files|*.*"), wxFD_SAVE|wxFD_OVERWRITE_PROMPT), wxWindowDestroyer);
    if (savedlg->ShowModal() == wxID_OK) {
        m_xmlfile = savedlg->GetPath();
        SaveFile();
        //m_stcMain->SaveFile(m_xmlfile);
    }
}

/** @brief OnMenuSave
  *
  * @todo: document this function
  */
void frmMain::OnMenuSave(wxCommandEvent& event) {
    if (!m_xmlfile.IsEmpty())
        SaveFile();
    else
        OnMenuSaveAs(event);
/*
    if (wxFileName(m_xmlfile).FileExists()) {
        ::wxRenameFile(m_xmlfile, m_xmlfile+".validatorsavebackup");
        if (!m_stcMain->SaveFile(m_xmlfile+".validatorsave")) {
            wxLogError(_("Saving Failed!"));
            return;
        }
        m_stcMain->SaveFile(m_xmlfile);
        if ((wxFileName(m_xmlfile).GetSize() == wxInvalidSize) || (wxFileName(m_xmlfile+".validatorsave").GetSize() == wxInvalidSize) ||
            (wxFileName(m_xmlfile).GetSize() != wxFileName(m_xmlfile+".validatorsave").GetSize())) {
            wxLogError(_("Saving Failed!"));
            return;
        }
        ::wxRenameFile(m_xmlfile+".validatorsavebackup", m_xmlfile+".bak");
        ::wxRemoveFile(m_xmlfile+".validatorsave");
    } else {
        m_stcMain->SaveFile(m_xmlfile);
    }
*/
}

/** @brief SaveFile
  *
  * @todo: document this function
  */
void frmMain::SaveFile() {
    if (wxFileName(m_xmlfile).FileExists()) {
        /*
        ::wxRenameFile(m_xmlfile, m_xmlfile+".validatorsavebackup");
        if (!m_stcMain->SaveFile(m_xmlfile+".validatorsave")) {
            wxLogError(_("Saving Failed!"));
            return;
        }
        m_stcMain->SaveFile(m_xmlfile);
        if ((wxFileName(m_xmlfile).GetSize() == wxInvalidSize) || (wxFileName(m_xmlfile+".validatorsave").GetSize() == wxInvalidSize) ||
            (wxFileName(m_xmlfile).GetSize() != wxFileName(m_xmlfile+".validatorsave").GetSize())) {
            wxLogError(_("Saving Failed!"));
            return;
        }
        */
        //::wxRenameFile(m_xmlfile+".validatorsavebackup", m_xmlfile+".bak");
        ::wxRenameFile(m_xmlfile, m_xmlfile+".bak");
        //::wxRemoveFile(m_xmlfile+".validatorsave");
//    } else {
//        m_stcMain->SaveFile(m_xmlfile);
    }
    wxFile savefile(m_xmlfile, wxFile::write);
    savefile.Write(m_stcMain->GetTextRaw(), strlen(m_stcMain->GetTextRaw()));
    m_stcMain->SetSavePoint();

}

void frmMain::UpdateUI() {
    wxString title("");
    if (m_stcMain->GetModify())
        title += "*";
    if (m_xmlfile.IsEmpty())
        title += _("Unnamed");
    else
        title += m_xmlfile;
    title += " - Schema Validator";
    SetTitle(title);
}

void frmMain::OnReloadXml( wxCommandEvent& event ) {
    if (!m_xmlfile.IsEmpty()) {
        //m_stcMain->LoadFile(m_xmlfile);
        wxFile loadfile(m_xmlfile, wxFile::read);
        wxFileOffset size = loadfile.SeekEnd(0);
        loadfile.Seek(0);

        boost::shared_array<char> buf(new char[size+1]);
        buf[size] = 0;
        loadfile.Read(buf.get(), size);
        m_stcMain->SetTextRaw(buf.get());
        m_stcMain->SetSavePoint();

        if (m_val)
            DoValidate();
    }
}

void frmMain::OnReloadSchema( wxCommandEvent& event ) {
    DoReloadSchema(true);
}

void frmMain::DoReloadSchema(bool validateit) {
    switch (m_internal) {
        case INT_EXTERNAL: {
            wxString val = m_fsSchema->GetValue();
            if (!val.IsEmpty()) {
                m_stcSchema->LoadFile(val);
                m_schemachanged = true;
            }
            break;
        }
        case INT_COMPILER: {
            m_fsSchema->SetValue(XMLCPP_RES_USE(rct3xml_ovlcompiler_v1, rngi));
            m_stcSchema->SetText(XMLCPP_RES_GET(rct3xml_ovlcompiler_v1, rngi));
            m_schemachanged = true;
            break;
        }
        case INT_RAW: {
            m_fsSchema->SetValue(XMLCPP_RES_USE(rct3xml_raw_v1, rngi));
            m_stcSchema->SetText(XMLCPP_RES_GET(rct3xml_raw_v1, rngi));
            m_schemachanged = true;
            break;
        }
        case INT_SCENERY: {
            m_stcSchema->SetText(XMLCPP_RES_GET(rct3xml_scenery_v1, rngi));
            m_schemachanged = true;
            break;
        }
        case INT_MODEL: {
            m_stcSchema->SetText(XMLCPP_RES_GET(model, rngi));
            m_schemachanged = true;
            break;
        }
        case INT_MS3D: {
            m_stcSchema->SetText(XMLCPP_RES_GET(ms3d_comment, rngi));
            m_schemachanged = true;
            break;
        }
    }
    if ((!m_xmlfile.IsEmpty()) && validateit)
        DoValidate();
}

/** @brief OnApplyXIncludes
  *
  * @todo: document this function
  */
void frmMain::OnApplyXIncludes(wxCommandEvent& event) {
    if (m_xmlfile.IsEmpty()) {
        wxMessageBox(_("You have no XML file loaded or not saved your current work."));
        return;
    }
    cXmlDoc doc;
    string x = static_cast<const char *>(m_stcMain->GetText().utf8_str());
    try {
        doc.read(x, m_xmlfile.utf8_str(), "UTF-8", XML_PARSE_DTDLOAD);
    } catch (exception& e) {
        wxMessageBox(e.what());
    }
    if (!doc) {
        m_lbResults->Append(_("Parsing xml failed"));
        for (vector<cXmlStructuredError>::const_iterator it = doc.getStructuredErrors().begin(); it != doc.getStructuredErrors().end(); ++it) {
            m_lbResults->Append(wxString::Format(wxT("Line %d: %s (Path: %s)"), it->line,
                wxString::FromUTF8(it->message.c_str()).c_str(), wxString::FromUTF8(it->getPath().c_str()).c_str()));
            m_stcMain->MarkerAdd(it->line-1, 0);
        }
        for (std::vector<std::string>::const_iterator it = doc.getGenericErrors().begin(); it != doc.getGenericErrors().end(); ++it)
            m_lbResults->Append(wxString::FromUTF8(it->c_str()));
        return;
    }
    int inc = doc.xInclude();
    if (inc < 0) {
        m_lbResults->Append(_("XInclude error"));
    } else {
        m_lbResults->Append(wxString::Format(_("Resolved %d XIncludes"), inc));
    }
    m_stcMain->SetText(doc.wxwrite(true));
}

/** @brief OnInsertFilename
  *
  * @todo: document this function
  */
void frmMain::OnInsertFilename(wxCommandEvent& event) {
    boost::shared_ptr<wxFileDialog> exdlg( new wxFileDialog(this,
        _("Select file ..."),
        wxT(""), wxT(""), _("All files|*.*")), wxWindowDestroyer);
    if (exdlg->ShowModal() == wxID_OK) {
        wxFileName fi(exdlg->GetPath());
        if (!m_xmlfile.IsEmpty()) {
            fi.MakeRelativeTo(wxFileName(m_xmlfile).GetPathWithSep());
        }
        m_stcMain->ReplaceSelection(fi.GetFullPath());
    }

}

/** @brief OnInsertXInclude
  *
  * @todo: document this function
  */
void frmMain::OnInsertXInclude(wxCommandEvent& event) {
    if (m_xmlfile.IsEmpty()) {
        wxMessageBox(_("You have no XML file loaded or not saved your current work."));
        return;
    }
    boost::shared_ptr<wxFileDialog> exdlg( new wxFileDialog(this,
        _("Select file(s) ..."),
        wxT(""), wxT(""), _("XML files|*.xml|All files|*.*"), wxFD_OPEN | wxFD_MULTIPLE), wxWindowDestroyer);
    if (exdlg->ShowModal() == wxID_OK) {
		wxArrayString files;
		exdlg->GetPaths(files);
		foreach(wxString &f, files) {
			wxFileName xi(f);
			xi.MakeRelativeTo(wxFileName(m_xmlfile).GetPathWithSep());
			int inspos = m_stcMain->GetCurrentPos();
			m_stcMain->InsertText(inspos, wxString::Format("<xi:include href=\"%s\" xmlns:xi=\"http://www.w3.org/2001/XInclude\" xpointer=\"element(/1)\" />", xi.GetFullPath().c_str()));
		}
    }
}

struct AbbreviationMacro {
    typedef vector<AbbreviationMacro> vec;
    int start;
    int length;
    wxString name;
    wxString def;
    wxString replaceby;
    enum {
        Mode_None,
        Mode_Absolute,
        Mode_Relative
    } getfile;

    AbbreviationMacro(int st): start(st), length(2), getfile(Mode_None) {}
    AbbreviationMacro& Finalize() {
        if (getfile == Mode_None)
            replaceby = def;
        return *this;
    }
};

/** @brief OnAbbreviationFull
  *
  * @todo: document this function
  */
void frmMain::OnAbbreviationFull(wxCommandEvent& event) {
    DoAbbreviation(false);
}

/** @brief OnAbbreviation
  *
  * @todo: document this function
  */
void frmMain::OnAbbreviation(wxCommandEvent& event) {
    DoAbbreviation(true);
}

/** @brief DoAbbreviation
  *
  * @todo: document this function
  */
void frmMain::DoAbbreviation(bool quick) {
    int currentLine = m_stcMain->GetCurrentLine();
    int lineInd = m_stcMain->GetLineIndentation(currentLine);
    wxString indentstr;
    if (m_stcMain->GetUseTabs())
        indentstr = wxString('\t', lineInd / m_stcMain->GetTabWidth());
    else
        indentstr = wxString('\t', lineInd);
    int inspos = m_stcMain->PositionFromLine(currentLine);
    wxString line = m_stcMain->GetLine(currentLine);
    int curlinpos = m_stcMain->GetCurrentPos() - inspos;
    wxString token = line.Left(curlinpos).AfterLast(' ').AfterLast('\t').AfterLast('\r').AfterLast('\n');

    ValidatorApp::mapfind abb = wxGetApp().getAbbreviations().find(token);
    if (abb != wxGetApp().getAbbreviations().end()) {
        m_stcMain->DelWordLeft();
        int newpos = m_stcMain->GetCurrentPos();
        wxString full = abb->second;
        string pr;

        AbbreviationMacro::vec macros;
        AbbreviationMacro currentmacro(0);
        bool primed = false;
        bool inside = false;
        bool indef = false;
        int pos = -1;
        int parlevel = 0;
        foreach(const wxUniChar& ch, full) {
            ++pos;
            if (inside) {
                ++currentmacro.length;
                if (ch == '(')
                    ++parlevel;
                if (ch == ')') {
                    if (parlevel) {
                        --parlevel;
                    } else {
                        inside = false;
                        macros.push_back(currentmacro.Finalize());
                        continue;
                    }
                }
                if (!indef) {
                    if (ch == ':') {
                        indef = true;
                    } else if (ch == '|') {
                        indef = true;
                        currentmacro.getfile = AbbreviationMacro::Mode_Absolute;
                    } else if (ch == ';') {
                        indef = true;
                        currentmacro.getfile = AbbreviationMacro::Mode_Relative;
                    } else {
                        currentmacro.name += ch;
                    }
                } else {
                    currentmacro.def += ch;
                }
            } else if (!primed) {
                if (ch == '$')
                    primed = true;
            } else {
                if (ch == '(') {
                    primed = false;
                    inside = true;
                    indef = false;
                    currentmacro = AbbreviationMacro(pos-1);
                } else {
                    if (ch != '$') {
                        primed = false;
                    }
                }
            }
        }

        foreach(AbbreviationMacro& m, macros) {
            if (m.getfile != AbbreviationMacro::Mode_None) {
                wxString extensions;
                if (m.def.IsEmpty())
                    extensions = _("All files|*.*");
                else
                    extensions = m.def;
                boost::shared_ptr<wxFileDialog> exdlg( new wxFileDialog(this,
                    wxString::Format(_("Select file for macro '%s'..."), m.name),
                    wxT(""), wxT(""), extensions), wxWindowDestroyer);
                if (exdlg->ShowModal() == wxID_OK) {
                    wxFileName fi(exdlg->GetPath());
                    if ((m.getfile == AbbreviationMacro::Mode_Relative) && (!m_xmlfile.IsEmpty())) {
                        fi.MakeRelativeTo(wxFileName(m_xmlfile).GetPathWithSep());
                    }
                    m.replaceby = fi.GetFullPath();
                } else {
                    return;
                }
            }
        }

        reverse_foreach(const AbbreviationMacro& m, macros) {
            m_lbResults->Append(wxString::Format(_("%d/%d %s/%s/%s"), m.start, m.length, m.name, m.def, m.replaceby));
            full.replace(m.start, m.length, m.replaceby);
        }

        boost::char_separator<char> sep("\r\n");
        typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
        tokenizer tok(string(full.utf8_str()), sep);
        bool first = true;
        foreach(const string& tline, tok) {
            if (!first) {
                pr += '\n';
                pr += indentstr.utf8_str();
            } else
                first = false;
            pr += tline;
        }
        wxString final(wxString::FromUTF8(pr.c_str()));
        int gopos = final.Find('|');
        final.Replace("|", "", true);

        m_stcMain->InsertText(newpos, final);
        if (gopos != wxNOT_FOUND) {
            newpos += gopos;
            m_stcMain->SetCurrentPos(newpos);
            m_stcMain->SetSelection(newpos, newpos);
        }
    } else {
        m_lbResults->Append(wxString::Format(_("Abbreviation '%s' not defined."), token.c_str()));
    }
}


/** @brief OnSchemaCompiler
  *
  * @todo: document this function
  */
void frmMain::OnSchemaCompiler(wxCommandEvent& event) {
    m_internal = INT_COMPILER;
    OnReloadSchema(event);
}

/** @brief OnSchemaRaw
  *
  * @todo: document this function
  */
void frmMain::OnSchemaRaw(wxCommandEvent& event) {
    m_internal = INT_RAW;
    OnReloadSchema(event);
}

/** @brief OnSchemaScenery
  *
  * @todo: document this function
  */
void frmMain::OnSchemaScenery(wxCommandEvent& event) {
    m_internal = INT_SCENERY;
    OnReloadSchema(event);
}

/** @brief OnSchemaModel
  *
  * @todo: document this function
  */
void frmMain::OnSchemaModel(wxCommandEvent& event) {
    m_internal = INT_MODEL;
    OnReloadSchema(event);
}

/** @brief OnSchemaMS3D
  *
  * @todo: document this function
  */
void frmMain::OnSchemaMS3D(wxCommandEvent& event) {
    m_internal = INT_MS3D;
    OnReloadSchema(event);
}


void frmMain::OnSTCChange (wxStyledTextEvent &event) {
    m_schemachanged = true;
}

/** @brief OnSTCSavePoint
  *
  * @todo: document this function
  */
void frmMain::OnSTCSavePoint(wxStyledTextEvent &event) {
    UpdateUI();
}

/** @brief OnMenuViewUnfoldTags
  *
  * @todo: document this function
  */
void frmMain::OnMenuViewUnfoldTags(wxCommandEvent& event) {
    for (int i = 0; i < m_stcMain->GetLineCount(); ++i) {
        int fl = m_stcMain->GetFoldLevel(i);
        if ((fl & wxSTC_FOLDLEVELHEADERFLAG) && m_stcMain->GetFoldExpanded(i)) {
            m_stcMain->ToggleFold(i);
        }
    }
}

/** @brief OnMenuViewFoldTags
  *
  * @todo: document this function
  */
void frmMain::OnMenuViewFoldTags(wxCommandEvent& event) {
    for (int i = 0; i < m_stcMain->GetLineCount(); ++i) {
        int fl = m_stcMain->GetFoldLevel(i);
        if ((fl & wxSTC_FOLDLEVELHEADERFLAG) && ((fl & wxSTC_FOLDLEVELNUMBERMASK)-wxSTC_FOLDLEVELBASE > 0)) {
            int lineMaxSubord = m_stcMain->GetLastChild(i, -1);
            m_stcMain->SetFoldExpanded(i, false);
            if (lineMaxSubord > i)
                m_stcMain->HideLines(i+1, lineMaxSubord);
        }
    }
}

/** @brief OnMenuViewToggleFold
  *
  * @todo: document this function
  */
void frmMain::OnMenuViewToggleFold(wxCommandEvent& event) {
    int cur = m_stcMain->GetCurrentLine();
    int fl = m_stcMain->GetFoldLevel(cur);
    if (fl & wxSTC_FOLDLEVELHEADERFLAG)
        m_stcMain->ToggleFold(cur);
    else
        m_stcMain->ToggleFold(m_stcMain->GetFoldParent(cur));
}


void frmMain::OnSchema( wxCommandEvent& event ) {
    m_stcSchema->ClearAll();
    m_stcSchema->LoadFile(event.GetString());
    m_schemachanged = true;
    m_internal = false;
    DoValidate();

#if 0
    cXmlValidatorMulti val;
    //cXmlValidatorRNVRelaxNG val;
    //cXmlValidatorIsoSchematron val;

    m_lbResults->Clear();
    m_stcMain->MarkerDeleteAll(-1);
    m_stcRNG->ClearAll();
    m_stcRNC->ClearAll();
    m_stcSchematron->ClearAll();

/*
    if (val.readRNG(event.GetString().utf8_str()))
        m_lbResults->Append(_("OK"));
    else {
        m_lbResults->Append(_("Kaput"));
        for (vector<cXmlStructuredError>::const_iterator it = val.getStructuredErrors().begin(); it != val.getStructuredErrors().end(); ++it) {
            m_lbResults->Append(wxString::Format(wxT("%d %d %d %d %d, %s, %s"), it->line, it->domain, it->code, it->int1, it->int2,
                wxString::FromUTF8(it->message.c_str()).c_str(), wxString::FromUTF8(it->getPath().c_str()).c_str()));
        }
        for (std::vector<std::string>::const_iterator it = val.getGenericErrors().begin(); it != val.getGenericErrors().end(); ++it)
            m_lbResults->Append(wxString::FromUTF8(it->c_str()));
    }
*/

    if (!val.read(event.GetString().utf8_str())) {
        m_lbResults->Append(_("Loading schema failed"));
        for (vector<cXmlStructuredError>::const_iterator it = val.getStructuredErrors().begin(); it != val.getStructuredErrors().end(); ++it) {
            m_lbResults->Append(wxString::Format(wxT("%d %d %d %d %d, %s, %s"), it->line, it->domain, it->code, it->int1, it->int2,
                wxString::FromUTF8(it->message.c_str()).c_str(), wxString::FromUTF8(it->getPath().c_str()).c_str()));
//            fprintf(stderr, "Line: %d\n", it->line);
//            wxLogError(wxString(it->message.c_str(), wxConvUTF8).Trim(true));
//            wxLogError(wxT("Path: ") + wxString(it->getPath().c_str(), wxConvUTF8).Trim(true));
        }
        for (std::vector<std::string>::const_iterator it = val.getGenericErrors().begin(); it != val.getGenericErrors().end(); ++it)
            m_lbResults->Append(wxString::FromUTF8(it->c_str()));
        return;
    }

    m_lbResults->Append(cXmlValidator::getTypeName(val.getType()));

/*
    if (val.rng()) {
        m_lbResults->Append(_("RNG"));
        if (val.rng().searchNsByHref("http://purl.oclc.org/dsdl/schematron")) {
            m_lbResults->Append(_("Embedded Schematron"));
        }
        m_stcRNG->SetText(val.rng().wxwrite(true));
        m_stcSchematron->SetText(val.schematron().wxwrite(true));
    } else {
        m_lbResults->Append(_("RNC"));
    }
    m_stcRNC->SetText(val.rnc());
*/

    cXmlValidatorRNVRelaxNG* valrng = dynamic_cast<cXmlValidatorRNVRelaxNG*>(val.primary().get());
    if (valrng) {
        if (valrng->rng())
            m_stcRNG->SetText(valrng->rng().wxwrite(true));
        m_stcRNC->SetText(valrng->rnc());
    }

    cXmlValidatorIsoSchematron* valsch = dynamic_cast<cXmlValidatorIsoSchematron*>(val.primary().get());
    if (!valsch)
        valsch = dynamic_cast<cXmlValidatorIsoSchematron*>(val.secondary().get());
    if (valsch)
        m_stcSchematron->SetText(valsch->schema().wxwrite(true));


    cXmlDoc doc;
    string x = static_cast<const char *>(m_stcMain->GetText().utf8_str());
    try {
        doc.read(x, "bogus", "UTF-8", XML_PARSE_DTDLOAD);
    } catch (exception& e) {
        wxMessageBox(e.what());
    }
    if (!doc) {
        m_lbResults->Append(_("Parsing xml failed"));
        for (vector<cXmlStructuredError>::const_iterator it = doc.getStructuredErrors().begin(); it != doc.getStructuredErrors().end(); ++it) {
            m_lbResults->Append(wxString::Format(wxT("%d %d %d %d %d, %s, %s"), it->line, it->domain, it->code, it->int1, it->int2,
                wxString::FromUTF8(it->message.c_str()).c_str(), wxString::FromUTF8(it->getPath().c_str()).c_str()));
            m_stcMain->MarkerAdd(it->line-1, 0);
        }
        for (std::vector<std::string>::const_iterator it = doc.getGenericErrors().begin(); it != doc.getGenericErrors().end(); ++it)
            m_lbResults->Append(wxString::FromUTF8(it->c_str()));
        return;
    }
    m_lbResults->Append(doc.root().wxns());
    if (doc.validate(val, cXmlValidator::OPT_DETERMINE_NODE_BY_XPATH)) {
        m_lbResults->Append(_("Validation failed"));
        for (vector<cXmlStructuredError>::const_iterator it = val.getStructuredErrors().begin(); it != val.getStructuredErrors().end(); ++it) {
            m_lbResults->Append(wxString::Format(wxT("%d %d %d %d %d, %s, %s"), it->line, it->domain, it->code, it->int1, it->int2,
                wxString::FromUTF8(it->message.c_str()).c_str(), wxString::FromUTF8(it->getPath().c_str()).c_str()));
            m_stcMain->MarkerAdd(it->line-1, 0);
//            fprintf(stderr, "Line: %d\n", it->line);
//            wxLogError(wxString(it->message.c_str(), wxConvUTF8).Trim(true));
//            wxLogError(wxT("Path: ") + wxString(it->getPath().c_str(), wxConvUTF8).Trim(true));
        }
        for (std::vector<std::string>::const_iterator it = val.getGenericErrors().begin(); it != val.getGenericErrors().end(); ++it)
            m_lbResults->Append(wxString::FromUTF8(it->c_str()));
        return;
    }
#endif
}

void frmMain::OnValidate( wxCommandEvent& event ) {
    DoValidate();
}

bool frmMain::DoValidate() {
    wxBusyCursor bc;

    m_statusBar->SetStatusText(_("Parsing xml..."), 0);
    cXmlDoc doc;
    string x = static_cast<const char *>(m_stcMain->GetText().utf8_str());
    try {
        doc.read(x, m_xmlfile.utf8_str(), "UTF-8", XML_PARSE_DTDLOAD);
    } catch (exception& e) {
        wxMessageBox(e.what());
    }
    if (!doc) {
        m_statusBar->SetStatusText(_("Parsing xml failed"), 0);
        for (vector<cXmlStructuredError>::const_iterator it = doc.getStructuredErrors().begin(); it != doc.getStructuredErrors().end(); ++it) {
            m_lbResults->Append(wxString::Format(wxT("Line %d, Error %d: %s (Path: %s)"), it->line, it->code,
                wxString::FromUTF8(it->message.c_str()).c_str(), wxString::FromUTF8(it->getPath().c_str()).c_str()));
            MarkLine(m_stcMain, it->line, it->level);
        }
        for (std::vector<std::string>::const_iterator it = doc.getGenericErrors().begin(); it != doc.getGenericErrors().end(); ++it)
            m_lbResults->Append(wxString::FromUTF8(it->c_str()));
        return false;
    }
    m_lbResults->Append(doc.root().wxns());

    if (m_stcSchema->GetText().IsEmpty()) {
        ValidatorApp::mapfind sch = wxGetApp().getMappings().end();
        wxString ns = doc.root().ns();
        if (!ns.IsEmpty()) {
            sch = wxGetApp().getMappings().find("ns:" + ns);
        }
        if (sch == wxGetApp().getMappings().end()) {
            sch = wxGetApp().getMappings().find("root:" + doc.root().wxname());
        }
        if (sch == wxGetApp().getMappings().end()) {
            m_statusBar->SetStatusText(_("No schema selected..."), 0);
            return false;
        }
        if (sch->second.StartsWith("??")) {
            if (sch->second == "??Scenery")
                m_internal = INT_SCENERY;
            else if (sch->second == "??Raw")
                m_internal = INT_RAW;
            else if (sch->second == "??Compiler")
                m_internal = INT_COMPILER;
            else if (sch->second == "??Model")
                m_internal = INT_MODEL;
            else if (sch->second == "??MS3D")
                m_internal = INT_MS3D;
            else {
                m_statusBar->SetStatusText(_("Unknown internal mapping: ")+sch->second, 0);
                return false;
            }
        } else {
            m_internal = INT_EXTERNAL;
            m_fsSchema->SetValue(sch->second);
        }
        DoReloadSchema(false);
    }
    m_statusBar->SetStatusText(_("Parsing schema..."), 0);
    try {
    if (m_schemachanged)
        m_statusBar->SetStatusText(_("Undetermined"), 1);
    string schema(m_stcSchema->GetText().utf8_str());

    m_lbResults->Clear();
    m_stcMain->MarkerDeleteAll(-1);
    m_stcRNG->ClearAll();
    m_stcRNC->ClearAll();
    m_stcSchematron->ClearAll();
    cXmlErrorHandler::clearGlobalGenericErrors();
    cXmlErrorHandler::clearGlobalStructuredErrors();
    m_val.resetErrors();

    if (m_schemachanged) {
        m_stcSchema->MarkerDeleteAll(-1);
        wxString val = m_fsSchema->GetValue();
        if (!m_val.read(schema, val.IsEmpty()?NULL:static_cast<const char*>(val.utf8_str()))) {
            m_statusBar->SetStatusText(_("Loading schema failed"), 0);
            DisplayRelaxNG();
            DisplaySchematron();
            /*
            for (vector<cXmlStructuredError>::const_iterator it = m_val.getStructuredErrors().begin(); it != m_val.getStructuredErrors().end(); ++it) {
                m_lbResults->Append(wxString::Format(wxT("Line %d: %s (Path: %s)"), it->line,
                    wxString::FromUTF8(it->message.c_str()).c_str(), wxString::FromUTF8(it->getPath().c_str()).c_str()));
                m_stcSchema->MarkerAdd(it->line-1, 1);
            }
            for (std::vector<std::string>::const_iterator it = m_val.getGenericErrors().begin(); it != m_val.getGenericErrors().end(); ++it)
                m_lbResults->Append(wxString::FromUTF8(it->c_str()));
            */
            return false;
        }
    }

    m_statusBar->SetStatusText(cXmlValidator::getTypeName(m_val.getType()), 1);

    m_schemachanged = false;

    DisplayRelaxNG();
    DisplaySchematron();
/*
    cXmlValidatorRNVRelaxNG* valrng = dynamic_cast<cXmlValidatorRNVRelaxNG*>(m_val.primary().get());
    if (valrng) {
        if (valrng->rng()) {
            m_stcRNG->SetReadOnly(false);
            m_stcRNG->SetText(valrng->rng().wxwrite(true));
            m_stcRNG->SetReadOnly(true);
        }
        m_stcRNC->SetReadOnly(false);
        m_stcRNC->SetText(valrng->rnc());
        m_stcRNC->SetReadOnly(true);
    }
*/



    m_statusBar->SetStatusText(_("Validating..."), 0);
    if (doc.validate(m_val, cXmlValidator::OPT_DETERMINE_NODE_BY_XPATH, cXmlValidatorResult::VR_NONE)) {
        m_statusBar->SetStatusText(_("Validation failed"), 0);
        for (vector<cXmlStructuredError>::const_iterator it = m_val.getStructuredErrors().begin(); it != m_val.getStructuredErrors().end(); ++it) {
            m_lbResults->Append(wxString::Format(wxT("Line %d, Error %d: %s (Path: %s)"), it->line, it->code,
                wxString::FromUTF8(it->message.c_str()).c_str(), wxString::FromUTF8(it->getPath().c_str()).c_str()));
            MarkLine(m_stcMain, it->line, it->level);
        }
        for (std::vector<std::string>::const_iterator it = m_val.getGenericErrors().begin(); it != m_val.getGenericErrors().end(); ++it)
            m_lbResults->Append(wxString::FromUTF8(it->c_str()));
        return true;
    }
    m_statusBar->SetStatusText(wxT("OK!"), 0);
    return true;
    } catch (exception& e) {
        wxMessageBox(e.what());
        return false;
    }
}

/** @brief MarkLine
  *
  * @todo: document this function
  */
void frmMain::MarkLine(wxStyledTextCtrl* ctl, int line, int level) {
    if (level == XML_ERR_NONE) {
        ctl->MarkerAdd(line-1, 0);
    } else if (level == XML_ERR_WARNING) {
        ctl->MarkerAdd(line-1, 1);
    } else if (level == XML_ERR_FATAL) {
        ctl->MarkerAdd(line-1, 3);
    } else {
        ctl->MarkerAdd(line-1, 2);
    }

}

/** @brief DisplayRelaxNG
  *
  * @todo: document this function
  */
void frmMain::DisplayRelaxNG() {
    cXmlValidatorRNVRelaxNG* valrng = dynamic_cast<cXmlValidatorRNVRelaxNG*>(m_val.primary().get());
    if (valrng) {
        int valerr = valrng->getErrorStage();

        if (valerr > cXmlValidatorRNVRelaxNG::ERRSTAGE_RNG_PARSE) {
            // We should have a rng (if it's not rnc)
            if (valrng->rng()) {
                m_stcRNG->SetReadOnly(false);
                m_stcRNG->SetText(valrng->rng().wxwrite(true));
                m_stcRNG->SetReadOnly(true);
            }
            if (valerr > cXmlValidatorRNVRelaxNG::ERRSTAGE_RNG_CONVERSION) {
                // We should have RNC
                m_stcRNC->SetReadOnly(false);
                m_stcRNC->SetText(valrng->rnc());
                m_stcRNC->SetReadOnly(true);

                if (valerr != cXmlValidatorRNVRelaxNG::ERRSTAGE_OK) {
                    // Error must be in RNC
                    for (vector<cXmlStructuredError>::const_iterator it = m_val.getStructuredErrors().begin(); it != m_val.getStructuredErrors().end(); ++it) {
                        m_lbResults->Append(wxString::Format(wxT("Line %d: %s (Path: %s)"), it->line,
                            wxString::FromUTF8(it->message.c_str()).c_str(), wxString::FromUTF8(it->getPath().c_str()).c_str()));
                        MarkLine(m_stcRNC, it->line, it->level);
                    }
                }

            } else {
                // Error in rng
                for (vector<cXmlStructuredError>::const_iterator it = m_val.getStructuredErrors().begin(); it != m_val.getStructuredErrors().end(); ++it) {
                    m_lbResults->Append(wxString::Format(wxT("Line %d: %s (Path: %s)"), it->line,
                        wxString::FromUTF8(it->message.c_str()).c_str(), wxString::FromUTF8(it->getPath().c_str()).c_str()));
                    MarkLine(m_stcRNG, it->line, it->level);
                }
            }
        } else {
            // Error in schema file
            for (vector<cXmlStructuredError>::const_iterator it = m_val.getStructuredErrors().begin(); it != m_val.getStructuredErrors().end(); ++it) {
                m_lbResults->Append(wxString::Format(wxT("Line %d: %s (Path: %s)"), it->line,
                    wxString::FromUTF8(it->message.c_str()).c_str(), wxString::FromUTF8(it->getPath().c_str()).c_str()));
                MarkLine(m_stcSchema, it->line, it->level);
            }
        }

        if (valerr != cXmlValidatorRNVRelaxNG::ERRSTAGE_OK) {
            for (std::vector<std::string>::const_iterator it = m_val.getGenericErrors().begin(); it != m_val.getGenericErrors().end(); ++it)
                m_lbResults->Append(wxString::FromUTF8(it->c_str()));
        }
    }
}

/** @brief DisplaySchematron
  *
  * @todo: document this function
  */
void frmMain::DisplaySchematron() {
    cXmlValidatorIsoSchematron* valsch = dynamic_cast<cXmlValidatorIsoSchematron*>(m_val.primary().get());
    if (!valsch)
        valsch = dynamic_cast<cXmlValidatorIsoSchematron*>(m_val.secondary().get());
    if (valsch) {
        foreach(const std::string& ge, valsch->getGenericErrors())
            m_lbResults->Append(wxString::FromUTF8(ge.c_str()));
        foreach(const cXmlStructuredError& se, valsch->getStructuredErrors()) {
            m_lbResults->Append(wxString::Format(wxT("Line %d: %s (Path: %s)"), se.line,
                wxString::FromUTF8(se.message.c_str()).c_str(), wxString::FromUTF8(se.getPath().c_str()).c_str()));
            //m_stcSchema->MarkerAdd(it->line-1, 1);
        }

        foreach(const std::string& ge, cXmlErrorHandler::getGlobalGenericErrors())
            m_lbResults->Append(wxString::FromUTF8(ge.c_str()));
/*
        foreach(const cXmlStructuredError& se, cXmlErrorHandler::getGlobalStructuredErrors()) {
            m_lbResults->Append(wxString::Format(wxT("Line %d: %s (Path: %s)"), se.line,
                wxString::FromUTF8(se.message.c_str()).c_str(), wxString::FromUTF8(se.getPath().c_str()).c_str()));
            //m_stcSchema->MarkerAdd(it->line-1, 1);
        }
*/
        m_stcSchematron->SetReadOnly(false);
        m_stcSchematron->SetText(valsch->schema().wxwrite(true));
//        m_stcSchematron->SetText(valsch->transform().getDoc().wxwrite(true));
        m_stcSchematron->SetReadOnly(true);
    }
}

/** @brief OnSchematron
  *
  * @todo: document this function
  */
void frmMain::OnSchematron(wxCommandEvent& event) {
#ifdef XMLCPP_USE_SCHEMATRON_PATCHED_LIBXML
    string schema(m_stcSchema->GetText().utf8_str());
    m_lbResults->Clear();
    m_lbResults->Append(_("Validating with stock Schematron validator"));

    cXmlValidatorSchematron val(schema);

    if (!val) {
        m_lbResults->Append(_("Loading schema failed"));
        foreach(const cXmlStructuredError& se, val.getStructuredErrors()) {
            m_lbResults->Append(wxString::Format(wxT("Line %d: %s (Path: %s)"), se.line,
                wxString::FromUTF8(se.message.c_str()).c_str(), wxString::FromUTF8(se.getPath().c_str()).c_str()));
        }
        foreach(const std::string& ge, val.getGenericErrors())
            m_lbResults->Append(wxString::FromUTF8(ge.c_str()));
/*
        for (vector<cXmlStructuredError>::const_iterator it = m_val.getStructuredErrors().begin(); it != m_val.getStructuredErrors().end(); ++it) {
            m_lbResults->Append(wxString::Format(wxT("Line %d: %s (Path: %s)"), it->line,
                wxString::FromUTF8(it->message.c_str()).c_str(), wxString::FromUTF8(it->getPath().c_str()).c_str()));
        }
        for (std::vector<std::string>::const_iterator it = m_val.getGenericErrors().begin(); it != m_val.getGenericErrors().end(); ++it)
            m_lbResults->Append(wxString::FromUTF8(it->c_str()));
*/
        return;
    }

    cXmlDoc doc;
    string x = static_cast<const char *>(m_stcMain->GetText().utf8_str());
    try {
        doc.read(x, NULL, "UTF-8", XML_PARSE_DTDLOAD);
    } catch (exception& e) {
        wxMessageBox(e.what());
    }
    if (!doc) {
        m_lbResults->Append(_("Parsing xml failed"));
        for (vector<cXmlStructuredError>::const_iterator it = doc.getStructuredErrors().begin(); it != doc.getStructuredErrors().end(); ++it) {
            m_lbResults->Append(wxString::Format(wxT("Line %d: %s (Path: %s)"), it->line,
                wxString::FromUTF8(it->message.c_str()).c_str(), wxString::FromUTF8(it->getPath().c_str()).c_str()));
            m_stcMain->MarkerAdd(it->line-1, 0);
        }
        for (std::vector<std::string>::const_iterator it = doc.getGenericErrors().begin(); it != doc.getGenericErrors().end(); ++it)
            m_lbResults->Append(wxString::FromUTF8(it->c_str()));
        return;
    }
    m_lbResults->Append(doc.root().wxns());
    if (doc.validate(val, cXmlValidator::OPT_DETERMINE_NODE_BY_XPATH)) {
        m_lbResults->Append(_("Validation failed"));
        for (vector<cXmlStructuredError>::const_iterator it = val.getStructuredErrors().begin(); it != val.getStructuredErrors().end(); ++it) {
            m_lbResults->Append(wxString::Format(wxT("Line %d: %s (Path: %s)"), it->line,
                wxString::FromUTF8(it->message.c_str()).c_str(), wxString::FromUTF8(it->getPath().c_str()).c_str()));
            if (it->level == XML_ERR_WARNING) {
                //if (!m_stcMain->MarkerGet(it->line-1))
                    m_stcMain->MarkerAdd(it->line-1, 0);
            } else if (it->level == XML_ERR_FATAL) {
                //m_stcMain->MarkerDelete(it->line-1, -1);
                m_stcMain->MarkerAdd(it->line-1, 2);
            } else {
                //if (!m_stcMain->MarkerGet(it->line-1)>2) {
                    m_stcMain->MarkerAdd(it->line-1, 1);
                //}
            }
        }
        for (std::vector<std::string>::const_iterator it = val.getGenericErrors().begin(); it != val.getGenericErrors().end(); ++it)
            m_lbResults->Append(wxString::FromUTF8(it->c_str()));
        return;
    }
    m_statusBar->SetStatusText(wxT("OK!"), 0);
#else
    ::wxMessageBox(_("Basic schematron validation not available."));
#endif
}

/** @brief OnRelaxNG
  *
  * @todo: document this function
  */
void frmMain::OnRelaxNG(wxCommandEvent& event) {
    string schema(m_stcSchema->GetText().utf8_str());
    m_lbResults->Clear();
    m_lbResults->Append(_("Validating with stock RelaxNG validator"));

    cXmlValidatorRelaxNG val(schema);

    if (!val) {
        m_lbResults->Append(_("Loading schema failed"));
        foreach(const cXmlStructuredError& se, val.getStructuredErrors()) {
            m_lbResults->Append(wxString::Format(wxT("Line %d: %s (Path: %s)"), se.line,
                wxString::FromUTF8(se.message.c_str()).c_str(), wxString::FromUTF8(se.getPath().c_str()).c_str()));
        }
        foreach(const std::string& ge, val.getGenericErrors())
            m_lbResults->Append(wxString::FromUTF8(ge.c_str()));
/*
        for (vector<cXmlStructuredError>::const_iterator it = m_val.getStructuredErrors().begin(); it != m_val.getStructuredErrors().end(); ++it) {
            m_lbResults->Append(wxString::Format(wxT("Line %d: %s (Path: %s)"), it->line,
                wxString::FromUTF8(it->message.c_str()).c_str(), wxString::FromUTF8(it->getPath().c_str()).c_str()));
        }
        for (std::vector<std::string>::const_iterator it = m_val.getGenericErrors().begin(); it != m_val.getGenericErrors().end(); ++it)
            m_lbResults->Append(wxString::FromUTF8(it->c_str()));
*/
        return;
    }

    cXmlDoc doc;
    string x = static_cast<const char *>(m_stcMain->GetText().utf8_str());
    try {
        doc.read(x, NULL, "UTF-8", XML_PARSE_DTDLOAD);
    } catch (exception& e) {
        wxMessageBox(e.what());
    }
    if (!doc) {
        m_lbResults->Append(_("Parsing xml failed"));
        for (vector<cXmlStructuredError>::const_iterator it = doc.getStructuredErrors().begin(); it != doc.getStructuredErrors().end(); ++it) {
            m_lbResults->Append(wxString::Format(wxT("Line %d: %s (Path: %s)"), it->line,
                wxString::FromUTF8(it->message.c_str()).c_str(), wxString::FromUTF8(it->getPath().c_str()).c_str()));
            m_stcMain->MarkerAdd(it->line-1, 0);
        }
        for (std::vector<std::string>::const_iterator it = doc.getGenericErrors().begin(); it != doc.getGenericErrors().end(); ++it)
            m_lbResults->Append(wxString::FromUTF8(it->c_str()));
        return;
    }
    m_lbResults->Append(doc.root().wxns());
    if (doc.validate(val, cXmlValidator::OPT_DETERMINE_NODE_BY_XPATH)) {
        m_lbResults->Append(_("Validation failed"));
        for (vector<cXmlStructuredError>::const_iterator it = val.getStructuredErrors().begin(); it != val.getStructuredErrors().end(); ++it) {
            m_lbResults->Append(wxString::Format(wxT("Line %d: %s (Path: %s)"), it->line,
                wxString::FromUTF8(it->message.c_str()).c_str(), wxString::FromUTF8(it->getPath().c_str()).c_str()));
            if (it->level == XML_ERR_WARNING) {
                //if (!m_stcMain->MarkerGet(it->line-1))
                    m_stcMain->MarkerAdd(it->line-1, 0);
            } else if (it->level == XML_ERR_FATAL) {
                //m_stcMain->MarkerDelete(it->line-1, -1);
                m_stcMain->MarkerAdd(it->line-1, 2);
            } else {
                //if (!m_stcMain->MarkerGet(it->line-1)>2) {
                    m_stcMain->MarkerAdd(it->line-1, 1);
                //}
            }
        }
        for (std::vector<std::string>::const_iterator it = val.getGenericErrors().begin(); it != val.getGenericErrors().end(); ++it)
            m_lbResults->Append(wxString::FromUTF8(it->c_str()));
        return;
    }
    m_statusBar->SetStatusText(wxT("OK!"), 0);
}

/** @brief OnSchemaProfile1000
  *
  * @todo: document this function
  */
void frmMain::OnSchemaProfile1000(wxCommandEvent& event) {
    DoProfileValidation(1000);
}

/** @brief OnSchemaProfile100
  *
  * @todo: document this function
  */
void frmMain::OnSchemaProfile100(wxCommandEvent& event) {
    DoProfileValidation(100);
}

/** @brief OnSchemaProfile10
  *
  * @todo: document this function
  */
void frmMain::OnSchemaProfile10(wxCommandEvent& event) {
    DoProfileValidation(10);
}

/** @brief DoProfileValidation
  *
  * @todo: document this function
  */
void frmMain::DoProfileValidation(int times) {
    wxWindowDisabler block;
    if (DoValidate()) {
        cXmlDoc doc;
        string x = static_cast<const char *>(m_stcMain->GetText().utf8_str());
        try {
            doc.read(x, NULL, "UTF-8", XML_PARSE_DTDLOAD);
        } catch (exception& e) {
            wxMessageBox(e.what());
        }
        if (!doc) {
            m_statusBar->SetStatusText(_("Parsing xml failed"), 0);
        }

        wxStopWatch sw;
        for (int i = 0; i < times; ++i)
            doc.validate(m_val, cXmlValidator::OPT_DETERMINE_NODE_BY_XPATH);
        m_lbResults->Append(wxString::Format(_("Profiling Result: Validating %d times took %ld ms"), times, sw.Time()));

    }
}

/** @brief OnKeyframe
  *
  * @todo: document this function
  */
void frmMain::OnKeyframe(wxCommandEvent& event) {
    m_kf->Show();
}

/** @brief OnReloadBoth
  *
  * @todo: document this function
  */
void frmMain::OnReloadBoth(wxCommandEvent& event) {
    if (!m_xmlfile.IsEmpty()) {
        m_stcMain->LoadFile(m_xmlfile);
    }
    OnReloadSchema(event);
}

/** @brief OnExtraEditAbbreviations
  *
  * @todo: document this function
  */
void frmMain::OnExtraEditAbbreviations(wxCommandEvent& event) {
    boost::shared_ptr<dlgAbbreviations> dlg(new dlgAbbreviations(this), wxWindowDestroyer);
    dlg->ShowModal();
}




/*
void frmMain::OnClose(wxCloseEvent& event) {
    if (prepareClose(event.CanVeto())) {
        Destroy();
    } else {
        event.Veto();
    }
}

void frmMain::OnMenuOpen( wxMenuEvent& event ) {
    m_menuItemUndo->Enable(canUndo());
    m_menuItemRedo->Enable(canRedo());
    event.Skip();
}

void frmMain::OnMenuAbout(wxCommandEvent& WXUNUSED(event)) {
    dlgAbout info(this);
    info.ShowModal();
}

void frmMain::OnMenuExit(wxCommandEvent& WXUNUSED(event)) {
    Close();
}

void frmMain::OnMenuNewProject( wxCommandEvent& event ) {
    impRawovlFileManager::getManager().newProject();
}

void frmMain::OnMenuOpenProject( wxCommandEvent& event ) {
    impRawovlFileManager::getManager().loadProject();
}

void frmMain::OnMenuSaveProject( wxCommandEvent& event ) {
    impRawovlFileManager::getManager().saveProject();
}

void frmMain::OnMenuSaveAsProject( wxCommandEvent& event ) {
    impRawovlFileManager::getManager().saveasProject();
}

void frmMain::OnMenuUndo(wxCommandEvent& WXUNUSED(event)) {
    undo();
}

void frmMain::OnMenuRedo(wxCommandEvent& WXUNUSED(event)) {
    redo();
}

void frmMain::OnMenuViewPackageInfo( wxCommandEvent& WXUNUSED(event) ) {
//    if (m_mgr.GetPane(wxT("info")).IsShown()) {
//        m_mgr.GetPane(wxT("info")).Show(false);
//    } else {
//        m_mgr.GetPane(wxT("info")).Show(true);
//    }
//    m_mgr.Update();
}

void frmMain::OnMenuViewPackageOptions( wxCommandEvent& WXUNUSED(event) ) {
//    if (m_mgr.GetPane(wxT("options")).IsShown()) {
//        m_mgr.GetPane(wxT("options")).Show(false);
//    } else {
//        m_mgr.GetPane(wxT("options")).Show(true);
//    }
//    m_mgr.Update();
}

void frmMain::OnMenuViewRefresh( wxCommandEvent& event ) {
    impRawovlFileManager::getManager().update(wxT("/"));
}


wxAuiNotebook* frmMain::getNotebook() {
    return m_nb;
}
*/

