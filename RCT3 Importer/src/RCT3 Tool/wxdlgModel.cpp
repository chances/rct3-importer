///////////////////////////////////////////////////////////////////////////////
//
// Model Settings Dialog
// Copyright (C) 2006 Tobias Minch, Jonathan Wilson
//
// Part of rct3tool
// Copyright 2005 Jonathan Wilson
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
// Written by
//   Tobias Minich - belgabor@gmx.de
//
// Parts of this code are based on code written for rct3tool (Copyright 2005
// Jonathan Wilson).
//
///////////////////////////////////////////////////////////////////////////////

#include "wxdlgModel.h"

#include <wx/popupwin.h>
#include <wx/renderer.h>
#include <wx/valgen.h>

//#include <ssg.h>

#include "choicewrapper.h"
#include "comboboxwrapper.h"
#include "confhelp.h"
#include "htmlentities.h"
#include "lib3Dconfig.h"
#include "matrix.h"
#include "pretty.h"
#include "SCNFile.h"
#include "valext.h"
#include "wrappereditevent.h"
#include "wxInputBox.h"

#include "wxapp.h"
#include "wxdlgCreateScenery.h"
#include "wxdlgEffect.h"
//#include "wxdlgInfo.h"
#include "wxdlgMatrix.h"

using namespace pretty;
using namespace r3;
using namespace std;

////////////////////////////////////////////////////////////////////////
//
//  wxMeshListModel
//
////////////////////////////////////////////////////////////////////////

class wxMeshListModel : public wxDataViewListModel {
public:
    wxMeshListModel(/*wxWindow *parent,*/ cModel *content/*, wxArrayString& textures*/):wxDataViewListModel() {
        m_content = content;
//        m_parent = parent;
//        m_textures = textures;
    };
    virtual unsigned int GetNumberOfRows();
    virtual unsigned int GetNumberOfCols();
    virtual wxString GetColType( unsigned int col );
    virtual void GetValue( wxVariant &variant, unsigned int col, unsigned int row );
    virtual bool SetValue( wxVariant &variant, unsigned int col, unsigned int row );
//    wxWindow* GetParent() const {return m_parent;};
    wxArrayString m_textures;
private:
    cModel* m_content;
//    wxWindow* m_parent;
};

unsigned int wxMeshListModel::GetNumberOfRows() {
    if (m_content)
        return m_content->meshstructs.size();
    else
        return 0;
}

unsigned int wxMeshListModel::GetNumberOfCols() {
    return 8;
}

wxString wxMeshListModel::GetColType( unsigned int col ) {
    switch (col) {
        case 0:
            return wxT("bool");
        case 4:
        case 5:
        case 6:
        case 7:
            return wxT("long");
        default:
            return wxT("string");
    }
}

void wxMeshListModel::GetValue( wxVariant &variant, unsigned int col, unsigned int row ) {
    if (m_content && (row < m_content->meshstructs.size())) {
        switch (col) {
            case 0:
                variant = (void *) &m_content->meshstructs[row];
                break;
            case 1:
                variant = (void *) &m_content->meshstructs[row];
                break;
            case 2:
                variant = (void *) &m_content->meshstructs[row];
                break;
            case 3:
                variant = (void *) &m_content->meshstructs[row];
                break;
            case 4:
                variant = (void *) &m_content->meshstructs[row];
                break;
            case 5:
                variant = (void *) &m_content->meshstructs[row];
                break;
            case 6:
                variant = (void *) &m_content->meshstructs[row];
                break;
            case 7:
                variant = (void *) &m_content->meshstructs[row];
                break;
            default:
                variant = wxT("Error");
        }
    }
}

bool wxMeshListModel::SetValue( wxVariant &variant, unsigned int col, unsigned int row ) {
    if (m_content && (row < m_content->meshstructs.size())) {
        switch (col) {
            case 0: {
                    long sel = variant.GetLong();
                    switch (sel) {
                        case 1:
                            for(long it = 0; it < m_content->meshstructs.size(); it++) {
                                if (it == row) {
                                    if (m_content->meshstructs[it].disabled) {
                                        m_content->meshstructs[it].disabled = false;
                                        RowChanged(it);
/*
                                        for(int i = 0; i < 7; i++)
                                            ValueChanged(i, it);
*/
                                    }
                                } else if (m_content->meshstructs[it].valid && !m_content->meshstructs[it].disabled) {
                                    m_content->meshstructs[it].disabled = true;
                                    RowChanged(it);
/*
                                    for(int i = 0; i < 7; i++)
                                        ValueChanged(i, it);
*/
                                }
                            }
                            break;
                        case 2:
                            if (::wxMessageBox(_("Do you want to apply the settings of the currently selected mesh to all?"), _("Question"), wxYES_NO|wxNO_DEFAULT|wxICON_QUESTION)==wxYES) {
                                for(long it = 0; it < m_content->meshstructs.size(); it++) {
                                    if ((it != row) && m_content->meshstructs[it].valid) {
                                        m_content->meshstructs[it].CopySettingsFrom(m_content->meshstructs[row]);
                                        RowChanged(it);
/*
                                        for(int i = 0; i < 7; i++)
                                           ValueChanged(i, it);
*/
                                    }
                                }
                            }
                            break;
                        default:
                            ::wxMessageBox(_("Illegal Selection"), _("Internal Error"), wxICON_ERROR);
                    }
                }
                break;
            case 2:
                m_content->meshstructs[row].FTX = variant.GetString();
                RowChanged(row);
/*
                for(int i = 0; i < 7; i++)
                    ValueChanged(i, row);
*/
                break;
            case 3:
                m_content->meshstructs[row].TXS = variant.GetString();
                {
                    unsigned long right_value = 0;
                    if (m_content->meshstructs[row].TXS.StartsWith(wxT("SIAlphaMask")))
                        right_value = 1;
                    else if (m_content->meshstructs[row].TXS.StartsWith(wxT("SIAlpha")) || m_content->meshstructs[row].TXS.IsSameAs(wxT("SIGlass")) || m_content->meshstructs[row].TXS.IsSameAs(wxT("SIWater")))
                        right_value = 2;
                    m_content->meshstructs[row].place = right_value;
                }
                RowChanged(row);
/*
                for(int i = 0; i < 7; i++)
                    ValueChanged(i, row);
*/
                break;
            case 4:
                m_content->meshstructs[row].place = variant.GetLong();
                break;
            case 5: {
                    long sel = variant.GetLong();
                    switch (sel) {
                        case 1:
                            m_content->meshstructs[row].flags = 12;
                            m_content->meshstructs[row].TXS = wxT("SIOpaqueUnlit");
                            break;
                        case 2:
                            m_content->meshstructs[row].flags = 12288;
                            m_content->meshstructs[row].FTX = wxT("useterraintexture");
                            m_content->meshstructs[row].TXS = wxT("SIOpaque");
                            break;
                        case 3:
                            m_content->meshstructs[row].flags = 20480;
                            m_content->meshstructs[row].FTX = wxT("useclifftexture");
                            m_content->meshstructs[row].TXS = wxT("SIOpaque");
                            break;
                        case 4:
                            m_content->meshstructs[row].flags = 0;
                            m_content->meshstructs[row].FTX = wxT("siwater");
                            m_content->meshstructs[row].TXS = wxT("SIWater");
                            break;
                        case 5:
                            m_content->meshstructs[row].flags = 0;
                            m_content->meshstructs[row].FTX = wxT("watermask");
                            m_content->meshstructs[row].TXS = wxT("SIFillZ");
                            break;
                        case 6:
                            m_content->meshstructs[row].flags = 32788;
                            m_content->meshstructs[row].FTX = wxT("UseAdTexture");
                            m_content->meshstructs[row].TXS = wxT("SIOpaque");
                            break;
                        case 7:
                            m_content->meshstructs[row].flags = 32768;
                            m_content->meshstructs[row].FTX = wxT("UseAdTexture");
                            m_content->meshstructs[row].TXS = wxT("SIOpaque");
                            break;
						case 8:
                            m_content->meshstructs[row].flags = r3::Constants::Mesh::Flags::AnimSlow;
							break;
						case 9:
                            m_content->meshstructs[row].flags = r3::Constants::Mesh::Flags::AnimMed;
							break;
						case 10:
                            m_content->meshstructs[row].flags = r3::Constants::Mesh::Flags::AnimFast;
							break;
                        default:
                            {
                                m_content->meshstructs[row].flags = 0;
                                if ((m_content->meshstructs[row].FTX == "useterraintexture") ||
                                    (m_content->meshstructs[row].FTX == "useclifftexture") ||
                                    (m_content->meshstructs[row].FTX == "siwater") ||
                                    (m_content->meshstructs[row].FTX == "watermask") ||
                                    (m_content->meshstructs[row].FTX == "UseAdTexture")) {
                                        m_content->meshstructs[row].FTX = "";
                                    }
                            }
                    }
/*                    for(int i = 0; i < 7; i++)
                        ValueChanged(i, row);
                        */
                    RowChanged(row);
                }
                break;
            case 6:
                if (variant.GetLong()==0)
                    m_content->meshstructs[row].unknown = 1;
                else
                    m_content->meshstructs[row].unknown = 3;
                break;
            case 7:
                m_content->meshstructs[row].fudgenormals = variant.GetLong();
                break;
        }
    }
    return true;
}

////////////////////////////////////////////////////////////////////////
//
//  wxGetSelRenderer
//
////////////////////////////////////////////////////////////////////////

class wxGetSelRenderer: public wxDataViewCustomRenderer {
public:
    wxGetSelRenderer(dlgModel* dialog, const wxString& varianttype = wxT("string"), wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT):
                        wxDataViewCustomRenderer(varianttype, mode) {
        m_value = NULL;
        m_dialog = dialog;
    };

protected:
    cMeshStruct* m_value;
    dlgModel* m_dialog;
	
	bool LockFtx() {
		long x = GetSel();
		return ((x>=2) && (x<8));
	}
	
	bool LockTxs() {
		long x = GetSel();
		return ((x>=2) && (x<8));		
	}
	
    long GetSel() {
        long sel = 0;
        if (m_value->flags == 0) {
			if (m_value->FTX != wxT("")) {
				if (m_value->FTX.CmpNoCase("siwater") == 0) {
					sel = 4;
				}
				if (m_value->FTX.CmpNoCase("watermask") == 0) {
					sel = 5;
				}
			} else
				sel = 0;
        }
        if (m_value->flags == 12) {
            sel = 1;
        }
        if (m_value->flags == r3::Constants::Mesh::Flags::AnimSlow) {
            sel = 8;
        }
        if (m_value->flags == r3::Constants::Mesh::Flags::AnimMed) {
            sel = 9;
        }
        if (m_value->flags == r3::Constants::Mesh::Flags::AnimFast) {
            sel = 10;
        }
        if (m_value->flags == 12288) {
            sel = 2;
        }
        if (m_value->flags == 20480) {
            sel = 3;
        }
        if (m_value->flags == 32788) {
            sel = 6;
        }
        if (m_value->flags == 32768) {
            sel = 7;
        }
        return sel;
    };

    wxColour GetStateColour(wxColour colFg, int WXUNUSED(state)) const {
/*
        if (state & wxDATAVIEW_CELL_SELECTED) {
            wxColour col;
            if (colFg == *wxBLACK)
                col = *wxWHITE;
            else
                col = wxColour(((int) colFg.Red() + 255) / 2, ((int) colFg.Green() + 255) / 2, ((int) colFg.Blue() + 255) / 2);
            return col;
        } else
*/
            return colFg;
    }
};

////////////////////////////////////////////////////////////////////////
//
//  wxMeshListEnableRenderer
//
////////////////////////////////////////////////////////////////////////

