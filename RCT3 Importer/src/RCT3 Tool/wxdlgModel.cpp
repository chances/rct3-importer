///////////////////////////////////////////////////////////////////////////////
//
// Model Settings Dialog
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

#include "wxdlgModel.h"

#include <wx/valgen.h>

#include "htmlentities.h"
#include "matrix.h"
#include "SCNFile.h"
#include "valext.h"

#include "wxapp.h"
#include "wxdlgEffect.h"
#include "wxdlgMatrix.h"
#include "wxdlgMesh.h"

////////////////////////////////////////////////////////////////////////
//
//  wxMeshListBox
//
////////////////////////////////////////////////////////////////////////

wxMeshListBox::wxMeshListBox(wxWindow *parent, cModel *content):
        wxColourHtmlListBox(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER) {
    m_contents = content;
    UpdateContents();
    SetSelection(0);
}

void wxMeshListBox::UpdateContents() {
    SetItemCount(m_contents->meshstructs.size());
    RefreshAll();
}

wxString wxMeshListBox::OnGetItem(size_t n) const {
    cMeshStruct *ms = &m_contents->meshstructs[n];
    wxString res = wxT("<font ");
    if (ms->effectpoint)
        res += wxT("color='#A0D2A0' ");
    else if (!ms->valid)
        res += wxT("color='#CC0000' ");
    else if (ms->disabled)
        res += wxT("color='#808080' ");
    else if ((ms->FTX == wxT(""))
             || (ms->TXS == wxT("")))
        res += wxT("color='#FF3C3C' ");

    res += wxT("size='2'>")+wxEncodeHtmlEntities(ms->Name)+wxT("</font>");
    return res;
}

////////////////////////////////////////////////////////////////////////
//
//  wxEffectListBox
//
////////////////////////////////////////////////////////////////////////

wxEffectListBox::wxEffectListBox(wxWindow *parent, cModel *content):
        wxHtmlListBox(parent, wxID_ANY, wxDefaultPosition, wxSize(-1,150), wxSUNKEN_BORDER) {
    m_contents = content;
    UpdateContents();
    SetSelection(0);
}

void wxEffectListBox::UpdateContents() {
    SetItemCount(m_contents->effectpoints.size());
    RefreshAll();
}

wxString wxEffectListBox::OnGetItem(size_t n) const {
    return wxT("<font size='2'>")+wxEncodeHtmlEntities(m_contents->effectpoints[n].Name)+wxT("</font>");
}


////////////////////////////////////////////////////////////////////////
//
//  dlgModel
//
////////////////////////////////////////////////////////////////////////

const int ID_htlbMesh = ::wxNewId();

BEGIN_EVENT_TABLE(dlgModel,wxDialog)
EVT_BUTTON(XRCID("m_btModelOpen"), dlgModel::OnModelOpen)

EVT_BUTTON(XRCID("m_btMatrixEdit"), dlgModel::OnMatrixEdit)
EVT_BUTTON(XRCID("m_btMatrixSave"), dlgModel::OnMatrixSave)
EVT_BUTTON(XRCID("m_btMatrixClear"), dlgModel::OnMatrixClear)

EVT_LISTBOX(ID_htlbMesh, dlgModel::OnControlUpdate)
EVT_LISTBOX_DCLICK(ID_htlbMesh, dlgModel::OnMeshEdit)
EVT_BUTTON(XRCID("m_btEditMesh"), dlgModel::OnMeshEdit)

EVT_LISTBOX(XRCID("m_htlbEffect"), dlgModel::OnControlUpdate)
EVT_SPIN_UP(XRCID("m_spinEffect"), dlgModel::OnEffectUp)
EVT_SPIN_DOWN(XRCID("m_spinEffect"), dlgModel::OnEffectDown)
EVT_BUTTON(XRCID("m_btEffectAdd"), dlgModel::OnEffectAdd)
EVT_BUTTON(XRCID("m_btEffectEdit"), dlgModel::OnEffectEdit)
EVT_LISTBOX_DCLICK(XRCID("m_htlbEffect"), dlgModel::OnEffectEdit)
EVT_BUTTON(XRCID("m_btEffectCopy"), dlgModel::OnEffectCopy)
EVT_BUTTON(XRCID("m_btEffectDel"), dlgModel::OnEffectDel)
EVT_BUTTON(XRCID("m_btEffectAuto"), dlgModel::OnEffectAuto)
EVT_BUTTON(XRCID("m_btEffectClear"), dlgModel::OnEffectClear)

