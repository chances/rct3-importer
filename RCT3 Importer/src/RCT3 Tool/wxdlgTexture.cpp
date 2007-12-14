///////////////////////////////////////////////////////////////////////////////
//
// Texture Settings Dialog
// Copyright (C) 2006 Tobias Minch, Jonathan Wilson
//
// Part of rct3tool
// Copyright 2005 Jonathan Wilson
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
// Jonathan Wilson).
//
///////////////////////////////////////////////////////////////////////////////

#include "wx_pch.h"

#include "wxdlgTexture.h"

#include <math.h>

#include <wx/numdlg.h>
#include <wx/valtext.h>
#include <wx/valgen.h>

#include "auipicfiledlg.h"
#include "confhelp.h"
#include "htmlentities.h"
#include "rct3log.h"
#include "SCNFile.h"
#include "texcheck.h"
#include "valext.h"
#include "valtexture.h"
#include "wxgmagick.h"

#include "ilhelper.h"

#include "wxapp.h"

////////////////////////////////////////////////////////////////////////
//
//  wxTextureEditListBox
//
////////////////////////////////////////////////////////////////////////

wxTextureEditListBox::wxTextureEditListBox(wxWindow *parent, cFlexiTexture *content, int style):
        wxColourHtmlListBox(parent, wxID_ANY, wxDefaultPosition, wxSize(-1, -1), style | wxSUNKEN_BORDER) {
    m_contents = content;
}

////////////////////////////////////////////////////////////////////////
//
//  wxFrameListBox
//
////////////////////////////////////////////////////////////////////////

wxFrameListBox::wxFrameListBox(wxWindow *parent, cFlexiTexture *content):
        wxTextureEditListBox(parent, content, wxLB_MULTIPLE) {
    UpdateContents();
    if (m_contents) {
        SetSelection(m_contents->Frames.size()?0:wxNOT_FOUND);
    } else {
        SetSelection(wxNOT_FOUND);
    }
}

void wxFrameListBox::UpdateContents() {
    SetItemCount(m_contents->Frames.size());
    RefreshAll();
}

wxString wxFrameListBox::OnGetItem(size_t n) const {
    int rel = (int) log10(m_contents->Frames.size());
    wxString color = wxT("");
    if (m_contents->Frames.size()>1) {
        color = wxT(" color='#FF3C3C'");
        for (cFlexiTextureAnim::iterator i_anim = m_contents->Animation.begin(); i_anim != m_contents->Animation.end(); i_anim++) {
            if (i_anim->frame() == n) {
                color = wxT("");
                break;
            }
        }
    }
    wxString alpha = wxT("");
    if (m_contents->Frames[n].alphasource() != CFTF_ALPHA_NONE)
        alpha = _(" (Alpha)");
    return wxString::Format(wxT("<font size='2'%s>"), color.c_str()) +
           wxEncodeHtmlEntities(wxString::Format(wxT("%0*d: %s"), rel, n+1, m_contents->Frames[n].texture().GetName().c_str()))+
           alpha+wxT("</font>");
}

////////////////////////////////////////////////////////////////////////
//
//  wxAnimationListBox
//
////////////////////////////////////////////////////////////////////////

wxAnimationListBox::wxAnimationListBox(wxWindow *parent, cFlexiTexture *content):
        wxTextureEditListBox(parent, content) {
    UpdateContents();
    SetSelection(0);
}

void wxAnimationListBox::UpdateContents() {
    SetItemCount(m_contents->Animation.size()+1);
    RefreshAll();
}

wxString wxAnimationListBox::OnGetItem(size_t n) const {
    if (n == 0) {
        return _("<font color='#C0C0C0' size='2'>(Insert at start)</font>");
    } else {
        int rel = (int) log10(m_contents->Animation.size());
        wxString res;
        if (m_contents->Animation[n-1].frame() >= m_contents->Frames.size()) {
            res = _("<unavailable>");
        } else {
            res = m_contents->Frames[m_contents->Animation[n-1].frame()].texture().GetName();
        }
        return wxT("<font size='2'>")+wxEncodeHtmlEntities(wxString::Format(wxT("%0*d: %ldx %s (%ld)"), rel, n, m_contents->Animation[n-1].count(), res.c_str(), m_contents->Animation[n-1].frame()+1))+wxT("</font>");
    }
}

////////////////////////////////////////////////////////////////////////
//
//  dlgTextureFrame
//
////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(dlgTextureFrame,wxDialog)
EVT_BUTTON(XRCID("m_btTextureInfo"), dlgTextureFrame::OnTextureInfo)
EVT_BUTTON(XRCID("m_btAlphaInfo"), dlgTextureFrame::OnAlphaInfo)
EVT_COMBOBOX(XRCID("m_textTextureFile"), dlgTextureFrame::OnTextureOpen)
EVT_COMBOBOX(XRCID("m_textAlphaFile"), dlgTextureFrame::OnTextureOpen)
EVT_CHOICE(XRCID("m_choiceAlpha"), dlgTextureFrame::OnAlpha)
EVT_BUTTON(XRCID("m_btLoad"), dlgTextureFrame::OnLoad)
END_EVENT_TABLE()

