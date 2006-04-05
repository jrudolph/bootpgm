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
	void internPrint(char *buffer)
	{
		printf(buffer);
	}
	char *getVersion()
	{
		return "Usermode IO";
	}
	void *malloc(unsigned int length)
	{
		return new char[length];
	}
	void free(void *buffer)
	{
		delete buffer;
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

void findAddress(IO &io,char *args)
{
	int size=strlen(args);
	if (size<2)
	{
		io.println("need at least 1 argument");
		return;
	}
	args++;

	io.print("Trying to find ");
	io.print(args);
	io.println("...");

	HANDLE h=0;

	NTSTATUS Status=LdrGetDllHandle(0,0,&io.getUnicodeString(args),&h);
	CHECK_STATUS(Status,findAddress)
	char *buffer=(char*)io.malloc(100);
	_snprintf(buffer,99,"%x 0x%x %p",Status,h,h);

	io.println(buffer);
}

void setCompnameFromFile(IO &io,char *args);
void initFileTools(IO *io);

void testStringFunctions(IO &io,char *args);
void testMatcher(IO &io,char *args);

int __cdecl main(int argc, _TCHAR* argv[])
{
	UsermodeIO io;
	Main main(io,argc,(char**)argv);

	initFileTools(&io);

	main.addCommand("test",wurst);
	main.addCommand("findAddress",findAddress);

	main.addCommand("setComputername",setCompnameFromFile);
	main.addCommand("testString",testStringFunctions);

	main.addCommand("testMatcher",testMatcher);

	main.run();
}