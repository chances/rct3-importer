///////////////////////////////////////////////////////////////////////////////
//
// raw ovl xml interpreter
// Command line ovl creation utility
// Copyright (C) 2007 Tobias Minch
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
///////////////////////////////////////////////////////////////////////////////



#include "RawParse_cpp.h"

#include "ManagerBTBL.h"
#include "ManagerGSI.h"
#include "ManagerTXT.h"

#define RAWXML_METADATA "metadata"

void cRawParser::Parse(cXmlNode& node) {
    std::string oldprefix = m_prefix;
    ParseStringOption(m_prefix, node, wxT("prefix"), NULL, false);

    cXmlNode child(node.children());
    while (child) {
        DO_CONDITION_COMMENT(child);

        if (child(RAWXML_SECTION)) {
            wxString incfile = UTF8STRINGWRAP(child.getPropVal("include"));
            if (incfile.IsEmpty()) {
                Parse(child);
            } else {
                if (child.children())
                    throw MakeNodeException<RCT3Exception>("A section tag with include attribute may not have content", child);

                wxFSFileName filename = incfile;
                wxFSFileName filenamebake = incfile;
                if (!filename.IsAbsolute()) {
                    filename.MakeAbsolute(m_input.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME));
                    filenamebake.MakeAbsolute(m_bakeroot.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME));
                } else {
                    filenamebake.MakeRelativeTo(m_input.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME));
                    filenamebake.MakeAbsolute(m_bakeroot.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME));
                }

                if (m_mode == MODE_BAKE) {
                    if (m_bake.Index(wxT(RAWBAKE_XML)) == wxNOT_FOUND) {
                        wxString newfile;
                        if (m_bake.Index(wxT(RAWBAKE_ABSOLUTE)) == wxNOT_FOUND) {
                            filename.MakeRelativeTo(m_bakeroot.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME));
                        }
                        newfile = filename.GetFullPath();
                        child.delProp("include");
                        child.addProp("include", newfile.mb_str(wxConvUTF8));
                        child.go_next();
                        continue;
                    }
                }
/*
                wxXmlDocument doc;
                wxFileSystem fs;
                std::auto_ptr<wxFSFile> inputfsfile(fs.OpenFile(filename.GetFullPath(), wxFS_READ | wxFS_SEEKABLE));
                if (!inputfsfile.get())
                    throw RCT3Exception(_("Cannot read input file ")+filename.GetFullPath());

                doc.Load(*inputfsfile->GetStream());
                wxXmlNode* root = doc.GetRoot();
*/
                cXmlDoc doc(filename.GetFullPath().mb_str(wxConvUTF8), NULL, XML_PARSE_DTDLOAD);
                cXmlNode root(doc.root());
                if (!root(RAWXML_SECTION))
                    throw MakeNodeException<RCT3Exception>(wxString::Format(_("Included section raw xml file has wrong root '%s'."), STRING_FOR_FORMAT(root.name())), child);

                wxFSFileName bakebackup = m_bakeroot;
                wxFSFileName inputbackup = m_input;
                m_input = filename;
                m_bakeroot = filenamebake;
                Parse(root);
                m_input = inputbackup;
                m_bakeroot = bakebackup;

                if (m_mode == MODE_BAKE) {
                    root.unlink();
                    root.name(RAWXML_SECTION);
                    child.appendChildren(root);
                    child.delProp("include");
                }
            }
        } else if (child(RAWXML_SUBROOT)) {
            // see above or
            // <subovl include="raw xml file" />
            wxString incfile = UTF8STRINGWRAP(child.getPropVal("include"));

            cRawParser c_raw;
            c_raw.SetParent(this);
            c_raw.SetPrefix(m_prefix);
            c_raw.SetOptions(m_mode, m_dryrun);
            c_raw.PassBakeStructures(m_bake);
            if (m_mode == MODE_BAKE)
                c_raw.PassBakeRoot(m_bakeroot);
            c_raw.PassVariables(m_commandvariables, m_variables);
            if (incfile.IsEmpty()) {
                c_raw.Process(child, m_input, m_outputbasedir);
            } else {
                if (child.children())
                    throw MakeNodeException<RCT3Exception>("A rawovl tag with include attribute may not have content", child);

                wxFSFileName filename = incfile;
                if (!filename.IsAbsolute())
                    filename.MakeAbsolute(m_input.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME));

                if (m_mode == MODE_BAKE) {
                    if (m_bake.Index(wxT(RAWBAKE_XML)) == wxNOT_FOUND) {
                        wxString newfile;
                        if (m_bake.Index(wxT(RAWBAKE_ABSOLUTE)) == wxNOT_FOUND) {
                            filename.MakeRelativeTo(m_bakeroot.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME));
                        }
                        newfile = filename.GetFullPath();
                        child.delProp("include");
                        child.addProp("include", newfile.mb_str(wxConvUTF8));
                        child.go_next();
                        continue;
                    }
                }