class wxMeshListEnableRenderer: public wxDataViewCustomRenderer
{
public:
    wxMeshListEnableRenderer():wxDataViewCustomRenderer( wxT("bool"), wxDATAVIEW_CELL_INERT ) {
        m_value = NULL;
    }
    bool SetValue( const wxVariant &value ) {
        m_value = (cMeshStruct *) value.GetVoidPtr();
        return true;
    }
    bool Render( wxRect cell, wxDC *dc, int WXUNUSED(state) ) {
        if (m_value) {
            // User wxRenderer here
            if (m_value->valid) {
/*
                wxRect rect;
                rect.x = cell.x + cell.width/2 - 6;
                rect.width = 12;
                rect.y = cell.y + cell.height/2 - 6;
                rect.height = 12;

                wxBrush s = dc->GetBrush();
                dc->SetBrush(*wxWHITE_BRUSH);
                dc->DrawRectangle(rect);

                //int flags = 0;
                if (!m_value->disabled) {
                    //flags |= wxCONTROL_CHECKED;
                    dc->DrawLine(rect.x, rect.y, rect.x + rect.width, rect.y + rect.height);
                    dc->DrawLine(rect.x, rect.y + rect.height - 1, rect.x + rect.width, rect.y - 1 );
                }
                dc->SetBrush(s);
*/
                wxCoord x = cell.x + (cell.width-14)/2;
                wxCoord y = cell.y + (cell.height-14)/2;
                if (!m_value->disabled) {
                    if ((m_value->FTX == wxT("")) || (m_value->TXS == wxT(""))) {
                        dc->DrawBitmap(wxXmlResource::Get()->LoadBitmap(wxT("yellowled_16x16")), x, y);
                    } else {
                        dc->DrawBitmap(wxXmlResource::Get()->LoadBitmap(wxT("greenled_16x16")), x, y);
                    }
                } else
                    dc->DrawBitmap(wxXmlResource::Get()->LoadBitmap(wxT("redled_16x16")), x, y);
            } else {
/*
                wxCoord x = cell.x + cell.width/2;
                wxCoord y = cell.y + cell.height/2;
                wxBrush s = dc->GetBrush();
                if (m_value->effectpoint)
                    dc->SetBrush(*wxGREEN_BRUSH);
                else
                    dc->SetBrush(*wxRED_BRUSH);
                dc->DrawCircle(x, y, 4);
                dc->SetBrush(s);
*/
                wxCoord x = cell.x + (cell.width-16)/2;
                wxCoord y = cell.y + (cell.height-16)/2;
//                if (m_value->effectpoint) {
//                    dc->DrawBitmap(wxXmlResource::Get()->LoadBitmap(wxT("virus_detected_2_16x16")), x, y);
//                } else {
                    dc->DrawBitmap(wxXmlResource::Get()->LoadBitmap(wxT("virus_detected_16x16")), x, y);
//                }
            }
        }
        return true;
    }
    wxSize GetSize() {
        return wxSize(20,20);
    }
    /*
    bool Activate( wxRect WXUNUSED(rect),
                   wxDataViewListModel *model,
                   unsigned int WXUNUSED(col),
                   unsigned int row) {
        bool ret = false;
        if (m_value) {
            if (m_value->valid) {
                if (::wxGetKeyState(WXK_SHIFT)) {
                    wxVariant var = (long) 1;
                    model->SetValue(var, 0, row);
                } else if (::wxGetKeyState(WXK_CONTROL)) {
                    wxVariant var = (long) 2;
                    model->SetValue(var, 0, row);
                } else {
                    m_value->disabled = !m_value->disabled;
                    //model->RowChanged(row); // Not worky -.-
                    for(int i = 0; i < 7; i++)
                        model->ValueChanged(i, row);
                }
                ret = true;
            }
        }
        return ret;
    }
*/

    bool LeftClick(wxPoint WXUNUSED(cursor), wxRect WXUNUSED(cell), wxDataViewListModel* model, unsigned int WXUNUSED(col), unsigned int row) {
        bool ret = false;
        if (m_value) {
            if (m_value->valid) {
                m_value->disabled = !m_value->disabled;
                model->RowChanged(row);
                ret = true;
            }
        }
        return ret;
    }

private:
    cMeshStruct* m_value;
};

////////////////////////////////////////////////////////////////////////
//
//  wxMeshListNameRenderer
//
////////////////////////////////////////////////////////////////////////

class wxMeshListNameRenderer: public wxGetSelRenderer
{
public:
    wxMeshListNameRenderer(dlgModel* dialog, wxSize& max):wxGetSelRenderer( dialog, wxT("string"), wxDATAVIEW_CELL_ACTIVATABLE ) {
        m_max = max;
        m_value = NULL;
    }
    bool SetValue( const wxVariant &value ) {
        m_value = (cMeshStruct *) value.GetVoidPtr();
        return true;
    }
    bool Render( wxRect rect, wxDC *dc, int state ) {
/*
        if (state)
            ::wxMessageBox(wxString::Format(wxT("%d"), state));
*/
        if (m_value) {
            wxDCClipper clip(*dc, rect);
            wxCoord w, h;
            dc->GetTextExtent(m_value->Name, &w, &h);
            wxCoord top = (rect.height - h) / 2;
            if (!m_value->valid)
                dc->SetTextForeground(GetStateColour(wxColor(wxT("#CC0000")), state));
            else if (m_value->disabled)
                dc->SetTextForeground(GetStateColour(wxColor(wxT("#808080")), state));
            else if ((m_value->FTX == wxT(""))
                     || (m_value->TXS == wxT("")))
                dc->SetTextForeground(GetStateColour(wxColor(wxT("#FF3C3C")), state));
            else {
                dc->SetTextForeground(GetStateColour(wxColor(wxT("#000000")), state));
            }
            dc->DrawText( m_value->Name, rect.x+5, rect.y + top );
            if (m_value->multibone)
                dc->DrawBitmap(wxXmlResource::Get()->LoadBitmap(wxT("multibone_on_16x16")), rect.GetRight()-16, rect.GetBottom()-16);
            else if (m_value->boneassignment.size() > 1)
                dc->DrawBitmap(wxXmlResource::Get()->LoadBitmap(wxT("multibone_off_16x16")), rect.GetRight()-16, rect.GetBottom()-16);
        }
        return true;
    }
    wxSize GetSize() {
        return m_max;
    }
    bool Activate( wxRect WXUNUSED(rect),
                   wxDataViewListModel *model,
                   unsigned int WXUNUSED(col),
                   unsigned int row) {
        bool ret = false;
        if (m_value) {
            if (m_value->valid) {
                if (::wxGetKeyState(WXK_SHIFT)) {
                    wxVariant var = (long) 1;
                    model->SetValue(var, 0, row);
                } else if (::wxGetKeyState(WXK_CONTROL)) {
                    wxVariant var = (long) 2;
                    model->SetValue(var, 0, row);
                } else if (::wxGetKeyState(WXK_ALT)) {
                    m_value->multibone = true;
                } else {
                    if ((m_value->boneassignment.size() > 1) || m_value->multibone)
                        m_value->multibone = !m_value->multibone;
                    else
                        m_value->disabled = !m_value->disabled;
                    model->RowChanged(row);
/*
                    for(int i = 0; i < 7; i++)
                        model->ValueChanged(i, row);
*/
                }
                ret = true;
            }
        }
        return ret;
    }
private:
    cMeshStruct* m_value;
    wxSize m_max;
};

////////////////////////////////////////////////////////////////////////
//
//  wxMeshListFTXRenderer
//
////////////////////////////////////////////////////////////////////////
/*
class wxMeshListFTXListBox : public wxHtmlListBox, public wxComboPopup {
public:
    wxMeshListFTXListBox():wxHtmlListBox()  {
        Init();
    }

    void SetStringList(const wxArrayString& contents) {
        m_contents = contents;
        SetItemCount(m_contents.size());
    };


    virtual bool Create( wxWindow* parent ) {
        return wxHtmlListBox::Create(parent,1,
                                  wxPoint(0,0),wxDefaultSize);
    }

    virtual void Init() {
        m_value = -1;
        m_itemHere = -1; // hot item in list
    }

    virtual wxWindow *GetControl() { return this; }

    virtual void SetStringValue( const wxString& s ) {
        int n = m_contents.Index(s);
        if ( n >= 0 && n < GetItemCount() )
            SetSelection(n);
    }

    virtual wxString GetStringValue() const {
        if (m_value >= 0)
            return m_contents[m_value];
        return wxEmptyString;
    }

    //
    // Popup event handlers
    //

    // Mouse hot-tracking
    void OnMouseMove(wxMouseEvent& event) {
        // Move selection to cursor if it is inside the popup

        int itemHere = HitTest(event.GetPosition());
        if ( itemHere >= 0 )
        {
            SetSelection(itemHere);
            m_itemHere = itemHere;
        }
        event.Skip();
    }

    // On mouse left, set the value and close the popup
    void OnMouseClick(wxMouseEvent& WXUNUSED(event))
    {
        m_value = m_itemHere;
        Dismiss();
    }

    //
    // Utilies for item manipulation
    //

    void AddSelection( const wxString& selstr )
    {
        m_contents.Add(selstr);
        SetItemCount(m_contents.size());
        RefreshAll();
    }

private:
    wxArrayString   m_contents;
    int             m_value; // current item index
    int             m_itemHere; // hot item in popup
protected:
    virtual wxString OnGetItem(size_t n) const {
        return wxT("<font size='2'>")+wxEncodeHtmlEntities(m_contents[n])+wxT("</font>");
    }

private:
    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxMeshListFTXListBox, wxHtmlListBox)
    EVT_MOTION(wxMeshListFTXListBox::OnMouseMove)
    EVT_LEFT_UP(wxMeshListFTXListBox::OnMouseClick)
END_EVENT_TABLE()
*/
////////////////

class wxMeshListFTXRenderer: public wxGetSelRenderer
{
public:
    wxMeshListFTXRenderer(dlgModel* dialog, wxSize& max, wxArrayString *textures):wxGetSelRenderer( dialog, wxT("string"), wxDATAVIEW_CELL_ACTIVATABLE ) {
        m_max = max;
        m_value = NULL;
        m_textures = textures;
    }
    bool SetValue( const wxVariant &value ) {
        m_value = (cMeshStruct *) value.GetVoidPtr();
        return true;
    }
    bool Render( wxRect rect, wxDC *dc, int state ) {
        if (m_value) {
            if (m_value->valid) {
                wxDCClipper clip(*dc, rect);
                wxCoord w, h;
                dc->GetTextExtent(m_value->FTX, &w, &h);
                wxCoord top = (rect.height - h) / 2;
                if (m_value->disabled) {
                    dc->SetTextForeground(GetStateColour(wxColor(wxT("#808080")), state));
                } else {
                    dc->SetTextForeground(GetStateColour(wxColor(wxT("#000000")), state));
                }
                dc->DrawText( m_value->FTX, rect.x+5, rect.y + top );
                if (LockFtx())
                    dc->DrawBitmap(wxXmlResource::Get()->LoadBitmap(wxT("admin_icon_16x16")), rect.x+2, rect.GetBottom()-12);
            }
        }
        return true;
    }
    bool Activate( wxRect cell, wxDataViewListModel *model, unsigned int col, unsigned int row ) {
        if (m_value && (!LockFtx())) {
            if (m_value->valid) {
//            wxComboCtrl* combo = new wxComboCtrl();
//            wxMeshListFTXListBox* list = new wxMeshListFTXListBox();
                wxComboBox* combo = new wxComboBox();
                if (m_textures->Index(m_value->FTX) == wxNOT_FOUND)
                    m_textures->Add(m_value->FTX);
                wxComboBoxWrapper* popup = new wxComboBoxWrapper(GetOwner()->GetOwner()->GetChildren()[0], m_value->FTX, model, combo, m_textures, col, row, cell);
//            list->SetStringList(*m_textures);
//            combo->UseAltPopupWindow();
//            combo->SetPopupControl(list);

/*
            wxMeshListFTXRendererPopupTransient *popup = new wxMeshListFTXRendererPopupTransient(
                GetOwner()->GetOwner()->GetParent(), &val, model, col, row);
            popup->SetSize(cell.GetWidth(), cell.GetHeight());
            dynamic_cast<wxMeshListModel*>(model)->GetParent()->ClientToScreen(&cell.x, &cell.y);
            wxPoint pos = wxPoint(cell.x, cell.y);
            popup->Move( pos );
            popup->Layout();
            popup->Popup( popup->m_edit );
*/
            }
        }
        return true;
    }
    wxSize GetSize() {
        return m_max;
    }
private:
    wxSize m_max;
    wxArrayString* m_textures;
};

////////////////////////////////////////////////////////////////////////
//
//  wxMeshListTXSRenderer
//
////////////////////////////////////////////////////////////////////////

