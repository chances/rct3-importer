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

#include <math.h>

#include <wx/valtext.h>
#include <wx/valgen.h>

#include "auipicfiledlg.h"
#include "htmlentities.h"
#include "SCNFile.h"
#include "valext.h"
#include "valtexture.h"
#include "wxdevil.h"

#include "ilhelper.h"
#include "wxapp.h"
#include "wxdlgTexture.h"

////////////////////////////////////////////////////////////////////////
//
//  wxTextureEditListBox
//
////////////////////////////////////////////////////////////////////////

wxTextureEditListBox::wxTextureEditListBox(wxWindow *parent, cFlexiTexture *content):
        wxColourHtmlListBox(parent, wxID_ANY, wxDefaultPosition, wxSize(-1, 300), wxSUNKEN_BORDER) {
    m_contents = content;
}

////////////////////////////////////////////////////////////////////////
//
//  wxFrameListBox
//
////////////////////////////////////////////////////////////////////////

wxFrameListBox::wxFrameListBox(wxWindow *parent, cFlexiTexture *content):
        wxTextureEditListBox(parent, content) {
    UpdateContents();
    SetSelection(0);
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
        for (std::vector<unsigned long>::iterator i_anim = m_contents->Animation.begin(); i_anim != m_contents->Animation.end(); i_anim++) {
            if (*i_anim == n) {
                color = wxT("");
                break;
            }
        }
    }
    return wxString::Format(wxT("<font size='2'%s>"), color.c_str()) + wxEncodeHtmlEntities(wxString::Format(wxT("%0*d: %s"), rel, n+1, m_contents->Frames[n].Texture.GetName().c_str()))+wxT("</font>");
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
        if (m_contents->Animation[n-1] >= m_contents->Frames.size()) {
            res = _("<unavailable>");
        } else {
            res = m_contents->Frames[m_contents->Animation[n-1]].Texture.GetName();
        }
        return wxT("<font size='2'>")+wxEncodeHtmlEntities(wxString::Format(wxT("%0*d: %s (%ld)"), rel, n, res.c_str(), m_contents->Animation[n-1]+1))+wxT("</font>");
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
EVT_BUTTON(XRCID("m_btTextureOpen"), dlgTextureFrame::OnTextureOpen)
EVT_BUTTON(XRCID("m_btAlphaOpen"), dlgTextureFrame::OnAlphaOpen)
EVT_BUTTON(XRCID("m_btLoad"), dlgTextureFrame::OnLoad)
END_EVENT_TABLE()

dlgTextureFrame::dlgTextureFrame(wxWindow *parent) {
    m_size = 0;
    InitWidgetsFromXRC((wxWindow *)parent);

    m_textTextureFile->SetValidator(wxTextureValidator(&m_frame.Texture, _("Texture File"), false, &m_size));
    m_textAlphaFile->SetValidator(wxTextureValidator(&m_frame.Alpha, _("Alpha Texture File"), true, &m_size));
    m_textAlphaCutoff->SetValidator(wxExtendedValidator(&m_frame.AlphaCutoff));

    m_btOk->SetId(wxID_OK);
    m_btCancel->SetId(wxID_CANCEL);
}

void dlgTextureFrame::SetFlexiTextureFrame(const cFlexiTextureFrame& val, unsigned int force_size) {
    m_frame = val;
    m_size = force_size;
    TransferDataToWindow();
}

