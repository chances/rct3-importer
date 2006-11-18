/////////////////////////////////////////////////////////////////////////////
// Name:        auipicfiledlg.h
// Purpose:     AUI file dialog with picture preview
// Author:      Tobias Minich
// Modified by:
// Created:     Oct 28 2006
// RCS-ID:
// Copyright:   (c) 2006 Tobias Minich
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef AUIPICFILEDLGG_H_INCLUDED
#define AUIPICFILEDLGG_H_INCLUDED

#include "wx_pch.h"

#include "auifiledlg.h"
#include "ilpict.h"

class wxPicPanel: public wxPanel {
public:
    wxPicPanel(): wxPanel() {Create();};
    wxPicPanel(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL, const wxString& name = "panel"):
            wxPanel(parent, id, pos, size, style, name) {Create();};
private:
    void Create();
    void OnPaint(wxPaintEvent &event);
    void OnSize(wxSizeEvent &event);

    wxILPicture *m_pic;

    DECLARE_EVENT_TABLE()
};

class wxAUIPicFileDialog: public wxAUIFileDialog {
public:
    wxAUIPicFileDialog() : wxAUIFileDialog() {};
    wxAUIPicFileDialog(wxWindow *parent,
                       const wxString& message = wxFileSelectorPromptStr,
                       const wxString& defaultDir = wxEmptyString,
                       const wxString& defaultFile = wxEmptyString,
                       const wxString& wildCard = wxFileSelectorDefaultWildcardStr,
                       long style = wxFD_DEFAULT_STYLE,
                       const wxPoint& pos = wxDefaultPosition,
                       const wxSize& sz = wxDefaultSize,
                       const wxString& name = wxFileDialogNameStr) : wxAUIFileDialog() {
            Create(parent, message, defaultDir, defaultFile, wildCard,
                            style, pos, sz, name); };
    bool Create(wxWindow *parent,
                       const wxString& message = wxFileSelectorPromptStr,
                       const wxString& defaultDir = wxEmptyString,
                       const wxString& defaultFile = wxEmptyString,
                       const wxString& wildCard = wxFileSelectorDefaultWildcardStr,
                       long style = wxFD_DEFAULT_STYLE,
                       const wxPoint& pos = wxDefaultPosition,
                       const wxSize& sz = wxDefaultSize,
                       const wxString& name = wxFileDialogNameStr);
protected:
    virtual void HandleFilenameChange();
    virtual void CreateAUIPanes();

private:
    wxPanel *m_Preview;
    wxILPicture *m_pic;
    wxStaticText *m_picFileName;
    wxStaticText *m_picDetails;

    DECLARE_DYNAMIC_CLASS(wxAUIPicFileDialog)

};


#endif // AUIPICFILEDLGG_H_INCLUDED
