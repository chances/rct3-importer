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

#ifndef WXDLGCREATESCENERY_H_INCLUDED
#define WXDLGCREATESCENERY_H_INCLUDED

#include <wx/xrc/xmlres.h>
#include <wx/htmllbox.h>
#include <wx/notebook.h>
#include <wx/spinbutt.h>

#include "colhtmllbox.h"
#include "fileselectorcombo.h"
#include "SCNFile.h"
#include "wxutilityfunctions.h"

#include "xrc\res_createscenery.h"

#include "wxdlgCreateScenery_HTLB.h"
#include "wxdlgLoadFromFile.h"

class dlgCreateScenery : public rcdlgCreateScenery {
protected:
    wxToolBar *m_tbCS;

    virtual bool TransferDataFromWindow();
    virtual bool TransferDataToWindow();

    void MakeDirty(bool dirty = true);
    bool Save(bool as = false);

    void OnToolBar(wxCommandEvent& event);
    void OnControlUpdate(wxCommandEvent& event);
    void OnMakeDirty(wxCommandEvent& event);
    void OnClose(wxCloseEvent& event);
    void OnInitDialog(wxInitDialogEvent& event);

    void OnTextureUp(wxSpinEvent& event);
    void OnTextureDown(wxSpinEvent& event);
    void OnTextureAdd(wxCommandEvent& event);
    void OnTextureEdit(wxCommandEvent& event);
    void OnTextureCopy(wxCommandEvent& event);
    void OnTextureDel(wxCommandEvent& event);
    void OnTextureLoad(wxCommandEvent& event);
    void OnTextureClear(wxCommandEvent& event);

    void OnReferenceAdd(wxCommandEvent& event);
    void OnReferenceDel(wxCommandEvent& event);
    void OnReferenceLoad(wxCommandEvent& event);
    void OnReferenceClear(wxCommandEvent& event);

    void OnModelUp(wxSpinEvent& event);
    void OnModelDown(wxSpinEvent& event);
    void OnModelAdd(wxCommandEvent& event);
    void OnModelEdit(wxCommandEvent& event);
    void OnModelCopy(wxCommandEvent& event);
    void OnModelConvert(wxCommandEvent& event);
    void OnModelDel(wxCommandEvent& event);
    void OnModelLoad(wxCommandEvent& event);
    void OnModelClear(wxCommandEvent& event);

    void OnAModelUp(wxSpinEvent& event);
    void OnAModelDown(wxSpinEvent& event);
    void OnAModelAdd(wxCommandEvent& event);
    void OnAModelEdit(wxCommandEvent& event);
    void OnAModelCopy(wxCommandEvent& event);
    void OnAModelConvert(wxCommandEvent& event);
    void OnAModelDel(wxCommandEvent& event);
    void OnAModelLoad(wxCommandEvent& event);
    void OnAModelClear(wxCommandEvent& event);

	void OnLODShowAnimation( wxCommandEvent& event );
    void OnLODAdd(wxCommandEvent& event);
    void OnLODEdit(wxCommandEvent& event);
    void OnLODCopy(wxCommandEvent& event);
    void OnLODDel(wxCommandEvent& event);
    void OnLODAuto(wxCommandEvent& event);
    void OnLODsRDClick(wxMouseEvent& event);
    void OnLODClear(wxCommandEvent& event);
    void OnLODRDClear(wxMouseEvent& event);

    void OnVisualNormal(wxCommandEvent& event);
    void OnVisualSwayingTree(wxCommandEvent& event);

    void OnAnimationUp(wxSpinEvent& event);
    void OnAnimationDown(wxSpinEvent& event);
    void OnAnimationAdd(wxCommandEvent& event);
    void OnAnimationEdit(wxCommandEvent& event);
    void OnAnimationCopy(wxCommandEvent& event);
    void OnAnimationAddToLod(wxCommandEvent& event);
    void OnAnimationDel(wxCommandEvent& event);
    void OnAnimationLoad(wxCommandEvent& event);
    void OnAnimationClear(wxCommandEvent& event);
    void OnAnimationRDLoad(wxMouseEvent& event);
    void OnAnimationRDClear(wxMouseEvent& event);

    virtual void OnSplineUp( wxSpinEvent& event );
    virtual void OnSplineDown( wxSpinEvent& event );
    virtual void OnSplineEdit( wxCommandEvent& event );
    virtual void OnSplineCopy( wxCommandEvent& event );
    virtual void OnSplineDel( wxCommandEvent& event );
    virtual void OnSplineLoad( wxCommandEvent& event );
    virtual void OnSplineClear( wxCommandEvent& event );
    virtual void OnSplineRDLoad( wxMouseEvent& event );
    virtual void OnSplineRDClear( wxMouseEvent& event );

    void OnAutoName(wxCommandEvent& event);
    void OnCheck(wxCommandEvent& event);
    void OnCreate(wxCommandEvent& event);

private:
    bool m_dirtyfile;
    wxString m_defaultRef;
    wxString m_prefix;
    wxString m_theme;

