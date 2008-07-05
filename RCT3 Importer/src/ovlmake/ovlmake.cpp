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
#include <exception>
#include <stdlib.h>

#include <boost/format.hpp>

#include "version.h"

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
#include "xmldefs.h"

#ifdef UNICODE
#define UNIPTR(s) s.mb_str(wxConvLocal).data()
#else
#define UNIPTR(s) s.c_str()
#endif

using namespace r3;
using namespace std;
using namespace xmlcpp;

inline void maximize(int& domax, const int withmax) {
    if (withmax>domax)
        domax = withmax;
}

void printVersion() {
    wxString strdate = _("Built on ") + wxString(AutoVersion::DATE, wxConvLocal) + wxT(".")
                          + wxString(AutoVersion::MONTH, wxConvLocal) + wxT(".")
                          + wxString(AutoVersion::YEAR, wxConvLocal);
    wxString strversion = wxString::Format(wxT("ovlmake v%ld.%ld, Build %ld, svn %s, "), AutoVersion::MAJOR, AutoVersion::MINOR, AutoVersion::BUILD, AutoVersion::SVN_REVISION)+ wxString(AutoVersion::STATUS, wxConvLocal);
    wxString compversion("Compiler: ");
#ifdef __GNUC__
    compversion += "GCC ";
#else
    compversion += "Non-GCC ";
#endif
    compversion += wxString(__VERSION__);
#ifdef UNICODE
    strdate += wxT(", Unicode");
#else
    strdate += wxT(", ANSI");
#endif
#ifdef PUBLICDEBUG
    strdate += wxT(" Debug");
#endif
    strdate += _(" version");
    int linelen = strdate.size();
    maximize(linelen, strversion.size());
    maximize(linelen, compversion.size());
    wxString line(wxT('-'), linelen);
    fprintf(stderr, "\n%s\n", UNIPTR(line));
    fprintf(stderr, "%s\n", UNIPTR(strversion));
    fprintf(stderr, "%s\n", UNIPTR(strdate));
    fprintf(stderr, "%s", UNIPTR(compversion));
    fprintf(stderr, "\nCopyright (C) 2008 Belgabor");
    fprintf(stderr, "\n%s\n\n", UNIPTR(line));
    fprintf(stderr, "This program comes with ABSOLUTELY NO WARRANTY.\n");
    fprintf(stderr, "This is free software, and you are welcome to redistribute it\n");
    fprintf(stderr, "under certain conditions; see License.txt for details.\n\n");
}

void logXmlErrors(cXmlErrorHandler* err, bool verbose) {
    foreach(const std::string& ge, err->getGenericErrors())
        wxLogError(wxString::FromUTF8(ge.c_str()));
    foreach(const cXmlStructuredError& se, err->getStructuredErrors()) {
        wxString message = wxString::Format(_("Line %d: %s"), se.line, wxString::FromUTF8(se.message.c_str()).c_str());
        if (se.level == XML_ERR_NONE) {
            wxLogMessage(message);
        } else if (se.level == XML_ERR_WARNING) {
            wxLogWarning(message);
        } else {
            wxLogError(message);
        }
        if (verbose)
            wxLogVerbose(wxString::Format(_("   XPath: %s"), wxString::FromUTF8(se.getPath().c_str()).c_str()));
    }
}