class wxMeshListTXSRenderer: public wxGetSelRenderer
{
public:
    wxMeshListTXSRenderer(dlgModel* dialog, wxSize& max):wxGetSelRenderer( dialog, wxT("string"), wxDATAVIEW_CELL_ACTIVATABLE ) {
        m_max = max;
        m_value = NULL;
        if (wxMeshListTXSRenderer::m_styles.size()==0) {
            /*
			wxMeshListTXSRenderer::m_styles.Add(wxT("SIOpaque"));
			wxMeshListTXSRenderer::m_styles.Add(wxT("SIOpaqueDS"));
			wxMeshListTXSRenderer::m_styles.Add(wxT("SIAlpha"));
			wxMeshListTXSRenderer::m_styles.Add(wxT("SIAlphaDS"));
			wxMeshListTXSRenderer::m_styles.Add(wxT("SIAlphaMask"));
			wxMeshListTXSRenderer::m_styles.Add(wxT("SIAlphaMaskDS"));
			wxMeshListTXSRenderer::m_styles.Add(wxT("SIAlphaMaskLow"));
			wxMeshListTXSRenderer::m_styles.Add(wxT("SIEmissiveMask"));
			wxMeshListTXSRenderer::m_styles.Add(wxT("SIGlass"));
			wxMeshListTXSRenderer::m_styles.Add(wxT("SIWater"));
			wxMeshListTXSRenderer::m_styles.Add(wxT("SIOpaqueCaustics"));
			wxMeshListTXSRenderer::m_styles.Add(wxT("SIOpaqueCausticsGlossSpecular100"));
			wxMeshListTXSRenderer::m_styles.Add(wxT("SIOpaqueCausticsReflection"));
			wxMeshListTXSRenderer::m_styles.Add(wxT("SIOpaqueCausticsSpecular50"));
			wxMeshListTXSRenderer::m_styles.Add(wxT("SIOpaqueCausticsSpecular100"));
            wxMeshListTXSRenderer::m_styles.Add(wxT("SIOpaqueChrome"));
            wxMeshListTXSRenderer::m_styles.Add(wxT("SIOpaqueChromeModulate"));
            wxMeshListTXSRenderer::m_styles.Add(wxT("SIOpaqueGlossReflection"));
            wxMeshListTXSRenderer::m_styles.Add(wxT("SIOpaqueGlossSpecular50"));
            wxMeshListTXSRenderer::m_styles.Add(wxT("SIOpaqueGlossSpecular100"));
            wxMeshListTXSRenderer::m_styles.Add(wxT("SIOpaqueReflection"));
            wxMeshListTXSRenderer::m_styles.Add(wxT("SIOpaqueSpecular50"));
            wxMeshListTXSRenderer::m_styles.Add(wxT("SIOpaqueSpecular50Reflection"));
            wxMeshListTXSRenderer::m_styles.Add(wxT("SIOpaqueSpecular100"));
            wxMeshListTXSRenderer::m_styles.Add(wxT("SIOpaqueSpecular100Reflection"));
            wxMeshListTXSRenderer::m_styles.Add(wxT("SIOpaqueText"));
            wxMeshListTXSRenderer::m_styles.Add(wxT("SIOpaqueUnlit"));
            wxMeshListTXSRenderer::m_styles.Add(wxT("SIAlphaChrome"));
            wxMeshListTXSRenderer::m_styles.Add(wxT("SIAlphaReflection"));
            wxMeshListTXSRenderer::m_styles.Add(wxT("SIAlphaSpecular50"));
            wxMeshListTXSRenderer::m_styles.Add(wxT("SIAlphaSpecular50Reflection"));
            wxMeshListTXSRenderer::m_styles.Add(wxT("SIAlphaSpecular100"));
            wxMeshListTXSRenderer::m_styles.Add(wxT("SIAlphaSpecular100Reflection"));
            wxMeshListTXSRenderer::m_styles.Add(wxT("SIAlphaText"));
            wxMeshListTXSRenderer::m_styles.Add(wxT("SIAlphaUnlit"));
            wxMeshListTXSRenderer::m_styles.Add(wxT("SIAlphaMaskCaustics"));
            wxMeshListTXSRenderer::m_styles.Add(wxT("SIAlphaMaskChrome"));
            wxMeshListTXSRenderer::m_styles.Add(wxT("SIAlphaMaskReflection"));
            wxMeshListTXSRenderer::m_styles.Add(wxT("SIAlphaMaskSpecular50"));
            wxMeshListTXSRenderer::m_styles.Add(wxT("SIAlphaMaskSpecular50Reflection"));
            wxMeshListTXSRenderer::m_styles.Add(wxT("SIAlphaMaskSpecular100"));
            wxMeshListTXSRenderer::m_styles.Add(wxT("SIAlphaMaskSpecular100Reflection"));
            wxMeshListTXSRenderer::m_styles.Add(wxT("SIAlphaMaskUnlit"));
            wxMeshListTXSRenderer::m_styles.Add(wxT("SIAlphaMaskLowCaustics"));
            wxMeshListTXSRenderer::m_styles.Add(wxT("SIAlphaMaskLowSpecular50"));
            wxMeshListTXSRenderer::m_styles.Add(wxT("SIAlphaMaskLowSpecular50Reflection"));
            wxMeshListTXSRenderer::m_styles.Add(wxT("SIAlphaMaskLowSpecular100"));
            wxMeshListTXSRenderer::m_styles.Add(wxT("SIAlphaMaskLowSpecular100Reflection"));
            wxMeshListTXSRenderer::m_styles.Add(wxT("SIAlphaMaskUnlitLow"));
            wxMeshListTXSRenderer::m_styles.Add(wxT("SIEmissiveMaskReflection"));
            wxMeshListTXSRenderer::m_styles.Add(wxT("SIEmissiveMaskSpecular50"));
            wxMeshListTXSRenderer::m_styles.Add(wxT("SIEmissiveMaskSpecular50Reflection"));
            wxMeshListTXSRenderer::m_styles.Add(wxT("SIEmissiveMaskSpecular100"));
            wxMeshListTXSRenderer::m_styles.Add(wxT("SIEmissiveMaskSpecular100Reflection"));
            wxMeshListTXSRenderer::m_styles.Add(wxT("SIFillZ"));
            */

            wxMeshListTXSRenderer::m_styles.resize(cTextureStyle::getTextureStyles().size());
            copy(cTextureStyle::getTextureStyles().begin(), cTextureStyle::getTextureStyles().end(), wxMeshListTXSRenderer::m_styles.begin());
        }
    }
    bool SetValue( const wxVariant &value ) {
        m_value = (cMeshStruct *) value.GetVoidPtr();
        return true;
    }
    bool Render( wxRect rect, wxDC *dc, int state ) {
        if (m_value) {
            if (m_value->valid) {
                wxDCClipper clip(*dc, rect);
                wxCoord w, h;
                dc->GetTextExtent(m_value->TXS, &w, &h);
                wxCoord top = (rect.height - h) / 2;
                if (m_value->disabled) {
                    dc->SetTextForeground(GetStateColour(wxColor(wxT("#808080")), state));
                } else {
                    dc->SetTextForeground(GetStateColour(wxColor(wxT("#000000")), state));
                }
                dc->DrawText( m_value->TXS, rect.x+5, rect.y + top );
                if (LockTxs())
                    dc->DrawBitmap(wxXmlResource::Get()->LoadBitmap(wxT("admin_icon_16x16")), rect.x+2, rect.GetBottom()-12);
            }
        }
        return true;
    }
    bool Activate( wxRect cell, wxDataViewListModel *model, unsigned int col, unsigned int row ) {
        if (m_value && (!LockTxs())) {
            if (m_value->valid) {
                wxChoice* combo = new wxChoice();
                wxString val = m_value->TXS;
                if (val == wxT(""))
                    val = wxT("SIOpaque");
                wxChoiceWrapper* popup = new wxChoiceWrapper(GetOwner()->GetOwner()->GetChildren()[0], val, model, combo, &wxMeshListTXSRenderer::m_styles, col, row, cell, wxALIGN_RIGHT);
            }
        }
        return true;
    }
    wxSize GetSize() {
        return m_max;
    }
private:
    static wxArrayString m_styles;
    wxSize m_max;
};

wxArrayString wxMeshListTXSRenderer::m_styles;

////////////////////////////////////////////////////////////////////////
//
//  wxMeshListPlaceRenderer
//
////////////////////////////////////////////////////////////////////////

class wxMeshListPlaceRenderer: public wxGetSelRenderer
{
public:
    wxMeshListPlaceRenderer(dlgModel* dialog, wxSize& max):wxGetSelRenderer( dialog, wxT("string"), wxDATAVIEW_CELL_ACTIVATABLE ) {
        m_max = max;
        m_value = 0;
/*
        if (wxMeshListPlaceRenderer::m_styles.size()==0) {
			wxMeshListPlaceRenderer::m_styles.Add(_("Show Texture And Mesh When Placing"));
			wxMeshListPlaceRenderer::m_styles.Add(_("Show Only Texture When Placing"));
			wxMeshListPlaceRenderer::m_styles.Add(_("Unknown"));
        }
*/
    }
    bool SetValue( const wxVariant &value ) {
        m_value = (cMeshStruct *) value.GetVoidPtr();
        return true;
    }
    bool Render( wxRect rect, wxDC *dc, int state ) {
        if (m_value) {
            if (m_value->valid) {
                wxString s;
                switch (m_value->place) {
                    case 1:
                        s = _("masked");
                        break;
                    case 2:
                        s = _("regular");
                        break;
                    default:
                        s = _("none");
                }
                wxDCClipper clip(*dc, rect);
                wxCoord w, h;
                dc->GetTextExtent(s, &w, &h);
                wxCoord top = (rect.height - h) / 2;
                if (m_value->disabled) {
                    dc->SetTextForeground(GetStateColour(wxColor(wxT("#808080")), state));
                } else {
                    unsigned long right_value = cMeshStruct::getRightTransparencyValue(m_value->TXS);
                    if (m_value->place != right_value)
                        dc->SetTextForeground(GetStateColour(wxColor(wxT("#CC0000")), state));
                    else
                        dc->SetTextForeground(GetStateColour(wxColor(wxT("#000000")), state));
                }
                dc->DrawText( s, rect.x+5, rect.y + top );
            }
        }
        return true;
    }
    bool Activate( wxRect WXUNUSED(cell), wxDataViewListModel *model, unsigned int col, unsigned int row ) {
        if (m_value) {
            if (m_value->valid) {
                m_value->place++;
                if (m_value->place > 2)
                    m_value->place = 0;
                model->ValueChanged(col, row);
/*
                wxChoice* combo = new wxChoice();
                wxChoiceWrapper* popup = new wxChoiceWrapper(GetOwner()->GetOwner()->GetChildren()[0], (long) m_value->place, model, combo, &wxMeshListPlaceRenderer::m_styles, col, row, cell, wxALIGN_RIGHT);
*/
            }
        }
        return true;
    }
    wxSize GetSize() {
        return m_max;
    }
private:
    cMeshStruct* m_value;
//    static wxArrayString m_styles;
    wxSize m_max;
};

//wxArrayString wxMeshListPlaceRenderer::m_styles;

////////////////////////////////////////////////////////////////////////
//
//  wxMeshListFlagsRenderer
//
////////////////////////////////////////////////////////////////////////

class wxMeshListFlagsRenderer: public wxGetSelRenderer
{
public:
    wxMeshListFlagsRenderer(dlgModel* dialog, wxSize& max):wxGetSelRenderer( dialog, wxT("string"), wxDATAVIEW_CELL_ACTIVATABLE ) {
        m_max = max;
        m_value = 0;
        if (wxMeshListFlagsRenderer::m_styles.size()==0) {
			wxMeshListFlagsRenderer::m_styles.Add(_("None"));
			wxMeshListFlagsRenderer::m_styles.Add(_("Scrolling Sign"));
			wxMeshListFlagsRenderer::m_styles.Add(_("Terrain Texture"));
			wxMeshListFlagsRenderer::m_styles.Add(_("Cliffs Texture"));
			wxMeshListFlagsRenderer::m_styles.Add(_("Water Texture"));
			wxMeshListFlagsRenderer::m_styles.Add(_("Water Mask Texture"));
			wxMeshListFlagsRenderer::m_styles.Add(_("Billboard Texture"));
			wxMeshListFlagsRenderer::m_styles.Add(_("Animated Billboard"));
			wxMeshListFlagsRenderer::m_styles.Add(_("Auto-animated, slow"));
			wxMeshListFlagsRenderer::m_styles.Add(_("Auto-animated, medium"));
			wxMeshListFlagsRenderer::m_styles.Add(_("Auto-animated, fast"));
        }
    }
    bool SetValue( const wxVariant &value ) {
        m_value = (cMeshStruct *) value.GetVoidPtr();
        return true;
    }
    bool Render( wxRect rect, wxDC *dc, int state ) {
        if (m_value) {
            if (m_value->valid) {
                wxString s = wxMeshListFlagsRenderer::m_styles[GetSel()];
                wxDCClipper clip(*dc, rect);
                wxCoord w, h;
                dc->GetTextExtent(s, &w, &h);
                wxCoord top = (rect.height - h) / 2;
                if (m_value->disabled) {
                    dc->SetTextForeground(GetStateColour(wxColor(wxT("#808080")), state));
                } else {
                    dc->SetTextForeground(GetStateColour(wxColor(wxT("#000000")), state));
                }
                dc->DrawText( s, rect.x+5, rect.y + top );
            }
        }
        return true;
    }
    bool Activate( wxRect cell, wxDataViewListModel *model, unsigned int col, unsigned int row ) {
        if (m_value) {
            if (m_value->valid) {
                wxChoice* combo = new wxChoice();
                wxChoiceWrapper* popup = new wxChoiceWrapper(GetOwner()->GetOwner()->GetChildren()[0], GetSel(), model, combo, &wxMeshListFlagsRenderer::m_styles, col, row, cell, wxALIGN_RIGHT);
            }
        }
        return true;
    }
    wxSize GetSize() {
        return m_max;
    }
private:
    static wxArrayString m_styles;
    wxSize m_max;
};

wxArrayString wxMeshListFlagsRenderer::m_styles;

////////////////////////////////////////////////////////////////////////
//
//  wxMeshListUnknownRenderer
//
////////////////////////////////////////////////////////////////////////

class wxMeshListUnknownRenderer: public wxGetSelRenderer
{
public:
    wxMeshListUnknownRenderer(dlgModel* dialog, wxSize& max):wxGetSelRenderer( dialog, wxT("string"), wxDATAVIEW_CELL_ACTIVATABLE ) {
        m_max = max;
        m_value = 0;
/*
        if (wxMeshListUnknownRenderer::m_styles.size()==0) {
			wxMeshListUnknownRenderer::m_styles.Add(_("1 (Unknown1)"));
			wxMeshListUnknownRenderer::m_styles.Add(_("3 (Unknown2)"));
        }
*/
    }
    bool SetValue( const wxVariant &value ) {
        m_value = (cMeshStruct *) value.GetVoidPtr();
        return true;
    }
    bool Render( wxRect rect, wxDC *dc, int state ) {
        if (m_value) {
            if (m_value->valid) {
                wxString s;
                switch (m_value->unknown) {
                    case 1:
                        s = _("Double Sided (1, Uk1)");
                        break;
                    default:
                        s = _("Single Sided (3, Uk2)");
                }
                wxDCClipper clip(*dc, rect);
                wxCoord w, h;
                dc->GetTextExtent(s, &w, &h);
                wxCoord top = (rect.height - h) / 2;
                if (m_value->disabled) {
                    dc->SetTextForeground(GetStateColour(wxColor(wxT("#808080")), state));
                } else {
                    dc->SetTextForeground(GetStateColour(wxColor(wxT("#000000")), state));
                }
                dc->DrawText( s, rect.x+5, rect.y + top );
            }
        }
        return true;
    }
    bool Activate( wxRect cell, wxDataViewListModel *model, unsigned int col, unsigned int row ) {
        if (m_value) {
            if (m_value->valid) {
                if (m_value->unknown == 1)
                    m_value->unknown = 3;
                else
                    m_value->unknown = 1;
                model->ValueChanged(col, row);
/*
                wxChoice* combo = new wxChoice();
                wxChoiceWrapper* popup = new wxChoiceWrapper(GetOwner()->GetOwner()->GetChildren()[0], (long) ((m_value->unknown==1)?0:1), model, combo, &wxMeshListUnknownRenderer::m_styles, col, row, cell, wxALIGN_RIGHT);
*/
            }
        }
        return true;
    }
    wxSize GetSize() {
        return m_max;
    }
private:
    cMeshStruct* m_value;
//    static wxArrayString m_styles;
    wxSize m_max;
};

