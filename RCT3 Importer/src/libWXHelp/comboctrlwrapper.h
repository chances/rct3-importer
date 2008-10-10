/////////////////////////////////////////////////////////////////////////////
// Name:        comboctrlwrapper.h
// Purpose:     Wrapper for wxComboCtrl as editor for a wxDataViewCtrl
// Author:      Tobias Minich
// Modified by:
// Created:     Dec 03 2006
// RCS-ID:
// Copyright:   (c) 2006 Tobias Minich
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef COMBOCTRLWRAPPER_H_INCLUDED
#define COMBOCTRLWRAPPER_H_INCLUDED

#include <wx/combo.h>
#include <wx/dataview.h>

class wxComboCtrlWrapper : public wxEvtHandler
{
public:
    // NB: box must be a valid object but not Create()d yet
    wxComboCtrlWrapper( wxWindow *owner,
                               const wxVariant& value,
                               wxDataViewListModel *model,
                               wxComboCtrl *box,
                               unsigned int col, unsigned int row,
                               wxRect cellLabel );

    wxComboCtrl *GetCombo() const { return m_box; }

    void AcceptChangesAndFinish();

protected:
    void OnChar( wxKeyEvent &event );
    void OnKeyUp( wxKeyEvent &event );
    void OnCombo( wxCommandEvent &event );
    void OnKillFocus( wxFocusEvent &event );

    bool AcceptChanges();
    void Finish();

private:
    wxWindow   *m_owner;
    wxComboCtrl            *m_box;
    wxString                m_startValue;
    wxDataViewListModel    *m_model;
    unsigned int                  m_col;
    unsigned int                  m_row;
    bool                    m_finished;
    bool                    m_aboutToFinish;

    DECLARE_EVENT_TABLE()
};


#endif // COMBOCTRLWRAPPER_H_INCLUDED
