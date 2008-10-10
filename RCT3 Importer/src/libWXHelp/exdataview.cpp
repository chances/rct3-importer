/////////////////////////////////////////////////////////////////////////////
// Name:        exdataview.cpp
// Orig. Name:  src/generic/datavgen.cpp
// Purpose:     Extended generic wxDataViewCtrl
// Orig.Author: Robert Roebling
// Modified by: Tobias Minich
// Id:          $Id: datavgen.cpp,v 1.43 2006/12/10 15:37:31 VZ Exp $
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef WXHELP_BASE

#include "exdataview.h"

#ifdef _WIN32
#include "win_predefine.h"
#endif

#include <wx/dcbuffer.h>
#include <wx/renderer.h>
#include <wx/sizer.h>
#include <wx/timer.h>

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxExDataViewCtrl;

//-----------------------------------------------------------------------------
// wxExDataViewHeaderWindow
//-----------------------------------------------------------------------------

class wxExDataViewHeaderWindow: public wxWindow
{
public:
    wxExDataViewHeaderWindow( wxExDataViewCtrl *parent,
                            wxWindowID id,
                            const wxPoint &pos = wxDefaultPosition,
                            const wxSize &size = wxDefaultSize,
                            const wxString &name = wxT("wxexdataviewctrlheaderwindow") );
    virtual ~wxExDataViewHeaderWindow();

    void SetOwner( wxExDataViewCtrl* owner ) { m_owner = owner; }
    wxExDataViewCtrl *GetOwner() { return m_owner; }

    void OnPaint( wxPaintEvent &event );
    void OnMouse( wxMouseEvent &event );
    void OnSetFocus( wxFocusEvent &event );

private:
    wxExDataViewCtrl      *m_owner;
    wxCursor            *m_resizeCursor;

private:
    DECLARE_DYNAMIC_CLASS(wxDataViewHeaderWindow)
    DECLARE_EVENT_TABLE()
};

//-----------------------------------------------------------------------------
// wxExDataViewRenameTimer
//-----------------------------------------------------------------------------

class wxExDataViewRenameTimer: public wxTimer
{
private:
    wxExDataViewMainWindow *m_owner;

public:
    wxExDataViewRenameTimer( wxExDataViewMainWindow *owner );
    void Notify();
};

//-----------------------------------------------------------------------------
// wxDataViewTextCtrlWrapper: wraps a wxTextCtrl for inline editing
//-----------------------------------------------------------------------------

class wxExDataViewTextCtrlWrapper : public wxEvtHandler
{
public:
    // NB: text must be a valid object but not Create()d yet
    wxExDataViewTextCtrlWrapper( wxExDataViewMainWindow *owner,
                               wxTextCtrl *text,
                               wxDataViewListModel *model,
                               unsigned int col, unsigned int row,
                               wxRect cellLabel );

    wxTextCtrl *GetText() const { return m_text; }

    void AcceptChangesAndFinish();

protected:
    void OnChar( wxKeyEvent &event );
    void OnKeyUp( wxKeyEvent &event );
    void OnKillFocus( wxFocusEvent &event );

    bool AcceptChanges();
    void Finish();

private:
    wxExDataViewMainWindow   *m_owner;
    wxTextCtrl             *m_text;
    wxString                m_startValue;
    wxDataViewListModel    *m_model;
    unsigned int                  m_col;
    unsigned int                  m_row;
    bool                    m_finished;
    bool                    m_aboutToFinish;

    DECLARE_EVENT_TABLE()
};

//-----------------------------------------------------------------------------
// wxDataViewMainWindow
//-----------------------------------------------------------------------------

WX_DEFINE_SORTED_USER_EXPORTED_ARRAY_SIZE_T(unsigned int, wxExDataViewSelection, WXDLLIMPEXP_ADV);

class wxExDataViewMainWindow: public wxWindow
{
public:
    wxExDataViewMainWindow( wxExDataViewCtrl *parent,
                            wxWindowID id,
                            const wxPoint &pos = wxDefaultPosition,
                            const wxSize &size = wxDefaultSize,
                            const wxString &name = wxT("wxexdataviewctrlmainwindow") );
    virtual ~wxExDataViewMainWindow();

    // notifications from wxDataViewListModel
    bool RowAppended();
    bool RowPrepended();
    bool RowInserted( unsigned int before );
    bool RowDeleted( unsigned int row );
    bool RowChanged( unsigned int row );
    bool ValueChanged( unsigned int col, unsigned int row );
    bool RowsReordered( unsigned int *new_order );
    bool Cleared();

    void SetOwner( wxExDataViewCtrl* owner ) { m_owner = owner; }
    wxExDataViewCtrl *GetOwner() { return m_owner; }

    void OnPaint( wxPaintEvent &event );
    void OnArrowChar(unsigned int newCurrent, const wxKeyEvent& event);
    void OnChar( wxKeyEvent &event );
    void OnMouse( wxMouseEvent &event );
    void OnSetFocus( wxFocusEvent &event );
    void OnKillFocus( wxFocusEvent &event );

    void UpdateDisplay();
    void RecalculateDisplay();
    void OnInternalIdle();

    void OnRenameTimer();
    void FinishEditing( wxTextCtrl *text );

    void ScrollWindow( int dx, int dy, const wxRect *rect );

    bool HasCurrentRow() { return m_currentRow != (unsigned int)-1; }
    void ChangeCurrentRow( unsigned int row );

    bool IsSingleSel() const { return !GetParent()->HasFlag(wxDV_MULTIPLE); };
    bool IsEmpty() { return GetRowCount() == 0; }

