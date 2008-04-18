///////////////////////////////////////////////////////////////////////////////
//
// Importing data from files Dialog
// Copyright (C) 2008 Tobias Minch
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
// Note: ressource in misc
//
///////////////////////////////////////////////////////////////////////////////


#include "wxdlgLoadFromFile.h"

#include <exception>

#include "wxapp.h"

#include "fileselectorcombowrap.h"
#include "pretty.h"
#include "RCT3Exception.h"

#include "colhtmllbox.h"

using namespace std;

dlgLoadFromFile::dlgLoadFromFile(wxWindow* parent, cRCT3Xml& client, int type, bool* updated, wxColourHtmlListBox* update):
    rcdlgLoadFromFile(parent), m_client(client), m_type(type), m_updated(updated), m_update(update) {

    wxFileDialog *filedlg = new wxFileDialog(
                               this,
                               _("Open Source File"),
                               wxEmptyString,
                               wxEmptyString,
                                cSCNFile::GetSupportedFilesFilter()+_("|All Files (*.*)|*.*"),
                               wxFD_FILE_MUST_EXIST|wxFD_CHANGE_DIR,
                               wxDefaultPosition,
                               wxSize(600,400)
                           );
    m_comboFile->Assign(filedlg, &::wxGetApp().g_workdir);
    m_comboFile->GetTextCtrl()->SetEditable(false);

    wxString name;
    switch (m_type) {
        case Type_AnimatedModels:
            name = cAnimatedModel::getPlural();
            break;
        case Type_Animations:
            name = cAnimation::getPlural();
            break;
        case Type_Models:
            name = cModel::getPlural();
            break;
        case Type_Splines:
            name = cImpSpline::getPlural();
            break;
        case Type_Textures:
            name = cFlexiTexture::getPlural();
            break;
        default:
            throw RCT3Exception(wxT("Internal error: unsupported type"));
    }
    SetTitle(wxString::Format(_("Load %s"), name.c_str()));
}

/** @brief ShowModal
  *
  * @todo: document this function
  */
int dlgLoadFromFile::ShowModal() {
    return wxDialog::ShowModal();
}

/** @brief OnFileSelect
  *
  * @todo: document this function
  */
void dlgLoadFromFile::OnFileSelect(wxCommandEvent& event) {
    try {
        m_checklbLoad->Clear();
        m_selections.clear();
        m_loaded.Load(event.GetString());
        switch (m_type) {
            case Type_AnimatedModels: {
                    FillSingular<cAnimatedModel>();
                }
                break;
            case Type_Animations: {
                    FillSingular<cAnimation>();
                }
                break;
            case Type_Models: {
                    FillSingular<cModel>();
                }
                break;
            case Type_Splines: {
                    FillSingular<cImpSpline>();
                }
                break;
            case Type_Textures: {
                    FillSingular<cFlexiTexture>();
                }
                break;
            default:
                throw RCT3Exception(wxT("Internal error: unsupported type"));
        }
    } catch (exception& e) {
        wxLogError(e.what());
    }
    UpdateControlState();
}

/** @brief OnLoadClose
  *
  * @todo: document this function
  */
void dlgLoadFromFile::OnLoadClose(wxCommandEvent& event) {
    OnLoad(event);
    OnClose(event);
}

/** @brief OnLoad
  *
  * @todo: document this function
  */
void dlgLoadFromFile::OnLoad(wxCommandEvent& event) {
    int count = 0;
    try {
        for(int i = 0; i < m_checklbLoad->GetCount(); ++i) {
            if (m_checklbLoad->IsChecked(i)) {
                ++count;
                switch (m_type) {
                    case Type_AnimatedModels: {
                            LoadSingular<cAnimatedModel>(i);
                        }
                        break;
                    case Type_Animations: {
                            LoadSingular<cAnimation>(i);
                        }
                        break;
                    case Type_Models: {
                            LoadSingular<cModel>(i);
                        }
                        break;
                    case Type_Splines: {
                            LoadSingular<cImpSpline>(i);
                        }
                        break;
                    case Type_Textures: {
                            LoadSingular<cFlexiTexture>(i);
                        }
                        break;
                    default:
                        throw RCT3Exception(wxT("Internal error: unsupported type"));
                }
            }
        }
    } catch (exception& e) {
        ::wxMessageBox(e.what());
    }
    if (count) {
        if (m_update)
            m_update->UpdateContents();
        if (m_updated)
            *m_updated = true;
    }
    ClearMarks();
}

/** @brief OnInitDialog
  *
  * @todo: document this function
  */
void dlgLoadFromFile::OnInitDialog(wxInitDialogEvent& event) {
    //m_comboFile->ShowPopup();
    m_comboFile->OnButtonClick();
}

/** @brief OnClose
  *
  * @todo: document this function
  */
void dlgLoadFromFile::OnClose(wxCommandEvent& event) {
    EndDialog(0);
}

/** @brief OnUpdateControlState
  *
  * @todo: document this function
  */
void dlgLoadFromFile::OnUpdateControlState(wxCommandEvent& event) {
    UpdateControlState();
}

/** @brief UpdateControlState
  *
  * @todo: document this function
  */
void dlgLoadFromFile::UpdateControlState() {
    bool en = false;
    for(int i = 0; i < m_checklbLoad->GetCount(); ++i) {
        if (m_checklbLoad->IsChecked(i)) {
            en = true;
            break;
        }
    }
    m_btLoad->Enable(en);
    m_btLoadClose->Enable(en);
}

/** @brief OnListRightUp
  *
  * @todo: document this function
  */
void dlgLoadFromFile::OnListRightUp(wxMouseEvent& event) {
    if (m_checklbLoad->GetCount()) {
        ClearMarks();
    } else {
        m_comboFile->OnButtonClick();
    }
}

/** @brief OnListDblClk
  *
  * @todo: document this function
  */
void dlgLoadFromFile::OnListDblClk(wxCommandEvent& event) {
    for(int i = 0; i < m_checklbLoad->GetCount(); ++i)
        m_checklbLoad->Check(i, true);
    UpdateControlState();
}

/** @brief ClearMarks
  *
  * @todo: document this function
  */
void dlgLoadFromFile::ClearMarks() {
    for(int i = 0; i < m_checklbLoad->GetCount(); ++i)
        m_checklbLoad->Check(i, false);
    UpdateControlState();
}



