/* The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 * 
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 * License for the specific language governing rights and limitations
 * under the License.
 * 
 * The Initial Developer of the Original Code is Johannes Rudolph.
 * Portions created by the Initial Developer are Copyright (C) 2006
 * the Initial Developer. All Rights Reserved.
 * 
 * Contributor(s): 
 *    Johannes Rudolph <johannes_rudolph@gmx.de>
 */ 

#include "StdAfx.h"
#include "IO.h"

IO::IO(void):indent(0),current(0)
{
}

IO::~IO(void)
{
}
void IO::print(char *buffer)
{
	if (current==0)
	{
		for (int i=0;i<indent;i++)
			internalPrint(" ");
		current+=indent;
	}
	current+=strlen(buffer);
	internalPrint(buffer);
}

void IO::println(char *buffer)
{
	print(buffer);
	print("\n");
	current=0;
}

void IO::readln(char *buffer,unsigned int length)
{
	debugout("readln gestartet");
	//NT::DbgPrint("readln startet\n");
	unsigned int curlength=0;
	char curChar[2];
	curChar[0]=0;
	curChar[1]=0;
	while (curlength<length&&curChar[0]!='\n')
	{
		curChar[0]=getChar();
		if (curChar[0]!=8) //BACKSPACE
		{
			buffer[curlength]=curChar[0];
			handleCharEcho(curChar[0],buffer,curlength);
			curlength++;
		}
		else
		{
			if (curlength>0)
			{
				curlength--;
				handleCharEcho(curChar[0],buffer,curlength);
			}
		}
	}
	buffer[curlength-1]=0;

	debugout("readln beendet");
	//NT::DbgPrint("readln beendet\n");
}
wchar_t *IO::char2wchar(char *buffer)
{
	unsigned int size=strlen(buffer)+1;
	wchar_t *buffer2=(wchar_t*)malloc(sizeof(wchar_t)*size);
	mbstowcs(buffer2,(char*)buffer,size);
	buffer2[size-1]=0;
	
	return buffer2;
}
NT::UNICODE_STRING IO::getUnicodeString(char *buffer)
{
	wchar_t *buffer2=char2wchar(buffer);
	
	NT::UNICODE_STRING UnicodeFilespec;
	RtlInitUnicodeString(&UnicodeFilespec, buffer2);

	return UnicodeFilespec;
}

void IO::handleStatus(NT::NTSTATUS status,char *place,char *file,char *line,bool onlyWhenDebugging){
#ifndef DEBUGGING
	if (!onlyWhenDebugging)
#endif
	if (status!=STATUS_SUCCESS)
	{
		print("[error]");
		print(file);
		print("(");
		print(line);
		println("):");
		print("[error] Fehler: ");
		println(place);
		NT::DbgPrint("Fehler (Datei: %s \tZeile: %s): %s, %d\n",file,line,place,status);
	}
	else
	{
		print("[success] Erfolg: ");
		println(place);
		NT::DbgPrint("Erfolg: %s\n",place);
	}
}
inline void IO::debugout(char *string)
{
#ifdef DEBUGGING
	print("[debug] ");
	println(string);
	NT::DbgPrint("[bootpgm] %s\n",string);*/
#endif
}

void IO::setIndent(unsigned char indent)
{
	this->indent=indent;
}
unsigned char IO::getIndent()
{
	return indent;
}