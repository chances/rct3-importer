///////////////////////////////////////////////////////////////////////////////
//
// Subfiles for html listboxes
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


#include "wx_pch.h"
#include "wxapp.h"
#include "wxdlgCreateScenery_HTLB.h"

#include "htmlentities.h"

#include "SCNFile.h"

////////////////////////////////////////////////////////////////////////
//
//  wxTextureListBox
//
////////////////////////////////////////////////////////////////////////

wxTextureListBox::wxTextureListBox(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, int n, const wxString choices[], long style):
    wxSceneryListBox(parent, id, pos, size, n, choices, style) {
    UpdateContents();
}

void wxTextureListBox::UpdateContents() {
    if (m_contents) {
        SetItemCount(m_contents->flexitextures.size() + ::wxGetApp().g_texturecache.size());
        if (GetSelection() >= m_contents->flexitextures.size())
            SetSelection(wxNOT_FOUND);
    } else {
        SetItemCount(0);
        SetSelection(wxNOT_FOUND);
    }
    RefreshAll();
}

wxString wxTextureListBox::OnGetItem(size_t n) const {
    if (!m_contents)
            return wxT("Internal Error");
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


////////////////////////////////////////////////////////////////////////
//
//  wxReferenceListBox
//
////////////////////////////////////////////////////////////////////////

wxReferenceListBox::wxReferenceListBox(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, int n, const wxString choices[], long style):
    wxSceneryListBox(parent, id, pos, size, n, choices, style) {
    UpdateContents();
}

void wxReferenceListBox::UpdateContents() {
    if (m_contents) {
        SetItemCount(m_contents->references.size());
        if (GetSelection() >= m_contents->references.size())
            SetSelection(wxNOT_FOUND);
    } else {
        SetItemCount(0);
        SetSelection(wxNOT_FOUND);
    }
    RefreshAll();
}

wxString wxReferenceListBox::OnGetItem(size_t n) const {
    return wxT("<font size='2'>")+wxEncodeHtmlEntities(m_contents->references[n].name)+wxT("</font>");
}

////////////////////////////////////////////////////////////////////////
//
//  wxModelListBox
//
////////////////////////////////////////////////////////////////////////

wxModelListBox::wxModelListBox(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, int n, const wxString choices[], long style):
    wxSceneryListBox(parent, id, pos, size, n, choices, style) {
    UpdateContents();
}

void wxModelListBox::UpdateContents() {
    if (m_contents) {
        SetItemCount(m_contents->models.size());
        if (GetSelection() >= m_contents->models.size())
            SetSelection(wxNOT_FOUND);
    } else {
        SetItemCount(0);
        SetSelection(wxNOT_FOUND);
    }
    RefreshAll();
}

wxString wxModelListBox::OnGetItem(size_t n) const {
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

////////////////////////////////////////////////////////////////////////
//
//  wxAnimatedModelListBox
//
////////////////////////////////////////////////////////////////////////

wxAnimatedModelListBox::wxAnimatedModelListBox(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, int n, const wxString choices[], long style):
    wxSceneryListBox(parent, id, pos, size, n, choices, style) {
    UpdateContents();
}

void wxAnimatedModelListBox::UpdateContents() {
    if (m_contents) {
        SetItemCount(m_contents->animatedmodels.size());
        if (GetSelection() >= m_contents->animatedmodels.size())
            SetSelection(wxNOT_FOUND);
    } else {
        SetItemCount(0);
        SetSelection(wxNOT_FOUND);
    }
    RefreshAll();
}

wxString wxAnimatedModelListBox::OnGetItem(size_t n) const {
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


////////////////////////////////////////////////////////////////////////
//
//  wxLODListBox
//
////////////////////////////////////////////////////////////////////////

wxLODListBox::wxLODListBox(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, int n, const wxString choices[], long style):
    wxSceneryListBox(parent, id, pos, size, n, choices, style) {
    UpdateContents();
}

void wxLODListBox::UpdateContents() {
    if (m_contents) {
        SetItemCount(m_contents->lods.size());
        if (GetSelection() >= m_contents->lods.size())
            SetSelection(wxNOT_FOUND);
    } else {
        SetItemCount(0);
        SetSelection(wxNOT_FOUND);
    }
    RefreshAll();
}

wxString wxLODListBox::OnGetItem(size_t n) const {
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


////////////////////////////////////////////////////////////////////////
//
//  wxAnimationListBox
//
////////////////////////////////////////////////////////////////////////

wxAnimationsListBox::wxAnimationsListBox(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, int n, const wxString choices[], long style):
    wxSceneryListBox(parent, id, pos, size, n, choices, style) {
    UpdateContents();
}

void wxAnimationsListBox::UpdateContents() {
    if (m_contents) {
        SetItemCount(m_contents->animations.size());
        if (GetSelection() >= m_contents->animations.size())
            SetSelection(wxNOT_FOUND);
    } else {
        SetItemCount(0);
        SetSelection(wxNOT_FOUND);
    }
    RefreshAll();
}

wxString wxAnimationsListBox::OnGetItem(size_t n) const {
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


////////////////////////////////////////////////////////////////////////
//
//  wxSplineListBox
//
////////////////////////////////////////////////////////////////////////

wxSplineListBox::wxSplineListBox(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, int n, const wxString choices[], long style):
    wxSceneryListBox(parent, id, pos, size, n, choices, style) {
    UpdateContents();
}

void wxSplineListBox::UpdateContents() {
    if (m_contents) {
        SetItemCount(m_contents->splines.size());
        if (GetSelection() >= m_contents->splines.size())
            SetSelection(wxNOT_FOUND);
    } else {
        SetItemCount(0);
        SetSelection(wxNOT_FOUND);
    }
    RefreshAll();
}

wxString wxSplineListBox::OnGetItem(size_t n) const {
    wxString ret = wxT("<font size='2'>");
    ret += wxEncodeHtmlEntities(m_contents->splines[n].spline.m_name);
    /*
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
    */
    return ret + wxT("</font>");
}


