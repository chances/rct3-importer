///////////////////////////////////////////////////////////////////////////////
//
// Matrix Load Dialog
// Copyright (C) 2007 Tobias Minch, Jonathan Wilson
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

#include "matrix.h"

#include "wxdlgMatrixLoad.h"

class wxMatrixData: public wxTreeItemData {
public:
    wxMatrixData(): wxTreeItemData(){};

    virtual MATRIX GetMatrix() const = 0;
    virtual wxString GetName() const = 0;
    virtual std::vector<MATRIX> GetStack() const = 0;
    virtual wxArrayString GetNames() const = 0;
};

class wxStackData: public wxMatrixData {
public:
    wxString name;
	std::vector<MATRIX> transforms;
	wxArrayString transformnames;

    wxStackData(const wxString& n, const std::vector<MATRIX>& ts, const wxArrayString& tns): wxMatrixData() {
        name = n;
        transforms = ts;
        transformnames = tns;
    }
    virtual MATRIX GetMatrix() const {
        return matrixMultiply(transforms);
    }
    virtual wxString GetName() const {
        return wxString::Format(_("Multiplied Stack (%s)"), name.c_str());
    }
    virtual std::vector<MATRIX> GetStack() const {
        return transforms;
    }
    virtual wxArrayString GetNames() const {
        return transformnames;
    }
};

class wxSingleData: public wxMatrixData {
public:
    wxString name;
	MATRIX transform;

    wxSingleData(const wxString& n, const MATRIX& ts): wxMatrixData() {
        name = n;
        transform = ts;
    }
    virtual MATRIX GetMatrix() const {
        return transform;
    }
    virtual wxString GetName() const {
        return name;
    }
    virtual std::vector<MATRIX> GetStack() const {
        std::vector<MATRIX> temp;
        temp.push_back(transform);
        return temp;
    }
    virtual wxArrayString GetNames() const {
        wxArrayString temp;
        temp.Add(name);
        return temp;
    }
};

BEGIN_EVENT_TABLE(dlgLoadMatrix,wxDialog)
EVT_TREE_SEL_CHANGED(XRCID("m_treeMatrix"), dlgLoadMatrix::OnTree)
END_EVENT_TABLE()

