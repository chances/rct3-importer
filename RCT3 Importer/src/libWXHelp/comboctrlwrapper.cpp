/////////////////////////////////////////////////////////////////////////////
// Name:        comboctrlwrapper.cpp
// Purpose:     Wrapper for wxComboCtrl as editor for a wxDataViewCtrl
// Author:      Tobias Minich
// Modified by:
// Created:     Dec 03 2006
// RCS-ID:
// Copyright:   (c) 2006 Tobias Minich
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "comboctrlwrapper.h"

BEGIN_EVENT_TABLE(wxComboCtrlWrapper, wxEvtHandler)
    EVT_COMBOBOX       (wxID_ANY, wxComboCtrlWrapper::OnCombo)
    EVT_CHAR           (wxComboCtrlWrapper::OnChar)
    EVT_KEY_UP         (wxComboCtrlWrapper::OnKeyUp)
    EVT_KILL_FOCUS     (wxComboCtrlWrapper::OnKillFocus)
END_EVENT_TABLE()

wxComboCtrlWrapper::wxComboCtrlWrapper(
                        wxWindow *owner,
                        const wxVariant& value,
                        wxDataViewListModel *model,
                        wxComboCtrl *box,
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

//    wxVariant value;
//    model->GetValue( value, col, row );
    m_startValue = value.GetString();

    dynamic_cast<wxScrollHelperNative*>(m_owner->GetParent())->CalcScrolledPosition(
        rectLabel.x, rectLabel.y, &rectLabel.x, &rectLabel.y );

    m_box->Create( owner, wxID_ANY, m_startValue,
                    wxPoint(rectLabel.x-2,rectLabel.y-1),
                    wxSize(rectLabel.width+4,rectLabel.height+2) );
    m_box->SetFocus();

    m_box->GetTextCtrl()->PushEventHandler(this);
}

void wxComboCtrlWrapper::AcceptChangesAndFinish()
{
    m_aboutToFinish = true;

    // Notify the owner about the changes
    AcceptChanges();

    // Even if vetoed, close the control (consistent with MSW)
    Finish();
}

void wxComboCtrlWrapper::OnChar( wxKeyEvent &event )
{
::wxMessageBox(wxT("OnChar"));
    switch ( event.m_keyCode )
    {
        case WXK_RETURN:
            AcceptChangesAndFinish();
            break;

        case WXK_ESCAPE:
            // m_owner->OnRenameCancelled( m_itemEdited );
            Finish();
            break;

        default:
            event.Skip();
    }
}

void wxComboCtrlWrapper::OnKeyUp( wxKeyEvent &event )
{
::wxMessageBox(wxT("OnKeyUp"));
    if (m_finished)
    {
        event.Skip();
        return;
    }
/*
    // auto-grow the textctrl
    wxSize parentSize = m_owner->GetSize();
    wxPoint myPos = m_box->GetPosition();
    wxSize mySize = m_box->GetSize();
    int sx, sy;
    m_box->GetTextExtent(m_box->GetValue() + _T("MM"), &sx, &sy);
    if (myPos.x + sx > parentSize.x)
        sx = parentSize.x - myPos.x;
    if (mySize.x > sx)
        sx = mySize.x;
    m_box->SetSize(sx, wxDefaultCoord);
*/
    event.Skip();
}

void wxComboCtrlWrapper::OnCombo( wxCommandEvent &event )
{
::wxMessageBox(wxT("OnCombo"));
    if ( !m_finished && !m_aboutToFinish )
    {
        AcceptChanges();
        Finish();
    }
}

void wxComboCtrlWrapper::OnKillFocus( wxFocusEvent &event )
{
if (event.GetWindow())
::wxMessageBox(event.GetWindow()->GetClassInfo()->GetClassName(), wxT("OnKillFocus:Window"));
else
::wxMessageBox(wxT("OnKillFocus:NoWindow"));
    if ( !m_finished && !m_aboutToFinish )
    {
//        AcceptChanges();
        //if ( !AcceptChanges() )
        //    m_owner->OnRenameCancelled( m_itemEdited );

//        Finish();
    }

    // We must let the native text control handle focus
    event.Skip();
}

bool wxComboCtrlWrapper::AcceptChanges()
{
    const wxString value = m_box->GetValue();

    if ( value == m_startValue )
        // nothing changed, always accept
        return true;

//    if ( !m_owner->OnRenameAccept(m_itemEdited, value) )
        // vetoed by the user
//        return false;

    // accepted, do rename the item
    wxVariant variant;
    variant = value;
    m_model->SetValue( variant, m_col, m_row );
    m_model->ValueChanged( m_col, m_row );

    return true;
}

void wxComboCtrlWrapper::Finish()
{
    if ( !m_finished )
    {
        m_finished = true;

        m_box->RemoveEventHandler(this);
//        m_owner->FinishEditing(m_text);

        // delete later
        wxPendingDelete.Append( m_box );
        wxPendingDelete.Append( this );
    }
}

