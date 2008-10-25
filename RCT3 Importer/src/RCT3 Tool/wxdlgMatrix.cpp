///////////////////////////////////////////////////////////////////////////////
//
// Matrix Editor Dialog
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

#include "wxdlgMatrix.h"

#include "wxapp.h"

#include "htmlentities.h"
#include "matrix.h"
#include "pretty.h"
#include "SCNFile.h"

#include "confhelp.h"

#include "wxdlgModel.h"

using namespace r3;

////////////////////////////////////////////////////////////////////////
//
//  wxMatrixListBox
//
////////////////////////////////////////////////////////////////////////

wxMatrixListBox::wxMatrixListBox(wxWindow *parent, wxArrayString *content):
        wxColourHtmlListBox(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER) {
    m_contents = content;
    UpdateContents();
    SetSelection(0);
}

void wxMatrixListBox::UpdateContents() {
    SetItemCount(m_contents->GetCount()+1);
    RefreshAll();
}

wxString wxMatrixListBox::OnGetItem(size_t n) const {
    if (n == 0) {
        return _("<font color='#C0C0C0' size='2'>(Insert at start)</font>");
    } else {
        if (m_contents->Item(n-1) == wxT("-"))
            return wxT("<hr>");
        else
            return wxT("<font size='2'>")+wxEncodeHtmlEntities(m_contents->Item(n-1))+wxT("</font>");
    }
}


////////////////////////////////////////////////////////////////////////
//
//  dlgMatrix
//
////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(dlgMatrix,wxDialog)
EVT_LISTBOX(XRCID("m_htlbMatrix"), dlgMatrix::OnMatrixChange)
EVT_SPIN_UP(XRCID("m_spinMatrix"), dlgMatrix::OnMatrixMoveUp)
EVT_SPIN_DOWN(XRCID("m_spinMatrix"), dlgMatrix::OnMatrixMoveDown)
EVT_BUTTON(XRCID("m_btMatrixCopy"), dlgMatrix::OnMatrixCopy)
EVT_BUTTON(XRCID("m_btMatrixInvert"), dlgMatrix::OnMatrixInverse)
EVT_BUTTON(XRCID("m_btMatrixDel"), dlgMatrix::OnMatrixDelete)
EVT_BUTTON(XRCID("m_btMatrixClearAll"), dlgMatrix::OnMatrixDeleteAll)

EVT_TEXT(XRCID("m_text11"), dlgMatrix::OnMatrixContentChange)
EVT_TEXT(XRCID("m_text12"), dlgMatrix::OnMatrixContentChange)
EVT_TEXT(XRCID("m_text13"), dlgMatrix::OnMatrixContentChange)
EVT_TEXT(XRCID("m_text14"), dlgMatrix::OnMatrixContentChange)
EVT_TEXT(XRCID("m_text21"), dlgMatrix::OnMatrixContentChange)
EVT_TEXT(XRCID("m_text22"), dlgMatrix::OnMatrixContentChange)
EVT_TEXT(XRCID("m_text23"), dlgMatrix::OnMatrixContentChange)
EVT_TEXT(XRCID("m_text24"), dlgMatrix::OnMatrixContentChange)
EVT_TEXT(XRCID("m_text31"), dlgMatrix::OnMatrixContentChange)
EVT_TEXT(XRCID("m_text32"), dlgMatrix::OnMatrixContentChange)
EVT_TEXT(XRCID("m_text33"), dlgMatrix::OnMatrixContentChange)
EVT_TEXT(XRCID("m_text34"), dlgMatrix::OnMatrixContentChange)
EVT_TEXT(XRCID("m_text41"), dlgMatrix::OnMatrixContentChange)
EVT_TEXT(XRCID("m_text42"), dlgMatrix::OnMatrixContentChange)
EVT_TEXT(XRCID("m_text43"), dlgMatrix::OnMatrixContentChange)
EVT_TEXT(XRCID("m_text44"), dlgMatrix::OnMatrixContentChange)
EVT_TEXT(XRCID("m_textMatrixName"), dlgMatrix::OnMatrixContentChange)

EVT_CHOICE(XRCID("m_choiceTranslationPoint"), dlgMatrix::OnTranslateChoice)
EVT_BUTTON(XRCID("m_btTranslate"), dlgMatrix::OnTranslate)
EVT_BUTTON(XRCID("m_btTranslateOrigin"), dlgMatrix::OnTranslateOrigin)

