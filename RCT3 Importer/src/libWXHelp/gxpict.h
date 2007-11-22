/////////////////////////////////////////////////////////////////////////////
// Name:        gxpict.h
// Purpose:     wxGXPicture class
// Author:      Tobias Minich
// Modified by:
// Created:     2007-11-13
// RCS-ID:
// Copyright:   (c) Tobias Minich
// Licence:     wxWindows licence
// Based upon wxStaticPicture
/////////////////////////////////////////////////////////////////////////////

#ifndef GXPICT_H_INCLUDED
#define GXPICT_H_INCLUDED

#include "wx_pch.h"

#include "wx/control.h"

#include "wx/icon.h"
#include "wx/bitmap.h"
#include "wx/image.h"

#include "gximage.h"

enum
{
    wxGXSCALE_NONE       = 0x0,
    wxGXSCALE_HORIZONTAL = 0x1,
    wxGXSCALE_VERTICAL   = 0x2,
    wxGXSCALE_UNIFORM    = 0x4,
    wxGXSCALE_CUSTOM     = 0x8,
    wxGXSCALE_RESERVE_SQUARE = 0x10
};

enum
{
    wxGXDESC_NONE       = 0x0,
    wxGXDESC_BOTTOM     = 0x1,
    wxGXDESC_RIGHT      = 0x2
};

class wxGXPicture : public wxControl
{
    DECLARE_DYNAMIC_CLASS(wxGXPicture)

public:
    wxGXPicture() {Init();}

    wxGXPicture( wxWindow* parent, wxWindowID id,
        const wxBitmap& label,
#ifdef CACHE_GXIMAGE
        const bool docache = true,
#endif
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0,
        const wxString& name = wxT("ilPicture") )
    {
        Init();
        Create( parent, id, label, wxT(""),
#ifdef CACHE_GXIMAGE
                docache,
#endif
                pos, size, style, name );
    }

    wxGXPicture( wxWindow* parent, wxWindowID id,
        const wxString& filename,
#ifdef CACHE_GXIMAGE
        const bool docache = true,
#endif
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0,
        const wxString& name = wxT("ilPicture") )
    {
        Init();
        Create( parent, id, wxNullBitmap, filename,
#ifdef CACHE_GXIMAGE
                docache,
#endif
                pos, size, style, name );
    }

    virtual ~wxGXPicture() {
        //ilDeleteImage(m_ilimage);
    }

    bool Create( wxWindow* parent, wxWindowID id,
        const wxBitmap& label,
        const wxString& filename,
#ifdef CACHE_GXIMAGE
        const bool docache = true,
#endif
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0,
        const wxString& name = wxT("ilPicture") );

    virtual void Command(wxCommandEvent& WXUNUSED(event)) {}
    virtual bool ProcessCommand(wxCommandEvent& WXUNUSED(event)) {return true;}
    void OnPaint(wxPaintEvent& event);

    void SetFileName( const wxString& filename )
    {
        m_image.FromFile(filename);
        if (m_image.Ok())
            m_filename = filename;
    }

    wxString GetFilename() const
    {
        return m_filename;
    }

    void SetImage( const wxImage& image)
    {
        m_cache = false;
        m_filename = wxT("");
        m_image.FromImage(image);
    }

    wxImage GetImage(int nw = -1, int nh = -1)
    {
        return m_image.GetImage(nw, nh);
    }

    void SetBitmap( const wxBitmap& bmp )
    {
        m_cache = false;
        m_filename = wxT("");
        m_image.FromBitmap(bmp);
    }

    wxBitmap GetBitmap()
    {
        wxBitmap bmp(m_image.GetImage());
        return bmp;
    }

    // Icon interface for compatibility with wxStaticBitmap.
    void SetIcon( const wxIcon& icon )
    {
        m_cache = false;
        m_filename = wxT("");
        m_image.FromIcon(icon);
    }

    wxIcon GetIcon()
    {
        return m_image.GetIcon();
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

    void UseAlpha(bool use_alpha = true) {
        m_alpha = use_alpha;
    }

    bool Ok() const
    {
        return m_image.Ok();
    }

    void LoadFromFileSystem(const wxString& filename)
    {
        m_image.FromFileSystem(filename);
        if (m_image.Ok())
            m_filename = filename;
    }

protected:
    wxGXImage m_image;
    wxString m_filename;
    bool m_alpha;
#ifdef CACHE_GXIMAGE
    bool m_cache;
#endif

    int Align;

    int Scale;
    float ScaleX;
    float ScaleY;

    void Init() {
        m_filename = wxT("");
        m_alpha = false;
#ifdef CACHE_GXIMAGE
        m_cache = true;
#endif
    }

    DECLARE_EVENT_TABLE()
};

#endif // ILPICT_H_INCLUDED
