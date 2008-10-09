/////////////////////////////////////////////////////////////////////////////
// Name:        wxInputBox.cpp
// Purpose:     Show validation state
// Author:      http://wxwidgets.info/?q=data_validation_tutorial_ru
// Modified by:
// Copyright:   (c) 2006 http://wxwidgets.info/?q=data_validation_tutorial_ru
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef WXHELP_BASE

#include "wxInputBox.h"

#include <wx/combo.h>

#include "silence.h"

const
#include "inputbox_ok.xpm"
const
#include "inputbox_error.xpm"

IMPLEMENT_DYNAMIC_CLASS(wxInputBox, wxControl);

BEGIN_EVENT_TABLE(wxInputBox, wxControl)
EVT_SIZE(wxInputBox::OnSize)
END_EVENT_TABLE()

wxInputBox::wxInputBox()
: wxControl(), m_Editor(NULL)
{
}

wxInputBox::wxInputBox(wxWindow * parent, wxWindowID id, wxWindow * editor,
		const wxBitmap & ok_bitmap, const wxBitmap & error_bitmap,
		const wxPoint & pos, const wxSize & size, long style,
		const wxValidator & validator, const wxString & name)
: m_Editor(NULL)
{
	Create(parent, id, editor, ok_bitmap, error_bitmap,
		pos, size, style, validator, name);
}

wxInputBox::wxInputBox(wxWindow * parent, wxWindowID id, const wxPoint & pos,
		const wxSize & size, long style,
		const wxValidator & validator, const wxString & name)
: m_Editor(NULL)
{
	Create(parent, id, NULL, wxNullBitmap, wxNullBitmap,
		pos, size, style, wxDefaultValidator, name);
}

bool wxInputBox::Create(wxWindow * parent, wxWindowID id, wxWindow * editor,
		const wxBitmap & ok_bitmap, const wxBitmap & error_bitmap,
		const wxPoint & pos, const wxSize & size, long style,
		const wxValidator & validator, const wxString & name)
{
	bool res = wxWindow::Create(parent, id, pos, size, style, name);
	if(res)
	{
		m_OKBitmap = ok_bitmap;
		m_ErrorBitmap = error_bitmap;
		if(!m_OKBitmap.Ok()) m_OKBitmap =
			wxInputBox::GetDefaultStatusBitmap(wxINPUTBOX_OK);
		if(!m_ErrorBitmap.Ok()) m_ErrorBitmap =
			wxInputBox::GetDefaultStatusBitmap(wxINPUTBOX_ERROR);
		wxBoxSizer * sizer = new wxBoxSizer(wxHORIZONTAL);
		SetSizer(sizer);
		if(editor)
		{
			SetEditor(editor);
			m_Editor->SetValidator(validator);
		}
	}
	return res;
}

void wxInputBox::SetEditor(wxWindow * editor)
{
	if(GetSizer())
	{
		GetSizer()->Clear(true);
	}
	wxBoxSizer * sizer = new wxBoxSizer(wxHORIZONTAL);
	SetSizer(sizer);
	wxBitmap bitmap;
	if(editor)
	{
		m_Editor = editor;

		wxSizer *editor_sizer = editor->GetContainingSizer();
		if (editor_sizer) {
		    // The editor is already in a sizer environment
		    if (GetContainingSizer()) {
		        // The input box is also in a sizer, so we just detach the editor
		        editor_sizer->Detach(editor);
		        editor_sizer->Layout();
		    } else {
		        // Need to "resizer"
		        editor_sizer->Replace(editor, this);
		    }
		}

		m_Editor->Reparent(this);
		sizer->Add(m_Editor, 1, wxEXPAND|wxRIGHT, 5);
		if(m_Editor->IsKindOf(CLASSINFO(wxTextCtrl)))
		{
			m_Editor->Connect(m_Editor->GetId(),
				wxEVT_COMMAND_TEXT_UPDATED,
				(wxObjectEventFunction)&wxInputBox::OnValueChanged);
		} else if(m_Editor->IsKindOf(CLASSINFO(wxComboCtrl))) {
		    // Need to connect to this due to wxComboCtrls event pass-through handling of
		    // the text control events.
			Connect(m_Editor->GetId(),
				wxEVT_COMMAND_TEXT_UPDATED,
				(wxObjectEventFunction)&wxInputBox::OnValueChanged);
            // For a wxComboCtrl that sends EVT_COMBOBOX
			Connect(m_Editor->GetId(),
				wxEVT_COMMAND_COMBOBOX_SELECTED,
				(wxObjectEventFunction)&wxInputBox::OnValueChanged);
            // For a wxComboCtrl that uses a popupmenu
			Connect(m_Editor->GetId(),
				wxEVT_COMMAND_MENU_SELECTED,
				(wxObjectEventFunction)&wxInputBox::OnValueChanged);
		} else if(m_Editor->IsKindOf(CLASSINFO(wxListBox))) {
			m_Editor->Connect(m_Editor->GetId(),
				wxEVT_COMMAND_LISTBOX_SELECTED,
				(wxObjectEventFunction)&wxInputBox::OnValueChanged);
		} else if(m_Editor->IsKindOf(CLASSINFO(wxChoice))) {
			m_Editor->Connect(m_Editor->GetId(),
				wxEVT_COMMAND_CHOICE_SELECTED,
				(wxObjectEventFunction)&wxInputBox::OnValueChanged);
		}
        wxValidator* validator = m_Editor->GetValidator();
        if(validator)
        {
            wxSilencer sil(dynamic_cast<wxSilent*> (validator));
			if(validator->Validate(this)) {
				bitmap = GetStatusBitmap(wxINPUTBOX_OK);
			} else {
			    bitmap = GetStatusBitmap(wxINPUTBOX_ERROR);
			}
		} else bitmap = GetStatusBitmap(wxINPUTBOX_OK);
	}
	else
	{
		bitmap = wxInputBox::GetStatusBitmap(wxINPUTBOX_ERROR);
		sizer->Add(100, -1, 1, wxEXPAND|wxRIGHT, 5);
	}
	m_StatusBitmap = new wxStaticBitmap(this, wxID_ANY, bitmap);
	sizer->Add(m_StatusBitmap, 0, wxALIGN_CENTER);
	sizer->Fit(this);
	SetBestSize(GetSize());

	if (GetContainingSizer())
        GetContainingSizer()->Layout();
}

