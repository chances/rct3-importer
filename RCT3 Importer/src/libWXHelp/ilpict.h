/////////////////////////////////////////////////////////////////////////////
// Name:        ilpict.h
// Purpose:     wxILPicture class
// Author:      Tobias Minich
// Modified by:
// Created:     2006-10-02
// RCS-ID:
// Copyright:   (c) Tobias Minich
// Licence:     wxWindows licence
// Based upon wxStaticPicture
/////////////////////////////////////////////////////////////////////////////

#ifndef ILPICT_H_INCLUDED
#define ILPICT_H_INCLUDED

#include "wx_pch.h"

#include <IL/il.h>
#include <IL/ilu.h>

#include "wx/control.h"

#include "wx/icon.h"
#include "wx/bitmap.h"
#include "wx/image.h"

enum
{
    wxILSCALE_HORIZONTAL = 0x1,
    wxILSCALE_VERTICAL   = 0x2,
    wxILSCALE_UNIFORM    = 0x4,
    wxILSCALE_CUSTOM     = 0x8,
    wxILSCALE_RESERVE_SQUARE = 0x10
};

enum
{
    wxILDESC_NONE       = 0x0,
    wxILDESC_BOTTOM     = 0x1,
    wxILDESC_RIGHT      = 0x2
};

class wxILPicture : public wxControl
{
    DECLARE_DYNAMIC_CLASS(wxILPicture)

public:
    wxILPicture() {Init();}

    wxILPicture( wxWindow* parent, wxWindowID id,
        const wxBitmap& label,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0,
        const wxString& name = wxT("ilPicture") )
    {
        Init();
        Create( parent, id, label, wxT(""), pos, size, style, name );
    }

    wxILPicture( wxWindow* parent, wxWindowID id,
        const wxString& filename,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0,
        const wxString& name = wxT("ilPicture") )
    {
        Init();
        Create( parent, id, wxNullBitmap, filename, pos, size, style, name );
    }

    virtual ~wxILPicture() {
        ilDeleteImage(m_ilimage);
    }

    bool Create( wxWindow* parent, wxWindowID id,
        const wxBitmap& label,
        const wxString& filename,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0,
        const wxString& name = wxT("ilPicture") );

    virtual void Command(wxCommandEvent& WXUNUSED(event)) {}
    virtual bool ProcessCommand(wxCommandEvent& WXUNUSED(event)) {return true;}
    void OnPaint(wxPaintEvent& event);

    void SetFileName( const wxString& filename )
    {
        FromFile(filename);
    }

    wxString GetFilename() const
    {
        return m_filename;
    }

    void SetImage( const wxImage& image)
    {
        FromImage(image);
    }

    wxImage GetImage(int nw = -1, int nh = -1);

    void SetBitmap( const wxBitmap& bmp )
    {
        SetImage(bmp.ConvertToImage());
    }

    wxBitmap GetBitmap()
    {
        wxBitmap bmp(GetImage());
        return bmp;
    }

    // Icon interface for compatibility with wxStaticBitmap.
    void SetIcon( const wxIcon& icon )
    {
        wxBitmap bmp;
        bmp.CopyFromIcon( icon );
        SetBitmap( bmp );
    }

    wxIcon GetIcon()
    {
        wxIcon icon;
        icon.CopyFromBitmap( wxBitmap(GetImage()) );
        return icon;
    }

    void SetAlignment( int align )
    {
        Align = align;
    }

    int GetAlignment() const
    {
        return Align;
    }

    void SetScale( int scale )
    {
        Scale = scale;
    }

    int GetScale() const
    {
        return Scale;
    }

    void SetCustomScale( float sx, float sy )
    {
        ScaleX = sx;
        ScaleY = sy;
    }

    void GetCustomScale( float* sx, float* sy ) const
    {
        *sx = ScaleX;
        *sy = ScaleY;
    }

    bool Ok() const
    {
        return m_valid;
    }

protected:
    ILuint m_ilimage;
    wxString m_filename;
    bool m_valid;
    ILenum m_filter;
    ILuint m_width, m_height;

    int Align;

    int Scale;
    float ScaleX;
    float ScaleY;

    void Init() {
        ilInit();
        iluInit();
        m_ilimage = ilGenImage();
        m_valid = false;
        m_filter = ILU_BILINEAR;
        m_width = 0;
        m_height = 0;
    }

    void FromImage(const wxImage& image);
    void FromFile(const wxString& filename);

    DECLARE_EVENT_TABLE()
};

#endif // ILPICT_H_INCLUDED
