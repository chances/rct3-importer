///////////////////////////////////////////////////////////////////////////////
//
// ovlmake
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
// Idea stolen from ovlcompiler by The_Cook
//
///////////////////////////////////////////////////////////////////////////////

#include "wx_pch.h"

#include <wx/config.h>
#include <wx/cmdline.h>
#include <wx/filename.h>
#include <exception>
#include <stdlib.h>

#include "SCNFile.h"
#include "RCT3Exception.h"
#include "OVLException.h"
#include "RawOvlMake.h"
#include "lib3Dconfig.h"
#include "confhelp.h"

#define VERSION_OVLMAKE "ovlmake v0.1.5"

int DoCompile(const wxCmdLineParser& parser) {
    int ret = 0;
    try {
        wxString p;
        wxFileName inputfile;
        wxFileName outputfile;
        bool convert = parser.Found(wxT("c"));
        bool checkonly = parser.Found(wxT("check"));
        bool dryrun = parser.Found(wxT("dryrun"));
        bool install = parser.Found(wxT("install"));
        wxString installdir = wxT("");
        if (!parser.Found(wxT("installdir"), &installdir)) {
            if (install) {
                HKEY key;
                char *temp = new char[MAX_PATH+1];
                LONG res = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                                        "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{907B4640-266B-4A21-92FB-CD1A86CD0F63}",
                                        0, KEY_QUERY_VALUE, &key);
                unsigned long length = MAX_PATH;
                res = RegQueryValueEx(key, "InstallLocation", 0, NULL, (LPBYTE) temp, &length);
                installdir = temp;
                delete[] temp;
                RegCloseKey(key);
                if (res != ERROR_SUCCESS) {
                    throw RCT3Exception(_("Cannot determine installatinon directory. Use --installdir to set manually."));
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
        inputfile = parser.GetParam(0);
        if (!inputfile.IsFileReadable())
            throw RCT3Exception(_("Cannot read input file"));

        cSCNFile c_scn;
        if (inputfile.GetExt().Lower() == wxT("xml")) {
            wxXmlDocument doc;
            if (doc.Load(inputfile.GetFullPath())) {
                wxXmlNode* root = doc.GetRoot();
                if (root->GetName() == RAWXML_ROOT) {
                    if (convert)
                        throw RCT3Exception(_("You cannot convert raw xml files"));

                    wxLogMessage(_("Processing raw xml file..."));
                    WRITE_RCT3_EXPERTMODE(true);

                    cRawOvl rovl;
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
                        rovl.Process(root, inputfile);
                    } else {
                        rovl.Process(root, inputfile, inputfile.GetPathWithSep(),parser.GetParam(1));
                    }
                    if (!bake.IsEmpty()) {
                        if (parser.GetParamCount() < 2) {
                            outputfile = inputfile;
                            outputfile.SetName(outputfile.GetName() + wxT("_baked"));
                        } else {
                            outputfile = parser.GetParam(1);
                        }
                        wxLogMessage(_("Writing baked raw xml file ") + outputfile.GetFullPath());
                        doc.Save(outputfile.GetFullPath(), 4);
                    }
                    if (dryrun) {
                        fprintf(stderr, "\nDryrun results:\n");
                        for (std::vector<wxFileName>::const_iterator it = rovl.GetModifiedFiles().begin(); it != rovl.GetModifiedFiles().end(); ++it) {
                            fprintf(stderr, "Modified: %s\n", it->GetFullPath().fn_str());
                        }
                        for (std::vector<wxFileName>::const_iterator it = rovl.GetNewFiles().begin(); it != rovl.GetNewFiles().end(); ++it) {
                            fprintf(stderr, "New: %s\n", it->GetFullPath().fn_str());
                        }
                    }
                    return ret;
                } else {
                    c_scn.filename = inputfile;
                    c_scn.LoadXML(&doc);
                }
            } else {
                throw RCT3Exception(_("Error in xml file"));
            }
        } else {
            c_scn.filename = inputfile.GetFullPath();
            c_scn.Load();
        }

        if (install || dryrun)
            throw RCT3Exception(_("Cannot dryrun or install non-raw xml file."));

        if (parser.GetParamCount() < 2) {
            outputfile = inputfile;
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
            if (!convert)
                temp.SetExt(wxT("common.ovl"));
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
                wxLogMessage(_("The file would be compiled  to ")+outputfile.GetPathWithSep()+outputfile.GetName()+wxT(".common.ovl"));
            } else
                wxLogMessage(_("Compiling ")+inputfile.GetFullPath()+_(" to ")+outputfile.GetPathWithSep()+outputfile.GetName()+wxT(".common.ovl"));
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

    } catch (RCT3Exception& e) {
        wxLogMessage(_("Compiling error: ")+e.wxwhat());
        ret = -1;
    } catch (EOvl& e) {
        wxLogMessage(_("Error in ovl creation: %s"), e.what());
        ret = -1;
    } catch (std::exception& e) {
        wxLogMessage(_("General error: %s"), e.what());
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

    // *&^$% GraphicsMagick
    wxFileName app = wxString(wxArgv[0]);
    wxString appenv = wxT("MAGICK_CONFIGURE_PATH=") + app.GetPathWithSep();
    putenv(appenv.c_str());

    wxConfig::Set(new wxConfig(wxT("ovlmake")));

    wxInitializer initializer;
    if ( !initializer )
    {
        fprintf(stderr, "Failed to initialize the wxWidgets library, aborting.");

        return -1;
    }

    fprintf(stderr, "--------------\n");
    fprintf(stderr, VERSION_OVLMAKE);
    fprintf(stderr, "\n--------------\n\n");

    static const wxCmdLineEntryDesc cmdLineDesc[] =
    {
        { wxCMD_LINE_SWITCH, wxT("h"), wxT("help"), _("show this help message"),
            wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
        { wxCMD_LINE_SWITCH, wxT("c"), wxT("convert"), _("convert the input file to new importer xml format (instead of creating ovl)") },
        { wxCMD_LINE_SWITCH, NULL, wxT("ignore"), _("ignore warnings durnig the sanity check and compile nevertheless") },
        { wxCMD_LINE_SWITCH, NULL, wxT("check"), _("only check the input file, do not actually write output") },

        { wxCMD_LINE_PARAM,  NULL, NULL, _("input file"), wxCMD_LINE_VAL_STRING },
        { wxCMD_LINE_PARAM,  NULL, NULL, _("output file"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },

        { wxCMD_LINE_NONE }
    };

    static const wxCmdLineEntryDesc cmdLineRealDesc[] =
    {
        { wxCMD_LINE_SWITCH, NULL, wxT("realhelp"), _("show this help message"),
            wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
        { wxCMD_LINE_SWITCH, wxT("c"), wxT("convert"), _("convert the input file to new importer xml format (instead of creating ovl)") },
        { wxCMD_LINE_SWITCH, NULL, wxT("ignore"), _("ignore warnings durnig the sanity check and compile nevertheless") },
        { wxCMD_LINE_SWITCH, NULL, wxT("check"), _("only check the input file, do not actually write output") },
        { wxCMD_LINE_SWITCH, NULL, wxT("dryrun"), _("only show which files would be generated, do not actually write output") },
        { wxCMD_LINE_SWITCH, NULL, wxT("install"), _("install ovls to RCT3 (output file is ignored)") },
        { wxCMD_LINE_OPTION, NULL, wxT("installdir"), _("name install directory (implies --install)") },
        { wxCMD_LINE_OPTION, NULL, wxT("bake"), _("bake included data into a new xml file. Space delimitered list.") },

        { wxCMD_LINE_PARAM,  NULL, NULL, _("input file"), wxCMD_LINE_VAL_STRING },
        { wxCMD_LINE_PARAM,  NULL, NULL, _("output file"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },

        { wxCMD_LINE_NONE }
    };

    wxCmdLineParser parser(cmdLineDesc, argc, wxArgv);
    parser.SetSwitchChars("-");

    if (parser.Parse(false) != -1) {
        wxCmdLineParser parser2(cmdLineRealDesc, argc, wxArgv);
        parser2.SetSwitchChars("-");
        switch ( parser2.Parse(false) )
        {
            case -1:
                parser2.Usage();
                break;
            case 0:
                ret = DoCompile(parser2);
                break;

            default:
                parser.Usage();
                wxLogError(_("Command line syntax error detected, aborting."));
                ret = -1;
                break;
        }
    } else {
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

