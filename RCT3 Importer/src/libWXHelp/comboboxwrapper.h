/////////////////////////////////////////////////////////////////////////////
// Name:        comboboxwrapper.h
// Purpose:     Wrapper for wxComboBox as editor for a wxDataViewCtrl
// Author:      Tobias Minich
// Modified by:
// Created:     Dec 03 2006
// RCS-ID:
// Copyright:   (c) 2006 Tobias Minich
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef COMBOBOXWRAPPER_H_INCLUDED
#define COMBOBOXWRAPPER_H_INCLUDED

#ifndef DEF_DVC

#include <wx/combobox.h>
#include <wx/dataview.h>

class wxComboBoxWrapper : public wxEvtHandler
{
public:
    // NB: box must be a valid object but not Create()d yet
    wxComboBoxWrapper( wxWindow *owner,
                               const wxVariant& value,
                               wxDataViewListModel *model,
                               wxComboBox *box,
                               wxArrayString *strings,
                               unsigned int col, unsigned int row,
                               wxRect cellLabel);

    wxComboBox *GetCombo() const { return m_box; }

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
    wxComboBox             *m_box;
    wxString                m_startValue;
    wxDataViewListModel    *m_model;
    unsigned int                  m_col;
    unsigned int                  m_row;
    bool                    m_finished;
    bool                    m_aboutToFinish;

    DECLARE_EVENT_TABLE()
};

#endif

#endif // COMBOCTRLWRAPPER_H_INCLUDED
