/////////////////////////////////////////////////////////////////////////////
// Name:        wxgmagick.cpp
// Purpose:     Mutex to make devIL thread safe & related helper functions
// Author:      Tobias Minich
// Modified by:
// Created:     Oct 28 2006
// RCS-ID:
// Copyright:   (c) 2006 Tobias Minich
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx_pch.h"

#include <Magick++.h>

#include "wxgmagick.h"

wxMutex wxILMutex(wxMUTEX_DEFAULT);

wxSize getBitmapSize(const char *filename) {
    Magick::Image img;
    wxSize res;
    try {
        img.read(filename);
        res = wxSize(img.columns(), img.rows());
    } catch (Magick::Exception e) {
        res = wxSize(-1, -1);
        wxLogDebug(wxT("Error in wxGXImage::FromImage: %s"), e.what());
    }

    return res;
}
/*
bool getBitmapInfo(const char *filename, ILinfo& info) {
    ILuint bmp;
    ILenum Error;
    bool res = true;

    {
        wxMutexLocker lock(wxILMutex);
        ILuint old = ilGetInteger(IL_ACTIVE_IMAGE);
        bmp = ilGenImage();
        ilBindImage(bmp);

        if (!ilLoadImage(filename)) {
            // Flush error buffer
            while ((Error = ilGetError())) {}
            res = false;
        } else {
            iluGetImageInfo(&info);
        }
        ilDeleteImage(bmp);
        ilBindImage(old);
    }
    return res;
}

bool ilInfoHasAlpha(const ILinfo& info) {
    switch (info.Format) {
        case IL_COLOR_INDEX: {
            return false; // devIL currently doesn't support fetching alpha from indexed bitmaps
        }
        case IL_RGBA:
        case IL_LUMINANCE_ALPHA:
        case IL_BGRA:
            return true;
        default:
            return false;
    }
}
*/
