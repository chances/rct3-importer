/*	RCT3 libOVL
	Copyright 2005 Jonathan Wilson

	This file is part of the RCT3 libOVL
	The RCT3 libOVL library is free software; you can redistribute it and/or modify it under
	the terms of the GNU General Public License as published by the Free
	Software Foundation; either version 2, or (at your option) any later
	version. See the file COPYING for more details.
*/

#include "CommonOVL.h"
unsigned long CurrentFile;
OvlFile OpenFiles[2];

LIBOVL_API unsigned int LibOVLCheckVersion(unsigned int version) {
    if (version == LIBOVL_VERSION)
        return 0;
    else
        return LIBOVL_VERSION;
}

////////////////////////////////////////////////////////////////////////////////
//
//  AddInfo_FileReference
//
////////////////////////////////////////////////////////////////////////////////

LIBOVL_API void AddInfo_FileReference(char* reference, unsigned long CurrentFile)
{
	unsigned long i=OpenFiles[CurrentFile].ReferenceCount;
	i++;
	OpenFiles[CurrentFile].References = (char **)realloc(OpenFiles[CurrentFile].References,i);
	char *str;
	str = new char[strlen(reference)+1];
	strcpy(str,reference);
	OpenFiles[CurrentFile].References[i] = str;
	OpenFiles[CurrentFile].ReferenceCount=i;
}


////////////////////////////////////////////////////////////////////////////////
//
//  DeleteInfo_FileReferences
//
////////////////////////////////////////////////////////////////////////////////

LIBOVL_API void DeleteInfo_FileReferences(unsigned long CurrentFile)
{
	if (OpenFiles[CurrentFile].ReferenceCount)
	{
		for(unsigned long i=0;i<OpenFiles[CurrentFile].ReferenceCount;i++)
		{
			if ((*OpenFiles[CurrentFile].References[i])!=0)
				delete OpenFiles[CurrentFile].References[i];
		}
		if( (*(OpenFiles[CurrentFile].References))!=0)
			delete OpenFiles[CurrentFile].References;
	}

}


////////////////////////////////////////////////////////////////////////////////
//
//  AddInfo_Resource
//
////////////////////////////////////////////////////////////////////////////////

//Note resource items actually do not allocate space, they just use the pointer
LIBOVL_API void AddInfo_Resource(unsigned long length, unsigned long* data)
{
	long curItem=-1;
	libOVL_Resource *t = new libOVL_Resource;
	ResourceItems.push_back(t);
	curItem = ResourceItems.size()-1;

	ResourceItems[curItem]->data = data;
	ResourceItems[curItem]->length=length;
}


////////////////////////////////////////////////////////////////////////////////
//
//  DeleteInfo_Resources
//
////////////////////////////////////////////////////////////////////////////////

LIBOVL_API void DeleteInfo_Resources()
{
	for (unsigned long i = 0;i < ResourceItems.size();i++)
	{
		delete ResourceItems[i];
	}

	ResourceItems.clear();
}


////////////////////////////////////////////////////////////////////////////////
//
//  DoRelocationForSaving
//
////////////////////////////////////////////////////////////////////////////////

unsigned long DoRelocationForSaving(unsigned long *reloc)
{
	for (unsigned long i = 0;i < 9;i++)
	{
		for (unsigned long j = 0; j < OpenFiles[CurrentFile].Types[i].count;j++)
		{
			if ((reloc >= (unsigned long *)OpenFiles[CurrentFile].Types[i].dataptr[j].data) && (reloc <= (unsigned long *)(((unsigned char *)OpenFiles[CurrentFile].Types[i].dataptr[j].data)+OpenFiles[CurrentFile].Types[i].dataptr[j].size)))
			{
				return ((unsigned long)reloc - (unsigned long)(OpenFiles[CurrentFile].Types[i].dataptr[j].data)+(unsigned long)(OpenFiles[CurrentFile].Types[i].dataptr[j].reloffset));
			}
		}
	}

	//Added code : IF the file is .Common OVL return
	if (CurrentFile==0)
		return 0xFFFFFFFF;


	for (unsigned long i = 0;i < 9;i++)
	{
		for (unsigned long j = 0; j < OpenFiles[CurrentFile-1].Types[i].count;j++)
		{
			if ((reloc >= (unsigned long *)OpenFiles[CurrentFile-1].Types[i].dataptr[j].data) && (reloc <= (unsigned long *)(((unsigned char *)OpenFiles[CurrentFile-1].Types[i].dataptr[j].data)+OpenFiles[CurrentFile-1].Types[i].dataptr[j].size)))
			{
				return ((unsigned long)reloc - (unsigned long)(OpenFiles[CurrentFile-1].Types[i].dataptr[j].data)+(unsigned long)(OpenFiles[CurrentFile-1].Types[i].dataptr[j].reloffset));
			}
		}
	}
	return 0xFFFFFFFF;
}
