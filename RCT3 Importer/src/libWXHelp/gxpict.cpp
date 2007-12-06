/////////////////////////////////////////////////////////////////////////////
// Name:        gxpict.cpp
// Purpose:     wxGXPicture
// Author:      Tobias Minich (belgabor@gmx.de)
// Modified by:
// Created:     2007-11-13
// RCS-ID:
// Copyright:   (c) Tobias Minich
// Licence:       wxWindows licence
// Based upon wxStaticPicture
/////////////////////////////////////////////////////////////////////////////

#include "wx_pch.h"

#include "wx/wxprec.h"
#include "wx/defs.h"

#include <wx/filesys.h>
#include <wx/mstream.h>

#include "gxpict.h"

IMPLEMENT_DYNAMIC_CLASS(wxGXPicture, wxControl)

/*
 * wxILPicture
 */

BEGIN_EVENT_TABLE(wxGXPicture, wxControl)
EVT_PAINT(wxGXPicture::OnPaint)
END_EVENT_TABLE()

bool wxGXPicture::Create(wxWindow *parent, wxWindowID id,
                         const wxBitmap& bitmap,
                         const wxString& filename,
#ifdef CACHE_GXIMAGE
                         const bool docache,
#endif
                         const wxPoint& pos,
                         const wxSize& s,
                         long style,
                         const wxString& name) {
    SetName(name);
#ifdef CACHE_GXIMAGE
    m_cache = docache;
    m_image = wxGXImage(m_cache);
#endif

    if (bitmap.Ok())
        m_image.FromImage(bitmap.ConvertToImage());
    else
        SetFileName(filename);


    wxSize size = s ;
    if ( m_image.Ok() ) {
        if (( size.x == wxDefaultCoord ) && ( size.y == wxDefaultCoord )) {
            size.x = m_image.GetWidth() ;
            size.y = m_image.GetHeight() ;
        } else {
            if ( size.x == wxDefaultCoord )
                size.x = (m_image.GetWidth() * size.y) / m_image.GetHeight();
            if ( size.y == wxDefaultCoord )
                size.y = (m_image.GetHeight() * size.x) / m_image.GetWidth();
        }
    }

    m_backgroundColour = parent->GetBackgroundColour() ;
    m_foregroundColour = parent->GetForegroundColour() ;

    Align = 0;
    Scale = 0;
    ScaleX = ScaleY = 1;

/*
    if ( id == wxID_ANY )
        m_windowId = (int)NewControlId();
    else */
        m_windowId = id;

    m_windowStyle = style;
    bool ret = wxControl::Create( parent, m_windowId, pos, size, style, wxDefaultValidator, name );

    SetSize( size ) ;
    SetMinSize( wxSize(4, 4) );

    return ret;
}

