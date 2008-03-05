///////////////////////////////////////////////////////////////////////////////
//
// RCT3 Importer TNG
// Custom object importer for Ataris Roller Coaster Tycoon 3
// Copyright (C) 2008 Belgabor (Tobias Minich)
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 3
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, <http://www.gnu.org/licenses/>.
//
// Written by
//   Belgabor (Tobias Minich) - belgabor@gmx.de
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _IMPRAWOVLFILEMANAGER_H
#define _IMPRAWOVLFILEMANAGER_H


#include "cXmlDoc.h"
#include "cXmlXPath.h"

#include <sigc++/signal.h>
#include <sigc++/trackable.h>
#include <wx/aui/aui.h>

#include <vector>

#include "impTypes.h"
#include "sigchelper.h"

struct impUndoState {
    xmlcpp::cXmlDoc state;
    bool dirty;
    impUndoState(const xmlcpp::cXmlDoc& _state, bool _dirty = true):state(_state.getRaw(), false), dirty(_dirty) {}
};

class impView;
class impRawovlFileManager : public sigc::trackable {
public:
    class UpdateFreezer: public sigc::trackable {
    private:
        impRawovlFileManager* m_man;
        inline void freeze(bool _freeze) const {
            if (m_man)
                m_man->freeze(_freeze);
        }
    public:
        UpdateFreezer(): m_man(NULL) {};
        UpdateFreezer(impRawovlFileManager& man): m_man(&man) {
            freeze(true);
        }
        UpdateFreezer(const UpdateFreezer& _freeze): m_man(_freeze.m_man) {
            freeze(true);
        }
        UpdateFreezer& operator= (const UpdateFreezer& _freeze) {
            _freeze.freeze(true);
            freeze(false);
            m_man = _freeze.m_man;
        }
        ~UpdateFreezer() {
            freeze(false);
        }
    };

private:
    static impRawovlFileManager g_manager;

    xmlcpp::cXmlDoc m_xml;
    bool m_dirty;
    wxString m_filename;
    int m_freeze;

    std::vector<impUndoState> m_undostack;
    std::vector<impUndoState> m_redostack;
    wxString m_undoPath;
    wxString m_undoValue;
    wxString m_undoOldValue;

    impRawovlFileManager();
    virtual ~impRawovlFileManager();

    void freeze(bool freeze = true);
    void undoPush();
    void undoClean();
    void undoChangeValue(const wxString& path, const wxString& value);

public: // Signals
    sigc::signal<void, const wxString&> updateSignal;
    sigc::signal<void, bool> dirtyChange;
    sigc::signal<void, wxLogLevel, const wxChar*, const wxChar*, time_t> log;
    sigc::signal0<wxAuiNotebook*, PointerReturner<wxAuiNotebook> > getNotebook;

private: // Slots
    void setDirty(bool ndirty) { m_dirty = ndirty; }

public: // Slots
    void unregisterView(const impView& view);
    void changeValue(const wxString& path, const wxString& value);
    void newNode(const wxString& parent, xmlcpp::cXmlNode& node);
    void deleteNode(const wxString& parent, xmlcpp::cXmlNode& node);
    void executeCommand(impTypes::COMMAND command, impTypes::ELEMENTTYPE elementtype, impTypes::VIEWTYPE viewtype, const wxString& path);

    void undo();
    size_t canUndo();
    void redo();
    size_t canRedo();

    UpdateFreezer getFreezer();

    xmlcpp::cXmlDoc getXml();
    wxString getProjectFileName();
    xmlcpp::cXmlXPath getXPath();
    bool prepareReset(bool cancancel = true);

public:
    inline static impRawovlFileManager& getManager() { return g_manager; }
    static wxString newGUID();

    void update(const wxString& path) {
        if (m_freeze)
            return;
        updateSignal(path);
    }

    void newProject(bool silent = false);
    void loadProject(bool silent = false);
    bool saveProject(bool silent = false);
    bool saveasProject(bool silent = false);

    void registerView(impView& view);


    void createView(impTypes::VIEWTYPE type, const wxString& path);

};


#endif