int DoCompile(const wxCmdLineParser& parser) {
    int ret = 0;
    try {
        bool verbose = parser.Found(wxT("v"));
        if (!parser.Found(wxT("s"))) {
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
        wxString inputfilestr;
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
            //wxXmlDocument doc;
            cXmlDoc doc;

#ifdef LIBXMLTEST
{
            {
                xmlcpp::cXmlDoc doc2;
                xmlcpp::cXmlInputOutputCallbackString::Init();
                xmlcpp::wxXmlInputCallbackFileSystem::Init();
                doc2 = xmlcpp::cXmlDoc("testa.xml");
//                doc2 = xmlcpp::cXmlDoc("stalltest_baked.xml");
//                doc2.write("string:test");
//                fprintf(stderr, "%s", xmlcpp::cXmlInputOutputCallbackString::get("test").c_str());
                //xmlcpp::cXmlValidatorRelaxNG val;
                //xmlcpp::cXsltStylesheet sht;

                //sht.read("C:\\Download\\Development\\iso_simple.xsl");
                //if (sht.ok()) {
                //    xmlcpp::cXmlDoc docs = sht.getDoc();
                //    docs.write("simpletest.xsl");
                //}

                //val.read("C:\\Development\\svn\\RCT3 Importer\\doc\\xml\\rct3xml-raw-v1.rng");
                xmlcpp::cXmlValidatorIsoSchematron val;
                //xmlcpp::cXmlValidatorRNVRelaxNG val;
                std::string st =
                   "<schema xmlns=\"http://www.ascc.net/xml/schematron\" >\
                      <pattern name=\"Test\">\
                        <rule context=\"rawovl\">\
                          <assert test=\"@version\">Version missing</assert>\
                          <report test=\"@version\">Version present</report>\
                          <assert test=\"@bogus\">Bogus missing</assert>\
                          <report test=\"@version\">Bogus present</report>\
                        </rule>\
                      </pattern>\
                    </schema>\
                    ";
                //val.read("schematron.xml");
                val.read("C:\\Development\\svn\\RCT3 Importer\\doc\\xml\\rct3xml-raw-v1.sch");
                //val.read("test.rnc");
                //val.read("docbook.rnc");
                fprintf(stderr, "Validator parsing errors\n");
                for (std::vector<xmlcpp::cXmlStructuredError>::const_iterator it = val.getStructuredErrors().begin(); it != val.getStructuredErrors().end(); ++it)
                    wxLogError(wxString(it->message.c_str(), wxConvUTF8).Trim(true));
                val.clearStructuredErrors();

#if 1
                // XPath tests
                {
                    xmlcpp::cXmlXPath xpath(doc2);
                    xpath.registerNs("rct3", "http://rct3.sourceforge.net/rct3xml/raw");
                    xmlcpp::cXmlXPathResult xres = xpath("//rct3:rawovl");
                    for (int i = 0; i < xres.size(); ++i) {
                        fprintf(stderr, "%d, %s, %s\n", i, xres[i].name().c_str(), xres[i].path().c_str());
                    }
                    xpath.setNodeContext(xres[1]);
                    xres = xpath("@comment");
                    for (int i = 0; i < xres.size(); ++i) {
                        fprintf(stderr, "%d, %s %s\n", i, xres[i].name().c_str(), xres[i].content().c_str());
                    }
                }
#endif

#if 0
                {
                    if (val.ok()) {
                        wxDateTime start = wxDateTime::Now();

                        for (int p = 0; p < 100; ++p) {
                            val.reparse();
                        }

                        wxTimeSpan took = wxDateTime::Now().Subtract(start);
                        wxLogError(wxString::Format(_("Took %s for rnc performance test."), took.Format(wxT("%H:%M:%S")).c_str()));
                    }
                    fprintf(stderr, "Validator parsing errors (after test)\n");
                    for (std::vector<xmlcpp::cXmlStructuredError>::const_iterator it = val.getStructuredErrors().begin(); it != val.getStructuredErrors().end(); ++it)
                        wxLogError(wxString(it->message.c_str(), wxConvUTF8).Trim(true));
                    val.clearStructuredErrors();

                    {
                        xmlcpp::cXmlDoc doct("c:\\Development\\docbook-rng-1.0b1\\docbook.rng");
                        xmlcpp::cXsltStylesheet sht("c:\\Development\\rng-incelim-1.2\\incelim.xsl");

                        if (sht.ok() && doct.ok()) {
                            wxDateTime start = wxDateTime::Now();

                            for (int p = 0; p < 1; ++p) {
                                sht.transform(doct);
                            }

                            wxTimeSpan took = wxDateTime::Now().Subtract(start);
                            wxLogError(wxString::Format(_("Took %s for include performance test."), took.Format(wxT("%H:%M:%S")).c_str()));

                            xmlcpp::cXmlDoc doct2(sht.transform(doct));
                            xmlcpp::cXsltStylesheet sht2("c:\\Development\\RngToRnc-1_4\\RngToRncClassic.xsl");
                            if (sht2.ok() && doct2.ok()) {
                                wxDateTime start = wxDateTime::Now();

                                for (int p = 0; p < 1; ++p) {
                                    sht2.transformToString(doct2);
                                }

                                wxTimeSpan took = wxDateTime::Now().Subtract(start);
                                wxLogError(wxString::Format(_("Took %s for transform performance test."), took.Format(wxT("%H:%M:%S")).c_str()));
                            }
                        }
                    }
                } // Performance test

#endif

//                val.read("test.rnc");
//                fprintf(stderr, "Validator parsing errors\n");
//                for (std::vector<xmlcpp::cXmlStructuredError>::const_iterator it = val.getStructuredErrors().begin(); it != val.getStructuredErrors().end(); ++it)
//                    wxLogError(wxString(it->message.c_str(), wxConvUTF8).Trim(true));
//                doc2.validate(val);
//                fprintf(stderr, "Validator errors\n");
//                for (std::vector<xmlcpp::cXmlStructuredError>::const_iterator it = val.getStructuredErrors().begin(); it != val.getStructuredErrors().end(); ++it)
//                    wxLogError(wxString(it->message.c_str(), wxConvUTF8).Trim(true));
//                val.clearStructuredErrors();
//

/*
                wxInputStream* filestream = inputfsfile->GetStream(); // Stream is destroyed by wxFSFile
                filestream->SeekI(0, wxFromEnd);

                int datasize = filestream->TellI();
                boost::shared_array<unsigned char> buf(new unsigned char[datasize]);
                filestream->SeekI(0);
                std::auto_ptr<wxMemoryOutputStream> buffer(new wxMemoryOutputStream(buf.get(), datasize));
                buffer->Write(*filestream);
                filestream->SeekI(0);
*/
//                if (doc2.read(buf.get(), inputfilestr.mb_str(wxConvUTF8), NULL, XML_PARSE_DTDLOAD)) {
                if (doc2.read(inputfilestr.mb_str(wxConvUTF8), NULL, XML_PARSE_DTDLOAD)) {
                    wxLogMessage(wxT("xml2 ok"));

                    wxLogMessage(wxT("%d"), doc2.validate(val, xmlcpp::cXmlValidator::OPT_DETERMINE_NODE_BY_XPATH));
                    fprintf(stderr, "Validation errors\n");
                    for (std::vector<xmlcpp::cXmlStructuredError>::const_iterator it = val.getStructuredErrors().begin(); it != val.getStructuredErrors().end(); ++it) {
                        fprintf(stderr, "Line: %d\n", it->line);
                        wxLogError(wxString(it->message.c_str(), wxConvUTF8).Trim(true));
                        wxLogError(wxT("Path: ") + wxString(it->getPath().c_str(), wxConvUTF8).Trim(true));
                    }
                    for (std::vector<std::string>::const_iterator it = val.getGenericErrors().begin(); it != val.getGenericErrors().end(); ++it)
                        wxLogError(wxString(it->c_str(), wxConvUTF8));
                    //doc2.write("string:test");
                    //fprintf(stderr, "%s", xmlcpp::cXmlOutputCallbackString::get("test").c_str());

                } else {
                    wxLogMessage(wxT("xml2 kaput"));
                    fprintf(stderr, "Document parsing errors\n");
                    for (std::vector<xmlcpp::cXmlStructuredError>::const_iterator it = doc2.getStructuredErrors().begin(); it != doc2.getStructuredErrors().end(); ++it)
                        wxLogError(wxString(it->message.c_str(), wxConvUTF8).Trim(true));
                    for (std::vector<std::string>::const_iterator it = doc2.getGenericErrors().begin(); it != doc2.getGenericErrors().end(); ++it)
                        wxLogError(wxString(it->c_str(), wxConvUTF8));
//                    xmlErrorPtr err = xmlGetLastError();
//                    wxLogError(wxT("%s in line %d. Error %d"), wxString(err->message, wxConvUTF8).c_str(), err->line, err->code);
                }
            }
}
#endif

//            if (doc.Load(inputfile.GetFullPath())) {
            if (doc.read(inputfilestr.mb_str(wxConvUTF8), NULL, XML_PARSE_DTDLOAD)) {
                cXmlNode root(doc.root());
                if (root(RAWXML_ROOT)) {
                    if (convert)
                        throw RCT3Exception(_("You cannot convert raw xml files"));

                    wxLogMessage(_("Processing raw xml file..."));

                    if (READ_RCT3_VALIDATE()) {
                        wxLogMessage(_("Validating..."));
                        boost::shared_ptr<cXmlValidator> val = cRawParser::Validator();
                        cXmlValidatorResult res = doc.validate(*val, cXmlValidator::OPT_DETERMINE_NODE_BY_XPATH, cXmlValidatorResult::VR_NONE);
                        if (res) {
                            /*
                            foreach(const std::string& ge, val->getGenericErrors())
                                wxLogError(wxString::FromUTF8(ge.c_str()));
                            foreach(const cXmlStructuredError& se, val->getStructuredErrors()) {
                                wxString message = wxString::Format(_("Line %d: %s"), se.line, wxString::FromUTF8(se.message.c_str()).c_str());
                                if (se.level == XML_ERR_NONE) {
                                    wxLogMessage(message);
                                } else if (se.level == XML_ERR_WARNING) {
                                    wxLogWarning(message);
                                } else {
                                    wxLogError(message);
                                }
                                if (verbose)
                                    wxLogVerbose(wxString::Format(_("   XPath: %s"), wxString::FromUTF8(se.getPath().c_str()).c_str()));
                            }
                            */
                            logXmlErrors(val.get(), verbose);
                        }
                        if (res(parser.Found("strict")?cXmlValidatorResult::VR_WARNING:cXmlValidatorResult::VR_ERROR)) {
                            wxLogMessage(_("...Failed!"));
                            return -1;
                        }
                        wxLogMessage(_("...Ok!"));
                    }

                    if (checkonly) {
                        wxLogMessage(_("Remember, for raw xml files, check only mode means only validation is done."));
                        return 0;
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
                    if (dryrun) {
                        fprintf(stderr, "\nDryrun results:\n");
                        for (std::vector<wxFileName>::const_iterator it = rovl.GetModifiedFiles().begin(); it != rovl.GetModifiedFiles().end(); ++it) {
#ifdef UNICODE
                            fprintf(stderr, "Modified: %s\n", it->GetFullPath().mb_str(wxConvFile).data());
#else
                            fprintf(stderr, "Modified: %s\n", it->GetFullPath().c_str());
#endif
                        }
                        for (std::vector<wxFileName>::const_iterator it = rovl.GetNewFiles().begin(); it != rovl.GetNewFiles().end(); ++it) {
#ifdef UNICODE
                            fprintf(stderr, "New: %s\n", it->GetFullPath().mb_str(wxConvFile).data());
#else
                            fprintf(stderr, "New: %s\n", it->GetFullPath().c_str());
#endif
                        }
                    }
                    return ret;
                } else {
                    c_scn.filename = inputfile.GetFullPath();
                    //c_scn.LoadXML(&doc);
                    c_scn.Load();
                }
            } else {
                logXmlErrors(&doc, verbose);
                throw RCT3Exception(_("Error in xml file"));
            }
        } else if (inputfile.GetExt().Lower() == wxT("ms3d")) {
            boost::shared_ptr<c3DLoader> object = c3DLoader::LoadFile(inputfile.GetFullPath().c_str());
            if (!object.get()) {
                wxLogError(_("Failed to load input file."));
                return -1;
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

            for (int i = 0; i < object->GetObjectCount(); ++i) {
                if (object->IsObjectValid(i)) {
                    cXmlNode geom("geomobj");
                    geom.prop("name", object->GetObjectName(i).utf8_str());
                    geom.prop("ftx", "???");
                    geom.prop("txs", "SIOpaque");
                    if (object->GetObjectVertex2(i, 0).bone[0] >= 0) {
                        geom.prop("bone", object->GetBone(object->GetObjectVertex2(i, 0).bone[0]).m_name.utf8_str());
                    }
                    bsh.appendChildren(geom);
                }
            }
            for (int i = 0; i < object->GetBoneCount(); ++i) {
                cXmlNode bone("bone");
                bone.prop("name", object->GetBone(i).m_name.utf8_str());
                if (!object->GetBone(i).m_parent.IsEmpty()) {
                    bone.prop("parent", object->GetBone(i).m_parent.utf8_str());
                }
                MATRIX p = object->GetBone(i).m_pos[0];
                bone.newChild("pos1", boost::str(boost::format("%f %f %f %f   %f %f %f %f   %f %f %f %f   %f %f %f %f") % p._11 % p._12 % p._13 % p._14 %
                    p._21 % p._22 % p._23 % p._24 % p._31 % p._32 % p._33 % p._34 % p._41 % p._42 % p._43 % p._44).c_str());
                p = object->GetBone(i).m_pos[1];
                bone.newChild("pos2", boost::str(boost::format("%f %f %f %f   %f %f %f %f   %f %f %f %f   %f %f %f %f") % p._11 % p._12 % p._13 % p._14 %
                    p._21 % p._22 % p._23 % p._24 % p._31 % p._32 % p._33 % p._34 % p._41 % p._42 % p._43 % p._44).c_str());
                bsh.appendChildren(bone);
            }
            root.appendChildren(bsh);

            cXmlNode ban("ban");
            ban.prop("name", inputfile.GetName().utf8_str());
            const c3DAnimation& an = object->GetAnimations().begin()->second;

            typedef pair<wxString, c3DAnimBone> bonepair;
            foreach(const bonepair& bp, an.m_bones) {
                cXmlNode bone("bone");
                bone.prop("name", bp.second.m_name.utf8_str());
                for (int f = 0; f < bp.second.m_translations.size(); ++f) {
                    txyz t = bp.second.m_translations[f];
                    cXmlNode trans("translate", boost::str(boost::format("%f %f %f") % t.X % t.Y % t.Z).c_str());
                    trans.prop("time", boost::str(boost::format("%f") % t.Time).c_str());
                    bone.appendChildren(trans);
                }
                for (int f = 0; f < bp.second.m_rotations.size(); ++f) {
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
            throw RCT3Exception(_("Cannot dryrun or install non-raw xml file."));

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
        wxLogMessage(_("Error in ovl creation: %s"), wxString(e.what(), wxConvLocal).c_str());
        ret = -1;
    } catch (std::exception& e) {
        wxLogMessage(_("General error: %s"), wxString(e.what(), wxConvLocal).c_str());
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
    wxFileName app = wxString(wxArgv[0]);
    if (!app.IsAbsolute())
        app.MakeAbsolute();
    /*
    wxString appenv = wxT("MAGICK_CONFIGURE_PATH=") + app.GetPathWithSep();
    putenv(appenv.mb_str(wxConvLocal));
    */
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
        { wxCMD_LINE_SWITCH, "q", "quiet", "in quiet mode only warnings and errors are shown" },
        { wxCMD_LINE_SWITCH, "s", "silent", "in silent mode only errors are shown" },
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

