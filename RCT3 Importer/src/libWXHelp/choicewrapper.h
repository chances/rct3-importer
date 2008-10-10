/////////////////////////////////////////////////////////////////////////////
// Name:        choicewrapper.h
// Purpose:     Wrapper for wxChoice as editor for a wxDataViewCtrl
// Author:      Tobias Minich
// Modified by:
// Created:     Dec 03 2006
// RCS-ID:
// Copyright:   (c) 2006 Tobias Minich
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef CHOICEWRAPPER_H_INCLUDED
#define CHOICEWRAPPER_H_INCLUDED

#include <wx/choice.h>
#include <wx/dataview.h>

class wxChoiceWrapper : public wxEvtHandler
{
public:
    // NB: box must be a valid object but not Create()d yet
    wxChoiceWrapper( wxWindow *owner,
                               const wxVariant& value,
                               wxDataViewListModel *model,
                               wxChoice *box,
                               wxArrayString *strings,
                               unsigned int col, unsigned int row,
                               wxRect cellLabel,
                               int align );

    wxChoice *GetCombo() const { return m_box; }

    void AcceptChangesAndFinish();

protected:
    void OnChar( wxKeyEvent &event );
    void OnCombo( wxCommandEvent &event );
    void OnKillFocus( wxFocusEvent &event );

    bool AcceptChanges();
    void Finish();

private:
    wxWindow   *m_owner;
    wxChoice               *m_box;
    wxVariant               m_startValue;
    wxDataViewListModel    *m_model;
    unsigned int                  m_col;
    unsigned int                  m_row;
    bool                    m_finished;
    bool                    m_aboutToFinish;

    DECLARE_EVENT_TABLE()
};


#endif // COMBOCTRLWRAPPER_H_INCLUDED
