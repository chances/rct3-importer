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

#include "validator_keyframes.h"

#include <wx/aboutdlg.h>

#include "cXmlDoc.h"
#include "cXmlNode.h"
#include "cXmlValidatorIsoSchematron.h"
#include "cXmlValidatorMulti.h"
#include "cXmlValidatorRNVRelaxNG.h"
#include "cXmlValidatorRelaxNG.h"
#include "cXmlValidatorSchematron.h"
#include "wxutilityfunctions.h"

#include "fileselectorcombowrap.h"
#include "pretty.h"
#include "version.h"

#define wxStyledTextEventHandler(func) \
    (wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxStyledTextEventFunction, &func)

using namespace std;
using namespace xmlcpp;

frmMain::frmMain(wxWindow* parent):rcfrmMain(parent) {
    m_fdlgSchema.reset(new wxFileDialog(this, _("Select schema for validaton"), wxT(""), wxT(""),
        _("All supported schema files|*.xml;*.rnc;*.rng;*.srng;*.shrng;*.sch|RelaxNG|*.rnc;*.rng;*.srng|"
          "Short Hand RelaxNG|*.srng;*.shrng|Schematron|*.sch|Examplotron|*.xml|All files|*.*")), wxWindowDestroyer);
    m_fdlgFile.reset(new wxFileDialog(this, _("Select xml file to validate"), wxT(""), wxT(""),
        _("XML files|*.xml|All files|*.*")), wxWindowDestroyer);
    m_fsSchema->Assign(m_fdlgSchema.get());

    m_kf.reset(new dlgKeyframes(this, m_stcMain), wxWindowDestroyer);

    InitXMLSTC(m_stcMain);
    InitXMLSTC(m_stcSchema);
    InitXMLSTC(m_stcRNG);
    InitXMLSTC(m_stcSchematron);

    m_stcRNC->SetReadOnly(true);
    m_stcRNG->SetReadOnly(true);
    m_stcSchematron->SetReadOnly(true);

    m_schemachanged = true;

	this->Connect( m_stcMain->GetId(), wxEVT_STC_CHARADDED, wxStyledTextEventHandler(frmMain::OnSTCCharAdded) );
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
    ctl->SetTabWidth (4);
    ctl->SetUseTabs (true);
    ctl->SetTabIndents (true);
    ctl->SetBackSpaceUnIndents (true);
    ctl->SetLexer(wxSTC_LEX_XML);
    ctl->SetIndentationGuides(true);
    ctl->MarkerDefine (0, wxSTC_MARK_CIRCLE, _T("BLACK"), _T("YELLOW"));
    ctl->MarkerDefine (1, wxSTC_MARK_CIRCLE, _T("BLACK"), _T("RED"));
    ctl->MarkerDefine (2, wxSTC_MARK_CIRCLE, _T("BLACK"), _T("BLACK"));
    ctl->MarkerDefine (wxSTC_MARKNUM_FOLDER,        wxSTC_MARK_BOXPLUS, _T("WHITE"), _T("BLACK"));
    ctl->MarkerDefine (wxSTC_MARKNUM_FOLDEROPEN,    wxSTC_MARK_BOXMINUS, _T("WHITE"), _T("BLACK"));
    ctl->MarkerDefine (wxSTC_MARKNUM_FOLDERSUB,     wxSTC_MARK_VLINE,     _T("BLACK"), _T("BLACK"));
    ctl->MarkerDefine (wxSTC_MARKNUM_FOLDEREND,     wxSTC_MARK_BOXPLUSCONNECTED, _T("WHITE"), _T("BLACK"));
    ctl->MarkerDefine (wxSTC_MARKNUM_FOLDEROPENMID, wxSTC_MARK_BOXMINUSCONNECTED, _T("WHITE"), _T("BLACK"));
    ctl->MarkerDefine (wxSTC_MARKNUM_FOLDERMIDTAIL, wxSTC_MARK_TCORNER,     _T("BLACK"), _T("BLACK"));
    ctl->MarkerDefine (wxSTC_MARKNUM_FOLDERTAIL,    wxSTC_MARK_LCORNER,     _T("BLACK"), _T("BLACK"));

    wxFont font (8, wxMODERN, wxNORMAL, wxNORMAL);
    ctl->StyleSetFont (wxSTC_STYLE_DEFAULT, font);
    ctl->StyleSetForeground (wxSTC_STYLE_DEFAULT, *wxBLACK);
    ctl->StyleSetBackground (wxSTC_STYLE_DEFAULT, *wxWHITE);
    ctl->StyleSetForeground (wxSTC_STYLE_LINENUMBER, wxColour (_T("DARK GREY")));
    ctl->StyleSetBackground (wxSTC_STYLE_LINENUMBER, *wxWHITE);
    ctl->StyleSetForeground(wxSTC_STYLE_INDENTGUIDE, wxColour (_T("DARK GREY")));

    ctl->StyleSetForeground (wxSTC_H_TAG, wxColor(0,0,160));
    ctl->StyleSetBackground (wxSTC_H_TAG, *wxWHITE);
    ctl->StyleSetForeground (wxSTC_H_ATTRIBUTE, wxColor(160,0,0));
    ctl->StyleSetBackground (wxSTC_H_ATTRIBUTE, *wxWHITE);
    ctl->StyleSetForeground (wxSTC_H_DOUBLESTRING, wxColor(0,160,0));
    ctl->StyleSetBackground (wxSTC_H_DOUBLESTRING, *wxWHITE);
    ctl->StyleSetForeground (wxSTC_H_COMMENT, wxColor(128,128,128));
    ctl->StyleSetBackground (wxSTC_H_COMMENT, *wxWHITE);
    ctl->StyleSetItalic (wxSTC_H_COMMENT, true);

    ctl->SetMarginType (0, wxSTC_MARGIN_NUMBER);
    ctl->StyleSetForeground (wxSTC_STYLE_LINENUMBER, wxColour (_T("DARK GREY")));
    ctl->StyleSetBackground (wxSTC_STYLE_LINENUMBER, *wxWHITE);
    ctl->SetMarginWidth (0, 40); // start out not visible

    ctl->SetMarginType (1, wxSTC_MARGIN_SYMBOL);
    ctl->SetMarginWidth (1, 15);
    ctl->SetMarginSensitive (1, true);

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

}

