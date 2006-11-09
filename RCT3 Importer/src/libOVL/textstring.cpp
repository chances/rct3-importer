/*	RCT3 libOVL
	Copyright 2006 Jonathan Wilson

	This file is part of the RCT3 libOVL
	The RCT3 libOVL library is free software; you can redistribute it and/or modify it under
	the terms of the GNU General Public License as published by the Free
	Software Foundation; either version 2, or (at your option) any later
	version. See the file COPYING for more details.
*/

#include "Common.h"
#include "CommonOVL.h"
#include "StyleOVL.h"
#include "textstring.h"
std::vector<TextString *> TextString::TextStrings;
int TextString::LoaderNumber;

unsigned long TextString::ComputeSize()
{
 	unsigned long textstringsize = 0;
   	for (unsigned int i = 0;i < TextStrings.size();i++)
	{
		AddStringSize(TextStrings[i]->name);
		textstringsize += (strlen(TextStrings[i]->string)+1)*2;
		loadercount++;
		symbolcount++;
	}
	return textstringsize;
}

TextString::TextString(char *Name,char *String)
{
    name = new char[strlen(Name)+1];;
    strcpy(name,Name);
    string = new char[strlen(String)+1];;
    strcpy(string,String);
}

TextString::~TextString()
{
    delete name;
    delete string;
}

void TextString::AddData(unsigned long *data)
{
	for (unsigned int i = 0;i < TextStrings.size();i++)
	{
	    AddLoader(data,LoaderNumber,0,TextStrings[i]->name,":txt",true);
		mbstowcs((wchar_t *)data,TextStrings[i]->string,strlen(TextStrings[i]->string)+1);
		// Ugly pointer magic alarm!
		wchar_t **dataptr = (wchar_t **) &data;
		*dataptr += strlen(TextStrings[i]->string)+1;
	}
}

void TextString::WriteExtraData(FILE *f)
{
    int t1 = 4;
	int t2 = 1;
	for (unsigned int i = 0;i < TextStrings.size();i++)
	{
		fwrite(&t1,4,1,f);
		fwrite(&t2,4,1,f);
	}
}

void TextString::WriteFileHeader(FILE *f)
{
	unsigned short len1 = 4;
	char *str1 = "FGDK";
	unsigned short len2 = 4;
	char *str2 = "Text";
	unsigned long type = 2;
	unsigned short len3 = 3;
	char *str3 = "txt";
	fwrite(&len1,sizeof(len1),1,f);
	fwrite(str1,len1,1,f);
	fwrite(&len2,sizeof(len2),1,f);
	fwrite(str2,len2,1,f);
	fwrite(&type,sizeof(type),1,f);
	fwrite(&len3,sizeof(len3),1,f);
	fwrite(str3,len3,1,f);
}