    int GetCountPerPage();
    int GetEndOfLastCol();
    unsigned int GetFirstVisibleRow();
    unsigned int GetLastVisibleRow();
    unsigned int GetRowCount();

    void SelectAllRows( bool on );
    void SelectRow( unsigned int row, bool on );
    void SelectRows( unsigned int from, unsigned int to, bool on );
    void ReverseRowSelection( unsigned int row );
    bool IsRowSelected( unsigned int row );

    void RefreshRow( unsigned int row );
    void RefreshRows( unsigned int from, unsigned int to );
    void RefreshRowsAfter( unsigned int firstRow );

private:
    wxExDataViewCtrl             *m_owner;
    int                         m_lineHeight;
    bool                        m_dirty;

    wxDataViewColumn           *m_currentCol;
    unsigned int                      m_currentRow;
    wxExDataViewSelection         m_selection;

    wxExDataViewRenameTimer      *m_renameTimer;
    wxExDataViewTextCtrlWrapper  *m_textctrlWrapper;
    bool                        m_lastOnSame;

    bool                        m_hasFocus;

    int                         m_dragCount;
    wxPoint                     m_dragStart;

    // for double click logic
    unsigned int m_lineLastClicked,
           m_lineBeforeLastClicked,
           m_lineSelectSingleOnUp;

private:
    DECLARE_DYNAMIC_CLASS(wxExDataViewMainWindow)
    DECLARE_EVENT_TABLE()
};

// ---------------------------------------------------------
// wxExDataViewListModelNotifier
// ---------------------------------------------------------

class wxExDataViewListModelNotifier: public wxDataViewListModelNotifier
{
public:
    wxExDataViewListModelNotifier( wxExDataViewMainWindow *mainWindow )
        { m_mainWindow = mainWindow; }

    virtual bool RowAppended()
        { return m_mainWindow->RowAppended(); }
    virtual bool RowPrepended()
        { return m_mainWindow->RowPrepended(); }
    virtual bool RowInserted( unsigned int before )
        { return m_mainWindow->RowInserted( before ); }
    virtual bool RowDeleted( unsigned int row )
        { return m_mainWindow->RowDeleted( row ); }
    virtual bool RowChanged( unsigned int row )
        { return m_mainWindow->RowChanged( row ); }
    virtual bool ValueChanged( unsigned int col, unsigned int row )
        { return m_mainWindow->ValueChanged( col, row ); }
    virtual bool RowsReordered( unsigned int *new_order )
        { return m_mainWindow->RowsReordered( new_order ); }
    virtual bool Cleared()
        { return m_mainWindow->Cleared(); }

    wxExDataViewMainWindow    *m_mainWindow;
};

//-----------------------------------------------------------------------------
// wxExDataViewHeaderWindow
//-----------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxExDataViewHeaderWindow, wxWindow)

BEGIN_EVENT_TABLE(wxExDataViewHeaderWindow,wxWindow)
    EVT_PAINT         (wxExDataViewHeaderWindow::OnPaint)
    EVT_MOUSE_EVENTS  (wxExDataViewHeaderWindow::OnMouse)
    EVT_SET_FOCUS     (wxExDataViewHeaderWindow::OnSetFocus)
END_EVENT_TABLE()

wxExDataViewHeaderWindow::wxExDataViewHeaderWindow( wxExDataViewCtrl *parent, wxWindowID id,
     const wxPoint &pos, const wxSize &size, const wxString &name ) :
    wxWindow( parent, id, pos, size, 0, name )
{
    SetOwner( parent );

    m_resizeCursor = new wxCursor( wxCURSOR_SIZEWE );

    wxVisualAttributes attr = wxPanel::GetClassDefaultAttributes();
    SetBackgroundStyle( wxBG_STYLE_CUSTOM );
    SetOwnForegroundColour( attr.colFg );
    SetOwnBackgroundColour( attr.colBg );
    if (!m_hasFont)
        SetOwnFont( attr.font );
}

wxExDataViewHeaderWindow::~wxExDataViewHeaderWindow()
{
    delete m_resizeCursor;
}

void wxExDataViewHeaderWindow::OnPaint( wxPaintEvent &WXUNUSED(event) )
{
    int w, h;
    GetClientSize( &w, &h );

    wxAutoBufferedPaintDC dc( this );

    dc.SetBackground(GetBackgroundColour());
    dc.Clear();

    int xpix;
    m_owner->GetScrollPixelsPerUnit( &xpix, NULL );

    int x;
    m_owner->GetViewStart( &x, NULL );

    // account for the horz scrollbar offset
    dc.SetDeviceOrigin( -x * xpix, 0 );

    dc.SetFont( GetFont() );

    unsigned int cols = GetOwner()->GetNumberOfColumns();
    unsigned int i;
    int xpos = 0;
    for (i = 0; i < cols; i++)
    {
        wxDataViewColumn *col = GetOwner()->GetColumn( i );
        int width = col->GetWidth();

        int cw = width;
        int ch = h;

        wxRendererNative::Get().DrawHeaderButton
                                (
                                    this,
                                    dc,
                                    wxRect(xpos, 0, cw, ch-1),
                                    m_parent->IsEnabled() ? 0
                                                          : (int)wxCONTROL_DISABLED
                                );

        dc.DrawText( col->GetTitle(), xpos+3, 3 );

        xpos += width;
    }
}

void wxExDataViewHeaderWindow::OnMouse( wxMouseEvent &WXUNUSED(event) )
{
}

void wxExDataViewHeaderWindow::OnSetFocus( wxFocusEvent &event )
{
    GetParent()->SetFocus();
    event.Skip();
}

//-----------------------------------------------------------------------------
// wxExDataViewRenameTimer
//-----------------------------------------------------------------------------