//wxArrayString wxMeshListUnknownRenderer::m_styles;

////////////////////////////////////////////////////////////////////////
//
//  wxMeshListFudgeRenderer
//
////////////////////////////////////////////////////////////////////////

class wxMeshListFudgeRenderer: public wxGetSelRenderer
{
public:
    wxMeshListFudgeRenderer(dlgModel* dialog, wxSize& max):wxGetSelRenderer( dialog, wxT("string"), wxDATAVIEW_CELL_ACTIVATABLE ) {
        m_max = max;
        m_value = 0;
        if (wxMeshListFudgeRenderer::m_styles.size()==0) {
			wxMeshListFudgeRenderer::m_styles.Add(_("Leave alone"));
			wxMeshListFudgeRenderer::m_styles.Add(_("+X"));
			wxMeshListFudgeRenderer::m_styles.Add(_("+Y"));
			wxMeshListFudgeRenderer::m_styles.Add(_("+Z"));
			wxMeshListFudgeRenderer::m_styles.Add(_("-X"));
			wxMeshListFudgeRenderer::m_styles.Add(_("-Y"));
			wxMeshListFudgeRenderer::m_styles.Add(_("-Z"));
			wxMeshListFudgeRenderer::m_styles.Add(_("Straight Rim"));
        }
    }
    bool SetValue( const wxVariant &value ) {
        m_value = (cMeshStruct *) value.GetVoidPtr();
        return true;
    }
    bool Render( wxRect rect, wxDC *dc, int state ) {
        if (m_value) {
            if (m_value->valid) {
                wxString s = wxMeshListFudgeRenderer::m_styles[m_value->fudgenormals];
                wxDCClipper clip(*dc, rect);
                wxCoord w, h;
                dc->GetTextExtent(s, &w, &h);
                wxCoord top = (rect.height - h) / 2;
                if (m_value->disabled) {
                    dc->SetTextForeground(GetStateColour(wxColor(wxT("#808080")), state));
                } else {
                    dc->SetTextForeground(GetStateColour(wxColor(wxT("#000000")), state));
                }
                dc->DrawText( s, rect.x+5, rect.y + top );
            }
        }
        return true;
    }
    bool Activate( wxRect cell, wxDataViewListModel *model, unsigned int col, unsigned int row ) {
        if (m_value) {
            if (m_value->valid) {
                wxChoice* combo = new wxChoice();
                wxChoiceWrapper* popup = new wxChoiceWrapper(GetOwner()->GetOwner()->GetChildren()[0], (long) m_value->fudgenormals, model, combo, &wxMeshListFudgeRenderer::m_styles, col, row, cell, wxALIGN_RIGHT);
            }
        }
        return true;
    }
    wxSize GetSize() {
        return m_max;
    }
private:
    static wxArrayString m_styles;
    wxSize m_max;
};

wxArrayString wxMeshListFudgeRenderer::m_styles;

////////////////////////////////////////////////////////////////////////
//
//  wxMeshListBox
//
////////////////////////////////////////////////////////////////////////
/*
wxMeshListBox::wxMeshListBox(wxWindow *parent, cModel *content):
        wxColourHtmlListBox(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER) {
    m_contents = content;
    UpdateContents();
    SetSelection(0);
}

void wxMeshListBox::UpdateContents() {
    SetItemCount(m_contents->meshstructs.size());
    RefreshAll();
}

wxString wxMeshListBox::OnGetItem(size_t n) const {
    cMeshStruct *ms = &m_contents->meshstructs[n];
    wxString res = wxT("<font ");
    if (ms->effectpoint)
        res += wxT("color='#A0D2A0' ");
    else if (!ms->valid)
        res += wxT("color='#CC0000' ");
    else if (ms->disabled)
        res += wxT("color='#808080' ");
    else if ((ms->FTX == wxT(""))
             || (ms->TXS == wxT("")))
        res += wxT("color='#FF3C3C' ");

    res += wxT("size='2'>")+wxEncodeHtmlEntities(ms->Name)+wxT("</font>");
    return res;
}
*/
////////////////////////////////////////////////////////////////////////
//
//  wxEffectListBox
//
////////////////////////////////////////////////////////////////////////

wxEffectListBox::wxEffectListBox(wxWindow *parent):
        wxColourHtmlListBox(parent, wxID_ANY, wxDefaultPosition, wxSize(-1,150), wxSUNKEN_BORDER) {}

wxEffectListBox::wxEffectListBox(wxWindow *parent, cModel *content):
        wxColourHtmlListBox(parent, wxID_ANY, wxDefaultPosition, wxSize(-1,150), wxSUNKEN_BORDER) {
    m_contents = content;
    UpdateContents();
    if (m_contents) {
        SetSelection(m_contents->effectpoints.size()?0:wxNOT_FOUND);
    } else {
        SetSelection(wxNOT_FOUND);
    }
}

void wxEffectListBox::UpdateContents() {
    SetItemCount(m_contents->effectpoints.size());
    RefreshAll();
}

wxString wxEffectListBox::OnGetItem(size_t n) const {
    return wxT("<font size='2'>")+wxEncodeHtmlEntities(m_contents->effectpoints[n].name)+wxT("</font>");
}


////////////////////////////////////////////////////////////////////////
//
//  wxBoneListBox
//
////////////////////////////////////////////////////////////////////////

wxBoneListBox::wxBoneListBox(wxWindow *parent, cModel *content):
        wxEffectListBox(parent) {
    m_contents = dynamic_cast<cAnimatedModel*>(content);
    UpdateContents();
    SetSelection(0);
}

void wxBoneListBox::UpdateContents() {
    SetItemCount(m_contents->modelbones.size());
    RefreshAll();
}

wxString wxBoneListBox::OnGetItem(size_t n) const {
    wxString addon = wxT("");
    if (m_contents->modelbones[n].parent != wxT(""))
        addon += wxT("<br>&nbsp;&nbsp;") + wxString::Format(_("child of %s"), m_contents->modelbones[n].parent.c_str());
    if (m_contents->modelbones[n].meshes.size()) {
        addon += wxT("<br>&nbsp;&nbsp;");
        addon += _("applied to:");
        addon += wxT("<ol>");
        for (cStringIterator it = m_contents->modelbones[n].meshes.begin(); it != m_contents->modelbones[n].meshes.end(); it++) {
            addon += wxT("<li>")+*it+wxT("</li>");
        }
        addon += wxT("</ol>");
    }
    return wxT("<font size='2'>")+wxEncodeHtmlEntities(m_contents->modelbones[n].name)+addon+wxT("</font>");
}


////////////////////////////////////////////////////////////////////////
//
//  dlgModel
//
////////////////////////////////////////////////////////////////////////

DECLARE_EVENT_TYPE(wxEVT_DOUPDATE, -1)
DEFINE_EVENT_TYPE(wxEVT_DOUPDATE)

const int ID_htlbMesh = ::wxNewId();

BEGIN_EVENT_TABLE(dlgModel,wxDialog)
//EVT_KEY_UP(dlgModel::OnChar)
//EVT_KEY_DOWN(dlgModel::OnKeyUp)
//EVT_BUTTON(wxID_OK, dlgModel::OnOKClick)
EVT_COMMAND(wxID_ANY, wxEVT_WRAPPER, dlgModel::OnWrapperEdit)
EVT_COMMAND(wxID_ANY, wxEVT_DOUPDATE, dlgModel::OnDoUpdate)
//EVT_AUI_PANE_MAXIMIZE(dlgModel::OnPaneMaximize)

EVT_COMBOBOX(XRCID("m_textModelName"), dlgModel::OnNameAuto)
EVT_COMBOBOX(XRCID("m_textModelFile"), dlgModel::OnModelOpen)
EVT_BUTTON(XRCID("m_btCoordinateSystem"), dlgModel::OnSystemAuto)
//EVT_RIGHT_DCLICK(XRCID("m_btCoordinateSystem"), dlgModel::OnSystemAutoRDbl)

EVT_BUTTON(XRCID("m_btMatrixEdit"), dlgModel::OnMatrixEdit)
EVT_BUTTON(XRCID("m_btMatrixClear"), dlgModel::OnMatrixClear)

//EVT_LISTBOX(ID_htlbMesh, dlgModel::OnControlUpdate)
//EVT_LISTBOX_DCLICK(ID_htlbMesh, dlgModel::OnMeshEdit)
//EVT_BUTTON(XRCID("m_btEditMesh"), dlgModel::OnMeshEdit)

EVT_LISTBOX(XRCID("m_htlbEffect"), dlgModel::OnControlUpdate)
/*
EVT_SPIN_UP(XRCID("m_spinEffect"), dlgModel::OnEffectUp)
EVT_SPIN_DOWN(XRCID("m_spinEffect"), dlgModel::OnEffectDown)
EVT_BUTTON(XRCID("m_btEffectAdd"), dlgModel::OnEffectAdd)
EVT_BUTTON(XRCID("m_btEffectEdit"), dlgModel::OnEffectEdit)
EVT_LISTBOX_DCLICK(XRCID("m_htlbEffect"), dlgModel::OnEffectEdit)
EVT_BUTTON(XRCID("m_btEffectCopy"), dlgModel::OnEffectCopy)
EVT_BUTTON(XRCID("m_btEffectDel"), dlgModel::OnEffectDel)
EVT_BUTTON(XRCID("m_btEffectAuto"), dlgModel::OnEffectAuto)
EVT_BUTTON(XRCID("m_btEffectClear"), dlgModel::OnEffectClear)
*/

EVT_BUTTON(XRCID("m_btLoad"), dlgModel::OnLoad)
END_EVENT_TABLE()

