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

#include <boost/shared_ptr.hpp>
#include <wx/artprov.h>
#include <wx/progdlg.h>
#include <wx/valgen.h>
#include <wx/tooltip.h>

#include <algorithm>
#include <exception>

//#include <IL/il.h>
//#include <IL/ilu.h>

#include "auipicfiledlg.h"
#include "confhelp.h"
#include "fileselectorcombowrap.h"
#include "htmlentities.h"
#include "lib3Dconfig.h"
#include "logext.h"
#include "matrix.h"
#include "OVLException.h"
#include "popupmessagewin.h"
#include "pretty.h"
#include "rct3log.h"
#include "texcheck.h"
#include "valext.h"
#include "wxgmagick.h"

#include "OVL.H"

#include "wxapp.h"
#include "wxdlgAnimation.h"
#include "wxdlgLOD.h"
#include "wxdlgModel.h"
#include "wxdlgSave.h"
#include "wxdlgSpline.h"
#include "wxdlgTexture.h"

const long idToolBar = ::wxNewId();
const long tbLoadCache = ::wxNewId();
const long tbClearCache = ::wxNewId();

using namespace pretty;

////////////////////////////////////////////////////////////////////////
//
//  dlgCreateScenery
//
////////////////////////////////////////////////////////////////////////
/*
BEGIN_EVENT_TABLE(dlgCreateScenery,wxDialog)
EVT_TOOL(wxID_ANY, dlgCreateScenery::OnToolBar)
EVT_CLOSE(dlgCreateScenery::OnClose)
EVT_INIT_DIALOG(dlgCreateScenery::OnInitDialog)
EVT_CHECKBOX(XRCID("m_checkShowUnknown"), dlgCreateScenery::OnShowUnknowns)
// Update Control En-/Disabeling when the listboxes are clicked
EVT_LISTBOX(XRCID("m_htlbTexture"), dlgCreateScenery::OnControlUpdate)
EVT_LISTBOX(XRCID("m_htlbReferences"), dlgCreateScenery::OnControlUpdate)
EVT_LISTBOX(XRCID("m_htlbModel"), dlgCreateScenery::OnControlUpdate)
EVT_LISTBOX(XRCID("m_htlbAModel"), dlgCreateScenery::OnControlUpdate)
EVT_LISTBOX(XRCID("m_htlbLOD"), dlgCreateScenery::OnControlUpdate)
EVT_LISTBOX(XRCID("m_htlbAnimation"), dlgCreateScenery::OnControlUpdate)
// Texture Controls
EVT_SPIN_UP(XRCID("m_spinTexture"), dlgCreateScenery::OnTextureUp)
EVT_SPIN_DOWN(XRCID("m_spinTexture"), dlgCreateScenery::OnTextureDown)
EVT_BUTTON(XRCID("m_btTextureAdd"), dlgCreateScenery::OnTextureAdd)
EVT_BUTTON(XRCID("m_btTextureEdit"), dlgCreateScenery::OnTextureEdit)
EVT_LISTBOX_DCLICK(XRCID("m_htlbTexture"), dlgCreateScenery::OnTextureEdit)
EVT_BUTTON(XRCID("m_btTextureCopy"), dlgCreateScenery::OnTextureCopy)
EVT_BUTTON(XRCID("m_btTextureDel"), dlgCreateScenery::OnTextureDel)
EVT_BUTTON(XRCID("m_btTextureClear"), dlgCreateScenery::OnTextureClear)
// Reference Controls
EVT_BUTTON(XRCID("m_btReferenceAdd"), dlgCreateScenery::OnReferenceAdd)
EVT_BUTTON(XRCID("m_btReferenceDel"), dlgCreateScenery::OnReferenceDel)
EVT_BUTTON(XRCID("m_btReferenceClear"), dlgCreateScenery::OnReferenceClear)
// Model Controls
EVT_SPIN_UP(XRCID("m_spinModel"), dlgCreateScenery::OnModelUp)
EVT_SPIN_DOWN(XRCID("m_spinModel"), dlgCreateScenery::OnModelDown)
EVT_BUTTON(XRCID("m_btModelAdd"), dlgCreateScenery::OnModelAdd)
EVT_BUTTON(XRCID("m_btModelEdit"), dlgCreateScenery::OnModelEdit)
EVT_LISTBOX_DCLICK(XRCID("m_htlbModel"), dlgCreateScenery::OnModelEdit)
EVT_BUTTON(XRCID("m_btModelCopy"), dlgCreateScenery::OnModelCopy)
EVT_BUTTON(XRCID("m_btModelConvert"), dlgCreateScenery::OnModelConvert)
EVT_BUTTON(XRCID("m_btModelDel"), dlgCreateScenery::OnModelDel)
EVT_BUTTON(XRCID("m_btModelClear"), dlgCreateScenery::OnModelClear)
// Animated Model Controls
EVT_SPIN_UP(XRCID("m_spinAModel"), dlgCreateScenery::OnAModelUp)
EVT_SPIN_DOWN(XRCID("m_spinAModel"), dlgCreateScenery::OnAModelDown)
EVT_BUTTON(XRCID("m_btAModelAdd"), dlgCreateScenery::OnAModelAdd)
EVT_BUTTON(XRCID("m_btAModelEdit"), dlgCreateScenery::OnAModelEdit)
EVT_LISTBOX_DCLICK(XRCID("m_htlbAModel"), dlgCreateScenery::OnAModelEdit)
EVT_BUTTON(XRCID("m_btAModelCopy"), dlgCreateScenery::OnAModelCopy)
EVT_BUTTON(XRCID("m_btAModelConvert"), dlgCreateScenery::OnAModelConvert)
EVT_BUTTON(XRCID("m_btAModelDel"), dlgCreateScenery::OnAModelDel)
EVT_BUTTON(XRCID("m_btAModelClear"), dlgCreateScenery::OnAModelClear)
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
// Animation Controls
EVT_SPIN_UP(XRCID("m_spinAnimation"), dlgCreateScenery::OnAnimationUp)
EVT_SPIN_DOWN(XRCID("m_spinAnimation"), dlgCreateScenery::OnAnimationDown)
EVT_BUTTON(XRCID("m_btAnimationAdd"), dlgCreateScenery::OnAnimationAdd)
EVT_BUTTON(XRCID("m_btAnimationEdit"), dlgCreateScenery::OnAnimationEdit)
EVT_LISTBOX_DCLICK(XRCID("m_htlbAnimation"), dlgCreateScenery::OnAnimationEdit)
EVT_BUTTON(XRCID("m_btAnimationCopy"), dlgCreateScenery::OnAnimationCopy)
EVT_BUTTON(XRCID("m_btAnimationDel"), dlgCreateScenery::OnAnimationDel)
EVT_BUTTON(XRCID("m_btAnimationAddToLod"), dlgCreateScenery::OnAnimationAddToLod)
EVT_BUTTON(XRCID("m_btAnimationClear"), dlgCreateScenery::OnAnimationClear)

EVT_TEXT(XRCID("m_textName"), dlgCreateScenery::OnMakeDirty)
EVT_BUTTON(XRCID("m_btAutoName"), dlgCreateScenery::OnAutoName)
EVT_COMBOBOX(XRCID("m_textPath"), dlgCreateScenery::OnMakeDirty)
EVT_BUTTON(XRCID("m_btCheck"), dlgCreateScenery::OnCheck)
EVT_BUTTON(XRCID("m_btCreate"), dlgCreateScenery::OnCreate)

END_EVENT_TABLE()

*/

dlgCreateScenery::dlgCreateScenery(wxWindow *parent):rcdlgCreateScenery(parent) {
    m_tbCS = NULL;

//    InitWidgetsFromXRC((wxWindow *)parent);
    SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);

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

