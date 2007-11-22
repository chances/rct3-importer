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
#include <wx/valgen.h>

#include "matrix.h"
#include "SCNFile.h"
#include "valext.h"
#include "wxInputBox.h"

#include "wxapp.h"
#include "wxdlgCreateScenery.h"
#include "wxdlgEffectLight.h"
#include "wxdlgMatrix.h"
#include "wxdlgMatrixLoad.h"
#include "wxdlgModel.h"

////////////////////////////////////////////////////////////////////////
//
//  dlgPosition
//
////////////////////////////////////////////////////////////////////////

class dlgPosition : public wxDialog {
protected:
    wxTextCtrl* m_textX;
    wxTextCtrl* m_textY;
    wxTextCtrl* m_textZ;
    wxButton* wxID_OK;
    wxButton* wxID_CANCEL;

private:
    void InitWidgetsFromXRC(wxWindow *parent){
        wxXmlResource::Get()->LoadObject(this,parent,_T("dlgPosition"), _T("wxDialog"));
        m_textX = XRCCTRL(*this,"m_textX",wxTextCtrl);
        m_textY = XRCCTRL(*this,"m_textY",wxTextCtrl);
        m_textZ = XRCCTRL(*this,"m_textZ",wxTextCtrl);
        wxID_OK = XRCCTRL(*this,"wxID_OK",wxButton);
        wxID_CANCEL = XRCCTRL(*this,"wxID_CANCEL",wxButton);
    }
public:
    float x, y, z;
    dlgPosition(wxWindow *parent=NULL): x(0.0), y(0.0), z(0.0) {
        InitWidgetsFromXRC((wxWindow *)parent);
        m_textX->SetValidator(wxExtendedValidator(&x));
        m_textY->SetValidator(wxExtendedValidator(&y));
        m_textZ->SetValidator(wxExtendedValidator(&z));
    }
};

////////////////////////////////////////////////////////////////////////
//
//  wxEffectCombo
//
////////////////////////////////////////////////////////////////////////

class wxEffectCombo : public wxComboCtrl
{
public:
    wxEffectCombo() : wxComboCtrl() { Init(); }
    virtual ~wxEffectCombo() {delete m_menuPopup;};

    wxEffectCombo(wxWindow *parent,
                        wxWindowID id = wxID_ANY,
                        const wxString& value = wxEmptyString,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        long style = 0,
                        const wxValidator& validator = wxDefaultValidator,
                        const wxString& name = wxComboBoxNameStr)
        : wxComboCtrl()
    {
        Init();
        Create(parent,id,value,
               pos,size,
               // Style flag wxCC_STD_BUTTON makes the button
               // behave more like a standard push button.
               style | wxCC_STD_BUTTON,
               validator,name);

        m_menuPopup = wxXmlResource::Get()->LoadMenu(wxT("menu_effect"));

        SetButtonBitmaps(wxXmlResource::Get()->LoadBitmap("bookmark_16x16"),true);
    }

    virtual void OnButtonClick()
    {
        PopupMenu(m_menuPopup);
    }

    // Implement empty DoSetPopupControl to prevent assertion failure.
    virtual void DoSetPopupControl(wxComboPopup* WXUNUSED(popup))
    {
    }

private:
    wxMenu* m_menuPopup;

    void Init()
    {
        // Initialize member variables here
    }
};

////////////////////////////////////////////////////////////////////////
//
//  dlgEffectBase
//
////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(dlgEffectBase,wxDialog)
//EVT_BUTTON(XRCID("m_btCreate"), dlgEffect::OnCreateClick)

