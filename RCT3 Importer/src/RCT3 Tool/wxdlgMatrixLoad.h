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
// Note: ressource in xrc_matrix
//
///////////////////////////////////////////////////////////////////////////////

#ifndef WXDLGMATRIXLOAD_H_INCLUDED
#define WXDLGMATRIXLOAD_H_INCLUDED

#include "wx_pch.h"

#include <wx/xrc/xmlres.h>
#include <wx/treectrl.h>

#include "SCNFile.h"

class wxMatrixData;
class dlgLoadMatrix : public wxDialog {
protected:
    wxTreeCtrl* m_treeMatrix;
    wxStaticText* m_Matrix[4][4];
    wxChoice* m_choiceMatrix;
    wxButton* m_OK;
    wxButton* m_CANCEL;

    cSCNFile* m_file;
    wxMatrixData* m_md;


    void OnTree(wxTreeEvent& event);
    DECLARE_EVENT_TABLE()
private:
    void InitWidgetsFromXRC(wxWindow *parent){
        wxXmlResource::Get()->LoadObject(this,parent,_T("dlgLoadMatrix"), _T("wxDialog"));
        m_treeMatrix = XRCCTRL(*this,"m_treeMatrix",wxTreeCtrl);
        m_Matrix[0][0] = XRCCTRL(*this,"m_11",wxStaticText);
        m_Matrix[0][1] = XRCCTRL(*this,"m_12",wxStaticText);
        m_Matrix[0][2] = XRCCTRL(*this,"m_13",wxStaticText);
        m_Matrix[0][3] = XRCCTRL(*this,"m_14",wxStaticText);
        m_Matrix[1][0] = XRCCTRL(*this,"m_21",wxStaticText);
        m_Matrix[1][1] = XRCCTRL(*this,"m_22",wxStaticText);
        m_Matrix[1][2] = XRCCTRL(*this,"m_23",wxStaticText);
        m_Matrix[1][3] = XRCCTRL(*this,"m_24",wxStaticText);
        m_Matrix[2][0] = XRCCTRL(*this,"m_31",wxStaticText);
        m_Matrix[2][1] = XRCCTRL(*this,"m_32",wxStaticText);
        m_Matrix[2][2] = XRCCTRL(*this,"m_33",wxStaticText);
        m_Matrix[2][3] = XRCCTRL(*this,"m_34",wxStaticText);
        m_Matrix[3][0] = XRCCTRL(*this,"m_41",wxStaticText);
        m_Matrix[3][1] = XRCCTRL(*this,"m_42",wxStaticText);
        m_Matrix[3][2] = XRCCTRL(*this,"m_43",wxStaticText);
        m_Matrix[3][3] = XRCCTRL(*this,"m_44",wxStaticText);
        m_choiceMatrix = XRCCTRL(*this,"m_choiceMatrix",wxChoice);
        m_OK = XRCCTRL(*this,"wxID_OK",wxButton);
        m_CANCEL = XRCCTRL(*this,"wxID_CANCEL",wxButton);
    }
public:
    dlgLoadMatrix(cSCNFile* file, wxWindow *parent = NULL);

    bool DoSingle() const {
        return m_choiceMatrix->GetSelection()==0;
    }

    void GetMatrix(wxString& name, D3DMATRIX& matrix);
    void GetStack(wxArrayString& names, std::vector<D3DMATRIX>& matrices);
};

#endif
