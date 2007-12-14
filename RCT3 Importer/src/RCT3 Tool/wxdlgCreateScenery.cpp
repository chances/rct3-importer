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
#include "confhelp.h"
#include "htmlentities.h"
#include "lib3Dconfig.h"
#include "logext.h"
#include "matrix.h"
#include "OVLException.h"
#include "popupmessagewin.h"
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
#include "wxdlgTexture.h"

const long idToolBar = ::wxNewId();
const long tbLoadCache = ::wxNewId();
const long tbClearCache = ::wxNewId();

////////////////////////////////////////////////////////////////////////
//
//  wxSceneryListBox
//
////////////////////////////////////////////////////////////////////////

class wxSceneryListBox : public wxColourHtmlListBox {
public:
    wxSceneryListBox(wxWindow *parent, cSCNFile *contents):
        wxColourHtmlListBox(parent, wxID_ANY, wxDefaultPosition, wxSize(-1, -1), wxSUNKEN_BORDER) {
            m_contents = contents;
    }
    virtual void UpdateContents() = 0;
protected:
    cSCNFile* m_contents;
};

////////////////////////////////////////////////////////////////////////
//
//  wxTextureListBox
//
////////////////////////////////////////////////////////////////////////

class wxTextureListBox : public wxSceneryListBox {
public:
    wxTextureListBox(wxWindow *parent, cSCNFile *contents): wxSceneryListBox(parent, contents) {
        UpdateContents();
        if (m_contents) {
            SetSelection(m_contents->flexitextures.size()?0:wxNOT_FOUND);
        } else {
            SetSelection(wxNOT_FOUND);
        }
    }
    virtual void UpdateContents() {
        SetItemCount(m_contents->flexitextures.size() + ::wxGetApp().g_texturecache.size());
        if (GetSelection() >= m_contents->flexitextures.size())
            SetSelection(wxNOT_FOUND);
        RefreshAll();
    }
protected:
    virtual wxString OnGetItem(size_t n) const {
        if (n < m_contents->flexitextures.size()) {
            wxString addon = wxT("");
            for (cFlexiTextureFrame::iterator it = m_contents->flexitextures[n].Frames.begin(); it != m_contents->flexitextures[n].Frames.end(); it++) {
                if (it->alphasource() != CFTF_ALPHA_NONE) {
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
                    +HTML_INSET_START+addon+HTML_INSET_END
                    +wxT("</font>");
            else
                return wxT("<font size='2'>")+wxEncodeHtmlEntities(m_contents->flexitextures[n].Name)+wxT("</font>");
        } else if (n < (m_contents->flexitextures.size() + ::wxGetApp().g_texturecache.size())) {
            wxString s = wxT("");
            int i = n-m_contents->flexitextures.size();
            for(std::list<wxString>::iterator it = ::wxGetApp().g_texturecache.begin(); it != ::wxGetApp().g_texturecache.end(); it++) {
                if (!i) {
                    s = *it;
                    break;
                }
                i--;
            }
            return wxT("<font size='2' color='#808080'>")+wxEncodeHtmlEntities(s)+wxT("</font>");
        } else {
                return wxT("Internal Error");
        }
    }
};


////////////////////////////////////////////////////////////////////////
//
//  wxReferenceListBox
//
////////////////////////////////////////////////////////////////////////

class wxReferenceListBox : public wxSceneryListBox {
public:
    wxReferenceListBox(wxWindow *parent, cSCNFile *contents): wxSceneryListBox(parent, contents) {
        UpdateContents();
        if (m_contents) {
            SetSelection(m_contents->references.size()?0:wxNOT_FOUND);
        } else {
            SetSelection(wxNOT_FOUND);
        }
    }
    virtual void UpdateContents() {
        SetItemCount(m_contents->references.size());
        if (GetSelection() >= m_contents->references.size())
            SetSelection(wxNOT_FOUND);
        RefreshAll();
    }
protected:
    virtual wxString OnGetItem(size_t n) const {
        return wxT("<font size='2'>")+wxEncodeHtmlEntities(m_contents->references[n])+wxT("</font>");
    }
};


////////////////////////////////////////////////////////////////////////
//
//  wxModelListBox
//
////////////////////////////////////////////////////////////////////////

class wxModelListBox : public wxSceneryListBox {
public:
    wxModelListBox(wxWindow *parent, cSCNFile *contents): wxSceneryListBox(parent, contents) {
        UpdateContents();
        if (m_contents) {
            SetSelection(m_contents->models.size()?0:wxNOT_FOUND);
        } else {
            SetSelection(wxNOT_FOUND);
        }
    }
    virtual void UpdateContents() {
        SetItemCount(m_contents->models.size());
        if (GetSelection() >= m_contents->models.size())
            SetSelection(wxNOT_FOUND);
        RefreshAll();
    }

protected:
    virtual wxString OnGetItem(size_t n) const {
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
        for(cMeshStruct::iterator it = m_contents->models[n].meshstructs.begin(); it != m_contents->models[n].meshstructs.end(); it++) {
            if (!it->disabled) {
                mesh_enabled++;
                mesh_faces += it->faces;
            }
        }
        ret += HTML_INSET_START;
        ret += wxString::Format(_("%ld/%d groups"), mesh_enabled, m_contents->models[n].meshstructs.size());
        if (mesh_faces >= 1200)
            ret += wxString::Format(wxT(", <font color='#FF0000'>%ld faces</font>"), mesh_faces);
        else if (mesh_faces >= 600)
            ret += wxString::Format(wxT(", <font color='#DDDD00'>%ld faces</font>"), mesh_faces);
        else if (mesh_faces >= 300)
            ret += wxString::Format(wxT(", %ld faces"), mesh_faces);
        else if (mesh_faces)
            ret += wxString::Format(wxT(", <font color='#66CC00'>%ld faces</font>"), mesh_faces);

        int ep = m_contents->models[n].effectpoints.size();
        if (ep == 1)
            ret += wxT("<br>1 effect point");
        else if (ep)
            ret += wxString::Format(wxT("<br>%d effect points"), m_contents->models[n].effectpoints.size());
        if (mesh_faces > 3000)
            ret += wxT("</td><td width=16 valign=top><img width=16 height=16 align=center src='memory:XRC_resource/xrc_bitmap_crystal.cpp$icons_no_16x16.png'>");
        return ret + HTML_INSET_END + wxT("</font>");
    }
};


////////////////////////////////////////////////////////////////////////
//
//  wxAnimatedModelListBox
//
////////////////////////////////////////////////////////////////////////

class wxAnimatedModelListBox : public wxSceneryListBox {
public:
    wxAnimatedModelListBox(wxWindow *parent, cSCNFile *contents): wxSceneryListBox(parent, contents) {
        UpdateContents();
        if (m_contents) {
            SetSelection(m_contents->animatedmodels.size()?0:wxNOT_FOUND);
        } else {
            SetSelection(wxNOT_FOUND);
        }
    }
    virtual void UpdateContents() {
        SetItemCount(m_contents->animatedmodels.size());
        if (GetSelection() >= m_contents->animatedmodels.size())
            SetSelection(wxNOT_FOUND);
        RefreshAll();
    }
protected:
    virtual wxString OnGetItem(size_t n) const {
        wxString ret = wxT("<font size='2'>");
        if (m_contents->animatedmodels[n].fatal_error) {
            ret += wxT("<font color='#FF3C3C'>");
        } else if (m_contents->animatedmodels[n].error.size()) {
            ret += wxT("<font color='#FFCC00'>");
        }
        ret += wxEncodeHtmlEntities(m_contents->animatedmodels[n].name);
        if ((m_contents->animatedmodels[n].fatal_error) || (m_contents->animatedmodels[n].error.size())) {
            ret += wxT("</font>");
        }
        unsigned long mesh_enabled = 0;
        unsigned long mesh_faces = 0;
        for(cMeshStruct::iterator it = m_contents->animatedmodels[n].meshstructs.begin(); it != m_contents->animatedmodels[n].meshstructs.end(); it++) {
            if (!it->disabled) {
                mesh_enabled++;
                mesh_faces += it->faces;
            }
        }

        ret += HTML_INSET_START;
        ret += wxString::Format(_("%ld/%d groups"), mesh_enabled, m_contents->animatedmodels[n].meshstructs.size());
        if (mesh_faces >= 1200)
            ret += wxString::Format(wxT(", <font color='#FF0000'>%ld faces</font>"), mesh_faces);
        else if (mesh_faces >= 600)
            ret += wxString::Format(wxT(", <font color='#DDDD00'>%ld faces</font>"), mesh_faces);
        else if (mesh_faces >= 300)
            ret += wxString::Format(wxT(", %ld faces"), mesh_faces);
        else if (mesh_faces)
            ret += wxString::Format(wxT(", <font color='#66CC00'>%ld faces</font>"), mesh_faces);

        int bn = m_contents->animatedmodels[n].modelbones.size();
        if (bn == 1)
            ret += wxT("<br>1 bone");
        else if (bn)
            ret += wxString::Format(wxT(", %d bones"), m_contents->animatedmodels[n].modelbones.size());
        if (mesh_faces > 3000)
            ret += wxT("</td><td width=16 valign=top><img width=16 height=16 align=center src='memory:XRC_resource/xrc_bitmap_crystal.cpp$icons_no_16x16.png'>");
        return ret + HTML_INSET_END + wxT("</font>");
    }
};


////////////////////////////////////////////////////////////////////////
//
//  wxLODListBox
//
////////////////////////////////////////////////////////////////////////

class wxLODListBox : public wxSceneryListBox {
public:
    wxLODListBox(wxWindow *parent, cSCNFile *contents): wxSceneryListBox(parent, contents) {
        UpdateContents();
        if (m_contents) {
            SetSelection(m_contents->lods.size()?0:wxNOT_FOUND);
        } else {
            SetSelection(wxNOT_FOUND);
        }
    }
    virtual void UpdateContents() {
        SetItemCount(m_contents->lods.size());
        if (GetSelection() >= m_contents->lods.size())
            SetSelection(wxNOT_FOUND);
        RefreshAll();
    }
protected:
    virtual wxString OnGetItem(size_t n) const {
        wxString addon = wxT("");
        if ((m_contents->lods[n].unk2 != 0) || (m_contents->lods[n].unk4!=0) || (m_contents->lods[n].unk14!=0))
            addon = wxString::Format(wxT(" (%ld/%ld/%ld)"),
                m_contents->lods[n].unk2,
                m_contents->lods[n].unk4,
                m_contents->lods[n].unk14);
        wxString sub = HTML_INSET_START + wxEncodeHtmlEntities(m_contents->lods[n].modelname);
        if (m_contents->lods[n].animations.size()) {
            sub += wxT("<ul>");
            for (cStringIterator it = m_contents->lods[n].animations.begin(); it != m_contents->lods[n].animations.end(); it++) {
                sub += wxT("<li>") + *it + wxT("</li>");
            }
            sub += wxT("</ul>");
        }
        sub += wxT("</td><td width=16 valign=top><img width=16 height=16 align=center src='memory:XRC_resource/xrc_bitmap_crystal.cpp$icons_");
        if (m_contents->lods[n].animated)
            sub += wxT("aim_online");
        else
            sub += wxT("folder_home");
        sub += wxT("_16x16.png'>");
        sub += HTML_INSET_END;
        return wxT("<font size='2'>")+wxString::Format(wxT("%.1f"), m_contents->lods[n].distance) + addon + sub +wxT("</font>");
    }
};


////////////////////////////////////////////////////////////////////////
//
//  wxAnimationListBox
//
////////////////////////////////////////////////////////////////////////

class wxAnimationsListBox : public wxSceneryListBox {
public:
    wxAnimationsListBox(wxWindow *parent, cSCNFile *contents): wxSceneryListBox(parent, contents) {
        UpdateContents();
        if (m_contents) {
            SetSelection(m_contents->animations.size()?0:wxNOT_FOUND);
        } else {
            SetSelection(wxNOT_FOUND);
        }
    }
    virtual void UpdateContents() {
        SetItemCount(m_contents->animations.size());
        if (GetSelection() >= m_contents->animations.size())
            SetSelection(wxNOT_FOUND);
        RefreshAll();
    }
protected:
    virtual wxString OnGetItem(size_t n) const {
        wxString ret = wxT("<font size='2'>");
        ret += wxEncodeHtmlEntities(m_contents->animations[n].name);
        float length = 0.0;
        for(cBoneAnimation::iterator it = m_contents->animations[n].boneanimations.begin(); it != m_contents->animations[n].boneanimations.end(); it++) {
            for(cTXYZ::iterator iter = it->translations.begin(); iter != it->translations.end(); iter++) {
                if (iter->v.Time > length)
                    length = iter->v.Time;
            }
            for(cTXYZ::iterator iter = it->rotations.begin(); iter != it->rotations.end(); iter++) {
                if (iter->v.Time > length)
                    length = iter->v.Time;
            }
        }
        ret += HTML_INSET_START;
        ret += wxString::Format(wxT("%2f s long, %d bone(s)"), length, m_contents->animations[n].boneanimations.size());
        return ret + HTML_INSET_END + wxT("</font>");
    }
};





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
EVT_COMBOBOX(XRCID("m_textPath"), dlgCreateScenery::OnMakeDirty)
EVT_BUTTON(XRCID("m_btCheck"), dlgCreateScenery::OnCheck)
EVT_BUTTON(XRCID("m_btCreate"), dlgCreateScenery::OnCreate)

END_EVENT_TABLE()

dlgCreateScenery::dlgCreateScenery(wxWindow *parent) {
    m_tbCS = NULL;

    InitWidgetsFromXRC((wxWindow *)parent);
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

    m_htlbTexture = new wxTextureListBox(m_panTextures, &m_SCN);
    m_htlbTexture->SetToolTip(_("Texture list\nTextures from the texture chache are shown in grey."));
    wxXmlResource::Get()->AttachUnknownControl(wxT("m_htlbTexture"), m_htlbTexture, m_panTextures);
    m_htlbReferences = new wxReferenceListBox(m_panReferences, &m_SCN);
    wxXmlResource::Get()->AttachUnknownControl(wxT("m_htlbReferences"), m_htlbReferences, m_panReferences);

    m_htlbModel = new wxModelListBox(m_panSModel, &m_SCN);
    m_htlbModel->SetToolTip(_("Static model list\nOrange: Warnings occured during loading the model form the scenery file.\nRed: An error occured during loading the model form the scenery file. Usually the model file was not found."));
    wxXmlResource::Get()->AttachUnknownControl(wxT("m_htlbModel"), m_htlbModel, m_panSModel);
    m_htlbAModel = new wxAnimatedModelListBox(m_panAModel, &m_SCN);
    m_htlbAModel->SetToolTip(_("Animated model list\nOrange: Warnings occured during loading the model form the scenery file.\nRed: An error occured during loading the model form the scenery file. Usually the model file was not found."));
    wxXmlResource::Get()->AttachUnknownControl(wxT("m_htlbAModel"), m_htlbAModel, m_panAModel);

    m_htlbLOD = new wxLODListBox(this, &m_SCN);
    wxXmlResource::Get()->AttachUnknownControl(wxT("m_htlbLOD"), m_htlbLOD, this);

    wxTextCtrl *t_text = XRCCTRL(*this,"m_textPath",wxTextCtrl);
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

    m_textPath = new wxFileSelectorCombo<wxDirDialog>(this, dirdlg, &::wxGetApp().g_workdir, XRCID("m_textPath"),
                                         wxEmptyString, wxDefaultPosition, wxDefaultSize, 0/*wxCB_READONLY*/);
    m_textPath->GetTextCtrl()->SetEditable(false);
    t_text->GetContainingSizer()->Replace(t_text, m_textPath);
    t_text->Destroy();

    m_textName->SetValidator(wxExtendedValidator(&m_SCN.name));
    m_textPath->SetValidator(wxExtendedValidator(&m_SCN.ovlpath, true, true, true));

    m_htlbAnimation = new wxAnimationsListBox(this, &m_SCN);
    wxXmlResource::Get()->AttachUnknownControl(wxT("m_htlbAnimation"), m_htlbAnimation, this);

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

    m_dirtyfile = false;
}

dlgCreateScenery::~dlgCreateScenery() {
    WRITE_APP_SIZE("CreateSceney", GetSize());
}

wxString dlgCreateScenery::GetOVLPath() {
    if (m_SCN.ovlpath.GetPath() != wxT(""))
        return m_SCN.ovlpath.GetPathWithSep() + m_SCN.name + wxT(".common.ovl");
    else
        return wxEmptyString;
}

void dlgCreateScenery::UpdateControlState() {
    int count = m_SCN.flexitextures.size();
    int sel = m_htlbTexture->GetSelection();
    m_spinTexture->Enable(count>=2);
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
    m_spinModel->Enable(count>=2);
    m_btModelEdit->Enable(sel>=0);
    m_btModelCopy->Enable(sel>=0);
    m_btModelConvert->Enable(sel>=0);
    m_btModelDel->Enable(sel>=0);
    m_btModelClear->Enable(count>=1);
    m_btLODAdd->Enable(count>=1);
    m_btLODAuto->Enable(((count==1) || (m_SCN.animatedmodels.size()==1)) && !((count==1) && (m_SCN.animatedmodels.size()==1)));
    m_nbModels->SetPageText(0, wxString::Format("Static Models (%d)", count));

    count = m_SCN.animatedmodels.size();
    sel = m_htlbAModel->GetSelection();
    m_spinAModel->Enable(count>=2);
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

    count = m_SCN.animations.size();
    sel = m_htlbAnimation->GetSelection();
    m_spinAnimation->Enable(count>=2);
    m_btAnimationEdit->Enable((sel>=0) && (sel<count));
    m_btAnimationCopy->Enable((sel>=0) && (sel<count));
    m_btAnimationDel->Enable((sel>=0) && (sel<count));
    m_btAnimationClear->Enable(count>=1);
    bool en = false;
    if (sel >= 0) {
        int l = m_htlbLOD->GetSelection();
        if (l >= 0) {
            if (m_SCN.lods[l].animated)
                en = true;
        }
    }
    m_btAnimationAddToLod->Enable(en);

    m_btCreate->Enable(m_SCN.flexitextures.size() || ((m_SCN.models.size() || m_SCN.animatedmodels.size()) && m_SCN.lods.size()));
    /*
    if (m_SCN.models.size())
        m_btCreate->SetLabel(_("Create Scenery OVL"));
    else
        m_btCreate->SetLabel(_("Create Texture OVL"));
    */

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
    m_htlbReferences->UpdateContents();
    m_htlbModel->UpdateContents();
    m_htlbAModel->UpdateContents();
    m_htlbLOD->UpdateContents();
    m_htlbAnimation->UpdateContents();

    TransferDataToWindow();

    m_textReference->ChangeValue(m_defaultRef);

    UpdateControlState();
}

bool dlgCreateScenery::Save(bool as) {
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
            if (!m_SCN.Save()) {
                ::wxMessageBox(_("Saving scenery file failed. You will be asked for a new file name now."), _("Error"), wxOK|wxICON_ERROR, this);
                // fall through
            } else {
                MakeDirty(false);
                UpdateControlState();
                return true;
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
        if (!m_SCN.Save()) {
            ::wxMessageBox(_("Error saving SCN file."), _("Error"), wxOK | wxICON_ERROR, this);
        } else {
            MakeDirty(false);
            UpdateControlState();
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
                                   _("Scenery Files (*.scn, *.xml)|*.scn;*.xml"),
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

void dlgCreateScenery::OnShowUnknowns(wxCommandEvent& WXUNUSED(event)) {
//    if (m_checkShowUnknown->IsChecked()) {
//        if (::wxMessageBox(_("You should better leave these settings alone.\nDo you want to continue?"), _("Warning"), wxYES_NO | wxNO_DEFAULT | wxICON_WARNING, this)==wxNO) {
//            m_checkShowUnknown->SetValue(false);
//            return;
//        }
//    }
//    bool savedirty = m_dirtyfile;
//    TransferDataFromWindow();
    m_panelUnknown->Show( m_checkShowUnknown->IsChecked());
    Fit();
    Layout();
//    TransferDataToWindow();
//    MakeDirty(savedirty);
}

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
            ILinfo info;
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
    dialog->SetModel(cModel(READ_APP_MATRIX(), READ_RCT3_ORIENTATION()));
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
    dialog->SetAnimatedModel(cAnimatedModel(READ_APP_MATRIX(), READ_RCT3_ORIENTATION()));
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
        wxString errtext = m_SCN.animatedmodels[sel].error[0];
        while (m_SCN.animatedmodels[sel].error.erase(m_SCN.animatedmodels[sel].error.begin()) != m_SCN.animatedmodels[sel].error.end()) {
            errtext += wxT("\n\n") + m_SCN.animatedmodels[sel].error[0];
        }
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
        if (::wxMessageBox(_("During the conversion all additional information of the animated model will be lost (Bone <-> Group assignment, bone parent and position 2 settings).\nDo you want to continue?"), _("Question"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION, this)==wxNO)
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

////////////////////////////////////////////////////////////////////////
//
//  dlgCreateScenery, LODs
//
////////////////////////////////////////////////////////////////////////

void dlgCreateScenery::OnLODAdd(wxCommandEvent& WXUNUSED(event)) {
    if (!m_SCN.CheckForModelNameDuplicates()) {
        ::wxMessageBox(_("There are duplicate model names. A model name must be unique, no matter whether it's animated or not"), _("Error"), wxOK|wxICON_ERROR, this);
        return;
    }
    dlgLOD *dialog = new dlgLOD(&m_SCN.models, &m_SCN.animatedmodels, this);
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
    dlgLOD *dialog = new dlgLOD(&m_SCN.models, &m_SCN.animatedmodels, this);
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
}

void dlgCreateScenery::OnAnimationDown(wxSpinEvent& WXUNUSED(event)) {
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
}

void dlgCreateScenery::OnAnimationDel(wxCommandEvent& WXUNUSED(event)) {
    int sel = m_htlbAnimation->GetSelection();
    if (sel<0)
        return;

    m_SCN.animations.erase(m_SCN.animations.begin() + sel);

    MakeDirty();
    m_htlbAnimation->UpdateContents();
    m_htlbAnimation->SetSelection(sel-1);
    UpdateControlState();
}

void dlgCreateScenery::OnAnimationAddToLod(wxCommandEvent& WXUNUSED(event)) {
// TODO (tobi#1#): OnAnimationAddToLod
    int lod = m_htlbLOD->GetSelection();
    int ani = m_htlbAnimation->GetSelection();
    if ((ani < 0) or (lod < 0))
        return;

    cLOD* plod = &m_SCN.lods[lod];
    if (!plod->animated)
        return;

    cAnimation* pani = &m_SCN.animations[ani];
    if (plod->animations.Index(pani->name) == wxNOT_FOUND) {
        plod->animations.Add(pani->name);
        m_htlbLOD->UpdateContents();
    }
}

void dlgCreateScenery::OnAnimationClear(wxCommandEvent& WXUNUSED(event)) {
    if (m_SCN.animations.size()) {
        if (::wxMessageBox(_("Do you really want to delete all animations?"), _("Question"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION, this)==wxNO)
            return;
    }

    m_SCN.animations.clear();

    MakeDirty();
    m_htlbAnimation->UpdateContents();
    m_htlbAnimation->SetSelection(-1);
    UpdateControlState();
}

////////////////////////////////////////////////////////////////////////
//
//  dlgCreateScenery, Create Scenery
//
////////////////////////////////////////////////////////////////////////

WX_DECLARE_STRING_HASH_MAP(char *, ovlNameLookup);

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
    if (all_ok)
        wxLogMessage(_("All data seems to be ok."));
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
    unsigned int meshes = 0;
    unsigned int textures = 0;
    unsigned int progress_count = 0;

    bool filenameok = false;
    int diaret = wxID_OK;

    TransferDataFromWindow();

    //cSCNFile work = m_SCN;
    wxFileName sfile;
    wxFileDialog *dialog = NULL;

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

/*
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
                for (cModel::iterator i_mod = work.models.begin(); i_mod != work.models.end(); i_mod++) {
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
                    for (cMeshStruct::iterator i_mesh = i_mod->meshstructs.begin(); i_mesh != i_mod->meshstructs.end(); i_mesh++) {
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
                for (cLOD::iterator i_lod = work.lods.begin(); i_lod != work.lods.end(); i_lod++) {
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
                for (cModel::iterator i_mod = work.models.begin(); i_mod != work.models.end(); i_mod++) {
                    if (i_mod->used) {
                        for (cMeshStruct::iterator i_mesh = i_mod->meshstructs.begin(); i_mesh != i_mod->meshstructs.end(); i_mesh++) {
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
            for (cFlexiTexture::iterator i_ftx = work.flexitextures.begin(); i_ftx != work.flexitextures.end(); i_ftx++) {
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
                for (cFlexiTextureFrame::iterator i_ftxfr = i_ftx->Frames.begin(); i_ftxfr != i_ftx->Frames.end(); i_ftxfr++) {
                    if (!(i_ftxfr->Texture.IsOk() && i_ftxfr->Texture.FileExists())) {
                        error = true;
                        wxLogError(_("Texture '%s': Texture file '%s' not found."), i_ftx->Name.c_str(), i_ftxfr->Texture.GetName().c_str());
                        break;
                    }
                    if ((i_ftxfr->AlphaSource==CFTF_ALPHA_EXTERNAL) && (i_ftxfr->Alpha != wxT("")) && (!(i_ftxfr->Alpha.IsOk() && i_ftxfr->Alpha.FileExists()))) {
                        i_ftxfr->AlphaSource = CFTF_ALPHA_NONE;
                        wxLogWarning(_("Texture '%s': Alpha channel file '%s' not found. Alpha deactivated."), i_ftx->Name.c_str(), i_ftxfr->Alpha.GetName().c_str());
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
                        if (i_ftx->Animation[i].frame >= i_ftx->Frames.size()) {
                            // Illegal reference
                            error = true;
                            wxLogError(_("Texture '%s': Animation step %d references non-existing frame."), i_ftx->Name.c_str(), i+1);
                            break;
                        }
                        i_ftx->Frames[i_ftx->Animation[i].frame].used = true;
                    }
                } else {
                    // We need to add one refering to the first frame
                    cFlexiTextureAnim an(0);
                    i_ftx->Animation.push_back(an);
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
                        for (cFlexiTextureAnim::iterator i_anim = i_ftx->Animation.begin(); i_anim != i_ftx->Animation.end(); i_anim++) {
                            if (i_anim->frame > i)
                                i_anim->frame -= 1;
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
*/
    if (!cont)
        goto deinitlogger;

    try {
        wxBusyCursor bc;
        was_going = true;
        m_SCN.Make();
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

/*
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
            for (cModel::iterator i_mod = work.models.begin(); i_mod != work.models.end(); i_mod++) {
                if (!i_mod->used)
                    continue;
                char *name = strdup(i_mod->name.c_str());
                ssnamestore.push_back(name);
                sslookup[i_mod->name] = name;

                StaticShape1 *sh = new StaticShape1;
                ss1store.push_back(sh);

                int mesh_count = 0;
                for (cMeshStruct::iterator i_mesh = i_mod->meshstructs.begin(); i_mesh != i_mod->meshstructs.end(); i_mesh++) {
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
                D3DMATRIX undoDamage;
                int m = 0;
                for(m = i_mod->transformnames.size()-1; m>=0; m--) {
                    if (i_mod->transformnames[m] == wxT("-"))
                        break;
                }
                if (m>0) {
                    // Note: if m=0, the first is the separator, so this can fall through to no separator
                    std::vector<D3DMATRIX> undostack;
                    for (int n = m; n < i_mod->transforms.size(); n++)
                        undostack.push_back(i_mod->transforms[n]);
                    undoDamage = matrixInverse(matrixMultiply(undostack));
                } else {
                    undoDamage = matrixInverse(transformMatrix);
                }
                // We need to reevaluate do_transform
                if (!do_transform)
                    do_transform = !matrixIsEqual(undoDamage, matrixGetUnity());

                if (i_mod->effectpoints.size() != 0) {
                    unsigned int e;
                    sh->EffectName = new char *[sh->EffectCount];
                    sh->EffectPosition = new D3DMATRIX[sh->EffectCount];
                    for (e = 0; e < i_mod->effectpoints.size(); e++) {
                        sh->EffectName[e] = strdup(i_mod->effectpoints[e].name.c_str());

                        if (do_transform) {
                            std::vector<D3DMATRIX> tempstack = i_mod->effectpoints[e].transforms;
                            // to correctely apply the model transformation matrix to effect points we have to
                            // transform in-game objects into modeler-space first by applying the inverse
                            // of the model transformation matrix
                            // (now replaced by the undoDamage matrix to allow separation of the coordinate
                            // system fix from other model transformations)
                            tempstack.insert(tempstack.begin(), undoDamage);
                            // At the end of the stack, transform back
                            tempstack.push_back(transformMatrix);
                            sh->EffectPosition[e] = matrixMultiply(tempstack);
                        } else {
                            D3DMATRIX me = matrixMultiply(i_mod->effectpoints[e].transforms);
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
                D3DVECTOR temp_min, temp_max;
                //D3DVECTOR glass_min, glass_max;
                //boundsInit(&glass_min, &glass_max);
                //bool has_unknown = false;
                for (cMeshStruct::iterator i_mesh = i_mod->meshstructs.begin(); i_mesh != i_mod->meshstructs.end(); i_mesh++) {
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

                        boundsInit(&temp_min, &temp_max);
//                        if (i_mesh->place != SS2_PLACE_UNKNOWN) {
                            object->FetchObject(CurrentObj, &sh2->VertexCount, &sh2->Vertexes, &sh2->IndexCount,
                                                &sh2->Triangles, &temp_min, &temp_max,
                                                const_cast<D3DMATRIX *> ((do_transform)?(&transformMatrix):NULL));
//                        } else {
//                            D3DVECTOR up;
//                            up.x = -1.0;
//                            up.y = 0.0;
//                            up.z = -0.000000131179;
//                            object->FetchObject(CurrentObj, &sh2->VertexCount, &sh2->Vertexes, &sh2->IndexCount,
//                                                &sh2->Triangles, &temp_min, &temp_max,
//                                                const_cast<D3DMATRIX *> ((do_transform)?(&transformMatrix):NULL),
//                                                &up);
//                        }
                        boundsContain(&temp_min, &temp_max, &sh->BoundingBox1, &sh->BoundingBox2);

//                        if (i_mesh->place == SS2_PLACE_UNKNOWN) {
//                            boundsContain(&temp_min, &temp_max, &glass_min, &glass_max);
//                            has_unknown = true;
//                        }

                        CurrentMesh++;
                    }
                    CurrentObj++;
                }

//                if (has_unknown) {
//                    if ((glass_min.x - sh->BoundingBox1.x) < 0.001)
//                        sh->BoundingBox1.x -= 0.001;
//                    if ((glass_min.y - sh->BoundingBox1.y) < 0.001)
//                        sh->BoundingBox1.y -= 0.001;
//                    if ((glass_min.z - sh->BoundingBox1.z) < 0.001)
//                        sh->BoundingBox1.z -= 0.001;
//                    if ((sh->BoundingBox2.x - glass_max.x) < 0.001)
//                        sh->BoundingBox2.x += 0.001;
//                    if ((sh->BoundingBox2.y - glass_max.y) < 0.001)
//                        sh->BoundingBox2.y += 0.001;
//                    if ((sh->BoundingBox2.z - glass_max.z) < 0.001)
//                        sh->BoundingBox2.z += 0.001;
//                }

                char **ftx = new char *[mesh_count];
                ftxstore.push_back(ftx);
                char **txs = new char *[mesh_count];
                txsstore.push_back(txs);
                meshcountstore.push_back(mesh_count);

                CurrentObj = 0;
                for (cMeshStruct::iterator ms = i_mod->meshstructs.begin();
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
        for (cFlexiTexture::iterator i_ftx = work.flexitextures.begin(); i_ftx != work.flexitextures.end(); i_ftx++) {
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
            fti->offsetCount = 0;
            for(cFlexiTextureAnim::iterator i_anim = i_ftx->Animation.begin(); i_anim != i_ftx->Animation.end(); i_anim++)
                fti->offsetCount += i_anim->count;
            fti->offset1 = new unsigned long[fti->offsetCount];
            unsigned long x = 0;
            for(cFlexiTextureAnim::iterator i_anim = i_ftx->Animation.begin(); i_anim != i_ftx->Animation.end(); i_anim++) {
                for (unsigned long i = 0; i < i_anim->count; i++) {
                    fti->offset1[x] = i_anim->frame;
                    x++;
                }
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
            for (cFlexiTextureFrame::iterator i_ftxfr = i_ftx->Frames.begin(); i_ftxfr != i_ftx->Frames.end(); i_ftxfr++) {
                ilBindImage(tex[0]);

                unsigned char *alphadata = 0;

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
                    wxLogError(_("Texture '%s' size is different from size of the first frame."), i_ftxfr->Texture.GetFullPath().fn_str());
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
                if (i_ftxfr->AlphaSource == CFTF_ALPHA_INTERNAL) {
                    alphadata = ilGetAlpha(IL_UNSIGNED_BYTE);
                    if (!alphadata) {
                        error = true;
                        wxLogError(_("Texture '%s' was supposed to have an alpha channel but there was an error retrieving it."), i_ftxfr->Texture.GetFullPath().fn_str());
                        goto scenerycleanup;
                    }
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
//                bool AlphaPresent = false;
//                if (i_ftxfr->Alpha != wxT("")) {
//                    AlphaPresent = true;
//                }
                if (i_ftxfr->AlphaSource == CFTF_ALPHA_EXTERNAL) {
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
                    alphadata = (unsigned char *) malloc(width * height);
                    memcpy(alphadata, ilGetData(), width * height);

                }

                RGBQUAD *colors;
                colors = new RGBQUAD[256];
                memset(colors, 0, 256 * sizeof(RGBQUAD));
                unsigned char *texture = 0;
                texture = new unsigned char[width * height];
                ilBindImage(tex[0]);
                memcpy(texture, ilGetData(), width * height);
                memcpy(colors, ilGetPalette(), ilGetInteger(IL_PALETTE_NUM_COLS) * 4);
                for (unsigned int j = 0; j < 256; j++)
                    colors[j].rgbReserved = i_ftxfr->AlphaCutoff;
//                if (AlphaPresent == true) {
//                    alphadata = malloc(width * height);
//                    ilBindImage(tex[1]);
//                    memcpy(alphadata, ilGetData(), width * height);
//                }

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
                free(ftid->fts2[i].alpha);
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
*/

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
#ifndef __WXDEBUG__
    wxLog::SetActiveTarget(old);
    delete logex;
#endif
}
