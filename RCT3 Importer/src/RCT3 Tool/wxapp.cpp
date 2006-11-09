#include "wx_pch.h"

#include "wxapp.h"

IMPLEMENT_APP_NO_MAIN(ToolApp)

bool ToolApp::OnInit()
{
	//(*AppInitialize
	bool wxsOK = true;
	::wxInitAllImageHandlers();

	return wxsOK;
}
