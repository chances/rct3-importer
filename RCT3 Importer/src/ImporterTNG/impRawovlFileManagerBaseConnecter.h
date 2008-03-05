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

#ifndef IMPRAWOVLFILEMANAGERBASECONNECTER_H_INCLUDED
#define IMPRAWOVLFILEMANAGERBASECONNECTER_H_INCLUDED

#include "cXmlDoc.h"
#include "cXmlXPath.h"

#include "impRawovlFileManager.h"
#include "sigchelper.h"

class impRawovlFileManagerBaseConnecter: public sigc::trackable {
public: // Signals
    sigc::signal0<xmlcpp::cXmlDoc, Returner<xmlcpp::cXmlDoc> > getXml;
    sigc::signal0<xmlcpp::cXmlXPath, Returner<xmlcpp::cXmlXPath> > getXPath;
    sigc::signal<impRawovlFileManager::UpdateFreezer> getFreezer;
    sigc::signal<void, const wxString&, const wxString&> changeValue;
    sigc::signal<void, const wxString&, xmlcpp::cXmlNode&> newNode;
    sigc::signal<void, const wxString&, xmlcpp::cXmlNode&> deleteNode;

    virtual ~impRawovlFileManagerBaseConnecter() {} // Enforce polymorphism
protected:
    void attachConnection(impRawovlFileManagerBaseConnecter* slotowner) {
        if (slotowner) {
            getXml.connect(slotowner->getXml.make_slot());
            getXPath.connect(slotowner->getXPath.make_slot());
            getFreezer.connect(slotowner->getFreezer.make_slot());
            changeValue.connect(slotowner->changeValue.make_slot());
            newNode.connect(slotowner->newNode.make_slot());
            deleteNode.connect(slotowner->deleteNode.make_slot());
        }
    }
};

#endif // IMPRAWOVLFILEMANAGERBASECONNECTER_H_INCLUDED