//    m_htlbTexture = new wxTextureListBox(m_panTextures, &m_SCN);
//    m_htlbTexture->SetToolTip(_("Texture list\nTextures from the texture chache are shown in grey."));
//    wxXmlResource::Get()->AttachUnknownControl(wxT("m_htlbTexture"), m_htlbTexture, m_panTextures);
//    m_htlbReferences = new wxReferenceListBox(m_panReferences, &m_SCN);
//    wxXmlResource::Get()->AttachUnknownControl(wxT("m_htlbReferences"), m_htlbReferences, m_panReferences);
    m_htlbTexture->Init(&m_SCN);
    m_htlbReferences->Init(&m_SCN);

//    m_htlbModel = new wxModelListBox(m_panSModel, &m_SCN);
//    m_htlbModel->SetToolTip(_("Static model list\nOrange: Warnings occured during loading the model form the scenery file.\nRed: An error occured during loading the model form the scenery file. Usually the model file was not found."));
//    wxXmlResource::Get()->AttachUnknownControl(wxT("m_htlbModel"), m_htlbModel, m_panSModel);
//    m_htlbAModel = new wxAnimatedModelListBox(m_panAModel, &m_SCN);
//    m_htlbAModel->SetToolTip(_("Animated model list\nOrange: Warnings occured during loading the model form the scenery file.\nRed: An error occured during loading the model form the scenery file. Usually the model file was not found."));
//    wxXmlResource::Get()->AttachUnknownControl(wxT("m_htlbAModel"), m_htlbAModel, m_panAModel);
    m_htlbModel->Init(&m_SCN);
    m_htlbAModel->Init(&m_SCN);

//    m_htlbLOD = new wxLODListBox(this, &m_SCN);
//    wxXmlResource::Get()->AttachUnknownControl(wxT("m_htlbLOD"), m_htlbLOD, this);
    m_htlbLOD->Init(&m_SCN);

//    wxTextCtrl *t_text = XRCCTRL(*this,"m_textPath",wxTextCtrl);
/*
    wxFileDialog *filedlg = new wxFileDialog(
                               this,
                               _("Select OVL Save File"),
                               wxEmptyString,
                               wxEmptyString,
                               _("Scenery OVL File (*.common.ovl)|*.common.ovl"),
                               wxFD_CHANGE_DIR,
                               wxDefaultPosition,
                               wxSize(600,400)
                           );
*/
    wxDirDialog *dirdlg = new wxDirDialog(
                               this,
                               _("Select OVL Save Directory"),
                               wxEmptyString,
                               wxDD_CHANGE_DIR | wxDD_DIR_MUST_EXIST | wxDD_NEW_DIR_BUTTON,
                               wxDefaultPosition,
                               wxDefaultSize
                           );
/*
    m_textPath = new wxFileSelectorCombo<wxDirDialog>(this, dirdlg, &::wxGetApp().g_workdir, XRCID("m_textPath"),
                                         wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
*/
    m_textPath->Assign(dirdlg, &::wxGetApp().g_workdir);
    m_textPath->GetTextCtrl()->SetEditable(false);
//    t_text->GetContainingSizer()->Replace(t_text, m_textPath);
//    t_text->Destroy();

    m_textPrefix->SetValidator(wxExtendedValidator(&m_SCN.prefix));
    m_textName->SetValidator(wxExtendedValidator(&m_SCN.name));
    m_textPath->SetValidator(wxExtendedValidator(&m_SCN.ovlpath, true, true, true));

//    m_htlbAnimation = new wxAnimationsListBox(this, &m_SCN);
//    wxXmlResource::Get()->AttachUnknownControl(wxT("m_htlbAnimation"), m_htlbAnimation, this);
    m_htlbAnimation->Init(&m_SCN);
    m_htlbSpline->Init(&m_SCN);

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

    Fit();
    Layout();
    GetSizer()->SetSizeHints(this);
    m_panTextures->GetSizer()->Layout();
    m_panReferences->GetSizer()->Layout();
    m_panSModel->GetSizer()->Layout();
    m_panAModel->GetSizer()->Layout();

    UpdateAll();

    SetSize(READ_APP_SIZE("CreateSceney", GetSize()));
    Center();

    wxToolTip::Enable(READ_APP_TOOLTIPS());

    Connect(wxID_ANY, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( dlgCreateScenery::OnToolBar ));

    m_dirtyfile = false;
}

dlgCreateScenery::~dlgCreateScenery() {
    WRITE_APP_SIZE("CreateSceney", GetSize());
}

wxString dlgCreateScenery::GetOVLPath() {
    if (m_SCN.ovlpath.GetPath() != wxT(""))
        return m_SCN.ovlpath.GetPathWithSep() + m_SCN.prefix + m_SCN.name + wxT(".common.ovl");
    else
        return wxEmptyString;
}

void dlgCreateScenery::UpdateControlState() {
    int count = m_SCN.flexitextures.size();
    int sel = m_htlbTexture->GetSelection();
    m_spinTexture->Enable((sel>=0) && (count>=2));
    m_btTextureEdit->Enable((sel>=0) && (sel<count));
    m_btTextureCopy->Enable((sel>=0) && (sel<count));
    m_btTextureDel->Enable((sel>=0) && (sel<count));
    m_btTextureClear->Enable(count>=1);
    m_nbTexRef->SetPageText(0, wxString::Format("Textures (%d)", count));

    count = m_SCN.references.size();
    sel = m_htlbReferences->GetSelection();
    m_btReferenceDel->Enable(sel>=0);
    m_btReferenceClear->Enable(count>=1);
    m_nbTexRef->SetPageText(1, wxString::Format("References (%d)", count));

    count = m_SCN.models.size();
    sel = m_htlbModel->GetSelection();
    m_spinModel->Enable((sel>=0) && (count>=2));
    m_btModelEdit->Enable(sel>=0);
    m_btModelCopy->Enable(sel>=0);
    m_btModelConvert->Enable(sel>=0);
    m_btModelDel->Enable(sel>=0);
    m_btModelClear->Enable(count>=1);
    m_btLODAdd->Enable(count>=1);
    m_btLODAuto->Enable((count==1) && (!m_SCN.animatedmodels.size()));
    m_nbModels->SetPageText(0, wxString::Format("Static Models (%d)", count));

    count = m_SCN.animatedmodels.size();
    sel = m_htlbAModel->GetSelection();
    m_spinAModel->Enable((sel>=0) && (count>=2));
    m_btAModelEdit->Enable(sel>=0);
    m_btAModelCopy->Enable(sel>=0);
    m_btAModelConvert->Enable(sel>=0);
    m_btAModelDel->Enable(sel>=0);
    m_btAModelClear->Enable(count>=1);
    if (count>=1)
        m_btLODAdd->Enable(true);
    m_nbModels->SetPageText(1, wxString::Format("Animated Models (%d)", count));

    count = m_SCN.lods.size();
    sel = m_htlbLOD->GetSelection();
    m_btLODEdit->Enable(sel>=0);
    m_btLODCopy->Enable(sel>=0);
    m_btLODDel->Enable(sel>=0);
    m_btLODClear->Enable(count>=1);
    m_nbLodOptions->SetPageText(0, wxString::Format("LODs (%d)", count));

    count = m_SCN.animations.size();
    sel = m_htlbAnimation->GetSelection();
    m_spinAnimation->Enable((sel>=0) && (count>=2));
    m_btAnimationEdit->Enable((sel>=0) && (sel<count));
    m_btAnimationCopy->Enable((sel>=0) && (sel<count));
    m_btAnimationDel->Enable((sel>=0) && (sel<count));
    m_btAnimationClear->Enable(count>=1);
    bool en = false;
    if (sel >= 0) {
        int l = m_htlbLOD->GetSelection();
        if (l >= 0) {
            if (m_SCN.lods[l].animated) {
                cAnimation* pani = &m_SCN.animations[sel];
                if (find_in(m_SCN.lods[l].animations, pani->name) == m_SCN.lods[l].animations.end()) {
                    en = true;
                }
            }
        }
    }
    m_btAnimationAddToLod->Enable(en);
    m_nbAnSpline->SetPageText(0, wxString::Format("Animations (%d)", count));

    count = m_SCN.splines.size();
    sel = m_htlbSpline->GetSelection();
    m_spinSpline->Enable((sel>=0) && (count>=2));
    m_btSplineEdit->Enable((sel>=0) && (sel<count));
    m_btSplineCopy->Enable((sel>=0) && (sel<count));
    m_btSplineDel->Enable((sel>=0) && (sel<count));
    m_btSplineClear->Enable(count>=1);
    m_nbAnSpline->SetPageText(1, wxString::Format("Splines (%d)", count));

    m_btCreate->Enable(m_SCN.flexitextures.size() || ((m_SCN.models.size() || m_SCN.animatedmodels.size()) && m_SCN.lods.size()));
    if (m_SCN.lods.size())
        m_btCreate->SetToolTip(_("Create Scenery OVL"));
    else
        m_btCreate->SetToolTip(_("Create Texture OVL"));

    wxString title = _("Create OVL");
    if (m_SCN.filename != wxT(""))
        title += wxT(" - ") + m_SCN.filename.GetFullName();
    if (m_SCN.name != wxT(""))
        title += wxT(" (") + m_SCN.name + wxT(")");
    if (READ_RCT3_EXPERTMODE())
        title += wxT(" !!") + wxString(_("EXPERT MODE")) + wxT("!!");
    SetLabel(title);
}

