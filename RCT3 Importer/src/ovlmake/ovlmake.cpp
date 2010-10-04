///////////////////////////////////////////////////////////////////////////////
//
// ovlmake
// Command line ovl creation utility
// Copyright (C) 2008 Tobias Minch
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
//   Tobias Minich - belgabor@gmx.de
//
// Idea stolen from ovlcompiler by The_Cook
//
///////////////////////////////////////////////////////////////////////////////

#include <wx/wxprec.h>

#include <wx/config.h>
#include <wx/cmdline.h>
#include <wx/filename.h>
#include <wx/filesys.h>
#include <wx/fs_arc.h>
#include <wx/fs_filter.h>
#include <wx/mstream.h>
//#include <wx/fs_zip.h>
#include <wx/stdpaths.h>

#ifdef _WIN32
#include <wx/msw/registry.h>
#endif

#include <exception>
#include <iostream>
#include <stdlib.h>

#include <boost/format.hpp>
#include <boost/shared_ptr.hpp>

//#define LIBXMLTEST

#include "cXmlDoc.h"
#include "cXmlInputOutputCallbackString.h"
#include "cXmlNode.h"
#include "cXmlValidatorRelaxNG.h"
#include "cXmlValidatorRNVRelaxNG.h"
#include "cXmlValidatorIsoSchematron.h"
#include "cXmlXPath.h"
#include "cXmlXPathResult.h"
#include "cXsltStylesheet.h"
#include "wxXmlInputCallbackFileSystem.h"

#include "SCNFile.h"
#include "RCT3Exception.h"
#include "OVLException.h"
#include "gximage.h"
#include "pretty.h"
#include "RawParse.h"
#include "lib3Dconfig.h"
#include "confhelp.h"
#include "bzipstream.h"
#include "wxexception_libxmlcpp.h"
#include "xmldefs.h"

#include "version_wrap.h"

#ifdef UNICODE
#define UNIPTR(s) s.mb_str(wxConvLocal).data()
#else
#define UNIPTR(s) s.c_str()
#endif

using namespace r3;
using namespace std;
using namespace xmlcpp;

#define LOGINFO "Info"

void logOutput(bool brief, const char* file, int line, const char* severity, const char* message) {
	if (brief) {
		fprintf(stderr, "%s::%u::%s::%s\n", file, line, severity, message);
	} else {
		if (strcasecmp(severity, LOGINFO))
			fprintf(stderr, "%s: ", severity);
		fprintf(stderr, "%s\n", message);
		if (line) {
			fprintf(stderr, "  Line: %d\n", line);
			fprintf(stderr, "  File: %s\n", file);
		}
	}
}

class wxAutoLog {
public:
	wxAutoLog(wxLog* log) {
		m_oldlog = wxLog::GetActiveTarget();
		m_thelog.reset(log);
		wxLog::SetActiveTarget(log);
	}
	~wxAutoLog() {
		if (m_thelog)
			m_thelog->Flush();
		wxLog::SetActiveTarget(m_oldlog);
	}
	wxLog* get() {
		return m_thelog.get();
	}
private:
	wxLog* m_oldlog;
	boost::shared_ptr<wxLog> m_thelog;
};

class wxCustomLog: public wxLog {
public:
	wxCustomLog(const wxString& file, bool brief): m_file(file), m_brief(brief) {}
    virtual void DoLog(wxLogLevel level, const wxString& szString, time_t t) {
		const char* severity;
		if (level <= wxLOG_Error) {
			severity = "Error";
		} else if (level == wxLOG_Warning) {
			severity = "Warning";
		} else {
			severity = LOGINFO;
		}
		logOutput(m_brief, m_file.mb_str(), 0, severity, szString.mb_str());
	}
private:
	const wxString& m_file;
	bool m_brief;
};

void printVersion() {
    fprintf(stderr, "%s", UNIPTR(GetAppVersion()));
}