dlgModel::dlgModel(wxWindow *parent, cSCNFile& scn, bool animated):
	m_model(NULL), m_animated(animated), m_loadoverlay(false), m_scn(scn), m_boneshutup(false)
{
    m_hookhandler = NULL;

    if (m_animated)
        m_model = &m_amodel;
    else
        m_model = &m_smodel;

    m_mgr.SetManagedWindow(this);

    InitWidgetsFromXRC((wxWindow *)parent);
    SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
    SetMinSize(GetSize());

    m_Editing = false;

    dlgCreateScenery *dlg = dynamic_cast<dlgCreateScenery *> (parent);
    if (dlg) {
        for(cFlexiTexture::iterator it = dlg->m_SCN.flexitextures.begin(); it != dlg->m_SCN.flexitextures.end(); it++) {
            m_textures.push_back(it->Name);
        }
    }

    for(std::list<wxString>::iterator it = ::wxGetApp().g_texturecache.begin(); it != ::wxGetApp().g_texturecache.end(); it++) {
        m_textures.push_back(*it);
    }

    // Set up model
    m_panModel = new wxPanel();
    InitModelFromXRC(m_panModel);

    wxTextCtrl *t_text  = XRCCTRL(*m_panModel,"m_textModelName",wxTextCtrl);
    m_textModelName = new wxStarComboCtrl(m_panModel, XRCID("m_textModelName"));
    t_text->GetContainingSizer()->Replace(t_text, m_textModelName);
    t_text->Destroy();

    t_text = XRCCTRL(*m_panModel,"m_textModelFile",wxTextCtrl);
    wxFileDialog *filedlg = new wxFileDialog(
                               this,
                               _("Open Model File"),
                               wxEmptyString,
                               wxEmptyString,
                                c3DLoader::GetSupportedFilesFilter()+_("|All Files (*.*)|*.*"),
                               wxFD_FILE_MUST_EXIST|wxFD_CHANGE_DIR,
                               wxDefaultPosition,
                               wxSize(600,400)
                           );

    m_textModelFile = new wxFileSelectorCombo<wxFileDialog>(m_panModel, filedlg, &::wxGetApp().g_workdir, XRCID("m_textModelFile"),
                                                wxEmptyString, wxDefaultPosition, wxDefaultSize, 0/*wxCB_READONLY*/);
    m_textModelFile->GetTextCtrl()->SetEditable(false);
    t_text->GetContainingSizer()->Replace(t_text, m_textModelFile);
    t_text->Destroy();

    m_textModelName->SetValidator(wxExtendedValidator(&m_model->name, false));
    m_textModelFile->SetValidator(wxExtendedValidator(&m_model->file, false, true));
    m_choiceCoordinateSystem->SetValidator(wxGenericValidator(reinterpret_cast<int*>(&m_model->usedorientation)));

    wxInputBox *t_ibModelName = new wxInputBox(m_panModel, wxID_ANY);
    t_ibModelName->SetEditor(m_textModelName);
    wxInputBox *t_ibModelFile = new wxInputBox(m_panModel, wxID_ANY);
    t_ibModelFile->SetEditor(m_textModelFile);
/*
    m_panModel->Fit();
    m_panModel->Layout();

    m_mgr.AddPane(m_panModel, wxAuiPaneInfo().Name(wxT("model")).Caption(_("Model")).Top().
                                              MinSize(m_panModel->GetSize()).BestSize(m_panModel->GetSize()).MaxSize(m_panModel->GetSize()).
                                              Fixed().CloseButton(false));
*/

    // Set up Effect Points
    m_panEffect = new wxPanel();
    InitEffectFromXRC(m_panEffect);

    if (m_animated) {
        SetTitle(_("Edit animated model settings"));
        m_htlbEffect = new wxBoneListBox(m_panEffect, m_model);
        m_htlbEffect->SetToolTip(_("List of the bones"));
        Connect(XRCID("m_spinEffect"), wxEVT_SCROLL_LINEUP, wxSpinEventHandler(dlgModel::OnBoneUp));
        Connect(XRCID("m_spinEffect"), wxEVT_SCROLL_LINEDOWN, wxSpinEventHandler(dlgModel::OnBoneDown));
        Connect(XRCID("m_btEffectAdd"), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(dlgModel::OnBoneAdd));
        Connect(XRCID("m_btEffectEdit"), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(dlgModel::OnBoneEdit));
        Connect(XRCID("m_htlbEffect"), wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler(dlgModel::OnBoneEdit));
        Connect(XRCID("m_btEffectCopy"), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(dlgModel::OnBoneCopy));
        Connect(XRCID("m_btEffectDel"), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(dlgModel::OnBoneDel));
        Connect(XRCID("m_btEffectAuto"), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(dlgModel::OnBoneAuto));
        m_btEffectAuto->Connect(wxEVT_RIGHT_DCLICK, wxMouseEventHandler(dlgModel::OnBoneAutoQuick), NULL, this);
        Connect(XRCID("m_btEffectClear"), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(dlgModel::OnBoneClear));
    } else {
        SetTitle(_("Edit static model settings"));
        m_htlbEffect = new wxEffectListBox(m_panEffect, m_model);
        m_htlbEffect->SetToolTip(_("List of the effect points"));
        Connect(XRCID("m_spinEffect"), wxEVT_SCROLL_LINEUP, wxSpinEventHandler(dlgModel::OnEffectUp));
        Connect(XRCID("m_spinEffect"), wxEVT_SCROLL_LINEDOWN, wxSpinEventHandler(dlgModel::OnEffectDown));
        Connect(XRCID("m_btEffectAdd"), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(dlgModel::OnEffectAdd));
        Connect(XRCID("m_btEffectEdit"), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(dlgModel::OnEffectEdit));
        Connect(XRCID("m_htlbEffect"), wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler(dlgModel::OnEffectEdit));
        Connect(XRCID("m_btEffectCopy"), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(dlgModel::OnEffectCopy));
        Connect(XRCID("m_btEffectDel"), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(dlgModel::OnEffectDel));
        Connect(XRCID("m_btEffectAuto"), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(dlgModel::OnEffectAuto));
        m_btEffectAuto->Connect(wxEVT_RIGHT_DCLICK, wxMouseEventHandler(dlgModel::OnEffectAutoQuick), NULL, this);
        Connect(XRCID("m_btEffectClear"), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(dlgModel::OnEffectClear));
    }
    Connect(XRCID("m_cbSyncBones"), wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(dlgModel::OnSyncBones));
    Connect(XRCID("m_cbDelBones"), wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(dlgModel::OnDelBones));
    Connect(XRCID("m_cbSortBones"), wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(dlgModel::OnSortBones));
    m_btCoordinateSystem->Connect(wxEVT_RIGHT_DCLICK, wxMouseEventHandler( dlgModel::OnSystemAutoRDbl ));
    wxXmlResource::Get()->AttachUnknownControl(wxT("m_htlbEffect"), m_htlbEffect, m_panEffect);
/*
    m_panEffect->Fit();
    m_panEffect->Layout();

    m_mgr.AddPane(m_panEffect, wxAuiPaneInfo().Name(wxT("effect")).Caption(_("Effect Points")).Bottom().
                                              MinSize(m_panEffect->GetSize()).CloseButton(false));
*/

    // Set up the dual panel
    wxPanel* t_panBoth = new wxPanel(this);
    m_panModel->Reparent(t_panBoth);
    m_panEffect->Reparent(t_panBoth);
    wxBoxSizer* bsiz = new wxBoxSizer(wxHORIZONTAL);
    wxStaticBoxSizer* sbtop = new wxStaticBoxSizer(wxVERTICAL, t_panBoth, _("Model"));
    sbtop->Add(m_panModel, wxSizerFlags().Expand().Proportion(1));
    wxStaticBoxSizer* sbbot;
    if (m_animated) {
        sbbot = new wxStaticBoxSizer(wxVERTICAL, t_panBoth, _("Bones"));
    } else {
        sbbot = new wxStaticBoxSizer(wxVERTICAL, t_panBoth, _("Effect Points"));
    }
    sbbot->Add(m_panEffect, wxSizerFlags().Expand().Proportion(1));
    bsiz->Add(sbtop, wxSizerFlags().Expand().Proportion(0).Border(wxLEFT|wxTOP|wxBOTTOM,5));
    bsiz->Add(sbbot, wxSizerFlags().Expand().Proportion(1).Border(wxLEFT|wxRIGHT|wxTOP|wxBOTTOM,5));
    t_panBoth->SetSizer(bsiz);
    t_panBoth->Fit();
    t_panBoth->Layout();
    bsiz->SetSizeHints(t_panBoth);

    m_mgr.AddPane(t_panBoth, wxAuiPaneInfo().Name(wxT("both")).Top().CaptionVisible(false).PaneBorder(false).
                                              MinSize(t_panBoth->GetSize()).CloseButton(false));



    // Set up Meshes
    m_panMeshes = new wxPanel(this);
    wxBoxSizer* siz = new wxBoxSizer(wxVERTICAL);

    m_modelMesh = new wxMeshListModel(m_model);
    m_dataviewMesh = new wxExDataViewCtrl(m_panMeshes, wxID_ANY);
    m_dataviewMesh->AssociateModel(m_modelMesh);

    siz->Add(m_dataviewMesh, wxSizerFlags().Expand());
    m_panMeshes->SetSizer(siz);
    m_panMeshes->Fit();
    m_panMeshes->Layout();
    siz->SetSizeHints(m_panMeshes);

    m_mgr.AddPane(m_panMeshes, wxAuiPaneInfo().Name(wxT("meshes")).Caption(_("Groups (Meshes)")).CenterPane().
                                               MaximizeButton().
//                                               PaneBorder(false).
                                               CaptionVisible());


    // Set up buttons
    m_panButtons = new wxPanel();
    InitButtonsFromXRC(m_panButtons);
    m_panButtons->Fit();
    m_panButtons->Layout();
    m_mgr.AddPane(m_panButtons, wxAuiPaneInfo().Name(wxT("buttons")).Bottom().Layer(1).CaptionVisible(false).
                                                MinSize(m_panButtons->GetSize()).
                                                PaneBorder(false).
                                                Fixed());

/*
    Fit();
    Layout();
    GetSizer()->SetSizeHints(this);
    Center();
*/
//    Connect(wxEVT_CHAR, wxKeyEventHandler(dlgModel::OnChar));
//    Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(dlgModel::OnKeyUp));

    m_btOk->SetId(wxID_OK);
    m_btCancel->SetId(wxID_CANCEL);

    m_menuAutoBones.reset(new wxMenu());
    wxString menutext = m_animated?_("bones"):_("effect points");
    Connect(m_menuAutoBones->Append(wxID_ANY, wxString::Format("&Add missing %s", menutext.c_str()))->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(dlgModel::OnMenuAddMissingBones));
    Connect(m_menuAutoBones->Append(wxID_ANY, wxString::Format("S&ynchronize %s", menutext.c_str()))->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(dlgModel::OnMenuSyncBones));
    Connect(m_menuAutoBones->Append(wxID_ANY, wxString::Format("&Sort %s", menutext.c_str()))->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(dlgModel::OnMenuSortBones));
    m_menuAutoBones->AppendSeparator();
    m_miSyncBones = m_menuAutoBones->AppendCheckItem(wxID_ANY, wxString::Format("Auto-synchronise %s", menutext.c_str()));
    m_miDelBones = m_menuAutoBones->AppendCheckItem(wxID_ANY, wxString::Format("Full auto-synchronize %s", menutext.c_str()));
    m_miSortBones = m_menuAutoBones->AppendCheckItem(wxID_ANY, wxString::Format("Auto-sort %s", menutext.c_str()));
    Connect(m_miSyncBones->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(dlgModel::OnMenuAutoSyncBones));
    Connect(m_miDelBones->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(dlgModel::OnMenuAutoDelBones));
    Connect(m_miSortBones->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(dlgModel::OnMenuAutoSortBones));


//    m_mgr.GetArtProvider()->SetColour(wxAUI_DOCKART_INACTIVE_CAPTION_COLOUR, wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION));
//    m_mgr.SetFlags(wxAUI_MGR_ALLOW_FLOATING|wxAUI_MGR_TRANSPARENT_DRAG|wxAUI_MGR_TRANSPARENT_HINT|wxAUI_MGR_ALLOW_ACTIVE_PANE);
    m_mgr.SetFlags(0);

    UpdateAll();

    SetSize(READ_APP_SIZE("Model", GetSize()));
    Center();
}

dlgModel::~dlgModel() {
//    if (m_model)
//        delete m_model;
    WRITE_APP_SIZE("Model", GetSize());
    m_mgr.UnInit();
}

void dlgModel::CheckModel() {
//    if (m_model)
//        delete m_model;
//    if (dynamic_cast<cAnimatedModel*>(model)) {
//        m_model = new cAnimatedModel();
//        cAnimatedModel* t = dynamic_cast<cAnimatedModel*>(model);
//        *m_model = *t;
//    } else {
//        m_model = new cModel();
//        *m_model = *model;
//    }
    cModel smodel = m_smodel;
    cAnimatedModel amodel = m_amodel;
    while (m_model->stored_exception) {
        if (::wxMessageBox(_("The model file could not be found or read, please select a new one.\nIf you press 'Cancel', all model related data (file name and mesh settings) will be cleared."), _("Error loading model file"), wxOK|wxCANCEL|wxICON_ERROR) == wxCANCEL) {
            // User requested delete
            m_model->stored_exception.reset();
			m_model->errors.clear();
            m_model->file = wxT("");
            m_model->meshstructs.clear();
            break;
        }
        boost::shared_ptr<wxFileDialog> dialog(new wxFileDialog(
                                   this,
                                   _("Select Model File"),
                                   wxEmptyString,
                                   wxEmptyString,
                                   c3DLoader::GetSupportedFilesFilter()+_("|All Files (*.*)|*.*"),
                                   wxFD_FILE_MUST_EXIST|wxFD_CHANGE_DIR,
                                   wxDefaultPosition,
                                   wxSize(600,400)
                               ), wxWindowDestroyer);
        if (m_model->file != wxT("")) {
            dialog->SetDirectory(m_model->file.GetPath());
            dialog->SetFilename(m_model->file.GetFullName());
        } else
            dialog->SetDirectory(::wxGetApp().g_workdir.GetPath());
        if (dialog->ShowModal() == wxID_OK) {
            ::wxGetApp().g_workdir.AssignDir(wxFileName(dialog->GetPath()).GetPath());
            m_model->Sync(dialog->GetPath());
            if (m_model->stored_exception) {
                // Load error
                if (m_animated) {
                    m_amodel = amodel;
                } else {
                    m_smodel = smodel;
                }
                continue;
            }
            if (m_model->errors.size()) {
                wxString errtext;
                foreach(const wxString& err, m_model->errors) {
                    errtext += wxT("\n\n") + err;
                }
                if (::wxMessageBox(_("The following warnings were encountered while loading the model file:") + errtext + _("\n\nDo you want to select a different one?"), _("Warning during model file loading"), wxYES_NO | wxNO_DEFAULT | wxICON_WARNING) == wxYES) {
                    // Select a different one, restore the model
                    if (m_animated) {
                        m_amodel = amodel;
                    } else {
                        m_smodel = smodel;
                    }
                } else {
                    m_model->errors.clear();
                }
            }
        }
    }
    UpdateAll();
};
/*
void dlgModel::FetchOneVertexMeshes(wxArrayString& names, std::vector<VECTOR>& points) {
    for (cMeshStruct::iterator ms = m_model->meshstructs.begin();
            ms != m_model->meshstructs.end(); ms++) {
        if (ms->effectpoint) {
            names.push_back(ms->Name);
            points.push_back(ms->effectpoint_vert);
        }
    }
}
*/
void dlgModel::ShowTransform(int pr) {
    MATRIX m = matrixMultiply(m_model->transforms);
    for (int i=0; i<4; i++)
        for (int j=0; j<4; j++)
            m_Matrix[i][j]->SetLabel(wxString::Format("%.*f", pr, m.m[i][j]));
}

