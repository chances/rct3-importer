///////////////////////////////////////////////////////////////////////////////
//
// Create Scenery OVL Dialog
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

#include "wxdlgCreateScenery.h"

#include <wx/artprov.h>
#include <wx/progdlg.h>
#include <wx/valgen.h>

#include <IL/il.h>
#include <IL/ilu.h>

#include "auipicfiledlg.h"
#include "htmlentities.h"
#include "logext.h"
#include "matrix.h"
#include "popupmessagewin.h"
#include "rct3log.h"
#include "valext.h"
#include "wxdevil.h"

#include "OVL.H"

#include "wxapp.h"
#include "wxdlgLOD.h"
#include "wxdlgModel.h"
#include "wxdlgTexture.h"

const long idToolBar = ::wxNewId();
const long tbLoadCache = ::wxNewId();
const long tbClearCache = ::wxNewId();

////////////////////////////////////////////////////////////////////////
//
//  wxSceneryListBox
//
////////////////////////////////////////////////////////////////////////

wxSceneryListBox::wxSceneryListBox(wxWindow *parent, cSCNFile *content):
        wxColourHtmlListBox(parent, wxID_ANY, wxDefaultPosition, wxSize(-1, 300), wxSUNKEN_BORDER) {
    m_contents = content;
}

////////////////////////////////////////////////////////////////////////
//
//  wxTextureListBox
//
////////////////////////////////////////////////////////////////////////

wxTextureListBox::wxTextureListBox(wxWindow *parent, cSCNFile *content):
        wxSceneryListBox(parent, content) {
    UpdateContents();
    SetSelection(0);
}

void wxTextureListBox::UpdateContents() {
    SetItemCount(m_contents->flexitextures.size());
    RefreshAll();
}

wxString wxTextureListBox::OnGetItem(size_t n) const {
    wxString addon = wxT("");
    for (cFlexiTextureFrameIterator it = m_contents->flexitextures[n].Frames.begin(); it != m_contents->flexitextures[n].Frames.end(); it++) {
        if (it->Alpha != wxT("")) {
            addon += _("Alpha");
            break;
        }
    }
    if (m_contents->flexitextures[n].Recolorable) {
        addon += (addon != wxT(""))?wxT(", "):wxT("");
        addon += _("Recolorable");
    }
    if (m_contents->flexitextures[n].FPS > 0) {
         addon += (addon != wxT(""))?wxT(", "):wxT("");
         addon += _("Animated");
    }
    if (addon != wxT(""))
        return wxT("<font size='2'>")+wxEncodeHtmlEntities(m_contents->flexitextures[n].Name)
            +wxT("<br>&nbsp;&nbsp;(")+addon+wxT(")")
            +wxT("</font>");
    else
        return wxT("<font size='2'>")+wxEncodeHtmlEntities(m_contents->flexitextures[n].Name)+wxT("</font>");
}

////////////////////////////////////////////////////////////////////////
//
//  wxModelListBox
//
////////////////////////////////////////////////////////////////////////

wxModelListBox::wxModelListBox(wxWindow *parent, cSCNFile *content):
        wxSceneryListBox(parent, content) {
    UpdateContents();
    SetSelection(0);
}

void wxModelListBox::UpdateContents() {
    SetItemCount(m_contents->models.size());
    RefreshAll();
}

wxString wxModelListBox::OnGetItem(size_t n) const {
    wxString ret = wxT("<font size='2'>");
    if (m_contents->models[n].fatal_error) {
        ret += wxT("<font color='#FF3C3C'>");
    } else if (m_contents->models[n].error.size()) {
        ret += wxT("<font color='#FFCC00'>");
    }
    ret += wxEncodeHtmlEntities(m_contents->models[n].name);
    if ((m_contents->models[n].fatal_error) || (m_contents->models[n].error.size())) {
        ret += wxT("</font>");
    }
    unsigned long mesh_enabled = 0;
    unsigned long mesh_faces = 0;
    for(cMeshStructIterator it = m_contents->models[n].meshstructs.begin(); it != m_contents->models[n].meshstructs.end(); it++) {
        if (!it->disabled) {
            mesh_enabled++;
            mesh_faces += it->faces;
        }
    }
    ret += wxString::Format(wxT("<br>&nbsp;&nbsp;(%ld/%d meshes"), mesh_enabled, m_contents->models[n].meshstructs.size());
    if (mesh_faces)
        ret += wxString::Format(wxT(", %ld faces"), mesh_faces);
    if (m_contents->models[n].effectpoints.size())
        ret += wxString::Format(wxT(", %d effect points"), m_contents->models[n].effectpoints.size());
    return ret + wxT(")</font>");
}

////////////////////////////////////////////////////////////////////////
//
//  wxLODListBox
//
////////////////////////////////////////////////////////////////////////

wxLODListBox::wxLODListBox(wxWindow *parent, cSCNFile *content):
        wxSceneryListBox(parent, content) {
    UpdateContents();
    SetSelection(0);
}

void wxLODListBox::UpdateContents() {
    SetItemCount(m_contents->lods.size());
    RefreshAll();
}

wxString wxLODListBox::OnGetItem(size_t n) const {
    return wxT("<font size='2'>")+wxString::Format(wxT("%.1f<br>&nbsp;&nbsp;"), m_contents->lods[n].distance) + wxEncodeHtmlEntities(m_contents->lods[n].modelname)+wxT("</font>");
}

////////////////////////////////////////////////////////////////////////
//
//  wxReferenceListBox
//
////////////////////////////////////////////////////////////////////////

wxReferenceListBox::wxReferenceListBox(wxWindow *parent, cSCNFile *content):
        wxSceneryListBox(parent, content) {
    UpdateContents();
    SetSelection(0);
}

void wxReferenceListBox::UpdateContents() {
    SetItemCount(m_contents->references.size());
    RefreshAll();
}

wxString wxReferenceListBox::OnGetItem(size_t n) const {
    return wxT("<font size='2'>")+wxEncodeHtmlEntities(m_contents->references[n])+wxT("</font>");
}


////////////////////////////////////////////////////////////////////////
//
//  dlgCreateScenery
//
////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(dlgCreateScenery,wxDialog)
EVT_TOOL(wxID_ANY, dlgCreateScenery::OnToolBar)
EVT_CLOSE(dlgCreateScenery::OnClose)
EVT_INIT_DIALOG(dlgCreateScenery::OnInitDialog)
EVT_CHECKBOX(XRCID("m_checkShowUnknown"), dlgCreateScenery::OnShowUnknowns)
// Update Control En-/Disabeling when the listboxes are clicked
EVT_LISTBOX(XRCID("m_htlbTexture"), dlgCreateScenery::OnControlUpdate)
EVT_LISTBOX(XRCID("m_htlbModel"), dlgCreateScenery::OnControlUpdate)
EVT_LISTBOX(XRCID("m_htlbLOD"), dlgCreateScenery::OnControlUpdate)
EVT_LISTBOX(XRCID("m_htlbReferences"), dlgCreateScenery::OnControlUpdate)
// Texture Controls
EVT_SPIN_UP(XRCID("m_spinTexture"), dlgCreateScenery::OnTextureUp)
EVT_SPIN_DOWN(XRCID("m_spinTexture"), dlgCreateScenery::OnTextureDown)
EVT_BUTTON(XRCID("m_btTextureAdd"), dlgCreateScenery::OnTextureAdd)
EVT_BUTTON(XRCID("m_btTextureEdit"), dlgCreateScenery::OnTextureEdit)
EVT_LISTBOX_DCLICK(XRCID("m_htlbTexture"), dlgCreateScenery::OnTextureEdit)
EVT_BUTTON(XRCID("m_btTextureCopy"), dlgCreateScenery::OnTextureCopy)
EVT_BUTTON(XRCID("m_btTextureDel"), dlgCreateScenery::OnTextureDel)
EVT_BUTTON(XRCID("m_btTextureClear"), dlgCreateScenery::OnTextureClear)
// Model Controls
EVT_SPIN_UP(XRCID("m_spinModel"), dlgCreateScenery::OnModelUp)
EVT_SPIN_DOWN(XRCID("m_spinModel"), dlgCreateScenery::OnModelDown)
EVT_BUTTON(XRCID("m_btModelAdd"), dlgCreateScenery::OnModelAdd)
EVT_BUTTON(XRCID("m_btModelEdit"), dlgCreateScenery::OnModelEdit)
EVT_LISTBOX_DCLICK(XRCID("m_htlbModel"), dlgCreateScenery::OnModelEdit)
EVT_BUTTON(XRCID("m_btModelCopy"), dlgCreateScenery::OnModelCopy)
EVT_BUTTON(XRCID("m_btModelDel"), dlgCreateScenery::OnModelDel)
EVT_BUTTON(XRCID("m_btModelClear"), dlgCreateScenery::OnModelClear)
// LOD Controls
EVT_BUTTON(XRCID("m_btLODAdd"), dlgCreateScenery::OnLODAdd)
EVT_BUTTON(XRCID("m_btLODEdit"), dlgCreateScenery::OnLODEdit)
EVT_LISTBOX_DCLICK(XRCID("m_htlbLOD"), dlgCreateScenery::OnLODEdit)
EVT_BUTTON(XRCID("m_btLODCopy"), dlgCreateScenery::OnLODCopy)
EVT_BUTTON(XRCID("m_btLODDel"), dlgCreateScenery::OnLODDel)
EVT_BUTTON(XRCID("m_btLODAuto"), dlgCreateScenery::OnLODAuto)
EVT_BUTTON(XRCID("m_btLODClear"), dlgCreateScenery::OnLODClear)
// Visual Controls
EVT_CHOICE(XRCID("m_choiceExpansion"), dlgCreateScenery::OnMakeDirty)
EVT_CHECKBOX(XRCID("m_checkGreenery"), dlgCreateScenery::OnMakeDirty)
EVT_TEXT(XRCID("m_textSway"), dlgCreateScenery::OnMakeDirty)
EVT_CHECKBOX(XRCID("m_checkShadow"), dlgCreateScenery::OnMakeDirty)
EVT_TEXT(XRCID("m_textBrightness"), dlgCreateScenery::OnMakeDirty)
EVT_CHECKBOX(XRCID("m_checkRotation"), dlgCreateScenery::OnMakeDirty)
EVT_TEXT(XRCID("m_textScale"), dlgCreateScenery::OnMakeDirty)
EVT_BUTTON(XRCID("m_btVisualNormal"), dlgCreateScenery::OnVisualNormal)
EVT_BUTTON(XRCID("m_btVisualSwayingTree"), dlgCreateScenery::OnVisualSwayingTree)
EVT_CHECKBOX(XRCID("m_checkUnknown"), dlgCreateScenery::OnMakeDirty)
EVT_TEXT(XRCID("m_textUnknownFactor"), dlgCreateScenery::OnMakeDirty)
EVT_TEXT(XRCID("m_textUnknown6"), dlgCreateScenery::OnMakeDirty)
EVT_TEXT(XRCID("m_textUnknown7"), dlgCreateScenery::OnMakeDirty)
EVT_TEXT(XRCID("m_textUnknown8"), dlgCreateScenery::OnMakeDirty)
EVT_TEXT(XRCID("m_textUnknown9"), dlgCreateScenery::OnMakeDirty)
EVT_TEXT(XRCID("m_textUnknown10"), dlgCreateScenery::OnMakeDirty)
EVT_TEXT(XRCID("m_textUnknown11"), dlgCreateScenery::OnMakeDirty)
// Reference Controls
EVT_BUTTON(XRCID("m_btReferenceAdd"), dlgCreateScenery::OnReferenceAdd)
EVT_BUTTON(XRCID("m_btReferenceDel"), dlgCreateScenery::OnReferenceDel)
EVT_BUTTON(XRCID("m_btReferenceClear"), dlgCreateScenery::OnReferenceClear)