bool dlgCreateScenery::TransferDataFromWindow() {
    int flags;
    switch (m_choiceExpansion->GetSelection()) {
    case 1:
        flags = r3::Constants::SVD::Flags::Soaked;
        break;
    case 2:
        flags = r3::Constants::SVD::Flags::Wild;
        break;
    default:
        flags = 0;
    }
    if (m_checkGreenery->IsChecked())
        flags |= r3::Constants::SVD::Flags::Greenery;
    if (m_checkShadow->IsChecked())
        flags |= r3::Constants::SVD::Flags::No_Shadow;
    if (m_checkRotation->IsChecked())
        flags |= r3::Constants::SVD::Flags::Rotation;
    if (m_checkTransformed->IsChecked())
        flags |= r3::Constants::SVD::Flags::Animated_Preview;
    if (m_checkUnknown01->IsChecked())
        flags |= r3::Constants::SVD::Flags::Unknown01;
    if (m_checkUnknown02->IsChecked())
        flags |= r3::Constants::SVD::Flags::Unknown02;
    if (m_checkUnknown03->IsChecked())
        flags |= r3::Constants::SVD::Flags::Unknown03;
    if (m_checkUnknownFerris->IsChecked())
        flags |= r3::Constants::SVD::Flags::Unknown_Giant_Ferris;
    m_SCN.sivsettings.sivflags = flags;

    return wxDialog::TransferDataFromWindow();
}

bool dlgCreateScenery::TransferDataToWindow() {
    if (m_SCN.sivsettings.sivflags & r3::Constants::SVD::Flags::Soaked)
        m_choiceExpansion->SetSelection(1);
    else if (m_SCN.sivsettings.sivflags & r3::Constants::SVD::Flags::Wild)
        m_choiceExpansion->SetSelection(2);
    else
        m_choiceExpansion->SetSelection(0);

    m_checkGreenery->SetValue(m_SCN.sivsettings.sivflags & r3::Constants::SVD::Flags::Greenery);
    m_checkShadow->SetValue(m_SCN.sivsettings.sivflags & r3::Constants::SVD::Flags::No_Shadow);
    m_checkRotation->SetValue(m_SCN.sivsettings.sivflags & r3::Constants::SVD::Flags::Rotation);
    m_checkTransformed->SetValue(m_SCN.sivsettings.sivflags & r3::Constants::SVD::Flags::Animated_Preview);
    m_checkUnknown01->SetValue(m_SCN.sivsettings.sivflags & r3::Constants::SVD::Flags::Unknown01);
    m_checkUnknown02->SetValue(m_SCN.sivsettings.sivflags & r3::Constants::SVD::Flags::Unknown02);
    m_checkUnknown03->SetValue(m_SCN.sivsettings.sivflags & r3::Constants::SVD::Flags::Unknown03);
    m_checkUnknownFerris->SetValue(m_SCN.sivsettings.sivflags & r3::Constants::SVD::Flags::Unknown_Giant_Ferris);

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
    m_htlbReferences->UpdateContents();
    m_htlbModel->UpdateContents();
    m_htlbAModel->UpdateContents();
    m_htlbLOD->UpdateContents();
    m_htlbAnimation->UpdateContents();
    m_htlbSpline->UpdateContents();

    TransferDataToWindow();

    m_textReference->ChangeValue(m_defaultRef);

    UpdateControlState();
}

