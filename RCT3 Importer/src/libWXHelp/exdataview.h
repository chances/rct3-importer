/////////////////////////////////////////////////////////////////////////////
// Name:        exdataview.h
// Orig. Name:  wx/generic/dataview.h
// Purpose:     Extended generic wxDataViewCtrl
// Orig.Author: Robert Roebling
// Modified by: Tobias Minich
// Id:          $Id: dataview.h,v 1.19 2006/10/06 09:50:39 RR Exp $
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __EXDATAVIEWCTRLH__
#define __EXDATAVIEWCTRLH__

#include "wx_pch.h"

#include <wx/dataview.h>


#define wxEDV_STRIPED               0x0100     // back colour different lines
#define wxEDV_SINGLE_ACTIVATE       0x0200     // single click activates cell
#define wxEDV_NO_SELECT             0x0400     // do not draw focus and selection

// Mask for all style flags as I have no idea what they might cause if passed upwards
#define wxEDV_ALL                   wxEDV_STRIPED|wxEDV_SINGLE_ACTIVATE|wxEDV_NO_SELECT

// ---------------------------------------------------------
// classes
// ---------------------------------------------------------

class wxExDataViewCtrl;
class wxExDataViewMainWindow;
class wxExDataViewHeaderWindow;

// ---------------------------------------------------------
// wxDataViewCtrl
// ---------------------------------------------------------

class wxExDataViewCtrl: public wxDataViewCtrlBase,
                                       public wxScrollHelperNative
{
public:
    wxExDataViewCtrl() : wxScrollHelperNative(this)
    {
        Init();
    }

    wxExDataViewCtrl( wxWindow *parent, wxWindowID id,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, long style = 0,
           const wxValidator& validator = wxDefaultValidator )
             : wxScrollHelperNative(this)
    {
        Create(parent, id, pos, size, style, validator );
    }

    virtual ~wxExDataViewCtrl();

    void Init();

    bool Create(wxWindow *parent, wxWindowID id,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, long style = 0,
           const wxValidator& validator = wxDefaultValidator );

    virtual bool AssociateModel( wxDataViewListModel *model );
    virtual bool AppendColumn( wxDataViewColumn *col );

    virtual void SetSelection( int row ); // -1 for unselect
    virtual void SetSelectionRange( unsigned int from, unsigned int to );
    virtual void SetSelections( const wxArrayInt& aSelections);
    virtual void Unselect( unsigned int row );

    virtual bool IsSelected( unsigned int row ) const;
    virtual int GetSelection() const;
    virtual int GetSelections(wxArrayInt& aSelections) const;

private:
    friend class wxExDataViewMainWindow;
    friend class wxExDataViewHeaderWindow;
    wxDataViewListModelNotifier *m_notifier;
    wxExDataViewMainWindow        *m_clientArea;
    wxExDataViewHeaderWindow      *m_headerArea;
    bool            m_Striped;
    bool            m_SingleActivate;
    bool            m_NoSelect;

private:
    void OnSize( wxSizeEvent &event );

    // we need to return a special WM_GETDLGCODE value to process just the
    // arrows but let the other navigation characters through
#ifdef __WXMSW__
    virtual WXLRESULT MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam);
#endif // __WXMSW__

    WX_FORWARD_TO_SCROLL_HELPER()

private:
    DECLARE_DYNAMIC_CLASS(wxExDataViewCtrl)
    DECLARE_NO_COPY_CLASS(wxExDataViewCtrl)
    DECLARE_EVENT_TABLE()
};


#endif // __GENERICDATAVIEWCTRLH__