EVT_BUTTON(XRCID("m_btCreate"), dlgCreateScenery::OnCreate)

END_EVENT_TABLE()

dlgCreateScenery::dlgCreateScenery(wxWindow *parent) {
    m_tbCS = NULL;

    InitWidgetsFromXRC((wxWindow *)parent);

    m_tbCS = new wxToolBar(this, idToolBar, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_HORIZONTAL | wxTB_NODIVIDER);
    m_tbCS->SetToolBitmapSize(wxSize(wxArtProvider::GetBitmap(wxART_NEW, wxART_TOOLBAR).GetWidth(), wxArtProvider::GetBitmap(wxART_NEW, wxART_TOOLBAR).GetHeight()));
    m_tbCS->AddTool(wxID_NEW, _("New"), wxXmlResource::Get()->LoadBitmap("filenew_16x16"), _("New Scenery File"));
    m_tbCS->AddTool(wxID_OPEN, _("Open"), wxXmlResource::Get()->LoadBitmap("fileopen_16x16"), _("Open Scenery File..."));
    m_tbCS->AddTool(wxID_SAVE, _("Save"), wxXmlResource::Get()->LoadBitmap("filesave_16x16"), _("Save Scenery File"));
    m_tbCS->AddTool(wxID_SAVEAS, _("Save As"), wxXmlResource::Get()->LoadBitmap("filesaveas_16x16"), _("Save Scenery File As..."));
    m_tbCS->AddSeparator();
    m_tbCS->AddTool(tbLoadCache, _("Load Cache"), wxXmlResource::Get()->LoadBitmap("edit_add_16x16"), _("Load SCN into Texture Cache..."));
    m_tbCS->AddTool(tbClearCache, _("Clear Cache"), wxXmlResource::Get()->LoadBitmap("editdelete_16x16"), _("Clear Texture Cache"));
    m_tbCS->AddSeparator();
    m_tbCS->AddTool(wxID_CANCEL, _("Exit"), wxXmlResource::Get()->LoadBitmap("exit_16x16"), _("Leave Create Scenery OVL window"));
    m_tbCS->Realize();
    m_tbCS->EnableTool(wxID_SAVE, false);

    GetSizer()->Insert(0, m_tbCS, wxSizerFlags().Expand().Border(wxTOP));
    m_htlbTexture = new wxTextureListBox(this, &m_SCN);
    wxXmlResource::Get()->AttachUnknownControl(wxT("m_htlbTexture"), m_htlbTexture, this);
    m_htlbModel = new wxModelListBox(this, &m_SCN);
    m_htlbModel->SetToolTip(_("Model list\nOrange: Warnings occured during loading the model form the scenery file.\nRed: An error occured during loading the model form the scenery file. Usually the model file was not found."));
    wxXmlResource::Get()->AttachUnknownControl(wxT("m_htlbModel"), m_htlbModel, this);
    m_htlbLOD = new wxLODListBox(this, &m_SCN);
    wxXmlResource::Get()->AttachUnknownControl(wxT("m_htlbLOD"), m_htlbLOD, this);
    m_htlbReferences = new wxReferenceListBox(this, &m_SCN);
    wxXmlResource::Get()->AttachUnknownControl(wxT("m_htlbReferences"), m_htlbReferences, this);

    Fit();
    Layout();
    GetSizer()->SetSizeHints(this);

    m_textSway->SetValidator(wxExtendedValidator(&m_SCN.sivsettings.sway, 2));
    m_textBrightness->SetValidator(wxExtendedValidator(&m_SCN.sivsettings.brightness, 2));
    m_textScale->SetValidator(wxExtendedValidator(&m_SCN.sivsettings.scale, 2));
    m_textUnknownFactor->SetValidator(wxExtendedValidator(&m_SCN.sivsettings.unknown, 2));
    m_textUnknown6->SetValidator(wxExtendedValidator(&m_SCN.sivsettings.unk6));
    m_textUnknown7->SetValidator(wxExtendedValidator(&m_SCN.sivsettings.unk7));
    m_textUnknown8->SetValidator(wxExtendedValidator(&m_SCN.sivsettings.unk8));
    m_textUnknown9->SetValidator(wxExtendedValidator(&m_SCN.sivsettings.unk9));
    m_textUnknown10->SetValidator(wxExtendedValidator(&m_SCN.sivsettings.unk10));
    m_textUnknown11->SetValidator(wxExtendedValidator(&m_SCN.sivsettings.unk11));

    UpdateAll();

    m_dirtyfile = false;
}

wxString dlgCreateScenery::GetOVLPath() {
    if (m_SCN.ovlname.GetFullPath() != wxT(""))
        return m_SCN.ovlname.GetFullPath() + wxT(".common.ovl");
    else
        return wxEmptyString;
}

void dlgCreateScenery::UpdateControlState() {
    int count = m_SCN.flexitextures.size();
    int sel = m_htlbTexture->GetSelection();
    m_spinTexture->Enable(count>=2);
    m_btTextureEdit->Enable(sel>=0);
    m_btTextureCopy->Enable(sel>=0);
    m_btTextureDel->Enable(sel>=0);
    m_btTextureClear->Enable(count>=1);

    count = m_SCN.models.size();
    sel = m_htlbModel->GetSelection();
    m_spinModel->Enable(count>=2);
    m_btModelEdit->Enable(sel>=0);
    m_btModelCopy->Enable(sel>=0);
    m_btModelDel->Enable(sel>=0);
    m_btModelClear->Enable(count>=1);
    m_btLODAdd->Enable(count>=1);
    m_btLODAuto->Enable(count==1);

    count = m_SCN.lods.size();
    sel = m_htlbLOD->GetSelection();
    m_btLODEdit->Enable(sel>=0);
    m_btLODCopy->Enable(sel>=0);
    m_btLODDel->Enable(sel>=0);
    m_btLODClear->Enable(count>=1);

    count = m_SCN.references.size();
    sel = m_htlbReferences->GetSelection();
    m_btReferenceDel->Enable(sel>=0);
    m_btReferenceClear->Enable(count>=1);

    m_btCreate->Enable(m_SCN.flexitextures.size() || (m_SCN.models.size() && m_SCN.lods.size()));
    if (m_SCN.models.size())
        m_btCreate->SetLabel(_("Create Scenery OVL"));
    else
        m_btCreate->SetLabel(_("Create Texture OVL"));

    wxString title = _("Create OVL");
    if (m_SCN.filename != wxT(""))
        title += wxT(" - ") + m_SCN.filename.GetFullName();
    if (m_SCN.ovlname != wxT(""))
        title += wxT(" (") + m_SCN.ovlname.GetFullName() + wxT(")");
    SetLabel(title);
}

bool dlgCreateScenery::TransferDataFromWindow() {
    int flags;
    switch (m_choiceExpansion->GetSelection()) {
    case 1:
        flags = OVL_SIV_SOAKED;
        break;
    case 2:
        flags = OVL_SIV_WILD;
        break;
    default:
        flags = 0;
    }
    if (m_checkGreenery->IsChecked())
        flags |= OVL_SIV_GREENERY;
    if (m_checkShadow->IsChecked())
        flags |= OVL_SIV_NO_SHADOW;
    if (m_checkRotation->IsChecked())
        flags |= OVL_SIV_ROTATION;
    if (m_checkUnknown->IsChecked())
        flags |= OVL_SIV_UNKNOWN;
    m_SCN.sivsettings.sivflags = flags;

    return wxDialog::TransferDataFromWindow();
}