dlgTextureFrame::dlgTextureFrame(wxWindow *parent) {
    m_size = 0;
    InitWidgetsFromXRC((wxWindow *)parent);
    SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);

    wxTextCtrl *t_text = XRCCTRL(*this,"m_textTextureFile",wxTextCtrl);

    wxAUIFileDialog *auidlg = new wxAUIPicFileDialog(
                                     this,
                                     _T("Select Texture File"),
                                     wxEmptyString,
                                     wxEmptyString,
                                     _T("Supported Image Files|*.png;*.jpg;*.bmp;*.gif;*.jpeg;*.tiff;*.tif|All Files (*.*)|*.*"),
                                     wxFD_FILE_MUST_EXIST|wxFD_CHANGE_DIR,
                                     wxDefaultPosition,
                                     wxSize(600,400)
                                 );

    m_textTextureFile = new wxFileSelectorCombo<wxFileDialogBase>(this, auidlg, &::wxGetApp().g_workdir, XRCID("m_textTextureFile"));
    t_text->GetContainingSizer()->Replace(t_text, m_textTextureFile);
    t_text->Destroy();
    t_text = XRCCTRL(*this,"m_textAlphaFile",wxTextCtrl);

    auidlg = new wxAUIPicFileDialog(
                                     this,
                                     _T("Select Alpha Texture File"),
                                     wxEmptyString,
                                     wxEmptyString,
                                     _T("Supported Image Files|*.png;*.jpg;*.bmp;*.gif;*.jpeg;*.tiff;*.tif|All Files (*.*)|*.*"),
                                     wxFD_FILE_MUST_EXIST|wxFD_CHANGE_DIR,
                                     wxDefaultPosition,
                                     wxSize(600,400)
                                 );

    m_textAlphaFile = new wxFileSelectorCombo<wxFileDialogBase>(this, auidlg, &::wxGetApp().g_workdir, XRCID("m_textAlphaFile"));
    t_text->GetContainingSizer()->Replace(t_text, m_textAlphaFile);
    t_text->Destroy();

    m_textTextureFile->SetValidator(wxTextureValidator(&m_frame.Texture, _("Texture File"), false, false, &m_size));
    m_textAlphaFile->SetValidator(wxTextureValidator(&m_frame.Alpha, _("Alpha Texture File"), true, false, &m_size));
    m_textAlphaCutoff->SetValidator(wxExtendedValidator(&m_frame.AlphaCutoff));
    m_choiceAlpha->SetValidator(wxGenericValidator((int*) &m_frame.AlphaSource));

    m_ibTextureFile = new wxInputBox(this, wxID_ANY);
    m_ibTextureFile->SetEditor(m_textTextureFile);
    m_ibAlphaFile = new wxInputBox(this, wxID_ANY);
    m_ibAlphaFile->SetEditor(m_textAlphaFile);

    m_btTextureInfo->SetBitmapLabel(wxXmlResource::Get()->LoadBitmap("documentinfo_16x16"));
    m_btAlphaInfo->SetBitmapLabel(wxXmlResource::Get()->LoadBitmap("documentinfo_16x16"));

    Fit();
    Layout();
    GetSizer()->SetSizeHints(this);


    m_btOk->SetId(wxID_OK);
    m_btCancel->SetId(wxID_CANCEL);
}

void dlgTextureFrame::SetFlexiTextureFrame(const cFlexiTextureFrame& val, unsigned int force_size) {
    m_frame = val.raw();
    m_size = force_size;
    TransferDataToWindow();
    wxCommandEvent e;
    e.SetInt(m_frame.AlphaSource);
    OnAlpha(e);
}

cFlexiTextureFrame dlgTextureFrame::GetFlexiTextureFrame() {
    return cFlexiTextureFrame(m_frame);
}

bool dlgTextureFrame::Validate() {
    if (!wxDialog::Validate())
        return false;

    if (m_textAlphaFile->GetValue() == wxT(""))
        return true;

    wxSize tex = getBitmapSize(m_textTextureFile->GetValue().fn_str());
    wxSize alph = getBitmapSize(m_textAlphaFile->GetValue().fn_str());

    if (tex.GetWidth() != alph.GetWidth()) {
        ::wxMessageBox(_("Both texture and alpha need to be the same size!"), _("Validation conflict"), wxICON_ERROR | wxOK, this);
        return false;
    }

    return true;
}

void dlgTextureFrame::OnTextureInfo(wxCommandEvent& WXUNUSED(event)) {
    if (m_textTextureFile->GetValue() == wxT("")) {
        ::wxMessageBox(_("Please select a texture file before requesting info on it ;-)"), _("Error"), wxICON_ERROR | wxOK, this);
    } else {
        showBitmapInfo(this, m_textTextureFile->GetValue().fn_str());
    }
}

void dlgTextureFrame::OnAlphaInfo(wxCommandEvent& WXUNUSED(event)) {
    if (m_textAlphaFile->GetValue() == wxT("")) {
        ::wxMessageBox(_("Please select an alpha texture file before requesting info on it ;-)"), _("Error"), wxICON_ERROR | wxOK, this);
    } else {
        showBitmapInfo(this, m_textAlphaFile->GetValue().fn_str());
    }
}

void dlgTextureFrame::OnAlpha(wxCommandEvent& event) {
    switch (event.GetInt()) {
        case CFTF_ALPHA_INTERNAL: {
            m_textAlphaFile->Enable(false);
            m_textTextureFile->SetValidator(wxTextureValidator(&m_frame.Texture, _("Texture File"), false, true));
            m_textAlphaFile->SetValidator(wxTextureValidator(&m_frame.Alpha, _("Alpha Channel File"), true));
        }
        break;
        case CFTF_ALPHA_EXTERNAL: {
            m_textAlphaFile->Enable(true);
            m_textTextureFile->SetValidator(wxTextureValidator(&m_frame.Texture, _("Texture File"), false));
            m_textAlphaFile->SetValidator(wxTextureValidator(&m_frame.Alpha, _("Alpha Channel File"), false));
        }
        break;
        default: {
            m_textAlphaFile->Enable(false);
            m_textTextureFile->SetValidator(wxTextureValidator(&m_frame.Texture, _("Texture File"), false));
            m_textAlphaFile->SetValidator(wxTextureValidator(&m_frame.Alpha, _("Alpha Channel File"), true));
        }
    }
    m_ibTextureFile->Update();
    m_ibAlphaFile->Update();
    if ((event.GetInt() == CFTF_ALPHA_EXTERNAL) && (m_textAlphaFile->GetValue() == wxT("")))
        m_textAlphaFile->OnButtonClick();
}

void dlgTextureFrame::OnTextureOpen(wxCommandEvent& event) {
    ::wxGetApp().g_workdir.AssignDir(wxFileName(event.GetString()).GetPath());
}

void dlgTextureFrame::OnLoad(wxCommandEvent& WXUNUSED(event)) {
    wxFileDialog *dialog = new wxFileDialog(
                                     this,
                                     _T("Open Scenery File (Load texture settings)"),
                                     wxEmptyString,
                                     wxEmptyString,
                                     _T("Scenery Files (*.scn)|*.scn"),
                                     wxFD_FILE_MUST_EXIST|wxFD_CHANGE_DIR,
                                     wxDefaultPosition,
                                     wxSize(600,400)
                                 );
    dialog->SetDirectory(::wxGetApp().g_workdir.GetPath());
    if (dialog->ShowModal() == wxID_OK) {
        ::wxGetApp().g_workdir.AssignDir(wxFileName(dialog->GetPath()).GetPath());
        cSCNFile *texscn = new cSCNFile(dialog->GetPath().fn_str());
        if (texscn) {
            if (texscn->flexitextures.size()==0) {
                wxMessageBox(_("The selected scenery file contains no textures."), _("Warning"), wxOK | wxICON_WARNING, this);
                delete texscn;
                dialog->Destroy();
                return;
            }
            wxArrayString choices;
            std::vector<cFlexiTextureFrame> framechoices;
            for (cFlexiTexture::iterator ms = texscn->flexitextures.begin(); ms != texscn->flexitextures.end(); ms++) {
                for (cFlexiTextureFrame::iterator it = ms->Frames.begin(); it != ms->Frames.end(); it++) {
                    choices.Add(ms->Name + wxT(", ") + it->texture().GetName());
                    framechoices.push_back(*it);
                }
            }
            if (choices.size() > 0) {
                int choice = ::wxGetSingleChoiceIndex(dialog->GetPath(), _("Choose texture frame settings to import"), choices, this);
                if (choice >= 0) {
                    SetFlexiTextureFrame(framechoices[choice], m_size);
                }
            }
            delete texscn;
        }
    }
    dialog->Destroy();
}