bool dlgCreateScenery::Save(bool as) {
    if (m_SCN.imported) {
        ::wxMessageBox(_("You are saving a scenery file that was imported from a model file.\nWhen choosing the file name, make absolutely sure you don't overwrite your original model file!"), _("Warning"), wxOK|wxICON_WARNING, this);
    }
    if (!as) {
        if (m_SCN.filename == wxT("")) {
            // Shouldn't happen, but you never know...
            // Happens in OnClose
            // fall through
        } else {
            TransferDataFromWindow();
            if (m_SCN.version < 5) {
                if (::wxMessageBox(_("You are going to overwrite a old version scenery file.\nYou will not be able to open it with older importer versions anymore.\nDo you want to continue?"), _("Warning"), wxYES_NO|wxICON_WARNING|wxNO_DEFAULT, this) == wxNO) {
                    return false;
                }
            }
            try {
                if (!m_SCN.Save())
                    throw RCT3Exception(_("Saving scenery file failed. You will be asked for a new file name now."));
                MakeDirty(false);
                UpdateControlState();
                return true;
            } catch (std::exception& e) {
                ::wxMessageBox(wxString(e.what(), wxConvLocal), _("Error"), wxOK|wxICON_ERROR, this);
            }
        }
    }
    ////////////////////////////
    // Save Scenery As
    TransferDataFromWindow();
    wxFileDialog *dialog = new wxFileDialog(
                               this,
                               _T("Save Scenery File"),
                               wxEmptyString,
                               wxEmptyString,
                               _T("Scenery Files (*.xml)|*.xml"),
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
        try {
            if (!m_SCN.Save())
                throw RCT3Exception(_("Error saving SCN file."));
            MakeDirty(false);
            UpdateControlState();
            return true;
        } catch (std::exception& e) {
            ::wxMessageBox(wxString(e.what(), wxConvLocal), _("Error"), wxOK|wxICON_ERROR, this);
        }
    } else {
        dialog->Destroy();
        return false;
    }

    dialog->Destroy();
    return true;
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
                                   _("Open Scenery File"),
                                   wxEmptyString,
                                   wxEmptyString,
                                   cSCNFile::GetSupportedFilesFilter()+_("|All Files (*.*)|*.*"),
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

            wxLogGui *logex = new wxLogGui();
            wxLog *old = wxLog::SetActiveTarget(logex);
            try {
                /*
                if (!m_SCN.Load()) {
                    if (m_SCN.error != CSCNFILE_NO_ERROR) {
                        if (m_SCN.error == CSCNFILE_ERROR_VERSION)
                            ::wxMessageBox(_("Error loading SCN file.\nIncompatible file version."), _("Error"), wxOK | wxICON_ERROR, this);
                        else if (m_SCN.error == CSCNFILE_ERROR_VERSION_XML)
                            ::wxMessageBox(_("The xml scenery file was created with a newer importer version.\nThere might be some errors, so check whether everything is alright."), _("Warning"), wxOK | wxICON_WARNING, this);
                        else if (m_SCN.error == CSCNFILE_ERROR_WRONG_ROOT)
                            ::wxMessageBox(_("Error loading XML scenery file.\nThe file doesn't seem to be a valid scenery xml.\nProbably you tried to load an xml file made for a different purpose."), _("Error"), wxOK | wxICON_ERROR, this);
                        else
                            ::wxMessageBox(_("Error loading SCN file."), _("Error"), wxOK | wxICON_ERROR, this);
                    } else {
                        // Errors in the models
                        ::wxMessageBox(_("There were warings or errors loading the model files.\nThey will be shown to you when you open the respective model settings."), _("Warning"), wxOK | wxICON_WARNING, this);
                    }
                }
                */
                m_SCN.Load();

                if (m_SCN.references.size() && (!m_SCN.flexitextures.size())) {
                    m_nbTexRef->ChangeSelection(1);
                } else if (!m_SCN.references.size()) {
                    m_nbTexRef->ChangeSelection(0);
                }
                if (m_SCN.animatedmodels.size() && (!m_SCN.models.size())) {
                    m_nbModels->ChangeSelection(1);
                } else if (!m_SCN.animatedmodels.size()) {
                    m_nbModels->ChangeSelection(0);
                }
                if (m_SCN.splines.size() && (!m_SCN.animations.size())) {
                    m_nbAnSpline->ChangeSelection(1);
                } else if (!m_SCN.splines.size()) {
                    m_nbAnSpline->ChangeSelection(0);
                }
            } catch (RCT3Exception& e) {
                wxLogError(_("Error loading scenery file.\n") + e.wxwhat());
                m_SCN = cSCNFile();
            }
            wxLog::FlushActive();
            wxLog::SetActiveTarget(old);
            delete logex;

            TransferDataToWindow();
            UpdateAll();
            MakeDirty(false);
            UpdateControlState();
        }

        dialog->Destroy();
    }
    ////////////////////////////
    // Save Scenery
    if (id == wxID_SAVE) {
        Save(m_SCN.filename == wxT("")); // Does a save as if filename not set.
//        if (m_SCN.filename == wxT("")) {
//            // Shouldn't happen, but you never know...
//            id = wxID_SAVEAS;
//        } else {
//            TransferDataFromWindow();
//            if (m_SCN.version < 5) {
//                if (::wxMessageBox(_("You are going to overwrite a old version scenery file.\nYou will not be able to open it with older importer versions anymore.\nDo you want to continue?"), _("Warning"), wxYES_NO|wxICON_WARNING|wxNO_DEFAULT, this) == wxNO) {
//                    return;
//                }
//            }
//            if (!m_SCN.Save()) {
//                ::wxMessageBox(_("Saving scenery file failed. You will be asked for a new file name now."), _("Error"), wxOK|wxICON_ERROR, this);
//                id = wxID_SAVEAS;
//            } else {
//                MakeDirty(false);
//                UpdateControlState();
//            }
//        }
    }
    ////////////////////////////
    // Save Scenery As
    if (id == wxID_SAVEAS) {
        Save(true);
//        TransferDataFromWindow();
//        wxFileDialog *dialog = new wxFileDialog(
//                                   this,
//                                   _T("Save Scenery File"),
//                                   wxEmptyString,
//                                   wxEmptyString,
//                                   _T("Scenery Files (*.scn)|*.scn"),
//                                   wxFD_SAVE|wxFD_OVERWRITE_PROMPT|wxFD_CHANGE_DIR,
//                                   wxDefaultPosition,
//                                   wxSize(600,400)
//                               );
//        if (m_SCN.filename != wxT("")) {
//            dialog->SetFilename(m_SCN.filename.GetFullName());
//            dialog->SetDirectory(m_SCN.filename.GetPath());
//        } else
//            dialog->SetDirectory(::wxGetApp().g_workdir.GetPath());
//        if (dialog->ShowModal() == wxID_OK) {
//            ::wxGetApp().g_workdir.AssignDir(wxFileName(dialog->GetPath()).GetPath());
//            m_SCN.filename = dialog->GetPath();
//            if (!m_SCN.Save()) {
//                ::wxMessageBox(_("Error saving SCN file."), _("Error"), wxOK | wxICON_ERROR, this);
//            } else {
//                MakeDirty(false);
//                UpdateControlState();
//            }
//        }
//
//        dialog->Destroy();
    }
    /////////////////////////////////
    // Load texture ovl into cache
    if (id == tbLoadCache) {
        wxFileDialog *dialog = new wxFileDialog(
                                         this,
                                         _T("Open Scenery File (Load textures into cache)"),
                                         wxEmptyString,
                                         wxEmptyString,
                                         cSCNFile::GetSupportedFilesFilter()+_("|All Files (*.*)|*.*"),
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
                for (cFlexiTexture::iterator ms = texscn->flexitextures.begin(); ms != texscn->flexitextures.end(); ms++) {
                    ::wxGetApp().g_texturecache.push_back(ms->Name);
                }
                ::wxGetApp().g_texturecache.sort();
                ::wxGetApp().g_texturecache.unique();
                if (::wxGetApp().g_texturecache.size()>oldcount) {
                    ::wxMessageBox(wxString::Format(_("Texture cache updated.\n%d new textures were added to the cache."), ::wxGetApp().g_texturecache.size()-oldcount), _("Texture Cache"), wxOK | wxICON_INFORMATION, this);
                    m_htlbTexture->UpdateContents();
                    UpdateControlState();
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
            m_htlbTexture->UpdateContents();
            UpdateControlState();
        }
    }
    ////////////////////////////
    // Close
    if (id == wxID_CANCEL) {
        Close();
    }
}

/*
void dlgCreateScenery::OnShowUnknowns(wxCommandEvent& WXUNUSED(event)) {
//    if (m_checkShowUnknown->IsChecked()) {
//        if (::wxMessageBox(_("You should better leave these settings alone.\nDo you want to continue?"), _("Warning"), wxYES_NO | wxNO_DEFAULT | wxICON_WARNING, this)==wxNO) {
//            m_checkShowUnknown->SetValue(false);
//            return;
//        }
//    }
//    bool savedirty = m_dirtyfile;
//    TransferDataFromWindow();
//    m_panelUnknown->Show( m_checkShowUnknown->IsChecked());
    //Fit();
    wxSize best = GetBestSize();
    wxSize now = GetClientSize();
    SetClientSize(std::max(best.GetWidth(), now.GetWidth()), std::max(best.GetHeight(), now.GetHeight()));
    Layout();
//    TransferDataToWindow();
//    MakeDirty(savedirty);
}
*/

void dlgCreateScenery::OnControlUpdate(wxCommandEvent& WXUNUSED(event)) {
    UpdateControlState();
}