EVT_MENU(XRCID("menu_effect_light_start"), dlgEffectBase::OnMenuLightStart)
EVT_MENU(XRCID("menu_effect_light_colour"), dlgEffectBase::OnMenuLightColour)
EVT_MENU(XRCID("menu_effect_light_end_normal"), dlgEffectBase::OnMenuLightEndNormal)
EVT_MENU(XRCID("menu_effect_light_end_simple"), dlgEffectBase::OnMenuLightEndSimple)
EVT_MENU(XRCID("menu_effect_light_end_nb"), dlgEffectBase::OnMenuLightEndNB)
EVT_MENU(XRCID("menu_effect_particle_start"), dlgEffectBase::OnMenuParticleStart)
EVT_MENU(XRCID("menu_effect_particle_end"), dlgEffectBase::OnMenuParticleEnd)
EVT_MENU(XRCID("menu_effect_launcher_start"), dlgEffectBase::OnMenuLauncherStart)
EVT_MENU(XRCID("menu_effect_launcher_end"), dlgEffectBase::OnMenuLauncherEnd)
EVT_MENU(XRCID("menu_effect_peep_peep"), dlgEffectBase::OnMenuPeepPeep)
EVT_MENU(XRCID("menu_effect_peep_vendor"), dlgEffectBase::OnMenuPeepVendor)
EVT_MENU(XRCID("menu_effect_fountain_nozbl"), dlgEffectBase::OnMenuFountainNOZBL)
EVT_MENU(XRCID("menu_effect_fountain_nozbr"), dlgEffectBase::OnMenuFountainNOZBR)
EVT_MENU(XRCID("menu_effect_fountain_noztl"), dlgEffectBase::OnMenuFountainNOZTL)
EVT_MENU(XRCID("menu_effect_fountain_noztr"), dlgEffectBase::OnMenuFountainNOZTR)

EVT_MENU(XRCID("menu_quickeffect_pos1"), dlgEffectBase::OnQuickMenu1)
EVT_MENU(XRCID("menu_quickeffect_this1"), dlgEffectBase::OnQuickMenu1)
EVT_MENU(XRCID("menu_quickeffect_load1"), dlgEffectBase::OnQuickMenu1)

END_EVENT_TABLE()


dlgEffectBase::dlgEffectBase(wxWindow *parent, bool effect) {
    if (effect)
        InitWidgetsFromXRCEffect(parent);
    else
        InitWidgetsFromXRCBone(parent);

    SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);

    m_menuPos1 = wxXmlResource::Get()->LoadMenu(wxT("menu_quickeffect1"));
    m_menuPos2 = (effect)?NULL:wxXmlResource::Get()->LoadMenu(wxT("menu_quickeffect2"));


    wxTextCtrl *t_text = XRCCTRL(*this,"m_textEffectName",wxTextCtrl);
    m_textEffectName = new wxEffectCombo(this, XRCID("m_textEffectName"));
    t_text->GetContainingSizer()->Replace(t_text, m_textEffectName);
    t_text->Destroy();