EVT_BUTTON(XRCID("m_btLoad"), dlgModel::OnLoad)
END_EVENT_TABLE()

dlgModel::dlgModel(wxWindow *parent) {
    InitWidgetsFromXRC((wxWindow *)parent);

    m_textModelName->SetValidator(wxExtendedValidator(&m_model.name, false));
    m_textModelFile->SetValidator(wxExtendedValidator(&m_model.file, false, true));

    m_htlbMesh = new wxMeshListBox(this, &m_model);
    m_htlbMesh->SetId(ID_htlbMesh);
    m_htlbMesh->SetToolTip(_("List of the meshes/groups/objects in your model file.\nColours:\nred - Mesh lacks data needed for import.\ndark red - Mesh is invalid/broken.\ngrey - Deactivatet mesh.\ngreen - One-vertex mesh that can be used for matrices (e.g. effect points).\nNote: If a mesh is initially deactivated when you load a model\nit's a sure indication that something is wrong with it.\n\nShift-Doubleclick on a mesh will single it out."));
    m_htlbMesh->SetMinSize(wxSize(-1,150));
    wxSizer *s = m_btEditMesh->GetContainingSizer();
    s->Insert(0, m_htlbMesh, wxSizerFlags().Expand().Border(wxALL));
    //wxXmlResource::Get()->AttachUnknownControl(wxT("m_htlbMesh"), m_htlbMesh, this);
    m_htlbEffect = new wxEffectListBox(this, &m_model);
    m_htlbEffect->SetToolTip(_("List of the effect points"));
    wxXmlResource::Get()->AttachUnknownControl(wxT("m_htlbEffect"), m_htlbEffect, this);

    Fit();
    Layout();
    GetSizer()->SetSizeHints(this);
    Center();

    m_btOk->SetId(wxID_OK);
    m_btCancel->SetId(wxID_CANCEL);

    UpdateAll();
}

void dlgModel::SetModel(const cModel& model) {
    m_model = model;
    while (m_model.fatal_error) {
        if (::wxMessageBox(_("The model file could not be found or read, please select a new one.\nIf you press 'Cancel', all model related data (file name and mesh settings) will be cleared."), _("Error loading model file"), wxOK|wxCANCEL|wxICON_ERROR) == wxCANCEL) {
            // User requested delete
            m_model.fatal_error = false;
            m_model.file = wxT("");
            m_model.meshstructs.clear();
            break;
        }
        wxFileDialog *dialog = new wxFileDialog(
                                   this,
                                   _("Select Model File"),
                                   wxEmptyString,
                                   wxEmptyString,
                                   _("Supported 3D Files|*.ase;*.ms3d|ASE Files (*.ase)|*.ase|Milkshape 3D (*.ms3d)|*.ms3d|All Files (*.*)|*.*"),
                                   wxFD_FILE_MUST_EXIST|wxFD_CHANGE_DIR,
                                   wxDefaultPosition,
                                   wxSize(600,400)
                               );
        if (m_model.file != wxT("")) {
            dialog->SetDirectory(m_model.file.GetPath());
            dialog->SetFilename(m_model.file.GetFullName());
        } else
            dialog->SetDirectory(::wxGetApp().g_workdir.GetPath());
        if (dialog->ShowModal() == wxID_OK) {
            ::wxGetApp().g_workdir.AssignDir(wxFileName(dialog->GetPath()).GetPath());
            m_model.Sync(dialog->GetPath());
            if (m_model.fatal_error) {
                // Load error
                m_model = model;
                continue;
            }
            if (m_model.error.size()) {
                wxString errtext = m_model.error[0];
                while (m_model.error.erase(m_model.error.begin()) != m_model.error.end()) {
                    errtext += wxT("\n\n") + m_model.error[0];
                }
                if (::wxMessageBox(_("The following warnings were encountered while loading the model file:\n") + errtext + _("\n\nDo you want to select a different one?"), _("Warning during model file loading"), wxYES_NO | wxNO_DEFAULT | wxICON_WARNING) == wxYES) {
                    // Select a different one, restore the model
                    m_model = model;
                } else {
                    m_model.error.clear();
                }
            }
        }
    }
    UpdateAll();
};