/*
                wxXmlDocument doc;
                wxFileSystem fs;
                std::auto_ptr<wxFSFile> inputfsfile(fs.OpenFile(filename.GetFullPath(), wxFS_READ | wxFS_SEEKABLE));
                if (!inputfsfile.get())
                    throw RCT3Exception(_("Cannot read input file ")+filename.GetFullPath());

                doc.Load(*inputfsfile->GetStream());
                wxXmlNode* root = doc.GetRoot();
*/
                cXmlDoc doc(filename.GetFullPath().mb_str(wxConvUTF8), NULL, XML_PARSE_DTDLOAD);
                cXmlNode root(doc.root());
                c_raw.Process(root, filename, m_outputbasedir);
                if (m_mode == MODE_BAKE) {
                    root.unlink();
                    root.name(RAWXML_SUBROOT);
                    child.appendChildren(root);
                    child.delProp("include");
                }
            }
            for (std::vector<wxFileName>::const_iterator it = c_raw.GetModifiedFiles().begin(); it != c_raw.GetModifiedFiles().end(); ++it) {
                m_modifiedfiles.push_back(*it);
            }
            for (std::vector<wxFileName>::const_iterator it = c_raw.GetNewFiles().begin(); it != c_raw.GetNewFiles().end(); ++it) {
                m_newfiles.push_back(*it);
            }
        } else if (child(RAWXML_DATAREF)) {
            wxString guid = ParseString(child, wxT(RAWXML_DATAREF), wxT("guid"), NULL);
            guid.MakeUpper();
            m_datareferences[guid] = child;
        } else if (child(RAWXML_METADATA)) {
            // Ignore
        } else if (child(RAWXML_SET)) {
            ParseSet(child);
        } else if (child(RAWXML_UNSET)) {
            ParseUnset(child);
        } else if (child(RAWXML_VARIABLES)) {
            ParseVariables(child);
        } else if (child(RAWXML_CHECK)) {
            BAKE_SKIP(child);
            bool filenamevar;
            wxFileName filename = ParseString(child, wxT(RAWXML_CHECK), wxT("file"), &filenamevar);
            wxString what = ParseString(child, wxT(RAWXML_CHECK), wxT("for"), NULL);
            unsigned long filebase = FILEBASE_INSTALLDIR;
            OPTION_PARSE(unsigned long, filebase, ParseUnsigned(child, wxT(RAWXML_CHECK), wxT("filebase")));
            if (!filename.IsAbsolute()) {
                if (filebase == FILEBASE_INSTALLDIR)
                    filename.MakeAbsolute(m_outputbasedir.GetPathWithSep());
                else if (filebase == FILEBASE_USERDIR)
                    filename.MakeAbsolute(m_userdir.GetPathWithSep());
                else
                    throw MakeNodeException<RCT3Exception>(wxString::Format(_("A check tag has unknown filebase attribute '%ld'"),filebase), child);
            }
            if (!filename.FileExists()) {
                child.go_next();
                continue;
            }
            if (what == wxT("presence")) {
                ParseSet(child, false, wxT(RAWXML_CHECK));
            } else if (what == wxT("reference")) {
                wxString ref = ParseString(child, wxT(RAWXML_CHECK), wxT("parameter"), NULL);
                cOVLDump od;
                od.Load(filename.GetFullPath().mb_str(wxConvFile));
                for (std::vector<std::string>::const_iterator it = od.GetReferences(OVLT_UNIQUE).begin(); it != od.GetReferences(OVLT_UNIQUE).end(); ++it) {
                    wxString check(it->c_str(), wxConvFile);
                    if (check == ref) {
                        ParseSet(child, false, wxT(RAWXML_CHECK));
                        break;
                    }
                }
            } else {
                throw MakeNodeException<RCT3Exception>(wxString::Format(_("A check tag has unknown for attribute '%s'"), what.c_str()), child);
            }
        } else if (child(RAWXML_PATCH)) {
            BAKE_SKIP(child);
            bool filenamevar;
            wxFileName filename = ParseString(child, wxT(RAWXML_PATCH), wxT("file"), &filenamevar);
            wxString what = ParseString(child, wxT(RAWXML_PATCH), wxT("what"), NULL);
            unsigned long failmode = 0;
            OPTION_PARSE(unsigned long, failmode, ParseUnsigned(child, wxT(RAWXML_PATCH), wxT("failmode")));
            if (!filename.IsAbsolute())
                filename.MakeAbsolute(m_outputbasedir.GetPathWithSep());
            wxLogVerbose(_("Patch ") + filename.GetFullPath());
            if (!filename.FileExists()) {
                if ((failmode == FAILMODE_FAIL_FAIL) || (m_mode == MODE_INSTALL))
                    throw MakeNodeException<RCT3Exception>(wxString::Format(_("File '%s' to patch does not exist."), filename.GetFullPath().c_str()), child);
                wxLogWarning(wxString::Format(_("File '%s' to patch does not exist."), filename.GetFullPath().c_str()));
                child.go_next();
                continue;
            } else if (!filename.IsFileWritable()) {
                if ((failmode == FAILMODE_FAIL_FAIL) || (m_mode == MODE_INSTALL))
                    throw MakeNodeException<RCT3Exception>(wxString::Format(_("File '%s' to patch cannot be written to."), filename.GetFullPath().c_str()), child);
                wxLogWarning(wxString::Format(_("File '%s' to patch cannot be written to."), filename.GetFullPath().c_str()));
                child.go_next();
                continue;
            }
            if (what == wxT("addreference")) {
                wxString ref = ParseString(child, wxT(RAWXML_PATCH), wxT("parameter"), NULL);
                m_modifiedfiles.push_back(filename);
                wxString t = filename.GetFullPath();
                t.Replace(wxT(".common."), wxT(".unique."));
                m_modifiedfiles.push_back(t);
                if (!m_dryrun) {
                    cOVLDump od;
                    od.Load(filename.GetFullPath().mb_str(wxConvFile));
                    od.InjectReference(OVLT_UNIQUE, ref.mb_str(wxConvFile));
                    od.Save();
                }
            } else {
                throw MakeNodeException<RCT3Exception>(wxString::Format(_("A patch tag has unknown what attribute '%s'"), what.c_str()), child);
            }
        } else if (child(RAWXML_COPY)) {
            BAKE_SKIP(child);
            bool filenamevar;
            wxFileName filename = ParseString(child, wxT(RAWXML_COPY), wxT("from"), &filenamevar);
            wxFileName targetname = ParseString(child, wxT(RAWXML_COPY), wxT("to"), NULL);
            unsigned long failmode = 0;
            OPTION_PARSE(unsigned long, failmode, ParseUnsigned(child, wxT(RAWXML_PATCH), wxT("failmode")));
            unsigned long filebase = FILEBASE_INSTALLDIR;
            OPTION_PARSE(unsigned long, filebase, ParseUnsigned(child, wxT(RAWXML_CHECK), wxT("filebase")));
            if (!filename.IsAbsolute()) {
                if (filebase == FILEBASE_INSTALLDIR)
                    filename.MakeAbsolute(m_outputbasedir.GetPathWithSep());
                else if (filebase == FILEBASE_USERDIR)
                    filename.MakeAbsolute(m_userdir.GetPathWithSep());
                else
                    throw MakeNodeException<RCT3Exception>(wxString::Format(_("A copy tag has unknown filebase attribute '%ld'"),filebase), child);
            }
            if (!targetname.IsAbsolute()) {
                if (filebase == FILEBASE_INSTALLDIR)
                    targetname.MakeAbsolute(m_outputbasedir.GetPathWithSep());
                else if (filebase == FILEBASE_USERDIR)
                    targetname.MakeAbsolute(m_userdir.GetPathWithSep());
                else
                    throw MakeNodeException<RCT3Exception>(wxString::Format(_("A copy tag has unknown filebase attribute '%ld'"),filebase), child);
            }
            wxLogVerbose(_("Copy ") + filename.GetFullPath() + _(" to ") + targetname.GetFullPath());
            if (!filename.FileExists()) {
                if ((failmode == FAILMODE_FAIL_FAIL) || (m_mode == MODE_INSTALL))
                    throw MakeNodeException<RCT3Exception>(wxString::Format(_("File '%s' to copy does not exist."), filename.GetFullPath().c_str()), child);
                wxLogWarning(wxString::Format(_("File '%s' to copy does not exist."), filename.GetFullPath().c_str()));
                child.go_next();
                continue;
            }
            if (!CanBeWrittenTo(targetname)) {
                if ((failmode == FAILMODE_FAIL_FAIL) || (m_mode == MODE_INSTALL))
                    throw MakeNodeException<RCT3Exception>(wxString::Format(_("File '%s' to copy to cannot be written."), targetname.GetFullPath().c_str()), child);
                wxLogWarning(wxString::Format(_("File '%s' to copy to cannot be written."), targetname.GetFullPath().c_str()));
                child.go_next();
                continue;
            }
            if (targetname.FileExists())
                m_modifiedfiles.push_back(targetname);
            else
                m_newfiles.push_back(targetname);
            if (!m_dryrun) {
                if (!EnsureDir(targetname))
                    throw MakeNodeException<RCT3Exception>(wxString::Format(_("Failed to dreate directory for file '%s' to copy to."), targetname.GetFullPath().c_str()), child);
                ::wxCopyFile(filename.GetFullPath(), targetname.GetFullPath());
            }
        } else if (child(RAWXML_WRITE)) {
            wxFileName targetname = ParseString(child, wxT(RAWXML_WRITE), wxT("to"), NULL);
            wxString type = ParseString(child, wxT(RAWXML_WRITE), wxT("type"), NULL);
            unsigned long filebase = FILEBASE_INSTALLDIR;
            OPTION_PARSE(unsigned long, filebase, ParseUnsigned(child, wxT(RAWXML_WRITE), wxT("filebase")));
            if (!targetname.IsAbsolute()) {
                if (filebase == FILEBASE_INSTALLDIR)
                    targetname.MakeAbsolute(m_outputbasedir.GetPathWithSep());
                else if (filebase == FILEBASE_USERDIR)
                    targetname.MakeAbsolute(m_userdir.GetPathWithSep());
                else
                    throw MakeNodeException<RCT3Exception>(wxString::Format(_("A copy tag has unknown filebase attribute '%ld'"),filebase), child);
            }
            if ((m_mode != MODE_BAKE) && (!CanBeWrittenTo(targetname))) {
                if (m_mode == MODE_INSTALL)
                    throw MakeNodeException<RCT3Exception>(wxString::Format(_("File '%s' cannot be written."), targetname.GetFullPath().c_str()), child);
                wxLogWarning(wxString::Format(_("File '%s' cannot be written."), targetname.GetFullPath().c_str()));
                child.go_next();
                continue;
            }
            if (targetname.FileExists())
                m_modifiedfiles.push_back(targetname);
            else
                m_newfiles.push_back(targetname);
            if (m_dryrun)  {
                child.go_next();
                continue;
            }
            wxLogVerbose(wxString::Format(_("Writing file %s."), targetname.GetFullPath().c_str()));
            if (type == wxT("file")) {
                wxString temp = UTF8STRINGWRAP(child.content());
                bool filevar = MakeVariable(temp);
                wxFSFileName wfile = temp;

                if (!wfile.IsAbsolute())
                    wfile.MakeAbsolute(m_input.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME));

                wxFileSystem fs;
                auto_ptr<wxFSFile> fsfile(fs.OpenFile(wfile.GetFullPath()));

                if (!fsfile.get())
                    throw MakeNodeException<RCT3Exception>(_("write tag: File not readable: ") + wfile.GetFullPath(), child);

                wxInputStream* filestream = fsfile->GetStream(); // Stream is destroyed by wxFSFile
                filestream->SeekI(0, wxFromEnd);
                int length = filestream->TellI();
                filestream->SeekI(0);
                counted_array_ptr<unsigned char> tempch(new unsigned char[length]);
                std::auto_ptr<wxMemoryOutputStream> buffer(new wxMemoryOutputStream(tempch.get(), length));
                buffer->Write(*filestream);

                if (m_mode == MODE_BAKE) {
                    if (m_bake.Index(wxT(RAWXML_WRITE)) == wxNOT_FOUND) {
                        if (!filevar) {
                            if (m_bake.Index(wxT(RAWBAKE_ABSOLUTE)) == wxNOT_FOUND) {
                                wfile.MakeRelativeTo(m_bakeroot.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME));
                            }
                            if (child.children())
                                child.children().detach();
                            child.content(wfile.GetFullPath().mb_str(wxConvUTF8));
                        }
                        child.go_next();
                        continue;
                    }

                    counted_array_ptr<unsigned char> buf(new unsigned char[length * 4]);
                    unsigned long outlen = length * 4;
                    int bret = base64_encode(tempch.get(), length, buf.get(), &outlen);
                    switch (bret) {
                        case CRYPT_OK:
                            break;
                        case CRYPT_BUFFER_OVERFLOW:
                            throw MakeNodeException<RCT3Exception>(wxString(_("Buffer overflow baking file '")) + wfile.GetFullPath() + _("'."), child);
                        default:
                            throw MakeNodeException<RCT3Exception>(wxString(_("Unknown base64 error baking file '")) + wfile.GetFullPath() + _("'."), child);
                    }
                    child.delProp("type");
                    child.addProp("type", "data");
                    if (child.children())
                        child.children().detach();
                    child.content(reinterpret_cast<char*>(buf.get()));
                } else {
                    EnsureDir(targetname);

                    wxFile f(targetname.GetFullPath(), wxFile::write);
                    f.Write(tempch.get(), length);
                }
            } else if (type == wxT("data")) {
                if (m_mode == MODE_BAKE) {
                    child.go_next();
                    continue;
                }
                wxString data = UTF8STRINGWRAP(child.content());
                MakeVariable(data);

                unsigned long datasize = data.Length();
                unsigned long outlen = datasize;
                counted_array_ptr<unsigned char> pdata(new unsigned char [datasize]);

#ifdef UNICODE
                int bret = base64_decode(reinterpret_cast<const unsigned char*>(data.ToAscii().data()), data.Length(), pdata.get(), &outlen);
#else
                int bret = base64_decode(reinterpret_cast<const unsigned char*>(data.ToAscii()), data.Length(), pdata.get(), &outlen);
#endif
                switch (bret) {
                    case CRYPT_OK:
                        break;
                    case CRYPT_INVALID_PACKET:
                        throw MakeNodeException<RCT3Exception>(wxString(_("Decoding error in write tag ")) + targetname.GetFullPath(), child);
                    case CRYPT_BUFFER_OVERFLOW:
                        throw MakeNodeException<RCT3Exception>(wxString(_("Buffer overflow decoding write tag ")) + targetname.GetFullPath(), child);
                    default:
                        throw MakeNodeException<RCT3Exception>(wxString(_("Unknown base64 error decoding write tag ")) + targetname.GetFullPath(), child);
                }

                EnsureDir(targetname);
                wxFile f(targetname.GetFullPath(), wxFile::write);
                f.Write(pdata.get(), outlen);
            } else {
                throw MakeNodeException<RCT3Exception>(wxString::Format(_("Unknown type '%s' in write tag '%s'"), type.c_str(), targetname.GetFullPath().c_str()), child);
            }
        } else if (m_dryrun) {
            // The rest creates the file
        } else if (child(RAWXML_IMPORT)) {
            USE_PREFIX(child);
            // <import file="scenery file" (name="internal svd name") />
            bool filenamevar;
            wxFSFileName filename = ParseString(child, wxT(RAWXML_IMPORT), wxT("file"), &filenamevar);
            wxString name;
            ParseStringOption(name, child, wxT("name"), NULL);
            wxString use;
            ParseStringOption(use, child, wxT("use"), NULL);
            if (!filename.IsAbsolute()) {
                filename.MakeAbsolute(m_input.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME));
            }

            if (m_mode == MODE_BAKE) {
                if (m_bake.Index(wxT(RAWXML_IMPORT)) == wxNOT_FOUND) {
                    if (!filenamevar) {
                        if (m_bake.Index(wxT(RAWBAKE_ABSOLUTE)) == wxNOT_FOUND) {
                            filename.MakeRelativeTo(m_bakeroot.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME));
                        }
                        child.prop("file", filename.GetFullPath());
                    }
                    child.go_next();
                    continue;
                }
            }

            wxLogVerbose(wxString::Format(_("Importing from %s..."), filename.GetFullPath().c_str()));
            cSCNFile c_scn(filename.GetFullPath());
            if (!name.IsEmpty()) {
                c_scn.name = name;
            }
            c_scn.prefix = "";
            if (useprefix && (m_prefix != "")) {
                c_scn.prefix = wxString(m_prefix.c_str(), wxConvLocal);
            }

            if (m_mode == MODE_BAKE) {
                c_scn.Check();
                child.name(RAWXML_SECTION);
                child.delProp("name");
                child.delProp("file");
                if (child.children())
                    child.children().detach();
// TODO (belgabor#1#): bake use attribute
                BakeScenery(child, *c_scn.m_work);
                Parse(child); // Bake contained stuff
            } else {
                wxLogVerbose(wxString::Format(_("Importing %s (%s) to %s."), filename.GetFullPath().c_str(), c_scn.name.c_str(), m_output.GetFullPath().c_str()));
                if (use.IsEmpty()) {
                    c_scn.MakeToOvl(m_ovl);
                } else if (use == "main") {
                    c_scn.MakeToOvlMain(m_ovl);
                } else if (use == "animations") {
                    c_scn.MakeToOvlAnimations(m_ovl);
                } else if (use == "splines") {
                    c_scn.MakeToOvlSplines(m_ovl);
                } else  {
                    throw MakeNodeException<RCT3Exception>(wxString::Format(_("Unknown value '%s' for use attribute in import tag"), use.c_str()), child);
                }
            }
        } else if (child(RAWXML_ANR)) {
            BAKE_SKIP(child);
            ParseANR(child);
        } else if (child(RAWXML_BAN)) {
            BAKE_SKIP(child);
            ParseBAN(child);
        } else if (child(RAWXML_BSH)) {
            ParseBSH(child);
        } else if (child(RAWXML_BTBL)) {
            BAKE_SKIP(child);
            m_ovl.GetManager<ovlBTBLManager>();
        } else if (child(RAWXML_CED)) {
            BAKE_SKIP(child);
            ParseCED(child);
        } else if (child(RAWXML_CHG)) {
            BAKE_SKIP(child);
            ParseCHG(child);
        } else if (child(RAWXML_CID)) {
            BAKE_SKIP(child);
            ParseCID(child);
        } else if (child(RAWXML_FTX)) {
            ParseFTX(child);
        } else if (child(RAWXML_GSI)) {
            // <gsi name="string" tex="string" top="long int" left="long int" bottom="long int" right="long int" />
            USE_PREFIX(child);
            BAKE_SKIP(child);
            wxString name = ParseString(child, wxT(RAWXML_GSI), wxT("name"), NULL, useprefix);
            wxString tex = ParseString(child, wxT(RAWXML_GSI), wxT("tex"), NULL, useprefix);
            unsigned long top = ParseUnsigned(child, wxT(RAWXML_GSI), wxT("top"));
            unsigned long left = ParseUnsigned(child, wxT(RAWXML_GSI), wxT("left"));
            unsigned long bottom = ParseUnsigned(child, wxT(RAWXML_GSI), wxT("bottom"));
            unsigned long right = ParseUnsigned(child, wxT(RAWXML_GSI), wxT("right"));
            wxLogVerbose(wxString::Format(_("Adding gsi %s to %s."), name.c_str(), m_output.GetFullPath().c_str()));

            ovlGSIManager* c_gsi = m_ovl.GetManager<ovlGSIManager>();
            c_gsi->AddItem(name.ToAscii(), tex.ToAscii(), left, top, right, bottom);
        } else if (child(RAWXML_PTD)) {
            BAKE_SKIP(child);
            ParsePTD(child);
        } else if (child(RAWXML_QTD)) {
            BAKE_SKIP(child);
            ParseQTD(child);
        } else if (child(RAWXML_SAT)) {
            BAKE_SKIP(child);
            ParseSAT(child);
        } else if (child(RAWXML_SHS)) {
            ParseSHS(child);
        } else if (child(RAWXML_SID)) {
            // <sid name="string" ovlpath="ovl path, relative to install dir" nametxt="txt" icon="gsi" svd="svd">
            //     (All following tags and attributes are optional, if not mentioned otherwise)
            //     <sidgroup name="txt" icon="gsi" /> (both name and gsi must be present)
            //     <sidtype scenerytype="long int" cost="long int" removalcost="long int" />
            //       (costs are signed integers)
            //     <sidposition type="short int" xsquares="long int" ysquares="long int"
            //               xpos="float" ypos="float" zpos="float"
            //               xsize="float" ysize="float" zsize="float"
            //               supports="string" />
            //     <sidcolours choice1="long int" choice2="long int" choice3="long int" />
            //     <sidflags set="1010101010101010101010101010101010101010101010101010101010101010" />
            //       (flags are all or nothing, 64 0's and 1's. 1 set, 0 not set. 1-64 from left to right)
            //     <sidimporterunknowns u1="long int" u2="long int" u6="long int" u7="long int" u8="long int" u9="long int" />
            //       (if you leave out u8, it will be disabled)
            //     <sidstallunknowns u1="long int" u2="long int" />
            //       (if you leave out both attributes, the stall defaults will be used (1440/2880))
            //     (The next tags can be used multiple times)
            //     <sidvisual name="svd" />
            //       (add another svd to the sid. As far as I know only used for trees (2d/3d versions).
            //     <sidparameter name="string" param="string" />
            //       (add a parameter. name is required, param is optional in most cases. If present, it usually starts with a space.)
            // </sid>
            BAKE_SKIP(child);
            ParseSID(child);
        } else if (child(RAWXML_SND)) {
            ParseSND(child);
        } else if (child(RAWXML_SPL)) {
            BAKE_SKIP(child);
            ParseSPL(child);
        } else if (child(RAWXML_STA)) {
            // <sta name="string" nametxt="txt" description="txt" sid="sid">
            //     <stais type="long int" icon="gsi" spline="spl" />
            //     <staitem cid="cid" cost="long int" />
            //     <stastallunknowns u1="long int" u2="long int" u3="long int" u4="long int" u5="long int" u6="long int" />
            // </sta>
            BAKE_SKIP(child);
            ParseSTA(child);
        } else if (child(RAWXML_SVD)) {
            BAKE_SKIP(child);
            ParseSVD(child);
        } else if (child(RAWXML_TEX)) {
            // <tex name="string" texture="texture file" />
//            if ((m_mode == MODE_BAKE) && (m_bake.Index(RAWXML_TEX) == wxNOT_FOUND)) {
//                child = child->GetNext();
//                continue;
//            }
            ParseTEX(child);
        } else if (child(RAWXML_TKS)) {
            BAKE_SKIP(child);
            ParseTKS(child);
        } else if (child(RAWXML_TRR)) {
            BAKE_SKIP(child);
            ParseTRR(child);
        } else if (child(RAWXML_TXT)) {
            // <txt name="string" text="string" />
            USE_PREFIX(child);
            wxString name = ParseString(child, wxT(RAWXML_TXT), wxT("name"), NULL, useprefix);
            wxString type = ParseString(child, wxT(RAWXML_TXT), wxT("type"), NULL);
            wxLogVerbose(wxString::Format(_("Adding txt %s to %s."), name.c_str(), m_output.GetFullPath().c_str()));
            if (type == wxT("text")) {
                if ((m_mode == MODE_BAKE) && (m_bake.Index(wxT(RAWXML_TXT)) == wxNOT_FOUND)) {
                    child.go_next();
                    continue;
                }
                wxString text = UTF8STRINGWRAP(child.content());
                MakeVariable(text);

                if (m_mode == MODE_BAKE) {
                    int wlength = text.Length()+1;
                    counted_array_ptr<wchar_t> tempch(new wchar_t[wlength]);
#ifdef UNICODE
                    wcscpy(tempch.get(), text.wc_str(wxCSConv(wxFONTENCODING_UTF16LE)));
#else
                    mbstowcs(tempch.get(), text.c_str(), wlength);
#endif
                    counted_array_ptr<unsigned char> buf = counted_array_ptr<unsigned char>(new unsigned char[wlength * 2 * 4]);
                    unsigned long outlen = wlength * 2 * 4;
                    int bret = base64_encode(reinterpret_cast<unsigned char*>(tempch.get()), wlength * 2, buf.get(), &outlen);
                    switch (bret) {
                        case CRYPT_OK:
                            break;
                        case CRYPT_BUFFER_OVERFLOW:
                            throw MakeNodeException<RCT3Exception>(wxString(_("Buffer overflow baking text '")) + text + _("' in txt tag ") + name, child);
                        default:
                            throw MakeNodeException<RCT3Exception>(wxString(_("Unknown base64 error baking text '")) + text + _("' in txt tag ") + name, child);
                    }
                    child.prop("type","raw");
                    child.content(reinterpret_cast<char*>(buf.get()));
                } else {
                    ovlTXTManager* c_txt = m_ovl.GetManager<ovlTXTManager>();
#ifdef UNICODE
                    c_txt->AddText(name.ToAscii(), text.wc_str(wxCSConv(wxFONTENCODING_UTF16LE)));
#else
                    c_txt->AddText(name.ToAscii(), text.c_str());
#endif
                }
            } else if (type == wxT("raw")) {
                if (m_mode == MODE_BAKE) {
                    child.go_next();
                    continue;
                }
                wxString text = UTF8STRINGWRAP(child.content());
                MakeVariable(text);

                unsigned long datasize = text.Length();
                unsigned long outlen = datasize;
                counted_array_ptr<wchar_t> data = counted_array_ptr<wchar_t>(new wchar_t[datasize/2]);

#ifdef UNICODE
                int bret = base64_decode(reinterpret_cast<const unsigned char*>(text.ToAscii().data()), text.Length(), reinterpret_cast<unsigned char*>(data.get()), &outlen);
#else
                int bret = base64_decode(reinterpret_cast<const unsigned char*>(text.ToAscii()), text.Length(), reinterpret_cast<unsigned char*>(data.get()), &outlen);
#endif
                switch (bret) {
                    case CRYPT_OK:
                        break;
                    case CRYPT_INVALID_PACKET:
                        throw MakeNodeException<RCT3Exception>(wxString(_("Decoding error in txt tag ")) + name, child);
                    case CRYPT_BUFFER_OVERFLOW:
                        throw MakeNodeException<RCT3Exception>(wxString(_("Buffer overflow decoding txt tag ")) + name, child);
                    default:
                        throw MakeNodeException<RCT3Exception>(wxString(_("Unknown base64 error decoding txt tag ")) + name, child);
                }
                ovlTXTManager* c_txt = m_ovl.GetManager<ovlTXTManager>();
                c_txt->AddText(name.ToAscii(), data.get());

            } else {
                throw MakeNodeException<RCT3Exception>(wxString::Format(_("Unknown type '%s' in txt tag '%s'."), type.c_str(), name.c_str()), child);
            }

        } else if (child(RAWXML_WAI)) {
            BAKE_SKIP(child);
            ParseWAI(child);
        } else if (child(RAWXML_REFERENCE)) {
            // <reference path="relative path" />
            BAKE_SKIP(child);
            wxString ref = wxT("");
            if (child.hasProp("path")) {
                ref = UTF8STRINGWRAP(child.getPropVal("path"));
            } else {
                ref = UTF8STRINGWRAP(child.content());
                MakeVariable(ref);
            }
            if (ref.IsEmpty())
                throw MakeNodeException<RCT3Exception>(_("REFERENCE tag misses path attribute or content."), child);
            wxLogVerbose(wxString::Format(_("Adding reference %s to %s."), ref.c_str(), m_output.GetFullPath().c_str()));
            m_ovl.AddReference(ref.mb_str(wxConvFile));
        } else if (child(RAWXML_SYMBOL)) {
            // <symbol target="common|unique" name="string" type="int|float" data="data" />
            BAKE_SKIP(child);
            cOvlType target = ParseType(child, wxT(RAWXML_SYMBOL));
            wxString name = ParseString(child, wxT(RAWXML_SYMBOL), wxT("name"), NULL);
            wxString type = ParseString(child, wxT(RAWXML_SYMBOL), wxT("type"), NULL);
            wxLogVerbose(wxString::Format(_("Adding symbol %s to %s."), name.c_str(), m_output.GetFullPath().c_str()));
            unsigned long data;
            if (type == wxT("int")) {
                name += wxT(":int");
                data = ParseUnsigned(child, wxT(RAWXML_SYMBOL), wxT("data"));
            } else if (type == wxT("float")) {
                name += wxT(":flt");
                float f = ParseFloat(child, wxT(RAWXML_SYMBOL), wxT("data"));
                data = *reinterpret_cast<unsigned long*>(&f);
            } else {
                throw MakeNodeException<RCT3Exception>(wxString::Format(_("symbol tag has unimplemented type value '%s'."), type.c_str()), child);
            }
            m_ovl.AddDataSymbol(target, name.ToAscii(), data);
        } else if (child.is(XML_ELEMENT_NODE)) {
            throw MakeNodeException<RCT3Exception>(wxString::Format(_("Unknown tag '%s' in rawovl tag."), STRING_FOR_FORMAT(child.name())), child);
        }

        child.go_next();
    }

    m_prefix = oldprefix;
}