wxBitmap wxInputBox::GetStatusBitmap(wxInputBoxBitmapType bitmap_type)
{
	switch(bitmap_type)
	{
	case wxINPUTBOX_OK:
		if(m_OKBitmap.Ok())
		{
			return m_OKBitmap;
		}
	case wxINPUTBOX_ERROR:
		if(m_ErrorBitmap.Ok())
		{
			return m_ErrorBitmap;
		}
	default:
		break;
	}
	return wxInputBox::GetDefaultStatusBitmap(bitmap_type);
}

void wxInputBox::SetStatusBitmap(wxInputBoxBitmapType bitmap_type,
								 const wxBitmap & bitmap)
{
	switch(bitmap_type)
	{
	case wxINPUTBOX_OK:
		m_OKBitmap = bitmap;
		break;
	case wxINPUTBOX_ERROR:
		m_ErrorBitmap = bitmap;
		break;
	default:
		break;
	}
}

wxBitmap wxInputBox::GetDefaultStatusBitmap(wxInputBoxBitmapType bitmap_type)
{
	if(!wxImage::FindHandler(wxBITMAP_TYPE_XPM))
	{
		wxImage::AddHandler(new wxXPMHandler);
	}
	switch(bitmap_type)
	{
	case wxINPUTBOX_OK:
		return wxBitmap(inputbox_ok_xpm);
	case wxINPUTBOX_ERROR:
		return wxBitmap(inputbox_error_xpm);
	default:
		break;
	}

	return wxBitmap(16,16);
}

void wxInputBox::OnValueChanged(wxCommandEvent & event)
{
//::wxMessageBox(wxT("wxInputBox::OnValueChanged"));
	wxWindow * target = wxDynamicCast(event.GetEventObject(), wxWindow);
	if(target)
	{
		wxInputBox * parent = wxDynamicCast(target->GetParent(), wxInputBox);
		if(parent)
		{
			wxValidator * validator = target->GetValidator();
			wxInputBoxBitmapType bitmap_type = wxINPUTBOX_ERROR;
			if(validator)
			{
			    wxSilencer sil(dynamic_cast<wxSilent*> (validator));
				if(validator->Validate(this))
				{
					bitmap_type = wxINPUTBOX_OK;
				}
				parent->m_StatusBitmap->SetBitmap(
					parent->GetStatusBitmap(bitmap_type));
			}
		}
	}
	event.Skip();
}

void wxInputBox::OnSize(wxSizeEvent & event)
{
	Layout();
}

bool wxInputBox::Validate()
{
	if(m_Editor)
	{
		wxValidator * validator = m_Editor->GetValidator();
		if(validator)
		{
			return validator->Validate(this);
		}
	}
	return true;
}

void wxInputBox::Update()
{
    wxInputBoxBitmapType bitmap_type = wxINPUTBOX_OK;
	if(m_Editor) {
		wxValidator * validator = m_Editor->GetValidator();
		if(validator) {
            wxSilencer sil(dynamic_cast<wxSilent*> (validator));
			if (!validator->Validate(this))
                bitmap_type = wxINPUTBOX_ERROR;
		}
	}
    m_StatusBitmap->SetBitmap(GetStatusBitmap(bitmap_type));
}

void wxInputBox::SetValidator(const wxValidator& validator)
{
	if(m_Editor)
	{
		m_Editor->SetValidator(validator);
	}
}

wxValidator* wxInputBox::GetValidator() const
{
	if(m_Editor)
	{
		return m_Editor->GetValidator();
	}
	return NULL;
}

#endif