void wxGXPicture::OnPaint(wxPaintEvent& WXUNUSED(event)) {
//    if ( !Bitmap.Ok() )
//        return;

    wxPaintDC dc( this );
    PrepareDC( dc );
    //dc.SetBackground(wxBrush(m_backgroundColour));
    //dc.Clear();

    if (m_image.Ok()) {
        wxSize sz = GetSize();
        wxSize bmpsz( m_image.GetWidth(), m_image.GetHeight() );
        float sx = 1.0f, sy = 1.0f;

        if ( Scale & wxGXSCALE_UNIFORM ) {
            float _sx = (float)sz.GetWidth() / (float)bmpsz.GetWidth();
            float _sy = (float)sz.GetHeight() / (float)bmpsz.GetHeight();
            sx = sy = _sx < _sy ? _sx : _sy;
        } else
            if ( Scale & wxGXSCALE_CUSTOM ) {
                sx = ScaleX;
                sy = ScaleY;
            } else {
                if ( Scale & wxGXSCALE_HORIZONTAL )
                    sx = (float)sz.x/(float)bmpsz.x;
                if ( Scale & wxGXSCALE_VERTICAL )
                    sy = (float)sz.y/(float)bmpsz.y;
            }

        bmpsz = wxSize( (int)(bmpsz.x*sx), (int)(bmpsz.y*sy) );

        wxPoint pos( 0, 0 );

        if ( Align & wxALIGN_CENTER_HORIZONTAL ) pos.x = (sz.x-bmpsz.x)/2;
        else if ( Align & wxALIGN_RIGHT ) pos.x = sz.x-bmpsz.x;

        if ( Align & wxALIGN_CENTER_VERTICAL ) pos.y = (sz.y-bmpsz.y)/2;
        else if ( Align & wxALIGN_BOTTOM ) pos.y = sz.y-bmpsz.y;
        wxImage tmp = m_image.GetImage(bmpsz.GetWidth(), bmpsz.GetHeight());
#ifdef __WXDEBUG__
        if (tmp.HasAlpha())
            wxLogDebug(wxT("Trace, wxGXPicture::OnPaint Transparent %dx%d"), tmp.GetWidth(), tmp.GetHeight());
        else
            wxLogDebug(wxT("Trace, wxGXPicture::OnPaint Opaque %dx%d"), tmp.GetWidth(), tmp.GetHeight());
#endif
        //tmp.ConvertAlphaToMask(128);
        wxBitmap Bitmap(tmp);
/*
        if ( Scale ) {
            if ( LastScaleX != sx || LastScaleY != sy ) {
                LastScaleX = sx;
                LastScaleY = sy;
                ScaledBitmap = wxBitmap( OriginalImage.Scale( bmpsz.x, bmpsz.y ) );
            }
            dc.DrawBitmap( ScaledBitmap, pos.x, pos.y );
        } else
*/
        wxLogDebug(wxT("Trace, wxGXPicture::OnPaint Bitmap: %dx%d at %d,%d, %s, %s"), Bitmap.GetWidth(), Bitmap.GetHeight(), pos.x, pos.y, Bitmap.Ok()?wxT("Ok"):wxT("Bad"), m_alpha?wxT("show alpha"):wxT("no alpha"));
        dc.DrawBitmap( Bitmap, pos.x, pos.y, m_alpha );
    }
}
/*
wxImage wxILPicture::GetImage(int nw, int nh) {
    wxImage img(wxNullImage);
    if (m_valid) {
        ILuint temp = ilGenImage();
        wxMutexLocker lock (wxILMutex);
        ILuint old = ilGetInteger(IL_ACTIVE_IMAGE);
        ilBindImage(temp);
        if (!ilCopyImage(m_ilimage)) {
            ilBindImage(old);
            ilDeleteImage(temp);
            return img;
        }
        if (nw == -1)
            nw = m_width;
        if (nh == -1)
            nh = m_height;
        if ((nw != m_width) || (nh != m_height)) {
            iluImageParameter(ILU_FILTER, m_filter);
            if (!iluScale(nw, nh, 1)) {
                ilBindImage(old);
                ilDeleteImage(temp);
                return img;
            }
        }
        if (!ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE)) {
            ilBindImage(old);
            ilDeleteImage(temp);
            return img;
        }
        unsigned char *data = static_cast<unsigned char*> (malloc(nw*nh*3));
        memcpy(data, ilGetData(), nw*nh*3);
        img = wxImage(nw, nh);
        img.SetData(data);
        if (m_alpha)
            img.SetAlpha(ilGetAlpha(IL_UNSIGNED_BYTE));
        ilBindImage(old);
        ilDeleteImage(temp);
    }
    return img;
}

void wxILPicture::FromImage(const wxImage& image) {
    m_filename = wxT("");
    if (image.Ok()) {
        wxMutexLocker lock (wxILMutex);
        ILuint old = ilGetInteger(IL_ACTIVE_IMAGE);
        ilBindImage(m_ilimage);
        m_valid = ilTexImage(image.GetWidth(), image.GetHeight(), 1, 3, IL_RGB, IL_UNSIGNED_BYTE, image.GetData());
        m_width = ilGetInteger(IL_IMAGE_WIDTH);
        m_height = ilGetInteger(IL_IMAGE_HEIGHT);
        ilBindImage(old);
    } else {
        ilDeleteImage(m_ilimage);
        m_ilimage = ilGenImage();
        m_valid = false;
        m_width = 0;
        m_height = 0;
    }
}

void wxILPicture::FromFile(const wxString& filename) {
    if (wxFileExists(filename)) {
        wxMutexLocker lock (wxILMutex);
        ILuint old = ilGetInteger(IL_ACTIVE_IMAGE);
        ilBindImage(m_ilimage);
        m_valid = ilLoadImage(filename.mb_str(wxConvFile));
        m_width = ilGetInteger(IL_IMAGE_WIDTH);
        m_height = ilGetInteger(IL_IMAGE_HEIGHT);
        if (m_valid && (ilGetInteger(IL_IMAGE_ORIGIN) == IL_ORIGIN_LOWER_LEFT))
           m_valid = iluFlipImage();
        ilBindImage(old);
    } else {
        m_valid = false;
    }
    if (!m_valid) {
        ilDeleteImage(m_ilimage);
        m_ilimage = ilGenImage();
        m_width = 0;
        m_height = 0;
    }
}

void wxILPicture::LoadFromFileSystem(const wxString& filename, const ILenum type) {
    wxFileSystem fs;
    wxFSFile* file = fs.OpenFile(filename, wxFS_READ);
    if (file) {
        wxInputStream* filestream = file->GetStream();
        filestream->SeekI(0, wxFromEnd);
        int len = filestream->TellI();
        char* buf = new char[len];
        filestream->SeekI(0);
        wxMemoryOutputStream* buffer = new wxMemoryOutputStream(buf, len);
        buffer->Write(*filestream);
        {
            wxMutexLocker lock (wxILMutex);
            ILuint old = ilGetInteger(IL_ACTIVE_IMAGE);
            ilBindImage(m_ilimage);
            //m_valid = ilLoadImage(filename.mb_str(wxConvFile));
            m_valid = ilLoadL(type, buf, len);
            m_width = ilGetInteger(IL_IMAGE_WIDTH);
            m_height = ilGetInteger(IL_IMAGE_HEIGHT);
            if (m_valid && (ilGetInteger(IL_IMAGE_ORIGIN) == IL_ORIGIN_LOWER_LEFT))
               m_valid = iluFlipImage();
            ilBindImage(old);
        }
#ifdef __WXDEBUG__
        if (m_valid) {
            wxLogDebug(wxT("Trace, wxILPicture::LoadFromFileSystem, valid, length %d, %dx%d"), len, m_width, m_height);
        } else {
            wxLogDebug(wxT("Trace, wxILPicture::LoadFromFileSystem, invalid, length %d, %dx%d"), len, m_width, m_height);
        }
#endif
        delete buffer;
        delete[] buf;
        delete filestream;
    } else {
        wxLogDebug(wxT("Trace, wxILPicture::LoadFromFileSystem, invalid"));
        m_valid = false;
    }
    if (!m_valid) {
        ilDeleteImage(m_ilimage);
        m_ilimage = ilGenImage();
        m_width = 0;
        m_height = 0;
    }
}
*/
