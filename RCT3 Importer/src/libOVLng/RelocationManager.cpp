///////////////////////////////////////////////////////////////////////////////
//
// New OVL creation library
// Manager class for relocations
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
// Based on libOVL by Jonathan Wilson
//
///////////////////////////////////////////////////////////////////////////////


#include "RelocationManager.h"

#include "OVLDebug.h"

void ovlRelocationManager::Init(OvlInfo* info) {
    m_info = info;
}

void ovlRelocationManager::AddRelocation(unsigned long* reloc) {
    m_relocations.push(reloc);
}

unsigned long ovlRelocationManager::DoRelocationForSaving(unsigned long *reloc) {
	for (unsigned long i = 0;i < 9;i++) {
		for (unsigned long j = 0; j < m_info->OpenFiles[m_info->CurrentFile].Types[i].count;j++) {
			if ((reloc >= (unsigned long *)m_info->OpenFiles[m_info->CurrentFile].Types[i].dataptr[j].data)
			    && (reloc <= (unsigned long *)(((unsigned char *)m_info->OpenFiles[m_info->CurrentFile].Types[i].dataptr[j].data)+m_info->OpenFiles[m_info->CurrentFile].Types[i].dataptr[j].size))) {
				return ((unsigned long)reloc
                        - (unsigned long)(m_info->OpenFiles[m_info->CurrentFile].Types[i].dataptr[j].data)
                        + (unsigned long)(m_info->OpenFiles[m_info->CurrentFile].Types[i].dataptr[j].reloffset)
                       );
			}
		}
	}

	//Added code : IF the file is .Common OVL return
	if (m_info->CurrentFile == OVL_COMMON)
		return 0xFFFFFFFF;


	for (unsigned long i = 0;i < 9;i++) {
		for (unsigned long j = 0; j < m_info->OpenFiles[m_info->CurrentFile-1].Types[i].count;j++) {
			if ((reloc >= (unsigned long *)m_info->OpenFiles[m_info->CurrentFile-1].Types[i].dataptr[j].data)
			    && (reloc <= (unsigned long *)(((unsigned char *)m_info->OpenFiles[m_info->CurrentFile-1].Types[i].dataptr[j].data)+m_info->OpenFiles[m_info->CurrentFile-1].Types[i].dataptr[j].size))) {
				return ((unsigned long)reloc
				        - (unsigned long)(m_info->OpenFiles[m_info->CurrentFile-1].Types[i].dataptr[j].data)
				        + (unsigned long)(m_info->OpenFiles[m_info->CurrentFile-1].Types[i].dataptr[j].reloffset)
                       );
			}
		}
	}
	return 0xFFFFFFFF;
}

void ovlRelocationManager::Make() {
    while (m_relocations.empty() == false) {
        unsigned long *relocation = m_relocations.front();
        m_relocations.pop();
        unsigned long *reloc = *(unsigned long **)relocation;
        if (reloc != 0) {
            unsigned long fixup = DoRelocationForSaving(reloc);
            if (fixup == 0xFFFFFFFF) {
                fixup = 0;
            }
            *relocation = fixup;
            unsigned long fixup2 = DoRelocationForSaving(relocation);
            DUMP_RELOCATION(f,"relocation %x, reloc %x, fixup %x, fixup2 %x\n",relocation,reloc,fixup,fixup2);
            m_fixups.push(fixup2);
        }
    }
}
