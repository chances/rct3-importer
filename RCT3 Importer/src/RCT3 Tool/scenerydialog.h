/*	RCT3 Tool
	Copyright 2005 Jonathan Wilson

	This file is part of the RCT3 Tool
	The RCT3 File Importer Program is free software; you can redistribute it and/or modify it under
	the terms of the GNU General Public License as published by the Free
	Software Foundation; either version 2, or (at your option) any later
	version. See the file COPYING for more details.

	Small modifications made by Klinn - May 2005 - search for 'by KLN'
	1) Added crude support for recolorable objects:
			- controls added to IDD_CREATESCENERY dialog box
			- 'Recolor' parameter added to SaveOVL function
		 Note that the object texture bitmap's colors will be replaced when recolored in-game:
			- color 0 = not known, avoid use
			- colors 1-85 = re-color #1, shades from light to dark
			- colors 86-170 = re-color #2, light to dark
			- colors 171-255 = re-color #3, light to dark
		 A suitable custom indexed color table should be created and used when converting
		 an RGB-format image to indexed 256-color mode, so that the results can be more
		 easily visualized. Frontier appears to have used shades of blue for 1-85, magenta
		 for 86-170, and yellow for 171-255. Suggest leaving color 0 = white.
	2) Changed Texture Style from an edit box to a combobox.
	3) Added confirmation MessageBox after theme is successfully installed.
	4) Revised the GUISkinItemPos struct, swapped the 'Left' and 'Top' members, and
	swapped the 'Right' and 'Bottom' members, so that game will pick up correct coords.

	Changes by Belgabor:
    - Lots

*/

#ifndef SCENERYDIALOG_H_INCLUDED
#define SCENERYDIALOG_H_INCLUDED

#include "win_predefine.h"

BOOL CALLBACK SceneryDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);

#endif // SCENERYDIALOG_H_INCLUDED