////////////////////////////////////////////////////////////////////////
//
//  dlgTexture
//
////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(dlgTexture,dlgTextureBase)
EVT_BUTTON(XRCID("m_btLoad"), dlgTexture::OnLoad)
EVT_TOGGLEBUTTON(XRCID("m_togglePreview"), dlgTexture::OnPreview)
// Frame Controls
EVT_LISTBOX(XRCID("m_htlbFrame"), dlgTexture::OnFrameListUpdate)
EVT_SPIN_UP(XRCID("m_spinFrame"), dlgTexture::OnFrameUp)
EVT_SPIN_DOWN(XRCID("m_spinFrame"), dlgTexture::OnFrameDown)
EVT_BUTTON(XRCID("m_btFrameAdd"), dlgTexture::OnFrameAdd)
EVT_BUTTON(XRCID("m_btFrameEdit"), dlgTexture::OnFrameEdit)
EVT_LISTBOX_DCLICK(XRCID("m_htlbFrame"), dlgTexture::OnFrameEdit)
EVT_BUTTON(XRCID("m_btFrameCopy"), dlgTexture::OnFrameCopy)
EVT_BUTTON(XRCID("m_btFrameDel"), dlgTexture::OnFrameDel)
EVT_BUTTON(XRCID("m_btFrameClear"), dlgTexture::OnFrameClear)
// Animation Controls
EVT_LISTBOX(XRCID("m_htlbAnimation"), dlgTexture::OnAnimationListUpdate)
EVT_LISTBOX_DCLICK(XRCID("m_htlbAnimation"), dlgTexture::OnAnimationListSetCount)
EVT_SPIN_UP(XRCID("m_spinAnimation"), dlgTexture::OnAnimationUp)
EVT_SPIN_DOWN(XRCID("m_spinAnimation"), dlgTexture::OnAnimationDown)
EVT_SPIN_UP(XRCID("m_spinAnimationCount"), dlgTexture::OnAnimationCountUp)
EVT_SPIN_DOWN(XRCID("m_spinAnimationCount"), dlgTexture::OnAnimationCountDown)
EVT_BUTTON(XRCID("m_btAnimationAdd"), dlgTexture::OnAnimationAdd)
//EVT_BUTTON(XRCID("m_btAnimationCopy"), dlgTexture::OnAnimationCopy)
EVT_BUTTON(XRCID("m_btAnimationDel"), dlgTexture::OnAnimationDel)
EVT_TOGGLEBUTTON(XRCID("m_toggleAnimationFreeze"), dlgTexture::OnAnimationFreeze)
EVT_BUTTON(XRCID("m_btAnimationClear"), dlgTexture::OnAnimationClear)
END_EVENT_TABLE()

dlgTexture::dlgTexture(wxWindow *parent) {
    m_size = 0;

    InitWidgetsFromXRC((wxWindow *)parent);
    SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);

    m_htlbAnimation = new wxAnimationListBox(this, &m_ft);
    wxXmlResource::Get()->AttachUnknownControl(wxT("m_htlbAnimation"), m_htlbAnimation, this);
    m_htlbFrame = new wxFrameListBox(this, &m_ft);
    m_htlbFrame->SetToolTip(_("List of texture frames.\nUnused frames are shown in red. These will not be written to the ovl."));
    wxXmlResource::Get()->AttachUnknownControl(wxT("m_htlbFrame"), m_htlbFrame, this);
    m_gxpictPreview = new wxGXPicture(this, wxID_ANY, wxT(""));
    m_gxpictPreview->SetBackgroundColour(*wxWHITE);
    m_gxpictPreview->SetScale(wxGXSCALE_UNIFORM);
    m_gxpictPreview->SetAlignment(wxALIGN_CENTER);
    wxXmlResource::Get()->AttachUnknownControl(wxT("m_gxpictPreview"), m_gxpictPreview, this);

    m_textTextureName->SetValidator(wxExtendedValidator(&m_ft.Name, false));
    m_textFramerate->SetValidator(wxExtendedValidator(&m_ft.FPS));

    wxInputBox *t_ibTextureName = new wxInputBox(this, wxID_ANY);
    t_ibTextureName->SetEditor(m_textTextureName);

    Fit();
    Layout();
    GetSizer()->SetSizeHints(this);

    m_btOk->SetId(wxID_OK);
    m_btCancel->SetId(wxID_CANCEL);

    SetSize(READ_APP_SIZE("Texture", GetSize()));
    Center();
}

dlgTexture::~dlgTexture() {
    WRITE_APP_SIZE("Texture", GetSize());
}

void dlgTexture::SetFlexiTexture(const cFlexiTexture& val) {
    m_ft = val;
    if (val.Frames.size()) {
        wxSize s = getBitmapSize(val.Frames[0].texture().GetFullPath().fn_str());
        if (s.GetWidth() > 0)
            m_size = s.GetWidth();
        else
            m_size = 0;
    } else
        m_size = 0;
    m_checkCustom1->SetValue(val.Recolorable & FTX_CHOICE_FIRST);
    m_checkCustom2->SetValue(val.Recolorable & FTX_CHOICE_SECOND);
    m_checkCustom3->SetValue(val.Recolorable & FTX_CHOICE_THIRD);
    TransferDataToWindow();
    m_htlbFrame->UpdateContents();
    m_htlbAnimation->UpdateContents();
    UpdateControlState();
}

cFlexiTexture dlgTexture::GetFlexiTexture() {
    cFlexiTexture ret = m_ft;
    ret.Recolorable = 0;
    ret.Recolorable |= (m_checkCustom1->IsChecked())?FTX_CHOICE_FIRST:0;
    ret.Recolorable |= (m_checkCustom2->IsChecked())?FTX_CHOICE_SECOND:0;
    ret.Recolorable |= (m_checkCustom3->IsChecked())?FTX_CHOICE_THIRD:0;
    if (ret.Animation.size() < 2)
        ret.FPS = 0;
    return ret;
}

