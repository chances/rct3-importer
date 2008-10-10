/////////////////////////////////////////////////////////////////////////////
// Name:        wxInputBox.h
// Purpose:     Show validation state
// Author:      http://wxwidgets.info/?q=data_validation_tutorial_ru
// Modified by:
// Copyright:   (c) 2006 http://wxwidgets.info/?q=data_validation_tutorial_ru
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_INPUT_BOX_H
#define _WX_INPUT_BOX_H

#include <wx/bitmap.h>
#include <wx/control.h>
#include <wx/statbmp.h>
#include <wx/validate.h>

#define wxInputBoxName wxT("wxInputBox")

enum wxInputBoxBitmapType
{
	wxINPUTBOX_OK,
	wxINPUTBOX_ERROR
};

class wxInputBox : public wxControl
{
	DECLARE_DYNAMIC_CLASS(wxInputBox);
	wxStaticBitmap * m_StatusBitmap;
	wxWindow * m_Editor;
	wxBitmap m_OKBitmap;
	wxBitmap m_ErrorBitmap;
public:
	wxInputBox();
	wxInputBox(wxWindow * parent, wxWindowID id,
		const wxPoint & pos = wxDefaultPosition,
		const wxSize & size = wxDefaultSize,
		long style = wxNO_BORDER,
		const wxValidator & validator = wxDefaultValidator,
		const wxString & name = wxInputBoxName);
	wxInputBox(wxWindow * parent, wxWindowID id, wxWindow * editor,
		const wxBitmap & ok_bitmap = wxNullBitmap,
		const wxBitmap & error_bitmap = wxNullBitmap,
		const wxPoint & pos = wxDefaultPosition,
		const wxSize & size = wxDefaultSize,
		long style = wxNO_BORDER,
		const wxValidator & validator = wxDefaultValidator,
		const wxString & name = wxInputBoxName);
	bool Create(wxWindow * parent, wxWindowID id = wxID_ANY,
		wxWindow * editor = NULL,
		const wxBitmap & ok_bitmap = wxNullBitmap,
		const wxBitmap & error_bitmap = wxNullBitmap,
		const wxPoint & pos = wxDefaultPosition,
		const wxSize & size = wxDefaultSize,
		long style = wxNO_BORDER,
		const wxValidator & validator = wxDefaultValidator,
		const wxString & name = wxInputBoxName);

	virtual bool Validate();
	virtual void SetValidator(const wxValidator& validator);
	void Update();

	static wxBitmap GetDefaultStatusBitmap(wxInputBoxBitmapType bitmap_type);
	void SetEditor(wxWindow * editor);

	wxBitmap GetStatusBitmap(wxInputBoxBitmapType bitmap_type);
	void SetStatusBitmap(wxInputBoxBitmapType bitmap_type,
		const wxBitmap & bitmap);

	wxValidator * GetValidator() const;

	DECLARE_EVENT_TABLE()
	void OnValueChanged(wxCommandEvent & event);
	void OnSize(wxSizeEvent & event);
};

#endif
