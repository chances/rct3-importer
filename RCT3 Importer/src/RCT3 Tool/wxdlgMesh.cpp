///////////////////////////////////////////////////////////////////////////////
//
// Mesh Settings Dialog
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

#include "wxdlgMesh.h"

#include <wx/valgen.h>

#include "SCNFile.h"

#include "wxapp.h"
#include "wxdlgCreateScenery.h"

////////////////////////////////////////////////////////////////////////
//
//  dlgMesh
//
////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(dlgMesh,wxDialog)
EVT_CHOICE(XRCID("m_choiceTextureFlags"), dlgMesh::OnChange)
EVT_CHECKBOX(XRCID("m_checkDontImport"), dlgMesh::OnChange)
EVT_BUTTON(XRCID("m_btLoad"), dlgMesh::OnLoad)
EVT_RIGHT_DCLICK(dlgMesh::OnRDbl)
END_EVENT_TABLE()

dlgMesh::dlgMesh(wxWindow *parent) {
    InitWidgetsFromXRC((wxWindow *)parent);

    m_ms.unknown = 3;
    m_unknown = 1;
    m_applyall = false;
    m_rareadded = false;

    if (parent) {
        dlgCreateScenery *dlg = dynamic_cast<dlgCreateScenery *> (parent->GetParent());
        if (dlg) {
            for(cFlexiTextureIterator it = dlg->m_SCN.flexitextures.begin(); it != dlg->m_SCN.flexitextures.end(); it++) {
                m_cbTextureName->Append(it->Name);
            }
        }
    }

    for(std::list<wxString>::iterator it = ::wxGetApp().g_texturecache.begin(); it != ::wxGetApp().g_texturecache.end(); it++) {
        m_cbTextureName->Append(*it);
    }

    m_checkDontImport->SetValidator(wxGenericValidator(&m_ms.disabled));
    m_cbTextureName->SetValidator(wxGenericValidator(&m_ms.FTX));
    m_choiceTextureStyle->SetValidator(wxGenericValidator(&m_ms.TXS));
    m_choiceTextureFlags->SetValidator(wxGenericValidator(&m_flags));
    m_choiceShowTexture->SetValidator(wxGenericValidator(&m_place));
    m_choiceUnknown->SetValidator(wxGenericValidator(&m_unknown));
    m_checkApplyAll->SetValidator(wxGenericValidator(&m_applyall));

    m_btOk->SetId(wxID_OK);
    m_btCancel->SetId(wxID_CANCEL);
}

void dlgMesh::RefreshAll() {
    TransferDataFromWindow();
    m_choiceTextureFlags->Enable(!m_ms.disabled);
    m_choiceShowTexture->Enable(!m_ms.disabled);
    m_choiceUnknown->Enable(!m_ms.disabled);
    if (m_ms.disabled) {
        m_cbTextureName->Enable(false);
        m_choiceTextureStyle->Enable(false);
        return;
    }
    bool do_enable = true;
    switch (m_flags) {
    case 1:
        m_choiceTextureStyle->SetStringSelection(wxT("SIOpaqueUnlit"));
        break;
    case 2:
        do_enable = false;
        m_cbTextureName->SetValue(wxT("useterraintexture"));
        m_choiceTextureStyle->SetStringSelection(wxT("SIOpaque"));
        break;
    case 3:
        do_enable = false;
        m_cbTextureName->SetValue(wxT("useclifftexture"));
        m_choiceTextureStyle->SetStringSelection(wxT("SIOpaque"));
        break;
    case 4:
        do_enable = false;
        m_cbTextureName->SetValue(wxT("siwater"));
        m_choiceTextureStyle->SetStringSelection(wxT("SIWater"));
        break;
    case 5:
        do_enable = false;
        m_cbTextureName->SetValue(wxT("watermask"));
        m_choiceTextureStyle->SetStringSelection(wxT("SIFillZ"));
        break;
    case 6:
        do_enable = false;
        m_cbTextureName->SetValue(wxT("UseAdTexture"));
        m_choiceTextureStyle->SetStringSelection(wxT("SIOpaque"));
        break;
    }
    m_cbTextureName->Enable(do_enable);
    m_choiceTextureStyle->Enable(do_enable);
}