void dlgTexture::UpdateControlState() {
    long count = m_ft.Frames.size();
    long sel = m_htlbFrame->GetSelection();
    long selcount = m_htlbFrame->GetSelectedCount();
    bool frozen = m_toggleAnimationFreeze->GetValue();

    m_spinFrame->Enable((count>=2) && (selcount == 1));
    m_btFrameEdit->Enable((sel>=0) && (selcount == 1));
    m_btFrameCopy->Enable((sel>=0) && (selcount == 1));
    m_btFrameDel->Enable(sel>=0);
    m_btFrameClear->Enable(count>=1);
    m_btAnimationAdd->Enable(frozen?false:(sel>=0));

    count = m_ft.Animation.size();
    sel = m_htlbAnimation->GetSelection();
    m_spinAnimation->Enable(frozen?false:((count>=2)&&(sel>0)));
    //m_btAnimationCopy->Enable(frozen?false:(sel>=0));
    m_spinAnimationCount->Enable(sel>0);
    m_btAnimationDel->Enable(frozen?false:(sel>0));
    m_toggleAnimationFreeze->Enable(count>=1);
    m_btAnimationClear->Enable(frozen?false:(count>=1));

    m_btOk->Enable(!frozen);
    m_textFramerate->Enable(count>=2);
}

void dlgTexture::UpdatePreview() {
    if (m_togglePreview->GetValue()) {
        m_gxpictPreview->SetFileName(m_previewFile);
        int sel = m_htlbFrame->GetSelection();
        if (sel >= 0) {
            m_gxpictPreview->UseAlpha(m_ft.Frames[sel].alphasource() == CFTF_ALPHA_INTERNAL);
        }

        m_gxpictPreview->Refresh();
    }
}

void dlgTexture::OnLoad(wxCommandEvent& WXUNUSED(event)) {
    wxFileDialog *dialog = new wxFileDialog(
                                     this,
                                     _T("Open Scenery File (Load texture settings)"),
                                     wxEmptyString,
                                     wxEmptyString,
                                     _T("Scenery Files (*.scn)|*.scn"),
                                     wxFD_FILE_MUST_EXIST|wxFD_CHANGE_DIR,
                                     wxDefaultPosition,
                                     wxSize(600,400)
                                 );
    dialog->SetDirectory(::wxGetApp().g_workdir.GetPath());
    if (dialog->ShowModal() == wxID_OK) {
        ::wxGetApp().g_workdir.AssignDir(wxFileName(dialog->GetPath()).GetPath());
        cSCNFile *texscn = new cSCNFile(dialog->GetPath().fn_str());
        if (texscn) {
            if (texscn->flexitextures.size()==0) {
                wxMessageBox(_("The selected scenery file contains no textures."), _("Warning"), wxOK | wxICON_WARNING, this);
                delete texscn;
                dialog->Destroy();
                return;
            }
            wxArrayString choices;
            for (cFlexiTexture::iterator ms = texscn->flexitextures.begin(); ms != texscn->flexitextures.end(); ms++) {
                choices.Add(ms->Name);
            }
            if (choices.size() > 0) {
                int choice = ::wxGetSingleChoiceIndex(dialog->GetPath(), _("Choose texture settings to import"), choices, this);
                if (choice >= 0) {
                    SetFlexiTexture(texscn->flexitextures[choice]);
                }
            }
            delete texscn;
        }
    }
    dialog->Destroy();
}

void dlgTexture::OnPreview(wxCommandEvent& WXUNUSED(event)) {
    m_panelPreview->Show(m_togglePreview->GetValue());
    UpdatePreview();
    Fit();
    Layout();
}

bool dlgTexture::Validate() {
    if (!wxDialog::Validate())
        return false;

    if (m_ft.Frames.size() == 0) {
        wxMessageBox(_("Your texture needs at least one frame."), _("Validation Error"), wxOK|wxICON_ERROR, this);
        return false;
    }

    if (m_ft.Animation.size() > 0) {
        long a = 0;
        m_textFramerate->GetValue().ToLong(&a);
        if (a == 0) {
            if (wxMessageBox(_("You set up an animation, but the speed is zero. Therefore the animation will not run.\nDo you still want to close the texture window?"), _("Warning"), wxYES_NO|wxNO_DEFAULT|wxICON_WARNING, this) == wxNO) {
                return false;
            }
        }
    }

    return true;
}

////////////////////////////////////////////////////////////////////////
//
//  dlgTexture, Frames
//
////////////////////////////////////////////////////////////////////////

void dlgTexture::FramePreview() {
    UpdateControlState();

    int sel = m_htlbFrame->GetSelection();
    if (sel >= 0) {
        m_previewFile = m_ft.Frames[sel].texture().GetFullPath();
    } else {
        m_previewFile = wxT("");
    }
    UpdatePreview();
}

void dlgTexture::OnFrameListUpdate(wxCommandEvent& WXUNUSED(event)) {
    FramePreview();
}

void dlgTexture::OnFrameUp(wxSpinEvent& WXUNUSED(event)) {
    int sel = m_htlbFrame->GetSelection();
    if (sel < 1)
        return;
    cFlexiTextureFrame ft = m_ft.Frames[sel];
    m_ft.Frames.erase(m_ft.Frames.begin() + sel);
    m_ft.Frames.insert(m_ft.Frames.begin() + sel - 1, ft);

    if (!m_toggleAnimationFreeze->GetValue()) {
        for (cFlexiTextureAnim::iterator i_anim = m_ft.Animation.begin(); i_anim != m_ft.Animation.end(); i_anim++) {
            if (i_anim->frame() == sel)
                i_anim->frame(i_anim->frame() - 1);
            else if (i_anim->frame() == sel-1)
                i_anim->frame(i_anim->frame() + 1);
        }
    }

    m_htlbFrame->UpdateContents();
    m_htlbAnimation->UpdateContents();
    m_htlbFrame->SetSelection(sel-1);
    FramePreview();
    UpdateControlState();
}

void dlgTexture::OnFrameDown(wxSpinEvent& WXUNUSED(event)) {
    int count = m_ft.Frames.size();
    int sel = m_htlbFrame->GetSelection();
    if ((count-sel) <= 1)
        return;
    cFlexiTextureFrame ft = m_ft.Frames[sel];
    m_ft.Frames.erase(m_ft.Frames.begin() + sel);
    m_ft.Frames.insert(m_ft.Frames.begin() + sel + 1, ft);

    if (!m_toggleAnimationFreeze->GetValue()) {
        for (cFlexiTextureAnim::iterator i_anim = m_ft.Animation.begin(); i_anim != m_ft.Animation.end(); i_anim++) {
            if (i_anim->frame() == sel)
                i_anim->frame(i_anim->frame() + 1);
            else if (i_anim->frame() == sel+1)
                i_anim->frame(i_anim->frame() - 1);
        }
    }

    m_htlbFrame->UpdateContents();
    m_htlbAnimation->UpdateContents();
    m_htlbFrame->SetSelection(sel+1);
    FramePreview();
    UpdateControlState();
}