/*
    m_choicePoint->Append(_("(Custom)"));
    m_choicePoint->SetSelection(0);
    if (!effect) {
        m_choicePoint1->Append(_("(Custom)"));
        m_choicePoint1->SetSelection(0);
    }

    dlgModel *mod = dynamic_cast<dlgModel *> (parent);
    if (mod) {
        wxArrayString names;
        mod->FetchOneVertexMeshes(names, m_points);
        m_choicePoint->Append(names);
        if (!effect)
            m_choicePoint1->Append(names);
    }
    m_choicePoint->Enable(m_points.size()>0);
    if (!effect)
        m_choicePoint1->Enable(m_points.size()>0);
*/
    dlgModel *mod = dynamic_cast<dlgModel *> (parent);
    if (mod) {
        wxArrayString names;
        std::vector<D3DVECTOR> points;
        mod->FetchOneVertexMeshes(names, points);
        if (points.size()) {
            wxMenuItemList p1menuitems = m_menuPos1->GetMenuItems();
            wxMenu* p1item = NULL;
            for (wxMenuItemList::iterator node = p1menuitems.begin(); node != p1menuitems.end(); ++node) {
                wxMenuItem* i = *node;
                if (i->GetId() == XRCID("menu_quickeffect_ovm1")) {
                    p1item = i->GetSubMenu();
                    if (!p1item) {
                        p1item = new wxMenu();
                        i->SetSubMenu(p1item);
                    }
                    break;
                }
            }
            wxMenu* p2item = NULL;
            if (m_menuPos2) {
                wxMenuItemList p2menuitems = m_menuPos2->GetMenuItems();
                for (wxMenuItemList::iterator node = p2menuitems.begin(); node != p2menuitems.end(); ++node) {
                    wxMenuItem* i = *node;
                    if (i->GetId() == XRCID("menu_quickeffect_ovm2")) {
                        p2item = i->GetSubMenu();
                        if (!p2item) {
                            p2item = new wxMenu();
                            i->SetSubMenu(p2item);
                        }
                        break;
                    }
                }
            }
            for (int i = 0; i < points.size(); i++) {
                int newid = wxNewId();
                m_IdMap[0][newid] = points[i];
                m_IdNameMap[0][newid] = names[i];
                if (p1item) {
                    p1item->Append(newid, names[i]);
                    Connect(newid, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(dlgEffectBase::OnQuickMenu1));
                }
                if (p2item) {
                    newid = wxNewId();
                    m_IdMap[1][newid] = points[i];
                    m_IdNameMap[1][newid] = names[i];
                    p2item->Append(newid, names[i]);
                    Connect(newid, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(dlgEffectBase::OnQuickMenu2));
                }
            }
        }

    }

    wxInputBox *t_ibEffectName = new wxInputBox(this, wxID_ANY);
    t_ibEffectName->SetEditor(m_textEffectName);

    Fit();
    Layout();
    GetSizer()->SetSizeHints(this);

    m_btOk->SetId(wxID_OK);
    m_btCancel->SetId(wxID_CANCEL);

    //UpdateAll();
}

void dlgEffectBase::UpdateAll() {
    ShowTransform();
    UpdateMeshes();
}

void dlgEffectBase::SelectNr(wxString pt, unsigned int from) {
    m_textEffectName->GetTextCtrl()->SetValue(pt);
    m_textEffectName->SetFocus();
    m_textEffectName->SetSelection(from, from+2);
}
/*
void dlgEffect::OnCreateClick(wxCommandEvent& WXUNUSED(event)) {
//    PopupMenu(m_menuPopup);
}
*/

#define XRCID2(str_id, n) \
    wxXmlResource::GetXRCID(wxString(wxT(str_id))+wxString(wxT(n)))