    void UpdateAll();

    template<class T>
    inline wxColourHtmlListBox* listbox() { return T::breakme; }

    template<class T>
    inline void implementSpinUp() {
        int sel = listbox<T>()->GetSelection();
        if (sel < 1)
            return;
        T ft = m_SCN.collection<T>()[sel];
        m_SCN.collection<T>().erase(m_SCN.collection<T>().begin() + sel);
        m_SCN.collection<T>().insert(m_SCN.collection<T>().begin() + sel - 1, ft);

        MakeDirty();
        listbox<T>()->UpdateContents();
        listbox<T>()->SetSelection(sel-1);
        UpdateControlState();
    }
    template<class T>
    inline void implementSpinDown() {
        int count = m_SCN.collection<T>().size();
        int sel = listbox<T>()->GetSelection();
        if ((count-sel) <= 1)
            return;
        T ft = m_SCN.collection<T>()[sel];
        m_SCN.collection<T>().erase(m_SCN.collection<T>().begin() + sel);
        m_SCN.collection<T>().insert(m_SCN.collection<T>().begin() + sel + 1, ft);

        MakeDirty();
        listbox<T>()->UpdateContents();
        listbox<T>()->SetSelection(sel+1);
        UpdateControlState();
    }
    template<class T>
    inline void implementCopy() {
        int sel = listbox<T>()->GetSelection();
        if (sel<0)
            return;

        T ft = m_SCN.collection<T>()[sel];
        int m = 2;
        while (std::find_if(m_SCN.collection<T>().begin(), m_SCN.collection<T>().end(), NamePredicate<T>(wxString::Format("%s %d", NameExtractorC<T>(ft).c_str(), m))) != m_SCN.collection<T>().end())
            m++;
        NameExtractor<T>(ft) = wxString::Format("%s %d", NameExtractorC<T>(ft).c_str(), m);
        m_SCN.collection<T>().insert(m_SCN.collection<T>().begin() + sel + 1, ft);

        MakeDirty();
        listbox<T>()->UpdateContents();
        listbox<T>()->SetSelection(sel+1);
        UpdateControlState();
    }
    template<class T>
    inline void implementDel() {
        int sel = listbox<T>()->GetSelection();
        if (sel<0)
            return;

        m_SCN.collection<T>().erase(m_SCN.collection<T>().begin() + sel);

        MakeDirty();
        listbox<T>()->UpdateContents();
        listbox<T>()->SetSelection(sel-1);
        UpdateControlState();
    }
    template<class T>
    inline void implementClear(bool do_ask = true) {
        if (do_ask && m_SCN.collection<T>().size()) {
            if (::wxMessageBox(wxString::Format(_("Do you really want to delete all %s?"), T::getPlural().c_str()), _("Question"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION, this)==wxNO)
                return;
        }

        m_SCN.collection<T>().clear();

        MakeDirty();
        listbox<T>()->UpdateContents();
        listbox<T>()->SetSelection(-1);
        UpdateControlState();
    }
    template<class T>
    inline void implementLoad(bool do_clear = false) {
        bool updated = do_clear;
        if (do_clear) {
            m_SCN.collection<T>().clear();
            listbox<T>()->UpdateContents();
            listbox<T>()->SetSelection(-1);
        }

        boost::shared_ptr<dlgLoadFromFile> dlg(new dlgLoadFromFile(this, m_SCN, dlgLoadFromFile::type<T>(), &updated, listbox<T>()), wxWindowDestroyer);
        dlg->ShowModal();
        if (updated) {
            MakeDirty();
            UpdateControlState();
        }
    }

public:
    cSCNFile m_SCN;

    dlgCreateScenery(wxWindow *parent=NULL);
    virtual ~dlgCreateScenery();
    void SetDefaultRef(const wxString& prefix, const wxString& theme) {
        m_defaultRef = wxT("../shared/") + prefix + theme + wxT("-texture");
        m_prefix = prefix;
        m_SCN.prefix = prefix;
        m_theme = theme;
        UpdateAll();
        TransferDataToWindow();
    };
    wxString GetOVLPath();

    void UpdateControlState();
};

template<>
inline wxColourHtmlListBox* dlgCreateScenery::listbox<cFlexiTexture>() { return m_htlbTexture; }
template<>
inline wxColourHtmlListBox* dlgCreateScenery::listbox<cModel>() { return m_htlbModel; }
template<>
inline wxColourHtmlListBox* dlgCreateScenery::listbox<cAnimatedModel>() { return m_htlbAModel; }
template<>
inline wxColourHtmlListBox* dlgCreateScenery::listbox<cAnimation>() { return m_htlbAnimation; }
template<>
inline wxColourHtmlListBox* dlgCreateScenery::listbox<cImpSpline>() { return m_htlbSpline; }
template<>
inline wxColourHtmlListBox* dlgCreateScenery::listbox<cReference>() { return m_htlbReferences; }

#endif // WXDLGCREATESCENERY_H_INCLUDED