cFlexiTextureFrame dlgTextureFrame::GetFlexiTextureFrame() {
    return m_frame;
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

void dlgTextureFrame::OnTextureOpen(wxCommandEvent& WXUNUSED(event)) {
    wxFileName name = m_textTextureFile->GetValue();
    wxAUIPicFileDialog *dialog = new wxAUIPicFileDialog(
                                     this,
                                     _T("Select Texture File"),
                                     wxEmptyString,
                                     wxEmptyString,
                                     _T("Supported Image Files|*.png;*.jpg;*.bmp;*.gif;*.jpeg;*.tiff;*.tif|All Files (*.*)|*.*"),
                                     wxFD_FILE_MUST_EXIST|wxFD_CHANGE_DIR,
                                     wxDefaultPosition,
                                     wxSize(600,400)
                                 );
    if (name!=wxT("")) {
        dialog->SetFilename(name.GetFullName());
        dialog->SetDirectory(name.GetPath());
    } else {
        dialog->SetDirectory(::wxGetApp().g_workdir.GetPath());
    }
    if (dialog->ShowModal() == wxID_OK) {
        ::wxGetApp().g_workdir.AssignDir(wxFileName(dialog->GetPath()).GetPath());
        m_textTextureFile->SetValue(dialog->GetPath());
    }
    dialog->Destroy();
}

void dlgTextureFrame::OnAlphaOpen(wxCommandEvent& WXUNUSED(event)) {
    wxFileName name = m_textAlphaFile->GetValue();
    wxAUIPicFileDialog *dialog = new wxAUIPicFileDialog(
                                     this,
                                     _T("Select Alpha Texture File"),
                                     wxEmptyString,
                                     wxEmptyString,
                                     _T("Supported Image Files|*.png;*.jpg;*.bmp;*.gif;*.jpeg;*.tiff;*.tif|All Files (*.*)|*.*"),
                                     wxFD_FILE_MUST_EXIST|wxFD_CHANGE_DIR,
                                     wxDefaultPosition,
                                     wxSize(600,400)
                                 );
    if (name!=wxT("")) {
        dialog->SetFilename(name.GetFullName());
        dialog->SetDirectory(name.GetPath());
    } else
        dialog->SetDirectory(::wxGetApp().g_workdir.GetPath());
    if (dialog->ShowModal() == wxID_OK) {
        ::wxGetApp().g_workdir.AssignDir(wxFileName(dialog->GetPath()).GetPath());
        m_textAlphaFile->SetValue(dialog->GetPath());
    }
    dialog->Destroy();
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
            for (cFlexiTextureIterator ms = texscn->flexitextures.begin(); ms != texscn->flexitextures.end(); ms++) {
                for (cFlexiTextureFrameIterator it = ms->Frames.begin(); it != ms->Frames.end(); it++) {
                    choices.Add(ms->Name + wxT(", ") + it->Texture.GetName());
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
EVT_SPIN_UP(XRCID("m_spinAnimation"), dlgTexture::OnAnimationUp)
EVT_SPIN_DOWN(XRCID("m_spinAnimation"), dlgTexture::OnAnimationDown)
EVT_BUTTON(XRCID("m_btAnimationAdd"), dlgTexture::OnAnimationAdd)
EVT_BUTTON(XRCID("m_btAnimationCopy"), dlgTexture::OnAnimationCopy)
EVT_BUTTON(XRCID("m_btAnimationDel"), dlgTexture::OnAnimationDel)
EVT_TOGGLEBUTTON(XRCID("m_toggleAnimationFreeze"), dlgTexture::OnAnimationFreeze)
EVT_BUTTON(XRCID("m_btAnimationClear"), dlgTexture::OnAnimationClear)
END_EVENT_TABLE()

dlgTexture::dlgTexture(wxWindow *parent) {
    m_size = 0;

    InitWidgetsFromXRC((wxWindow *)parent);

    m_htlbAnimation = new wxAnimationListBox(this, &m_ft);
    wxXmlResource::Get()->AttachUnknownControl(wxT("m_htlbAnimation"), m_htlbAnimation, this);
    m_htlbFrame = new wxFrameListBox(this, &m_ft);
    m_htlbFrame->SetToolTip(_("List of texture frames.\nUnused frames are shown in red. These will not be written to the ovl."));
    wxXmlResource::Get()->AttachUnknownControl(wxT("m_htlbFrame"), m_htlbFrame, this);
    m_ilpictPreview = new wxILPicture(this, wxID_ANY, wxT(""));
    m_ilpictPreview->SetBackgroundColour(*wxWHITE);
    m_ilpictPreview->SetScale(wxILSCALE_UNIFORM);
    m_ilpictPreview->SetAlignment(wxALIGN_CENTER);
    wxXmlResource::Get()->AttachUnknownControl(wxT("m_ilpictPreview"), m_ilpictPreview, this);

    m_textTextureName->SetValidator(wxExtendedValidator(&m_ft.Name, false));
    m_textFramerate->SetValidator(wxExtendedValidator(&m_ft.FPS));

    m_btOk->SetId(wxID_OK);
    m_btCancel->SetId(wxID_CANCEL);
}

void dlgTexture::SetFlexiTexture(const cFlexiTexture& val) {
    m_ft = val;
    if (val.Frames.size()) {
        wxSize s = getBitmapSize(val.Frames[0].Texture.GetFullPath().fn_str());
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
    bool frozen = m_toggleAnimationFreeze->GetValue();

    m_spinFrame->Enable(count>=2);
    m_btFrameEdit->Enable(sel>=0);
    m_btFrameCopy->Enable(sel>=0);
    m_btFrameDel->Enable(sel>=0);
    m_btFrameClear->Enable(count>=1);
    m_btAnimationAdd->Enable(frozen?false:(sel>=0));

    count = m_ft.Animation.size();
    sel = m_htlbAnimation->GetSelection();
    m_spinAnimation->Enable(frozen?false:(count>=2));
    m_btAnimationCopy->Enable(frozen?false:(sel>=0));
    m_btAnimationDel->Enable(frozen?false:(sel>=0));
    m_toggleAnimationFreeze->Enable(count>=1);
    m_btAnimationClear->Enable(frozen?false:(count>=1));

    m_btOk->Enable(!frozen);
    m_textFramerate->Enable(count>=2);
}

void dlgTexture::UpdatePreview() {
    if (m_togglePreview->GetValue()) {
        m_ilpictPreview->SetFileName(m_previewFile);
        m_ilpictPreview->Refresh();
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
            for (cFlexiTextureIterator ms = texscn->flexitextures.begin(); ms != texscn->flexitextures.end(); ms++) {
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

////////////////////////////////////////////////////////////////////////
//
//  dlgTexture, Frames
//
////////////////////////////////////////////////////////////////////////

void dlgTexture::FramePreview() {
    UpdateControlState();

    int sel = m_htlbFrame->GetSelection();
    if (sel >= 0) {
        m_previewFile = m_ft.Frames[sel].Texture.GetFullPath();
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
        for (std::vector<unsigned long>::iterator i_anim = m_ft.Animation.begin(); i_anim != m_ft.Animation.end(); i_anim++) {
            if (*i_anim == sel)
                *i_anim -= 1;
            else if (*i_anim == sel-1)
                *i_anim += 1;
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
        for (std::vector<unsigned long>::iterator i_anim = m_ft.Animation.begin(); i_anim != m_ft.Animation.end(); i_anim++) {
            if (*i_anim == sel)
                *i_anim += 1;
            else if (*i_anim == sel+1)
                *i_anim -= 1;
        }
    }

    m_htlbFrame->UpdateContents();
    m_htlbAnimation->UpdateContents();
    m_htlbFrame->SetSelection(sel+1);
    FramePreview();
    UpdateControlState();
}

void dlgTexture::OnFrameAdd(wxCommandEvent& WXUNUSED(event)) {
    dlgTextureFrame *dialog = new dlgTextureFrame(this);
    dialog->SetFlexiTextureFrame(cFlexiTextureFrame(), m_size);
    if (dialog->ShowModal() == wxID_OK) {
        cFlexiTextureFrame fr = dialog->GetFlexiTextureFrame();
        m_ft.Frames.push_back(fr);
        if (m_size == 0) {
            wxSize s = getBitmapSize(fr.Texture.GetFullPath().fn_str());
            m_size = s.GetWidth();
        }
        if (m_textTextureName->GetValue() == wxT("")) {
            m_textTextureName->SetValue(fr.Texture.GetName());
        }

        m_htlbFrame->UpdateContents();
        m_htlbFrame->SetSelection(m_ft.Frames.size()-1);
        FramePreview();
        UpdateControlState();
    }
    dialog->Destroy();
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
            wxSize s = getBitmapSize(dialog->GetFlexiTextureFrame().Texture.GetFullPath().fn_str());
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
        for (std::vector<unsigned long>::iterator i_anim = m_ft.Animation.begin(); i_anim != m_ft.Animation.end(); i_anim++) {
            if (*i_anim > sel)
                *i_anim += 1;
        }
    }

    m_htlbFrame->UpdateContents();
    m_htlbAnimation->UpdateContents();
    m_htlbFrame->SetSelection(sel+1);
    FramePreview();
    UpdateControlState();
}

void dlgTexture::OnFrameDel(wxCommandEvent& WXUNUSED(event)) {
    int sel = m_htlbFrame->GetSelection();
    if (sel<0)
        return;

    m_ft.Frames.erase(m_ft.Frames.begin() + sel);
    if (!m_toggleAnimationFreeze->GetValue()) {
        for(long i = m_ft.Animation.size()-1; i >=0; i--) {
            if (m_ft.Animation[i] > sel)
                m_ft.Animation[i]--;
            else if (m_ft.Animation[i] == sel)
                m_ft.Animation.erase(m_ft.Animation.begin() + i);
        }
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
        int fr = m_ft.Animation[sel-1];
        if (fr < m_ft.Frames.size())
            m_previewFile = m_ft.Frames[fr].Texture.GetFullPath();
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

void dlgTexture::OnAnimationUp(wxSpinEvent& WXUNUSED(event)) {
    int sel = m_htlbAnimation->GetSelection();
    if (sel < 2)
        return;
    unsigned long ft = m_ft.Animation[sel-1];
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
    unsigned long ft = m_ft.Animation[sel-1];
    m_ft.Animation.erase(m_ft.Animation.begin() + sel - 1);
    m_ft.Animation.insert(m_ft.Animation.begin() + sel, ft);

    m_htlbAnimation->UpdateContents();
    m_htlbAnimation->SetSelection(sel+1);
    AnimationPreview();
    UpdateControlState();
}

void dlgTexture::OnAnimationAdd(wxCommandEvent& WXUNUSED(event)) {
    int frame = m_htlbFrame->GetSelection();
    if (frame<0)
        return;
    int sel = m_htlbAnimation->GetSelection();
    if (sel<0)
        sel = 0;

    m_ft.Animation.insert(m_ft.Animation.begin() + sel, (unsigned long) frame);


    m_htlbAnimation->UpdateContents();
    m_htlbAnimation->SetSelection(sel+1);
    m_htlbFrame->UpdateContents();
    AnimationPreview();
    UpdateControlState();
}

void dlgTexture::OnAnimationCopy(wxCommandEvent& WXUNUSED(event)) {
    int sel = m_htlbAnimation->GetSelection();
    if (sel<1)
        return;

    unsigned long ft = m_ft.Animation[sel-1];
    m_ft.Animation.insert(m_ft.Animation.begin() + sel, ft);

    m_htlbAnimation->UpdateContents();
    m_htlbAnimation->SetSelection(sel+1);
    AnimationPreview();
    UpdateControlState();
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
            if (m_ft.Animation[i] >= m_ft.Frames.size())
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
EVT_BUTTON(XRCID("m_btTextureOpen"), dlgTextureSimple::OnTextureOpen)
EVT_BUTTON(XRCID("m_btAlphaOpen"), dlgTextureSimple::OnAlphaOpen)
EVT_BUTTON(XRCID("m_btAdvanced"), dlgTextureSimple::OnAdvanced)
END_EVENT_TABLE()

dlgTextureSimple::dlgTextureSimple(wxWindow *parent) {
    m_recol = 0;

    InitWidgetsFromXRC((wxWindow *)parent);

    m_textTextureName->SetValidator(wxExtendedValidator(&m_ft.Name, false));
    m_textTextureFile->SetValidator(wxTextureValidator(&m_frame.Texture, _("Texture File"), false));
    m_textAlphaFile->SetValidator(wxTextureValidator(&m_frame.Alpha, _("Alpha Texture File"), true));
    m_choiceColors->SetValidator(wxGenericValidator(&m_recol));

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
    ret.Frames.push_back(m_frame);
    return ret;
}

void dlgTextureSimple::SetTextureName(const wxString& val) {
    m_frame.Texture = val;
    m_ft.Name = wxFileName(val).GetName();
};

bool dlgTextureSimple::Validate() {
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

void dlgTextureSimple::OnTextureOpen(wxCommandEvent& WXUNUSED(event)) {
    wxFileName name = m_textTextureFile->GetValue();
    wxAUIPicFileDialog *dialog = new wxAUIPicFileDialog(
                                     this,
                                     _T("Select Texture File"),
                                     wxEmptyString,
                                     wxEmptyString,
                                     _T("Supported Image Files|*.png;*.jpg;*.bmp;*.gif;*.jpeg;*.tiff;*.tif|All Files (*.*)|*.*"),
                                     wxFD_FILE_MUST_EXIST|wxFD_CHANGE_DIR,
                                     wxDefaultPosition,
                                     wxSize(600,400)
                                 );
    if (name!=wxT("")) {
        dialog->SetFilename(name.GetFullName());
        dialog->SetDirectory(name.GetPath());
    }
    if (dialog->ShowModal() == wxID_OK) {
        ::wxGetApp().g_workdir.AssignDir(wxFileName(dialog->GetPath()).GetPath());
        m_textTextureFile->SetValue(dialog->GetPath());
        if (m_textTextureName->GetValue() == wxT("")) {
            name = dialog->GetPath();
            m_textTextureName->SetValue(name.GetName());
        }
    }
    delete dialog;
}

void dlgTextureSimple::OnAlphaOpen(wxCommandEvent& WXUNUSED(event)) {
    wxFileName name = m_textAlphaFile->GetValue();
    wxAUIPicFileDialog *dialog = new wxAUIPicFileDialog(
                                     this,
                                     _T("Select Alpha Texture File"),
                                     wxEmptyString,
                                     wxEmptyString,
                                     _T("Supported Image Files|*.png;*.jpg;*.bmp;*.gif;*.jpeg;*.tiff;*.tif|All Files (*.*)|*.*"),
                                     wxFD_FILE_MUST_EXIST|wxFD_CHANGE_DIR,
                                     wxDefaultPosition,
                                     wxSize(600,400)
                                 );
    if (name!=wxT("")) {
        dialog->SetFilename(name.GetFullName());
        dialog->SetDirectory(name.GetPath());
    }
    if (dialog->ShowModal() == wxID_OK) {
        ::wxGetApp().g_workdir.AssignDir(wxFileName(dialog->GetPath()).GetPath());
        m_textAlphaFile->SetValue(dialog->GetPath());
    }
    delete dialog;
}

void dlgTextureSimple::OnAdvanced(wxCommandEvent& WXUNUSED(event)) {
    m_textTextureName->SetValidator(wxExtendedValidator(&m_ft.Name, true));
    m_textTextureFile->SetValidator(wxTextureValidator(&m_frame.Texture, _("Texture File"), true));
    TransferDataFromWindow();
    EndModal(wxID_SAVE);
}