void dlgTexture::OnFrameAdd(wxCommandEvent& WXUNUSED(event)) {
    dlgTextureBase *dialog = NULL;
    wxAUIPicFileDialog *fdialog = new wxAUIPicFileDialog(
                                     this,
                                     _("Select texture file(s) for new texture frame(s)"),
                                     wxEmptyString,
                                     wxEmptyString,
                                     _("Supported Image Files|*.png;*.jpg;*.bmp;*.gif;*.jpeg;*.tiff;*.tif|All Files (*.*)|*.*"),
                                     wxFD_FILE_MUST_EXIST|wxFD_CHANGE_DIR|wxFD_MULTIPLE,
                                     wxDefaultPosition,
                                     wxSize(600,400)
                                 );
    fdialog->SetDirectory(::wxGetApp().g_workdir.GetPath());

    if (fdialog->ShowModal() == wxID_OK) {
        ::wxGetApp().g_workdir.AssignDir(wxFileName(fdialog->GetPath()).GetPath());

        wxArrayString t_paths;
        fdialog->GetPaths(t_paths);

//        ILinfo info;
        if (t_paths.GetCount() == 1) {
            cFlexiTextureFrame ftxf;
            //getBitmapInfo(t_paths[0].fn_str(), info);
            ftxf.texture(t_paths[0]);
            if (wxGXImage(t_paths[0]).HasAlpha())
                ftxf.alphasource(CFTF_ALPHA_INTERNAL);

            dlgTextureFrame *dialog = new dlgTextureFrame(this);
            dialog->SetFlexiTextureFrame(ftxf, m_size);
            if (dialog->ShowModal() == wxID_OK) {
                cFlexiTextureFrame fr = dialog->GetFlexiTextureFrame();
                m_ft.Frames.push_back(fr);
                if (m_size == 0) {
                    wxSize s = getBitmapSize(fr.texture().GetFullPath().fn_str());
                    m_size = s.GetWidth();
                }
                if (m_textTextureName->GetValue() == wxT("")) {
                    m_textTextureName->SetValue(fr.texture().GetName());
                }

                m_htlbFrame->UpdateContents();
                m_htlbFrame->SetSelection(m_ft.Frames.size()-1);
                FramePreview();
                UpdateControlState();
            }
            dialog->Destroy();
        } else {
            int succeeded = 0;
            for (wxArrayString::iterator it = t_paths.begin(); it != t_paths.end(); it++) {
                cFlexiTextureFrame fr;
                wxGXImage img;

                try {
                    checkRCT3Texture(*it);
                    img.FromFile(*it);
                    if ((m_size != 0) && (m_size != img.GetWidth()))
                        continue;
                } catch (RCT3TextureException) {
                    continue;
                }


                fr.texture(*it);
                if (img.HasAlpha())
                    fr.alphasource(CFTF_ALPHA_INTERNAL);
                m_ft.Frames.push_back(fr);
                if (m_size == 0) {
                    m_size = img.GetWidth();
                }
                if (m_textTextureName->GetValue() == wxT("")) {
                    m_textTextureName->SetValue(fr.texture().GetName());
                }
                succeeded++;
            }
            if (succeeded != t_paths.GetCount()) {
                wxMessageBox(wxString::Format(_("%d file(s) could not be added due to errors."), t_paths.GetCount()-succeeded), _("Warning"), wxOK|wxICON_WARNING, this);
            }
            if (succeeded) {
                m_htlbFrame->UpdateContents();
                m_htlbFrame->SetSelection(m_ft.Frames.size()-1);
                FramePreview();
                UpdateControlState();
            }
        }
    } else {
        dlgTextureFrame *dialog = new dlgTextureFrame(this);
        dialog->SetFlexiTextureFrame(cFlexiTextureFrame(), m_size);
        if (dialog->ShowModal() == wxID_OK) {
            cFlexiTextureFrame fr = dialog->GetFlexiTextureFrame();
            m_ft.Frames.push_back(fr);
            if (m_size == 0) {
                wxSize s = getBitmapSize(fr.texture().GetFullPath().fn_str());
                m_size = s.GetWidth();
            }
            if (m_textTextureName->GetValue() == wxT("")) {
                m_textTextureName->SetValue(fr.texture().GetName());
            }

            m_htlbFrame->UpdateContents();
            m_htlbFrame->SetSelection(m_ft.Frames.size()-1);
            FramePreview();
            UpdateControlState();
        }
        dialog->Destroy();
    }

    fdialog->Destroy();
}

void dlgTexture::OnFrameEdit(wxCommandEvent& WXUNUSED(event)) {
    int sel = m_htlbFrame->GetSelection();
    if (sel < 0)
        return;

    // Don't force size if only one frame present.
    int f_size = m_size;
    if (m_ft.Frames.size() == 1)
        f_size = 0;
    dlgTextureFrame *dialog = new dlgTextureFrame(this);
    dialog->SetFlexiTextureFrame(m_ft.Frames[sel], f_size);
    if (dialog->ShowModal() == wxID_OK) {
        m_ft.Frames[sel] = dialog->GetFlexiTextureFrame();
        if (m_ft.Frames.size() == 1) {
            wxSize s = getBitmapSize(dialog->GetFlexiTextureFrame().texture().GetFullPath().fn_str());
            m_size = s.GetWidth();
        }

        m_htlbFrame->UpdateContents();
        m_htlbAnimation->UpdateContents();
        FramePreview();
        UpdateControlState();
    }
    dialog->Destroy();
}

void dlgTexture::OnFrameCopy(wxCommandEvent& WXUNUSED(event)) {
    int sel = m_htlbFrame->GetSelection();
    if (sel<0)
        return;

    m_ft.Frames.insert(m_ft.Frames.begin() + sel + 1, m_ft.Frames[sel]);

    if (!m_toggleAnimationFreeze->GetValue()) {
        for (cFlexiTextureAnim::iterator i_anim = m_ft.Animation.begin(); i_anim != m_ft.Animation.end(); i_anim++) {
            if (i_anim->frame() > sel)
                i_anim->frame(i_anim->frame() + 1);
        }
    }

    m_htlbFrame->UpdateContents();
    m_htlbAnimation->UpdateContents();
    m_htlbFrame->SetSelection(sel+1);
    FramePreview();
    UpdateControlState();
}