void dlgCreateScenery::OnClose(wxCloseEvent& event) {
    if ((!m_dirtyfile) || (!event.CanVeto())) {
        EndModal(0);
        return;
    }
    dlgSave* dlg = new dlgSave(_("Unsaved changes in your current scenery file.\nWhat do you want to do?"), _("Unsaved Changes"), this);
    switch (dlg->ShowModal()) {
        case wxID_NO:
            EndModal(0);
            break;
        case wxID_SAVE:
            if (Save())
                EndModal(0);
            // Fall through if cancelled
        case wxID_CANCEL:
        default:
            event.Veto();
    }
/*
    if (::wxMessageBox(_("Unsaved changes in your current scenery file.\nDo you want to continue?"), _("Question"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION, this)==wxNO) {
        event.Veto();
    } else {
        EndModal(0);
    }
*/
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
    if ((sel < 1) || (sel >= m_SCN.flexitextures.size()))
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
    if (((count-sel) <= 1) || (sel >= m_SCN.flexitextures.size()))
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
                                     _("Select texture file(s) for new texture(s)"),
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

        if (t_paths.GetCount() == 1) {
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
//            ILinfo info;
            int succeeded = 0;
            for (wxArrayString::iterator it = t_paths.begin(); it != t_paths.end(); it++) {
                cFlexiTexture ftx;
                cFlexiTextureFrame ftxf;
                try {
                    checkRCT3Texture(*it);
                } catch (RCT3TextureException) {
                    continue;
                }

                ftxf.texture(*it);
                {
                    wxGXImage img(*it);
                    if (img.HasAlpha())
                        ftxf.alphasource(CFTF_ALPHA_INTERNAL);
                }
                ftx.Frames.push_back(ftxf);
                ftx.Name = wxFileName(*it).GetName();
                m_SCN.flexitextures.push_back(ftx);
                succeeded++;
            }
            if (succeeded != t_paths.GetCount()) {
                wxMessageBox(wxString::Format(_("%d file(s) could not be added due to errors."), t_paths.GetCount()-succeeded), _("Warning"), wxOK|wxICON_WARNING, this);
            }
            if (succeeded) {
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
    if (dialog)
        dialog->Destroy();
}

void dlgCreateScenery::OnTextureEdit(wxCommandEvent& WXUNUSED(event)) {
    int sel = m_htlbTexture->GetSelection();
    if ((sel < 0) || (sel >= m_SCN.flexitextures.size()))
        return;

    dlgTextureBase *dialog = NULL;
    bool simple = ((m_SCN.flexitextures[sel].Frames.size()==1) &&
                   (m_SCN.flexitextures[sel].Animation.size()==0) &&
                   (m_SCN.flexitextures[sel].FPS == 0) &&
                   ((m_SCN.flexitextures[sel].Recolorable <= 1) ||
                    (m_SCN.flexitextures[sel].Recolorable == 3) ||
                    (m_SCN.flexitextures[sel].Recolorable == 7)));
    if (simple)
        dialog = new dlgTextureSimple(this);
    else
        dialog = new dlgTexture(this);
    dialog->SetFlexiTexture(m_SCN.flexitextures[sel]);
    int res = dialog->ShowModal();
    if (res == wxID_OK) {
        m_SCN.flexitextures[sel] = dialog->GetFlexiTexture();
        MakeDirty();
        m_htlbTexture->UpdateContents();
        UpdateControlState();
    } else if (res == wxID_SAVE) {
        cFlexiTexture temp = dialog->GetFlexiTexture();
        dialog->Destroy();

        dialog = new dlgTexture(this);
        dialog->SetFlexiTexture(temp);
        if (dialog->ShowModal() == wxID_OK) {
            m_SCN.flexitextures[sel] = dialog->GetFlexiTexture();
            MakeDirty();
            m_htlbTexture->UpdateContents();
            UpdateControlState();
        }
    }
    dialog->Destroy();
}

void dlgCreateScenery::OnTextureCopy(wxCommandEvent& WXUNUSED(event)) {
    int sel = m_htlbTexture->GetSelection();
    if ((sel < 0) || (sel >= m_SCN.flexitextures.size()))
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
    if ((sel < 0) || (sel >= m_SCN.flexitextures.size()))
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

/** @brief OnTextureLoad
  *
  * @todo: document this function
  */
void dlgCreateScenery::OnTextureLoad(wxCommandEvent& event) {
    implementLoad<cFlexiTexture>();
}

////////////////////////////////////////////////////////////////////////
//
//  dlgCreateScenery, References
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
    m_textReference->ChangeValue(m_SCN.references[sel].name);
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

/** @brief OnReferenceLoad
  *
  * @todo: document this function
  */
void dlgCreateScenery::OnReferenceLoad(wxCommandEvent& event) {
    implementLoad<cReference>();
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
    dialog->SetModel(cModel(matrixGetUnity(), READ_RCT3_ORIENTATION()));
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
        wxString errtext;
        foreach(wxString& err, m_SCN.models[sel].error)
            errtext += (errtext.IsEmpty()?wxT(""):wxT("\n\n")) + err;
        ::wxMessageBox(errtext, wxString(m_SCN.models[sel].fatal_error?_("Error"):_("Warning")) + _(" during model file loading"), wxOK | (m_SCN.models[sel].fatal_error?wxICON_ERROR:wxICON_WARNING), this);
        m_SCN.models[sel].error.clear();
    }
    m_SCN.models[sel].Sync();
    if (m_SCN.models[sel].error.size()) {
        wxString errtext;
        foreach(wxString& err, m_SCN.models[sel].error)
            errtext += (errtext.IsEmpty()?wxT(""):wxT("\n\n")) + err;
        ::wxMessageBox(errtext, wxString(m_SCN.models[sel].fatal_error?_("Error"):_("Warning")) + _(" during model file loading"), wxOK | (m_SCN.models[sel].fatal_error?wxICON_ERROR:wxICON_WARNING), this);
        m_SCN.models[sel].error.clear();
    }
    dialog->SetModel(m_SCN.models[sel]);
    if (dialog->ShowModal() == wxID_OK) {
        cModel mod = dialog->GetModel();
        if (!READ_RCT3_EXPERTMODE()) {
            if (mod.name != m_SCN.models[sel].name) {
                for (cLOD::iterator it = m_SCN.lods.begin(); it != m_SCN.lods.end(); it++) {
                    if (it->modelname == m_SCN.models[sel].name)
                        it->modelname = mod.name;
                }
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

void dlgCreateScenery::OnModelConvert(wxCommandEvent& WXUNUSED(event)) {
    int sel = m_htlbModel->GetSelection();
    if (sel<0)
        return;

    cAnimatedModel ft = cAnimatedModel(m_SCN.models[sel]);
    wxString orgname = ft.name;
    m_SCN.models.erase(m_SCN.models.begin() + sel);
    for (cAnimatedModel::iterator it = m_SCN.animatedmodels.begin(); it != m_SCN.animatedmodels.end(); ++it) {
        if (it->name = ft.name) {
            ft.name += _(", Converted");
            break;
        }
    }
    for (cLOD::iterator it = m_SCN.lods.begin(); it != m_SCN.lods.end(); ++it) {
        if (it->modelname == orgname) {
            it->modelname = ft.name;
            it->animated = true;
        }
    }
    m_SCN.animatedmodels.push_back(ft);

    MakeDirty();
    m_htlbModel->UpdateContents();
    m_htlbModel->SetSelection(sel-1);
    m_htlbAModel->UpdateContents();
    m_htlbLOD->UpdateContents();
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

/** @brief OnModelLoad
  *
  * @todo: document this function
  */
void dlgCreateScenery::OnModelLoad(wxCommandEvent& event) {
    implementLoad<cModel>();
}


////////////////////////////////////////////////////////////////////////
//
//  dlgCreateScenery, Animated Models
//
////////////////////////////////////////////////////////////////////////

void dlgCreateScenery::OnAModelUp(wxSpinEvent& WXUNUSED(event)) {
    int sel = m_htlbAModel->GetSelection();
    if (sel < 1)
        return;
    cAnimatedModel ft = m_SCN.animatedmodels[sel];
    m_SCN.animatedmodels.erase(m_SCN.animatedmodels.begin() + sel);
    m_SCN.animatedmodels.insert(m_SCN.animatedmodels.begin() + sel - 1, ft);

    MakeDirty();
    m_htlbAModel->UpdateContents();
    m_htlbAModel->SetSelection(sel-1);
    UpdateControlState();
}

void dlgCreateScenery::OnAModelDown(wxSpinEvent& WXUNUSED(event)) {
    int count = m_SCN.animatedmodels.size();
    int sel = m_htlbAModel->GetSelection();
    if ((count-sel) <= 1)
        return;
    cAnimatedModel ft = m_SCN.animatedmodels[sel];
    m_SCN.animatedmodels.erase(m_SCN.animatedmodels.begin() + sel);
    m_SCN.animatedmodels.insert(m_SCN.animatedmodels.begin() + sel + 1, ft);

    MakeDirty();
    m_htlbAModel->UpdateContents();
    m_htlbAModel->SetSelection(sel+1);
    UpdateControlState();
}

void dlgCreateScenery::OnAModelAdd(wxCommandEvent& WXUNUSED(event)) {
    dlgModel *dialog = new dlgModel(this, true);
    dialog->SetAnimatedModel(cAnimatedModel(matrixGetUnity(), READ_RCT3_ORIENTATION()));
    if (dialog->ShowModal() == wxID_OK) {
        m_SCN.animatedmodels.push_back(dialog->GetAnimatedModel());
        MakeDirty();
        m_htlbAModel->UpdateContents();
        m_htlbAModel->SetSelection(m_SCN.animatedmodels.size()-1);
        UpdateControlState();
    }
    dialog->Destroy();
}

void dlgCreateScenery::OnAModelEdit(wxCommandEvent& WXUNUSED(event)) {
    int sel = m_htlbAModel->GetSelection();
    if (sel < 0)
        return;
    dlgModel *dialog = new dlgModel(this, true);
    if (m_SCN.animatedmodels[sel].error.size()) {
        wxString errtext;
        foreach(wxString& err, m_SCN.animatedmodels[sel].error)
            errtext += (errtext.IsEmpty()?wxT(""):wxT("\n\n")) + err;
        ::wxMessageBox(errtext, wxString(m_SCN.animatedmodels[sel].fatal_error?_("Error"):_("Warning")) + _(" during model file loading"), wxOK | (m_SCN.animatedmodels[sel].fatal_error?wxICON_ERROR:wxICON_WARNING), this);
        m_SCN.animatedmodels[sel].error.clear();
    }
    m_SCN.animatedmodels[sel].Sync();
    if (m_SCN.animatedmodels[sel].error.size()) {
        wxString errtext;
        foreach(wxString& err, m_SCN.animatedmodels[sel].error)
            errtext += (errtext.IsEmpty()?wxT(""):wxT("\n\n")) + err;
        ::wxMessageBox(errtext, wxString(m_SCN.animatedmodels[sel].fatal_error?_("Error"):_("Warning")) + _(" during model file loading"), wxOK | (m_SCN.animatedmodels[sel].fatal_error?wxICON_ERROR:wxICON_WARNING), this);
        m_SCN.animatedmodels[sel].error.clear();
    }
    dialog->SetAnimatedModel(m_SCN.animatedmodels[sel]);
    if (dialog->ShowModal() == wxID_OK) {
        cAnimatedModel mod = dialog->GetAnimatedModel();
        if (!READ_RCT3_EXPERTMODE()) {
            if (mod.name != m_SCN.animatedmodels[sel].name) {
                for (cLOD::iterator it = m_SCN.lods.begin(); it != m_SCN.lods.end(); it++) {
                    if (it->modelname == m_SCN.animatedmodels[sel].name)
                        it->modelname = mod.name;
                }
            }
        }
        m_SCN.animatedmodels[sel] = mod;
        MakeDirty();
        m_htlbAModel->UpdateContents();
        m_htlbLOD->UpdateContents();
        UpdateControlState();
    }
    dialog->Destroy();
}

void dlgCreateScenery::OnAModelCopy(wxCommandEvent& WXUNUSED(event)) {
    int sel = m_htlbAModel->GetSelection();
    if (sel<0)
        return;

    cAnimatedModel ft = m_SCN.animatedmodels[sel];
    ft.name += _(" Copy");
    m_SCN.animatedmodels.insert(m_SCN.animatedmodels.begin() + sel + 1, ft);

    MakeDirty();
    m_htlbAModel->UpdateContents();
    m_htlbAModel->SetSelection(sel+1);
    UpdateControlState();
}

void dlgCreateScenery::OnAModelConvert(wxCommandEvent& WXUNUSED(event)) {
    int sel = m_htlbAModel->GetSelection();
    if (sel<0)
        return;

    bool infolost = false;
    for (cModelBone::iterator it = m_SCN.animatedmodels[sel].modelbones.begin(); it != m_SCN.animatedmodels[sel].modelbones.end(); it++) {
        if ((it->usepos2) || (it->parent != wxT("")) || (it->meshes.size())) {
            infolost = true;
            break;
        }
    }
    if (infolost) {
        if (::wxMessageBox(_("During the conversion all additional information of the animated model will be lost (Bone <-> Group assignment, bone parent and position 1 settings if position2 is activated).\nDo you want to continue?"), _("Question"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION, this)==wxNO)
            return;
    }

    cModel ft = cModel(m_SCN.animatedmodels[sel]);
    wxString orgname = ft.name;
    m_SCN.animatedmodels.erase(m_SCN.animatedmodels.begin() + sel);
    for (cModel::iterator it = m_SCN.models.begin(); it != m_SCN.models.end(); it++) {
        if (it->name = ft.name) {
            ft.name += _(", Converted");
            break;
        }
    }
    for (cLOD::iterator it = m_SCN.lods.begin(); it != m_SCN.lods.end(); ++it) {
        if (it->modelname == orgname) {
            it->modelname = ft.name;
            it->animated = false;
        }
    }
    m_SCN.models.push_back(ft);

    MakeDirty();
    m_htlbAModel->UpdateContents();
    m_htlbAModel->SetSelection(sel-1);
    m_htlbModel->UpdateContents();
    m_htlbLOD->UpdateContents();
    UpdateControlState();
}

void dlgCreateScenery::OnAModelDel(wxCommandEvent& WXUNUSED(event)) {
    int sel = m_htlbAModel->GetSelection();
    if (sel<0)
        return;

    m_SCN.animatedmodels.erase(m_SCN.animatedmodels.begin() + sel);

    MakeDirty();
    m_htlbAModel->UpdateContents();
    m_htlbAModel->SetSelection(sel-1);
    UpdateControlState();
}

void dlgCreateScenery::OnAModelClear(wxCommandEvent& WXUNUSED(event)) {
    if (m_SCN.animatedmodels.size()) {
        if (::wxMessageBox(_("Do you really want to delete all animated model entries?"), _("Question"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION, this)==wxNO)
            return;
    }

    m_SCN.animatedmodels.clear();

    MakeDirty();
    m_htlbAModel->UpdateContents();
    m_htlbAModel->SetSelection(-1);
    UpdateControlState();
}

/** @brief OnAModelLoad
  *
  * @todo: document this function
  */
void dlgCreateScenery::OnAModelLoad(wxCommandEvent& event) {
    implementLoad<cAnimatedModel>();
}


////////////////////////////////////////////////////////////////////////
//
//  dlgCreateScenery, LODs
//
////////////////////////////////////////////////////////////////////////

/** @brief OnLODShowAnimation
  *
  * @todo: document this function
  */
void dlgCreateScenery::OnLODShowAnimation(wxCommandEvent& event) {
    m_htlbLOD->setMode(m_cbShowAnim->Get3StateValue());
}

void dlgCreateScenery::OnLODAdd(wxCommandEvent& WXUNUSED(event)) {
    if (!m_SCN.CheckForModelNameDuplicates()) {
        ::wxMessageBox(_("There are duplicate model names. A model name must be unique, no matter whether it's animated or not"), _("Error"), wxOK|wxICON_ERROR, this);
        return;
    }
    dlgLOD *dialog = new dlgLOD(m_SCN, this);
    if (m_nbModels->GetSelection() != 1) {
        int sel = m_htlbModel->GetSelection();
        if (sel >= 0) {
            cLOD lod;
            lod.modelname = m_SCN.models[sel].name;
            dialog->SetLOD(lod);
        }
    } else {
        int sel = m_htlbAModel->GetSelection();
        if (sel >= 0) {
            cLOD lod;
            lod.modelname = m_SCN.animatedmodels[sel].name;
            lod.animated = true;
            dialog->SetLOD(lod);
        }
    }
    if (dialog->ShowModal() == wxID_OK) {
        cLOD nl = dialog->GetLOD();
        bool atend = true;
        int sel = 0;
        for (cLOD::iterator it = m_SCN.lods.begin(); it != m_SCN.lods.end(); it++) {
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
    if (!m_SCN.CheckForModelNameDuplicates()) {
        ::wxMessageBox(_("There are duplicate model names. A model name must be unique, no matter whether it's animated or not"), _("Error"), wxOK|wxICON_ERROR, this);
        return;
    }
    int sel = m_htlbLOD->GetSelection();
    if (sel < 0)
        return;
    dlgLOD *dialog = new dlgLOD(m_SCN, this);
    dialog->SetLOD(m_SCN.lods[sel]);
    if (dialog->ShowModal() == wxID_OK) {
        m_SCN.lods.erase(m_SCN.lods.begin() + sel);
        cLOD nl = dialog->GetLOD();
        bool atend = true;
        sel = 0;
        for (cLOD::iterator it = m_SCN.lods.begin(); it != m_SCN.lods.end(); it++) {
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
    if (m_SCN.animatedmodels.size())
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

/** @brief OnLODsRDClick
  *
  * @todo: document this function
  */
void dlgCreateScenery::OnLODsRDClick(wxMouseEvent& event) {
    int sel = m_htlbLOD->GetSelection();
    if ((sel<1) || (sel >= m_SCN.lods.size()))
        return;

    m_SCN.lods[sel].animations = m_SCN.lods[0].animations;

    MakeDirty();
    m_htlbLOD->UpdateContents();
    UpdateControlState();
}

/** @brief OnLODRDClear
  *
  * @todo: document this function
  */
void dlgCreateScenery::OnLODRDClear(wxMouseEvent& event) {
    foreach(cLOD& lod, m_SCN.lods) {
        lod.animations.clear();
    }
    MakeDirty();
    m_htlbLOD->UpdateContents();
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
//  dlgCreateScenery, Animations
//
////////////////////////////////////////////////////////////////////////

void dlgCreateScenery::OnAnimationUp(wxSpinEvent& WXUNUSED(event)) {
    /*
    int sel = m_htlbAnimation->GetSelection();
    if (sel < 1)
        return;
    cAnimation ft = m_SCN.animations[sel];
    m_SCN.animations.erase(m_SCN.animations.begin() + sel);
    m_SCN.animations.insert(m_SCN.animations.begin() + sel - 1, ft);

    MakeDirty();
    m_htlbAnimation->UpdateContents();
    m_htlbAnimation->SetSelection(sel-1);
    UpdateControlState();
    */
    implementSpinUp<cAnimation>();
}

void dlgCreateScenery::OnAnimationDown(wxSpinEvent& WXUNUSED(event)) {
    /*
    int count = m_SCN.animations.size();
    int sel = m_htlbAnimation->GetSelection();
    if ((count-sel) <= 1)
        return;
    cAnimation ft = m_SCN.animations[sel];
    m_SCN.animations.erase(m_SCN.animations.begin() + sel);
    m_SCN.animations.insert(m_SCN.animations.begin() + sel + 1, ft);

    MakeDirty();
    m_htlbAnimation->UpdateContents();
    m_htlbAnimation->SetSelection(sel+1);
    UpdateControlState();
    */
    implementSpinDown<cAnimation>();
}

void dlgCreateScenery::OnAnimationAdd(wxCommandEvent& WXUNUSED(event)) {
    dlgAnimation *dialog = new dlgAnimation(this);
    dialog->SetAnimation(cAnimation(READ_RCT3_ORIENTATION()));
    if (dialog->ShowModal() == wxID_OK) {
        m_SCN.animations.push_back(dialog->GetAnimation());
        MakeDirty();
        m_htlbAnimation->UpdateContents();
        m_htlbAnimation->SetSelection(m_SCN.animations.size()-1);
        UpdateControlState();
    }
    dialog->Destroy();
}

void dlgCreateScenery::OnAnimationEdit(wxCommandEvent& WXUNUSED(event)) {
    int sel = m_htlbAnimation->GetSelection();
    if (sel < 0)
        return;
    dlgAnimation *dialog = new dlgAnimation(this);
    dialog->SetAnimation(m_SCN.animations[sel]);
    if (dialog->ShowModal() == wxID_OK) {
        cAnimation mod = dialog->GetAnimation();
        if (!READ_RCT3_EXPERTMODE()) {
            if (mod.name != m_SCN.animations[sel].name) {
                for (cLOD::iterator it = m_SCN.lods.begin(); it != m_SCN.lods.end(); it++) {
                    for (cStringIterator st = it->animations.begin(); st != it->animations.end(); st++) {
                        if ((*st) == m_SCN.animations[sel].name)
                            (*st) = mod.name;
                    }
                }
            }
        }
        m_SCN.animations[sel] = mod;
        MakeDirty();
        m_htlbAnimation->UpdateContents();
        m_htlbLOD->UpdateContents();
        UpdateControlState();
    }
    dialog->Destroy();
}

void dlgCreateScenery::OnAnimationCopy(wxCommandEvent& WXUNUSED(event)) {
    /*
    int sel = m_htlbAnimation->GetSelection();
    if (sel<0)
        return;

    cAnimation ft = m_SCN.animations[sel];
    ft.name += _(" Copy");
    m_SCN.animations.insert(m_SCN.animations.begin() + sel + 1, ft);

    MakeDirty();
    m_htlbAnimation->UpdateContents();
    m_htlbAnimation->SetSelection(sel+1);
    UpdateControlState();
    */
    implementCopy<cAnimation>();
}

void dlgCreateScenery::OnAnimationDel(wxCommandEvent& WXUNUSED(event)) {
    /*
    int sel = m_htlbAnimation->GetSelection();
    if (sel<0)
        return;

    m_SCN.animations.erase(m_SCN.animations.begin() + sel);

    MakeDirty();
    m_htlbAnimation->UpdateContents();
    m_htlbAnimation->SetSelection(sel-1);
    UpdateControlState();
    */
    implementDel<cAnimation>();
}

void dlgCreateScenery::OnAnimationAddToLod(wxCommandEvent& WXUNUSED(event)) {
    int lod = m_htlbLOD->GetSelection();
    int ani = m_htlbAnimation->GetSelection();
    if ((ani < 0) or (lod < 0))
        return;

    cLOD* plod = &m_SCN.lods[lod];
    if (!plod->animated)
        return;

    cAnimation* pani = &m_SCN.animations[ani];
    if (!contains(plod->animations, pani->name)) {
        plod->animations.push_back(pani->name);
        m_htlbLOD->UpdateContents();
    }
    MakeDirty();
    UpdateControlState();
}

void dlgCreateScenery::OnAnimationClear(wxCommandEvent& WXUNUSED(event)) {
    /*
    if (m_SCN.animations.size()) {
        if (::wxMessageBox(_("Do you really want to delete all animations?"), _("Question"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION, this)==wxNO)
            return;
    }

    m_SCN.animations.clear();

    MakeDirty();
    m_htlbAnimation->UpdateContents();
    m_htlbAnimation->SetSelection(-1);
    UpdateControlState();
    */
    implementClear<cAnimation>();
}

/** @brief OnAnimationRDClear
  *
  * @todo: document this function
  */
void dlgCreateScenery::OnAnimationRDClear(wxMouseEvent& event) {
    implementClear<cAnimation>(false);
}

/** @brief OnAnimationLoad
  *
  * @todo: document this function
  */
void dlgCreateScenery::OnAnimationLoad(wxCommandEvent& event) {
    /*
    bool updated = false;
    boost::shared_ptr<dlgLoadFromFile> dlg(new dlgLoadFromFile(this, m_SCN, dlgLoadFromFile::Type_Animations, &updated, m_htlbAnimation), wxWindowDestroyer);
    dlg->ShowModal();
    if (updated) {
        MakeDirty();
        UpdateControlState();
    }
    */
    implementLoad<cAnimation>();
}

/** @brief OnAnimationRDLoad
  *
  * @todo: document this function
  */
void dlgCreateScenery::OnAnimationRDLoad(wxMouseEvent& event) {
    implementLoad<cAnimation>(true);
}



////////////////////////////////////////////////////////////////////////
//
//  dlgCreateScenery, Splines
//
////////////////////////////////////////////////////////////////////////

/** @brief OnSplineClear
  *
  * @todo: document this function
  */
void dlgCreateScenery::OnSplineClear(wxCommandEvent& event) {
    implementClear<cImpSpline>();
}

/** @brief OnSplineRDClear
  *
  * @todo: document this function
  */
void dlgCreateScenery::OnSplineRDClear(wxMouseEvent& event) {
    implementClear<cImpSpline>(false);
}

/** @brief OnSplineLoad
  *
  * @todo: document this function
  */
void dlgCreateScenery::OnSplineLoad(wxCommandEvent& event) {
    /*
    bool updated = false;
    boost::shared_ptr<dlgLoadFromFile> dlg(new dlgLoadFromFile(this, m_SCN, dlgLoadFromFile::Type_Splines, &updated, m_htlbSpline), wxWindowDestroyer);
    dlg->ShowModal();
    UpdateControlState();
    if (updated) {
        MakeDirty();
        UpdateControlState();
    }
    */
    implementLoad<cImpSpline>();
}

/** @brief OnSplineRDLoad
  *
  * @todo: document this function
  */
void dlgCreateScenery::OnSplineRDLoad(wxMouseEvent& event) {
    implementLoad<cImpSpline>(true);
}

/** @brief OnSplineDel
  *
  * @todo: document this function
  */
void dlgCreateScenery::OnSplineDel(wxCommandEvent& event) {
    implementDel<cImpSpline>();
}

/** @brief OnSplineCopy
  *
  * @todo: document this function
  */
void dlgCreateScenery::OnSplineCopy(wxCommandEvent& event) {
    implementCopy<cImpSpline>();
}

/** @brief OnSplineEdit
  *
  * @todo: document this function
  */
void dlgCreateScenery::OnSplineEdit(wxCommandEvent& event) {
    int sel = m_htlbSpline->GetSelection();
    if (sel < 0)
        return;
    dlgSpline *dialog = new dlgSpline(this, m_SCN.splines[sel]);
    if (dialog->ShowModal() == wxID_OK) {
        m_SCN.splines[sel] = dialog->GetSpline();
        MakeDirty();
        m_htlbSpline->UpdateContents();
        UpdateControlState();
    }
    dialog->Destroy();
}

/** @brief OnSplineDown
  *
  * @todo: document this function
  */
void dlgCreateScenery::OnSplineDown(wxSpinEvent& event) {
    implementSpinDown<cImpSpline>();
}

/** @brief OnSplineUp
  *
  * @todo: document this function
  */
void dlgCreateScenery::OnSplineUp(wxSpinEvent& event) {
    implementSpinUp<cImpSpline>();
}



////////////////////////////////////////////////////////////////////////
//
//  dlgCreateScenery, Create Scenery
//
////////////////////////////////////////////////////////////////////////

WX_DECLARE_STRING_HASH_MAP(char *, ovlNameLookup);

void dlgCreateScenery::OnAutoName(wxCommandEvent& event) {
    int textures = m_SCN.flexitextures.size();
    int other = m_SCN.models.size() + m_SCN.animatedmodels.size() + m_SCN.lods.size() + m_SCN.animations.size();
    m_textPrefix->SetValue(m_prefix);
    if (textures && (!other)) {
        m_textName->SetValue(m_theme + wxT("-texture"));
    } else if (other) {
        wxString name = m_SCN.filename.GetName();
        if (name.IsEmpty()) {
            ::wxMessageBox(_("Cannot autoname in unsaved state."), _("Error"), wxOK | wxICON_ERROR, this);
            return;
        }
        m_textName->SetValue(name);
    } else {
        ::wxMessageBox(_("Cannot autoname as long as you didn't add any data."), _("Error"), wxOK | wxICON_ERROR, this);
    }
}

void dlgCreateScenery::OnCheck(wxCommandEvent& WXUNUSED(event)) {
    bool all_ok = true;
    bool cont = true;
    //wxLogGuiExt *logex = new wxLogGuiExt(NULL);
#ifndef __WXDEBUG__
    wxLogGui *logex = new wxLogGui();
    wxLog *old = wxLog::SetActiveTarget(logex);
#endif

    TransferDataFromWindow();

    try {
        all_ok = m_SCN.Check();
    } catch (RCT3Exception& e) {
        wxLogError(e.what());
        all_ok = false;
    } catch (std::exception& e) {
        wxLogError(_("Unknown exception during check: %s"), e.what());
        all_ok = false;
    }
    if (all_ok) {
        if (m_SCN.IsTextureOVL())
            wxLogMessage(_("All data seems to be ok (texture ovl)."));
        else
            wxLogMessage(_("All data seems to be ok (scenery ovl)."));
    }
    wxLog::FlushActive();

#ifndef __WXDEBUG__
    wxLog::SetActiveTarget(old);
    delete logex;
#endif
}

void dlgCreateScenery::OnCreate(wxCommandEvent& WXUNUSED(event)) {
    bool textureovl = false;
//    c3DLoader *object = NULL;
    bool error = false;
    bool warning = false;
    bool cont = true;
    bool was_going = false;

    // For progress display
//    unsigned int meshes = 0;
//    unsigned int textures = 0;
//    unsigned int progress_count = 0;

//    bool filenameok = false;
//    int diaret = wxID_OK;

    TransferDataFromWindow();

    //cSCNFile work = m_SCN;
//    wxFileName sfile;
//    wxFileDialog *dialog = NULL;

// If debugging, use the log window
#ifndef __WXDEBUG__
    wxLogGuiExt *logex = new wxLogGuiExt(&cont);
    wxLog *old = wxLog::SetActiveTarget(logex);
#endif

    try {
        warning = !m_SCN.Check();
    } catch (RCT3Exception& e) {
        wxLogError(e.what());
    } catch (std::exception& e) {
        wxLogError(_("Unknown exception during check: %s"), e.what());
        error = true;
    }
    wxLog::FlushActive();
#ifdef __WXDEBUG
    if (warning) {
        cont = ::wxMessageBox(wxT("There were warnings. Do you want to continue?"), wxT("Warning"), wxYES_NO | wxICON_WARNING, this) == wxYES;
    }
#endif

//    if (!cont)
//        goto deinitlogger;

    if (cont) {
        try {
            wxBusyCursor bc;
            was_going = true;
            m_SCN.Make();
            if (m_SCN.IsTextureOVL()) {
                textureovl = true;
                foreach(const cFlexiTexture& tex, m_SCN.flexitextures)
                    ::wxGetApp().g_texturecache.push_back(tex.Name);
                ::wxGetApp().g_texturecache.sort();
                ::wxGetApp().g_texturecache.unique();
            }
        } catch (RCT3Exception& e) {
            wxLogError(e.what());
            error = true;
        } catch (EOvl& e) {
            wxLogError(e.what());
            error = true;
        } catch (std::exception& e) {
            wxLogError(_("Unknown exception during creation: %s"), e.what());
            error = true;
        }
    }


//deinitlogger:
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
#ifndef __WXDEBUG__
    wxLog::SetActiveTarget(old);
    delete logex;
#endif
}