void logXmlErrors(cXmlErrorHandler* err, bool verbose, bool brief, const char* file) {
    foreach(const std::string& ge, err->getGenericErrors())
        //wxLogError(wxString::FromUTF8(ge.c_str()));
		logOutput(brief, file, 0, "Error", wxString::FromUTF8(ge.c_str()).mb_str());
    foreach(const cXmlStructuredError& se, err->getStructuredErrors()) {
        //wxString message = wxString::Format(_("Line %d: %s"), se.line, wxString::FromUTF8(se.message.c_str()).c_str());
		const char* severity;
        if (se.level == XML_ERR_NONE) {
            //wxLogMessage(message);
			severity = LOGINFO;
        } else if (se.level == XML_ERR_WARNING) {
            //wxLogWarning(message);
			severity = "Warning";
        } else {
            //wxLogError(message);
			severity = "Error";
        }
		logOutput(brief, file, se.line, severity, wxString::FromUTF8(se.message.c_str()).mb_str());
		
        if (verbose)
            //wxLogVerbose(wxString::Format(_("   XPath: %s"), wxString::FromUTF8(se.getPath().c_str()).c_str()));
			logOutput(brief, file, se.line, LOGINFO, wxString::Format(_("   XPath: %s"), wxString::FromUTF8(se.getPath().c_str()).c_str()).mb_str());
    }
}