void dlgModel::FetchOneVertexMeshes(wxArrayString& names, std::vector<D3DVECTOR>& points) {
    for (cMeshStructIterator ms = m_model.meshstructs.begin();
            ms != m_model.meshstructs.end(); ms++) {
        if (ms->effectpoint) {
            names.push_back(ms->Name);
            points.push_back(ms->effectpoint_vert);
        }
    }
}

void dlgModel::ShowTransform(int pr) {
    D3DMATRIX m = matrixMultiply(m_model.transforms);
    for (int i=0; i<4; i++)
        for (int j=0; j<4; j++)
            m_Matrix[i][j]->SetLabel(wxString::Format("%.*f", pr, m.m[i][j]));
}

void dlgModel::UpdateAll() {
    ShowTransform();
    m_htlbMesh->UpdateContents();
    m_htlbEffect->UpdateContents();

    UpdateControlState();
}

void dlgModel::UpdateControlState() {
    int count = m_model.effectpoints.size();
    int sel = m_htlbEffect->GetSelection();
    m_spinEffect->Enable(count>=2);
    m_btEffectEdit->Enable(sel>=0);
    m_btEffectCopy->Enable(sel>=0);
    m_btEffectDel->Enable(sel>=0);
    m_btEffectClear->Enable(count>=1);

    m_btEditMesh->Enable(m_htlbMesh->GetSelection()>=0);
    bool haspoints = false;
    for (cMeshStructIterator ms = m_model.meshstructs.begin();
            ms != m_model.meshstructs.end(); ms++) {
        if (ms->effectpoint) {
            haspoints = true;
            break;
        }
    }
    m_btEffectAuto->Enable(haspoints);

}