EVT_BUTTON(XRCID("m_btRotationX"), dlgMatrix::OnRotate)
EVT_BUTTON(XRCID("m_btRotationY"), dlgMatrix::OnRotate)
EVT_BUTTON(XRCID("m_btRotationZ"), dlgMatrix::OnRotate)

EVT_BUTTON(XRCID("m_btScale"), dlgMatrix::OnScale)
EVT_BUTTON(XRCID("m_btMirrorX"), dlgMatrix::OnMirror)
EVT_BUTTON(XRCID("m_btMirrorY"), dlgMatrix::OnMirror)
EVT_BUTTON(XRCID("m_btMirrorZ"), dlgMatrix::OnMirror)

EVT_BUTTON(XRCID("m_btFull"), dlgMatrix::OnSpecialBone)
EVT_BUTTON(XRCID("m_btDefault"), dlgMatrix::OnSpecialDefault)
EVT_BUTTON(XRCID("m_btLoad"), dlgMatrix::OnSpecialLoad)

EVT_BUTTON(XRCID("m_btMatrixLoadAll"), dlgMatrix::OnLoad)
END_EVENT_TABLE()

dlgMatrix::dlgMatrix(wxWindow *parent): m_bones(NULL) {
    InitWidgetsFromXRC((wxWindow *)parent);

    m_htlbMatrix = new wxMatrixListBox(this, &m_matrixnames);
    m_htlbMatrix->SetToolTip(_("Matrix stack.\nSelect the first entry to see a preview of the combined matrix below."));
    wxXmlResource::Get()->AttachUnknownControl(wxT("m_htlbMatrix"), m_htlbMatrix, this);
    m_staticDegree->SetLabel(wxT("°"));
    m_choiceTranslationPoint->Append(_("(Custom)"));
    m_choiceTranslationPoint->SetSelection(0);

    // Find the dlgModel ancestor. Either the parent or the grandparent.
    dlgModel *mod = dynamic_cast<dlgModel *> (parent);
    if (!mod) {
        if (parent)
            mod = dynamic_cast<dlgModel *> (parent->GetParent());
    }
    if (mod) {
        if (mod->GetModelPtr()->model_bones.size()) {
            m_bones = &mod->GetModelPtr()->model_bones;
/*
            for (std::vector<c3DBone>::const_iterator it = m_bones->begin(); it != m_bones->end(); ++it) {
                m_choiceTranslationPoint->Append(it->m_name);
                m_choiceBone->Append(it->m_name);
            }
*/
            foreach(const c3DBone::pair& bn, *m_bones) {
                m_choiceTranslationPoint->Append(bn.first);
                m_choiceBone->Append(bn.first);
                m_boneId.push_back(&bn.second);
            }
        }
    }
    m_choiceTranslationPoint->Enable(m_bones);
    m_choiceBone->Enable(m_bones);

    Fit();
    Layout();
    GetSizer()->SetSizeHints(this);

    m_btOk->SetId(wxID_OK);
    m_btCancel->SetId(wxID_CANCEL);

}

void dlgMatrix::AddMatrix(const wxString name, const MATRIX mat, bool setcur) {
    int sel = m_htlbMatrix->GetSelection();
    if (sel < 0)
        return;
    if (sel < m_matrices.size()) {
        m_matrices.insert(m_matrices.begin() + sel, mat);
        m_matrixnames.insert(m_matrixnames.begin() + sel, name);
    } else {
        m_matrices.push_back(mat);
        m_matrixnames.push_back(name);
    }
    m_htlbMatrix->UpdateContents();
    if (setcur) {
        m_htlbMatrix->SetSelection(sel+1);
        DisplayMatrix();
    } else
        m_htlbMatrix->SetSelection(sel);
}

void dlgMatrix::DisplayMatrix() {
    unsigned int i, j;
    int sel = m_htlbMatrix->GetSelection();
    m_spinMatrix->Enable(sel>0);
    m_btMatrixCopy->Enable(sel>0);
    m_btMatrixInvert->Enable(sel>0);
    m_btMatrixDel->Enable(sel>0);
    bool matrix_enable = true;
    if (sel<=0) {
        m_textMatrixName->ChangeValue(_("<Preview of the combined matrix>"));
        ShowMatrix(matrixMultiply(m_matrices));
        matrix_enable = false;
    } else if (m_matrixnames[sel-1] == wxT("-")) {
        m_textMatrixName->ChangeValue(_("<Separator>"));
        ShowMatrix(m_matrices[sel-1], 10);
        matrix_enable = false;
    } else {
        m_textMatrixName->ChangeValue(m_matrixnames[sel-1]);
        ShowMatrix(m_matrices[sel-1], 10);
    }
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            m_textM[i][j]->Enable(matrix_enable);
    m_textMatrixName->Enable(matrix_enable);
}

