///////////////////////////////////////////////////////////////////////////////
//
// Importing data from files Dialog
// Copyright (C) 2008 Tobias Minch
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
// Note: ressource in misc
//
///////////////////////////////////////////////////////////////////////////////

#ifndef DLGLOADFROMFILE_H
#define DLGLOADFROMFILE_H

#include "xrc\res_misc.h"

#include <vector>

#include "RCT3Structs.h"
#include "SCNFile.h"

class wxColourHtmlListBox;

class dlgLoadFromFile : public rcdlgLoadFromFile {
public:
    enum {
        Type_AnimatedModels,
        Type_Animations,
        Type_Models,
        Type_Splines,
        Type_Textures
    };
    dlgLoadFromFile(wxWindow* parent, cRCT3Xml& client, int type, bool* updated = NULL, wxColourHtmlListBox* update = NULL);
    int ShowModal();

    template<class T>
    static inline int type() { return T::breakme; }

protected:
    virtual void OnInitDialog( wxInitDialogEvent& event );
    virtual void OnFileSelect( wxCommandEvent& event );
    virtual void OnUpdateControlState( wxCommandEvent& event );
    virtual void OnListDblClk( wxCommandEvent& event );
    virtual void OnListRightUp( wxMouseEvent& event );
    virtual void OnLoad( wxCommandEvent& event );
    virtual void OnLoadClose( wxCommandEvent& event );
    virtual void OnClose( wxCommandEvent& event );

private:
    cRCT3Xml& m_client;
    std::vector<cRCT3Xml*> m_selections;
    int m_type;
    cSCNFile m_loaded;
    bool* m_updated;
    wxColourHtmlListBox* m_update;

    void ClearMarks();

    inline cSCNFile& scn() { return dynamic_cast<cSCNFile&>(m_client); }
    template<class T>
    inline T& getSelection(int i) { return dynamic_cast<T&>(*m_selections[i]); }

    template<class T>
    void FillSingular() {
        foreach(T& elem, m_loaded.collection<T>()) {
            m_checklbLoad->Append(NameExtractorC<T>(elem));
            m_selections.push_back(&elem);
        }
        if (m_loaded.collection<T>().size() == 1) {
            m_checklbLoad->Check(0, true);
        }
    }

    template<class T>
    void LoadSingular(int i) {
        typename T::iterator it = find_if(scn().collection<T>().begin(), scn().collection<T>().end(), NamePredicate<T>(m_checklbLoad->GetString(i)));
        if (it == scn().collection<T>().end()) {
            scn().collection<T>().push_back(getSelection<T>(i));
        } else {
            T t = getSelection<T>(i);
            int m = 2;
            while (find_if(scn().collection<T>().begin(), scn().collection<T>().end(), NamePredicate<T>(wxString::Format("%s %d", NameExtractorC<T>(t).c_str(), m))) != scn().collection<T>().end())
                m++;
            NameExtractor<T>(t) = wxString::Format("%s %d", NameExtractorC<T>(t).c_str(), m);
            scn().collection<T>().push_back(t);
        }
    }

    void UpdateControlState();
};

template<>
inline int dlgLoadFromFile::type<cAnimatedModel>() { return Type_AnimatedModels; }
template<>
inline int dlgLoadFromFile::type<cAnimation>() { return Type_Animations; }
template<>
inline int dlgLoadFromFile::type<cModel>() { return Type_Models; }
template<>
inline int dlgLoadFromFile::type<cImpSpline>() { return Type_Splines; }
template<>
inline int dlgLoadFromFile::type<cFlexiTexture>() { return Type_Textures; }


#endif // DLGLOADFROMFILE_H
