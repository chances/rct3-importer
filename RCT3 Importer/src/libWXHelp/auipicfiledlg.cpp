/////////////////////////////////////////////////////////////////////////////
// Name:        auipicfiledlg.cpp
// Purpose:     AUI file dialog with picture preview
// Author:      Tobias Minich
// Modified by:
// Created:     Oct 28 2006
// RCS-ID:
// Copyright:   (c) 2006 Tobias Minich
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx_pch.h"

#include "auipicfiledlg.h"

BEGIN_EVENT_TABLE(wxPicPanel, wxPanel)
    EVT_PAINT(wxPicPanel::OnPaint)
    EVT_SIZE(wxPicPanel::OnSize)
END_EVENT_TABLE()

void wxPicPanel::Create() {
    m_pic = new wxILPicture(this, 0, wxNullBitmap, wxDefaultPosition, wxSize(64,64), wxSIMPLE_BORDER);
    m_pic->SetScale(wxILSCALE_UNIFORM);
    m_pic->SetBackgroundColour(*wxWHITE);
    SetSizeHints(wxSize(70,70));
}

void wxPicPanel::OnPaint(wxPaintEvent &event) {
    wxPaintDC dc(this);
    int w, h;
    GetClientSize(&w,&h);
    dc.DrawText( _T("Test"), 0, w+5 );
}

void wxPicPanel::OnSize(wxSizeEvent &event) {
    int w, h;
    GetClientSize(&w,&h);
    if (w<h)
        m_pic->SetSize(0, (h-w)/2, w, w);
    else
        m_pic->SetSize((w-h)/2, 0, h, h);
}

IMPLEMENT_DYNAMIC_CLASS(wxAUIPicFileDialog, wxAUIFileDialog)

bool wxAUIPicFileDialog::Create( wxWindow *parent,
                              const wxString& message,
                              const wxString& defaultDir,
                              const wxString& defaultFile,
                              const wxString& wildCard,
                              long  style,
                              const wxPoint& pos,
                              const wxSize& sz,
                              const wxString& name) {

    if (!CreatePrepare(parent, message, defaultDir, defaultFile,
                       wildCard, style, pos, sz, name, false))
    {
        return false;
    }

    m_panelMain = new wxPanel(this);
    CreateDefaultControls(m_panelMain, wildCard, false);
    m_mgr.AddPane(m_panelMain, wxAuiPaneInfo().Name(wxT("fileselect")).CenterPane().BestSize(wxSize(600,400)));
    CreateAUIPanes();
    m_mgr.Update();
    CreateFinish(false);
    wxSizeEvent ev(GetSize());
    ev.SetEventObject(this);
    wxPostEvent(this, ev);

    return true;
}


void wxAUIPicFileDialog::HandleFilenameChange() {
    wxString fullname = m_list->GetDir() + wxFILE_SEP_PATH + m_text->GetValue();
    if (wxFileExists(fullname)) {
        m_picFileName->SetLabel(m_text->GetValue());
        wxImage img(fullname);
        if (img.Ok()) {
            wxString det = wxString::Format("%dx%d\n", img.GetWidth(), img.GetHeight());

            m_picDetails->SetLabel(det);
            //m_pic->SetBitmap(wxBitmap(img));
            m_pic->SetFileName(fullname);
        } else {
            m_picDetails->SetLabel(_("Unknown Format\n\n"));
            m_pic->SetBitmap(wxNullBitmap);
        }
        m_pic->Refresh();
    }
}

void wxAUIPicFileDialog::CreateAUIPanes() {
    m_Preview = new wxPanel(this);
    //m_PicPanel = new wxPicPanel(m_Preview);
    wxBoxSizer *bs = new wxBoxSizer(wxVERTICAL);
    m_pic = new wxILPicture(m_Preview, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize(64,64), wxSIMPLE_BORDER);
    m_pic->SetBackgroundColour(*wxWHITE);
    m_pic->SetScale(wxILSCALE_UNIFORM);
    m_pic->SetAlignment(wxALIGN_CENTER);
    bs->Add(m_pic, 1, wxALL | wxSHAPED | wxALIGN_CENTER, 5);

    wxPanel *picdesc = new wxPanel(m_Preview, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER);
    picdesc->SetBackgroundColour(*wxWHITE);
    wxBoxSizer *picdescsizer = new wxBoxSizer(wxVERTICAL);
    m_picFileName = new wxStaticText(picdesc, wxID_ANY, _("None"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER | wxST_NO_AUTORESIZE);
    m_picFileName->SetFont(wxFont(8, wxFONTFAMILY_SWISS, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL));
    picdescsizer->Add(m_picFileName, 0, wxALL | wxEXPAND, 5);
    m_picDetails = new wxStaticText(picdesc, wxID_ANY, _("Unknown Format\n\n"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER | wxST_NO_AUTORESIZE);
    m_picDetails->SetFont(wxFont(7, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
    picdescsizer->Add(m_picDetails, 0, wxLEFT | wxRIGHT | wxBOTTOM | wxEXPAND, 5);
    picdesc->SetSizer(picdescsizer);
    picdescsizer->Fit(picdesc);

    bs->Add(picdesc, 0, wxALL | wxEXPAND | wxALIGN_TOP, 5);
    m_Preview->SetSizer(bs);
    bs->Fit(m_Preview);
    m_mgr.AddPane(m_Preview, wxAuiPaneInfo().Name(wxT("pic")).Right().MinSize(wxSize(100,150)).Caption(_("Preview")).Floatable(false));
}