dlgLoadMatrix::dlgLoadMatrix(cSCNFile* file, wxWindow *parent): m_file(file), m_md(NULL) {
    InitWidgetsFromXRC((wxWindow *)parent);

    m_OK->Enable(false);
    for (int i=0; i<4; i++)
        for (int j=0; j<4; j++)
            m_Matrix[i][j]->SetLabel(wxT("---"));
    m_choiceMatrix->SetSelection(0);
    m_choiceMatrix->Enable(false);

    wxTreeItemId root = m_treeMatrix->AddRoot(_("Scenery Object"));
    if (m_file->models.size()) {
        wxTreeItemId models = m_treeMatrix->AppendItem(root, _("Models"));
        m_treeMatrix->SetItemBold(models);
        for (cModel::iterator m = m_file->models.begin(); m != m_file->models.end(); m++) {
            wxTreeItemId model = m_treeMatrix->AppendItem(models, m->name);
            m_treeMatrix->SetItemBold(model);
            if (m->transforms.size()) {
                wxTreeItemId transforms = m_treeMatrix->AppendItem(model, _("Model Transformation Matrix"), -1, -1,
                    new wxStackData(m->name+_(", Transformation Matrix"), m->transforms, m->transformnames));
                for (int i = 0; i < m->transforms.size(); i++) {
                    m_treeMatrix->AppendItem(transforms, m->transformnames[i], -1, -1, new wxSingleData(m->transformnames[i], m->transforms[i]));
                }
            }
            if (m->effectpoints.size()) {
                wxTreeItemId effectpoints = m_treeMatrix->AppendItem(model, _("Effect Points"));
                m_treeMatrix->SetItemBold(effectpoints);
                for (cEffectPoint::iterator e = m->effectpoints.begin(); e != m->effectpoints.end(); e++) {
                    wxTreeItemId effectpoint = m_treeMatrix->AppendItem(effectpoints, e->name, -1, -1,
                        new wxStackData(e->name, e->transforms, e->transformnames));
                    for (int i = 0; i < e->transforms.size(); i++) {
                        m_treeMatrix->AppendItem(effectpoint, e->transformnames[i], -1, -1, new wxSingleData(e->transformnames[i], e->transforms[i]));
                    }
                }
                m_treeMatrix->Expand(effectpoints);
            }
            m_treeMatrix->Expand(model);
        }
        m_treeMatrix->Expand(models);
    }
    if (m_file->animatedmodels.size()) {
        wxTreeItemId models = m_treeMatrix->AppendItem(root, _("Animated Models"));
        for (cAnimatedModel::iterator m = m_file->animatedmodels.begin(); m != m_file->animatedmodels.end(); m++) {
            wxTreeItemId model = m_treeMatrix->AppendItem(models, m->name);
            m_treeMatrix->SetItemBold(models);
            if (m->transforms.size()) {
                wxTreeItemId transforms = m_treeMatrix->AppendItem(model, _("Model Transformation Matrix"), -1, -1,
                    new wxStackData(m->name+_(", Transformation Matrix"), m->transforms, m->transformnames));
                for (int i = 0; i < m->transforms.size(); i++) {
                    m_treeMatrix->AppendItem(transforms, m->transformnames[i], -1, -1, new wxSingleData(m->transformnames[i], m->transforms[i]));
                }
            }
            if (m->modelbones.size()) {
                wxTreeItemId bones = m_treeMatrix->AppendItem(model, _("Bones"));
                m_treeMatrix->SetItemBold(bones);
                for (cModelBone::iterator e = m->modelbones.begin(); e != m->modelbones.end(); e++) {
                    wxTreeItemId bone = m_treeMatrix->AppendItem(bones, e->name);
                    m_treeMatrix->SetItemBold(bone);
                    wxTreeItemId pos1 = m_treeMatrix->AppendItem(bone, _("Position 1"), -1, -1,
                        new wxStackData(e->name+_(", Position 1"), e->positions1, e->position1names));
                    for (int i = 0; i < e->positions1.size(); i++) {
                        m_treeMatrix->AppendItem(pos1, e->position1names[i], -1, -1, new wxSingleData(e->position1names[i], e->positions1[i]));
                    }
                    if (e->usepos2) {
                        wxTreeItemId pos2 = m_treeMatrix->AppendItem(bone, _("Position 2"), -1, -1,
                            new wxStackData(e->name+_(", Position 2"), e->positions2, e->position2names));
                        for (int i = 0; i < e->positions2.size(); i++) {
                            m_treeMatrix->AppendItem(pos2, e->position2names[i], -1, -1, new wxSingleData(e->position2names[i], e->positions2[i]));
                        }
                    }
                    m_treeMatrix->Expand(bone);
                }
                m_treeMatrix->Expand(bones);
            }
            m_treeMatrix->Expand(models);
        }
    }

}

void dlgLoadMatrix::OnTree(wxTreeEvent& event) {
    wxTreeItemId id = event.GetItem();
    wxMatrixData* data = dynamic_cast<wxMatrixData*>(m_treeMatrix->GetItemData(id));
    m_md = data;
    if (dynamic_cast<wxStackData*>(data)) {
        if (!m_choiceMatrix->IsEnabled())
            m_choiceMatrix->SetSelection(1);
        m_choiceMatrix->Enable(true);
    } else {
        m_choiceMatrix->SetSelection(0);
        m_choiceMatrix->Enable(false);
    }
    if (data) {
        m_OK->Enable(true);
        MATRIX m = data->GetMatrix();
        for (int i=0; i<4; i++)
            for (int j=0; j<4; j++)
                m_Matrix[i][j]->SetLabel(wxString::Format("%.*f", 4, m.m[i][j]));
    } else {
        m_OK->Enable(false);
        for (int i=0; i<4; i++)
            for (int j=0; j<4; j++)
                m_Matrix[i][j]->SetLabel(wxT("---"));
    }
}

void dlgLoadMatrix::GetMatrix(wxString& name, MATRIX& matrix) {
    if (m_md) {
        name = m_md->GetName();
        matrix = m_md->GetMatrix();
    }
}

void dlgLoadMatrix::GetStack(wxArrayString& names, std::vector<MATRIX>& matrices) {
    if (m_md) {
        if (m_choiceMatrix->GetSelection()==0) {
            names.clear();
            matrices.clear();
            names.Add(m_md->GetName());
            matrices.push_back(m_md->GetMatrix());
        } else {
            names = m_md->GetNames();
            matrices = m_md->GetStack();
        }
    }
}
