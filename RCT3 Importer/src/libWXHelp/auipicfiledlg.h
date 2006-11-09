///////////////////////////////////////////////////////////////////////////////
//
// 3D Utility Library
// AUI file dialog with picture preview
// Copyright (C) 2006 Tobias Minch
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
///////////////////////////////////////////////////////////////////////////////

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
