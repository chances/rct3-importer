///////////////////////////////////////////////////////////////////////////////
//
// Effect Point Dialog
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


#include "wxdlgEffect.h"

#include <wx/dir.h>

#include "matrix.h"
#include "SCNFile.h"
#include "valext.h"

#include "wxapp.h"
#include "wxdlgEffectLight.h"
#include "wxdlgMatrix.h"
#include "wxdlgModel.h"

////////////////////////////////////////////////////////////////////////
//
//  dlgEffect
//
////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(dlgEffect,wxDialog)
EVT_BUTTON(XRCID("m_btCreate"), dlgEffect::OnCreateClick)

EVT_CHOICE(XRCID("m_choicePoint"), dlgEffect::OnPosChange)
EVT_BUTTON(XRCID("m_btPos"), dlgEffect::OnPosClick)
EVT_BUTTON(XRCID("m_btEdit"), dlgEffect::OnEditClick)
EVT_BUTTON(XRCID("m_btClear"), dlgEffect::OnClearClick)

EVT_MENU(XRCID("menu_effect_light_start"), dlgEffect::OnMenuLightStart)
EVT_MENU(XRCID("menu_effect_particle_start"), dlgEffect::OnMenuParticleStart)
EVT_MENU(XRCID("menu_effect_particle_end"), dlgEffect::OnMenuParticleEnd)
EVT_MENU(XRCID("menu_effect_launcher_start"), dlgEffect::OnMenuLauncherStart)
EVT_MENU(XRCID("menu_effect_launcher_end"), dlgEffect::OnMenuLauncherEnd)
EVT_MENU(XRCID("menu_effect_peep_peep"), dlgEffect::OnMenuPeepPeep)
EVT_MENU(XRCID("menu_effect_peep_vendor"), dlgEffect::OnMenuPeepVendor)
EVT_MENU(XRCID("menu_effect_fountain_nozbl"), dlgEffect::OnMenuFountainNOZBL)
EVT_MENU(XRCID("menu_effect_fountain_nozbr"), dlgEffect::OnMenuFountainNOZBR)
EVT_MENU(XRCID("menu_effect_fountain_noztl"), dlgEffect::OnMenuFountainNOZTL)
EVT_MENU(XRCID("menu_effect_fountain_noztr"), dlgEffect::OnMenuFountainNOZTR)

EVT_BUTTON(XRCID("m_btLoad"), dlgEffect::OnLoad)
END_EVENT_TABLE()

dlgEffect::dlgEffect(wxWindow *parent) {
    InitWidgetsFromXRC((wxWindow *)parent);

    m_textEffectName->SetValidator(wxExtendedValidator(&m_ef.Name, false));

    m_menuPopup = wxXmlResource::Get()->LoadMenu(wxT("menu_effect"));

    m_choicePoint->Append(_("(Custom)"));
    m_choicePoint->SetSelection(0);

    dlgModel *mod = dynamic_cast<dlgModel *> (parent);
    if (mod) {
        wxArrayString names;
        mod->FetchOneVertexMeshes(names, m_points);
        m_choicePoint->Append(names);
    }
    m_choicePoint->Enable(m_points.size()>0);

    m_btOk->SetId(wxID_OK);
    m_btCancel->SetId(wxID_CANCEL);

    UpdateAll();
}

void dlgEffect::ShowTransform(int pr) {
    D3DMATRIX m = matrixMultiply(m_ef.Transform);
    for (int i=0; i<4; i++)
        for (int j=0; j<4; j++)
            m_Matrix[i][j]->SetLabel(wxString::Format("%.*f", pr, m.m[i][j]));
}

void dlgEffect::UpdateAll() {
    ShowTransform();
}

void dlgEffect::SelectNr(wxString pt, unsigned int from) {
    m_textEffectName->SetValue(pt);
    m_textEffectName->SetFocus();
    m_textEffectName->SetSelection(from, from+2);
}

void dlgEffect::OnCreateClick(wxCommandEvent& WXUNUSED(event)) {
    PopupMenu(m_menuPopup);
}

bool dlgEffect::Validate() {
    if ((m_ef.Transform.size() == 0) && ((m_textX->GetValue() != wxT("")) || (m_textY->GetValue() != wxT("")) || (m_textZ->GetValue() != wxT("")))) {
        int r = ::wxMessageBox(_("You entered values in the position fields but didn't press the 'Pos' button.\nTherefore your effect point won't be at the indicated position."), _("Warning"), wxOK|wxCANCEL|wxICON_WARNING, this);
        if (r == wxCANCEL)
            return false;
    }
    return wxDialog::Validate();
}

