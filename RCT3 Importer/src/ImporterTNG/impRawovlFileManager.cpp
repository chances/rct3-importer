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

#include "impRawovlFileManager.h"

#include <boost/shared_ptr.hpp>
#include <cryptopp/osrng.h>

#include "xmldefs.h"
#include "cXmlDoc.h"
#include "cXmlXPath.h"
#include "cXmlXPathResult.h"

#include "wxutilityfunctions.h"

#include "impElement.h"
#include "impView.h"
#include "impViewProject.h"
#include "importerapp.h"
#include "importerconfig.h"
#include "importerresourcefiles.h"
#include "importersave.h"
#include "importerxpaths.h"

using namespace xmlcpp;
using namespace std;

impRawovlFileManager impRawovlFileManager::g_manager;

wxString impRawovlFileManager::newGUID() {
    CryptoPP::AutoSeededRandomPool rp;
    unsigned short guid[8];
    rp.GenerateBlock(reinterpret_cast<byte*>(guid), 16);
    return wxString::Format(wxT("%04hX%04hX-%04hX-%04hX-%04hX-%04hX%04hX%04hX"), guid[0], guid[1], guid[2], guid[3], guid[4], guid[5], guid[6], guid[7]);
}


impRawovlFileManager::impRawovlFileManager():
        m_dirty(false) {
    dirtyChange.connect(sigc::mem_fun(*this, &impRawovlFileManager::setDirty));
}

impRawovlFileManager::~impRawovlFileManager() {
}

void impRawovlFileManager::freeze(bool freeze) {
    if (freeze) {
        undoPush();
        ++m_freeze;
    } else {
        --m_freeze;
    }
    update(wxT("/"));
}

void impRawovlFileManager::undoPush() {
    if (m_freeze)
        return;
    m_undostack.push_back(impUndoState(m_xml, m_dirty));
    undoClean();
    m_redostack.clear();
}

void impRawovlFileManager::undoClean() {
    if (m_freeze)
        return;
    m_undoPath = wxT("");
    m_undoValue = wxT("");
    m_undoOldValue = wxT("");
}

void impRawovlFileManager::undoChangeValue(const wxString& path, const wxString& value) {
    if (m_freeze)
        return;
    if (path == m_undoPath) {
        m_undoValue = value;
        return;
    }

    undoPush();
    m_undoPath = path;
    m_undoValue = value;
    cXmlNode res = getXPath()(path.utf8_str())[0];
    if (res) {
        m_undoOldValue = res.wxcontent();
    } else {
        wxLogError(_("PANIC! XPath faild in undoChangeValue"));
    }
}


cXmlXPath impRawovlFileManager::getXPath() {
    return cXmlXPath(m_xml, "r", XML_NAMESPACE_RAW);
}

void impRawovlFileManager::newProject(bool silent) {
    if (!prepareReset())
        return;

    m_xml = cXmlDoc(wxString(wxT(MEMORY_PREFIX RES_TEMPLATE_PROJECT)).mb_str(wxConvUTF8), NULL, XML_PARSE_DTDLOAD);
    cXmlXPath c_xpath = getXPath();
    if (c_xpath) {
        cXmlXPathResult res = c_xpath(XPATH_PROJECT_NAME);
        if (res.size()) {
            res[0].content(wxString(_("New Project")).mb_str(wxConvUTF8));
            res = c_xpath(XPATH_PROJECT_METADATA);
            if (res.size()) {
                cXmlNode guidnode(XML_TAG_METADATA_GUID, WX2UTF8(newGUID()), getXml().defaultNs());
                res[0].appendChildren(guidnode);
            } else {
                log(wxLOG_Error, wxT("Internal Error: project template xpath no result"), wxT(""), 0);
            }
        } else {
            log(wxLOG_Error, wxT("Internal Error: project template xpath no result"), wxT(""), 0);
        }
    } else {
        log(wxLOG_Error, wxT("Internal Error: project template xpath broken"), wxT(""), 0);
    }

    if (!silent)
        wxLogMessage(_("New project started"));
    dirtyChange(false);
    m_filename = wxT("");
    update(wxT("/"));
}