void dlgMatrix::ShowMatrix(const MATRIX m, unsigned int l) {
    unsigned int i, j;
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            m_textM[i][j]->ChangeValue(wxString::Format("%.*f", l, m.m[i][j]));
}

////////////////////////////////////////////////////////////////////////
//
//  dlgMatrix, Matrix
//
////////////////////////////////////////////////////////////////////////

void dlgMatrix::OnMatrixChange(wxCommandEvent& WXUNUSED(event)) {
    DisplayMatrix();
}

void dlgMatrix::OnMatrixContentChange(wxCommandEvent& WXUNUSED(event)) {
    int sel = m_htlbMatrix->GetSelection();
    if ((sel>0) && (m_matrixnames[sel-1] != wxT("-"))) {
        MATRIX *m = &m_matrices[sel-1];
        wxString t_name = m_textMatrixName->GetValue();
        if (t_name == wxT("-"))
            t_name = wxT("- ");
        m_matrixnames[sel-1] = t_name;
        unsigned int i, j;
        double d;
        for (i = 0; i < 4; i++)
            for (j = 0; j < 4; j++) {
                if (m_textM[i][j]->GetValue().ToDouble(&d))
                    m->m[i][j] = d;
                else
                    m->m[i][j] = 0.0;
            }
    }
}

void dlgMatrix::OnMatrixMoveUp(wxSpinEvent& WXUNUSED(event)) {
    int sel = m_htlbMatrix->GetSelection();
    if (sel<=1)
        return;
    if (::wxGetKeyState(WXK_SHIFT)) {
        MATRIX temp = m_matrices[sel-1];
        m_matrices.erase(m_matrices.begin() + sel-1);
        matrixMultiplyIP(m_matrices[sel-2], temp);
        wxString temps = m_matrixnames[sel-1];
        m_matrixnames.erase(m_matrixnames.begin() + sel-1);
        m_matrixnames[sel-2] += " & " + temps;
    } else {
        MATRIX temp = m_matrices[sel-1];
        m_matrices.erase(m_matrices.begin() + sel-1);
        m_matrices.insert(m_matrices.begin() + sel-2, temp);
        wxString temps = m_matrixnames[sel-1];
        m_matrixnames.erase(m_matrixnames.begin() + sel-1);
        m_matrixnames.insert(m_matrixnames.begin() + sel-2, temps);
    }
    m_htlbMatrix->UpdateContents();
    m_htlbMatrix->SetSelection(sel-1);
    DisplayMatrix();
}

void dlgMatrix::OnMatrixMoveDown(wxSpinEvent& WXUNUSED(event)) {
    int sel = m_htlbMatrix->GetSelection();
    if ((sel<=0) || (sel>=m_matrices.size()))
        return;
    if (::wxGetKeyState(WXK_SHIFT)) {
        MATRIX temp = m_matrices[sel];
        matrixMultiplyIP(m_matrices[sel-1], temp);
        m_matrices.erase(m_matrices.begin() + sel);
        wxString temps = m_matrixnames[sel];
        m_matrixnames[sel-1] += " & " + temps;
        m_matrixnames.erase(m_matrixnames.begin() + sel);
        m_htlbMatrix->UpdateContents();
        m_htlbMatrix->SetSelection(sel);
    } else {
        MATRIX temp = m_matrices[sel-1];
        m_matrices.erase(m_matrices.begin() + sel-1);
        m_matrices.insert(m_matrices.begin() + sel, temp);
        wxString temps = m_matrixnames[sel-1];
        m_matrixnames.erase(m_matrixnames.begin() + sel-1);
        m_matrixnames.insert(m_matrixnames.begin() + sel, temps);
        m_htlbMatrix->UpdateContents();
        m_htlbMatrix->SetSelection(sel+1);
    }
    DisplayMatrix();
}

void dlgMatrix::OnMatrixCopy(wxCommandEvent& WXUNUSED(event)) {
    int sel = m_htlbMatrix->GetSelection();
    if (sel<=0)
        return;

    AddMatrix(m_matrixnames[sel-1] + _(" Copy"), m_matrices[sel-1]);
}

