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

#include <wx/aboutdlg.h>

#include "cXmlDoc.h"
#include "cXmlNode.h"
#include "cXmlValidatorRNVRelaxNG.h"
#include "wxutilityfunctions.h"

#include "fileselectorcombofile.h"
#include "version.h"

using namespace std;
using namespace xmlcpp;

frmMain::frmMain(wxWindow* parent):rcfrmMain(parent) {
    m_fdlgSchema.reset(new wxFileDialog(this, _("Select schema for validaton"), wxT(""), wxT(""),
        _("All supported schema files|*.xml;*.rnc;*.rng;*.srng;*.shrng;*.sch|RelaxNG|*.rnc;*.rng;*.srng|"
          "Short Hand RelaxNG|*.srng;*.shrng|Schematron|*.sch|Examplotron|*.xml|All files|*.*")), wxWindowDestroyer);
    m_fdlgFile.reset(new wxFileDialog(this, _("Select xml file to validate"), wxT(""), wxT(""),
        _("XML files|*.xml|All files|*.*")), wxWindowDestroyer);
    m_fsSchema->Assign(m_fdlgSchema.get());

    m_stcMain->SetLexer(wxSTC_LEX_XML);
    m_stcMain->SetIndentationGuides(true);
    m_stcMain->MarkerDefine (0, wxSTC_MARK_CIRCLE, _T("RED"), _T("RED"));
    m_stcMain->MarkerDefine (wxSTC_MARKNUM_FOLDER,        wxSTC_MARK_BOXPLUS, _T("WHITE"), _T("BLACK"));
    m_stcMain->MarkerDefine (wxSTC_MARKNUM_FOLDEROPEN,    wxSTC_MARK_BOXMINUS, _T("WHITE"), _T("BLACK"));
    m_stcMain->MarkerDefine (wxSTC_MARKNUM_FOLDERSUB,     wxSTC_MARK_VLINE,     _T("BLACK"), _T("BLACK"));
    m_stcMain->MarkerDefine (wxSTC_MARKNUM_FOLDEREND,     wxSTC_MARK_BOXPLUSCONNECTED, _T("WHITE"), _T("BLACK"));
    m_stcMain->MarkerDefine (wxSTC_MARKNUM_FOLDEROPENMID, wxSTC_MARK_BOXMINUSCONNECTED, _T("WHITE"), _T("BLACK"));
    m_stcMain->MarkerDefine (wxSTC_MARKNUM_FOLDERMIDTAIL, wxSTC_MARK_TCORNER,     _T("BLACK"), _T("BLACK"));
    m_stcMain->MarkerDefine (wxSTC_MARKNUM_FOLDERTAIL,    wxSTC_MARK_LCORNER,     _T("BLACK"), _T("BLACK"));

    wxFont font (8, wxMODERN, wxNORMAL, wxNORMAL);
    m_stcMain->StyleSetFont (wxSTC_STYLE_DEFAULT, font);
    m_stcMain->StyleSetForeground (wxSTC_STYLE_DEFAULT, *wxBLACK);
    m_stcMain->StyleSetBackground (wxSTC_STYLE_DEFAULT, *wxWHITE);
    m_stcMain->StyleSetForeground (wxSTC_STYLE_LINENUMBER, wxColour (_T("DARK GREY")));
    m_stcMain->StyleSetBackground (wxSTC_STYLE_LINENUMBER, *wxWHITE);
    m_stcMain->StyleSetForeground(wxSTC_STYLE_INDENTGUIDE, wxColour (_T("DARK GREY")));

    m_stcMain->StyleSetForeground (wxSTC_H_TAG, wxColor(0,0,160));
    m_stcMain->StyleSetBackground (wxSTC_H_TAG, *wxWHITE);
    m_stcMain->StyleSetForeground (wxSTC_H_ATTRIBUTE, wxColor(160,0,0));
    m_stcMain->StyleSetBackground (wxSTC_H_ATTRIBUTE, *wxWHITE);
    m_stcMain->StyleSetForeground (wxSTC_H_DOUBLESTRING, wxColor(0,160,0));
    m_stcMain->StyleSetBackground (wxSTC_H_DOUBLESTRING, *wxWHITE);
    m_stcMain->StyleSetForeground (wxSTC_H_COMMENT, wxColor(128,128,128));
    m_stcMain->StyleSetBackground (wxSTC_H_COMMENT, *wxWHITE);
    m_stcMain->StyleSetItalic (wxSTC_H_COMMENT, true);

    m_stcMain->SetMarginType (0, wxSTC_MARGIN_NUMBER);
    m_stcMain->StyleSetForeground (wxSTC_STYLE_LINENUMBER, wxColour (_T("DARK GREY")));
    m_stcMain->StyleSetBackground (wxSTC_STYLE_LINENUMBER, *wxWHITE);
    m_stcMain->SetMarginWidth (0, 40); // start out not visible

    m_stcMain->SetMarginType (1, wxSTC_MARGIN_SYMBOL);
    m_stcMain->SetMarginWidth (1, 15);
    m_stcMain->SetMarginSensitive (1, true);

    m_stcMain->SetMarginType (2, wxSTC_MARGIN_SYMBOL);
    m_stcMain->SetMarginMask (2, wxSTC_MASK_FOLDERS);
    m_stcMain->StyleSetBackground (2, *wxWHITE);
    m_stcMain->SetMarginWidth (2, 15);
    m_stcMain->SetMarginSensitive (2, true);

    m_stcMain->SetProperty (_T("fold"), _T("1"));
    m_stcMain->SetProperty (_T("fold.comment"), _T("1"));
    m_stcMain->SetProperty (_T("fold.compact"), _T("1"));
    m_stcMain->SetProperty (_T("fold.preprocessor"), _T("1"));
    m_stcMain->SetProperty (_T("fold.html"), _T("1"));
    m_stcMain->SetProperty (_T("fold.html.preprocessor"), _T("1"));
    m_stcMain->SetFoldFlags (wxSTC_FOLDFLAG_LINEBEFORE_CONTRACTED |
                  wxSTC_FOLDFLAG_LINEAFTER_CONTRACTED);

	this->Connect( m_stcMain->GetId(), wxEVT_STC_MARGINCLICK, (wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxStyledTextEventFunction, &frmMain::OnSTCMarginClick) );
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
    if (event.GetMargin() == 2) {
        int lineClick = m_stcMain->LineFromPosition (event.GetPosition());
        int levelClick = m_stcMain->GetFoldLevel (lineClick);
        if ((levelClick & wxSTC_FOLDLEVELHEADERFLAG) > 0) {
            m_stcMain->ToggleFold (lineClick);
        }
    }
}

void frmMain::OnMenuLoad( wxCommandEvent& event ) {
    if (m_fdlgFile->ShowModal() == wxID_OK) {
        m_stcMain->LoadFile(m_fdlgFile->GetPath());
    }
}

void frmMain::OnSchema( wxCommandEvent& event ) {
    cXmlValidatorRNVRelaxNG val;

    m_lbResults->Clear();
    m_stcMain->MarkerDeleteAll(-1);

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

    if (val.rng()) {
        m_lbResults->Append(_("RNG"));
        if (val.rng().searchNsByHref("http://purl.oclc.org/dsdl/schematron")) {
            m_lbResults->Append(_("Embedded Schematron"));
        }
    } else {
        m_lbResults->Append(_("RNC"));
    }

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
    if (doc.validate(val)) {
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