int DoCompile(const wxCmdLineParser& parser) {
    int ret = 0;
	wxString inputfilestr;
	bool briefout = parser.Found("b");
	bool debugout = parser.Found("d");
	wxAutoLog log(new wxCustomLog(inputfilestr, briefout));
    try {
        bool verbose = parser.Found(wxT("v"));
        if (!parser.Found(wxT("s"))) {
			if (!briefout)
				printVersion();
            wxLog::SetVerbose(verbose);
            if (parser.Found(wxT("q")))
                wxLog::SetLogLevel(wxLOG_Warning);
        } else {
            wxLog::SetLogLevel(wxLOG_Error);
        }
        WRITE_RCT3_REPORTVALIDATION(true);
        wxString p;
        wxFSFileName inputfile;
        std::auto_ptr<wxFSFile> inputfsfile;
        wxFileName outputfile;
        bool convert = parser.Found(wxT("c"));
        bool checkonly = parser.Found(wxT("check"));
        bool dryrun = parser.Found(wxT("dryrun"));
        bool install = parser.Found(wxT("install"));
        if (parser.Found("novalid")) {
            wxLogVerbose(_("Validation disabled"));
            WRITE_RCT3_VALIDATE(false);
        } else if (parser.Found("deep")) {
            WRITE_RCT3_DEEPVALIDATE(true);
        }
        {
            wxString sortalgo;
            if (parser.Found(wxT("t"), &sortalgo)) {
                wxLogVerbose(_("Generally sorting triangles by ")+sortalgo);
                WRITE_RCT3_TRIANGLESORT_X(sortalgo);
                WRITE_RCT3_TRIANGLESORT_Y(sortalgo);
                WRITE_RCT3_TRIANGLESORT_Z(sortalgo);
            }
            if (parser.Found(wxT("trianglesortx"), &sortalgo)) {
                wxLogVerbose(_("X-sorting triangles by ")+sortalgo);
                WRITE_RCT3_TRIANGLESORT_X(sortalgo);
            }
            if (parser.Found(wxT("trianglesorty"), &sortalgo)) {
                wxLogVerbose(_("Y-sorting triangles by ")+sortalgo);
                WRITE_RCT3_TRIANGLESORT_Y(sortalgo);
            }
            if (parser.Found(wxT("trianglesortz"), &sortalgo)) {
                wxLogVerbose(_("Z-sorting triangles by ")+sortalgo);
                WRITE_RCT3_TRIANGLESORT_Z(sortalgo);
            }
        }
        wxString installdir = wxT("");
        if (!parser.Found(wxT("installdir"), &installdir)) {
            if (install) {
				try {
#ifdef _WIN32
#if 0
                HKEY key;
                wxChar *temp = new wxChar[MAX_PATH+1];
                LONG res = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                                        wxT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{907B4640-266B-4A21-92FB-CD1A86CD0F63}"),
                                        0, KEY_QUERY_VALUE, &key);
                unsigned long length = MAX_PATH;
                res = RegQueryValueEx(key, wxT("InstallLocation"), 0, NULL, (LPBYTE) temp, &length);
                installdir = temp;
                delete[] temp;
                RegCloseKey(key);
                if (res != ERROR_SUCCESS) {
                    throw RCT3Exception(_("Cannot determine installation directory. Use --installdir to set manually"));
                }
#endif
					wxRegKey key("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{907B4640-266B-4A21-92FB-CD1A86CD0F63}");
					if (!key.Exists())
						throw 1;
					key.QueryValue("InstallLocation", installdir);
					if (installdir.IsEmpty())
						throw 1;
#else
					throw 1;
#endif
				} catch(...) {
                    throw RCT3Exception(_("Cannot determine installation directory. Use --installdir to set manually"));					
				}
            }

        } else {
            install = true;
        }
        wxString bake;
        parser.Found(wxT("bake"), &bake);
        if (convert)
            wxLogMessage(_("Mode: Convert to new xml"));
        else if (checkonly)
            wxLogMessage(_("Mode: Check input"));
        else if (dryrun)
            wxLogMessage(_("Mode: Dryrun"));
        else if (!bake.IsEmpty())
            wxLogMessage(_("Mode: Bake"));
        else if (install)
            wxLogMessage(_("Mode: Install"));
        else
            wxLogMessage(_("Mode: Create OVL"));

        if (parser.GetParamCount() == 0)
            throw RCT3Exception(_("No input file given"));
        inputfilestr = parser.GetParam(0);
        inputfile = inputfilestr;
        wxLogMessage(inputfile.GetFullPath());
/*
        if (!inputfile.IsFileReadable())
            throw RCT3Exception(_("Cannot read input file"));
*/
        wxFileSystem fs;
        inputfsfile = std::auto_ptr<wxFSFile>(fs.OpenFile(inputfilestr, wxFS_READ | wxFS_SEEKABLE));
        if (!inputfsfile.get())
            throw RCT3Exception(_("Cannot read input file ")+inputfilestr);

        cSCNFile c_scn;
        if (inputfile.GetExt().Lower() == wxT("xml")) {
            cXmlDoc doc;

            if (doc.read(inputfilestr.mb_str(wxConvUTF8), NULL, XML_PARSE_DTDLOAD)) {
                cXmlNode root(doc.root());
                if (root(RAWXML_ROOT)) {
                    doc.xInclude();

                    if (convert)
                        throw RCT3Exception(_("You cannot convert raw xml files"));

                    wxLogMessage(_("Processing raw xml file..."));

                    boost::shared_ptr<cXmlValidator> val;
                    bool repeatvalres = false;
                    cXmlValidatorResult valres;
                    if (READ_RCT3_VALIDATE()) {
                        wxLogMessage(_("Validating..."));
                        val = cRawParser::Validator();
                        valres = doc.validate(*val, cXmlValidator::OPT_DETERMINE_NODE_BY_XPATH, cXmlValidatorResult::VR_NONE);
                        if (valres(parser.Found("strict")?cXmlValidatorResult::VR_WARNING:cXmlValidatorResult::VR_ERROR)) {
							wxe_xml_error_infos einfos;
							transferXmlErrors(*val, einfos);
                            throw RCT3Exception(_("Validation Error")) << wxe_xml_errors(einfos);
                        } else if (valres) {
                            logXmlErrors(val.get(), verbose, briefout, inputfilestr.mb_str());							
						}
                        wxLogMessage(_("...Ok!"));
                    }

                    if (checkonly) {
                        wxLogMessage(_("Remember, for raw xml files, check only mode means only validation is done."));
                        return 0;
                    }

                    cXmlXPath path(doc, "raw", XML_NAMESPACE_RAW);
                    cXmlXPathResult metas = path("//raw:metadata[@role='ovlmake']");
                    for (int s = 0; s < metas.size(); ++s) {
                        cXmlNode meta = metas[s];

                        foreach(const cXmlNode& entry, meta.children()) {
                            if (entry("noPrefixWarn")) {
                                WRITE_RCT3_WARNPREFIX(false);
                            } else if (entry("repeatValidationResult")) {
                                repeatvalres = true;
                            } else {
                                //wxLogWarning(wxString::Format(_("Unknown ovlmake metadata element '%s'."), entry.wxname()));
								logOutput(briefout, inputfilestr.mb_str(), entry.line(), "Warning", wxString::Format(_("Unknown ovlmake metadata element '%s'."), entry.wxname()).mb_str());
                            }
                        }
                    }

                    WRITE_RCT3_EXPERTMODE(true);
                    if (!parser.Found(wxT("v")))
                        WRITE_RCT3_MOREEXPERTMODE(true);
                    cRawParser rovl;
                    rovl.SetUserDir(wxStandardPaths::Get().GetDocumentsDir());
                    if (!bake.IsEmpty()) {
                        rovl.SetOptions(MODE_BAKE, dryrun);
                        rovl.AddBakeStructures(bake);
                    } else if (install) {
                        rovl.SetOptions(MODE_INSTALL, dryrun);
                    } else {
                        rovl.SetOptions(MODE_COMPILE, dryrun);
                    }
                    if (install) {
                        wxFileName temp(installdir, wxEmptyString);
                        rovl.Process(root, inputfile, temp.GetPathWithSep());
                    } else if (parser.GetParamCount() < 2) {
                        rovl.Process(root, inputfile, inputfilestr.BeforeFirst(wxT('#')));
                    } else {
                        rovl.Process(root, inputfile, inputfilestr.BeforeFirst(wxT('#')), parser.GetParam(1));
                    }
                    if (!bake.IsEmpty()) {
                        if (parser.GetParamCount() < 2) {
                            outputfile = inputfilestr.BeforeFirst(wxT('#'));
                            outputfile.SetName(outputfile.GetName() + wxT("_baked"));
                            outputfile.SetExt(wxT("xml"));
                        } else {
                            outputfile = parser.GetParam(1);
                        }
                        wxLogMessage(_("Writing baked raw xml file ") + outputfile.GetFullPath());
                        doc.write(outputfile.GetFullPath(), true);
                    }

                    if (repeatvalres && val && val->ok() && valres) {
                        wxLogMessage("-----");
                        wxLogMessage("Validation results:");
                        logXmlErrors(val.get(), verbose, briefout, inputfilestr.mb_str());
                    }

                    if (dryrun) {
                        fprintf(stderr, "\nDryrun results:\n");
                        for (std::vector<wxFileName>::const_iterator it = rovl.GetModifiedFiles().begin(); it != rovl.GetModifiedFiles().end(); ++it) {
							if (briefout) {
								logOutput(true, it->GetFullPath().mb_str(), 0, "Report", "Modified");
							} else {
								logOutput(false, it->GetFullPath().mb_str(), 0, LOGINFO, wxString::Format(_("Modified: %s"), it->GetFullPath().c_str()).mb_str());
							}
/*
#ifdef UNICODE
                            fprintf(stderr, "Modified: %s\n", it->GetFullPath().mb_str(wxConvFile).data());
#else
                            fprintf(stderr, "Modified: %s\n", it->GetFullPath().c_str());
#endif
*/
                        }
                        for (std::vector<wxFileName>::const_iterator it = rovl.GetNewFiles().begin(); it != rovl.GetNewFiles().end(); ++it) {
							if (briefout) {
								logOutput(true, it->GetFullPath().mb_str(), 0, "Report", "New");
							} else {
								logOutput(false, it->GetFullPath().mb_str(), 0, LOGINFO, wxString::Format(_("New: %s"), it->GetFullPath().c_str()).mb_str());
							}
/*
#ifdef UNICODE
                            fprintf(stderr, "New: %s\n", it->GetFullPath().mb_str(wxConvFile).data());
#else
                            fprintf(stderr, "New: %s\n", it->GetFullPath().c_str());
#endif
*/
                        }
                    }
                    return ret;
                } else {
                    c_scn.filename = inputfile.GetFullPath();
                    //c_scn.LoadXML(&doc);
                    c_scn.Load();
                }
            } else {
                //logXmlErrors(&doc, verbose);
				wxe_xml_error_infos einfos;
				transferXmlErrors(doc, einfos);
                throw RCT3Exception(_("General XML parsing failure")) << wxe_xml_errors(einfos);
            }
        } else if (inputfile.GetExt().Lower() == wxT("ms3d")) {
            boost::shared_ptr<c3DLoader> object = c3DLoader::LoadFile(inputfile.GetFullPath().c_str());
            if (!object.get()) {
                throw RCT3Exception(_("Failed to load input file"));
            }
            wxString outputxml = inputfile.GetFullPath() + wxT(".xml");

            cXmlDoc doc(true);

            cXmlNode root("ovl");
            root.prop("xmlns", XML_NAMESPACE_COMPILER);

            cXmlNode fix("fix");
            fix.prop("handedness", "right");
            fix.prop("up", "y");
            root.appendChildren(fix);

            cXmlNode bsh("bsh");
            bsh.prop("name", inputfile.GetName().utf8_str());
            bsh.prop("model", inputfile.GetFullName().utf8_str());

            for (unsigned int i = 0; i < object->getObjectCount(); ++i) {
                if (object->isObjectValid(i)) {
                    cXmlNode geom("geomobj");
                    geom.prop("name", object->getObjectName(i).utf8_str());
                    geom.prop("ftx", "???");
                    geom.prop("txs", "SIOpaque");
                    if (object->getObjectVertex2(i, 0).bone[0] >= 0) {
                        geom.prop("bone", object->getBone(object->getObjectVertex2(i, 0).bone[0]).m_name.utf8_str());
                    }
                    bsh.appendChildren(geom);
                }
            }
            for (int i = 0; i < object->getBoneCount(); ++i) {
                cXmlNode bone("bone");
                bone.prop("name", object->getBone(i).m_name.utf8_str());
                if (!object->getBone(i).m_parent.IsEmpty()) {
                    bone.prop("parent", object->getBone(i).m_parent.utf8_str());
                }
                MATRIX p = object->getBone(i).m_pos[0];
                bone.newChild("pos1", boost::str(boost::format("%f %f %f %f   %f %f %f %f   %f %f %f %f   %f %f %f %f") % p._11 % p._12 % p._13 % p._14 %
                    p._21 % p._22 % p._23 % p._24 % p._31 % p._32 % p._33 % p._34 % p._41 % p._42 % p._43 % p._44).c_str());
                p = object->getBone(i).m_pos[1];
                bone.newChild("pos2", boost::str(boost::format("%f %f %f %f   %f %f %f %f   %f %f %f %f   %f %f %f %f") % p._11 % p._12 % p._13 % p._14 %
                    p._21 % p._22 % p._23 % p._24 % p._31 % p._32 % p._33 % p._34 % p._41 % p._42 % p._43 % p._44).c_str());
                bsh.appendChildren(bone);
            }
            root.appendChildren(bsh);

            cXmlNode ban("ban");
            ban.prop("name", inputfile.GetName().utf8_str());
            const c3DAnimation& an = object->getAnimations().begin()->second;

            typedef pair<wxString, c3DAnimBone> bonepair;
            foreach(const bonepair& bp, an.m_bones) {
                cXmlNode bone("bone");
                bone.prop("name", bp.second.m_name.utf8_str());
                for (size_t f = 0; f < bp.second.m_translations.size(); ++f) {
                    txyz t = bp.second.m_translations[f];
                    cXmlNode trans("translate", boost::str(boost::format("%f %f %f") % t.X % t.Y % t.Z).c_str());
                    trans.prop("time", boost::str(boost::format("%f") % t.Time).c_str());
                    bone.appendChildren(trans);
                }
                for (size_t f = 0; f < bp.second.m_rotations.size(); ++f) {
                    txyz t = bp.second.m_rotations[f];
                    cXmlNode rot("rotate", boost::str(boost::format("%f %f %f") % t.X % t.Y % t.Z).c_str());
                    rot.prop("time", boost::str(boost::format("%f") % t.Time).c_str());
                    bone.appendChildren(rot);
                }
                ban.appendChildren(bone);
            }
/*
            for (int i = 0; i < an.m_bones.size(); ++i) {
                cXmlNode bone("bone");
                bone.prop("name", an.m_bones[i].m_name.utf8_str());
                for (int f = 0; f < an.m_bones[i].m_translations.size(); ++f) {
                    txyz t = an.m_bones[i].m_translations[f];
                    cXmlNode trans("translate", boost::str(boost::format("%f %f %f") % t.X % t.Y % t.Z).c_str());
                    trans.prop("time", boost::str(boost::format("%f") % t.Time).c_str());
                    bone.appendChildren(trans);
                }
                for (int f = 0; f < an.m_bones[i].m_rotations.size(); ++f) {
                    txyz t = object->an.m_bones[i].m_rotations[f];
                    cXmlNode rot("rotate", boost::str(boost::format("%f %f %f") % t.X % t.Y % t.Z).c_str());
                    rot.prop("time", boost::str(boost::format("%f") % t.Time).c_str());
                    bone.appendChildren(rot);
                }
                ban.appendChildren(bone);
            }
*/
            root.appendChildren(ban);

            doc.root(root);
            doc.write(outputxml.utf8_str(), true, wxString(wxT("UTF-8")));

            return 0;
        } else {
            c_scn.filename = inputfile.GetFullPath();
            c_scn.Load();
        }

        if (install || dryrun)
            throw RCT3Exception(_("Cannot dryrun or install non-raw xml file"));

        if (parser.GetParamCount() < 2) {
            outputfile = inputfilestr.BeforeFirst(wxT('#'));
            if (convert) {
                outputfile.SetExt(wxT("xml"));
                if (outputfile.FileExists())
                    outputfile.SetName(outputfile.GetName() + wxT("_converted"));
            } else {
                if (!c_scn.name.IsEmpty())
                    outputfile.SetName(c_scn.name);
                if (!c_scn.ovlpath.GetPath().IsEmpty())
                    outputfile.SetPath(c_scn.ovlpath.GetPath());
            }
        } else {
            wxString temp = parser.GetParam(1);
            if (temp.Lower().EndsWith(wxT(".common.ovl")))
                temp = temp.BeforeLast('.');
            outputfile = temp;
        }

        if (!checkonly) {
            // Copy as wxFileName gets confused by common.ovl
            wxFileName temp = outputfile;
            if (!convert) {
                temp.SetName(c_scn.prefix + outputfile.GetName());
                temp.SetExt(wxT("common.ovl"));
            }
            if (temp.GetPath().IsEmpty())
                temp.SetPath(wxT("."));
            if (temp.FileExists()) {
                if (!temp.IsFileWritable())
                    throw RCT3Exception(_("Cannot write output file ")+temp.GetFullPath());
            } else {
                if (!temp.IsDirWritable())
                    throw RCT3Exception(_("Cannot write output file ")+temp.GetFullPath());
            }
        }

        if (convert) {
            wxLogMessage(_("Converting ")+inputfile.GetFullPath()+_(" to ")+outputfile.GetFullPath());
            c_scn.filename = outputfile;
            c_scn.Save();
        } else {
            c_scn.name = outputfile.GetName();
            c_scn.ovlpath = outputfile.GetPathWithSep();
            if (checkonly) {
                wxLogMessage(_("Checking ")+inputfile.GetFullPath());
                wxLogMessage(_("The file would be compiled  to ")+outputfile.GetPathWithSep()+c_scn.prefix+outputfile.GetName()+wxT(".common.ovl"));
            } else
                wxLogMessage(_("Compiling ")+inputfile.GetFullPath()+_(" to ")+outputfile.GetPathWithSep()+c_scn.prefix+outputfile.GetName()+wxT(".common.ovl"));
            wxLogMessage(_("Performing sanity check..."));
            if (!c_scn.Check()) {
                ret = 1;
                if ((!parser.Found(wxT("ignore"))) && (!checkonly))
                    throw RCT3Exception(_("There were warnings during the sanity check and you didn't specify the '--ignore' command line option"));
            } else if (checkonly) {
                wxLogMessage(_("Everything seems to be ok."));
            }
            if (!checkonly) {
                c_scn.Make();
                wxLogMessage(_("OVL written successfully."));
            }
        }

    } catch (WXException& e) {
        //wxLogMessage(_("Compiling error: ")+e.wxwhat());
        int* line = boost::get_error_info<wxe_xml_node_line>(e);
        wxString* file = boost::get_error_info<wxe_file>(e);
		wxe_xml_error_infos* xmlerr = boost::get_error_info<wxe_xml_errors>(e);
		wxString rfile = inputfilestr;
		if (file)
			rfile = *file;
		wxString msg = e.wxwhat();
		if (xmlerr && xmlerr->size())
			msg += _(", xml errors follow.");
		else
			msg += ".";
		logOutput(briefout, rfile.mb_str(), line?*line:0, "Error", msg.mb_str());
		if (xmlerr) {
			foreach(const wxe_xml_error_info& inf, *xmlerr) {
				logOutput(briefout, rfile.mb_str(), inf.line, inf.severity.mb_str(), inf.error.mb_str());
			}
		}
        //if (line)
        //    wxLogMessage(_("  Line: %d"), *line);
		
		if (debugout) {
			std::cout << "*****" << std::endl;
			std::cout << "Debug information:" << std::endl;
			std::cout << boost::diagnostic_information(e);
			std::cout << "*****" << std::endl;
		}
		
        ret = -1;
    } catch (EOvl& e) {
		wxString msg = _("OVL creation: ");
		msg += wxString(e.what(), wxConvLocal);
		logOutput(briefout, inputfilestr.mb_str(), 0, "Error", msg.mb_str());
        ret = -1;
    } catch (std::exception& e) {
		wxString msg = _("General: ");
		msg += wxString(e.what(), wxConvLocal);
		logOutput(briefout, inputfilestr.mb_str(), 0, "Error", msg.mb_str());
        ret = -1;
    }
    return ret;
}