void dlgMatrix::OnMatrixInverse(wxCommandEvent& WXUNUSED(event)) {
    int sel = m_htlbMatrix->GetSelection();
    if (sel<=0)
        return;
    m_matrices[sel-1] = matrixInverse(m_matrices[sel-1]);
    wxString temp;
    if (!m_matrixnames[sel-1].StartsWith(_("Inverse "), &temp))
        temp = _("Inverse ") + m_matrixnames[sel-1];
    m_matrixnames[sel-1] = temp;
    m_htlbMatrix->UpdateContents();
    m_htlbMatrix->SetSelection(sel);
    DisplayMatrix();
}

void dlgMatrix::OnMatrixDelete(wxCommandEvent& WXUNUSED(event)) {
    int sel = m_htlbMatrix->GetSelection();
    if (sel<=0)
        return;
    m_matrices.erase(m_matrices.begin() + sel-1);
    m_matrixnames.erase(m_matrixnames.begin() + sel-1);
    m_htlbMatrix->UpdateContents();
    m_htlbMatrix->SetSelection(sel-1);
    DisplayMatrix();
}

void dlgMatrix::OnMatrixDeleteAll(wxCommandEvent& WXUNUSED(event)) {
    if (m_matrices.size()) {
        if (::wxMessageBox(_("Do you really want to delete all matrices?"), _("Question"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION, this)==wxNO)
            return;
    }

    m_matrices.clear();
    m_matrixnames.clear();
    m_htlbMatrix->UpdateContents();
    m_htlbMatrix->SetSelection(0);
    DisplayMatrix();
}

////////////////////////////////////////////////////////////////////////
//
//  dlgMatrix, Translation
//
////////////////////////////////////////////////////////////////////////

void dlgMatrix::OnTranslateChoice(wxCommandEvent& WXUNUSED(event)) {
    int sel = m_choiceTranslationPoint->GetSelection();
    m_textTranslationX->Enable(sel<=0);
    m_textTranslationY->Enable(sel<=0);
    m_textTranslationZ->Enable(sel<=0);
    VECTOR v = matrixExtractTranslation(m_boneId[sel-1]->m_pos[1]);
    if (sel>0) {
        m_textTranslationX->ChangeValue(wxString::Format("%.6f", v.x));
        m_textTranslationY->ChangeValue(wxString::Format("%.6f", v.y));
        m_textTranslationZ->ChangeValue(wxString::Format("%.6f", v.z));
    }
}

void dlgMatrix::OnTranslate(wxCommandEvent& WXUNUSED(event)) {
    VECTOR v;
    double d;
    if (m_textTranslationX->GetValue().ToDouble(&d))
        v.x = d;
    else
        v.x = 0.0;
    if (m_textTranslationY->GetValue().ToDouble(&d))
        v.y = d;
    else
        v.y = 0.0;
    if (m_textTranslationZ->GetValue().ToDouble(&d))
        v.z = d;
    else
        v.z = 0.0;
    AddMatrix(wxString::Format(_("Translation by <%.4f,%.4f,%.4f>"), v.x, v.y, v.z), matrixGetTranslation(v));
    long sel = m_choiceTranslationPoint->GetSelection();
    if (sel<=0) {
        m_textTranslationX->SetValue(wxT(""));
        m_textTranslationY->SetValue(wxT(""));
        m_textTranslationZ->SetValue(wxT(""));
    }
}

void dlgMatrix::OnTranslateOrigin(wxCommandEvent& WXUNUSED(event)) {
    VECTOR v;
    double d;
    if (m_textTranslationX->GetValue().ToDouble(&d))
        v.x = d;
    else
        v.x = 0.0;
    if (m_textTranslationY->GetValue().ToDouble(&d))
        v.y = d;
    else
        v.y = 0.0;
    if (m_textTranslationZ->GetValue().ToDouble(&d))
        v.z = d;
    else
        v.z = 0.0;
    AddMatrix(wxString::Format(_("[ Set Origin to <%.4f,%.4f,%.4f>"), v.x, v.y, v.z), matrixGetTranslation(vectorInvert(v)));
    AddMatrix(wxString::Format(_("] Restore Origin from <%.4f,%.4f,%.4f>"), v.x, v.y, v.z), matrixGetTranslation(v));
    long sel = m_choiceTranslationPoint->GetSelection();
    if (sel<=0) {
        m_textTranslationX->SetValue(wxT(""));
        m_textTranslationY->SetValue(wxT(""));
        m_textTranslationZ->SetValue(wxT(""));
    }
}

////////////////////////////////////////////////////////////////////////
//
//  dlgMatrix, Rotation
//
////////////////////////////////////////////////////////////////////////

void dlgMatrix::OnRotate(wxCommandEvent& event) {
    double d = 0.0;
    m_textRotation->GetValue().ToDouble(&d);
    if (event.GetId() == XRCID("m_btRotationX"))
        AddMatrix(wxString::Format(_("Rotate by %.1f around the X-axis"), d), matrixGetRotationX(Deg2Rad(d)));
    else if (event.GetId() == XRCID("m_btRotationY"))
        AddMatrix(wxString::Format(_("Rotate by %.1f around the Y-axis"), d), matrixGetRotationY(Deg2Rad(d)));
    else if (event.GetId() == XRCID("m_btRotationZ"))
        AddMatrix(wxString::Format(_("Rotate by %.1f around the Z-axis"), d), matrixGetRotationZ(Deg2Rad(d)));
    else
        ::wxMessageBox(_("Internal Bug"), _("Error"), wxOK, this);
}

////////////////////////////////////////////////////////////////////////
//
//  dlgMatrix, Scale
//
////////////////////////////////////////////////////////////////////////

void dlgMatrix::OnScale(wxCommandEvent& WXUNUSED(event)) {
    VECTOR v;
    double d;
    if (m_textScaleX->GetValue().ToDouble(&d))
        v.x = d;
    else
        v.x = 0.0;
    if (m_textScaleY->GetValue().ToDouble(&d))
        v.y = d;
    else
        v.y = 0.0;
    if (m_textScaleZ->GetValue().ToDouble(&d))
        v.z = d;
    else
        v.z = 0.0;
    AddMatrix(wxString::Format(_("Scale by <%.4f,%.4f,%.4f>"), v.x, v.y, v.z), matrixGetScale(v));
    long sel = m_choiceTranslationPoint->GetSelection();
    if (sel<=0) {
        m_textScaleX->SetValue(wxT("1.0"));
        m_textScaleY->SetValue(wxT("1.0"));
        m_textScaleZ->SetValue(wxT("1.0"));
    }
}

void dlgMatrix::OnMirror(wxCommandEvent& event) {
    if (event.GetId() == XRCID("m_btMirrorX"))
        AddMatrix(_("Mirror along the X-axis"), matrixGetScale(-1.0, 1.0, 1.0));
    else if (event.GetId() == XRCID("m_btMirrorY"))
        AddMatrix(_("Mirror along the Y-axis"), matrixGetScale(1.0, -1.0, 1.0));
    else if (event.GetId() == XRCID("m_btMirrorZ"))
        AddMatrix(_("Mirror along the Z-axis"), matrixGetScale(1.0, 1.0, -1.0));
    else
        ::wxMessageBox(_("Internal Bug"), _("Error"), wxOK, this);
}

////////////////////////////////////////////////////////////////////////
//
//  dlgMatrix, Special
//
////////////////////////////////////////////////////////////////////////

void dlgMatrix::OnSpecialBone(wxCommandEvent& WXUNUSED(event)) {
    //AddMatrix(wxT("-"), matrixGetUnity());
// TODO (belgabor#1#): Implement
    ::wxMessageBox("Not yet implemented");
}

void dlgMatrix::OnSpecialDefault(wxCommandEvent& WXUNUSED(event)) {
    AddMatrix(_("Unity Matrix"), matrixGetUnity());
}

void dlgMatrix::OnSpecialLoad(wxCommandEvent& WXUNUSED(event)) {
    wxFileDialog *dialog = new wxFileDialog(
                               this,
                               _T("Open Scenery File (Load matrix)"),
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
            bool matrices_present = false;
            for (i = 0; i < texscn->models.size(); i++) {
                if ((texscn->models[i].transforms.size() != 0) || (texscn->models[i].effectpoints.size() != 0)) {
                    matrices_present = true;
                    break;
                }
            }
            if (!matrices_present) {
                wxMessageBox(_("The selected scenery file contains no matrices."), _("Warning"), wxOK | wxICON_WARNING, this);
                delete texscn;
                dialog->Destroy();
                return;
            }
            wxArrayString choices;
            std::vector<MATRIX> matchoices;
            for (long j = 0; j < texscn->models.size(); j++) {
                if (texscn->models[j].transforms.size()!=0) {
                    if ((texscn->models[j].transforms.size()>1) && (!matrixIsEqual(matrixMultiply(texscn->models[j].transforms), matrixGetUnity()))) {
                        choices.Add(texscn->models[j].name + _(" Model Transforms, combined"));
                        matchoices.push_back(matrixMultiply(texscn->models[j].transforms));
                    }
                    for (i = 0; i < texscn->models[j].transforms.size(); i++) {
                        choices.Add(texscn->models[j].name + _(" Model Transforms -> ")+texscn->models[j].transformnames[i]);
                        matchoices.push_back(texscn->models[j].transforms[i]);
                    }
                }
                for (cEffectPoint::iterator ep = texscn->models[j].effectpoints.begin(); ep != texscn->models[j].effectpoints.end(); ep++) {
                    if (ep->transforms.size()!=0) {
                        if ((ep->transforms.size()>1) && (!matrixIsEqual(matrixMultiply(ep->transforms), matrixGetUnity()))) {
                            choices.Add(texscn->models[j].name + wxT(", ") + ep->name + _(" Transforms, combined"));
                            matchoices.push_back(matrixMultiply(ep->transforms));
                        }
                        for (i = 0; i < ep->transforms.size(); i++) {
                            choices.Add(texscn->models[j].name + wxT(", ") + ep->name+_(" Transforms -> ")+ep->transformnames[i]);
                            matchoices.push_back(ep->transforms[i]);
                        }
                    }
                }
            }
            if (choices.size() > 0) {
                int choice = ::wxGetSingleChoiceIndex(dialog->GetPath(), _("Choose matrix to import"), choices, this);
                if (choice >= 0) {
                    AddMatrix(choices[choice], matchoices[choice]);
                }
            }
            delete texscn;
        }
    }
    dialog->Destroy();
}

