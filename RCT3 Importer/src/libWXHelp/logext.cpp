/////////////////////////////////////////////////////////////////////////////
// Name:        logext.cpp
// Purpose:     Extended GUI logger
// Author:      Tobias Minich
// Created:     29/10/2006
// Copyright:   (c) 2006 Tobias Minich
// Licence:     wxWindows licence
// (Based upon the wxLogGui code by Vadim Zeitlin)
/////////////////////////////////////////////////////////////////////////////

#ifndef WXHELP_BASE

#include "logext.h"

#include <wx/app.h>
#include <wx/artprov.h>
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/listctrl.h>
#include <wx/imaglist.h>
#include <wx/image.h>
#include <wx/file.h>
#include <wx/filedlg.h>
#include <wx/msgdlg.h>
#include <wx/sizer.h>
#include <wx/statbmp.h>
#include <wx/statline.h>
#include <wx/textfile.h>

// the suffix we add to the button to show that the dialog can be expanded
#define EXPAND_SUFFIX _T(" >>")

// this function is a wrapper around strftime(3)
// allows to exclude the usage of wxDateTime
static wxString TimeStamp(const wxChar *format, time_t t)
{
    wxChar buf[4096];
    if ( !wxStrftime(buf, WXSIZEOF(buf), format, localtime(&t)) )
    {
        // buffer is too small?
        wxFAIL_MSG(_T("strftime() failed"));
    }
    return wxString(buf);
}

// pass an uninitialized file object, the function will ask the user for the
// filename and try to open it, returns true on success (file was opened),
// false if file couldn't be opened/created and -1 if the file selection
// dialog was cancelled
static int OpenLogFile(wxFile& file, wxString *pFilename, wxWindow *parent)
{
    // get the file name
    // -----------------
    wxString filename = wxSaveFileSelector(wxT("log"), wxT("txt"), wxT("log.txt"), parent);
    if ( !filename ) {
        // cancelled
        return -1;
    }

    // open file
    // ---------
    bool bOk wxDUMMY_INITIALIZE(false);
    if ( wxFile::Exists(filename) ) {
        bool bAppend = false;
        wxString strMsg;
        strMsg.Printf(_("Append log to file '%s' (choosing [No] will overwrite it)?"),
                      filename.c_str());
        switch ( wxMessageBox(strMsg, _("Question"),
                              wxICON_QUESTION | wxYES_NO | wxCANCEL) ) {
            case wxYES:
                bAppend = true;
                break;

            case wxNO:
                bAppend = false;
                break;

            case wxCANCEL:
                return -1;

            default:
                wxFAIL_MSG(_("invalid message box return value"));
        }

        if ( bAppend ) {
            bOk = file.Open(filename, wxFile::write_append);
        }
        else {
            bOk = file.Create(filename, true /* overwrite */);
        }
    }
    else {
        bOk = file.Create(filename);
    }

    if ( pFilename )
        *pFilename = filename;

    return bOk;
}

class wxExtLogDialog : public wxDialog {
public:
    wxExtLogDialog(wxWindow *parent,
                   const wxArrayString& messages,
                   const wxArrayInt& severity,
                   const wxArrayLong& timess,
                   const wxString& caption,
                   long style,
                   bool show_save,
                   bool info_only,
                   bool allow_cont);
    virtual ~wxExtLogDialog();

