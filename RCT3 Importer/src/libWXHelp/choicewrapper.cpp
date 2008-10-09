/////////////////////////////////////////////////////////////////////////////
// Name:        choicewrapper.cpp
// Purpose:     Wrapper for wxChoice as editor for a wxDataViewCtrl
// Author:      Tobias Minich
// Modified by:
// Created:     Dec 03 2006
// RCS-ID:
// Copyright:   (c) 2006 Tobias Minich
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef WXHELP_BASE

#include "choicewrapper.h"

#include "wrappereditevent.h"

BEGIN_EVENT_TABLE(wxChoiceWrapper, wxEvtHandler)
    EVT_CHOICE         (wxID_ANY, wxChoiceWrapper::OnCombo)
    EVT_KEY_DOWN       (wxChoiceWrapper::OnChar)
    EVT_CHAR_HOOK      (wxChoiceWrapper::OnChar)
    EVT_KILL_FOCUS     (wxChoiceWrapper::OnKillFocus)
END_EVENT_TABLE()

wxChoiceWrapper::wxChoiceWrapper(
                        wxWindow *owner,
                        const wxVariant& value,
                        wxDataViewListModel *model,
                        wxChoice *box,
                        wxArrayString *strings,
                        unsigned int col, unsigned int row,
                        wxRect rectLabel,
                        int align )
{
    m_owner = owner;
    m_model = model;
    m_row = row;
    m_col = col;
    m_box = box;
    m_finished = false;
    m_aboutToFinish = false;

    m_startValue = value;

    dynamic_cast<wxScrollHelperNative*>(m_owner->GetParent())->CalcScrolledPosition(
        rectLabel.x, rectLabel.y, &rectLabel.x, &rectLabel.y );

    if (strings && align) {
        wxString longest = wxT("Default");
        for (wxArrayString::iterator it = strings->begin(); it != strings->end(); it++) {
            if (longest.Len() < it->Len())
                longest = *it;
        }
        longest += wxT("MMM");
        wxRect fr = rectLabel;
        wxCoord x, y;
        m_box->GetTextExtent(longest, &x, &y);
        if (x > fr.width)
            fr.width = x;
        if (y > fr.height)
            fr.height = y;
        wxSize parentSize = m_owner->GetSize();
        if (parentSize.GetWidth() < (fr.width+4))
            align = wxALIGN_RIGHT;
        switch (align) {
            case wxALIGN_LEFT: {
                int xm = fr.GetRight();
                if (xm+2 > parentSize.GetWidth())
                    fr.x -= (xm+2 - parentSize.GetWidth());
                break;
            }
            case wxALIGN_CENTER: {
                int xc = fr.x + (fr.width/2);
                int xoc = rectLabel.x + (rectLabel.width/2);
                fr.x += (xoc - xc);
                int xm = fr.GetRight();
                if (xm+2 > parentSize.GetWidth())
                    fr.x -= (xm+2 - parentSize.GetWidth());
                break;
            }
            case wxALIGN_RIGHT: {
                fr.x += (rectLabel.GetRight() - fr.GetRight());
                break;
            }
        }
        rectLabel = fr;
    }

    m_box->Create( owner, wxID_ANY,
                    wxPoint(rectLabel.x-2,rectLabel.y-1),
                    wxSize(rectLabel.width+4,rectLabel.height+2),
                    *strings, wxWANTS_CHARS );
    if (strings) {
//        m_box->Append(*strings);
        if (value.IsType(wxT("long"))) {
            m_box->SetSelection(value.GetLong());
        } else {
            m_box->SetStringSelection(value.GetString());
        }
    }

    m_box->SetFocus();

    m_box->PushEventHandler(this);

    wxCommandEvent evt(wxEVT_WRAPPER, m_box->GetId());
    evt.SetEventObject(this);
    evt.SetInt(wxWRAPPER_START_EDITING);
    ProcessEvent(evt);
}

void wxChoiceWrapper::AcceptChangesAndFinish()
{
    m_aboutToFinish = true;

    // Notify the owner about the changes
    AcceptChanges();

    // Even if vetoed, close the control (consistent with MSW)
    Finish();
}

void wxChoiceWrapper::OnChar( wxKeyEvent &event )
{
//::wxMessageBox(wxT("OnChar"));
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

void wxChoiceWrapper::OnCombo( wxCommandEvent &event )
{
//::wxMessageBox(wxT("OnCombo"));
    if ( !m_finished && !m_aboutToFinish )
    {
        AcceptChanges();
        Finish();
    }
}

void wxChoiceWrapper::OnKillFocus( wxFocusEvent &event )
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

    // We must let the native text control handle focus
    event.Skip();
}

bool wxChoiceWrapper::AcceptChanges()
{
    wxVariant variant;
    if (m_startValue.IsType(wxT("long"))) {
        long val = m_box->GetSelection();
        variant = val;
    } else {
        wxString val = m_box->GetStringSelection();
        variant = val;
    }

    if ( variant == m_startValue )
        return true;

    m_model->SetValue( variant, m_col, m_row );
    m_model->ValueChanged( m_col, m_row );

    return true;
}

void wxChoiceWrapper::Finish()
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
