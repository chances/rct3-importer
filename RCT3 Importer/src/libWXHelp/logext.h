/////////////////////////////////////////////////////////////////////////////
// Name:        logext.h
// Purpose:     Extended GUI logger
// Author:      Tobias Minich
// Created:     29/10/2006
// Copyright:   (c) 2006 Tobias Minich
// Licence:     wxWindows licence
// (Based upon the wxLogGui code by Vadim Zeitlin)
/////////////////////////////////////////////////////////////////////////////

#ifndef LOGEXT_H_INCLUDED
#define LOGEXT_H_INCLUDED

#include "wx_pch.h"

#include <wx/log.h>

class wxLogGuiExt : public wxLogGui
{
public:
    // ctor
    wxLogGuiExt(bool* do_continue = NULL, bool allow_continue_on_error = false, bool allow_save = false);

    // show all messages that were logged since the last Flush()
    virtual void Flush();

protected:
    bool* m_continue;
    bool  m_continueError;
    bool  m_allowSave;
};

#endif // LOGEXT_H_INCLUDED