bool dlgCreateScenery::TransferDataToWindow() {
    if (m_SCN.sivsettings.sivflags & OVL_SIV_SOAKED)
        m_choiceExpansion->SetSelection(1);
    else if (m_SCN.sivsettings.sivflags & OVL_SIV_WILD)
        m_choiceExpansion->SetSelection(2);
    else
        m_choiceExpansion->SetSelection(0);

    m_checkGreenery->SetValue(m_SCN.sivsettings.sivflags & OVL_SIV_GREENERY);
    m_checkShadow->SetValue(m_SCN.sivsettings.sivflags & OVL_SIV_NO_SHADOW);
    m_checkRotation->SetValue(m_SCN.sivsettings.sivflags & OVL_SIV_ROTATION);
    m_checkUnknown->SetValue(m_SCN.sivsettings.sivflags & OVL_SIV_UNKNOWN);

    return wxDialog::TransferDataToWindow();
}

void dlgCreateScenery::MakeDirty(bool dirty) {
    m_dirtyfile = dirty;
    if (m_tbCS)
        m_tbCS->EnableTool(wxID_SAVE, (m_dirtyfile && (m_SCN.filename != wxT(""))));
}

// Uses TransferDataToWindow, so it's only to be used if the visual settings
// didn't change or should be overwritten
void dlgCreateScenery::UpdateAll() {
    m_htlbTexture->UpdateContents();
    m_htlbModel->UpdateContents();
    m_htlbLOD->UpdateContents();
    m_htlbReferences->UpdateContents();

    TransferDataToWindow();

    m_textReference->ChangeValue(m_defaultRef);

    UpdateControlState();
}