void dlgModel::OnModelOpen(wxCommandEvent& WXUNUSED(event)) {
    wxFileDialog *dialog = new wxFileDialog(
                               this,
                               _("Open Model File"),
                               wxEmptyString,
                               wxEmptyString,
                               _("Supported 3D Files|*.ase;*.ms3d|ASE Files (*.ase)|*.ase|Milkshape 3D (*.ms3d)|*.ms3d|All Files (*.*)|*.*"),
                               wxFD_FILE_MUST_EXIST|wxFD_CHANGE_DIR,
                               wxDefaultPosition,
                               wxSize(600,400)
                           );
    if (m_model.file != wxT("")) {
        dialog->SetDirectory(m_model.file.GetPath());
        dialog->SetFilename(m_model.file.GetFullName());
    } else
        dialog->SetDirectory(::wxGetApp().g_workdir.GetPath());
    if (dialog->ShowModal() == wxID_OK) {
        ::wxGetApp().g_workdir.AssignDir(wxFileName(dialog->GetPath()).GetPath());
        TransferDataFromWindow();
        wxFileName oldfile = m_model.file;
        if (m_model.Load(dialog->GetPath())) {
            TransferDataToWindow();
            UpdateAll();
            if (m_model.orientation != ORIENTATION_UNKNOWN) {
                if (matrixIsEqual(matrixMultiply(m_model.transforms), matrixGetUnity())) {
                    // No transform matrix, set the right one.
                    m_model.transforms.clear();
                    m_model.transformnames.clear();
                    m_model.transforms.push_back(matrixGetFixOrientation(m_model.orientation));
                    m_model.transformnames.push_back(wxT("Fix-up from model file"));
                    ShowTransform();
                } else if (!matrixIsEqual(matrixMultiply(m_model.transforms), matrixGetFixOrientation(m_model.orientation))) {
                    // Model matrix differs from the current one
                    if (::wxMessageBox(_("The model file you just opened suggests a orientation fix matrix different from your current one.\nDo you want to replace the current one with the one suggested in the file?"), _("Question"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION, this) == wxYES) {
                        m_model.transforms.clear();
                        m_model.transformnames.clear();
                        m_model.transforms.push_back(matrixGetFixOrientation(m_model.orientation));
                        m_model.transformnames.push_back(wxT("Fix-up from model file"));
                        ShowTransform();
                    }
                }
            }
        } else {
            ::wxMessageBox(m_model.error[0], _("Error"), wxOK | wxICON_ERROR, this);
            m_model.file = oldfile;
        }
    }
    dialog->Destroy();
}

void dlgModel::OnControlUpdate(wxCommandEvent& WXUNUSED(event)) {
    UpdateControlState();
}

////////////////////////////////////////////////////////////////////////
//
//  dlgModel, Matrix
//
////////////////////////////////////////////////////////////////////////

void dlgModel::OnMatrixEdit(wxCommandEvent& WXUNUSED(event)) {
    dlgMatrix *dialog = new dlgMatrix(this);
    dialog->SetStack(m_model.transforms, m_model.transformnames);
    if (dialog->ShowModal() == wxID_OK) {
        m_model.transforms = dialog->GetMatrices();
        m_model.transformnames = dialog->GetMatrixNames();
        ShowTransform();
    }
    dialog->Destroy();
}

void dlgModel::OnMatrixSave(wxCommandEvent& WXUNUSED(event)) {
    ::wxGetApp().GetConfig()->m_def_matrix = matrixMultiply(m_model.transforms);
    ::wxGetApp().GetConfig()->Save();
}

void dlgModel::OnMatrixClear(wxCommandEvent& WXUNUSED(event)) {
    if (m_model.transforms.size()) {
        if (::wxMessageBox(_("Do you really want to delete all matrix entries?"), _("Question"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION, this)==wxNO)
            return;
    }

    m_model.transforms.clear();
    m_model.transformnames.clear();
    ShowTransform();
}

////////////////////////////////////////////////////////////////////////
//
//  dlgModel, Meshes
//
////////////////////////////////////////////////////////////////////////

void dlgModel::OnMeshEdit(wxCommandEvent& WXUNUSED(event)) {
    int sel = m_htlbMesh->GetSelection();
    if (sel < 0)
        return;

    if (!m_model.meshstructs[sel].valid) {
        ::wxMessageBox(_("The mesh you tried to edit is invalid. Possible reasons are:\n- It contains no faces.\n- It lacks a uv-mapping."), _("Error"), wxOK | wxICON_ERROR, this);
        return;
    }

    if (::wxGetKeyState(WXK_SHIFT)) {
        // Doubleclick on the list
        for (unsigned int i = 0; i < m_model.meshstructs.size(); i++) {
            if (i == sel)
                m_model.meshstructs[i].disabled = false;
            else
                m_model.meshstructs[i].disabled = true;
        }
        m_htlbMesh->UpdateContents();
        m_htlbMesh->ScrollToLine(sel);
        return;
    }


    dlgMesh *dialog = new dlgMesh(this);
    dialog->SetMeshStruct(m_model.meshstructs[sel]);
    if (dialog->ShowModal() == wxID_OK) {
        if (dialog->GetApplyAll()) {
            cMeshStruct mstr = dialog->GetMeshStruct();
            for (cMeshStructIterator ms = m_model.meshstructs.begin(); ms != m_model.meshstructs.end(); ms++) {
                if (ms->valid)
                ms->CopySettingsFrom(mstr);
            }
        } else {
            m_model.meshstructs[sel] = dialog->GetMeshStruct();
        }
        m_htlbMesh->UpdateContents();
        m_htlbMesh->ScrollToLine(sel);
    }
    dialog->Destroy();
}

////////////////////////////////////////////////////////////////////////
//
//  dlgModel, Effects
//
////////////////////////////////////////////////////////////////////////

void dlgModel::OnEffectUp(wxSpinEvent& WXUNUSED(event)) {
    int sel = m_htlbEffect->GetSelection();
    if (sel < 1)
        return;
    cEffectPoint ft = m_model.effectpoints[sel];
    m_model.effectpoints.erase(m_model.effectpoints.begin() + sel);
    m_model.effectpoints.insert(m_model.effectpoints.begin() + sel - 1, ft);

    m_htlbEffect->UpdateContents();
    m_htlbEffect->SetSelection(sel-1);
    UpdateControlState();
}

void dlgModel::OnEffectDown(wxSpinEvent& WXUNUSED(event)) {
    int count = m_model.effectpoints.size();
    int sel = m_htlbEffect->GetSelection();
    if ((count-sel) <= 1)
        return;
    cEffectPoint ft = m_model.effectpoints[sel];
    m_model.effectpoints.erase(m_model.effectpoints.begin() + sel);
    m_model.effectpoints.insert(m_model.effectpoints.begin() + sel + 1, ft);

    m_htlbEffect->UpdateContents();
    m_htlbEffect->SetSelection(sel+1);
    UpdateControlState();
}

void dlgModel::OnEffectAdd(wxCommandEvent& WXUNUSED(event)) {
    dlgEffect *dialog = new dlgEffect(this);
    if (dialog->ShowModal() == wxID_OK) {
        m_model.effectpoints.push_back(dialog->GetEffect());
        m_htlbEffect->UpdateContents();
        m_htlbEffect->SetSelection(m_model.effectpoints.size()-1);
        UpdateControlState();
    }
    dialog->Destroy();
}

void dlgModel::OnEffectEdit(wxCommandEvent& WXUNUSED(event)) {
    int sel = m_htlbEffect->GetSelection();
    if (sel < 0)
        return;
    dlgEffect *dialog = new dlgEffect(this);
    dialog->SetEffect(m_model.effectpoints[sel]);
    if (dialog->ShowModal() == wxID_OK) {
        m_model.effectpoints[sel] = dialog->GetEffect();
        m_htlbEffect->UpdateContents();
        UpdateControlState();
    }
    dialog->Destroy();
}

void dlgModel::OnEffectCopy(wxCommandEvent& WXUNUSED(event)) {
    int sel = m_htlbEffect->GetSelection();

    cEffectPoint ft = m_model.effectpoints[sel];
    ft.Name += _(" Copy");
    m_model.effectpoints.insert(m_model.effectpoints.begin() + sel + 1, ft);

    m_htlbEffect->UpdateContents();
    m_htlbEffect->SetSelection(sel+1);
    UpdateControlState();
}

void dlgModel::OnEffectDel(wxCommandEvent& WXUNUSED(event)) {
    int sel = m_htlbEffect->GetSelection();
    m_model.effectpoints.erase(m_model.effectpoints.begin() + sel);

    m_htlbEffect->UpdateContents();
    m_htlbEffect->SetSelection(sel-1);
    UpdateControlState();
}

void dlgModel::OnEffectAuto(wxCommandEvent& WXUNUSED(event)) {
    if (m_model.effectpoints.size()) {
        if (::wxMessageBox(_("Do you want to delete all current effect point entries before automatically creating new ones?"), _("Question"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION, this)==wxYES) {
            m_model.effectpoints.clear();
        }
    }

    for (cMeshStructIterator ms = m_model.meshstructs.begin();
            ms != m_model.meshstructs.end(); ms++) {
        if (ms->effectpoint) {
            cEffectPoint t;

            t.Name = ms->Name;
            t.Transform.push_back(matrixGetTranslation(ms->effectpoint_vert));
            wxString nam = _("Auto-generated form mesh '")+ms->Name+wxT("'");
            t.TransformNames.push_back(nam);
            m_model.effectpoints.push_back(t);
        }
    }

    m_htlbEffect->UpdateContents();
    m_htlbEffect->SetSelection(-1);
    UpdateControlState();
}

void dlgModel::OnEffectClear(wxCommandEvent& WXUNUSED(event)) {
    if (m_model.effectpoints.size()) {
        if (::wxMessageBox(_("Do you really want to delete all effect point entries?"), _("Question"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION, this)==wxNO)
            return;
    }

    m_model.effectpoints.clear();

    m_htlbEffect->UpdateContents();
    m_htlbEffect->SetSelection(-1);
    UpdateControlState();
}

////////////////////////////////////////////////////////////////////////
//
//  dlgModel, Load
//
////////////////////////////////////////////////////////////////////////

void dlgModel::OnLoad(wxCommandEvent& WXUNUSED(event)) {
    wxFileDialog *dialog = new wxFileDialog(
                                     this,
                                     _T("Open Scenery File (Load model)"),
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
            if (texscn->models.size()==0) {
                wxMessageBox(_("The selected scenery file contains no models."), _("Warning"), wxOK | wxICON_WARNING, this);
                delete texscn;
                dialog->Destroy();
                return;
            }
            wxArrayString choices;
            for (cModelIterator ms = texscn->models.begin(); ms != texscn->models.end(); ms++) {
                choices.Add(ms->name);
            }
            if (choices.size() > 0) {
                int choice = ::wxGetSingleChoiceIndex(dialog->GetPath(), _("Choose model to import"), choices, this);
                if (choice >= 0) {
                    SetModel(texscn->models[choice]);
                    TransferDataToWindow();
                    UpdateAll();
                }
            }
            delete texscn;
        }
    }
    dialog->Destroy();
}