void dlgModel::UpdateAll() {
    if (!m_model)
        return;
    ShowTransform();
    m_cbSyncBones->SetValue(m_model->auto_bones);
	m_cbDelBones->SetValue(m_model->auto_delete_bones);
    m_cbSortBones->SetValue(m_model->auto_sort);
    m_miSyncBones->Check(m_model->auto_bones);
    m_miDelBones->Check(m_model->auto_delete_bones);
    m_miSortBones->Check(m_model->auto_sort);
	
	m_cbDelBones->Enable(m_model->auto_bones);
	m_miDelBones->Enable(m_model->auto_bones); 
	m_btEffectAdd->Enable(!m_model->auto_delete_bones);
    //m_htlbMesh->UpdateContents();
    m_modelMesh->Cleared();
/*
    for(int i = m_dataviewMesh->GetNumberOfColumns(); i > 0; i--) {
        m_dataviewMesh->DeleteColumn(i-1);
    }
*/
    if (m_dataviewMesh) {
        m_panMeshes->GetSizer()->Detach(m_dataviewMesh);
        m_dataviewMesh->GetChildren()[0]->Disconnect(wxEVT_KEY_UP, wxKeyEventHandler(dlgModel::OnChar));
        m_dataviewMesh->Destroy();
    }

    m_dataviewMesh = new wxExDataViewCtrl(m_panMeshes, wxID_ANY, wxDefaultPosition, wxDefaultSize,
        wxEDV_STRIPED|wxEDV_SINGLE_ACTIVATE|wxEDV_NO_SELECT|wxTAB_TRAVERSAL);
    m_dataviewMesh->AssociateModel(m_modelMesh);
//    wxXmlResource::Get()->AttachUnknownControl(wxT("m_dataviewMesh"), m_dataviewMesh, this);
    m_dataviewMesh->GetChildren()[0]->SetToolTip(_("List of the meshes/groups/objects in your model file.\nIcon (Name Colours):\n"
                                "Green Led (black) - Mesh activated and ready for import.\n"
                                "Yellow Led (red) - Mesh activated but lacks data needed for import.\n"
                                "Red Led (grey) - Deactivatet mesh.\n"
                                "Red Star (dark red) - Mesh is invalid/broken.\n"
                                "Green Star (green) - One-vertex mesh that can be used for matrices (e.g. effect points).\n"
                                "\nClicking on the icon or on the name will activate/deactivate the mesh.\n"
                                "Clicking other values will let you edit them (Press Escape to cancel the edit).\n"
                                "Shift-Click on the name will single it out.\n"
                                "Ctrl-Click on the name will copy that mesh's settings to all others.\n"
                                "\nPress 'F5' to refresh the list."));

    m_dataviewMesh->AppendColumn(new wxDataViewColumn(wxT(""), new wxMeshListEnableRenderer(), 0, 20));
    wxCoord w, h;
    wxClientDC dc(m_dataviewMesh);
    dc.GetTextExtent(wxT("MINIMUM"), &w, &h);
    wxSize maxname(w,h);
    wxSize maxftx(w,h);
    dc.GetTextExtent(_("Texture Style"), &w, &h);
    wxSize maxtxs(w,h);
    dc.GetTextExtent(_("see-through"), &w, &h);
    wxSize maxplace(w,h);
    dc.GetTextExtent(_("Water Mask Texture"), &w, &h);
    wxSize maxflags(w,h);
    dc.GetTextExtent(_("Single Sided (3, Uk2)"), &w, &h);
    wxSize maxunk(w,h);
    dc.GetTextExtent(_("Leave alone"), &w, &h);
    wxSize maxfudge(w,h);
    for(int i = 0; i < m_model->meshstructs.size(); i++) {
        dc.GetTextExtent(m_model->meshstructs[i].Name, &w, &h);
        if (w > maxname.GetWidth())
            maxname.SetWidth(w);
        if (h > maxname.GetHeight())
            maxname.SetHeight(h);
        dc.GetTextExtent(m_model->meshstructs[i].FTX, &w, &h);
        if (w > maxftx.GetWidth())
            maxftx.SetWidth(w);
        if (h > maxftx.GetHeight())
            maxftx.SetHeight(h);
        dc.GetTextExtent(m_model->meshstructs[i].TXS, &w, &h);
        if (w > maxtxs.GetWidth())
            maxtxs.SetWidth(w);
        if (h > maxtxs.GetHeight())
            maxtxs.SetHeight(h);
    }
    maxname.SetWidth(maxname.GetWidth()+10);
    m_dataviewMesh->AppendColumn(new wxDataViewColumn(_("Name"), new wxMeshListNameRenderer(this, maxname), 1, maxname.GetWidth()));
    maxftx.SetWidth(maxftx.GetWidth()+10);
    m_dataviewMesh->AppendColumn(new wxDataViewColumn(_("Texture"), new wxMeshListFTXRenderer(this, maxftx, &m_textures), 2, maxftx.GetWidth()));
    //m_dataviewMesh->AppendTextColumn(wxT("Texture"), 2);
    m_dataviewMesh->AppendColumn(new wxDataViewColumn(_("Texture Style"), new wxMeshListTXSRenderer(this, maxtxs), 3, maxtxs.GetWidth()));
    //m_dataviewMesh->AppendTextColumn(wxT("Texture Style"), 3);
//    m_dataviewMesh->AppendColumn(new wxDataViewColumn(_("Transparency"), new wxMeshListPlaceRenderer(this, maxplace), 4, maxplace.GetWidth()));
    //m_dataviewMesh->AppendTextColumn(wxT("Place"), 4);
    m_dataviewMesh->AppendColumn(new wxDataViewColumn(_("Flags"), new wxMeshListFlagsRenderer(this, maxflags), 5, maxflags.GetWidth()));
    //m_dataviewMesh->AppendTextColumn(_("Flags"), 5);
    m_dataviewMesh->AppendColumn(new wxDataViewColumn(_("Faces"), new wxMeshListUnknownRenderer(this, maxunk), 6, maxunk.GetWidth()));
    //m_dataviewMesh->AppendTextColumn(_("Unknown"), 6);
    m_dataviewMesh->AppendColumn(new wxDataViewColumn(_("Normals"), new wxMeshListFudgeRenderer(this, maxfudge), 7, maxfudge.GetWidth()));
    m_dataviewMesh->AppendColumn(new wxDataViewColumn(_("Transparency"), new wxMeshListPlaceRenderer(this, maxplace), 4, maxplace.GetWidth()));
    m_dataviewMesh->GetChildren()[0]->Connect(wxEVT_KEY_UP, wxKeyEventHandler(dlgModel::OnChar));

    m_dataviewMesh->Refresh();

    m_panMeshes->GetSizer()->Add(m_dataviewMesh, wxSizerFlags().Expand().Border(wxALL,5).Proportion(1));
    m_panMeshes->Fit();
    m_panMeshes->Layout();
    m_panMeshes->GetSizer()->SetSizeHints(m_panMeshes);

    m_htlbEffect->UpdateContents();

    UpdateControlState();
    m_mgr.Update();
}

void dlgModel::UpdateControlState() {
    if (!m_model)
        return;
		
	int sel = m_htlbEffect->GetSelection();
	m_btEffectEdit->Enable(sel>=0);
	if (m_model->auto_delete_bones) {
		m_spinEffect->Enable(false);
		m_btEffectCopy->Enable(false);
		m_btEffectDel->Enable(false);
		m_btEffectClear->Enable(false);
	} else {
		int count = 0;
		if (m_animated) {
			count = dynamic_cast<cAnimatedModel*>(m_model)->modelbones.size();
		} else {
			count = m_model->effectpoints.size();
		}
		m_spinEffect->Enable((count>=2) and (!m_model->auto_sort));
		m_btEffectCopy->Enable(sel>=0);
		m_btEffectDel->Enable((sel>=0) and (IsBoneEditable(sel)));
		m_btEffectClear->Enable(count>=1);		
	}

//    m_btEditMesh->Enable(m_htlbMesh->GetSelection()>=0);

/*
    bool haspoints = false;
    for (cMeshStruct::iterator ms = m_model->meshstructs.begin();
            ms != m_model->meshstructs.end(); ms++) {
        if (ms->effectpoint) {
            haspoints = true;
            break;
        }
    }
*/
    m_btEffectAuto->Enable(m_model->model_bones.size());

}

/** @brief OnNameAuto
  *
  * @todo: document this function
  */
void dlgModel::OnNameAuto(wxCommandEvent& event) {
    wxFileName t = m_model->file;
    m_textModelName->SetText(t.GetName());
}