void dlgCreateScenery::OnToolBar(wxCommandEvent& event) {
    int id = event.GetId();
    ////////////////////////////
    // New Scenery
    if (id == wxID_NEW) {
        if (m_dirtyfile) {
            if (::wxMessageBox(_("Unsaved changes in your current scenery file.\nDo you want to continue?"), _("Question"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION, this)==wxNO)
                return;
        }
        m_SCN = cSCNFile();
        UpdateAll();
        MakeDirty(false);
    }
    ////////////////////////////
    // Open Scenery
    if (id == wxID_OPEN) {
        if (m_dirtyfile) {
            if (::wxMessageBox(_("Unsaved changes in your current scenery file.\nDo you want to continue?"), _("Question"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION, this)==wxNO)
                return;
        }
        wxFileDialog *dialog = new wxFileDialog(
                                   this,
                                   _T("Open Scenery File"),
                                   wxEmptyString,
                                   wxEmptyString,
                                   _T("Scenery Files (*.scn)|*.scn"),
                                   wxFD_FILE_MUST_EXIST|wxFD_CHANGE_DIR,
                                   wxDefaultPosition,
                                   wxSize(600,400)
                               );
        if (m_SCN.filename != wxT("")) {
            dialog->SetFilename(m_SCN.filename.GetFullName());
            dialog->SetDirectory(m_SCN.filename.GetPath());
        } else
            dialog->SetDirectory(::wxGetApp().g_workdir.GetPath());
        if (dialog->ShowModal() == wxID_OK) {
            ::wxGetApp().g_workdir.AssignDir(wxFileName(dialog->GetPath()).GetPath());
            m_SCN.filename = dialog->GetPath();
            if (!m_SCN.Load()) {
                if (m_SCN.error != CSCNFILE_NO_ERROR) {
                    ::wxMessageBox(_("Error loading SCN file."), _("Error"), wxOK | wxICON_ERROR, this);
                } else {
                    // Errors in the models
                    ::wxMessageBox(_("There were warings or errors loading the model files.\nThey will be shown to you when you open the respective model settings."), _("Warning"), wxOK | wxICON_WARNING, this);
                }
            }
            UpdateAll();
            MakeDirty(false);
            UpdateControlState();
        }

        dialog->Destroy();
    }
    ////////////////////////////
    // Save Scenery
    if (id == wxID_SAVE) {
        if (m_SCN.filename == wxT("")) {
            // Shouldn't happen, but you never know...
            id = wxID_SAVEAS;
        } else {
            TransferDataFromWindow();
            if (m_SCN.version < 4) {
                if (::wxMessageBox(_("You are going to overwrite a old version scenery file.\nYou will not be able to open it with older importer versions anymore.\nDo you want to continue?"), _("Warning"), wxYES_NO|wxICON_WARNING|wxNO_DEFAULT, this) == wxNO) {
                    return;
                }
            }
            if (!m_SCN.Save()) {
                ::wxMessageBox(_("Saving scenery file failed. You will be asked for a new file name now."), _("Error"), wxOK|wxICON_ERROR, this);
                id = wxID_SAVEAS;
            } else {
                MakeDirty(false);
                UpdateControlState();
            }
        }
    }
    ////////////////////////////
    // Save Scenery As
    if (id == wxID_SAVEAS) {
        TransferDataFromWindow();
        wxFileDialog *dialog = new wxFileDialog(
                                   this,
                                   _T("Save Scenery File"),
                                   wxEmptyString,
                                   wxEmptyString,
                                   _T("Scenery Files (*.scn)|*.scn"),
                                   wxFD_SAVE|wxFD_OVERWRITE_PROMPT|wxFD_CHANGE_DIR,
                                   wxDefaultPosition,
                                   wxSize(600,400)
                               );
        if (m_SCN.filename != wxT("")) {
            dialog->SetFilename(m_SCN.filename.GetFullName());
            dialog->SetDirectory(m_SCN.filename.GetPath());
        } else
            dialog->SetDirectory(::wxGetApp().g_workdir.GetPath());
        if (dialog->ShowModal() == wxID_OK) {
            ::wxGetApp().g_workdir.AssignDir(wxFileName(dialog->GetPath()).GetPath());
            m_SCN.filename = dialog->GetPath();
            if (!m_SCN.Save()) {
                ::wxMessageBox(_("Error saving SCN file."), _("Error"), wxOK | wxICON_ERROR, this);
            } else {
                MakeDirty(false);
                UpdateControlState();
            }
        }

        dialog->Destroy();
    }
    /////////////////////////////////
    // Load texture ovl into cache
    if (id == tbLoadCache) {
        wxFileDialog *dialog = new wxFileDialog(
                                         this,
                                         _T("Open Scenery File (Load textures into cache)"),
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
                    ::wxMessageBox(_("The selected scenery file contains no textures."), _("Warning"), wxOK | wxICON_WARNING, this);
                    delete texscn;
                    dialog->Destroy();
                    return;
                }
                if (texscn->models.size()) {
                    if (::wxMessageBox(_("The selected scenery file contains models, so it does not seem to be for a texture ovl.\nDo you still want to add the textures to the cache?"), _("Warning"), wxYES_NO | wxNO_DEFAULT | wxICON_WARNING, this) == wxNO) {
                        delete texscn;
                        dialog->Destroy();
                        return;
                    }
                }
                int oldcount = ::wxGetApp().g_texturecache.size();
                for (cFlexiTextureIterator ms = texscn->flexitextures.begin(); ms != texscn->flexitextures.end(); ms++) {
                    ::wxGetApp().g_texturecache.push_back(ms->Name);
                }
                ::wxGetApp().g_texturecache.sort();
                ::wxGetApp().g_texturecache.unique();
                if (::wxGetApp().g_texturecache.size()>oldcount) {
                    ::wxMessageBox(wxString::Format(_("Texture cache updated.\n%d new textures were added to the cache."), ::wxGetApp().g_texturecache.size()-oldcount), _("Texture Cache"), wxOK | wxICON_INFORMATION, this);
                } else {
                    ::wxMessageBox(wxString::Format(_("No new textures in file."), ::wxGetApp().g_texturecache.size()-oldcount), _("Texture Cache"), wxOK | wxICON_INFORMATION, this);
                }
                delete texscn;
            }
        }
        dialog->Destroy();
    }
    /////////////////////////////////
    // Clear texture cache
    if (id == tbClearCache) {
        if (::wxMessageBox(_("Do you really want to clear the texture cache?"), _("Texture Cache"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION, this) == wxYES) {
            ::wxGetApp().g_texturecache.clear();
        }
    }
    ////////////////////////////
    // Close
    if (id == wxID_CANCEL) {
        Close();
    }
}

void dlgCreateScenery::OnShowUnknowns(wxCommandEvent& WXUNUSED(event)) {
    if (m_checkShowUnknown->IsChecked()) {
        if (::wxMessageBox(_("You should better leave these settings alone.\nDo you want to continue?"), _("Warning"), wxYES_NO | wxNO_DEFAULT | wxICON_WARNING, this)==wxNO) {
            m_checkShowUnknown->SetValue(false);
            return;
        }
    }
    bool savedirty = m_dirtyfile;
    TransferDataFromWindow();
    m_panelUnknown->Show( m_checkShowUnknown->IsChecked());
    Fit();
    Layout();
    TransferDataToWindow();
    m_dirtyfile = savedirty;
}

void dlgCreateScenery::OnControlUpdate(wxCommandEvent& WXUNUSED(event)) {
    UpdateControlState();
}

void dlgCreateScenery::OnClose(wxCloseEvent& event) {
    if ((!m_dirtyfile) || (!event.CanVeto())) {
        EndModal(0);
        return;
    }
    if (::wxMessageBox(_("Unsaved changes in your current scenery file.\nDo you want to continue?"), _("Question"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION, this)==wxNO) {
        event.Veto();
    } else {
        EndModal(0);
    }
}

void dlgCreateScenery::OnInitDialog(wxInitDialogEvent& WXUNUSED(event)) {
    TransferDataToWindow();
    m_dirtyfile = false;
}

void dlgCreateScenery::OnMakeDirty(wxCommandEvent& WXUNUSED(event)) {
    MakeDirty();
}

////////////////////////////////////////////////////////////////////////
//
//  dlgCreateScenery, Textures
//
////////////////////////////////////////////////////////////////////////

void dlgCreateScenery::OnTextureUp(wxSpinEvent& WXUNUSED(event)) {
    int sel = m_htlbTexture->GetSelection();
    if (sel < 1)
        return;
    cFlexiTexture ft = m_SCN.flexitextures[sel];
    m_SCN.flexitextures.erase(m_SCN.flexitextures.begin() + sel);
    m_SCN.flexitextures.insert(m_SCN.flexitextures.begin() + sel - 1, ft);

    MakeDirty();
    m_htlbTexture->UpdateContents();
    m_htlbTexture->SetSelection(sel-1);
    UpdateControlState();
}

void dlgCreateScenery::OnTextureDown(wxSpinEvent& WXUNUSED(event)) {
    int count = m_SCN.flexitextures.size();
    int sel = m_htlbTexture->GetSelection();
    if ((count-sel) <= 1)
        return;
    cFlexiTexture ft = m_SCN.flexitextures[sel];
    m_SCN.flexitextures.erase(m_SCN.flexitextures.begin() + sel);
    m_SCN.flexitextures.insert(m_SCN.flexitextures.begin() + sel + 1, ft);

    MakeDirty();
    m_htlbTexture->UpdateContents();
    m_htlbTexture->SetSelection(sel+1);
    UpdateControlState();
}

void dlgCreateScenery::OnTextureAdd(wxCommandEvent& WXUNUSED(event)) {
    dlgTextureBase *dialog = NULL;
    wxAUIPicFileDialog *fdialog = new wxAUIPicFileDialog(
                                     this,
                                     _("Select texture file for new texture"),
                                     wxEmptyString,
                                     wxEmptyString,
                                     _("Supported Image Files|*.png;*.jpg;*.bmp;*.gif;*.jpeg;*.tiff;*.tif|All Files (*.*)|*.*"),
                                     wxFD_FILE_MUST_EXIST|wxFD_CHANGE_DIR,
                                     wxDefaultPosition,
                                     wxSize(600,400)
                                 );
    fdialog->SetDirectory(::wxGetApp().g_workdir.GetPath());
    if (fdialog->ShowModal() == wxID_OK) {
        ::wxGetApp().g_workdir.AssignDir(wxFileName(fdialog->GetPath()).GetPath());

        dialog = new dlgTextureSimple(this);
        dialog->SetTextureName(fdialog->GetPath());
        int res = dialog->ShowModal();
        if (res == wxID_OK) {
            m_SCN.flexitextures.push_back(dialog->GetFlexiTexture());
            MakeDirty();
            m_htlbTexture->UpdateContents();
            m_htlbTexture->SetSelection(m_SCN.flexitextures.size()-1);
            UpdateControlState();
        } else if (res == wxID_SAVE) {
            cFlexiTexture temp = dialog->GetFlexiTexture();
            dialog->Destroy();

            dialog = new dlgTexture(this);
            dialog->SetFlexiTexture(temp);
            if (dialog->ShowModal() == wxID_OK) {
                m_SCN.flexitextures.push_back(dialog->GetFlexiTexture());
                MakeDirty();
                m_htlbTexture->UpdateContents();
                m_htlbTexture->SetSelection(m_SCN.flexitextures.size()-1);
                UpdateControlState();
            }
        }
    } else {
        dialog = new dlgTexture(this);
        if (dialog->ShowModal() == wxID_OK) {
            m_SCN.flexitextures.push_back(dialog->GetFlexiTexture());
            MakeDirty();
            m_htlbTexture->UpdateContents();
            m_htlbTexture->SetSelection(m_SCN.flexitextures.size()-1);
            UpdateControlState();
        }
    }
    fdialog->Destroy();
    dialog->Destroy();
}

void dlgCreateScenery::OnTextureEdit(wxCommandEvent& WXUNUSED(event)) {
    int sel = m_htlbTexture->GetSelection();
    if (sel < 0)
        return;
    dlgTexture *dialog = new dlgTexture(this);
    dialog->SetFlexiTexture(m_SCN.flexitextures[sel]);
    if (dialog->ShowModal() == wxID_OK) {
        m_SCN.flexitextures[sel] = dialog->GetFlexiTexture();
        MakeDirty();
        m_htlbTexture->UpdateContents();
        UpdateControlState();
    }
    dialog->Destroy();
}

void dlgCreateScenery::OnTextureCopy(wxCommandEvent& WXUNUSED(event)) {
    int sel = m_htlbTexture->GetSelection();
    if (sel<0)
        return;

    cFlexiTexture ft = m_SCN.flexitextures[sel];
    ft.Name += _(" Copy");
    m_SCN.flexitextures.insert(m_SCN.flexitextures.begin() + sel + 1, ft);

    MakeDirty();
    m_htlbTexture->UpdateContents();
    m_htlbTexture->SetSelection(sel+1);
    UpdateControlState();
}

void dlgCreateScenery::OnTextureDel(wxCommandEvent& WXUNUSED(event)) {
    int sel = m_htlbTexture->GetSelection();
    if (sel<0)
        return;

    m_SCN.flexitextures.erase(m_SCN.flexitextures.begin() + sel);

    MakeDirty();
    m_htlbTexture->UpdateContents();
    m_htlbTexture->SetSelection(sel-1);
    UpdateControlState();
}

void dlgCreateScenery::OnTextureClear(wxCommandEvent& WXUNUSED(event)) {
    if (m_SCN.flexitextures.size()) {
        if (::wxMessageBox(_("Do you really want to delete all texture entries?"), _("Question"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION, this)==wxNO)
            return;
    }

    m_SCN.flexitextures.clear();

    MakeDirty();
    m_htlbTexture->UpdateContents();
    m_htlbTexture->SetSelection(-1);
    UpdateControlState();
}

////////////////////////////////////////////////////////////////////////
//
//  dlgCreateScenery, Models
//
////////////////////////////////////////////////////////////////////////

void dlgCreateScenery::OnModelUp(wxSpinEvent& WXUNUSED(event)) {
    int sel = m_htlbModel->GetSelection();
    if (sel < 1)
        return;
    cModel ft = m_SCN.models[sel];
    m_SCN.models.erase(m_SCN.models.begin() + sel);
    m_SCN.models.insert(m_SCN.models.begin() + sel - 1, ft);

    MakeDirty();
    m_htlbModel->UpdateContents();
    m_htlbModel->SetSelection(sel-1);
    UpdateControlState();
}

void dlgCreateScenery::OnModelDown(wxSpinEvent& WXUNUSED(event)) {
    int count = m_SCN.models.size();
    int sel = m_htlbModel->GetSelection();
    if ((count-sel) <= 1)
        return;
    cModel ft = m_SCN.models[sel];
    m_SCN.models.erase(m_SCN.models.begin() + sel);
    m_SCN.models.insert(m_SCN.models.begin() + sel + 1, ft);

    MakeDirty();
    m_htlbModel->UpdateContents();
    m_htlbModel->SetSelection(sel+1);
    UpdateControlState();
}

void dlgCreateScenery::OnModelAdd(wxCommandEvent& WXUNUSED(event)) {
    dlgModel *dialog = new dlgModel(this);
    dialog->SetModel(cModel(::wxGetApp().GetConfig()->m_def_matrix));
    if (dialog->ShowModal() == wxID_OK) {
        m_SCN.models.push_back(dialog->GetModel());
        MakeDirty();
        m_htlbModel->UpdateContents();
        m_htlbModel->SetSelection(m_SCN.models.size()-1);
        UpdateControlState();
    }
    dialog->Destroy();
}

void dlgCreateScenery::OnModelEdit(wxCommandEvent& WXUNUSED(event)) {
    int sel = m_htlbModel->GetSelection();
    if (sel < 0)
        return;
    dlgModel *dialog = new dlgModel(this);
    if (m_SCN.models[sel].error.size()) {
        wxString errtext = m_SCN.models[sel].error[0];
        while (m_SCN.models[sel].error.erase(m_SCN.models[sel].error.begin()) != m_SCN.models[sel].error.end()) {
            errtext += wxT("\n\n") + m_SCN.models[sel].error[0];
        }
        ::wxMessageBox(errtext, wxString(m_SCN.models[sel].fatal_error?_("Error"):_("Warning")) + _(" during model file loading"), wxOK | (m_SCN.models[sel].fatal_error?wxICON_ERROR:wxICON_WARNING), this);
        m_SCN.models[sel].error.clear();
    }
    dialog->SetModel(m_SCN.models[sel]);
    if (dialog->ShowModal() == wxID_OK) {
        cModel mod = dialog->GetModel();
        if (mod.name != m_SCN.models[sel].name) {
            for (cLODIterator it = m_SCN.lods.begin(); it != m_SCN.lods.end(); it++) {
                if (it->modelname == m_SCN.models[sel].name)
                    it->modelname = mod.name;
            }
        }
        m_SCN.models[sel] = mod;
        MakeDirty();
        m_htlbModel->UpdateContents();
        m_htlbLOD->UpdateContents();
        UpdateControlState();
    }
    dialog->Destroy();
}

void dlgCreateScenery::OnModelCopy(wxCommandEvent& WXUNUSED(event)) {
    int sel = m_htlbModel->GetSelection();
    if (sel<0)
        return;

    cModel ft = m_SCN.models[sel];
    ft.name += _(" Copy");
    m_SCN.models.insert(m_SCN.models.begin() + sel + 1, ft);

    MakeDirty();
    m_htlbModel->UpdateContents();
    m_htlbModel->SetSelection(sel+1);
    UpdateControlState();
}

void dlgCreateScenery::OnModelDel(wxCommandEvent& WXUNUSED(event)) {
    int sel = m_htlbModel->GetSelection();
    if (sel<0)
        return;

    m_SCN.models.erase(m_SCN.models.begin() + sel);

    MakeDirty();
    m_htlbModel->UpdateContents();
    m_htlbModel->SetSelection(sel-1);
    UpdateControlState();
}

void dlgCreateScenery::OnModelClear(wxCommandEvent& WXUNUSED(event)) {
    if (m_SCN.models.size()) {
        if (::wxMessageBox(_("Do you really want to delete all Model entries?"), _("Question"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION, this)==wxNO)
            return;
    }

    m_SCN.models.clear();

    MakeDirty();
    m_htlbModel->UpdateContents();
    m_htlbModel->SetSelection(-1);
    UpdateControlState();
}

////////////////////////////////////////////////////////////////////////
//
//  dlgCreateScenery, LODs
//
////////////////////////////////////////////////////////////////////////

void dlgCreateScenery::OnLODAdd(wxCommandEvent& WXUNUSED(event)) {
    dlgLOD *dialog = new dlgLOD(m_SCN.models, this);
    int sel = m_htlbModel->GetSelection();
    if (sel >= 0) {
        cLOD lod;
        lod.modelname = m_SCN.models[sel].name;
        dialog->SetLOD(lod);
    }
    if (dialog->ShowModal() == wxID_OK) {
        cLOD nl = dialog->GetLOD();
        bool atend = true;
        int sel = 0;
        for (cLODIterator it = m_SCN.lods.begin(); it != m_SCN.lods.end(); it++) {
            if (nl.distance < it->distance) {
                atend = false;
                m_SCN.lods.insert(it, nl);
                break;
            }
            sel++;
        }
        if (atend)
            m_SCN.lods.push_back(nl);
        MakeDirty();
        m_htlbLOD->UpdateContents();
        m_htlbLOD->SetSelection(sel);
        UpdateControlState();
    }
    dialog->Destroy();
}

void dlgCreateScenery::OnLODEdit(wxCommandEvent& WXUNUSED(event)) {
    int sel = m_htlbLOD->GetSelection();
    if (sel < 0)
        return;
    dlgLOD *dialog = new dlgLOD(m_SCN.models, this);
    dialog->SetLOD(m_SCN.lods[sel]);
    if (dialog->ShowModal() == wxID_OK) {
        m_SCN.lods.erase(m_SCN.lods.begin() + sel);
        cLOD nl = dialog->GetLOD();
        bool atend = true;
        sel = 0;
        for (cLODIterator it = m_SCN.lods.begin(); it != m_SCN.lods.end(); it++) {
            if (nl.distance < it->distance) {
                atend = false;
                m_SCN.lods.insert(it, nl);
                break;
            }
            sel++;
        }
        if (atend)
            m_SCN.lods.push_back(nl);
        MakeDirty();
        m_htlbLOD->UpdateContents();
        m_htlbLOD->SetSelection(sel);
        UpdateControlState();
    }
    dialog->Destroy();
}

void dlgCreateScenery::OnLODCopy(wxCommandEvent& WXUNUSED(event)) {
    int sel = m_htlbLOD->GetSelection();
    if (sel<0)
        return;

    cLOD ft = m_SCN.lods[sel];
    m_SCN.lods.insert(m_SCN.lods.begin() + sel + 1, ft);

    MakeDirty();
    m_htlbLOD->UpdateContents();
    m_htlbLOD->SetSelection(sel+1);
    UpdateControlState();
}

void dlgCreateScenery::OnLODDel(wxCommandEvent& WXUNUSED(event)) {
    int sel = m_htlbLOD->GetSelection();
    if (sel<0)
        return;
    m_SCN.lods.erase(m_SCN.lods.begin() + sel);

    MakeDirty();
    m_htlbLOD->UpdateContents();
    m_htlbLOD->SetSelection(sel-1);
    UpdateControlState();
}

void dlgCreateScenery::OnLODAuto(wxCommandEvent& WXUNUSED(event)) {
    if (m_SCN.models.size()!=1)
        return;
    if (m_SCN.lods.size()) {
        if (::wxMessageBox(_("This will delete all current levels of detail.\nDo you want to continue?"), _("Question"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION, this)==wxNO)
            return;
    }

    m_SCN.lods.clear();

    cLOD ft;
    ft.modelname = m_SCN.models[0].name;
    ft.distance = 40.0;
    m_SCN.lods.push_back(ft);
    ft.distance = 100.0;
    m_SCN.lods.push_back(ft);
    ft.distance = 4000.0;
    m_SCN.lods.push_back(ft);

    MakeDirty();
    m_htlbLOD->UpdateContents();
    m_htlbLOD->SetSelection(-1);
    UpdateControlState();
}

void dlgCreateScenery::OnLODClear(wxCommandEvent& WXUNUSED(event)) {
    if (m_SCN.lods.size()) {
        if (::wxMessageBox(_("Do you really want to delete all levels of detail?"), _("Question"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION, this)==wxNO)
            return;
    }

    m_SCN.lods.clear();

    MakeDirty();
    m_htlbLOD->UpdateContents();
    m_htlbLOD->SetSelection(-1);
    UpdateControlState();
}

////////////////////////////////////////////////////////////////////////
//
//  dlgCreateScenery, LODs
//
////////////////////////////////////////////////////////////////////////

void dlgCreateScenery::OnReferenceAdd(wxCommandEvent& WXUNUSED(event)) {
    m_SCN.references.push_back(m_textReference->GetValue());
    MakeDirty();
    m_htlbReferences->UpdateContents();
    m_htlbReferences->SetSelection(m_SCN.references.size()-1);
    UpdateControlState();
}

void dlgCreateScenery::OnReferenceDel(wxCommandEvent& WXUNUSED(event)) {
    int sel = m_htlbReferences->GetSelection();
    if (sel<0)
        return;
    m_textReference->ChangeValue(m_SCN.references[sel]);
    m_SCN.references.erase(m_SCN.references.begin() + sel);

    MakeDirty();
    m_htlbReferences->UpdateContents();
    m_htlbReferences->SetSelection(sel-1);
    UpdateControlState();
}

void dlgCreateScenery::OnReferenceClear(wxCommandEvent& WXUNUSED(event)) {
    if (m_SCN.references.size()) {
        if (::wxMessageBox(_("Do you really want to delete all references?"), _("Question"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION, this)==wxNO)
            return;
    }

    m_SCN.references.clear();

    MakeDirty();
    m_htlbReferences->UpdateContents();
    m_htlbReferences->SetSelection(-1);
    UpdateControlState();
}

////////////////////////////////////////////////////////////////////////
//
//  dlgCreateScenery, Visual Options
//
////////////////////////////////////////////////////////////////////////

void dlgCreateScenery::OnVisualNormal(wxCommandEvent& WXUNUSED(event)) {
    TransferDataFromWindow();
    m_SCN.sivsettings = cSIVSettings();
    TransferDataToWindow();
    MakeDirty();
    UpdateControlState();
}

void dlgCreateScenery::OnVisualSwayingTree(wxCommandEvent& WXUNUSED(event)) {
    TransferDataFromWindow();
    m_SCN.sivsettings = cSIVSettings(true);
    TransferDataToWindow();
    MakeDirty();
    UpdateControlState();
}

////////////////////////////////////////////////////////////////////////
//
//  dlgCreateScenery, Create Scenery
//
////////////////////////////////////////////////////////////////////////

WX_DECLARE_STRING_HASH_MAP(char *, ovlNameLookup);

void dlgCreateScenery::OnCreate(wxCommandEvent& WXUNUSED(event)) {
    bool textureovl = false;
//    c3DLoader *object = NULL;
    bool error = false;
    bool warning = false;
    bool cont = true;
    bool was_going = false;

    // For progress display
    unsigned int meshes = 0;
    unsigned int textures = 0;
    unsigned int progress_count = 0;

    bool filenameok = false;
    int diaret = wxID_OK;

    cSCNFile work = m_SCN;
    wxFileName sfile;
    wxFileDialog *dialog = NULL;

    wxLogGuiExt *logex = new wxLogGuiExt(&cont);
    wxLog *old = wxLog::SetActiveTarget(logex);

    TransferDataFromWindow();

    /////////////////////////////////////////////////////
    // Sanity Checks & fix the work copy for processing
    {
        wxSortedArrayString usedtextures;
        if (!work.models.size()) {
            if (::wxMessageBox(_("You did not add any models. Do you want to create a texture ovl?"),
                               _("Question"), wxYES_NO | wxICON_QUESTION, this) == wxNO)
                goto deinitlogger;
            textureovl = true;
        } else {
            // Sanity check for model related stuff

            // Check for LODs
            if (!work.lods.size()) {
                error = true;
                wxLogError(_("You didn't set any levels of detail!"));
            }

            // Check the models
            if ((!error) && (!work.models.size())) {
                error = true;
                wxLogError(_("You didn't add any models!"));
            }

            if (!error) {
                cModelMap modnames;
                for (cModelIterator i_mod = work.models.begin(); i_mod != work.models.end(); i_mod++) {
                    // Note: at this stage we issue only warnings. Errors occur when a LOD references
                    //   a broken model
                    if (!(i_mod->file.IsOk() && i_mod->file.FileExists())) {
                        i_mod->fatal_error = true;
                        warning = true;
                        wxLogWarning(_("Model '%s': Model file not found."), i_mod->name.c_str());
                        continue;
                    }
                    c3DLoader *object = c3DLoader::LoadFile(i_mod->file.GetFullPath().fn_str());
                    if (!object) {
                        i_mod->fatal_error = true;
                        warning = true;
                        wxLogWarning(_("Model '%s': Error loading model file."), i_mod->name.c_str());
                        delete object;
                        continue;
                    }
                    delete object;
                    for (cMeshStructIterator i_mesh = i_mod->meshstructs.begin(); i_mesh != i_mod->meshstructs.end(); i_mesh++) {
                        if ((!i_mesh->disabled) && (!i_mesh->valid)) {
                            i_mesh->disabled = true;
                            warning = true;
                            wxLogWarning(_("Model '%s': Mesh '%s' deactivated because it's invalid."), i_mod->name.c_str(), i_mesh->Name.c_str());
                        }
                        if ((!i_mesh->disabled) && ((i_mesh->FTX == wxT("")) || (i_mesh->TXS == wxT("")))) {
                            i_mesh->disabled = true;
                            warning = true;
                            wxLogWarning(_("Model '%s': Mesh '%s' deactivated due to missing data."), i_mod->name.c_str(), i_mesh->Name.c_str());
                        }
                    }
                    // Initialize for the next step
                    modnames[i_mod->name] = &(*i_mod);
                    i_mod->used = false;
                    i_mod->fatal_error = false;
                }

                // Now check the LODs
                for (cLODIterator i_lod = work.lods.begin(); i_lod != work.lods.end(); i_lod++) {
                    cModel *mod = modnames[i_lod->modelname];
                    if (!mod) {
                        error = true;
                        wxLogError(_("Level of Detail '%s' (%.1f): Corresponding model missing."));
                        break;
                    }
                    if (mod->fatal_error) {
                        error = true;
                        wxLogError(_("Level of Detail '%s' (%.1f): Error in corresponding model."));
                        break;
                    }
                    mod->used = true;
                }
            }

            // Last add warnings about unused models and remember texture names for used ones
            if (!error) {
                for (cModelIterator i_mod = work.models.begin(); i_mod != work.models.end(); i_mod++) {
                    if (i_mod->used) {
                        for (cMeshStructIterator i_mesh = i_mod->meshstructs.begin(); i_mesh != i_mod->meshstructs.end(); i_mesh++) {
                            if (!i_mesh->disabled) {
                                usedtextures.Add(i_mesh->FTX);
                                meshes++;
                            }
                        }
                    } else {
                        warning = true;
                        wxLogWarning(_("Model '%s': Unused. It will not be written to the ovl file."), i_mod->name.c_str());
                    }
                }
            }
        }

        // Check the textures
        if (!error) {
            for (cFlexiTextureIterator i_ftx = work.flexitextures.begin(); i_ftx != work.flexitextures.end(); i_ftx++) {
                if ((!textureovl) && (usedtextures.Index(i_ftx->Name) == wxNOT_FOUND)) {
                    // We're not writing a texture ovl and the texture is unused
                    i_ftx->used = false;
                    warning = true;
                    wxLogWarning(_("Texture '%s': Unused. It will not be written to the ovl file."), i_ftx->Name.c_str());
                    continue;
                } else {
                    // Either used or a texture ovl
                    i_ftx->used = true;
                    textures++;
                }

                if (!i_ftx->Frames.size()) {
                    // No texture frames added.
                    error = true;
                    wxLogError(_("Texture '%s': No texture frames defined."), i_ftx->Name.c_str());
                    break;
                }

                // The validators should make these checks unnecessary, but better safe than sorry
                // For performance reasons we leave size checks for later.
                for (cFlexiTextureFrameIterator i_ftxfr = i_ftx->Frames.begin(); i_ftxfr != i_ftx->Frames.end(); i_ftxfr++) {
                    if (!(i_ftxfr->Texture.IsOk() && i_ftxfr->Texture.FileExists())) {
                        error = true;
                        wxLogError(_("Texture '%s': Texture file '%s' not found."), i_ftx->Name.c_str(), i_ftxfr->Texture.GetName().c_str());
                        break;
                    }
                    if ((i_ftxfr->Alpha != wxT("")) && (!(i_ftxfr->Alpha.IsOk() && i_ftxfr->Alpha.FileExists()))) {
                        error = true;
                        wxLogError(_("Texture '%s': Alpha file '%s' not found."), i_ftx->Name.c_str(), i_ftxfr->Alpha.GetName().c_str());
                        break;
                    }
                    // Make sure recolorable flag matches
                    i_ftxfr->Recolorable = i_ftx->Recolorable;
                    // Init for next step
                    i_ftxfr->used = false;
                }

                if (error)
                    break;

                // Check the animation
                if (i_ftx->Animation.size()) {
                    for (unsigned long i = 0; i < i_ftx->Animation.size(); i++) {
                        if (i_ftx->Animation[i] >= i_ftx->Frames.size()) {
                            // Illegal reference
                            error = true;
                            wxLogError(_("Texture '%s': Animation step %d references non-existing frame."), i_ftx->Name.c_str(), i+1);
                            break;
                        }
                        i_ftx->Frames[i_ftx->Animation[i]].used = true;
                    }
                } else {
                    // We need to add one refering to the first frame
                    i_ftx->Animation.push_back(0);
                    i_ftx->Frames[0].used = true;
                }

                if (error)
                    break;

                // Now we go through the frames again and fix unreferenced ones
                // Doing this inverse to make removing easier
                for (long i = i_ftx->Frames.size() - 1; i >=0 ; i--) {
                    if (!i_ftx->Frames[i].used) {
                        // Great -.-
                        i_ftx->Frames.erase(i_ftx->Frames.begin() + i);
                        // Fix animation
                        for (std::vector<unsigned long>::iterator i_anim = i_ftx->Animation.begin(); i_anim != i_ftx->Animation.end(); i_anim++) {
                            if (*i_anim > i)
                                *i_anim -= 1;
                        }
                        warning = true;
                        wxLogWarning(_("Texture '%s', frame %d: Unused. It will not be written to the ovl file."), i_ftx->Name.c_str(), i);
                    }
                }

                // Zero FPS if there is only one animation step
                if (i_ftx->Animation.size()<=1) {
                    i_ftx->FPS = 0;
                }
            }
        }

        // Show warnings & errors
        wxLog::FlushActive();
    }

    if (!cont)
        goto deinitlogger;

    if (work.ovlname == wxT("")) {
        if (!textureovl) {
            if (work.filename != wxT(""))
                sfile = work.filename;
            else
                sfile = work.models[0].file;
            wxString prefixed = m_prefix + sfile.GetName();
            sfile.SetName(prefixed);
            sfile.SetExt(wxT(""));
        } else {
            if (work.filename != wxT(""))
                sfile = work.filename;
            else
                sfile = ::wxGetApp().g_workdir;
            sfile.SetName(m_prefix + m_theme + wxT("-texture"));
            sfile.SetExt(wxT(""));
        }
    } else {
        sfile = work.ovlname;
    }
    //sfile.SetExt(wxT("common.ovl"));
    dialog = new wxFileDialog(
                               this,
                               _T("Save Scenery OVL"),
                               wxEmptyString,
                               wxEmptyString,
                               _T("Scenery OVL (*.common.ovl)|*.common.ovl"),
                               wxFD_SAVE|wxFD_OVERWRITE_PROMPT|wxFD_CHANGE_DIR,
                               wxDefaultPosition,
                               wxSize(600,400)
                           );
    dialog->SetFilename(sfile.GetFullName());
    dialog->SetDirectory(sfile.GetPath());

    while (!(filenameok || (diaret != wxID_OK))) {
        diaret = dialog->ShowModal();
        filenameok = dialog->GetPath().IsAscii();
        if ((!filenameok) && (diaret == wxID_OK))
            ::wxMessageBox(_("The OVL filename may only contain AscII characters.\nOther characters (eg. German umlauts, accented characters, ...) can cause problems and are therefore not allowed."), _("Error"), wxOK|wxICON_ERROR, this);
    }

    if (diaret == wxID_OK) {
        ::wxGetApp().g_workdir.AssignDir(wxFileName(dialog->GetPath()).GetPath());
        wxBusyCursor bc;

        {
            // In some cases the dialog returns .common.ovl.common.ovl -.-
            wxString cleanup = dialog->GetPath();
            cleanup.Replace(wxT(".common.ovl.common.ovl"), wxT(".common.ovl"), true);
            sfile = cleanup;
        }

        was_going = true;

        wxProgressDialog progress(_("Creating Scenery OVL"),
                                  _("Creating Scenery OVL"),
                                  meshes+textures+2,    // range
                                  this,   // parent
                                  wxPD_APP_MODAL |
                                  wxPD_AUTO_HIDE |
                                  wxPD_ELAPSED_TIME |
                                  wxPD_ESTIMATED_TIME |
                                  wxPD_REMAINING_TIME |
                                  wxPD_SMOOTH);

        CreateSceneryOvl(const_cast<char *> (sfile.GetFullPath().fn_str()));

        char *ovlname;
        {
            // Remove .common
            wxFileName sovlname = sfile.GetName();
            ovlname = strdup(sovlname.GetName().fn_str());

            // Build the ovlname for saving in the scn file
            wxString temps = sovlname.GetName();
            sovlname.Assign(sfile.GetPath(), temps, wxT(""));
            if (sovlname.GetFullPath() != m_SCN.ovlname.GetFullPath()) {
                m_SCN.ovlname = sovlname.GetFullPath();
                MakeDirty();
                UpdateControlState();
            }
        }

        ILuint tex[2];
        ILenum Error;

        ilGenImages(2, (ILuint *) & tex);

        // Predeclare pointers for deletion
        std::vector<StaticShape1 *> ss1store;
        std::vector<char *> ssnamestore;
        std::vector<char **> ftxstore;
        std::vector<char **> txsstore;
        std::vector<int> meshcountstore;
        char **Names = NULL;
        unsigned char *svd = NULL;
        std::vector<FlexiTextureInfoStruct *> ftistore;
        std::vector<char *> ftinamestore;

        // Add SVD and SSH
        if (!textureovl) {
            // Hashmap for StaticShape Names
            ovlNameLookup sslookup;

            // Add StaticShapes
            progress.Update(progress_count, _("Adding Models..."));
            for (cModelIterator i_mod = work.models.begin(); i_mod != work.models.end(); i_mod++) {
                if (!i_mod->used)
                    continue;
                char *name = strdup(i_mod->name.c_str());
                ssnamestore.push_back(name);
                sslookup[i_mod->name] = name;

                StaticShape1 *sh = new StaticShape1;
                ss1store.push_back(sh);

                int mesh_count = 0;
                for (cMeshStructIterator i_mesh = i_mod->meshstructs.begin(); i_mesh != i_mod->meshstructs.end(); i_mesh++) {
                    if (!i_mesh->disabled)
                        mesh_count++;
                }

                sh->EffectCount = i_mod->effectpoints.size();
                sh->MeshCount = mesh_count;
                sh->MeshCount2 = mesh_count;
                sh->TotalIndexCount = 0;
                sh->TotalVertexCount = 0;
                boundsInit(&sh->BoundingBox1, &sh->BoundingBox2);
                sh->sh = new StaticShape2 *[mesh_count];
                D3DMATRIX transformMatrix = matrixMultiply(i_mod->transforms);
                bool do_transform = !matrixIsEqual(transformMatrix, matrixGetUnity());

                if (i_mod->effectpoints.size() != 0) {
                    unsigned int e;
                    sh->EffectName = new char *[sh->EffectCount];
                    sh->EffectPosition = new D3DMATRIX[sh->EffectCount];
                    for (e = 0; e < i_mod->effectpoints.size(); e++) {
                        sh->EffectName[e] = strdup(i_mod->effectpoints[e].Name.c_str());

                        if (do_transform) {
                            std::vector<D3DMATRIX> tempstack = i_mod->effectpoints[e].Transform;
                            // to correctely apply the model transformation matrix to effect points we have to
                            // transform in-game objects into modeler-space first by applying the inverse
                            // of the model transformation matrix
                            tempstack.insert(tempstack.begin(), matrixInverse(transformMatrix));
                            // At the end of the stack, transform back
                            tempstack.push_back(transformMatrix);
                            sh->EffectPosition[e] = matrixMultiply(tempstack);
                        } else {
                            D3DMATRIX me = matrixMultiply(i_mod->effectpoints[e].Transform);
                            sh->EffectPosition[e] = me;
                        }
                    }
                } else {
                    sh->EffectName = 0;
                    sh->EffectPosition = 0;
                }

                c3DLoader *object = c3DLoader::LoadFile(i_mod->file.GetFullPath().fn_str());
                if (!object) {
                    // Poof went the model!
                    error = true;
                    wxLogError(_("Something happened to the file of model '%s'."), i_mod->name.c_str());
                    goto scenerycleanup;
                }

                unsigned long CurrentMesh = 0;
                unsigned long CurrentObj = 0;
                for (cMeshStructIterator i_mesh = i_mod->meshstructs.begin(); i_mesh != i_mod->meshstructs.end(); i_mesh++) {
                    if (i_mesh->disabled == false) {
                        progress.Update(++progress_count);
                        StaticShape2 *sh2 = new StaticShape2;
                        sh->sh[CurrentMesh] = sh2;
                        sh2->fts = 0;
                        sh2->PlaceTexturing = i_mesh->place;
                        sh2->TextureData = 0;
                        sh2->textureflags = i_mesh->flags;
                        sh2->unk1 = 0xFFFFFFFF;
                        sh2->unk4 =i_mesh->unknown;

                        object->FetchObject(CurrentObj, &sh2->VertexCount, &sh2->Vertexes, &sh2->IndexCount,
                                            &sh2->Triangles, &sh->BoundingBox1, &sh->BoundingBox2,
                                            const_cast<D3DMATRIX *> ((do_transform)?(&transformMatrix):NULL));

                        CurrentMesh++;
                    }
                    CurrentObj++;
                }

                char **ftx = new char *[mesh_count];
                ftxstore.push_back(ftx);
                char **txs = new char *[mesh_count];
                txsstore.push_back(txs);
                meshcountstore.push_back(mesh_count);

                CurrentObj = 0;
                for (cMeshStructIterator ms = i_mod->meshstructs.begin();
                        ms != i_mod->meshstructs.end(); ms++) {
                    if (ms->disabled == false) {
                        ftx[CurrentObj] = strdup(ms->FTX.c_str());
                        txs[CurrentObj] = strdup(ms->TXS.c_str());
                        CurrentObj++;
                    }
                }
                AddStaticShape(name, ftx, txs, sh);
            }

            // SVD
            progress.Update(++progress_count, _("Adding Levels of Detail..."));

            // First, calculate the size
            unsigned long scenerysize = sizeof(SceneryItemVisual);
            // Pointers
            scenerysize += work.lods.size() * 4;
            // SceneryItemVisualLOD structs
            scenerysize += work.lods.size() * sizeof(SceneryItemVisualLOD);
            svd = new unsigned char[scenerysize];
            memset(svd, 0, scenerysize);

            Names = new char* [work.lods.size()];

            // Define convenience pointers
            SceneryItemVisual *sv = (SceneryItemVisual *) svd;
            SceneryItemVisualLOD **svlod = (SceneryItemVisualLOD **) (svd + sizeof(SceneryItemVisual));

            // Set up SceneryItemVisual
            sv->sivflags = work.sivsettings.sivflags;
            sv->sway = work.sivsettings.sway;
            sv->brightness = work.sivsettings.brightness;
            sv->unk4 = work.sivsettings.unknown;
            sv->scale = work.sivsettings.scale;
            sv->LODCount = work.lods.size();
            sv->LODMeshes = svlod;
            sv->unk6 = work.sivsettings.unk6;
            sv->unk7 = work.sivsettings.unk7;
            sv->unk8 = work.sivsettings.unk8;
            sv->unk9 = work.sivsettings.unk9;
            sv->unk10 = work.sivsettings.unk10;
            sv->unk11 = work.sivsettings.unk11;

            for (unsigned int i = 0; i < work.lods.size(); i++) {
                svlod[i] = (SceneryItemVisualLOD *) (svd + sizeof(SceneryItemVisual) + (work.lods.size() * 4) + (i * sizeof(SceneryItemVisualLOD)));
                svlod[i]->MeshType = 0;
                svlod[i]->LODName = ovlname;
                svlod[i]->StaticShape = 0;
                svlod[i]->unk2 = work.lods[i].unk2;
                svlod[i]->BoneShape = 0;
                svlod[i]->unk4 = work.lods[i].unk4;
                svlod[i]->fts = 0;
                svlod[i]->TextureData = 0;
                svlod[i]->unk7 = 1.0;
                svlod[i]->unk8 = 1.0;
                svlod[i]->unk9 = 0;
                svlod[i]->unk10 = 1.0;
                svlod[i]->unk11 = 0;
                svlod[i]->unk12 = 1.0;
                svlod[i]->distance = work.lods[i].distance;
                svlod[i]->AnimationCount = 0;
                svlod[i]->unk14 = work.lods[i].unk14;
                svlod[i]->AnimationArray = 0;
                char *name = sslookup[work.lods[i].modelname];
                if (!name) {
                    // A bug flew in...
                    error = true;
                    wxLogError(_("Internal bug (name went poof! on LOD '%s'). Please report."), work.lods[i].modelname.c_str());
                    goto scenerycleanup;
                }
                Names[i] = name;
            }
            AddSceneryItemVisual(ovlname, Names, NULL, NULL, sv);

        }

        // Add Textures
        progress.Update(progress_count, _("Adding Textures..."));
        for (cFlexiTextureIterator i_ftx = work.flexitextures.begin(); i_ftx != work.flexitextures.end(); i_ftx++) {
            if (!i_ftx->used)
                continue;

            progress.Update(++progress_count);

            if (textureovl) {
                ::wxGetApp().g_texturecache.push_back(i_ftx->Name);
            }

            FlexiTextureInfoStruct *fti = new FlexiTextureInfoStruct;
            ftistore.push_back(fti); // Store for deletion
            char *FlexiTextureName = strdup(i_ftx->Name.c_str());
            ftinamestore.push_back(FlexiTextureName);
            fti->unk1 = i_ftx->FPS;
            fti->Recolorable = i_ftx->Recolorable;

            // Handle Animation
            fti->offsetCount = i_ftx->Animation.size();
            fti->offset1 = new unsigned long[i_ftx->Animation.size()];
            for (unsigned long i = 0; i < i_ftx->Animation.size(); i++) {
                fti->offset1[i] = i_ftx->Animation[i];
            }

            FlexiTextureStruct *fts = new FlexiTextureStruct[i_ftx->Frames.size()];
            // Zero it, otherwise freeing will cause segfaults in case of error
            memset(fts, 0, i_ftx->Frames.size() * sizeof(FlexiTextureStruct));
            fti->fts2Count = i_ftx->Frames.size();
            fti->fts2 = fts;

            // needs to be filled in the loop
            fti->scale = 0;
            fti->height = 0;
            fti->width = 0;

            wxMutexLocker lock (wxILMutex);

            // Now loop through the frames
            unsigned int m = 0;
            for (cFlexiTextureFrameIterator i_ftxfr = i_ftx->Frames.begin(); i_ftxfr != i_ftx->Frames.end(); i_ftxfr++) {
                ilBindImage(tex[0]);

                if (!ilLoadImage(i_ftxfr->Texture.GetFullPath().fn_str())) {
                    error = true;
                    wxLogError(_("Can't open texture '%s'. devIL errors follow:"), i_ftxfr->Texture.GetFullPath().fn_str());
                    while ((Error = ilGetError())) {
                        wxLogError(iluErrorString(Error));
                    }
                    goto scenerycleanup;
                }
                int width = ilGetInteger(IL_IMAGE_WIDTH);
                int height = ilGetInteger(IL_IMAGE_HEIGHT);
                if (width != height) {
                    error = true;
                    wxLogError(_("Texture '%s' is not square."), i_ftxfr->Texture.GetFullPath().fn_str());
                    goto scenerycleanup;
                }
                if ((1 << local_log2(width)) != width) {
                    error = true;
                    wxLogError(_("The width/height of texture '%s' is not a power of 2."), i_ftxfr->Texture.GetFullPath().fn_str());
                    goto scenerycleanup;
                }
                if (fti->height == 0) {
                    fti->scale = local_log2(width);
                    fti->height = height;
                    fti->width = width;
                }
                if (width != fti->width) {
                    error = true;
                    wxLogError(_("Texture '%s' size is different from sise of the first frame."), i_ftxfr->Texture.GetFullPath().fn_str());
                    goto scenerycleanup;
                }
                if (ilGetInteger(IL_IMAGE_FORMAT) != IL_COLOUR_INDEX) {
                    if (i_ftxfr->Recolorable) {
                        if (ilGetInteger(IL_IMAGE_FORMAT) != IL_RGB)
                            ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);
                        ILubyte *tmpdata = new ILubyte[width * height];
                        ILubyte *picdata = ilGetData();
                        unsigned int paldist, palmax;
                        int r, d1, d2, d3;
                        int p;
                        for (r = 0; r < height * width * 3; r += 3) {
                            palmax = UINT_MAX;
                            for (p = 1; p < 256; p++) {	// Start with 1, do not assign white
                                d1 = (int) picdata[r] - (int) ::wxGetApp().g_recolpalette_rgb[p].rgbRed;
                                d2 = (int) picdata[r + 1] -
                                     (int) ::wxGetApp().g_recolpalette_rgb[p].rgbGreen;
                                d3 = (int) picdata[r + 2] -
                                     (int) ::wxGetApp().g_recolpalette_rgb[p].rgbBlue;
                                paldist = d1 * d1 + d2 * d2 + d3 * d3;
                                if (paldist < palmax) {
                                    palmax = paldist;
                                    tmpdata[r / 3] = p;
                                }
                            }
                        }

                        ILuint temppic = ilGenImage();
                        ilBindImage(temppic);
                        ilCopyImage(tex[0]);
                        if (iluColorsUsed() > 256)
                            ilSetInteger(IL_QUANTIZATION_MODE, IL_NEU_QUANT);
                        else
                            ilSetInteger(IL_QUANTIZATION_MODE, IL_WU_QUANT);
                        ilConvertImage(IL_COLOUR_INDEX, IL_UNSIGNED_BYTE);
                        ilSetData(tmpdata);
                        ilConvertPal(IL_PAL_BGR32);
                        memcpy(ilGetPalette(), &(::wxGetApp().g_recolpalette_bmy), 256 * 4);
                        ilBindImage(tex[0]);
                        ilCopyImage(temppic);
                        ilDeleteImage(temppic);
                        delete[] tmpdata;
                    } else {
                        if (iluColorsUsed() > 256)
                            ilSetInteger(IL_QUANTIZATION_MODE, IL_NEU_QUANT);
                        else
                            ilSetInteger(IL_QUANTIZATION_MODE, IL_WU_QUANT);
                        if (!ilConvertImage(IL_COLOUR_INDEX, IL_UNSIGNED_BYTE)) {
                            error = true;
                            wxLogError(_("Couldn't convert texture '%s' to indexed colour."), i_ftxfr->Texture.GetFullPath().fn_str());
                            while ((Error = ilGetError())) {
                                wxLogError(iluErrorString(Error));
                            }
                            goto scenerycleanup;
                        }
                    }
                }
                if (ilGetInteger(IL_PALETTE_TYPE) != IL_PAL_BGR32)
                    if (!ilConvertPal(IL_PAL_BGR32)) {
                        error = true;
                        wxLogError(_("Couldn't convert palette of texture '%s'."), i_ftxfr->Texture.GetFullPath().fn_str());
                        while ((Error = ilGetError())) {
                            wxLogError(iluErrorString(Error));
                        }
                        goto scenerycleanup;
                    }
                if (ilGetInteger(IL_IMAGE_ORIGIN) == IL_ORIGIN_UPPER_LEFT)
                    if (!iluFlipImage()) {
                        error = true;
                        wxLogError(_("Couldn't flip texture '%s'."), i_ftxfr->Texture.GetFullPath().fn_str());
                        while ((Error = ilGetError())) {
                            wxLogError(iluErrorString(Error));
                        }
                        goto scenerycleanup;
                    }
                bool AlphaPresent = false;
                if (i_ftxfr->Alpha != wxT("")) {
                    AlphaPresent = true;
                }
                if (AlphaPresent == true) {
                    ilBindImage(tex[1]);

                    if (!ilLoadImage(i_ftxfr->Alpha.GetFullPath().fn_str())) {
                        error = true;
                        wxLogError(_("Can't open alpha texture '%s'."), i_ftxfr->Alpha.GetFullPath().fn_str());
                        while ((Error = ilGetError())) {
                            wxLogError(iluErrorString(Error));
                        }
                        goto scenerycleanup;
                    }
                    int alwidth = ilGetInteger(IL_IMAGE_WIDTH);
                    int alheight = ilGetInteger(IL_IMAGE_HEIGHT);
                    if ((width != alwidth) || (height != alheight)) {
                        error = true;
                        wxLogError(_("Size of alpha texture '%s' does not match with '%s'."),
                                   i_ftxfr->Alpha.GetFullPath().fn_str(), i_ftxfr->Texture.GetFullPath().fn_str());
                        goto scenerycleanup;
                    }
                    if (ilGetInteger(IL_IMAGE_FORMAT) != IL_LUMINANCE)
                        if (!ilConvertImage(IL_LUMINANCE, IL_UNSIGNED_BYTE)) {
                            error = true;
                            wxLogError(_("Couldn't convert alpha texture '%s' to grayscale."), i_ftxfr->Alpha.GetFullPath().fn_str());
                            while ((Error = ilGetError())) {
                                wxLogError(iluErrorString(Error));
                            }
                            goto scenerycleanup;
                        }
                    if (ilGetInteger(IL_IMAGE_ORIGIN) == IL_ORIGIN_UPPER_LEFT)
                        if (!iluFlipImage()) {
                            error = true;
                            wxLogError(_("Couldn't flip alpha texture '%s'."), i_ftxfr->Alpha.GetFullPath().fn_str());
                            while ((Error = ilGetError())) {
                                wxLogError(iluErrorString(Error));
                            }
                            goto scenerycleanup;
                        }
                }

                RGBQUAD *colors;
                colors = new RGBQUAD[256];
                memset(colors, 0, 256 * sizeof(RGBQUAD));
                unsigned char *texture = 0;
                unsigned char *alphadata = 0;
                texture = new unsigned char[width * height];
                ilBindImage(tex[0]);
                memcpy(texture, ilGetData(), width * height);
                memcpy(colors, ilGetPalette(), ilGetInteger(IL_PALETTE_NUM_COLS) * 4);
                for (unsigned int j = 0; j < 256; j++)
                    colors[j].rgbReserved = i_ftxfr->AlphaCutoff;
                if (AlphaPresent == true) {
                    alphadata = new unsigned char[width * height];
                    ilBindImage(tex[1]);
                    memcpy(alphadata, ilGetData(), width * height);
                }

                fts[m].scale = local_log2(width);
                fts[m].width = width;
                fts[m].height = height;
                fts[m].Recolorable = i_ftxfr->Recolorable;
                fts[m].palette = (unsigned char *) colors;
                fts[m].texture = texture;
                fts[m].alpha = alphadata;

                m++;
            }

            AddFlexiTexture(FlexiTextureName, fti);
        }

        progress.Update(progress_count, _("Adding References..."));
        for (unsigned int i = 0; i < work.references.size(); i++) {
            AddReference(const_cast<char *> (work.references[i].c_str()));
        }
        progress.Update(++progress_count, _("Saving..."));
        SaveSceneryOvl();

scenerycleanup:
        ilDeleteImages(2, (ILuint *) & tex);
        if (textureovl) {
            ::wxGetApp().g_texturecache.sort();
            ::wxGetApp().g_texturecache.unique();
        }
        free(ovlname);
        delete[] Names;
        while (ssnamestore.size()) {
            char* name = ssnamestore.back();
            free(name);
            ssnamestore.pop_back();
        }
        while (ss1store.size()) {
            StaticShape1 *sh = ss1store.back();
            ss1store.pop_back();
            for (unsigned int i = 0; i < sh->MeshCount; i++) {
                delete[] sh->sh[i]->Vertexes;
                delete[] sh->sh[i]->Triangles;
                delete sh->sh[i];
            }
            delete[] sh->sh;
            for (unsigned int i = 0; i < sh->EffectCount; i++)
                free(sh->EffectName[i]);
            delete[] sh->EffectName;
            delete[] sh->EffectPosition;
            delete sh;
        }
        for (unsigned int i = 0; i < ftxstore.size(); i++) {
            char **ftx = ftxstore[i];
            char **txs = txsstore[i];
            int mesh_count = meshcountstore[i];

            for (unsigned int j = 0; j < mesh_count; j++) {
                free(ftx[j]);
                free(txs[j]);
            }
            delete[] ftx;
            delete[] txs;
        }
        ftxstore.clear();
        txsstore.clear();
        meshcountstore.clear();

        delete[] svd;
        while (ftistore.size()) {
            FlexiTextureInfoStruct *ftid = ftistore.back();
            ftistore.pop_back();
            delete[] ftid->offset1;
            for (unsigned int i = 0; i < ftid->fts2Count; i++) {
                delete[] ftid->fts2[i].palette;
                delete[] ftid->fts2[i].texture;
                delete[] ftid->fts2[i].alpha;
            }
            delete ftid->fts2;
            delete ftid;
        }
        while (ftinamestore.size()) {
            free(ftinamestore.back());
            ftinamestore.pop_back();
        }
    }
    dialog->Destroy();
deinitlogger:
    if (was_going) {
        if (error) {
            wxLog::FlushActive();
        } else {
            wxString msg = _("Saving ");
            msg += (textureovl)?_("Texture"):_("Scenery");
            msg += _(" OVL successful.");
//            ::wxMessageBox(msg);
            wxPopupTransientMessageWindow *popup = new wxPopupTransientMessageWindow(msg, wxT(""), this);
            popup->Popup();
        }
    }
    wxLog::SetActiveTarget(old);
    delete logex;
}
