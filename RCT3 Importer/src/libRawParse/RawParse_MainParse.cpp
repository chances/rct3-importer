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

void cRawParser::Parse(wxXmlNode* node) {
    std::string oldprefix = m_prefix;
    ParseStringOption(m_prefix, node, wxT("prefix"), NULL, false);

    wxXmlNode *child = node->GetChildren();
    while (child) {
        DO_CONDITION_COMMENT(child);

        if (child->GetName() == RAWXML_SECTION) {
            wxString incfile = child->GetPropVal(wxT("include"), wxT(""));
            if (incfile.IsEmpty()) {
                Parse(child);
            } else {
                if (child->GetChildren())
                    throw RCT3Exception(wxT("A section tag with include attribute may not have content"));

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
                    if (m_bake.Index(RAWBAKE_XML) == wxNOT_FOUND) {
                        wxString newfile;
                        if (m_bake.Index(RAWBAKE_ABSOLUTE) == wxNOT_FOUND) {
                            filename.MakeRelativeTo(m_bakeroot.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME));
                        }
                        newfile = filename.GetFullPath();
                        child->DeleteProperty(wxT("include"));
                        child->AddProperty(wxT("include"), newfile);
                        child = child->GetNext();
                        continue;
                    }
                }

                wxXmlDocument doc;
                wxFileSystem fs;
                std::auto_ptr<wxFSFile> inputfsfile(fs.OpenFile(filename.GetFullPath(), wxFS_READ | wxFS_SEEKABLE));
                if (!inputfsfile.get())
                    throw RCT3Exception(_("Cannot read input file ")+filename.GetFullPath());

                doc.Load(*inputfsfile->GetStream());
                wxXmlNode* root = doc.GetRoot();

                if (root->GetName() != RAWXML_SECTION)
                    throw RCT3Exception(wxString::Format(_("Included section raw xml file has wrong root '%s'."), root->GetName().c_str()));

                wxFSFileName bakebackup = m_bakeroot;
                wxFSFileName inputbackup = m_input;
                m_input = filename;
                m_bakeroot = filenamebake;
                Parse(root);
                m_input = inputbackup;
                m_bakeroot = bakebackup;

                if (m_mode == MODE_BAKE) {
                    doc.DetachRoot();
                    root->SetName(RAWXML_SECTION);
                    child->SetChildren(root);
                    root->SetParent(child);
                    child->DeleteProperty(wxT("include"));
                }
            }
        } else if (child->GetName() == RAWXML_SUBROOT) {
            // see above or
            // <subovl include="raw xml file" />
            wxString incfile = child->GetPropVal(wxT("include"), wxT(""));

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
                if (child->GetChildren())
                    throw RCT3Exception(wxT("A subovl tag with include attribute may not have content"));

                wxFSFileName filename = incfile;
                if (!filename.IsAbsolute())
                    filename.MakeAbsolute(m_input.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME));

                if (m_mode == MODE_BAKE) {
                    if (m_bake.Index(RAWBAKE_XML) == wxNOT_FOUND) {
                        wxString newfile;
                        if (m_bake.Index(RAWBAKE_ABSOLUTE) == wxNOT_FOUND) {
                            filename.MakeRelativeTo(m_bakeroot.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME));
                        }
                        newfile = filename.GetFullPath();
                        child->DeleteProperty(wxT("include"));
                        child->AddProperty(wxT("include"), newfile);
                        child = child->GetNext();
                        continue;
                    }
                }

                wxXmlDocument doc;
                wxFileSystem fs;
                std::auto_ptr<wxFSFile> inputfsfile(fs.OpenFile(filename.GetFullPath(), wxFS_READ | wxFS_SEEKABLE));
                if (!inputfsfile.get())
                    throw RCT3Exception(_("Cannot read input file ")+filename.GetFullPath());

                doc.Load(*inputfsfile->GetStream());
                wxXmlNode* root = doc.GetRoot();
                c_raw.Process(root, filename, m_outputbasedir);
                if (m_mode == MODE_BAKE) {
                    doc.DetachRoot();
                    root->SetName(RAWXML_SUBROOT);
                    child->SetChildren(root);
                    root->SetParent(child);
                    child->DeleteProperty(wxT("include"));
                }
            }
            for (std::vector<wxFileName>::const_iterator it = c_raw.GetModifiedFiles().begin(); it != c_raw.GetModifiedFiles().end(); ++it) {
                m_modifiedfiles.push_back(*it);
            }
            for (std::vector<wxFileName>::const_iterator it = c_raw.GetNewFiles().begin(); it != c_raw.GetNewFiles().end(); ++it) {
                m_newfiles.push_back(*it);
            }
        } else if (child->GetName() == RAWXML_DATAREF) {
            wxString guid = ParseString(child, RAWXML_DATAREF, wxT("guid"), NULL);
            guid.MakeUpper();
            m_datareferences[guid] = child;
        } else if (child->GetName() == RAWXML_SET) {
            ParseSet(child);
        } else if (child->GetName() == RAWXML_UNSET) {
            ParseUnset(child);
        } else if (child->GetName() == RAWXML_VARIABLES) {
            ParseVariables(child);
        } else if (child->GetName() == RAWXML_CHECK) {
            BAKE_SKIP(child);
            bool filenamevar;
            wxFileName filename = ParseString(child, RAWXML_CHECK, wxT("file"), &filenamevar);
            wxString what = ParseString(child, RAWXML_CHECK, wxT("for"), NULL);
            unsigned long filebase = FILEBASE_INSTALLDIR;
            OPTION_PARSE(unsigned long, filebase, ParseUnsigned(child, RAWXML_CHECK, wxT("filebase")));
            if (!filename.IsAbsolute()) {
                if (filebase == FILEBASE_INSTALLDIR)
                    filename.MakeAbsolute(m_outputbasedir.GetPathWithSep());
                else if (filebase == FILEBASE_USERDIR)
                    filename.MakeAbsolute(m_userdir.GetPathWithSep());
                else
                    throw RCT3Exception(wxString::Format(_("A check tag has unknown filebase attribute '%ld'"),filebase));
            }
            if (!filename.FileExists()) {
                child = child->GetNext();
                continue;
            }
            if (what == wxT("presence")) {
                ParseSet(child, false, RAWXML_CHECK);
            } else if (what == wxT("reference")) {
                wxString ref = ParseString(child, RAWXML_CHECK, wxT("parameter"), NULL);
                cOVLDump od;
                od.Load(filename.GetFullPath().mb_str(wxConvFile));
                for (std::vector<std::string>::const_iterator it = od.GetReferences(OVLT_UNIQUE).begin(); it != od.GetReferences(OVLT_UNIQUE).end(); ++it) {
                    wxString check(it->c_str(), wxConvFile);
                    if (check == ref) {
                        ParseSet(child, false, RAWXML_CHECK);
                        break;
                    }
                }
            } else {
                throw RCT3Exception(wxString::Format(_("A check tag has unknown for attribute '%s'"), what.c_str()));
            }
        } else if (child->GetName() == RAWXML_PATCH) {
            BAKE_SKIP(child);
            bool filenamevar;
            wxFileName filename = ParseString(child, RAWXML_PATCH, wxT("file"), &filenamevar);
            wxString what = ParseString(child, RAWXML_PATCH, wxT("what"), NULL);
            unsigned long failmode = 0;
            OPTION_PARSE(unsigned long, failmode, ParseUnsigned(child, RAWXML_PATCH, wxT("failmode")));
            if (!filename.IsAbsolute())
                filename.MakeAbsolute(m_outputbasedir.GetPathWithSep());
            wxLogVerbose(_("Patch ") + filename.GetFullPath());
            if (!filename.FileExists()) {
                if ((failmode == FAILMODE_FAIL_FAIL) || (m_mode == MODE_INSTALL))
                    throw RCT3Exception(wxString::Format(_("File '%s' to patch does not exist."), filename.GetFullPath().c_str()));
                wxLogWarning(wxString::Format(_("File '%s' to patch does not exist."), filename.GetFullPath().c_str()));
                child = child->GetNext();
                continue;
            } else if (!filename.IsFileWritable()) {
                if ((failmode == FAILMODE_FAIL_FAIL) || (m_mode == MODE_INSTALL))
                    throw RCT3Exception(wxString::Format(_("File '%s' to patch cannot be written to."), filename.GetFullPath().c_str()));
                wxLogWarning(wxString::Format(_("File '%s' to patch cannot be written to."), filename.GetFullPath().c_str()));
                child = child->GetNext();
                continue;
            }
            if (what == wxT("addreference")) {
                wxString ref = ParseString(child, RAWXML_CHECK, wxT("parameter"), NULL);
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
                throw RCT3Exception(wxString::Format(_("A patch tag has unknown what attribute '%s'"), what.c_str()));
            }
        } else if (child->GetName() == RAWXML_COPY) {
            BAKE_SKIP(child);
            bool filenamevar;
            wxFileName filename = ParseString(child, RAWXML_COPY, wxT("from"), &filenamevar);
            wxFileName targetname = ParseString(child, RAWXML_COPY, wxT("to"), NULL);
            unsigned long failmode = 0;
            OPTION_PARSE(unsigned long, failmode, ParseUnsigned(child, RAWXML_PATCH, wxT("failmode")));
            unsigned long filebase = FILEBASE_INSTALLDIR;
            OPTION_PARSE(unsigned long, filebase, ParseUnsigned(child, RAWXML_CHECK, wxT("filebase")));
            if (!filename.IsAbsolute()) {
                if (filebase == FILEBASE_INSTALLDIR)
                    filename.MakeAbsolute(m_outputbasedir.GetPathWithSep());
                else if (filebase == FILEBASE_USERDIR)
                    filename.MakeAbsolute(m_userdir.GetPathWithSep());
                else
                    throw RCT3Exception(wxString::Format(_("A copy tag has unknown filebase attribute '%ld'"),filebase));
            }
            if (!targetname.IsAbsolute()) {
                if (filebase == FILEBASE_INSTALLDIR)
                    targetname.MakeAbsolute(m_outputbasedir.GetPathWithSep());
                else if (filebase == FILEBASE_USERDIR)
                    targetname.MakeAbsolute(m_userdir.GetPathWithSep());
                else
                    throw RCT3Exception(wxString::Format(_("A copy tag has unknown filebase attribute '%ld'"),filebase));
            }
            wxLogVerbose(_("Copy ") + filename.GetFullPath() + _(" to ") + targetname.GetFullPath());
            if (!filename.FileExists()) {
                if ((failmode == FAILMODE_FAIL_FAIL) || (m_mode == MODE_INSTALL))
                    throw RCT3Exception(wxString::Format(_("File '%s' to copy does not exist."), filename.GetFullPath().c_str()));
                wxLogWarning(wxString::Format(_("File '%s' to copy does not exist."), filename.GetFullPath().c_str()));
                child = child->GetNext();
                continue;
            }
            if (!CanBeWrittenTo(targetname)) {
                if ((failmode == FAILMODE_FAIL_FAIL) || (m_mode == MODE_INSTALL))
                    throw RCT3Exception(wxString::Format(_("File '%s' to copy to cannot be written."), targetname.GetFullPath().c_str()));
                wxLogWarning(wxString::Format(_("File '%s' to copy to cannot be written."), targetname.GetFullPath().c_str()));
                child = child->GetNext();
                continue;
            }
            if (targetname.FileExists())
                m_modifiedfiles.push_back(targetname);
            else
                m_newfiles.push_back(targetname);
            if (!m_dryrun) {
                if (!EnsureDir(targetname))
                    throw RCT3Exception(wxString::Format(_("Failed to dreate directory for file '%s' to copy to."), targetname.GetFullPath().c_str()));
                ::wxCopyFile(filename.GetFullPath(), targetname.GetFullPath());
            }
        } else if (child->GetName() == RAWXML_WRITE) {
            wxFileName targetname = ParseString(child, RAWXML_WRITE, wxT("to"), NULL);
            wxString type = ParseString(child, RAWXML_WRITE, wxT("type"), NULL);
            unsigned long filebase = FILEBASE_INSTALLDIR;
            OPTION_PARSE(unsigned long, filebase, ParseUnsigned(child, RAWXML_WRITE, wxT("filebase")));
            if (!targetname.IsAbsolute()) {
                if (filebase == FILEBASE_INSTALLDIR)
                    targetname.MakeAbsolute(m_outputbasedir.GetPathWithSep());
                else if (filebase == FILEBASE_USERDIR)
                    targetname.MakeAbsolute(m_userdir.GetPathWithSep());
                else
                    throw RCT3Exception(wxString::Format(_("A copy tag has unknown filebase attribute '%ld'"),filebase));
            }
            if ((m_mode != MODE_BAKE) && (!CanBeWrittenTo(targetname))) {
                if (m_mode == MODE_INSTALL)
                    throw RCT3Exception(wxString::Format(_("File '%s' cannot be written."), targetname.GetFullPath().c_str()));
                wxLogWarning(wxString::Format(_("File '%s' cannot be written."), targetname.GetFullPath().c_str()));
                child = child->GetNext();
                continue;
            }
            if (targetname.FileExists())
                m_modifiedfiles.push_back(targetname);
            else
                m_newfiles.push_back(targetname);
            if (m_dryrun)  {
                child = child->GetNext();
                continue;
            }
            wxLogVerbose(wxString::Format(_("Writing file %s."), targetname.GetFullPath().c_str()));
            if (type == wxT("file")) {
                wxString temp = child->GetNodeContent();
                bool filevar = MakeVariable(temp);
                wxFSFileName wfile = temp;

                if (!wfile.IsAbsolute())
                    wfile.MakeAbsolute(m_input.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME));

                wxFileSystem fs;
                auto_ptr<wxFSFile> fsfile(fs.OpenFile(wfile.GetFullPath()));

                if (!fsfile.get())
                    throw RCT3Exception(_("write tag: File not readable: ") + wfile.GetFullPath());

                wxInputStream* filestream = fsfile->GetStream(); // Stream is destroyed by wxFSFile
                filestream->SeekI(0, wxFromEnd);
                int length = filestream->TellI();
                filestream->SeekI(0);
                counted_array_ptr<unsigned char> tempch(new unsigned char[length]);
                std::auto_ptr<wxMemoryOutputStream> buffer(new wxMemoryOutputStream(tempch.get(), length));
                buffer->Write(*filestream);

                if (m_mode == MODE_BAKE) {
                    if (m_bake.Index(RAWXML_WRITE) == wxNOT_FOUND) {
                        if (!filevar) {
                            if (m_bake.Index(RAWBAKE_ABSOLUTE) == wxNOT_FOUND) {
                                wfile.MakeRelativeTo(m_bakeroot.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME));
                            }
                            delete child->GetChildren();
                            child->SetChildren(new wxXmlNode(NULL, wxXML_TEXT_NODE, wxT(""), wfile.GetFullPath()));
                        }
                        child = child->GetNext();
                        continue;
                    }

                    counted_array_ptr<unsigned char> buf(new unsigned char[length * 4]);
                    unsigned long outlen = length * 4;
                    int bret = base64_encode(tempch.get(), length, buf.get(), &outlen);
                    switch (bret) {
                        case CRYPT_OK:
                            break;
                        case CRYPT_BUFFER_OVERFLOW:
                            throw RCT3Exception(wxString(_("Buffer overflow baking file '")) + wfile.GetFullPath() + _("'."));
                        default:
                            throw RCT3Exception(wxString(_("Unknown base64 error baking file '")) + wfile.GetFullPath() + _("'."));
                    }
                    child->DeleteProperty(wxT("type"));
                    child->AddProperty(wxT("type"), wxT("data"));
                    delete child->GetChildren();
                    child->SetChildren(new wxXmlNode(NULL, wxXML_TEXT_NODE, wxT(""), wxString(reinterpret_cast<char*>(buf.get()), wxConvLocal)));
                } else {
                    EnsureDir(targetname);

                    wxFile f(targetname.GetFullPath(), wxFile::write);
                    f.Write(tempch.get(), length);
                }
            } else if (type == wxT("data")) {
                if (m_mode == MODE_BAKE) {
                    child = child->GetNext();
                    continue;
                }
                wxString data = child->GetNodeContent();
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
                        throw RCT3Exception(wxString(_("Decoding error in write tag ")) + targetname.GetFullPath());
                    case CRYPT_BUFFER_OVERFLOW:
                        throw RCT3Exception(wxString(_("Buffer overflow decoding write tag ")) + targetname.GetFullPath());
                    default:
                        throw RCT3Exception(wxString(_("Unknown base64 error decoding write tag ")) + targetname.GetFullPath());
                }

                EnsureDir(targetname);
                wxFile f(targetname.GetFullPath(), wxFile::write);
                f.Write(pdata.get(), outlen);
            } else {
                throw RCT3Exception(wxString::Format(_("Unknown type '%s' in write tag '%s'."), type.c_str(), targetname.GetFullPath().c_str()));
            }
        } else if (m_dryrun) {
            // The rest creates the file
        } else if (child->GetName() == RAWXML_IMPORT) {
            USE_PREFIX(child);
            // <import file="scenery file" (name="internal svd name") />
            bool filenamevar;
            wxFSFileName filename = ParseString(child, RAWXML_IMPORT, wxT("file"), &filenamevar);
            wxString name = child->GetPropVal(wxT("name"), wxT(""));
            if (!filename.IsAbsolute()) {
                filename.MakeAbsolute(m_input.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME));
            }

            if (m_mode == MODE_BAKE) {
                if (m_bake.Index(RAWXML_IMPORT) == wxNOT_FOUND) {
                    if (!filenamevar) {
                        if (m_bake.Index(RAWBAKE_ABSOLUTE) == wxNOT_FOUND) {
                            filename.MakeRelativeTo(m_bakeroot.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME));
                        }
                        child->DeleteProperty(wxT("file"));
                        child->AddProperty(wxT("file"), filename.GetFullPath());
                    }
                    child = child->GetNext();
                    continue;
                }
            }

            cSCNFile c_scn(filename.GetFullPath());
            if (!name.IsEmpty()) {
                if (useprefix)
                    c_scn.name = wxString(m_prefix.c_str(), wxConvLocal) + name;
                else
                    c_scn.name = name;
            }

            if (m_mode == MODE_BAKE) {
                c_scn.Check();
                child->SetName(RAWXML_SECTION);
                child->DeleteProperty(wxT("name"));
                child->DeleteProperty(wxT("file"));
                if (child->GetChildren())
                    delete child->GetChildren();
                child->SetChildren(NULL);
                BakeScenery(child, *c_scn.m_work);
                Parse(child); // Bake contained stuff
            } else {
                wxLogVerbose(wxString::Format(_("Importing %s (%s) to %s."), filename.GetFullPath().c_str(), c_scn.name.c_str(), m_output.GetFullPath().c_str()));
                c_scn.MakeToOvl(m_ovl);
            }
        } else if (child->GetName() == RAWXML_BAN) {
            BAKE_SKIP(child);
            ParseBAN(child);
        } else if (child->GetName() == RAWXML_BSH) {
            ParseBSH(child);
        } else if (child->GetName() == RAWXML_BTBL) {
            BAKE_SKIP(child);
            m_ovl.GetManager<ovlBTBLManager>();
        } else if (child->GetName() == RAWXML_CED) {
            BAKE_SKIP(child);
            ParseCED(child);
        } else if (child->GetName() == RAWXML_CHG) {
            BAKE_SKIP(child);
            ParseCHG(child);
        } else if (child->GetName() == RAWXML_CID) {
            BAKE_SKIP(child);
            ParseCID(child);
        } else if (child->GetName() == RAWXML_FTX) {
            ParseFTX(child);
        } else if (child->GetName() == RAWXML_GSI) {
            // <gsi name="string" tex="string" top="long int" left="long int" bottom="long int" right="long int" />
            USE_PREFIX(child);
            BAKE_SKIP(child);
            wxString name = ParseString(child, RAWXML_GSI, wxT("name"), NULL, useprefix);
            wxString tex = ParseString(child, RAWXML_GSI, wxT("tex"), NULL, useprefix);
            unsigned long top = ParseUnsigned(child, RAWXML_GSI, wxT("top"));
            unsigned long left = ParseUnsigned(child, RAWXML_GSI, wxT("left"));
            unsigned long bottom = ParseUnsigned(child, RAWXML_GSI, wxT("bottom"));
            unsigned long right = ParseUnsigned(child, RAWXML_GSI, wxT("right"));
            wxLogVerbose(wxString::Format(_("Adding gsi %s to %s."), name.c_str(), m_output.GetFullPath().c_str()));

            ovlGSIManager* c_gsi = m_ovl.GetManager<ovlGSIManager>();
            c_gsi->AddItem(name.ToAscii(), tex.ToAscii(), left, top, right, bottom);
        } else if (child->GetName() == RAWXML_SAT) {
            BAKE_SKIP(child);
            ParseSAT(child);
        } else if (child->GetName() == RAWXML_SID) {
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
        } else if (child->GetName() == RAWXML_SHS) {
            ParseSHS(child);
        } else if (child->GetName() == RAWXML_SPL) {
            BAKE_SKIP(child);
            ParseSPL(child);
        } else if (child->GetName() == RAWXML_STA) {
            // <sta name="string" nametxt="txt" description="txt" sid="sid">
            //     <stais type="long int" icon="gsi" spline="spl" />
            //     <staitem cid="cid" cost="long int" />
            //     <stastallunknowns u1="long int" u2="long int" u3="long int" u4="long int" u5="long int" u6="long int" />
            // </sta>
            BAKE_SKIP(child);
            ParseSTA(child);
        } else if (child->GetName() == RAWXML_SVD) {
            BAKE_SKIP(child);
            ParseSVD(child);
        } else if (child->GetName() == RAWXML_TEX) {
            // <tex name="string" texture="texture file" />
//            if ((m_mode == MODE_BAKE) && (m_bake.Index(RAWXML_TEX) == wxNOT_FOUND)) {
//                child = child->GetNext();
//                continue;
//            }
            ParseTEX(child);
        } else if (child->GetName() == RAWXML_TXT) {
            // <txt name="string" text="string" />
            USE_PREFIX(child);
            wxString name = ParseString(child, RAWXML_TXT, wxT("name"), NULL, useprefix);
            wxString type = ParseString(child, RAWXML_TXT, wxT("type"), NULL);
            wxLogVerbose(wxString::Format(_("Adding txt %s to %s."), name.c_str(), m_output.GetFullPath().c_str()));
            if (type == wxT("text")) {
                if ((m_mode == MODE_BAKE) && (m_bake.Index(RAWXML_TXT) == wxNOT_FOUND)) {
                    child = child->GetNext();
                    continue;
                }
                wxString text = child->GetNodeContent();
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
                            throw RCT3Exception(wxString(_("Buffer overflow baking text '")) + text + _("' in txt tag ") + name);
                        default:
                            throw RCT3Exception(wxString(_("Unknown base64 error baking text '")) + text + _("' in txt tag ") + name);
                    }
                    child->DeleteProperty(wxT("type"));
                    child->AddProperty(wxT("type"), wxT("raw"));
                    delete child->GetChildren();
                    child->SetChildren(new wxXmlNode(NULL, wxXML_TEXT_NODE, wxT(""), wxString(reinterpret_cast<char*>(buf.get()), wxConvLocal)));
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
                    child = child->GetNext();
                    continue;
                }
                wxString text = child->GetNodeContent();
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
                        throw RCT3Exception(wxString(_("Decoding error in txt tag ")) + name);
                    case CRYPT_BUFFER_OVERFLOW:
                        throw RCT3Exception(wxString(_("Buffer overflow decoding txt tag ")) + name);
                    default:
                        throw RCT3Exception(wxString(_("Unknown base64 error decoding txt tag ")) + name);
                }
                ovlTXTManager* c_txt = m_ovl.GetManager<ovlTXTManager>();
                c_txt->AddText(name.ToAscii(), data.get());

            } else {
                throw RCT3Exception(wxString::Format(_("Unknown type '%s' in txt tag '%s'."), type.c_str(), name.c_str()));
            }

        } else if (child->GetName() == RAWXML_WAI) {
            BAKE_SKIP(child);
            ParseWAI(child);
        } else if (child->GetName() == RAWXML_REFERENCE) {
            // <reference path="relative path" />
            BAKE_SKIP(child);
            wxString ref = wxT("");
            if (child->HasProp(wxT("path"))) {
                ref = child->GetPropVal(wxT("path"), wxT(""));
            } else {
                ref = child->GetNodeContent();
                MakeVariable(ref);
            }
            if (ref.IsEmpty())
                throw RCT3Exception(_("REFERENCE tag misses path attribute or content."));
            wxLogVerbose(wxString::Format(_("Adding reference %s to %s."), ref.c_str(), m_output.GetFullPath().c_str()));
            m_ovl.AddReference(ref.mb_str(wxConvFile));
        } else if (child->GetName() == RAWXML_SYMBOL) {
            // <symbol target="common|unique" name="string" type="int|float" data="data" />
            BAKE_SKIP(child);
            cOvlType target = ParseType(child, RAWXML_SYMBOL);
            wxString name = ParseString(child, RAWXML_SYMBOL, wxT("name"), NULL);
            wxString type = ParseString(child, RAWXML_SYMBOL, wxT("type"), NULL);
            wxLogVerbose(wxString::Format(_("Adding symbol %s to %s."), name.c_str(), m_output.GetFullPath().c_str()));
            unsigned long data;
            if (type == wxT("int")) {
                name += wxT(":int");
                data = ParseUnsigned(child, RAWXML_SYMBOL, wxT("data"));
            } else if (type == wxT("float")) {
                name += wxT(":flt");
                float f = ParseFloat(child, RAWXML_SYMBOL, wxT("data"));
                data = *reinterpret_cast<unsigned long*>(&f);
            } else {
                throw RCT3Exception(_("symbol tag has unimplemented type value."));
            }
            m_ovl.AddDataSymbol(target, name.ToAscii(), data);
        } else if COMPILER_WRONGTAG(child) {
            throw RCT3Exception(wxString::Format(_("Unknown tag '%s' in rawovl or subovl tag."), child->GetName().c_str()));
        }

        child = child->GetNext();
    }

    m_prefix = oldprefix;
}