void impRawovlFileManager::loadProject(bool silent) {
    if (!prepareReset())
        return;

    boost::shared_ptr<wxFileDialog> dialog(new wxFileDialog(
                                     wxGetApp().GetTopWindow(),
                                     _("Open project file"),
                                     wxEmptyString,
                                     wxEmptyString,
                                     _("Importer project files (*.xml)|*.xml"),
                                     wxFD_FILE_MUST_EXIST|wxFD_CHANGE_DIR,
                                     wxDefaultPosition,
                                     READ_APP_SIZE("FileDialog", wxSize(600,400))
                                 ), wxWindowDestroyer);
    dialog->SetDirectory(READ_APP_WORKDIR());
    if (dialog->ShowModal() == wxID_OK) {
        WRITE_APP_WORKDIR(dialog->GetPath());
        WRITE_APP_SIZE("FileDialog", dialog->GetSize());

        wxString tempfn = dialog->GetFilename();
        cXmlDoc temp(WX2UTF8(tempfn), NULL, XML_PARSE_DTDLOAD);
        if (!temp) {
            wxLogError(_("XML error(s) loading file '%s':"), tempfn.c_str());
            for (vector<cXmlStructuredError>::const_iterator it = temp.getStructuredErrors().begin(); it != temp.getStructuredErrors().end(); ++it)
                wxLogError(UTF8STRINGWRAP(it->message).Trim(true));
            for (vector<string>::const_iterator it = temp.getGenericErrors().begin(); it != temp.getGenericErrors().end(); ++it)
                wxLogError(UTF8STRINGWRAP((*it)));
            return;
        }

        if (!temp.root()(RAW_NS(XML_TAG_OVL), true)) {
            wxLogError(_("Error loading file '%s': Not an importer project file (wrong root tag '%s')."), tempfn.c_str(), STRING_FOR_FORMAT(temp.root().name()));
            return;
        }

        m_filename = tempfn;
        m_xml = temp;
        dirtyChange(false);
        if (!silent)
            wxLogMessage(_("Successfully loaded file '%s'."), tempfn.c_str());
        update(wxT("/"));
    }
}

bool impRawovlFileManager::saveProject(bool silent) {
    if (m_filename.IsEmpty()) {
        return saveasProject(silent);
    }

    if (m_xml.write(m_filename, true, wxString(wxT("UTF-8"))) > 0) {
        dirtyChange(false);
        if (!silent)
            wxLogMessage(_("Project saved successfully."));
    } else {
        wxLogError(_("Saving project failed."));
        return false;
    }
    return true;
}

bool impRawovlFileManager::saveasProject(bool silent) {

    boost::shared_ptr<wxFileDialog> dialog (new wxFileDialog(
                               wxGetApp().GetTopWindow(),
                               _("Save project file"),
                               wxEmptyString,
                               wxEmptyString,
                               _("Importer project files (*.xml)|*.xml"),
                               wxFD_SAVE|wxFD_OVERWRITE_PROMPT|wxFD_CHANGE_DIR,
                               wxDefaultPosition,
                               READ_APP_SIZE("FileDialog", wxSize(600,400))
                           ), wxWindowDestroyer);
    if (!m_filename.IsEmpty()) {
        dialog->SetFilename(wxFileName(m_filename).GetFullName());
        dialog->SetDirectory(wxFileName(m_filename).GetPath());
    } else {
        cXmlXPath c_xpath = getXPath();
        wxString tfn;
        if (c_xpath) {
            cXmlXPathResult res = c_xpath(XPATH_PROJECT_NAME);
            if (res.size()) {
                tfn = UTF8STRINGWRAP(res[0].content());
            }
        }
        if (!tfn.IsEmpty())
            dialog->SetFilename(tfn+wxT(".xml"));
        dialog->SetDirectory(READ_APP_WORKDIR());
    }
    if (dialog->ShowModal() == wxID_OK) {
        WRITE_APP_WORKDIR(dialog->GetPath());
        WRITE_APP_SIZE("FileDialog", dialog->GetSize());

        m_filename = dialog->GetPath();
        return saveProject(silent);
    } else {
        return false;
    }
}

cXmlDoc impRawovlFileManager::getXml() {
    return m_xml;
}

wxString impRawovlFileManager::getProjectFileName() {
    return m_filename;
}


void impRawovlFileManager::registerView(impView& view) {
    updateSignal.connect(sigc::mem_fun(view, &impView::update));

    view.getXml.connect(sigc::mem_fun(*this, &impRawovlFileManager::getXml));
    view.getXPath.connect(sigc::mem_fun(*this, &impRawovlFileManager::getXPath));
    view.getFreezer.connect(sigc::mem_fun(*this, &impRawovlFileManager::getFreezer));
    view.changeValue.connect(sigc::mem_fun(*this, &impRawovlFileManager::changeValue));
    view.newNode.connect(sigc::mem_fun(*this, &impRawovlFileManager::newNode));
    view.deleteNode.connect(sigc::mem_fun(*this, &impRawovlFileManager::deleteNode));

    view.unregister.connect(sigc::mem_fun(*this, &impRawovlFileManager::unregisterView));
    view.executeCommand.connect(sigc::mem_fun(*this, &impRawovlFileManager::executeCommand));

    view.update(wxT("/"));
}