wxExDataViewRenameTimer::wxExDataViewRenameTimer( wxExDataViewMainWindow *owner )
{
    m_owner = owner;
}

void wxExDataViewRenameTimer::Notify()
{
    m_owner->OnRenameTimer();
}

//-----------------------------------------------------------------------------
// wxExDataViewTextCtrlWrapper: wraps a wxTextCtrl for inline editing
//-----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxExDataViewTextCtrlWrapper, wxEvtHandler)
    EVT_CHAR           (wxExDataViewTextCtrlWrapper::OnChar)
    EVT_KEY_UP         (wxExDataViewTextCtrlWrapper::OnKeyUp)
    EVT_KILL_FOCUS     (wxExDataViewTextCtrlWrapper::OnKillFocus)
END_EVENT_TABLE()

wxExDataViewTextCtrlWrapper::wxExDataViewTextCtrlWrapper(
                        wxExDataViewMainWindow *owner,
                        wxTextCtrl *text,
                        wxDataViewListModel *model,
                        unsigned int col, unsigned int row,
                        wxRect rectLabel )
{
    m_owner = owner;
    m_model = model;
    m_row = row;
    m_col = col;
    m_text = text;

    m_finished = false;
    m_aboutToFinish = false;

    wxVariant value;
    model->GetValue( value, col, row );
    m_startValue = value.GetString();

    m_owner->GetOwner()->CalcScrolledPosition(
        rectLabel.x, rectLabel.y, &rectLabel.x, &rectLabel.y );

    m_text->Create( owner, wxID_ANY, m_startValue,
                    wxPoint(rectLabel.x-2,rectLabel.y-2),
                    wxSize(rectLabel.width+7,rectLabel.height+4) );
    m_text->SetFocus();

    m_text->PushEventHandler(this);
}

void wxExDataViewTextCtrlWrapper::AcceptChangesAndFinish()
{
    m_aboutToFinish = true;

    // Notify the owner about the changes
    AcceptChanges();

    // Even if vetoed, close the control (consistent with MSW)
    Finish();
}