////////////////////////////////////////////////////////////////////////
//
//  dlgEffect, Matrix
//
////////////////////////////////////////////////////////////////////////

void dlgEffect::OnPosChange(wxCommandEvent& WXUNUSED(event)) {
    int sel = m_choicePoint->GetSelection();
    m_textX->Enable(sel<=0);
    m_textY->Enable(sel<=0);
    m_textZ->Enable(sel<=0);
    if (sel>0) {
        m_textX->ChangeValue(wxString::Format("%.4f", m_points[sel-1].x));
        m_textY->ChangeValue(wxString::Format("%.4f", m_points[sel-1].y));
        m_textZ->ChangeValue(wxString::Format("%.4f", m_points[sel-1].z));
    }
}

void dlgEffect::OnPosClick(wxCommandEvent& WXUNUSED(event)) {
    if (m_ef.Transform.size()) {
        if (::wxMessageBox(_("This delete all matrix entries and replace them with a Translation to the vector given.\nDo you want to continue?"), _("Question"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION, this)==wxNO)
            return;
    }

    m_ef.Transform.clear();
    m_ef.TransformNames.clear();

    D3DVECTOR v;
    double d;
    if (m_textX->GetValue().ToDouble(&d))
        v.x = d;
    else
        v.x = 0.0;
    if (m_textY->GetValue().ToDouble(&d))
        v.y = d;
    else
        v.y = 0.0;
    if (m_textZ->GetValue().ToDouble(&d))
        v.z = d;
    else
        v.z = 0.0;

    m_ef.TransformNames.push_back(wxString::Format(_("Translation by <%.4f,%.4f,%.4f>"), v.x, v.y, v.z));
    m_ef.Transform.push_back(matrixGetTranslation(v));
    long sel = m_choicePoint->GetSelection();
    if (sel<=0) {
        m_textX->SetValue(wxT(""));
        m_textY->SetValue(wxT(""));
        m_textZ->SetValue(wxT(""));
    }

    ShowTransform();
}

void dlgEffect::OnEditClick(wxCommandEvent& WXUNUSED(event)) {
    dlgMatrix *dialog = new dlgMatrix(this);
    dialog->SetStack(m_ef.Transform, m_ef.TransformNames);
    if (dialog->ShowModal() == wxID_OK) {
        m_ef.Transform = dialog->GetMatrices();
        m_ef.TransformNames = dialog->GetMatrixNames();
        ShowTransform();
    }
    dialog->Destroy();
}

void dlgEffect::OnClearClick(wxCommandEvent& WXUNUSED(event)) {
    if (m_ef.Transform.size()) {
        if (::wxMessageBox(_("Do you really want to delete all matrix entries?"), _("Question"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION, this)==wxNO)
            return;
    }

    m_ef.Transform.clear();
    m_ef.TransformNames.clear();
    ShowTransform();
}

////////////////////////////////////////////////////////////////////////
//
//  dlgEffect, Create button menu
//
////////////////////////////////////////////////////////////////////////

void dlgEffect::OnMenuLightStart(wxCommandEvent& WXUNUSED(event)) {
    dlgEffectLight *dialog = new dlgEffectLight(this);
    if (dialog->ShowModal()==wxID_OK) {
        SelectNr(dialog->GetLight(), 10+dialog->GetOffset());
    }
    dialog->Destroy();
}

void dlgEffect::OnMenuParticleStart(wxCommandEvent& WXUNUSED(event)) {
    wxArrayString choices;
    HKEY key;
    LONG res = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                            "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{907B4640-266B-4A21-92FB-CD1A86CD0F63}",
                            0, KEY_QUERY_VALUE, &key);
    char InstallLocation[MAX_PATH] = "";
    unsigned long length = MAX_PATH;
    res = RegQueryValueEx(key, "InstallLocation", 0, NULL, (LPBYTE) InstallLocation, &length);
    res = RegCloseKey(key);

    wxString Location = InstallLocation;
    Location += wxT("\\ParticleEffects\\");
    wxArrayString files;
    wxDir::GetAllFiles(Location, &files, wxT("*.frw"), wxDIR_FILES);

    choices.Add(_("(Enter Manually)"));
    for (wxArrayString::iterator s = files.begin(); s != files.end(); s++) {
        wxFileName f(*s);
        choices.Add(f.GetName());
    }

    int choice = ::wxGetSingleChoiceIndex(_("Select a particle for the effect point:"), _("Create Particle Effect Point (start)"), choices, this);
    if (choice >= 0) {
        if (choice)
            SelectNr(wxT("particlestart01_") + choices[choice].Lower(), 13);
        else
            SelectNr(wxT("particlestart01_"), 13);
    }
}

void dlgEffect::OnMenuParticleEnd(wxCommandEvent& WXUNUSED(event)) {
    wxArrayString choices;
    HKEY key;
    LONG res = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                            "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{907B4640-266B-4A21-92FB-CD1A86CD0F63}",
                            0, KEY_QUERY_VALUE, &key);
    char InstallLocation[MAX_PATH] = "";
    unsigned long length = MAX_PATH;
    res = RegQueryValueEx(key, "InstallLocation", 0, NULL, (LPBYTE) InstallLocation, &length);
    res = RegCloseKey(key);

    wxString Location = InstallLocation;
    Location += wxT("\\ParticleEffects\\");
    wxArrayString files;
    wxDir::GetAllFiles(Location, &files, wxT("*.frw"), wxDIR_FILES);

    choices.Add(_("(Enter Manually)"));
    for (wxArrayString::iterator s = files.begin(); s != files.end(); s++) {
        wxFileName f(*s);
        choices.Add(f.GetName());
    }

    int choice = ::wxGetSingleChoiceIndex(_("Select a particle for the effect point:"), _("Create Particle Effect Point (end)"), choices, this);
    if (choice >= 0) {
        if (choice)
            SelectNr(wxT("particleend01_") + choices[choice].Lower(), 11);
        else
            SelectNr(wxT("particleend01_"), 11);
    }
}