void dlgMesh::SetMeshStruct(const cMeshStruct& ms) {
    m_ms = ms;

    m_place = ms.place;

    if (ms.flags == 0) {
        m_flags = 0;
    }
    if (ms.flags == 12) {
        m_flags = 1;
    }
    if (ms.flags == 12288) {
        m_flags = 2;
    }
    if (ms.flags == 20480) {
        m_flags = 3;
    }
    if (ms.flags == 32788) {
        m_flags = 6;
    }
    if (ms.FTX != wxT("")) {
        if (ms.FTX.CmpNoCase("siwater") == 0) {
            m_flags = 4;
        }
        if (ms.FTX.CmpNoCase("watermask") == 0) {
            m_flags = 5;
        }
    }
    if (ms.unknown == 1) {
        m_unknown = 0;
    } else {
        m_unknown = 1;
    }

    TransferDataToWindow();
    RefreshAll();
}

cMeshStruct dlgMesh::GetMeshStruct() {
    cMeshStruct ret = m_ms;

    ret.place = m_place;

    if (m_unknown == 0)
        ret.unknown = 1;
    else
        ret.unknown = 3;
    switch (m_flags) {
    case 1:
        ret.flags = 12;
        break;
    case 2:
        ret.flags = 12288;
        break;
    case 3:
        ret.flags = 20480;
        break;
    case 6:
        ret.flags = 32788;
        break;
    default:
        ret.flags = 0;
    }
    return ret;
}

void dlgMesh::OnLoad(wxCommandEvent& WXUNUSED(event)) {
    wxFileDialog *dialog = new wxFileDialog(
                                     this,
                                     _T("Open Scenery File (Load mesh settings)"),
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
            bool mesh_present = false;
            for (i = 0; i < texscn->models.size(); i++) {
                if (texscn->models[i].meshstructs.size() != 0) {
                    mesh_present = true;
                    break;
                }
            }
            if (!mesh_present) {
                ::wxMessageBox(_("The selected scenery file contains no meshes."), _("Warning"), wxOK | wxICON_WARNING, this);
                delete texscn;
                dialog->Destroy();
                return;
            }
            wxArrayString choices;
            std::vector<cMeshStruct> meshchoices;
            for (i = 0; i < texscn->models.size(); i++) {
                for (cMeshStructIterator ms = texscn->models[i].meshstructs.begin(); ms != texscn->models[i].meshstructs.end(); ms++) {
                    choices.Add(texscn->models[i].name + wxT(", ") + ms->Name);
                    meshchoices.push_back(*ms);
                }
            }
            if (choices.size() > 0) {
                int choice = ::wxGetSingleChoiceIndex(dialog->GetPath(), _("Choose mesh settings to import"), choices, this);
                if (choice >= 0) {
                    cMeshStruct c = m_ms;
                    c.CopySettingsFrom(meshchoices[choice]);
                    SetMeshStruct(c);
                    TransferDataToWindow();
                    RefreshAll();
                }
            }
            delete texscn;
        }
    }
    dialog->Destroy();
}

void dlgMesh::OnRDbl(wxMouseEvent& WXUNUSED(event)) {
    if (m_rareadded)
        return;
    m_rareadded = true;
    ::wxMessageBox(_("Experimental texture styles have been added to the list.\nBe careful with those, they are mostly untested. Several so far crash the game."), _("Warning"), wxOK | wxICON_INFORMATION, this);
    m_choiceTextureStyle->Append(wxT("Background"));
    m_choiceTextureStyle->Append(wxT("CloudsUVScrollB"));
    m_choiceTextureStyle->Append(wxT("FullscreenFadeToBlack"));
    m_choiceTextureStyle->Append(wxT("FullscreenModulateX2NoBitmap"));
    m_choiceTextureStyle->Append(wxT("LapWater"));
    m_choiceTextureStyle->Append(wxT("LightVolumeFront"));
    m_choiceTextureStyle->Append(wxT("LightVolumeBack"));
    m_choiceTextureStyle->Append(wxT("LightVolumeFrontAlways"));
    m_choiceTextureStyle->Append(wxT("LightVolumeBackAlways"));
    m_choiceTextureStyle->Append(wxT("LightVolumeFront"));
    m_choiceTextureStyle->Append(wxT("ParticleAdditive"));
    m_choiceTextureStyle->Append(wxT("ParticleAdditiveBias"));
    m_choiceTextureStyle->Append(wxT("SimpleWater"));
    m_choiceTextureStyle->Append(wxT("SimpleAlphaTile"));
    m_choiceTextureStyle->Append(wxT("StarStyle"));
    m_choiceTextureStyle->Append(wxT("TerrainBlending"));
    m_choiceTextureStyle->Append(wxT("TerrainCliff"));
    m_choiceTextureStyle->Append(wxT("TerrainLightFalloff"));
    m_choiceTextureStyle->Append(wxT("WaterReflectionAnimatedBumpMap"));
}
