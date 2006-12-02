/////////////////////////////////////////////////////////////////////////////
// Name:        wxdevil.cpp
// Purpose:     Mutex to make devIL thread safe & related helper functions
// Author:      Tobias Minich
// Modified by:
// Created:     Oct 28 2006
// RCS-ID:
// Copyright:   (c) 2006 Tobias Minich
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx_pch.h"

#include "wxdevil.h"

#include <IL/il.h>

wxMutex wxILMutex(wxMUTEX_DEFAULT);

wxSize getBitmapSize(const char *filename) {
    ILuint bmp;
    ILenum Error;
    wxSize res;

    {
        wxMutexLocker lock(wxILMutex);
        ILuint old = ilGetInteger(IL_ACTIVE_IMAGE);
        bmp = ilGenImage();
        ilBindImage(bmp);

        if (!ilLoadImage(filename)) {
            res = wxDefaultSize;
            // Flush error buffer
            while ((Error = ilGetError())) {}
        } else {
            res = wxSize(ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT));
        }
        ilDeleteImage(bmp);
        ilBindImage(old);
    }
    return res;
}