void dlgEffect::OnMenuLauncherStart(wxCommandEvent& WXUNUSED(event)) {
    SelectNr(wxT("fireworkstart01"), 13);
}

void dlgEffect::OnMenuLauncherEnd(wxCommandEvent& WXUNUSED(event)) {
    SelectNr(wxT("fireworkend01"), 11);
}

void dlgEffect::OnMenuPeepPeep(wxCommandEvent& WXUNUSED(event)) {
    SelectNr(wxT("peep01"), 4);
}

void dlgEffect::OnMenuPeepVendor(wxCommandEvent& WXUNUSED(event)) {
    SelectNr(wxT("vendor"), 6);
}

void dlgEffect::OnMenuFountainNOZBL(wxCommandEvent& WXUNUSED(event)) {
    SelectNr(wxT("nozbl"), 5);
}

void dlgEffect::OnMenuFountainNOZBR(wxCommandEvent& WXUNUSED(event)) {
    SelectNr(wxT("nozbr"), 5);
}

void dlgEffect::OnMenuFountainNOZTL(wxCommandEvent& WXUNUSED(event)) {
    SelectNr(wxT("noztl"), 5);
}

void dlgEffect::OnMenuFountainNOZTR(wxCommandEvent& WXUNUSED(event)) {
    SelectNr(wxT("noztr"), 5);
}

////////////////////////////////////////////////////////////////////////
//
//  dlgEffect, Load
//
////////////////////////////////////////////////////////////////////////

void dlgEffect::OnLoad(wxCommandEvent& WXUNUSED(event)) {
    wxFileDialog *dialog = new wxFileDialog(
                                     this,
                                     _T("Open Scenery File (Load effect point)"),
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
            unsigned int i;
            bool ef_present = false;
            for (i = 0; i < texscn->models.size(); i++) {
                if (texscn->models[i].effectpoints.size() != 0) {
                    ef_present = true;
                    break;
                }
            }
            if (!ef_present) {
                wxMessageBox(_("The selected scenery file contains no effect points."), _("Warning"), wxOK | wxICON_WARNING, this);
                delete texscn;
                dialog->Destroy();
                return;
            }
            wxArrayString choices;
            std::vector<cEffectPoint> epchoices;
            for (i = 0; i < texscn->models.size(); i++) {
                for (cEffectPointIterator ms = texscn->models[i].effectpoints.begin(); ms != texscn->models[i].effectpoints.end(); ms++) {
                    choices.Add(texscn->models[i].name + wxT(", ") + ms->Name);
                    epchoices.push_back(*ms);
                }
            }
            if (choices.size() > 0) {
                int choice = ::wxGetSingleChoiceIndex(dialog->GetPath(), _("Choose effect point to import"), choices, this);
                if (choice >= 0) {
                    SetEffect(epchoices[choice]);
                    TransferDataToWindow();
                    UpdateAll();
                }
            }
            delete texscn;
        }
    }
    dialog->Destroy();
}