void dlgTexture::OnFrameDel(wxCommandEvent& WXUNUSED(event)) {
    long selcount = m_htlbFrame->GetSelectedCount();
    if (selcount == 0)
        return;
    int sel = -1;

    if (selcount == 1) {
        sel = m_htlbFrame->GetSelection();

        m_ft.Frames.erase(m_ft.Frames.begin() + sel);
        if (!m_toggleAnimationFreeze->GetValue()) {
            for(long i = m_ft.Animation.size()-1; i >=0; i--) {
                if (m_ft.Animation[i].frame() > sel)
                    m_ft.Animation[i].frame(m_ft.Animation[i].frame() - 1);
                else if (m_ft.Animation[i].frame() == sel)
                    m_ft.Animation.erase(m_ft.Animation.begin() + i);
            }
        }
    } else {
        for (int run = m_ft.Frames.size()-1; run >= 0; run--) {
            if (m_htlbFrame->IsSelected(run)) {
                m_ft.Frames.erase(m_ft.Frames.begin() + run);
                if (!m_toggleAnimationFreeze->GetValue()) {
                    for(long i = m_ft.Animation.size()-1; i >=0; i--) {
                        if (m_ft.Animation[i].frame() > run)
                            m_ft.Animation[i].frame(m_ft.Animation[i].frame() - 1);
                        else if (m_ft.Animation[i].frame() == run)
                            m_ft.Animation.erase(m_ft.Animation.begin() + i);
                    }
                }
                sel = run;
            }
        }
    }

    if (m_ft.Frames.size() == 0) {
        m_size = 0;
    }

    m_htlbFrame->UpdateContents();
    m_htlbAnimation->UpdateContents();
    m_htlbFrame->SetSelection(sel-1);
    m_htlbAnimation->SetSelection(m_ft.Animation.size()?0:-1);
    FramePreview();
    UpdateControlState();
}

