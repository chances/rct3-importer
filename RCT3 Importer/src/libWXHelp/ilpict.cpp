/////////////////////////////////////////////////////////////////////////////
// Name:        ilpict.cpp
// Purpose:     wxILPicture
// Author:      Tobias Minich (belgabor@gmx.de)
// Modified by:
// Created:     2006-10-02
// RCS-ID:
// Copyright:   (c) Tobias Minich
// Licence:       wxWindows licence
// Based upon wxStaticPicture
/////////////////////////////////////////////////////////////////////////////

#include "wx_pch.h"

#include "wx/wxprec.h"
#include "wx/defs.h"

#include "ilpict.h"
#include "wxdevil.h"

IMPLEMENT_DYNAMIC_CLASS(wxILPicture, wxControl)

/*
 * wxILPicture
 */

BEGIN_EVENT_TABLE(wxILPicture, wxControl)
EVT_PAINT(wxILPicture::OnPaint)
END_EVENT_TABLE()

bool wxILPicture::Create(wxWindow *parent, wxWindowID id,
                         const wxBitmap& bitmap,
                         const wxString& filename,
                         const wxPoint& pos,
                         const wxSize& s,
                         long style,
                         const wxString& name) {
    SetName(name);

    if (bitmap.Ok())
        FromImage(bitmap.ConvertToImage());
    else
        FromFile(filename);


    wxSize size = s ;
    if ( m_valid ) {
        if (( size.x == wxDefaultCoord ) && ( size.y == wxDefaultCoord )) {
            size.x = m_width ;
            size.y = m_height ;
        } else {
            if ( size.x == wxDefaultCoord )
                size.x = (m_width * size.y) / m_height;
            if ( size.y == wxDefaultCoord )
                size.y = (m_height * size.x) / m_width;
        }
    }

    m_backgroundColour = parent->GetBackgroundColour() ;
    m_foregroundColour = parent->GetForegroundColour() ;

    Align = 0;
    Scale = 0;
    ScaleX = ScaleY = 1;

    if ( id == wxID_ANY )
        m_windowId = (int)NewControlId();
    else
        m_windowId = id;

    m_windowStyle = style;

    bool ret = wxControl::Create( parent, id, pos, size, style, wxDefaultValidator, name );

    SetSize( size ) ;

    return ret;
}

void wxILPicture::OnPaint(wxPaintEvent& WXUNUSED(event)) {
//    if ( !Bitmap.Ok() )
//        return;

    wxPaintDC dc( this );
    PrepareDC( dc );

    if (m_valid) {
        wxSize sz = GetSize();
        wxSize bmpsz( m_width, m_height );
        float sx = 1.0f, sy = 1.0f;

        if ( Scale & wxILSCALE_UNIFORM ) {
            float _sx = (float)sz.GetWidth() / (float)bmpsz.GetWidth();
            float _sy = (float)sz.GetHeight() / (float)bmpsz.GetHeight();
            sx = sy = _sx < _sy ? _sx : _sy;
        } else
            if ( Scale & wxILSCALE_CUSTOM ) {
                sx = ScaleX;
                sy = ScaleY;
            } else {
                if ( Scale & wxILSCALE_HORIZONTAL )
                    sx = (float)sz.x/(float)bmpsz.x;
                if ( Scale & wxILSCALE_VERTICAL )
                    sy = (float)sz.y/(float)bmpsz.y;
            }

        bmpsz = wxSize( (int)(bmpsz.x*sx), (int)(bmpsz.y*sy) );

        wxPoint pos( 0, 0 );

        if ( Align & wxALIGN_CENTER_HORIZONTAL ) pos.x = (sz.x-bmpsz.x)/2;
        else if ( Align & wxALIGN_RIGHT ) pos.x = sz.x-bmpsz.x;

        if ( Align & wxALIGN_CENTER_VERTICAL ) pos.y = (sz.y-bmpsz.y)/2;
        else if ( Align & wxALIGN_BOTTOM ) pos.y = sz.y-bmpsz.y;
        wxBitmap Bitmap(GetImage(bmpsz.GetWidth(), bmpsz.GetHeight()));
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
        dc.DrawBitmap( Bitmap, pos.x, pos.y );
    }
}

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