int main(int argc, char **argv)
{
    int ret = 0;
#if wxUSE_UNICODE
    wxChar **wxArgv = new wxChar *[argc + 1];

    {
        int n;

        for (n = 0; n < argc; n++ )
        {
            wxMB2WXbuf warg = wxConvertMB2WX(argv[n]);
            wxArgv[n] = wxStrdup(warg);
        }

        wxArgv[n] = NULL;
    }
#else // !wxUSE_UNICODE
    #define wxArgv argv
#endif // wxUSE_UNICODE/!wxUSE_UNICODE

#ifdef PUBLICDEBUG
    if (!LoadLibrary("exchndl.dll"))
        fprintf(stderr, "Failed to load debug library!\n");
#endif

    // *&^$% GraphicsMagick
    wxFileName app = wxStandardPaths::Get().GetExecutablePath();
    if (!app.IsAbsolute())
        app.MakeAbsolute();
    try {
//        fprintf(stderr, "All .mgk files expected in '%s'.\n", static_cast<const char*>(app.GetPathWithSep().mb_str(wxConvLocal)));
        wxGXImage::CheckGraphicsMagick(app.GetPathWithSep());
    } catch (exception& e) {
        fprintf(stderr, "Error initializing the graphics library:\n");
        fprintf(stderr, "%s\n", e.what());
        fprintf(stderr, "All .mgk files expected in '%s'.\n", static_cast<const char*>(app.GetPathWithSep().mb_str(wxConvLocal)));
        return -1;
    }

    //wxFileSystem::AddHandler(new wxZipFSHandler);
    wxFileSystem::AddHandler(new wxArchiveFSHandler);
    wxFileSystem::AddHandler(new wxFilterFSHandler);
    wxBZipClassFactory cf;

    wxConfig::Set(new wxConfig(wxT("ovlmake")));
    WRITE_RCT3_WARNOVERWRITE(false);

    wxInitializer initializer;
    if ( !initializer )
    {
        fprintf(stderr, "Failed to initialize the wxWidgets library, aborting.");

        return -1;
    }

    wxLocale loc(wxLANGUAGE_ENGLISH);


    static const wxCmdLineEntryDesc cmdLineDesc[] =
    {
        { wxCMD_LINE_SWITCH, "h", "help", "show this help message",
            wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
        { wxCMD_LINE_SWITCH, "c", "convert", "convert the input file to new importer xml format (instead of creating ovl)" },
        { wxCMD_LINE_SWITCH, NULL, "ignore", "ignore warnings durnig the sanity check and compile nevertheless" },
        { wxCMD_LINE_SWITCH, NULL, "check", "only check the input file, do not actually write output" },
        { wxCMD_LINE_OPTION, "t", "trianglesort", "choose triangle sort algorithm (min, max, mean, minmax or maxmin. Default: minmax)." },
        { wxCMD_LINE_OPTION, NULL, "trianglesortx", "choose triangle sort algorithm in x-direction." },
        { wxCMD_LINE_OPTION, NULL, "trianglesorty", "choose triangle sort algorithm in y-direction." },
        { wxCMD_LINE_OPTION, NULL, "trianglesortz", "choose triangle sort algorithm in z-direction." },
        { wxCMD_LINE_SWITCH, "v", "verbose", "enable verbose logging" },
        { wxCMD_LINE_SWITCH, "q", "quiet", "in quiet mode only warnings and errors are shown" },
        { wxCMD_LINE_SWITCH, "s", "silent", "in silent mode only errors are shown" },
        { wxCMD_LINE_SWITCH, "b", "brief", "use brief message output format suitable for parsing by frontends" },
        { wxCMD_LINE_SWITCH, NULL, "novalid", "do not validate xml files" },
        { wxCMD_LINE_SWITCH, NULL, "deep", "do a deep validation of some xml files (warning: this can take a long time!)" },

        { wxCMD_LINE_PARAM,  NULL, NULL, "input file", wxCMD_LINE_VAL_STRING },
        { wxCMD_LINE_PARAM,  NULL, NULL, "output file", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },

        wxCMD_LINE_DESC_END
    };

    static const wxCmdLineEntryDesc cmdLineRealDesc[] =
    {
        { wxCMD_LINE_SWITCH, NULL, "realhelp", "show this help message",
            wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
        { wxCMD_LINE_SWITCH, "c", "convert", "convert the input file to new importer xml format (instead of creating ovl)" },
        { wxCMD_LINE_SWITCH, NULL, "ignore", "ignore warnings durnig the sanity check and compile nevertheless" },
        { wxCMD_LINE_SWITCH, NULL, "check", "only check the input file, do not actually write output" },
        { wxCMD_LINE_OPTION, "t", "trianglesort", "choose triangle sort algorithm (min, max, mean, minmax or maxmin. Default: minmax)." },
        { wxCMD_LINE_OPTION, NULL, "trianglesortx", "choose triangle sort algorithm in x-direction." },
        { wxCMD_LINE_OPTION, NULL, "trianglesorty", "choose triangle sort algorithm in y-direction." },
        { wxCMD_LINE_OPTION, NULL, "trianglesortz", "choose triangle sort algorithm in z-direction." },
        { wxCMD_LINE_SWITCH, "v", "verbose", "enable verbose logging" },
        { wxCMD_LINE_SWITCH, "d", "debug", "enable verbose error logging" },
        { wxCMD_LINE_SWITCH, "q", "quiet", "in quiet mode only warnings and errors are shown" },
        { wxCMD_LINE_SWITCH, "s", "silent", "in silent mode only errors are shown" },
        { wxCMD_LINE_SWITCH, "b", "brief", "use brief message output format suitable for parsing by frontends" },
        { wxCMD_LINE_SWITCH, NULL, "novalid", "do not validate xml files" },
        { wxCMD_LINE_SWITCH, NULL, "deep", "do a deep validation of some xml files (warning: this can take a long time!)" },
        { wxCMD_LINE_SWITCH, NULL, "strict", "treat validation warnings as errors" },
        { wxCMD_LINE_SWITCH, NULL, "dryrun", "only show which files would be generated, do not actually write output" },
        { wxCMD_LINE_SWITCH, NULL, "install", "install ovls to RCT3 (output file is ignored)" },
        { wxCMD_LINE_OPTION, NULL, "installdir", "name install directory (implies --install)" },
        { wxCMD_LINE_OPTION, NULL, "bake", "bake included data into a new xml file. Space delimitered list." },
        { wxCMD_LINE_SWITCH, NULL, "bakehelp", "show what structures can be baked.",
            wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },

        { wxCMD_LINE_PARAM,  NULL, NULL, "input file", wxCMD_LINE_VAL_STRING },
        { wxCMD_LINE_PARAM,  NULL, NULL, "output file", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },

        wxCMD_LINE_DESC_END
    };

    wxCmdLineParser parser(cmdLineDesc, argc, wxArgv);
    parser.SetSwitchChars(wxT("-"));

    if (parser.Parse(false) != -1) {
        wxCmdLineParser parser2(cmdLineRealDesc, argc, wxArgv);
        parser2.SetSwitchChars(wxT("-"));
        switch ( parser2.Parse(false) )
        {
            case -1:
                printVersion();
                if (parser2.Found(wxT("bakehelp"))) {
                    fprintf(stderr, "The following structures can be baked:\n");
                    wxSortedArrayString b;
                    cRawParser::FillAllBakes(b);
                    for (wxSortedArrayString::const_iterator it = b.begin(); it != b.end(); ++it) {
                        fprintf(stderr, "%s\n", UNIPTR((*it)));
                    }
                    fprintf(stderr, "Use 'all' to bake all of the above. 'all' only works alone.\n");
                    fprintf(stderr, "Use 'xml' to bake included raw xml files.\n");
                    fprintf(stderr, "Use 'absolute' to convert all input filenames to be absolute paths.\n");
                } else
                    parser2.Usage();
                break;
            case 0:
                ret = DoCompile(parser2);
                break;

            default:
                printVersion();
                parser.Usage();
                wxLogError(_("Command line syntax error detected, aborting."));
                ret = -1;
                break;
        }
    } else {
        printVersion();
        parser.Usage();
    }

#if wxUSE_UNICODE
    {
        for ( int n = 0; n < argc; n++ )
            free(wxArgv[n]);

        delete [] wxArgv;
    }
#endif // wxUSE_UNICODE

    wxConfig::Get()->DeleteAll();

    wxUnusedVar(argc);
    wxUnusedVar(argv);
    return ret;
}