    // event handlers
    void OnOk(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
    void OnDetails(wxCommandEvent& event);
    void OnSave(wxCommandEvent& event);
    void OnListSelect(wxListEvent& event);

private:
    // create controls needed for the details display
    void CreateDetailsControls();

    // the data for the listctrl
    wxArrayString m_messages;
    wxArrayInt m_severity;
    wxArrayLong m_times;

    // Settings
    bool m_showSave;

    // the "toggle" button and its state
    wxButton *m_btnDetails;
    bool      m_showingDetails;

    // the controls which are not shown initially (but only when details
    // button is pressed)
    wxListCtrl *m_listctrl;
    wxStaticLine *m_statline;
    wxButton *m_btnSave;

    // the translated "Details" string
    static wxString ms_details;

    DECLARE_EVENT_TABLE()
    DECLARE_NO_COPY_CLASS(wxExtLogDialog)
};

BEGIN_EVENT_TABLE(wxExtLogDialog, wxDialog)
EVT_BUTTON(wxID_OK, wxExtLogDialog::OnOk)
EVT_BUTTON(wxID_CANCEL, wxExtLogDialog::OnCancel)
EVT_BUTTON(wxID_MORE,   wxExtLogDialog::OnDetails)
EVT_BUTTON(wxID_SAVE,   wxExtLogDialog::OnSave)
EVT_LIST_ITEM_SELECTED(wxID_ANY, wxExtLogDialog::OnListSelect)
END_EVENT_TABLE()


wxLogGuiExt::wxLogGuiExt(bool* do_continue, bool allow_continue_on_error, bool allow_save):wxLogGui() {
    m_continue = do_continue;
    m_continueError = allow_continue_on_error;
    m_allowSave = allow_save;
}

void wxLogGuiExt::Flush() {
    if ( !m_bHasMessages )
        return;

    // do it right now to block any new calls to Flush() while we're here
    m_bHasMessages = false;

    unsigned repeatCount = 0;
    if ( wxLog::GetRepetitionCounting() ) {
#if !wxCHECK_VERSION(2, 9, 0)
        repeatCount = wxLog::DoLogNumberOfRepeats();
#endif
    }

    wxString appName = wxTheApp->GetAppName();
    if ( !appName.empty() )
        appName[0u] = (wxChar)wxToupper(appName[0u]);

    long style;
    wxString titleFormat;
    if ( m_bErrors ) {
        titleFormat = _("%s Error");
        style = wxICON_STOP;
    } else if ( m_bWarnings ) {
        titleFormat = _("%s Warning");
        style = wxICON_EXCLAMATION;
    } else {
        titleFormat = _("%s Information");
        style = wxICON_INFORMATION;
    }

    wxString title;
    title.Printf(titleFormat, appName.c_str());

    size_t nMsgCount = m_aMessages.Count();

    // avoid showing other log dialogs until we're done with the dialog we're
    // showing right now: nested modal dialogs make for really bad UI!
    Suspend();

    if ( repeatCount > 0 )
        m_aMessages[nMsgCount-1] += wxString::Format(wxT(" (%s)"), m_aMessages[nMsgCount-2].c_str());
    wxExtLogDialog dlg(NULL,
                       m_aMessages, m_aSeverity, m_aTimes,
                       title, style, m_allowSave, !m_continue, ((!m_bErrors) || m_continueError));

    // clear the message list before showing the dialog because while it's
    // shown some new messages may appear
    Clear();

    int res = dlg.ShowModal();
    if (m_continue) {
        *m_continue = (res == wxID_OK);
    }


    // allow flushing the logs again
    Resume();
}

wxString wxExtLogDialog::ms_details;

wxExtLogDialog::wxExtLogDialog(wxWindow *parent,
                                 const wxArrayString& messages,
                                 const wxArrayInt& severity,
                                 const wxArrayLong& times,
                                 const wxString& caption,
                                 long style,
                                 bool show_save,
                                 bool info_only,
                                 bool allow_cont)
           : wxDialog(parent, wxID_ANY, caption,
                      wxDefaultPosition, wxDefaultSize,
                      wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
    if ( ms_details.empty() )
    {
        // ensure that we won't loop here if wxGetTranslation()
        // happens to pop up a Log message while translating this :-)
        ms_details = wxTRANSLATE("&Details");
        ms_details = wxGetTranslation(ms_details);
    }

    size_t count = messages.GetCount();
    m_messages.Alloc(count);
    m_severity.Alloc(count);
    m_times.Alloc(count);

    m_showSave = show_save;

    for ( size_t n = 0; n < count; n++ )
    {
        wxString msg = messages[n];
        msg.Replace(wxT("\n"), wxT(" "));
        m_messages.Add(msg);
        m_severity.Add(severity[n]);
        m_times.Add(times[n]);
    }

    m_showingDetails = false; // not initially
    m_listctrl = (wxListCtrl *)NULL;

    m_statline = (wxStaticLine *)NULL;

    m_btnSave = (wxButton *)NULL;

    // create the controls which are always shown and layout them: we use
    // sizers even though our window is not resizeable to calculate the size of
    // the dialog properly
    wxBoxSizer *sizerTop = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *sizerButtons = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *sizerAll = new wxBoxSizer(wxHORIZONTAL);

    wxButton *btnOk;
    wxButton *btnCancel = NULL;
    if (info_only) {
        btnOk = new wxButton(this, wxID_OK);
        sizerButtons->Add(btnOk, 0, wxCENTRE|wxBOTTOM, 5);
    } else {
        btnOk = new wxButton(this, wxID_OK, _("Continue"));
        btnOk->Enable(allow_cont);
        sizerButtons->Add(btnOk, 0, wxCENTRE|wxBOTTOM, 5);
        btnCancel = new wxButton(this, wxID_CANCEL);
        sizerButtons->Add(btnCancel, 0, wxCENTRE|wxBOTTOM, 5);
    }
    if (m_messages.size()>1) {
        m_btnDetails = new wxButton(this, wxID_MORE, ms_details + EXPAND_SUFFIX);
        sizerButtons->Add(m_btnDetails, 0, wxCENTRE | wxTOP, 4);
    }

    wxBitmap bitmap;
    switch ( style & wxICON_MASK )
    {
        case wxICON_ERROR:
            bitmap = wxArtProvider::GetBitmap(wxART_ERROR, wxART_MESSAGE_BOX);
            break;

        case wxICON_INFORMATION:
            bitmap = wxArtProvider::GetBitmap(wxART_INFORMATION, wxART_MESSAGE_BOX);
            break;

        case wxICON_WARNING:
            bitmap = wxArtProvider::GetBitmap(wxART_WARNING, wxART_MESSAGE_BOX);
            break;

        default:
            wxFAIL_MSG(_T("incorrect log style"));
    }

    sizerAll->Add(new wxStaticBitmap(this, wxID_ANY, bitmap), 0, wxALIGN_CENTRE_VERTICAL);

    const wxString& message = messages.Last();
    sizerAll->Add(CreateTextSizer(message), 1, wxALIGN_CENTRE_VERTICAL | wxLEFT | wxRIGHT, 10);
    sizerAll->Add(sizerButtons, 0, (wxALIGN_RIGHT | wxLEFT), 10);

    sizerTop->Add(sizerAll, 0, wxALL | wxEXPAND | wxALIGN_CENTER_VERTICAL, 10);

    SetSizer(sizerTop);

    // see comments in OnDetails()
    //
    // Note: Doing this, this way, triggered a nasty bug in
    //       wxTopLevelWindowGTK::GtkOnSize which took -1 literally once
    //       either of maxWidth or maxHeight was set.  This symptom has been
    //       fixed there, but it is a problem that remains as long as we allow
    //       unchecked access to the internal size members.  We really need to
    //       encapuslate window sizes more cleanly and make it clear when -1 will
    //       be substituted and when it will not.

    wxSize size = sizerTop->Fit(this);
    m_maxHeight = size.y;
    SetSizeHints(size.x, size.y, m_maxWidth, m_maxHeight);

    if (allow_cont || info_only)
        btnOk->SetFocus();
    else
        btnCancel->SetFocus();

    Centre();

}

void wxExtLogDialog::CreateDetailsControls()
{
    // create the save button and separator line if possible
    if (m_showSave)
        m_btnSave = new wxButton(this, wxID_SAVE);

    m_statline = new wxStaticLine(this, wxID_ANY);

    // create the list ctrl now
    m_listctrl = new wxListCtrl(this, wxID_ANY,
                                wxDefaultPosition, wxDefaultSize,
                                wxSUNKEN_BORDER |
                                wxLC_REPORT |
                                wxLC_NO_HEADER |
                                wxLC_SINGLE_SEL);

    // no need to translate these strings as they're not shown to the
    // user anyhow (we use wxLC_NO_HEADER style)
    m_listctrl->InsertColumn(0, _T("Message"));
    m_listctrl->InsertColumn(1, _T("Time"));

    // prepare the imagelist
    static const int ICON_SIZE = 16;
    wxImageList *imageList = new wxImageList(ICON_SIZE, ICON_SIZE);

    // order should be the same as in the switch below!
    static const wxChar* icons[] =
    {
        wxART_ERROR,
        wxART_WARNING,
        wxART_INFORMATION
    };

    bool loadedIcons = true;

    for ( size_t icon = 0; icon < WXSIZEOF(icons); icon++ )
    {
        wxBitmap bmp = wxArtProvider::GetBitmap(icons[icon], wxART_MESSAGE_BOX,
                                                wxSize(ICON_SIZE, ICON_SIZE));

        // This may very well fail if there are insufficient colours available.
        // Degrade gracefully.
        if ( !bmp.Ok() )
        {
            loadedIcons = false;

            break;
        }

        imageList->Add(bmp);
    }

    m_listctrl->SetImageList(imageList, wxIMAGE_LIST_SMALL);

    // and fill it
    wxString fmt = wxLog::GetTimestamp();
    if ( !fmt )
    {
        // default format
        fmt = _T("%c");
    }

    size_t count = m_messages.GetCount();
    for ( size_t n = 0; n < count; n++ )
    {
        int image;

        if ( loadedIcons )
        {
            switch ( m_severity[n] )
            {
                case wxLOG_Error:
                    image = 0;
                    break;

                case wxLOG_Warning:
                    image = 1;
                    break;

                default:
                    image = 2;
            }
        }
        else // failed to load images
        {
            image = -1;
        }

        m_listctrl->InsertItem(n, m_messages[n], image);
        m_listctrl->SetItem(n, 1, TimeStamp(fmt.c_str(), (time_t)m_times[n]));
    }

    // let the columns size themselves
    m_listctrl->SetColumnWidth(0, wxLIST_AUTOSIZE);
    m_listctrl->SetColumnWidth(1, wxLIST_AUTOSIZE);

    // calculate an approximately nice height for the listctrl
    int height = GetCharHeight()*(count + 4);

    // but check that the dialog won't fall fown from the screen
    //
    // we use GetMinHeight() to get the height of the dialog part without the
    // details and we consider that the "Save" button below and the separator
    // line (and the margins around it) take about as much, hence double it
    int heightMax = wxGetDisplaySize().y - GetPosition().y - 2*GetMinHeight();

    // we should leave a margin
    heightMax *= 9;
    heightMax /= 10;

    m_listctrl->SetSize(wxDefaultCoord, wxMin(height, heightMax));
}

void wxExtLogDialog::OnListSelect(wxListEvent& event)
{
    // we can't just disable the control because this looks ugly under Windows
    // (wrong bg colour, no scrolling...), but we still want to disable
    // selecting items - it makes no sense here
    m_listctrl->SetItemState(event.GetIndex(), 0, wxLIST_STATE_SELECTED);
}

void wxExtLogDialog::OnOk(wxCommandEvent& WXUNUSED(event))
{
    EndModal(wxID_OK);
}

void wxExtLogDialog::OnCancel(wxCommandEvent& WXUNUSED(event))
{
    EndModal(wxID_CANCEL);
}


void wxExtLogDialog::OnSave(wxCommandEvent& WXUNUSED(event))
{
    wxFile file;
    int rc = OpenLogFile(file, NULL, this);
    if ( rc == -1 )
    {
        // cancelled
        return;
    }

    bool ok = rc != 0;

    wxString fmt = wxLog::GetTimestamp();
    if ( !fmt )
    {
        // default format
        fmt = _T("%c");
    }

    size_t count = m_messages.GetCount();
    for ( size_t n = 0; ok && (n < count); n++ )
    {
        wxString line;
        line << TimeStamp(fmt.c_str(), (time_t)m_times[n])
             << _T(": ")
             << m_messages[n]
             << wxTextFile::GetEOL();

        ok = file.Write(line);
    }

    if ( ok )
        ok = file.Close();

    if ( !ok )
        wxLogError(_("Can't save log contents to file."));
}


void wxExtLogDialog::OnDetails(wxCommandEvent& WXUNUSED(event))
{
    wxSizer *sizer = GetSizer();

    if ( m_showingDetails )
    {
        m_btnDetails->SetLabel(ms_details + EXPAND_SUFFIX);

        sizer->Detach( m_listctrl );


        sizer->Detach( m_statline );
        if (m_showSave)
            sizer->Detach( m_btnSave );
    }
    else // show details now
    {
        m_btnDetails->SetLabel(wxString(_T("<< ")) + ms_details);

        if ( !m_listctrl )
        {
            CreateDetailsControls();
        }

        sizer->Add(m_statline, 0, wxEXPAND | (wxALL & ~wxTOP), 10);

        sizer->Add(m_listctrl, 1, wxEXPAND | (wxALL & ~wxTOP), 10);

        // VZ: this doesn't work as this becomes the initial (and not only
        //     minimal) listctrl height as well - why?

        if (m_showSave)
            sizer->Add(m_btnSave, 0, wxALIGN_RIGHT | (wxALL & ~wxTOP), 10);
    }

    m_showingDetails = !m_showingDetails;

    // in any case, our size changed - relayout everything and set new hints
    // ---------------------------------------------------------------------

    // we have to reset min size constraints or Fit() would never reduce the
    // dialog size when collapsing it and we have to reset max constraint
    // because it wouldn't expand it otherwise

    m_minHeight =
    m_maxHeight = -1;

    // wxSizer::FitSize() is private, otherwise we might use it directly...
    wxSize sizeTotal = GetSize(),
           sizeClient = GetClientSize();

    wxSize size = sizer->GetMinSize();
    size.x += sizeTotal.x - sizeClient.x;
    size.y += sizeTotal.y - sizeClient.y;

    // we don't want to allow expanding the dialog in vertical direction as
    // this would show the "hidden" details but we can resize the dialog
    // vertically while the details are shown
    if ( !m_showingDetails )
        m_maxHeight = size.y;

    SetSizeHints(size.x, size.y, m_maxWidth, m_maxHeight);


    // don't change the width when expanding/collapsing
    SetSize(wxDefaultCoord, size.y);

#ifdef __WXGTK__
    // VS: this is necessary in order to force frame redraw under
    // WindowMaker or fvwm2 (and probably other broken WMs).
    // Otherwise, detailed list wouldn't be displayed.
    Show();
#endif // wxGTK
}

wxExtLogDialog::~wxExtLogDialog()
{
    if ( m_listctrl )
    {
        delete m_listctrl->GetImageList(wxIMAGE_LIST_SMALL);
    }
}

#endif