void dlgModel::OnModelOpen(wxCommandEvent& event) {
    ::wxGetApp().g_workdir.AssignDir(wxFileName(event.GetString()).GetPath());
    boost::shared_ptr<cModel> backup;
    if (m_animated)
        backup.reset(new cAnimatedModel(m_amodel));
    else
        backup.reset(new cModel(m_smodel));
    //wxFileName oldfile = m_model->file;
    TransferDataFromWindow();
    try {
        if (m_model->Load(event.GetString(), m_loadoverlay)) {
            if (m_model->fileorientation != ORIENTATION_UNKNOWN) {
                if (m_model->fileorientation != m_model->usedorientation) {
                    if (READ_RCT3_ORIENTATION() == ORIENTATION_UNKNOWN) {
                        m_model->usedorientation = m_model->fileorientation;
                    } else {
                        if (::wxMessageBox(_("The model file you just opened suggests a model orientation different from your current one.\nDo you want to replace the current one with the one suggested in the file?"), _("Question"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION, this) == wxYES) {
                            m_model->usedorientation = m_model->fileorientation;
                        }
                    }
                }
            }

            if (m_loadoverlay) {
                if (m_model->errors.size()) {
                    wxString errtext;
                    foreach(wxString& err, m_model->errors)
                        errtext += (errtext.IsEmpty()?wxT(""):wxT("\n\n")) + err;
                    ::wxMessageBox(errtext, _("Warning during model file loading"), wxOK | wxICON_WARNING, this);
                    m_model->errors.clear();
                }
            }

        } else {
			wxString errtext;
			if (m_model->stored_exception)
				errtext = m_model->stored_exception->formatLong();
			else
				errtext = _("Unknown error");
            ::wxMessageBox(errtext, _("Error"), wxOK | wxICON_ERROR, this);
            if (m_animated)
                m_amodel = *dynamic_cast<cAnimatedModel*>(backup.get());
            else
                m_smodel = *backup.get();
        }
    } catch (E3DLoader& e) {
        ::wxMessageBox(e.formatLong(), _("Error"), wxOK | wxICON_ERROR, this);
        if (m_animated)
            m_amodel = *dynamic_cast<cAnimatedModel*>(backup.get());
        else
            m_smodel = *backup.get();
    }
    m_loadoverlay = false;
    TransferDataToWindow();
    UpdateAll();
}

/** @brief OnSystemAuto
  *
  * @todo: document this function
  */
void dlgModel::OnSystemAuto(wxCommandEvent& event) {
    if (m_model->fileorientation != ORIENTATION_UNKNOWN) {
        TransferDataFromWindow();
        m_model->usedorientation = m_model->fileorientation;
        TransferDataToWindow();
    } else {
        if (::wxMessageBox(_("The model file does not contain coordinate system information. Likely it is an ASE file. Did you use SketchUp or Blender to create it?"), _("Question"), wxYES_NO|wxYES_DEFAULT|wxICON_QUESTION, this) == wxYES) {
            TransferDataFromWindow();
            m_model->usedorientation = ORIENTATION_RIGHT_ZUP;
            TransferDataToWindow();
        } else {
            ::wxMessageBox(_("Sorry, you have to find out yourself which coordinate system your modeller uses."));
        }
    }
}

void dlgModel::OnControlUpdate(wxCommandEvent& WXUNUSED(event)) {
    UpdateControlState();
}

void dlgModel::OnWrapperEdit(wxCommandEvent& event)
{
//::wxMessageBox(wxT("OnChar"));
    if (event.GetInt() == wxWRAPPER_START_EDITING) {
//        m_btOk->SetId(wxID_ANY);
//        m_btCancel->SetId(wxID_ANY);
        m_hookhandler = dynamic_cast<wxEvtHandler*>(event.GetEventObject());
    } else if (event.GetInt() == wxWRAPPER_STOP_EDITING) {
        m_hookhandler = NULL;
//        m_btOk->SetId(wxID_OK);
//        m_btCancel->SetId(wxID_CANCEL);
        if (m_dataviewMesh)
            m_dataviewMesh->Refresh();
    } else
        ::wxMessageBox(wxT("Illegal wxEVT_WRAPPER event."), wxT("Internal Error"), wxOK, this);
}

void dlgModel::OnChar(wxKeyEvent& event)
{
    if (event.GetKeyCode() == WXK_F5) {
        event.Skip(false);
        event.StopPropagation();
        wxCommandEvent ev(wxEVT_DOUPDATE, wxID_ANY);
        AddPendingEvent(ev);
    } else {
        event.Skip();
    }
}

void dlgModel::OnDoUpdate(wxCommandEvent& WXUNUSED(event))
{
    UpdateAll();
}
/*
void dlgModel::OnPaneMaximize(wxAuiManagerEvent& WXUNUSED(event)) {
wxLogError(wxT("Test"));
    if (m_dataviewMesh) {
        m_dataviewMesh->SetFocus();
    }
}
*/
bool dlgModel::ProcessEvent(wxEvent& event) {
    if ((event.GetEventType() == wxEVT_CHAR_HOOK) && m_hookhandler) {
//wxLogError(wxT("Hook"));
        return m_hookhandler->ProcessEvent(event);
    } else {
        return wxDialog::ProcessEvent(event);
    }
}

////////////////////////////////////////////////////////////////////////
//
//  dlgModel, Matrix
//
////////////////////////////////////////////////////////////////////////

void dlgModel::OnMatrixEdit(wxCommandEvent& WXUNUSED(event)) {
    dlgMatrix *dialog = new dlgMatrix(this);
    dialog->SetStack(m_model->transforms, m_model->transformnames);
    if (dialog->ShowModal() == wxID_OK) {
        m_model->transforms = dialog->GetMatrices();
        m_model->transformnames = dialog->GetMatrixNames();
        ShowTransform();
    }
    dialog->Destroy();
}

void dlgModel::OnSystemAutoRDbl(wxMouseEvent& WXUNUSED(event)) {
    //WRITE_APP_MATRIX(matrixMultiply(m_model->transforms));
    if (::wxMessageBox(_("Do you want to save the current model orientation as defalt?"), _("Save default"), wxYES_NO|wxICON_QUESTION, this) == wxYES) {
        WRITE_RCT3_ORIENTATION(m_model->usedorientation);
        wxcFlush();
    }
}

void dlgModel::OnMatrixClear(wxCommandEvent& WXUNUSED(event)) {
    if (m_model->transforms.size()) {
        if (::wxMessageBox(_("Do you really want to delete all matrix entries?"), _("Question"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION, this)==wxNO)
            return;
    }

    m_model->transforms.clear();
    m_model->transformnames.clear();
    ShowTransform();
}


////////////////////////////////////////////////////////////////////////
//
//  dlgModel, Effects
//
////////////////////////////////////////////////////////////////////////

void dlgModel::OnEffectUp(wxSpinEvent& WXUNUSED(event)) {
    int sel = m_htlbEffect->GetSelection();
    if (sel < 1)
        return;
    cEffectPoint ft = m_model->effectpoints[sel];
    m_model->effectpoints.erase(m_model->effectpoints.begin() + sel);
    m_model->effectpoints.insert(m_model->effectpoints.begin() + sel - 1, ft);

    m_htlbEffect->UpdateContents();
    m_htlbEffect->SetSelection(sel-1);
    UpdateControlState();
}

void dlgModel::OnEffectDown(wxSpinEvent& WXUNUSED(event)) {
    int count = m_model->effectpoints.size();
    int sel = m_htlbEffect->GetSelection();
    if ((count-sel) <= 1)
        return;
    cEffectPoint ft = m_model->effectpoints[sel];
    m_model->effectpoints.erase(m_model->effectpoints.begin() + sel);
    m_model->effectpoints.insert(m_model->effectpoints.begin() + sel + 1, ft);

    m_htlbEffect->UpdateContents();
    m_htlbEffect->SetSelection(sel+1);
    UpdateControlState();
}

void dlgModel::OnEffectAdd(wxCommandEvent& WXUNUSED(event)) {
    dlgEffect *dialog = new dlgEffect(this);
    if (dialog->ShowModal() == wxID_OK) {
        m_model->effectpoints.push_back(dialog->GetEffect());
        m_htlbEffect->UpdateContents();
        m_htlbEffect->SetSelection(m_model->effectpoints.size()-1);
        UpdateControlState();
    }
    dialog->Destroy();
}

void dlgModel::OnEffectEdit(wxCommandEvent& WXUNUSED(event)) {
    int sel = m_htlbEffect->GetSelection();
    if (sel < 0)
        return;
    dlgEffect *dialog = new dlgEffect(this, !IsBoneEditable(sel));
    dialog->SetEffect(m_model->effectpoints[sel]);
    if (dialog->ShowModal() == wxID_OK) {
        m_model->effectpoints[sel] = dialog->GetEffect();
        m_htlbEffect->UpdateContents();
        UpdateControlState();
    }
    dialog->Destroy();
}

void dlgModel::OnEffectCopy(wxCommandEvent& WXUNUSED(event)) {
    int sel = m_htlbEffect->GetSelection();

    cEffectPoint ft = m_model->effectpoints[sel];
    ft.name += _(" Copy");
    m_model->effectpoints.insert(m_model->effectpoints.begin() + sel + 1, ft);

    m_htlbEffect->UpdateContents();
    m_htlbEffect->SetSelection(sel+1);
    UpdateControlState();
}

void dlgModel::OnEffectDel(wxCommandEvent& WXUNUSED(event)) {
    int sel = m_htlbEffect->GetSelection();
    m_model->effectpoints.erase(m_model->effectpoints.begin() + sel);

    m_htlbEffect->UpdateContents();
    m_htlbEffect->SetSelection(sel-1);
    UpdateControlState();
}

void dlgModel::OnEffectAuto(wxCommandEvent& WXUNUSED(event)) {
    /*
    if (m_model->effectpoints.size()) {
        if (::wxMessageBox(_("Do you want to delete all current effect point entries before automatically creating new ones?"), _("Question"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION, this)==wxYES) {
            m_model->effectpoints.clear();
        }
    }

    foreach(const c3DBone::pair& bn, m_model->model_bones) {
        cEffectPoint t;

        t.name = bn.first;
        t.transforms.push_back(bn.second.m_pos[1]);
        wxString nam = _("Auto-generated form bone '")+bn.first+wxT("'");
        t.transformnames.push_back(nam);
        m_model->effectpoints.push_back(t);
    }

    m_htlbEffect->UpdateContents();
    m_htlbEffect->SetSelection(-1);
    UpdateControlState();
    */
    PopupMenu(m_menuAutoBones.get());
}

void dlgModel::OnEffectAutoQuick(wxMouseEvent& WXUNUSED(event)) {
    m_model->effectpoints.clear();

    foreach(const c3DBone::pair& bn, m_model->model_bones) {
        cEffectPoint t;

        t.name = bn.first;
        t.transforms.push_back(bn.second.m_pos[1]);
        wxString nam = _("Auto-generated form bone '")+bn.first+wxT("'");
        t.transformnames.push_back(nam);
        m_model->effectpoints.push_back(t);
    }

    m_htlbEffect->UpdateContents();
    m_htlbEffect->SetSelection(-1);
    UpdateControlState();
}

void dlgModel::OnEffectClear(wxCommandEvent& WXUNUSED(event)) {
    if (m_model->effectpoints.size()) {
        if (::wxMessageBox(_("Do you really want to delete all effect point entries?"), _("Question"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION, this)==wxNO)
            return;
    }

    m_model->effectpoints.clear();

    m_htlbEffect->UpdateContents();
    m_htlbEffect->SetSelection(-1);
    UpdateControlState();
}

/** @brief OnSortBones
  *
  * @todo: document this function
  */
void dlgModel::OnSortBones(wxCommandEvent& event) {
    SetSortBones(m_cbSortBones->GetValue());
}

/** @brief OnSyncBones
  *
  * @todo: document this function
  */
void dlgModel::OnSyncBones(wxCommandEvent& event) {
    SetSyncBones(m_cbSyncBones->GetValue());
}

/** @brief OnSyncBones
  *
  * @todo: document this function
  */
void dlgModel::OnDelBones(wxCommandEvent& event) {
	SetDelBones(m_cbDelBones->GetValue()) ;
}

void dlgModel::SetSyncBones(bool new_setting) {
	if (new_setting) {
		if (::wxMessageBox(_("This will undo all manual changes to bones defined in your model file! Do you want to continue?"), _("Warning"),  wxYES_NO, this) == wxNO)
			return;
	}
    m_model->auto_bones = new_setting;
	if (!new_setting)
		m_model->auto_delete_bones = false;

    if (m_model->auto_bones) {
		if (m_model->auto_delete_bones)
			m_model->deleteBones();
        m_model->syncBones();
		if (m_model->auto_delete_bones)
			m_model->addBones();
    }
    UpdateAll();
}

void dlgModel::SetDelBones(bool new_setting) {
	if (new_setting) {
		if (::wxMessageBox(_("This will delete all manually added bones and add all bones defined in the model not in the list! Do you want to continue?"), _("Warning"),  wxYES_NO, this) == wxNO)
			return;
	}
    m_model->auto_delete_bones = new_setting;
    if (m_model->auto_bones) {
		if (m_model->auto_delete_bones)
			m_model->deleteBones();
        m_model->syncBones();
		if (m_model->auto_delete_bones)
			m_model->addBones();
    }
    UpdateAll();
}

void dlgModel::SetSortBones(bool new_setting) {
	if (new_setting) {
		if (::wxMessageBox(_("This will undo all manual changes to the bone order and sort them alphabetically! Do you want to continue?"), _("Warning"),  wxYES_NO, this) == wxNO)
			return;
	}
    m_model->auto_sort = new_setting;
    if (m_model->auto_sort) {
        m_model->sortBones();
    }
    UpdateAll();
}

bool dlgModel::IsBoneEditable(int nr) {
	if (!m_model->auto_bones) {
		return true;
	}
	if (m_animated) {
		cAnimatedModel* amodel = dynamic_cast<cAnimatedModel*>(m_model);
		return !has(m_model->model_bones, amodel->modelbones[nr].name);
	} else {
		return !has(m_model->model_bones, m_model->effectpoints[nr].name);
	}
}

////////////////////////////////////////////////////////////////////////
//
//  dlgModel, Bones
//
////////////////////////////////////////////////////////////////////////

void dlgModel::OnBoneUp(wxSpinEvent& WXUNUSED(event)) {
    cAnimatedModel* amodel = dynamic_cast<cAnimatedModel*>(m_model);
    int sel = m_htlbEffect->GetSelection();
    if (sel < 1)
        return;
    cModelBone ft = amodel->modelbones[sel];
    amodel->modelbones.erase(amodel->modelbones.begin() + sel);
    amodel->modelbones.insert(amodel->modelbones.begin() + sel - 1, ft);

    m_htlbEffect->UpdateContents();
    m_htlbEffect->SetSelection(sel-1);
    UpdateControlState();
}

void dlgModel::OnBoneDown(wxSpinEvent& WXUNUSED(event)) {
    cAnimatedModel* amodel = dynamic_cast<cAnimatedModel*>(m_model);
    int count = amodel->modelbones.size();
    int sel = m_htlbEffect->GetSelection();
    if ((count-sel) <= 1)
        return;
    cModelBone ft = amodel->modelbones[sel];
    amodel->modelbones.erase(amodel->modelbones.begin() + sel);
    amodel->modelbones.insert(amodel->modelbones.begin() + sel + 1, ft);

    m_htlbEffect->UpdateContents();
    m_htlbEffect->SetSelection(sel+1);
    UpdateControlState();
}

void dlgModel::OnBoneAdd(wxCommandEvent& WXUNUSED(event)) {
    dlgBone *dialog = new dlgBone(this);
    dialog->SetBone(cModelBone());
    if (dialog->ShowModal() == wxID_OK) {
        cAnimatedModel* amodel = dynamic_cast<cAnimatedModel*>(m_model);
        amodel->modelbones.push_back(dialog->GetBone());

        foreach(cMeshStruct& ms, m_model->meshstructs) {
            if (ms.multibone) {
                foreach(const wxString& mname, amodel->modelbones[amodel->modelbones.size()-1].meshes) {
                    if (mname == ms.Name) {
                        ms.multibone = false;
                        break;
                    }
                }
            }
        }


        m_htlbEffect->UpdateContents();
        m_htlbEffect->SetSelection(amodel->modelbones.size()-1);
        UpdateAll();
    }
    dialog->Destroy();
}

void dlgModel::OnBoneEdit(wxCommandEvent& WXUNUSED(event)) {
    cAnimatedModel* amodel = dynamic_cast<cAnimatedModel*>(m_model);
    int sel = m_htlbEffect->GetSelection();
    if (sel < 0)
        return;
    dlgBone *dialog = new dlgBone(this, !IsBoneEditable(sel));
    dialog->SetBone(amodel->modelbones[sel]);
    if (dialog->ShowModal() == wxID_OK) {
		wxString oldname = amodel->modelbones[sel].name;
        amodel->modelbones[sel] = dialog->GetBone();

		foreach(cMeshStruct& ms, m_model->meshstructs) {
            if (ms.multibone) {
                foreach(const wxString& mname, amodel->modelbones[sel].meshes) {
                    if (mname == ms.Name) {
                        ms.multibone = false;
                        break;
                    }
                }
            }
        }

		if (amodel->modelbones[sel].name != oldname) {
			wxString& newname = amodel->modelbones[sel].name;
			foreach(cModelBone& b, amodel->modelbones) {
				if (b.parent == oldname)
					b.parent = newname;
			}
			
			if (!m_boneshutup) {
				bool affects_others = false;
				foreach(cAnimatedModel& m, m_scn.animatedmodels) {
					if (m.guid == amodel->guid)
						continue;
					foreach (cModelBone& b, m.modelbones) {
						if (b.name == oldname) {
							affects_others = true;
							break;
						}
					}
					if (affects_others)
						break;
				}
				if (!affects_others) {
					foreach(cAnimation& a, m_scn.animations) {
						foreach(cBoneAnimation& b, a.boneanimations) {
							if (b.name == oldname) {
								affects_others = true;
								break;
							}
						}
						if (affects_others)
							break;
					}
				}
				if (affects_others) {
					if (::wxMessageBox("The bone you just renamed is also present in other animated models and/or animations. "
									   "Do you want to automatically rename all of these instances?\n\n"
									   "WARNING: Pressing cancel on this model window will NOT undo these automatic changes, but it "
									   "WILL undo the change of the name of the bone you just did for this model. In short, "
									   "if you say yes, do not press cancel.\n\n"
									   "If you say no, the Importer will not bother you anymore about this until you close "
									   "and reopen the model window.", "Question", wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION, this) == wxYES) {
						foreach(cAnimatedModel& m, m_scn.animatedmodels) {
							if (m.guid == amodel->guid)
								continue;
							foreach (cModelBone& b, m.modelbones) {
								if (b.name == oldname) {
									b.name = newname;
								}
								if (b.parent == oldname) {
									b.parent = newname;
								}
							}
						}
						foreach(cAnimation& a, m_scn.animations) {
							foreach(cBoneAnimation& b, a.boneanimations) {
								if (b.name == oldname) {
									b.name = newname;
								}
							}
						}
					} else {
						m_boneshutup = true;
					}
				}
			}
		}

        m_htlbEffect->UpdateContents();
        UpdateAll();
    }
    dialog->Destroy();
}

void dlgModel::OnBoneCopy(wxCommandEvent& WXUNUSED(event)) {
    cAnimatedModel* amodel = dynamic_cast<cAnimatedModel*>(m_model);
    int sel = m_htlbEffect->GetSelection();

    cModelBone ft = amodel->modelbones[sel];
    ft.name += _(" Copy");
    amodel->modelbones.insert(amodel->modelbones.begin() + sel + 1, ft);

    m_htlbEffect->UpdateContents();
    m_htlbEffect->SetSelection(sel+1);
    UpdateControlState();
}

void dlgModel::OnBoneDel(wxCommandEvent& WXUNUSED(event)) {
    cAnimatedModel* amodel = dynamic_cast<cAnimatedModel*>(m_model);
    int sel = m_htlbEffect->GetSelection();
    amodel->modelbones.erase(amodel->modelbones.begin() + sel);

    m_htlbEffect->UpdateContents();
    m_htlbEffect->SetSelection(sel-1);
    UpdateControlState();
}

void dlgModel::OnBoneAuto(wxCommandEvent& WXUNUSED(event)) {
    /*
    cAnimatedModel* amodel = dynamic_cast<cAnimatedModel*>(m_model);
    if (amodel->modelbones.size()) {
        if (::wxMessageBox(_("Do you want to delete all current effect point entries before automatically creating new ones?"), _("Question"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION, this)==wxYES) {
            amodel->modelbones.clear();
        }
    }

    foreach(const c3DBone::pair& bn, m_model->model_bones) {
        cModelBone t;

        t.name = bn.first;
        t.parent = bn.second.m_parent;
        t.usepos2 = !t.parent.IsEmpty();
        if (t.usepos2) {
            t.positions1.push_back(bn.second.m_pos[0]);
            wxString nam = _("Auto-generated form bone '")+bn.first+wxT("'");
            t.position1names.push_back(nam);
            t.positions2.push_back(bn.second.m_pos[1]);
            t.position2names.push_back(nam);
        } else {
            t.positions1.push_back(bn.second.m_pos[1]);
            wxString nam = _("Auto-generated form bone '")+bn.first+wxT("'");
            t.position1names.push_back(nam);
        }
        foreach(const cMeshStruct& ms, m_model->meshstructs) {
            if (ms.boneassignment.size() == 1) {
                if (ms.boneassignment.find(t.name) != ms.boneassignment.end()) {
                    t.meshes.push_back(ms.Name);
                }
            }
        }
        amodel->modelbones.push_back(t);
    }

    m_htlbEffect->UpdateContents();
    m_htlbEffect->SetSelection(-1);
    UpdateControlState();
    */
    PopupMenu(m_menuAutoBones.get());
}

void dlgModel::OnBoneAutoQuick(wxMouseEvent& WXUNUSED(event)) {
    cAnimatedModel* amodel = dynamic_cast<cAnimatedModel*>(m_model);
    amodel->modelbones.clear();

    foreach(const c3DBone::pair& bn, m_model->model_bones) {
        cModelBone t;

        t.name = bn.first;
        t.parent = bn.second.m_parent;
        t.usepos2 = !t.parent.IsEmpty();
        if (t.usepos2) {
            t.positions1.push_back(bn.second.m_pos[0]);
            wxString nam = _("Auto-generated form bone '")+bn.first+wxT("'");
            t.position1names.push_back(nam);
            t.positions2.push_back(bn.second.m_pos[1]);
            t.position2names.push_back(nam);
        } else {
            t.positions1.push_back(bn.second.m_pos[1]);
            wxString nam = _("Auto-generated form bone '")+bn.first+wxT("'");
            t.position1names.push_back(nam);
        }
        foreach(const cMeshStruct& ms, m_model->meshstructs) {
            if (ms.boneassignment.size() == 1) {
                if (ms.boneassignment.find(t.name) != ms.boneassignment.end()) {
                    t.meshes.push_back(ms.Name);
                }
            }
        }
        amodel->modelbones.push_back(t);
    }

    m_htlbEffect->UpdateContents();
    m_htlbEffect->SetSelection(-1);
    UpdateControlState();
    ::wxBell();
}

void dlgModel::OnBoneClear(wxCommandEvent& WXUNUSED(event)) {
    cAnimatedModel* amodel = dynamic_cast<cAnimatedModel*>(m_model);
    if (amodel->modelbones.size()) {
        if (::wxMessageBox(_("Do you really want to delete all bone entries?"), _("Question"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION, this)==wxNO)
            return;
    }

    amodel->modelbones.clear();

    m_htlbEffect->UpdateContents();
    m_htlbEffect->SetSelection(-1);
    UpdateControlState();
}

////////////////////////////////////////////////////////////////////////
//
//  dlgModel, Auto menu
//
////////////////////////////////////////////////////////////////////////
/** @brief OnMenuSortBones
  *
  * @todo: document this function
  */
void dlgModel::OnMenuSortBones(wxCommandEvent& event) {
    m_model->sortBones();
    m_htlbEffect->UpdateContents();
    m_htlbEffect->SetSelection(-1);
    UpdateControlState();
}

/** @brief OnMenuSyncBones
  *
  * @todo: document this function
  */
void dlgModel::OnMenuSyncBones(wxCommandEvent& event) {
    m_model->syncBones();
    m_htlbEffect->UpdateContents();
    m_htlbEffect->SetSelection(-1);
    UpdateControlState();
}

/** @brief OnMenuAddMissingBones
  *
  * @todo: document this function
  */
void dlgModel::OnMenuAddMissingBones(wxCommandEvent& event) {
    if (m_animated) {
        foreach(const c3DBone::pair& bn, m_model->model_bones) {
            if (!contains_if(m_amodel.modelbones, NamePredicate<cModelBone>(bn.second))) {
                m_model->addBone(bn.second);
            }
        }
    } else {
        foreach(const c3DBone::pair& bn, m_model->model_bones) {
            if (!contains_if(m_model->effectpoints, NamePredicate<cEffectPoint>(bn.second))) {
                m_model->addBone(bn.second);
            }
        }
    }
    if (m_model->auto_sort)
        m_model->sortBones();
    m_htlbEffect->UpdateContents();
    m_htlbEffect->SetSelection(-1);
    UpdateControlState();
}

/** @brief OnMenuAutoSortBones
  *
  * @todo: document this function
  */
void dlgModel::OnMenuAutoSortBones(wxCommandEvent& event) {
    SetSortBones(!m_model->auto_sort);
}

/** @brief OnMenuAutoSyncBones
  *
  * @todo: document this function
  */
void dlgModel::OnMenuAutoSyncBones(wxCommandEvent& event) {
    SetSyncBones(!m_model->auto_bones);
}

void dlgModel::OnMenuAutoDelBones(wxCommandEvent& event) {
    SetDelBones(!m_model->auto_delete_bones);
}


////////////////////////////////////////////////////////////////////////
//
//  dlgModel, Load
//
////////////////////////////////////////////////////////////////////////
/*
wxString doEntity(wxString inset, ssgEntity* en) {
    wxString ret = inset + en->getTypeName();
    ssgBranch* br = dynamic_cast<ssgBranch*>(en);
    if (br) {
        ret +=  wxString::Format(wxT("(%d)"), br->getNumKids()) + wxT("\n");
        for (int i = 0; i < br->getNumKids(); ++i) {
            ret += doEntity(inset+wxT("  "), br->getKid(i));
        }
    } else {
        ssgLeaf* le = dynamic_cast<ssgLeaf*>(en);
        if (le) {
            ret +=  wxString::Format(wxT("(%d)"), le->getNumVertices());
        }
        ret += wxT("\n");
    }
    return ret;
}

class debugLoaderOptions: public ssgLoaderOptions
{
public:
  debugLoaderOptions ():ssgLoaderOptions() {};

  virtual void makeModelPath ( char* path, const char *fname ) const {
    wxLogDebug(wxT("debugLoaderOptions::makeModelPath( \"%s\", \"%s\" )"), path, fname);
    ssgLoaderOptions::makeModelPath ( path, fname );
  }
  virtual void makeTexturePath ( char* path, const char *fname ) const {
    wxLogDebug(wxT("debugLoaderOptions::makeTexturePath( \"%s\", \"%s\" )"), path, fname);
    ssgLoaderOptions::makeTexturePath ( path, fname );
  }

  virtual ssgLeaf* createLeaf ( ssgLeaf* leaf, const char* parent_name ) {
    wxLogDebug(wxT("debugLoaderOptions::createLeaf( %08u, \"%s\" )"), leaf, parent_name);
    return ssgLoaderOptions::createLeaf ( leaf, parent_name ) ;
  }
  virtual ssgTexture* createTexture ( char* tfname,
			      int wrapu  = TRUE, int wrapv = TRUE,
			      int mipmap = TRUE ) {
    wxLogDebug(wxT("debugLoaderOptions::createTexture( \"%s\", %d, %d, %d )"), tfname, wrapu, wrapv, mipmap);
    return ssgLoaderOptions::createTexture (tfname, wrapu, wrapv, mipmap) ;
  }
  virtual ssgTransform* createTransform ( ssgTransform* tr,
      ssgTransformArray* ta ) const {
    wxLogDebug(wxT("debugLoaderOptions::createLeaf( %08u, %08u )"), tr, ta);
    return ssgLoaderOptions::createTransform ( tr, ta );
  }
  virtual ssgSelector* createSelector ( ssgSelector* s ) const {
    wxLogDebug(wxT("debugLoaderOptions::createSelector( %08u )"), s);
    return ssgLoaderOptions::createSelector ( s );
  }
  virtual ssgBranch* createBranch ( char* text ) const {
    wxLogDebug(wxT("debugLoaderOptions::createBranch( \"%s\" )"), text);
    return ssgLoaderOptions::createBranch ( text );
  }
  virtual ssgState* createState ( char* tfname ) const {
    wxLogDebug(wxT("debugLoaderOptions::createState( \"%s\" )"), tfname);
    return ssgLoaderOptions::createState ( tfname ) ;
  }
  virtual ssgSimpleState* createSimpleState ( char* tfname ) const {
    wxLogDebug(wxT("debugLoaderOptions::createSimpleState( \"%s\" )"), tfname);
    return ssgLoaderOptions::createSimpleState ( tfname ) ;
  }

  virtual void endLoad ()
  {
    wxLogDebug(wxT("debugLoaderOptions::endLoad()"));
    ssgLoaderOptions::endLoad ();
  }

} ;

void plibErrorCallback ( enum ulSeverity severity, char* msg ) {
    switch (severity) {
        case UL_DEBUG:
            wxLogDebug(msg);
            break;
        case UL_WARNING:
            wxLogWarning(msg);
            break;
        case UL_FATAL:
        case UL_MAX_SEVERITY:
            wxLogError(msg);
    }
}
*/
void dlgModel::OnLoad(wxCommandEvent& WXUNUSED(event)) {
    //wxMessageBox(_("This functionality is currently disabled. Please use the 'L' button in the Create Scenery window."));
    m_loadoverlay = true;
    m_textModelFile->OnButtonClick();
/*
    wxFileDialog *dialog = new wxFileDialog(
                                     this,
                                     _T("Open Scenery File (Load model)"),
                                     wxEmptyString,
                                     wxEmptyString,
                                     _T("Scenery Files (*.scn)|*.scn"),
                                     wxFD_FILE_MUST_EXIST|wxFD_CHANGE_DIR,
                                     wxDefaultPosition,
                                     wxSize(600,400)
                                 );
    dialog->SetDirectory(::wxGetApp().g_workdir.GetPath());
    if (dialog->ShowModal() == wxID_OK) {
        ::wxGetApp().g_workdir.AssignDir(wxFileName(dialog->GetPath()).GetPath());
        cSCNFile *texscn = new cSCNFile(dialog->GetPath().fn_str());
        if (texscn) {
            if (texscn->models.size()==0) {
                wxMessageBox(_("The selected scenery file contains no models."), _("Warning"), wxOK | wxICON_WARNING, this);
                delete texscn;
                dialog->Destroy();
                return;
            }
            wxArrayString choices;
            for (cModel::iterator ms = texscn->models.begin(); ms != texscn->models.end(); ms++) {
                choices.Add(ms->name);
            }
            if (choices.size() > 0) {
                int choice = ::wxGetSingleChoiceIndex(dialog->GetPath(), _("Choose model to import"), choices, this);
                if (choice >= 0) {
                    SetModel(texscn->models[choice]);
                    TransferDataToWindow();
                    UpdateAll();
                }
            }
            delete texscn;
        }
    }
    dialog->Destroy();
*/
}

