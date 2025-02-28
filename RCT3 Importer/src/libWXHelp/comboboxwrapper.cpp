/////////////////////////////////////////////////////////////////////////////
// Name:        comboboxwrapper.cpp
// Purpose:     Wrapper for wxComboCtrl as editor for a wxDataViewCtrl
// Author:      Tobias Minich
// Modified by:
// Created:     Dec 03 2006
// RCS-ID:
// Copyright:   (c) 2006 Tobias Minich
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef WXHELP_BASE

#ifndef DEF_DVC

#include "comboboxwrapper.h"

#include "wrappereditevent.h"

BEGIN_EVENT_TABLE(wxComboBoxWrapper, wxEvtHandler)
    EVT_COMBOBOX       (wxID_ANY, wxComboBoxWrapper::OnCombo)
    EVT_KEY_DOWN       (wxComboBoxWrapper::OnChar)
    EVT_CHAR_HOOK      (wxComboBoxWrapper::OnChar)
    EVT_KILL_FOCUS     (wxComboBoxWrapper::OnKillFocus)
END_EVENT_TABLE()

wxComboBoxWrapper::wxComboBoxWrapper(
                        wxWindow *owner,
                        const wxVariant& value,
                        wxDataViewListModel *model,
                        wxComboBox *box,
                        wxArrayString *strings,
                        unsigned int col, unsigned int row,
                        wxRect rectLabel )
{
    m_owner = owner;
    m_model = model;
    m_row = row;
    m_col = col;
    m_box = box;
    m_finished = false;
    m_aboutToFinish = false;

    m_startValue = value.GetString();

    dynamic_cast<wxScrollHelperNative*>(m_owner->GetParent())->CalcScrolledPosition(
        rectLabel.x, rectLabel.y, &rectLabel.x, &rectLabel.y );

    m_box->Create( owner, wxID_ANY, m_startValue,
                    wxPoint(rectLabel.x-2,rectLabel.y-1),
                    wxSize(rectLabel.width+4,rectLabel.height+2),
                    *strings, wxTE_PROCESS_ENTER);
    if (strings) {
//        m_box->Append(*strings);
        m_box->SetStringSelection(m_startValue);
    }

    m_box->SetFocus();

    m_box->PushEventHandler(this);

    wxCommandEvent evt(wxEVT_WRAPPER, m_box->GetId());
    evt.SetEventObject(this);
    evt.SetInt(wxWRAPPER_START_EDITING);
    ProcessEvent(evt);
}

void wxComboBoxWrapper::AcceptChangesAndFinish()
{
    m_aboutToFinish = true;

    AcceptChanges();

    Finish();
}

void wxComboBoxWrapper::OnChar( wxKeyEvent &event )
{
//::wxMessageBox(wxT("OnChar"), wxT("wxComboBoxWrapper"));
    switch ( event.m_keyCode )
    {
        case WXK_RETURN:
            event.Skip(false);
            event.StopPropagation();
            AcceptChangesAndFinish();
            break;

        case WXK_ESCAPE:
            event.Skip(false);
            event.StopPropagation();
            Finish();
            break;

        default:
            event.Skip();
    }
}

void wxComboBoxWrapper::OnKeyUp( wxKeyEvent &event )
{
//::wxMessageBox(wxT("OnKeyUp"), wxT("wxComboBoxWrapper"));
    if (m_finished)
    {
        event.Skip();
        return;
    }
    // auto-grow the textctrl
    wxSize parentSize = m_owner->GetSize();
    wxPoint myPos = m_box->GetPosition();
    wxSize mySize = m_box->GetSize();
    int sx, sy;
    m_box->GetTextExtent(m_box->GetValue() + wxT("MMMM"), &sx, &sy);
    if (myPos.x + sx > parentSize.x)
        sx = parentSize.x - myPos.x;
    if (mySize.x > sx)
        sx = mySize.x;
    m_box->SetSize(sx, wxDefaultCoord);
    event.Skip();
}

void wxComboBoxWrapper::OnCombo( wxCommandEvent &event )
{
//::wxMessageBox(wxT("OnCombo"));
    if ( !m_finished && !m_aboutToFinish )
    {
        AcceptChanges();
        Finish();
    }
}

void wxComboBoxWrapper::OnKillFocus( wxFocusEvent &event )
{
/*
if (event.GetWindow())
::wxMessageBox(event.GetWindow()->GetClassInfo()->GetClassName(), wxT("OnKillFocus:Window"));
else
::wxMessageBox(wxT("OnKillFocus:NoWindow"));
*/
    if ( !m_finished && !m_aboutToFinish )
    {
        AcceptChanges();
        Finish();
    }

    event.Skip();
}

bool wxComboBoxWrapper::AcceptChanges()
{
    const wxString value = m_box->GetValue();

    if ( value == m_startValue )
        return true;

    wxVariant variant;
    variant = value;
    m_model->SetValue( variant, m_col, m_row );
    m_model->ValueChanged( m_col, m_row );

    return true;
}

void wxComboBoxWrapper::Finish()
{
    if ( !m_finished )
    {
        m_finished = true;

        wxCommandEvent evt(wxEVT_WRAPPER, m_box->GetId());
        evt.SetEventObject(this);
        evt.SetInt(wxWRAPPER_STOP_EDITING);
        ProcessEvent(evt);

        m_box->RemoveEventHandler(this);

        // delete later
        wxPendingDelete.Append( m_box );
        wxPendingDelete.Append( this );
    }
}

#endif

#endif