void dlgTexture::OnFrameClear(wxCommandEvent& WXUNUSED(event)) {
    if (m_ft.Frames.size()) {
        if (::wxMessageBox(_("Do you really want to delete all frame entries?\nThis will also delete the animation sequence."), _("Question"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION, this)==wxNO)
            return;
    }

    m_ft.Frames.clear();
    if (!m_toggleAnimationFreeze->GetValue()) {
        m_ft.Animation.clear();
    }
    m_size = 0;

    m_htlbFrame->UpdateContents();
    m_htlbAnimation->UpdateContents();
    m_htlbFrame->SetSelection(-1);
    m_htlbAnimation->SetSelection(-1);
    FramePreview();
    UpdateControlState();
}

////////////////////////////////////////////////////////////////////////
//
//  dlgTexture, Animations
//
////////////////////////////////////////////////////////////////////////

void dlgTexture::AnimationPreview() {
    UpdateControlState();

    int sel = m_htlbAnimation->GetSelection();
    if (sel >= 1) {
        int fr = m_ft.Animation[sel-1].frame();
        if (fr < m_ft.Frames.size())
            m_previewFile = m_ft.Frames[fr].texture().GetFullPath();
        else
            m_previewFile = wxT("");
    } else {
        m_previewFile = wxT("");
    }
    UpdatePreview();
}

void dlgTexture::OnAnimationListUpdate(wxCommandEvent& WXUNUSED(event)) {
    AnimationPreview();
}

void dlgTexture::OnAnimationListSetCount(wxCommandEvent& WXUNUSED(event)) {
    int sel = m_htlbAnimation->GetSelection();
    if (sel < 1)
        return;
    long newcount = ::wxGetNumberFromUser(wxEmptyString, _("Enter number of repetitions"), _("Frame Count"), m_ft.Animation[sel-1].count(), 1, LONG_MAX, this);
    if (newcount >= 1) {
        m_ft.Animation[sel-1].count(newcount);
        m_htlbAnimation->UpdateContents();
        m_htlbAnimation->SetSelection(sel);
    }
}

void dlgTexture::OnAnimationUp(wxSpinEvent& WXUNUSED(event)) {
    int sel = m_htlbAnimation->GetSelection();
    if (sel < 2)
        return;
    cFlexiTextureAnim ft = m_ft.Animation[sel-1];
    m_ft.Animation.erase(m_ft.Animation.begin() + sel - 1);
    m_ft.Animation.insert(m_ft.Animation.begin() + sel - 2, ft);

    m_htlbAnimation->UpdateContents();
    m_htlbAnimation->SetSelection(sel-1);
    AnimationPreview();
    UpdateControlState();
}

void dlgTexture::OnAnimationDown(wxSpinEvent& WXUNUSED(event)) {
    int count = m_ft.Animation.size();
    int sel = m_htlbAnimation->GetSelection();
    if ((count-(sel-1)) <= 1)
        return;
    cFlexiTextureAnim ft = m_ft.Animation[sel-1];
    m_ft.Animation.erase(m_ft.Animation.begin() + sel - 1);
    m_ft.Animation.insert(m_ft.Animation.begin() + sel, ft);

    m_htlbAnimation->UpdateContents();
    m_htlbAnimation->SetSelection(sel+1);
    AnimationPreview();
    UpdateControlState();
}

void dlgTexture::OnAnimationAdd(wxCommandEvent& WXUNUSED(event)) {
    long framecount = m_htlbFrame->GetSelectedCount();
    if (framecount == 0)
        return;
    int sel = m_htlbAnimation->GetSelection();
    if (sel<0)
        sel = 0;

    unsigned long cookie;
    long frame = m_htlbFrame->GetFirstSelected(cookie);
    bool lastnotadded = false;
    while (frame != wxNOT_FOUND) {
        if ((sel>0) && (m_ft.Animation[sel-1].frame() == frame)) {
            m_ft.Animation[sel-1].count(m_ft.Animation[sel-1].count() + 1);
            lastnotadded = true;
        } else {
            cFlexiTextureAnim an(frame);
            m_ft.Animation.insert(m_ft.Animation.begin() + sel, an);
            lastnotadded = false;
            sel++;
        }
        frame = m_htlbFrame->GetNextSelected(cookie);
    }
    if (!lastnotadded)
        sel--;
    if (sel+1 < m_ft.Animation.size()) {
        if (m_ft.Animation[sel].frame() == m_ft.Animation[sel+1].frame()) {
            m_ft.Animation[sel].count(m_ft.Animation[sel].count() + m_ft.Animation[sel+1].count());
            m_ft.Animation.erase(m_ft.Animation.begin() + sel + 1);
        }
    }

    m_htlbAnimation->UpdateContents();
    m_htlbAnimation->SetSelection(sel+1);
    m_htlbFrame->UpdateContents();
    AnimationPreview();
    UpdateControlState();
}

//void dlgTexture::OnAnimationCopy(wxCommandEvent& WXUNUSED(event)) {
void dlgTexture::OnAnimationCountUp(wxSpinEvent& WXUNUSED(event)) {
    int sel = m_htlbAnimation->GetSelection();
    if (sel<1)
        return;

    m_ft.Animation[sel-1].count(m_ft.Animation[sel-1].count() + 1);

    m_htlbAnimation->UpdateContents();
    m_htlbAnimation->SetSelection(sel);
//    AnimationPreview();
//    UpdateControlState();
}

void dlgTexture::OnAnimationCountDown(wxSpinEvent& WXUNUSED(event)) {
    int sel = m_htlbAnimation->GetSelection();
    if (sel<1)
        return;

    if (m_ft.Animation[sel-1].count() == 1)
        return;

    m_ft.Animation[sel-1].count(m_ft.Animation[sel-1].count() - 1);

    m_htlbAnimation->UpdateContents();
    m_htlbAnimation->SetSelection(sel);
//    AnimationPreview();
//    UpdateControlState();
}

void dlgTexture::OnAnimationDel(wxCommandEvent& WXUNUSED(event)) {
    int sel = m_htlbAnimation->GetSelection();
    if (sel<1)
        return;

    m_ft.Animation.erase(m_ft.Animation.begin() + sel - 1);

    m_htlbAnimation->UpdateContents();
    m_htlbAnimation->SetSelection(sel-1);
    m_htlbFrame->UpdateContents();
    AnimationPreview();
    UpdateControlState();
}

void dlgTexture::OnAnimationFreeze(wxCommandEvent& WXUNUSED(event)) {
    if (!m_toggleAnimationFreeze->GetValue()) {
        for(long i = m_ft.Animation.size()-1; i >=0; i--) {
            if (m_ft.Animation[i].frame() >= m_ft.Frames.size())
                m_ft.Animation.erase(m_ft.Animation.begin() + i);
        }

        m_htlbAnimation->UpdateContents();
        m_htlbAnimation->SetSelection(0);
        m_htlbFrame->UpdateContents();
        FramePreview();
    }
    UpdateControlState();
}

void dlgTexture::OnAnimationClear(wxCommandEvent& WXUNUSED(event)) {
    if (m_ft.Animation.size()) {
        if (::wxMessageBox(_("Do you really want to delete the complete animation sequence?"), _("Question"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION, this)==wxNO)
            return;
    }

    m_ft.Animation.clear();

    m_htlbAnimation->UpdateContents();
    m_htlbAnimation->SetSelection(0);
    m_htlbFrame->UpdateContents();
    AnimationPreview();
    UpdateControlState();
}


////////////////////////////////////////////////////////////////////////
//
//  dlgTextureSimple
//
////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(dlgTextureSimple,dlgTextureBase)
EVT_BUTTON(XRCID("m_btTextureInfo"), dlgTextureSimple::OnTextureInfo)
EVT_BUTTON(XRCID("m_btAlphaInfo"), dlgTextureSimple::OnAlphaInfo)
EVT_COMBOBOX(XRCID("m_textTextureFile"), dlgTextureSimple::OnTextureOpen)
EVT_COMBOBOX(XRCID("m_textAlphaFile"), dlgTextureSimple::OnTextureOpen)
EVT_CHOICE(XRCID("m_choiceAlpha"), dlgTextureSimple::OnAlpha)
EVT_BUTTON(XRCID("m_btAdvanced"), dlgTextureSimple::OnAdvanced)
END_EVENT_TABLE()

dlgTextureSimple::dlgTextureSimple(wxWindow *parent) {
    m_recol = 0;

    InitWidgetsFromXRC((wxWindow *)parent);
    SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);

    wxTextCtrl *t_text = XRCCTRL(*this,"m_textTextureFile",wxTextCtrl);

    wxAUIFileDialog *auidlg = new wxAUIPicFileDialog(
                                     this,
                                     _T("Select Texture File"),
                                     wxEmptyString,
                                     wxEmptyString,
                                     _T("Supported Image Files|*.png;*.jpg;*.bmp;*.gif;*.jpeg;*.tiff;*.tif|All Files (*.*)|*.*"),
                                     wxFD_FILE_MUST_EXIST|wxFD_CHANGE_DIR,
                                     wxDefaultPosition,
                                     wxSize(600,400)
                                 );

    m_textTextureFile = new wxFileSelectorCombo<wxFileDialogBase>(this, auidlg, &::wxGetApp().g_workdir, XRCID("m_textTextureFile"));
    t_text->GetContainingSizer()->Replace(t_text, m_textTextureFile);
    t_text->Destroy();
    t_text = XRCCTRL(*this,"m_textAlphaFile",wxTextCtrl);

    auidlg = new wxAUIPicFileDialog(
                                     this,
                                     _T("Select Alpha Texture File"),
                                     wxEmptyString,
                                     wxEmptyString,
                                     _T("Supported Image Files|*.png;*.jpg;*.bmp;*.gif;*.jpeg;*.tiff;*.tif|All Files (*.*)|*.*"),
                                     wxFD_FILE_MUST_EXIST|wxFD_CHANGE_DIR,
                                     wxDefaultPosition,
                                     wxSize(600,400)
                                 );

    m_textAlphaFile = new wxFileSelectorCombo<wxFileDialogBase>(this, auidlg, &::wxGetApp().g_workdir, XRCID("m_textAlphaFile"));
    t_text->GetContainingSizer()->Replace(t_text, m_textAlphaFile);
    t_text->Destroy();

    m_textTextureName->SetValidator(wxExtendedValidator(&m_ft.Name, false));
    m_textTextureFile->SetValidator(wxTextureValidator(&m_frame.Texture, _("Texture File"), false));
    m_choiceAlpha->SetValidator(wxGenericValidator((int*) &m_frame.AlphaSource));
    m_textAlphaFile->SetValidator(wxTextureValidator(&m_frame.Alpha, _("Alpha Channel File"), true));
    m_choiceColors->SetValidator(wxGenericValidator(&m_recol));

    wxInputBox *t_ibTextureName = new wxInputBox(this, wxID_ANY);
    t_ibTextureName->SetEditor(m_textTextureName);
    m_ibTextureFile = new wxInputBox(this, wxID_ANY);
    m_ibTextureFile->SetEditor(m_textTextureFile);
    m_ibAlphaFile = new wxInputBox(this, wxID_ANY);
    m_ibAlphaFile->SetEditor(m_textAlphaFile);

    m_btTextureInfo->SetBitmapLabel(wxXmlResource::Get()->LoadBitmap("documentinfo_16x16"));
    m_btAlphaInfo->SetBitmapLabel(wxXmlResource::Get()->LoadBitmap("documentinfo_16x16"));

    m_textAlphaFile->Enable(false);

    Fit();
    Layout();
    GetSizer()->SetSizeHints(this);

    m_btOk->SetId(wxID_OK);
    m_btCancel->SetId(wxID_CANCEL);
}

cFlexiTexture dlgTextureSimple::GetFlexiTexture() {
    cFlexiTexture ret = m_ft;
    ret.Recolorable = m_recol;
    switch (ret.Recolorable) {
    case 2:
        ret.Recolorable = 3;
        break;
    case 3:
        ret.Recolorable = 7;
        break;
    }
    m_frame.Recolorable = ret.Recolorable;
    ret.Frames.push_back(cFlexiTextureFrame(m_frame));
    return ret;
}

void dlgTextureSimple::SetTextureName(const wxString& val) {
    m_frame.Texture = val;
    m_ft.Name = wxFileName(val).GetName();

    try {
        if (wxGXImage(val).HasAlpha())
            m_frame.AlphaSource = CFTF_ALPHA_INTERNAL;
    } catch (Magick::Exception) {}

};

void dlgTextureSimple::SetFlexiTexture(const cFlexiTexture& val) {
    m_ft = val;
    m_frame = val.Frames[0].raw();
    m_ft.Frames.clear();
    m_ft.Animation.clear();
    m_ft.FPS = 0;
    m_recol = val.Recolorable;
    switch (m_recol) {
        case 3:
            m_recol = 2;
            break;
        case 7:
            m_recol = 3;
    }

    TransferDataToWindow();
    wxCommandEvent e;
    e.SetInt(m_frame.AlphaSource);
    OnAlpha(e);
}

bool dlgTextureSimple::Validate() {
    if (!wxDialog::Validate())
        return false;

    int sel = m_choiceAlpha->GetSelection();

    switch (sel) {
        case CFTF_ALPHA_INTERNAL: {
            //ILinfo inf;
            //getBitmapInfo(m_textTextureFile->GetValue().fn_str(), inf);
            try {
                wxGXImage img(m_textTextureFile->GetValue());
                if (!img.HasAlpha()) {
                    ::wxMessageBox(_("You selected to use an internal alpha channel but the selected file doesn't have one."), _("Validation conflict"), wxICON_ERROR | wxOK, this);
                    return false;
                }
            } catch (Magick::Exception& e) {
                ::wxMessageBox(wxString::Format(_("Error checking texture: %s"), e.what()), _("Validation conflict"), wxICON_ERROR | wxOK, this);
                return false;
            }
        }
        break;
        case CFTF_ALPHA_EXTERNAL: {
            if (m_textAlphaFile->GetValue() == wxT("")) {
                ::wxMessageBox(_("You selected to use an external alpha channel file but didn't give one."), _("Validation conflict"), wxICON_ERROR | wxOK, this);
                m_textAlphaFile->SetFocus();
                return false;
            }

            wxSize tex = getBitmapSize(m_textTextureFile->GetValue().fn_str());
            wxSize alph = getBitmapSize(m_textAlphaFile->GetValue().fn_str());

            if (tex.GetWidth() != alph.GetWidth()) {
                ::wxMessageBox(_("Both texture and alpha need to be the same size!"), _("Validation conflict"), wxICON_ERROR | wxOK, this);
                return false;
            }
        }
        break;
    }


    return true;
}

void dlgTextureSimple::OnTextureInfo(wxCommandEvent& WXUNUSED(event)) {
    if (m_textTextureFile->GetValue() == wxT("")) {
        ::wxMessageBox(_("Please select a texture file before requesting info on it ;-)"), _("Error"), wxICON_ERROR | wxOK, this);
    } else {
        showBitmapInfo(this, m_textTextureFile->GetValue().fn_str());
    }
}

void dlgTextureSimple::OnAlphaInfo(wxCommandEvent& WXUNUSED(event)) {
    if (m_textAlphaFile->GetValue() == wxT("")) {
        ::wxMessageBox(_("Please select an alpha texture file before requesting info on it ;-)"), _("Error"), wxICON_ERROR | wxOK, this);
    } else {
        showBitmapInfo(this, m_textAlphaFile->GetValue().fn_str());
    }
}

void dlgTextureSimple::OnAlpha(wxCommandEvent& event) {
    switch (event.GetInt()) {
        case CFTF_ALPHA_INTERNAL: {
            m_textAlphaFile->Enable(false);
            m_textTextureFile->SetValidator(wxTextureValidator(&m_frame.Texture, _("Texture File"), false, true));
            m_textAlphaFile->SetValidator(wxTextureValidator(&m_frame.Alpha, _("Alpha Channel File"), true));
        }
        break;
        case CFTF_ALPHA_EXTERNAL: {
            m_textAlphaFile->Enable(true);
            m_textTextureFile->SetValidator(wxTextureValidator(&m_frame.Texture, _("Texture File"), false));
            m_textAlphaFile->SetValidator(wxTextureValidator(&m_frame.Alpha, _("Alpha Channel File"), false));
        }
        break;
        default: {
            m_textAlphaFile->Enable(false);
            m_textTextureFile->SetValidator(wxTextureValidator(&m_frame.Texture, _("Texture File"), false));
            m_textAlphaFile->SetValidator(wxTextureValidator(&m_frame.Alpha, _("Alpha Channel File"), true));
        }
    }
    m_ibTextureFile->Update();
    m_ibAlphaFile->Update();
    if ((event.GetInt() == CFTF_ALPHA_EXTERNAL) && (m_textAlphaFile->GetValue() == wxT("")))
        m_textAlphaFile->OnButtonClick();
}

void dlgTextureSimple::OnTextureOpen(wxCommandEvent& event) {
    ::wxGetApp().g_workdir.AssignDir(wxFileName(event.GetString()).GetPath());
}

void dlgTextureSimple::OnAdvanced(wxCommandEvent& WXUNUSED(event)) {
    m_textTextureName->SetValidator(wxExtendedValidator(&m_ft.Name, true));
    m_textTextureFile->SetValidator(wxTextureValidator(&m_frame.Texture, _("Texture File"), true));
    m_textAlphaFile->SetValidator(wxTextureValidator(&m_frame.Alpha, _("Alpha Channel File"), true));
    TransferDataFromWindow();
    EndModal(wxID_SAVE);
}