void dlgEffectBase::DoQuickMenu(const int id, wxArrayString& names, std::vector<D3DMATRIX>& transforms, int nr) {
    const char* snr[2] = {"1", "2"};

    if (transforms.size()) {
        if (::wxMessageBox(_("This will delete all matrix entries and replace them with a quick-applied matrix.\nDo you want to continue?"), _("Question"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION, this)==wxNO)
            return;
    }

    wxString newmatrixname;
    D3DMATRIX newmatrix;

//    int id = event.GetId();
    if (id == XRCID2("menu_quickeffect_pos", snr[nr])) {
        dlgPosition* dialog = new dlgPosition(this);
        if (dialog->ShowModal() != wxID_OK) {
            dialog->Destroy();
            return;
        }
        D3DVECTOR v;
        v.x = dialog->x;
        v.y = dialog->y;
        v.z = dialog->z;
        newmatrixname = wxString::Format(_("Translation by <%.4f,%.4f,%.4f>"), v.x, v.y, v.z);
        newmatrix = matrixGetTranslation(v);
        dialog->Destroy();

        transforms.clear();
        names.clear();

        names.push_back(newmatrixname);
        transforms.push_back(newmatrix);
    } else if (id == XRCID2("menu_quickeffect_this", snr[nr])) {
        dlgCreateScenery* p = dynamic_cast<dlgCreateScenery*>(GetParent()->GetParent());
        if (p) {
            dlgLoadMatrix* dialog = new dlgLoadMatrix(&p->m_SCN, this);
            if (dialog->ShowModal() != wxID_OK) {
                dialog->Destroy();
                return;
            }
            dialog->GetStack(names, transforms);
            dialog->Destroy();
        }
    } else {
        wxIdOVMMap::iterator vec = m_IdMap[nr].find(id);
        if (vec != m_IdMap[nr].end()) {
            D3DVECTOR v = vec->second;
            newmatrixname = wxString::Format(_("Translation by <%.4f,%.4f,%.4f> (%s)"), v.x, v.y, v.z, m_IdNameMap[nr][id].c_str());
            newmatrix = matrixGetTranslation(v);

            transforms.clear();
            names.clear();

            names.push_back(newmatrixname);
            transforms.push_back(newmatrix);
        } else {
            return;
        }
    }


    ShowTransform();
}

////////////////////////////////////////////////////////////////////////
//
//  dlgEffectBase, Create button menu
//
////////////////////////////////////////////////////////////////////////

void dlgEffectBase::OnMenuLightStart(wxCommandEvent& WXUNUSED(event)) {
    dlgEffectLight *dialog = new dlgEffectLight(this);
    if (dialog->ShowModal()==wxID_OK) {
        SelectNr(dialog->GetLight(), 10+dialog->GetOffset());
    }
    dialog->Destroy();
}

void dlgEffectBase::OnMenuLightColour(wxCommandEvent& WXUNUSED(event)) {
    dlgEffectLight *dialog = new dlgEffectLight(this, true);
    if (dialog->ShowModal()==wxID_OK) {
        SelectNr(dialog->GetLight(), 11+dialog->GetOffset());
    }
    dialog->Destroy();
}

void dlgEffectBase::OnMenuLightEndNormal(wxCommandEvent& WXUNUSED(event)) {
    SelectNr(wxT("lightend01"), 8);
}

void dlgEffectBase::OnMenuLightEndSimple(wxCommandEvent& WXUNUSED(event)) {
    SelectNr(wxT("simplelightend01"), 14);
}

void dlgEffectBase::OnMenuLightEndNB(wxCommandEvent& WXUNUSED(event)) {
    SelectNr(wxT("nblightend01"), 10);
}

void dlgEffectBase::OnMenuParticleStart(wxCommandEvent& WXUNUSED(event)) {
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

void dlgEffectBase::OnMenuParticleEnd(wxCommandEvent& WXUNUSED(event)) {
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

void dlgEffectBase::OnMenuLauncherStart(wxCommandEvent& WXUNUSED(event)) {
    SelectNr(wxT("fireworkstart01"), 13);
}

void dlgEffectBase::OnMenuLauncherEnd(wxCommandEvent& WXUNUSED(event)) {
    SelectNr(wxT("fireworkend01"), 11);
}

void dlgEffectBase::OnMenuPeepPeep(wxCommandEvent& WXUNUSED(event)) {
    SelectNr(wxT("peep01"), 4);
}

void dlgEffectBase::OnMenuPeepVendor(wxCommandEvent& WXUNUSED(event)) {
    SelectNr(wxT("vendor"), 6);
}

void dlgEffectBase::OnMenuFountainNOZBL(wxCommandEvent& WXUNUSED(event)) {
    SelectNr(wxT("nozbl"), 5);
}

void dlgEffectBase::OnMenuFountainNOZBR(wxCommandEvent& WXUNUSED(event)) {
    SelectNr(wxT("nozbr"), 5);
}

void dlgEffectBase::OnMenuFountainNOZTL(wxCommandEvent& WXUNUSED(event)) {
    SelectNr(wxT("noztl"), 5);
}

void dlgEffectBase::OnMenuFountainNOZTR(wxCommandEvent& WXUNUSED(event)) {
    SelectNr(wxT("noztr"), 5);
}

////////////////////////////////////////////////////////////////////////
//
//  dlgEffect
//
////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(dlgEffect,dlgEffectBase)
EVT_BUTTON(XRCID("m_btEdit1"), dlgEffect::OnEditClick)
EVT_BUTTON(XRCID("m_btQuick1"), dlgEffect::OnQuickClick)
EVT_BUTTON(XRCID("m_btClear1"), dlgEffect::OnClearClick)

EVT_BUTTON(XRCID("m_btLoad"), dlgEffect::OnLoad)
END_EVENT_TABLE()

dlgEffect::dlgEffect(wxWindow *parent):dlgEffectBase(parent, true) {
    m_textEffectName->SetValidator(wxExtendedValidator(&m_ef.name, false));
    ShowTransform();
}

void dlgEffect::ShowTransform(int pr) {
    D3DMATRIX m = matrixMultiply(m_ef.transforms);
    for (int i=0; i<4; i++)
        for (int j=0; j<4; j++)
            m_Matrix[i][j]->SetLabel(wxString::Format("%.*f", pr, m.m[i][j]));
}

bool dlgEffect::Validate() {
/*
    if ((m_ef.transforms.size() == 0) && ((m_textX->GetValue() != wxT("")) || (m_textY->GetValue() != wxT("")) || (m_textZ->GetValue() != wxT("")))) {
        int r = ::wxMessageBox(_("You entered values in the position fields but didn't press the 'Pos' button.\nTherefore your effect point won't be at the indicated position."), _("Warning"), wxOK|wxCANCEL|wxICON_WARNING, this);
        if (r == wxCANCEL)
            return false;
    }
*/
    return wxDialog::Validate();
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
                for (cEffectPoint::iterator ms = texscn->models[i].effectpoints.begin(); ms != texscn->models[i].effectpoints.end(); ms++) {
                    choices.Add(texscn->models[i].name + wxT(", ") + ms->name);
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

////////////////////////////////////////////////////////////////////////
//
//  dlgEffect, Matrix
//
////////////////////////////////////////////////////////////////////////

//void dlgEffect::OnPosChange(wxCommandEvent& WXUNUSED(event)) {
//    int sel = m_choicePoint->GetSelection();
//    m_textX->Enable(sel<=0);
//    m_textY->Enable(sel<=0);
//    m_textZ->Enable(sel<=0);
//    if (sel>0) {
//        m_textX->ChangeValue(wxString::Format("%.4f", m_points[sel-1].x));
//        m_textY->ChangeValue(wxString::Format("%.4f", m_points[sel-1].y));
//        m_textZ->ChangeValue(wxString::Format("%.4f", m_points[sel-1].z));
//    }
//}

void dlgEffect::OnQuickMenu1(wxCommandEvent& event) {
    DoQuickMenu(event.GetId(), m_ef.transformnames, m_ef.transforms, 0);
}

void dlgEffect::OnEditClick(wxCommandEvent& WXUNUSED(event)) {
    dlgMatrix *dialog = new dlgMatrix(this);
    dialog->SetStack(m_ef.transforms, m_ef.transformnames);
    if (dialog->ShowModal() == wxID_OK) {
        m_ef.transforms = dialog->GetMatrices();
        m_ef.transformnames = dialog->GetMatrixNames();
        ShowTransform();
    }
    dialog->Destroy();
}

void dlgEffect::OnQuickClick(wxCommandEvent& WXUNUSED(event)) {
    PopupMenu(m_menuPos1);
}

void dlgEffect::OnClearClick(wxCommandEvent& WXUNUSED(event)) {
    if (m_ef.transforms.size()) {
        if (::wxMessageBox(_("Do you really want to delete all matrix entries?"), _("Question"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION, this)==wxNO)
            return;
    }

    m_ef.transforms.clear();
    m_ef.transformnames.clear();
    ShowTransform();
}


////////////////////////////////////////////////////////////////////////
//
//  dlgBone
//
////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(dlgBone,dlgEffectBase)
EVT_CHOICE(XRCID("m_choiceMesh"), dlgBone::OnUpdate)
EVT_LISTBOX(XRCID("m_lbMeshes"), dlgBone::OnUpdate)
EVT_BUTTON(XRCID("m_btMeshAdd"), dlgBone::OnAddMeshClick)
EVT_BUTTON(XRCID("m_btMeshDel"), dlgBone::OnDelMeshClick)
EVT_BUTTON(XRCID("m_btMeshClear"), dlgBone::OnClearMeshClick)

EVT_BUTTON(XRCID("m_btEdit1"), dlgBone::OnEditClick)
EVT_BUTTON(XRCID("m_btQuick1"), dlgBone::OnQuickClick)
EVT_BUTTON(XRCID("m_btClear1"), dlgBone::OnClearClick)
EVT_BUTTON(XRCID("m_btEdit2"), dlgBone::OnEdit1Click)
EVT_BUTTON(XRCID("m_btQuick2"), dlgBone::OnQuick1Click)
EVT_BUTTON(XRCID("m_btClear2"), dlgBone::OnClear1Click)
EVT_CHECKBOX(XRCID("m_cbUsePos2"), dlgBone::OnUse2Click)

EVT_BUTTON(XRCID("m_btLoad"), dlgBone::OnLoad)

EVT_MENU(XRCID("menu_quickeffect_pos2"), dlgBone::OnQuickMenu2)
EVT_MENU(XRCID("menu_quickeffect_pos12"), dlgBone::OnQuickMenu2)
EVT_MENU(XRCID("menu_quickeffect_this2"), dlgBone::OnQuickMenu2)
EVT_MENU(XRCID("menu_quickeffect_load2"), dlgBone::OnQuickMenu2)

END_EVENT_TABLE()

dlgBone::dlgBone(wxWindow *parent):dlgEffectBase(parent, false) {
    m_textEffectName->SetValidator(wxExtendedValidator(&m_bn.name, false));

    if (m_bn.parent == wxT("")) {
        m_bn.parent = wxT("-");
    }
    m_choiceParent->Append(wxT("-"));
    m_choiceParent->SetValidator(wxGenericValidator(&m_bn.parent));
    //ShowTransform();

    m_model = NULL;
    dlgModel* p = dynamic_cast<dlgModel*>(parent);
    if (p) {
        m_model = dynamic_cast<cAnimatedModel*>(p->m_model);
        if (m_model) {
            for (cMeshStruct::iterator it = m_model->meshstructs.begin(); it != m_model->meshstructs.end(); it++) {
                if (it->valid) {
                    m_meshes.Add(it->Name);
                }
            }
        }
    }
/*
    for (cStringIterator it = m_meshes.begin(); it != m_meshes.end(); it++) {
        m_choiceMesh->Append(*it);
    }
*/
    UpdateAll();
}

void dlgBone::ShowTransform(int pr) {
    D3DMATRIX m = matrixMultiply(m_bn.positions1);
    for (int i=0; i<4; i++)
        for (int j=0; j<4; j++) {
            m_Matrix[i][j]->SetLabel(wxString::Format("%.*f", pr, m.m[i][j]));
            if (!m_bn.usepos2)
                m_Matrix2[i][j]->SetLabel(wxString::Format("%.*f", pr, m.m[i][j]));
        }
    if (m_bn.usepos2) {
        m = matrixMultiply(m_bn.positions2);
        for (int i=0; i<4; i++)
            for (int j=0; j<4; j++)
                m_Matrix2[i][j]->SetLabel(wxString::Format("%.*f", pr, m.m[i][j]));
    }
}

bool dlgBone::Validate() {
/*
    if ((m_bn.positions1.size() == 0) && ((m_textX->GetValue() != wxT("")) || (m_textY->GetValue() != wxT("")) || (m_textZ->GetValue() != wxT("")))) {
        int r = ::wxMessageBox(_("You entered values in the position fields but didn't press the 'Pos' button.\nTherefore your effect point won't be at the indicated position."), _("Warning"), wxOK|wxCANCEL|wxICON_WARNING, this);
        if (r == wxCANCEL)
            return false;
    }
    if ((m_bn.usepos2) && (m_bn.positions2.size() == 0) && ((m_textX1->GetValue() != wxT("")) || (m_textY1->GetValue() != wxT("")) || (m_textZ1->GetValue() != wxT("")))) {
        int r = ::wxMessageBox(_("You entered values in the position fields for position2 but didn't press the 'Pos' button.\nTherefore your effect point won't be at the indicated position."), _("Warning"), wxOK|wxCANCEL|wxICON_WARNING, this);
        if (r == wxCANCEL)
            return false;
    }
*/
    return dlgEffectBase::Validate();
}

void dlgBone::UpdateMeshes() {
    m_choiceMesh->Clear();
    m_lbMeshes->Clear();
    if (m_meshes.size()) {
        for (cStringIterator it = m_meshes.begin(); it != m_meshes.end(); it++) {
            if (m_bn.meshes.Index(*it) == wxNOT_FOUND) {
                // It's not assigned to the current bone, now set's see if it's assigned to another
                bool found = false;
                if (m_model) {
                    for (cModelBone::iterator mb = m_model->modelbones.begin(); mb != m_model->modelbones.end(); mb++) {
                        if (mb->meshes.Index(*it) != wxNOT_FOUND) {
                            found = true;
                            break;
                        }
                    }
                }
                if (!found)
                    m_choiceMesh->Append(*it);
            } else {
                m_lbMeshes->Append(*it);
            }
        }
    }
    UpdateState();
}

void dlgBone::UpdateState() {
    m_choiceMesh->Enable(!m_choiceMesh->IsEmpty());
    m_btMeshAdd->Enable(m_choiceMesh->GetSelection()!=wxNOT_FOUND);
    m_btMeshDel->Enable(m_lbMeshes->GetSelection()!=wxNOT_FOUND);
    m_btMeshClear->Enable(!m_lbMeshes->IsEmpty());
}

void dlgBone::SetBone(const cModelBone& bn) {
    m_bn = bn;
    if (m_bn.parent == wxT("")) {
        m_bn.parent = wxT("-");
    }
//    dlgModel* pmodel = dynamic_cast<dlgModel*>(GetParent());
    m_choiceParent->Clear();
    m_choiceParent->Append(wxT("-"));
//    if (m_model) {
//        cAnimatedModel* amodel = dynamic_cast<cAnimatedModel*>(pmodel->m_model);
        if (m_model) {
            for (cModelBone::iterator it = m_model->modelbones.begin(); it != m_model->modelbones.end(); it++) {
                if ((m_bn.name.IsSameAs(wxT(""))) || ((!it->name.IsSameAs(m_bn.name)) && (!it->parent.IsSameAs(m_bn.name))))
                    m_choiceParent->Append(it->name);
            }
        }
//    }
    UpdateAll();
}

////////////////////////////////////////////////////////////////////////
//
//  dlgBone, Load
//
////////////////////////////////////////////////////////////////////////

void dlgBone::OnLoad(wxCommandEvent& WXUNUSED(event)) {
    wxFileDialog *dialog = new wxFileDialog(
                                     this,
                                     _T("Open Scenery File (Load bone)"),
                                     wxEmptyString,
                                     wxEmptyString,
                                     _T("Scenery Files (*.scn, *.xml)|*.scn;*.xml"),
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
                for (cEffectPoint::iterator ms = texscn->models[i].effectpoints.begin(); ms != texscn->models[i].effectpoints.end(); ms++) {
                    choices.Add(texscn->models[i].name + wxT(", ") + ms->name);
                    epchoices.push_back(*ms);
                }
            }
            if (choices.size() > 0) {
                int choice = ::wxGetSingleChoiceIndex(dialog->GetPath(), _("Choose effect point to import"), choices, this);
                if (choice >= 0) {
// FIXME (tobi#1#): Make load work for bones
//                    SetEffect(epchoices[choice]);
                    TransferDataToWindow();
                    UpdateAll();
                }
            }
            delete texscn;
        }
    }
    dialog->Destroy();
}

////////////////////////////////////////////////////////////////////////
//
//  dlgBone, Meshes
//
////////////////////////////////////////////////////////////////////////

void dlgBone::OnAddMeshClick(wxCommandEvent& WXUNUSED(event)) {
    wxString sel = m_choiceMesh->GetStringSelection();
    if (sel != wxT("")) {
        m_bn.meshes.Add(sel);
        UpdateMeshes();
    }
}

void dlgBone::OnDelMeshClick(wxCommandEvent& WXUNUSED(event)) {
    wxString sel = m_lbMeshes->GetStringSelection();
    if (sel != wxT("")) {
        m_bn.meshes.Remove(sel);
        UpdateMeshes();
    }
}

void dlgBone::OnClearMeshClick(wxCommandEvent& WXUNUSED(event)) {
    if (m_bn.meshes.size()) {
        if (::wxMessageBox(_("Do you really want to delete all assigned meshes?"), _("Question"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION, this)==wxNO)
            return;
    }

    m_bn.meshes.clear();
    UpdateMeshes();
}


////////////////////////////////////////////////////////////////////////
//
//  dlgBone, Position 1 Matrix
//
////////////////////////////////////////////////////////////////////////

void dlgBone::OnQuickMenu1(wxCommandEvent& event) {
    DoQuickMenu(event.GetId(), m_bn.position1names, m_bn.positions1, 0);
}

void dlgBone::OnEditClick(wxCommandEvent& WXUNUSED(event)) {
    dlgMatrix *dialog = new dlgMatrix(this);
    dialog->SetStack(m_bn.positions1, m_bn.position1names);
    if (dialog->ShowModal() == wxID_OK) {
        m_bn.positions1 = dialog->GetMatrices();
        m_bn.position1names = dialog->GetMatrixNames();
        ShowTransform();
    }
    dialog->Destroy();
}

void dlgBone::OnQuickClick(wxCommandEvent& WXUNUSED(event)) {
    PopupMenu(m_menuPos1);
}

void dlgBone::OnClearClick(wxCommandEvent& WXUNUSED(event)) {
    if (m_bn.positions1.size()) {
        if (::wxMessageBox(_("Do you really want to delete all matrix entries?"), _("Question"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION, this)==wxNO)
            return;
    }

    m_bn.positions1.clear();
    m_bn.position1names.clear();
    ShowTransform();
}

////////////////////////////////////////////////////////////////////////
//
//  dlgBone, Position 2 Matrix
//
////////////////////////////////////////////////////////////////////////

void dlgBone::OnQuickMenu2(wxCommandEvent& event) {
    DoQuickMenu(event.GetId(), m_bn.position2names, m_bn.positions2, 1);
}

void dlgBone::OnEdit1Click(wxCommandEvent& WXUNUSED(event)) {
    dlgMatrix *dialog = new dlgMatrix(this);
    dialog->SetStack(m_bn.positions2, m_bn.position2names);
    if (dialog->ShowModal() == wxID_OK) {
        m_bn.positions2 = dialog->GetMatrices();
        m_bn.position2names = dialog->GetMatrixNames();
        ShowTransform();
    }
    dialog->Destroy();
}

void dlgBone::OnQuick1Click(wxCommandEvent& WXUNUSED(event)) {
    PopupMenu(m_menuPos2);
}

void dlgBone::OnClear1Click(wxCommandEvent& WXUNUSED(event)) {
    if (m_bn.positions2.size()) {
        if (::wxMessageBox(_("Do you really want to delete all matrix entries?"), _("Question"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION, this)==wxNO)
            return;
    }

    m_bn.positions2.clear();
    m_bn.position2names.clear();
    ShowTransform();
}

void dlgBone::OnUse2Click(wxCommandEvent& WXUNUSED(event)) {
    if (m_bn.usepos2 && m_bn.positions2.size()) {
        if (::wxMessageBox(_("Deactivating the position 2 matrix will delete the complete stack for it.\nDo you want to continue?"), _("Question"), wxYES_NO|wxNO_DEFAULT|wxICON_QUESTION, this) == wxNO) {
            m_cbUsePos2->SetValue(m_bn.usepos2);
            return;
        }
        m_bn.positions2.clear();
        m_bn.position2names.clear();
    }

    m_bn.usepos2 = !m_bn.usepos2;
    m_btClear2->Enable(m_bn.usepos2);
    m_btEdit2->Enable(m_bn.usepos2);
    m_btQuick2->Enable(m_bn.usepos2);
    m_cbUsePos2->SetValue(m_bn.usepos2);
    ShowTransform();
}

