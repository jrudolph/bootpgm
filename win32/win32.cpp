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

#include "stdafx.h"
#include "io.h"
#include "main.h"

class UsermodeIO:public IO{
public:
	char getChar()
	{
		return getchar();
	}
	void internalPrint(char *buffer)
	{
		printf(buffer);
	}
	char *getVersion()
	{
		return "Usermode IO";
	}
	void *malloc(unsigned int length)
	{
		return ::malloc(length);
	}
	void free(void *buffer)
	{
		::free(buffer);
	}
	void handleCharEcho(char ch,char *buffer,unsigned int length)
	{
		// no echo nessecary because getchar() echos
		/*char b[2];
		b[0]=ch;
		b[1]=0;
		if (ch!=8)
			print(b);*/
	}
};

void wurst(IO &io,char *cmd)
{
	io.println(cmd);
}

void splitArgs(IO &io,char *args){
	if (strlen(args)==0)
		return;

	int argc;
	UNICODE_STRING str;
	wchar_t *wstr;
	char buffer[100];
	wstr=(wchar_t*)buffer;
	mbstowcs(wstr,args,50);
	NT::RtlInitUnicodeString(&str,wstr);
	str.Length-=2;
	char **argv=split_args(io,str.Buffer,str.Length/2,&argc);
	for (int i=0;i<argc;i++)
		io.println(argv[i]);
}

void setCompnameFromFile(IO &io,char *args);
void initFileTools(IO *io);

void testStringFunctions(IO &io,char *args);
void testMatcher(IO &io,char *args);

void testRegKey(IO &io,char *args);

int __cdecl main(int argc, _TCHAR* argv[])
{
	UsermodeIO io;
	
	Main main(io,argc,(char**)argv);
	
	initFileTools(&io);

	main.addCommand("test",wurst);

	main.addCommand("setComputername",setCompnameFromFile);
	main.addCommand("testString",testStringFunctions);

	main.addCommand("testMatcher",testMatcher);
	main.addCommand("splitArgs",splitArgs);

	main.addCommand("regTest",testRegKey);

	main.run();
}