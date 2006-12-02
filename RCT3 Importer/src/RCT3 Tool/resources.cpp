#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include <wx/filesys.h>
#include <wx/fs_mem.h>

//#include "crystal\icons\filenew_16x16.png.h"

#include "bmy.pal.h"

void InitResources()
{

    // Check for memory FS. If not present, load the handler:
    {
        wxMemoryFSHandler::AddFile(wxT("resources/dummy_file"), wxT("dummy one"));
        wxFileSystem fsys;
        wxFSFile *f = fsys.OpenFile(wxT("memory:resources/dummy_file"));
        wxMemoryFSHandler::RemoveFile(wxT("resources/dummy_file"));
        if (f) delete f;
        else wxFileSystem::AddHandler(new wxMemoryFSHandler);
    }

    //wxMemoryFSHandler::AddFile(wxT("resources/filenew_16x16.png"), filenew_16x16_png, sizeof(filenew_16x16_png));
}

const unsigned char* GetResource_BMYPal() {
    return (const unsigned char*) &bmy_pal;
}
