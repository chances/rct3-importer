///////////////////////////////////////////////////////////////////////////////
//
// Light Effect Point Dialog
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

#ifndef WXDLGEFFECTLIGHT_H_INCLUDED
#define WXDLGEFFECTLIGHT_H_INCLUDED

#include "wx_pch.h"

#include <wx/xrc/xmlres.h>

#include <wx/clrpicker.h>

class dlgEffectLight : public wxDialog {
protected:
    wxChoice* m_choiceLightType;
    wxCheckBox* m_checkShines;
    wxCheckBox* m_checkBulb;
    wxChoice* m_choiceLightOption;
    wxChoice* m_choiceColour;
    wxTextCtrl* m_textLightRed;
    wxTextCtrl* m_textLightGreen;
    wxTextCtrl* m_textLightBlue;
    wxTextCtrl* m_textLightRadius;
    wxCheckBox* m_checkCM;
    wxTextCtrl* m_textPreview;
    wxButton* m_btOk;
    wxButton* m_btCancel;

    wxColourPickerCtrl* m_colourPick;
    bool m_doUpdate;
    unsigned int m_offset;
    bool m_colour;

    void OnColourPick(wxColourPickerEvent& event);
    void OnColourEdit(wxCommandEvent& event);

    void UpdateLight();

private:
    void InitWidgetsFromXRC(wxWindow *parent) {
        wxXmlResource::Get()->LoadObject(this,parent,_T("dlgEffectLight"), _T("wxDialog"));
        m_choiceLightType = XRCCTRL(*this,"m_choiceLightType",wxChoice);
        m_checkShines = XRCCTRL(*this,"m_checkShines",wxCheckBox);
        m_checkBulb = XRCCTRL(*this,"m_checkBulb",wxCheckBox);
        m_choiceLightOption = XRCCTRL(*this,"m_choiceLightOption",wxChoice);
        m_choiceColour = XRCCTRL(*this,"m_choiceColour",wxChoice);
        m_textLightRed = XRCCTRL(*this,"m_textLightRed",wxTextCtrl);
        m_textLightGreen = XRCCTRL(*this,"m_textLightGreen",wxTextCtrl);
        m_textLightBlue = XRCCTRL(*this,"m_textLightBlue",wxTextCtrl);
        m_textLightRadius = XRCCTRL(*this,"m_textLightRadius",wxTextCtrl);
        m_checkCM = XRCCTRL(*this,"m_checkCM",wxCheckBox);
        m_textPreview = XRCCTRL(*this,"m_textPreview",wxTextCtrl);
        m_btOk = XRCCTRL(*this,"m_btOk",wxButton);
        m_btCancel = XRCCTRL(*this,"m_btCancel",wxButton);
    }

    DECLARE_EVENT_TABLE()
public:
    dlgEffectLight(wxWindow *parent=NULL, bool colour=false);
    wxString GetLight() {return m_textPreview->GetValue();};
    unsigned int GetOffset() const {return m_offset;};
};

#endif // WXDLGEFFECTLIGHT_H_INCLUDED
