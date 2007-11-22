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

#include "wx_pch.h"

#include "wxdlgEffectLight.h"

BEGIN_EVENT_TABLE(dlgEffectLight,wxDialog)
EVT_COLOURPICKER_CHANGED(XRCID("m_colourPick"), dlgEffectLight::OnColourPick)
EVT_TEXT(XRCID("m_textLightRed"), dlgEffectLight::OnColourEdit)
EVT_TEXT(XRCID("m_textLightGreen"), dlgEffectLight::OnColourEdit)
EVT_TEXT(XRCID("m_textLightBlue"), dlgEffectLight::OnColourEdit)
EVT_TEXT(XRCID("m_textLightRadius"), dlgEffectLight::OnColourEdit)
EVT_CHOICE(XRCID("m_choiceLightType"), dlgEffectLight::OnColourEdit)
EVT_CHOICE(XRCID("m_choiceLightOption"), dlgEffectLight::OnColourEdit)
EVT_CHOICE(XRCID("m_choiceColour"), dlgEffectLight::OnColourEdit)
EVT_CHECKBOX(XRCID("m_checkCM"), dlgEffectLight::OnColourEdit)
END_EVENT_TABLE()

dlgEffectLight::dlgEffectLight(wxWindow *parent, bool colour) {
    m_doUpdate = false;
    m_offset = 0;
    m_colour = colour;

    InitWidgetsFromXRC((wxWindow *)parent);

    m_choiceLightType->SetSelection(0);
    m_choiceLightOption->SetSelection(0);
    m_choiceColour->SetSelection(0);

    m_colourPick = new wxColourPickerCtrl(this, XRCID("m_colourPick"), *wxWHITE);
    m_colourPick->SetToolTip(_("Select light colour."));
    wxXmlResource::Get()->AttachUnknownControl(wxT("m_colourPick"), m_colourPick, this);

    Fit();
    Layout();
    GetSizer()->SetSizeHints(this);

    m_btOk->SetId(wxID_OK);
    m_btCancel->SetId(wxID_CANCEL);

    m_doUpdate = true;
    UpdateLight();

    if (m_colour) {
        m_choiceLightOption->Enable(false);
        m_textLightRadius->Enable(false);
        m_checkCM->Enable(false);
    }

}

void dlgEffectLight::OnColourPick(wxColourPickerEvent& event) {
    wxColour col = event.GetColour();
    m_doUpdate = false;
    m_textLightRed->SetValue(wxString::Format("%d", (unsigned int) col.Red()));
    m_textLightBlue->SetValue(wxString::Format("%d", (unsigned int) col.Blue()));
    m_doUpdate = true;
    m_textLightGreen->SetValue(wxString::Format("%d", (unsigned int) col.Green()));
}

void dlgEffectLight::OnColourEdit(wxCommandEvent& WXUNUSED(event)) {
    UpdateLight();
}

void dlgEffectLight::UpdateLight() {
    if (m_doUpdate) {
        unsigned long r, g, b;
        if (!m_textLightRed->GetValue().ToULong(&r))
            r = 0;
        if (!m_textLightGreen->GetValue().ToULong(&g))
            g = 0;
        if (!m_textLightBlue->GetValue().ToULong(&b))
            b = 0;
        wxColour col(r, g, b);
        m_colourPick->SetColour(col);
        int type = m_choiceLightType->GetSelection();
        int option = m_choiceLightOption->GetSelection();
        int ccolour = m_choiceColour->GetSelection();
        m_textLightBlue->Enable(ccolour==0);
        m_textLightRed->Enable(ccolour==0);
        m_textLightGreen->Enable(ccolour==0);
        m_colourPick->Enable(ccolour==0);
        m_textLightRadius->Enable(ccolour==0);
        m_checkCM->Enable(ccolour==0);
        wxString light = wxT("");
        switch (type) {
            case 1:
                m_checkShines->SetValue(false);
                m_checkBulb->SetValue(true);
                light += wxT("simple");
                m_offset = 6;
                break;
            case 2:
                m_checkShines->SetValue(true);
                m_checkBulb->SetValue(false);
                light += wxT("nb");
                m_offset = 2;
                break;
            default:
                m_checkShines->SetValue(true);
                m_checkBulb->SetValue(true);
                m_offset = 0;
                break;
        }
        light += wxT("light");
        if (m_colour)
            light += wxT("colour");
        else
            light += wxT("start");
        light += wxT("01");
        switch (option) {
            case 1:
                light += wxT("_lamp");
                break;
            case 2:
                light += wxT("_torch");
                break;
            case 3:
                light += wxT("_prehistoriclamp");
                break;
            case 4:
                light += wxT("_safarilamp");
                break;
            case 5:
                light += wxT("_lamp_big");
                break;
            case 6:
                light += wxT("_lamp_top");
                break;
            case 7:
                light += wxT("_main");
                break;
        }
        switch (ccolour) {
            case 0:
                light += wxString::Format(wxT("_r%dg%db%d"), col.Red(), col.Green(), col.Blue());
                break;
            case 1:
                light += wxT("_flexi0");
                break;
            case 2:
                light += wxT("_flexi1");
                break;
            case 3:
                light += wxT("_flexi2");
                break;
        }
        if ((!m_colour) && (ccolour < 4)) {
            if (!m_textLightRadius->GetValue().ToULong(&r))
                r = 0;
            light += wxString::Format(wxT("_radius%ld"), r);
            if (m_checkCM->IsChecked())
                light += wxT("cm");
        }
        m_textPreview->SetValue(light);
    }
}