////////////////////////////////////////////////////////////////////////
//
//  dlgMatrix, Load
//
////////////////////////////////////////////////////////////////////////

void dlgMatrix::OnLoad(wxCommandEvent& WXUNUSED(event)) {
    if (m_matrices.size()) {
        if (::wxMessageBox(_("This will replace your current matrix stack.\nDo you want to continue?"), _("Question"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION, this)==wxNO)
            return;
    }
    wxFileDialog *dialog = new wxFileDialog(
                               this,
                               _T("Open Scenery File (Load matrix stack)"),
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
            bool matrices_present = false;
            for (i = 0; i < texscn->models.size(); i++) {
                if ((texscn->models[i].transforms.size() != 0) || (texscn->models[i].effectpoints.size() != 0)) {
                    matrices_present = true;
                    break;
                }
            }
            if (!matrices_present) {
                ::wxMessageBox(_("The selected scenery file contains no matrices."), _("Warning"), wxOK | wxICON_WARNING, this);
                delete texscn;
                dialog->Destroy();
                return;
            }
            wxArrayString choices;
            std::vector< std::vector<MATRIX> > matchoices;
            std::vector< wxArrayString > matnames;
            for (long j = 0; j < texscn->models.size(); j++) {
                if (texscn->models[j].transforms.size()!=0) {
                    choices.push_back(texscn->models[j].name + _("Model Transforms"));
                    matchoices.push_back(texscn->models[j].transforms);
                    matnames.push_back(texscn->models[j].transformnames);
                }
                for (cEffectPoint::iterator ep = texscn->models[j].effectpoints.begin(); ep != texscn->models[j].effectpoints.end(); ep++) {
                    if (ep->transforms.size()!=0) {
                        choices.push_back(texscn->models[j].name + wxT(", ") + ep->name);
                        matchoices.push_back(ep->transforms);
                        matnames.push_back(ep->transformnames);
                    }
                }
            }
            if (choices.size() > 0) {
                int choice = ::wxGetSingleChoiceIndex(dialog->GetPath(), _("Choose matrix stack to import"), choices, this);
                if (choice >= 0) {
                    m_matrixnames.clear();
                    m_matrices.clear();

                    m_matrixnames = matnames[choice];
                    m_matrices = matchoices[choice];

                    m_htlbMatrix->UpdateContents();
                    m_htlbMatrix->SetSelection(0);
                    DisplayMatrix();
                }
            }
            delete texscn;
        }
    }
    dialog->Destroy();
}