void impRawovlFileManager::unregisterView(const impView& view) {
    wxLogMessage(wxT("unregisterView called"));
}

void impRawovlFileManager::changeValue(const wxString& path, const wxString& value) {
    cXmlXPath p = getXPath();
    cXmlNode res = p(path.utf8_str())[0];
    if (res) {
        if (res.wxcontent() != value) {
            undoChangeValue(path, value);
            dirtyChange(true);
            res.wxcontent(value);
            update(res.wxpath());
        }
    } else {
        wxString valname = path.AfterLast(wxT('/'));
        res = p(path.BeforeLast(wxT('/')).utf8_str())[0];
        if (!res) {
            wxLogError(_("Could not update '%s': owning node doesn't exist."), path.c_str());
            return;
        }
        undoClean();
        undoChangeValue(path, value);
        if (valname[0] == wxT('@')) {
            // Attribute
            cXmlNode newnode = res.wxprop(valname.AfterFirst(wxT('@')), value);
            if (newnode) {
                dirtyChange(true);
                update(newnode.wxpath());
            }
        } else {
            cXmlNode newnode(valname.utf8_str(), value.utf8_str(), getXml().defaultNs());
            if (newnode) {
                dirtyChange(true);
                update(newnode.wxpath());
            }
        }
    }
}

void impRawovlFileManager::newNode(const wxString& parent, cXmlNode& node) {
    cXmlXPath p = getXPath();
    cXmlNode res = p(parent.utf8_str())[0];
    if (res) {
        undoPush();
        res.appendChildren(node);
        dirtyChange(true);
        update(parent);
    } else {
        wxLogError(_("Could not add new node: owning node '%s' doesn't exist."), parent.c_str());
    }
}

void impRawovlFileManager::deleteNode(const wxString& parent, cXmlNode& node) {
    undoPush();
    node.unlink();
    dirtyChange(true);
    update(parent);
}

void impRawovlFileManager::executeCommand(impTypes::COMMAND command, impTypes::ELEMENTTYPE elementtype, impTypes::VIEWTYPE viewtype, const wxString& path) {
    switch (command) {
    case impTypes::COMMAND_NOTHING:
        break;
    case impTypes::COMMAND_VIEW: {
            if (viewtype == impTypes::VIEWTYPE_ABSTRACT) {
                createView(impElements[elementtype].defaultview, path);
            } else {
                createView(viewtype, path);
            }
        }
        break;
    default:
        wxLogError(_("Unimplemented command called!"));
    }
}


void impRawovlFileManager::undo() {
    if (!canUndo())
        return;

    m_redostack.push_back(impUndoState(m_xml, m_dirty));
    m_undoPath = wxT("");
    m_undoValue = wxT("");
    m_undoOldValue = wxT("");

    impUndoState old = m_undostack.back();
    m_undostack.pop_back();
    m_xml = old.state;
    dirtyChange(old.dirty);

    update(wxT("/"));
}

size_t impRawovlFileManager::canUndo() {
    size_t ssize = m_undostack.size();
    if (!ssize) {
        if ((!m_undoPath.IsEmpty()) && (m_undoValue != m_undoOldValue))
            ssize++;
    }
    return ssize;
}

void impRawovlFileManager::redo() {
    if (!canRedo())
        return;

    m_undostack.push_back(impUndoState(m_xml, m_dirty));
    m_undoPath = wxT("");
    m_undoValue = wxT("");
    m_undoOldValue = wxT("");

    impUndoState old = m_redostack.back();
    m_redostack.pop_back();
    m_xml = old.state;
    dirtyChange(old.dirty);

    update(wxT("/"));
}

size_t impRawovlFileManager::canRedo() {
    return m_redostack.size();
}

impRawovlFileManager::UpdateFreezer impRawovlFileManager::getFreezer() {
    return impRawovlFileManager::UpdateFreezer(*this);
}

bool impRawovlFileManager::prepareReset(bool cancancel) {
    if (!m_dirty)
        return true;

    dlgSave* dlg = new dlgSave(_("Unsaved changes in your current project file.\nWhat do you want to do?"), cancancel, _("Unsaved Changes"), wxGetApp().GetTopWindow());
    switch (dlg->ShowModal()) {
        case wxID_NO:
            return true;
        case wxID_SAVE:
            return saveProject();
        default:
            return false;
    }

}

void impRawovlFileManager::createView(impTypes::VIEWTYPE type, const wxString& path) {
    wxAuiNotebook* nb = getNotebook();
    if (nb) {
        impView* view = impView::createView(nb, type, path);
        if (view) {
            nb->AddPage(dynamic_cast<wxWindow*>(view), view->getName());
            view->setNotebook(nb);
            registerView(*view);
        }
    }
}