/** @brief OnMenuSave
  *
  * @todo: document this function
  */
void frmMain::OnMenuSave(wxCommandEvent& event) {
    if (m_xmlfile.IsEmpty())
        OnMenuSaveAs(event);
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
}



void frmMain::OnReloadXml( wxCommandEvent& event ) {
    if (!m_xmlfile.IsEmpty()) {
        m_stcMain->LoadFile(m_xmlfile);
        if (m_val)
            DoValidate();
    }
}

void frmMain::OnReloadSchema( wxCommandEvent& event ) {
    wxString val = m_fsSchema->GetValue();
    if (!val.IsEmpty()) {
        m_stcSchema->LoadFile(val);
        m_schemachanged = true;
        if (!m_xmlfile.IsEmpty())
            DoValidate();
    }
}

void frmMain::OnSTCChange (wxStyledTextEvent &event) {
    m_schemachanged = true;
}


void frmMain::OnSchema( wxCommandEvent& event ) {
    m_stcSchema->ClearAll();
    m_stcSchema->LoadFile(event.GetString());
    m_statusBar->SetStatusText(_("Undetermined"), 1);
    m_schemachanged = true;
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
    try {
    string schema(m_stcSchema->GetText().utf8_str());

    m_lbResults->Clear();
    m_stcMain->MarkerDeleteAll(-1);
    m_stcRNG->ClearAll();
    m_stcRNC->ClearAll();
    m_stcSchematron->ClearAll();

    if (m_schemachanged) {
        m_stcSchema->MarkerDeleteAll(-1);
        if (!m_val.read(schema)) {
            m_statusBar->SetStatusText(_("Loading schema failed"), 0);
            for (vector<cXmlStructuredError>::const_iterator it = m_val.getStructuredErrors().begin(); it != m_val.getStructuredErrors().end(); ++it) {
                m_lbResults->Append(wxString::Format(wxT("Line %d: %s (Path: %s)"), it->line,
                    wxString::FromUTF8(it->message.c_str()).c_str(), wxString::FromUTF8(it->getPath().c_str()).c_str()));
                m_stcSchema->MarkerAdd(it->line-1, 1);
            }
            for (std::vector<std::string>::const_iterator it = m_val.getGenericErrors().begin(); it != m_val.getGenericErrors().end(); ++it)
                m_lbResults->Append(wxString::FromUTF8(it->c_str()));
            return false;
        }
    }

    m_statusBar->SetStatusText(cXmlValidator::getTypeName(m_val.getType()), 1);

    m_schemachanged = false;

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

    cXmlValidatorIsoSchematron* valsch = dynamic_cast<cXmlValidatorIsoSchematron*>(m_val.primary().get());
    if (!valsch)
        valsch = dynamic_cast<cXmlValidatorIsoSchematron*>(m_val.secondary().get());
    if (valsch) {
        m_stcSchematron->SetReadOnly(false);
        m_stcSchematron->SetText(valsch->schema().wxwrite(true));
        m_stcSchematron->SetReadOnly(true);
    }


    cXmlDoc doc;
    string x = static_cast<const char *>(m_stcMain->GetText().utf8_str());
    try {
        doc.read(x, NULL, "UTF-8", XML_PARSE_DTDLOAD);
    } catch (exception& e) {
        wxMessageBox(e.what());
    }
    if (!doc) {
        m_statusBar->SetStatusText(_("Parsing xml failed"), 0);
        for (vector<cXmlStructuredError>::const_iterator it = doc.getStructuredErrors().begin(); it != doc.getStructuredErrors().end(); ++it) {
            m_lbResults->Append(wxString::Format(wxT("Line %d: %s (Path: %s)"), it->line,
                wxString::FromUTF8(it->message.c_str()).c_str(), wxString::FromUTF8(it->getPath().c_str()).c_str()));
            m_stcMain->MarkerAdd(it->line-1, 1);
        }
        for (std::vector<std::string>::const_iterator it = doc.getGenericErrors().begin(); it != doc.getGenericErrors().end(); ++it)
            m_lbResults->Append(wxString::FromUTF8(it->c_str()));
        return false;
    }
    m_lbResults->Append(doc.root().wxns());
    if (doc.validate(m_val, cXmlValidator::OPT_DETERMINE_NODE_BY_XPATH)) {
        m_statusBar->SetStatusText(_("Validation failed"), 0);
        for (vector<cXmlStructuredError>::const_iterator it = m_val.getStructuredErrors().begin(); it != m_val.getStructuredErrors().end(); ++it) {
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

/** @brief OnSchematron
  *
  * @todo: document this function
  */
void frmMain::OnSchematron(wxCommandEvent& event) {
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