void wxExDataViewTextCtrlWrapper::OnChar( wxKeyEvent &event )
{
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

void wxExDataViewTextCtrlWrapper::OnKeyUp( wxKeyEvent &event )
{
    if (m_finished)
    {
        event.Skip();
        return;
    }

    // auto-grow the textctrl
    wxSize parentSize = m_owner->GetSize();
    wxPoint myPos = m_text->GetPosition();
    wxSize mySize = m_text->GetSize();
    int sx, sy;
    m_text->GetTextExtent(m_text->GetValue() + _T("MM"), &sx, &sy);
    if (myPos.x + sx > parentSize.x)
        sx = parentSize.x - myPos.x;
    if (mySize.x > sx)
        sx = mySize.x;
    m_text->SetSize(sx, wxDefaultCoord);

    event.Skip();
}

void wxExDataViewTextCtrlWrapper::OnKillFocus( wxFocusEvent &event )
{
    if ( !m_finished && !m_aboutToFinish )
    {
        AcceptChanges();
        //if ( !AcceptChanges() )
        //    m_owner->OnRenameCancelled( m_itemEdited );

        Finish();
    }

    // We must let the native text control handle focus
    event.Skip();
}

bool wxExDataViewTextCtrlWrapper::AcceptChanges()
{
    const wxString value = m_text->GetValue();

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

void wxExDataViewTextCtrlWrapper::Finish()
{
    if ( !m_finished )
    {
        m_finished = true;

        m_text->RemoveEventHandler(this);
        m_owner->FinishEditing(m_text);

        // delete later
        wxPendingDelete.Append( this );
    }
}

//-----------------------------------------------------------------------------
// wxDataViewMainWindow
//-----------------------------------------------------------------------------

int wxExDataViewSelectionCmp( unsigned int row1, unsigned int row2 )
{
    if (row1 > row2) return 1;
    if (row1 == row2) return 0;
    return -1;
}


IMPLEMENT_ABSTRACT_CLASS(wxExDataViewMainWindow, wxWindow)

BEGIN_EVENT_TABLE(wxExDataViewMainWindow,wxWindow)
    EVT_PAINT         (wxExDataViewMainWindow::OnPaint)
    EVT_MOUSE_EVENTS  (wxExDataViewMainWindow::OnMouse)
    EVT_SET_FOCUS     (wxExDataViewMainWindow::OnSetFocus)
    EVT_KILL_FOCUS    (wxExDataViewMainWindow::OnKillFocus)
    EVT_CHAR          (wxExDataViewMainWindow::OnChar)
END_EVENT_TABLE()

wxExDataViewMainWindow::wxExDataViewMainWindow( wxExDataViewCtrl *parent, wxWindowID id,
    const wxPoint &pos, const wxSize &size, const wxString &name ) :
    wxWindow( parent, id, pos, size, wxWANTS_CHARS, name ),
    m_selection( wxExDataViewSelectionCmp )

{
    SetOwner( parent );

    m_lastOnSame = false;
    m_renameTimer = new wxExDataViewRenameTimer( this );
    m_textctrlWrapper = NULL;

    // TODO: user better initial values/nothing selected
    m_currentCol = NULL;
    m_currentRow = 0;

    // TODO: we need to calculate this smartly
    m_lineHeight = 20;

    m_dragCount = 0;
    m_dragStart = wxPoint(0,0);
    m_lineLastClicked = (unsigned int) -1;
    m_lineBeforeLastClicked = (unsigned int) -1;
    m_lineSelectSingleOnUp = (unsigned int) -1;

    m_hasFocus = false;

    SetBackgroundStyle( wxBG_STYLE_CUSTOM );
    SetBackgroundColour( *wxWHITE );

    UpdateDisplay();
}

wxExDataViewMainWindow::~wxExDataViewMainWindow()
{
    delete m_renameTimer;
}

void wxExDataViewMainWindow::OnRenameTimer()
{
    // We have to call this here because changes may just have
    // been made and no screen update taken place.
    if ( m_dirty )
        wxSafeYield();


    int xpos = 0;
    unsigned int cols = GetOwner()->GetNumberOfColumns();
    unsigned int i;
    for (i = 0; i < cols; i++)
    {
        wxDataViewColumn *c = GetOwner()->GetColumn( i );
        if (c == m_currentCol)
            break;
        xpos += c->GetWidth();
    }
    wxRect labelRect( xpos, m_currentRow * m_lineHeight, m_currentCol->GetWidth(), m_lineHeight );

    wxClassInfo *textControlClass = CLASSINFO(wxTextCtrl);

    wxTextCtrl * const text = (wxTextCtrl *)textControlClass->CreateObject();
    m_textctrlWrapper = new wxExDataViewTextCtrlWrapper(this, text, GetOwner()->GetModel(),
        m_currentCol->GetModelColumn(), m_currentRow, labelRect );
}

void wxExDataViewMainWindow::FinishEditing( wxTextCtrl *text )
{
    delete text;
    m_textctrlWrapper = NULL;
    SetFocus();
  //  SetFocusIgnoringChildren();
}

bool wxExDataViewMainWindow::RowAppended()
{
    return false;
}

bool wxExDataViewMainWindow::RowPrepended()
{
    return false;
}

bool wxExDataViewMainWindow::RowInserted( unsigned int WXUNUSED(before) )
{
    return false;
}

bool wxExDataViewMainWindow::RowDeleted( unsigned int WXUNUSED(row) )
{
    return false;
}

bool wxExDataViewMainWindow::RowChanged( unsigned int row )
{
    wxRect rect( 0, row*m_lineHeight, 10000, m_lineHeight );
    m_owner->CalcScrolledPosition( rect.x, rect.y, &rect.x, &rect.y );
    Refresh( true, &rect );

    return true;
}

bool wxExDataViewMainWindow::ValueChanged( unsigned int WXUNUSED(col), unsigned int row )
{
    wxRect rect( 0, row*m_lineHeight, 10000, m_lineHeight );
    m_owner->CalcScrolledPosition( rect.x, rect.y, &rect.x, &rect.y );
    Refresh( true, &rect );

    return true;
}

bool wxExDataViewMainWindow::RowsReordered( unsigned int *WXUNUSED(new_order) )
{
    Refresh();

    return true;
}

bool wxExDataViewMainWindow::Cleared()
{
    return false;
}

void wxExDataViewMainWindow::UpdateDisplay()
{
    m_dirty = true;
}

void wxExDataViewMainWindow::OnInternalIdle()
{
    wxWindow::OnInternalIdle();

    if (m_dirty)
    {
        RecalculateDisplay();
        m_dirty = false;
    }
}

void wxExDataViewMainWindow::RecalculateDisplay()
{
    wxDataViewListModel *model = GetOwner()->GetModel();
    if (!model)
    {
        Refresh();
        return;
    }

    int width = 0;
    unsigned int cols = GetOwner()->GetNumberOfColumns();
    unsigned int i;
    for (i = 0; i < cols; i++)
    {
        wxDataViewColumn *col = GetOwner()->GetColumn( i );
        width += col->GetWidth();
    }

    int height = model->GetNumberOfRows() * m_lineHeight;

    SetVirtualSize( width, height );
    GetOwner()->SetScrollRate( 10, m_lineHeight );

    Refresh();
}

void wxExDataViewMainWindow::ScrollWindow( int dx, int dy, const wxRect *rect )
{
    wxWindow::ScrollWindow( dx, dy, rect );
    GetOwner()->m_headerArea->ScrollWindow( dx, 0 );
}

void wxExDataViewMainWindow::OnPaint( wxPaintEvent &WXUNUSED(event) )
{
    wxAutoBufferedPaintDC dc( this );

    dc.SetBackground(GetBackgroundColour());
    dc.Clear();

    GetOwner()->PrepareDC( dc );

    dc.SetFont( GetFont() );

    wxRect update = GetUpdateRegion().GetBox();
    m_owner->CalcUnscrolledPosition( update.x, update.y, &update.x, &update.y );

    wxDataViewListModel *model = GetOwner()->GetModel();

    unsigned int item_start = wxMax( 0, (update.y / m_lineHeight) );
    unsigned int item_count = wxMin( (int)(((update.y + update.height) / m_lineHeight) - item_start + 1),
                               (int)(model->GetNumberOfRows()-item_start) );



    unsigned int item;
    for (item = item_start; item < item_start+item_count; item++)
    {
        wxRect rect( 0, item*m_lineHeight, GetEndOfLastCol(), m_lineHeight );
        if (GetOwner()->m_Striped && ((item % 2) == 1)) {
            wxBrush br = dc.GetBrush();
            wxPen pen = dc.GetPen();
            dc.SetBrush(wxBrush(wxColour(wxT("#FFFFDD"))));
            dc.SetPen(*wxTRANSPARENT_PEN);
            dc.DrawRectangle(rect);
            dc.SetBrush(br);
            dc.SetPen(pen);
        }
        if (!GetOwner()->m_NoSelect) {
            rect.Inflate(0,-2);
            if (m_selection.Index( item ) != wxNOT_FOUND)
            {
                int flags = wxCONTROL_SELECTED;
                if (item == m_currentRow)
                    flags |= wxCONTROL_CURRENT;
                if (m_hasFocus)
                    flags |= wxCONTROL_FOCUSED;

                wxRendererNative::Get().DrawItemSelectionRect
                                    (
                                        this,
                                        dc,
                                        rect,
                                        flags
                                    );
            }
            else
            {
                if (item == m_currentRow)
                {
                    int flags = wxCONTROL_CURRENT;
                    if (m_hasFocus)
                        flags |= wxCONTROL_FOCUSED;  // should have no effect
                    wxRendererNative::Get().DrawItemSelectionRect
                                    (
                                        this,
                                        dc,
                                        rect,
                                        flags
                                    );

                }
            }
        }
    }

    wxRect cell_rect;
    cell_rect.x = 0;
    cell_rect.height = m_lineHeight;
    unsigned int cols = GetOwner()->GetNumberOfColumns();
    unsigned int i;
    for (i = 0; i < cols; i++)
    {
        wxDataViewColumn *col = GetOwner()->GetColumn( i );
        wxDataViewRenderer *cell = col->GetRenderer();
        cell_rect.width = col->GetWidth();

        for (item = item_start; item < item_start+item_count; item++)
        {
            cell_rect.y = item*m_lineHeight;
            wxVariant value;
            model->GetValue( value, col->GetModelColumn(), item );
            cell->SetValue( value );
            wxSize size = cell->GetSize();
            // cannot be bigger than allocated space
            size.x = wxMin( size.x, cell_rect.width );
            size.y = wxMin( size.y, cell_rect.height );
            // TODO: check for left/right/centre alignment here
            wxRect item_rect;
            // for now: centre
            item_rect.x = cell_rect.x + (cell_rect.width / 2) - (size.x / 2);
            item_rect.y = cell_rect.y + (cell_rect.height / 2) - (size.y / 2);

            item_rect.width = size.x;
            item_rect.height= size.y;

            int state = 0;
            if (m_selection.Index( item ) != wxNOT_FOUND)
                state |= wxDATAVIEW_CELL_SELECTED;
            if (item == m_currentRow)
                state |= wxDATAVIEW_CELL_FOCUSED;
            cell->Render( item_rect, &dc, state );
        }

        cell_rect.x += cell_rect.width;
    }
}

int wxExDataViewMainWindow::GetCountPerPage()
{
    wxSize size = GetClientSize();
    return size.y / m_lineHeight;
}

int wxExDataViewMainWindow::GetEndOfLastCol()
{
    int width = 0;
    unsigned int i;
    for (i = 0; i < GetOwner()->GetNumberOfColumns(); i++)
    {
        wxDataViewColumn *c = GetOwner()->GetColumn( i );
        width += c->GetWidth();
    }
    return width;
}

unsigned int wxExDataViewMainWindow::GetFirstVisibleRow()
{
    int x = 0;
    int y = 0;
    m_owner->CalcUnscrolledPosition( x, y, &x, &y );

    return y / m_lineHeight;
}

unsigned int wxExDataViewMainWindow::GetLastVisibleRow()
{
    wxSize client_size = GetClientSize();
    m_owner->CalcUnscrolledPosition( client_size.x, client_size.y, &client_size.x, &client_size.y );

    return wxMin( GetRowCount()-1, ((unsigned)client_size.y/m_lineHeight)+1 );
}

unsigned int wxExDataViewMainWindow::GetRowCount()
{
    return GetOwner()->GetModel()->GetNumberOfRows();
}

void wxExDataViewMainWindow::ChangeCurrentRow( unsigned int row )
{
    m_currentRow = row;

    // send event
}

void wxExDataViewMainWindow::SelectAllRows( bool on )
{
    if (IsEmpty())
        return;

    if (on)
    {
        m_selection.Clear();
        for (unsigned int i = 0; i < GetRowCount(); i++)
            m_selection.Add( i );
        Refresh();
    }
    else
    {
        unsigned int first_visible = GetFirstVisibleRow();
        unsigned int last_visible = GetLastVisibleRow();
        unsigned int i;
        for (i = 0; i < m_selection.GetCount(); i++)
        {
            unsigned int row = m_selection[i];
            if ((row >= first_visible) && (row <= last_visible))
                RefreshRow( row );
        }
        m_selection.Clear();
    }
}

void wxExDataViewMainWindow::SelectRow( unsigned int row, bool on )
{
    if (m_selection.Index( row ) == wxNOT_FOUND)
    {
        if (on)
        {
            m_selection.Add( row );
            RefreshRow( row );
        }
    }
    else
    {
        if (!on)
        {
            m_selection.Remove( row );
            RefreshRow( row );
        }
    }
}

void wxExDataViewMainWindow::SelectRows( unsigned int from, unsigned int to, bool on )
{
    if (from > to)
    {
        unsigned int tmp = from;
        from = to;
        to = tmp;
    }

    unsigned int i;
    for (i = from; i <= to; i++)
    {
        if (m_selection.Index( i ) == wxNOT_FOUND)
        {
            if (on)
                m_selection.Add( i );
        }
        else
        {
            if (!on)
                m_selection.Remove( i );
        }
    }
    RefreshRows( from, to );
}

void wxExDataViewMainWindow::ReverseRowSelection( unsigned int row )
{
    if (m_selection.Index( row ) == wxNOT_FOUND)
        m_selection.Add( row );
    else
        m_selection.Remove( row );
    RefreshRow( row );
}

bool wxExDataViewMainWindow::IsRowSelected( unsigned int row )
{
    return (m_selection.Index( row ) != wxNOT_FOUND);
}

void wxExDataViewMainWindow::RefreshRow( unsigned int row )
{
    wxRect rect( 0, row*m_lineHeight, GetEndOfLastCol(), m_lineHeight );
    m_owner->CalcScrolledPosition( rect.x, rect.y, &rect.x, &rect.y );

    wxSize client_size = GetClientSize();
    wxRect client_rect( 0, 0, client_size.x, client_size.y );
    wxRect intersect_rect = client_rect.Intersect( rect );
    if (intersect_rect.width > 0)
        Refresh( true, &intersect_rect );
}

void wxExDataViewMainWindow::RefreshRows( unsigned int from, unsigned int to )
{
    if (from > to)
    {
        unsigned int tmp = to;
        to = from;
        from = tmp;
    }

    wxRect rect( 0, from*m_lineHeight, GetEndOfLastCol(), (to-from+1) * m_lineHeight );
    m_owner->CalcScrolledPosition( rect.x, rect.y, &rect.x, &rect.y );

    wxSize client_size = GetClientSize();
    wxRect client_rect( 0, 0, client_size.x, client_size.y );
    wxRect intersect_rect = client_rect.Intersect( rect );
    if (intersect_rect.width > 0)
        Refresh( true, &intersect_rect );
}

void wxExDataViewMainWindow::RefreshRowsAfter( unsigned int firstRow )
{
    unsigned int count = GetRowCount();
    if (firstRow > count)
        return;

    wxRect rect( 0, firstRow*m_lineHeight, GetEndOfLastCol(), count * m_lineHeight );
    m_owner->CalcScrolledPosition( rect.x, rect.y, &rect.x, &rect.y );

    wxSize client_size = GetClientSize();
    wxRect client_rect( 0, 0, client_size.x, client_size.y );
    wxRect intersect_rect = client_rect.Intersect( rect );
    if (intersect_rect.width > 0)
        Refresh( true, &intersect_rect );
}

void wxExDataViewMainWindow::OnArrowChar(unsigned int newCurrent, const wxKeyEvent& event)
{
    wxCHECK_RET( newCurrent < GetRowCount(),
                 _T("invalid item index in OnArrowChar()") );

    // if there is no selection, we cannot move it anywhere
    if (!HasCurrentRow())
        return;

    unsigned int oldCurrent = m_currentRow;

    // in single selection we just ignore Shift as we can't select several
    // items anyhow
    if ( event.ShiftDown() && !IsSingleSel() )
    {
        RefreshRow( oldCurrent );

        ChangeCurrentRow( newCurrent );

        // select all the items between the old and the new one
        if ( oldCurrent > newCurrent )
        {
            newCurrent = oldCurrent;
            oldCurrent = m_currentRow;
        }

        SelectRows( oldCurrent, newCurrent, true );
    }
    else // !shift
    {
        RefreshRow( oldCurrent );

        // all previously selected items are unselected unless ctrl is held
        if ( !event.ControlDown() )
            SelectAllRows(false);

        ChangeCurrentRow( newCurrent );

        if ( !event.ControlDown() )
            SelectRow( m_currentRow, true );
        else
            RefreshRow( m_currentRow );
    }

    // MoveToFocus();
}

void wxExDataViewMainWindow::OnChar( wxKeyEvent &event )
{
    if (event.GetKeyCode() == WXK_TAB)
    {
        wxNavigationKeyEvent nevent;
        nevent.SetWindowChange( event.ControlDown() );
        nevent.SetDirection( !event.ShiftDown() );
        nevent.SetEventObject( GetParent()->GetParent() );
        nevent.SetCurrentFocus( m_parent );
        if (GetParent()->GetParent()->GetEventHandler()->ProcessEvent( nevent ))
            return;
    }

    // no item -> nothing to do
    if (!HasCurrentRow())
    {
        event.Skip();
        return;
    }

    // don't use m_linesPerPage directly as it might not be computed yet
    const int pageSize = GetCountPerPage();
    wxCHECK_RET( pageSize, _T("should have non zero page size") );

    switch ( event.GetKeyCode() )
    {
        case WXK_UP:
            if ( m_currentRow > 0 )
                OnArrowChar( m_currentRow - 1, event );
            break;

        case WXK_DOWN:
            if ( m_currentRow < GetRowCount() - 1 )
                OnArrowChar( m_currentRow + 1, event );
            break;

        case WXK_END:
            if (!IsEmpty())
                OnArrowChar( GetRowCount() - 1, event );
            break;

        case WXK_HOME:
            if (!IsEmpty())
                OnArrowChar( 0, event );
            break;

        case WXK_PAGEUP:
            {
                int steps = pageSize - 1;
                int index = m_currentRow - steps;
                if (index < 0)
                    index = 0;

                OnArrowChar( index, event );
            }
            break;

        case WXK_PAGEDOWN:
            {
                int steps = pageSize - 1;
                unsigned int index = m_currentRow + steps;
                unsigned int count = GetRowCount();
                if ( index >= count )
                    index = count - 1;

                OnArrowChar( index, event );
            }
            break;

        default:
            event.Skip();
    }
}

void wxExDataViewMainWindow::OnMouse( wxMouseEvent &event )
{
    if (event.GetEventType() == wxEVT_MOUSEWHEEL)
    {
        // let the base handle mouse wheel events.
        event.Skip();
        return;
    }

    int x = event.GetX();
    int y = event.GetY();
    m_owner->CalcUnscrolledPosition( x, y, &x, &y );

    wxDataViewColumn *col = NULL;

    int xpos = 0;
    unsigned int cols = GetOwner()->GetNumberOfColumns();
    unsigned int i;
    for (i = 0; i < cols; i++)
    {
        wxDataViewColumn *c = GetOwner()->GetColumn( i );
        if (x < xpos + c->GetWidth())
        {
            col = c;
            break;
        }
        xpos += c->GetWidth();
    }
    if (!col)
        return;
    wxDataViewRenderer *cell = col->GetRenderer();

    unsigned int current = y / m_lineHeight;

    if ((current > GetRowCount()) || (x > GetEndOfLastCol()))
    {
        // Unselect all if below the last row ?
        return;
    }

    wxDataViewListModel *model = GetOwner()->GetModel();

    if (event.Dragging())
    {
        if (m_dragCount == 0)
        {
            // we have to report the raw, physical coords as we want to be
            // able to call HitTest(event.m_pointDrag) from the user code to
            // get the item being dragged
            m_dragStart = event.GetPosition();
        }

        m_dragCount++;

        if (m_dragCount != 3)
            return;

        if (event.LeftIsDown())
        {
            // Notify cell about drag
        }
        return;
    }
    else
    {
        m_dragCount = 0;
    }

    bool forceClick = false;

    if (event.ButtonDClick())
    {
        m_renameTimer->Stop();
        m_lastOnSame = false;
    }

    if (event.LeftDClick())
    {
        if ( current == m_lineLastClicked )
        {
            if (cell->GetMode() == wxDATAVIEW_CELL_ACTIVATABLE)
            {
                if (current >= model->GetNumberOfRows())
                    return;
                wxVariant value;
                model->GetValue( value, col->GetModelColumn(), current );
                cell->SetValue( value );
                wxRect cell_rect( xpos, current * m_lineHeight, col->GetWidth(), m_lineHeight );
                cell->Activate( cell_rect, model, col->GetModelColumn(), current );
            }
            return;
        }
        else
        {
            // The first click was on another item, so don't interpret this as
            // a double click, but as a simple click instead
            forceClick = true;
        }
    }

    if (event.LeftUp())
    {
        if (m_lineSelectSingleOnUp != (unsigned int)-1)
        {
            // select single line
            SelectAllRows( false );
            SelectRow( m_lineSelectSingleOnUp, true );
        }

        if (m_lastOnSame)
        {
            if ((col == m_currentCol) && (current == m_currentRow) &&
                (cell->GetMode() == wxDATAVIEW_CELL_EDITABLE) )
            {
                m_renameTimer->Start( 100, true );
            }
        }

        m_lastOnSame = false;
        m_lineSelectSingleOnUp = (unsigned int)-1;
    }
    else
    {
        // This is necessary, because after a DnD operation in
        // from and to ourself, the up event is swallowed by the
        // DnD code. So on next non-up event (which means here and
        // now) m_lineSelectSingleOnUp should be reset.
        m_lineSelectSingleOnUp = (unsigned int)-1;
    }

    if (event.RightDown())
    {
        m_lineBeforeLastClicked = m_lineLastClicked;
        m_lineLastClicked = current;

        // If the item is already selected, do not update the selection.
        // Multi-selections should not be cleared if a selected item is clicked.
        if (!IsRowSelected(current))
        {
            SelectAllRows(false);
            ChangeCurrentRow(current);
            SelectRow(m_currentRow,true);
        }

        // notify cell about right click
        // cell->...

        // Allow generation of context menu event
        event.Skip();
    }
    else if (event.MiddleDown())
    {
        // notify cell about middle click
        // cell->...
    }
    if (event.LeftDown() || forceClick)
    {
#ifdef __WXMSW__
        SetFocus();
#endif

        m_lineBeforeLastClicked = m_lineLastClicked;
        m_lineLastClicked = current;

        unsigned int oldCurrentRow = m_currentRow;
        bool oldWasSelected = IsRowSelected(m_currentRow);

        bool cmdModifierDown = event.CmdDown();
        if ( IsSingleSel() || !(cmdModifierDown || event.ShiftDown()) )
        {
            if ( IsSingleSel() || !IsRowSelected(current) )
            {
                SelectAllRows( false );

                ChangeCurrentRow(current);

                SelectRow(m_currentRow,true);
            }
            else // multi sel & current is highlighted & no mod keys
            {
                m_lineSelectSingleOnUp = current;
                ChangeCurrentRow(current); // change focus
            }
        }
        else // multi sel & either ctrl or shift is down
        {
            if (cmdModifierDown)
            {
                ChangeCurrentRow(current);

                ReverseRowSelection(m_currentRow);
            }
            else if (event.ShiftDown())
            {
                ChangeCurrentRow(current);

                unsigned int lineFrom = oldCurrentRow,
                       lineTo = current;

                if ( lineTo < lineFrom )
                {
                    lineTo = lineFrom;
                    lineFrom = m_currentRow;
                }

                SelectRows(lineFrom, lineTo, true);
            }
            else // !ctrl, !shift
            {
                // test in the enclosing if should make it impossible
                wxFAIL_MSG( _T("how did we get here?") );
            }
        }

        if (m_currentRow != oldCurrentRow)
            RefreshRow( oldCurrentRow );

        wxDataViewColumn *oldCurrentCol = m_currentCol;

        // Update selection here...
        m_currentCol = col;

        m_lastOnSame = !forceClick && ((col == oldCurrentCol) && (current == oldCurrentRow)) && oldWasSelected;

        // Activate if single activation style set
        if ((GetOwner()->m_SingleActivate) && (cell->GetMode() == wxDATAVIEW_CELL_ACTIVATABLE))
        {
            if (m_lineLastClicked >= model->GetNumberOfRows())
                return;
            wxVariant value;
            model->GetValue( value, col->GetModelColumn(), m_lineLastClicked );
            cell->SetValue( value );
            wxRect cell_rect( xpos, m_lineLastClicked * m_lineHeight, col->GetWidth(), m_lineHeight );
            cell->Activate( cell_rect, model, col->GetModelColumn(), m_lineLastClicked );
        }

        // Send LeftClick to Cell
        if (m_lineLastClicked >= model->GetNumberOfRows())
            return;
        wxVariant value;
        model->GetValue( value, col->GetModelColumn(), m_lineLastClicked );
        cell->SetValue( value );
        wxRect cell_rect( xpos, m_lineLastClicked * m_lineHeight, col->GetWidth(), m_lineHeight );
        cell->LeftClick( event.GetPosition(), cell_rect, model, col->GetModelColumn(), m_lineLastClicked );
    }
}

void wxExDataViewMainWindow::OnSetFocus( wxFocusEvent &event )
{
    m_hasFocus = true;

    if (HasCurrentRow())
        Refresh();

    event.Skip();
}

void wxExDataViewMainWindow::OnKillFocus( wxFocusEvent &event )
{
    m_hasFocus = false;

    if (HasCurrentRow())
        Refresh();

    event.Skip();
}

//-----------------------------------------------------------------------------
// wxExDataViewCtrl
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxExDataViewCtrl, wxDataViewCtrlBase)

BEGIN_EVENT_TABLE(wxExDataViewCtrl, wxDataViewCtrlBase)
    EVT_SIZE(wxExDataViewCtrl::OnSize)
END_EVENT_TABLE()

wxExDataViewCtrl::~wxExDataViewCtrl()
{
    if (m_notifier)
        GetModel()->RemoveNotifier( m_notifier );
}

void wxExDataViewCtrl::Init()
{
    m_notifier = NULL;
    m_Striped = false;
    m_SingleActivate = false;
    m_NoSelect = false;
}

bool wxExDataViewCtrl::Create(wxWindow *parent, wxWindowID id,
           const wxPoint& pos, const wxSize& size,
           long style, const wxValidator& validator )
{
    if (!wxControl::Create( parent, id, pos, size, (style&(!wxEDV_ALL)) | wxScrolledWindowStyle|wxSUNKEN_BORDER, validator))
        return false;

    Init();

    m_Striped = (style & wxEDV_STRIPED);
    m_SingleActivate = (style & wxEDV_SINGLE_ACTIVATE);
    m_NoSelect = (style & wxEDV_NO_SELECT);

#ifdef __WXMAC__
    MacSetClipChildren( true ) ;
#endif

    m_clientArea = new wxExDataViewMainWindow( this, wxID_ANY );
#ifdef __WXMSW__
    m_headerArea = new wxExDataViewHeaderWindow( this, wxID_ANY, wxDefaultPosition, wxSize(wxDefaultCoord,22) );
#else
    m_headerArea = new wxExDataViewHeaderWindow( this, wxID_ANY, wxDefaultPosition, wxSize(wxDefaultCoord,25) );
#endif

    SetTargetWindow( m_clientArea );

    wxBoxSizer *sizer = new wxBoxSizer( wxVERTICAL );
    sizer->Add( m_headerArea, 0, wxGROW );
    sizer->Add( m_clientArea, 1, wxGROW );
    SetSizer( sizer );

    return true;
}

#ifdef __WXMSW__
WXLRESULT wxExDataViewCtrl::MSWWindowProc(WXUINT nMsg,
                                       WXWPARAM wParam,
                                       WXLPARAM lParam)
{
    WXLRESULT rc = wxDataViewCtrlBase::MSWWindowProc(nMsg, wParam, lParam);

#ifndef __WXWINCE__
    // we need to process arrows ourselves for scrolling
    if ( nMsg == WM_GETDLGCODE )
    {
        rc |= DLGC_WANTARROWS;
    }
#endif

    return rc;
}
#endif

void wxExDataViewCtrl::OnSize( wxSizeEvent &WXUNUSED(event) )
{
    // We need to override OnSize so that our scrolled
    // window a) does call Layout() to use sizers for
    // positioning the controls but b) does not query
    // the sizer for their size and use that for setting
    // the scrollable area as set that ourselves by
    // calling SetScrollbar() further down.

    Layout();

    AdjustScrollbars();
}

bool wxExDataViewCtrl::AssociateModel( wxDataViewListModel *model )
{
    if (!wxDataViewCtrlBase::AssociateModel( model ))
        return false;

    m_notifier = new wxExDataViewListModelNotifier( m_clientArea );

    model->AddNotifier( m_notifier );

    m_clientArea->UpdateDisplay();

    return true;
}

bool wxExDataViewCtrl::AppendColumn( wxDataViewColumn *col )
{
    if (!wxDataViewCtrlBase::AppendColumn(col))
        return false;

    m_clientArea->UpdateDisplay();

    return true;
}

void wxExDataViewCtrl::SetSelection( int WXUNUSED(row) )
{
    // FIXME - TODO
}

void wxExDataViewCtrl::SetSelectionRange( unsigned int WXUNUSED(from), unsigned int WXUNUSED(to) )
{
    // FIXME - TODO
}

void wxExDataViewCtrl::SetSelections( const wxArrayInt& WXUNUSED(aSelections) )
{
    // FIXME - TODO
}

void wxExDataViewCtrl::Unselect( unsigned int WXUNUSED(row) )
{
    // FIXME - TODO
}

bool wxExDataViewCtrl::IsSelected( unsigned int WXUNUSED(row) ) const
{
    // FIXME - TODO

    return false;
}

int wxExDataViewCtrl::GetSelection() const
{
    // FIXME - TODO

    return -1;
}

int wxExDataViewCtrl::GetSelections(wxArrayInt& WXUNUSED(aSelections) ) const
{
    // FIXME - TODO

    return 0;
}

#endif
