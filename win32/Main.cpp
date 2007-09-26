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
#include "Main.h"

Main *mainSingleton=NULL;

char **split_args(IO &io,wchar_t* cmdLine,int length,int *pargc)
{
	int argCount=0;
	for (int i=0;i<length;i++)
		if (*(cmdLine+i)==L' ')
			argCount++;
	argCount++;

	char **argv=(char**)io.malloc(4*argCount);
	int last=0;
	int argNo=0;
	for (int i=0;i<=length;i++)
	{
		wchar_t ch= i<length ? *(cmdLine+i) : 0;
		if (ch==L' '||ch==0)
		{
			int argLength=i-last;
			char *arg=(char*)io.malloc(argLength+1);
			wcstombs(arg,cmdLine+last,argLength);
			arg[argLength]=0;
			argv[argNo]=arg;

			argNo++;
			last=i+1;
		}
	}
	*pargc=argCount;
	return argv;
}

void showCmds(IO &io,char *args)
{
	if (mainSingleton!=NULL)
	{
		io.println("Available commands:");
		Indenter i(io);
		for (int i=0;i<mainSingleton->funcc;i++)
		{
			io.println(mainSingleton->commands[i].name);
		}
	}
}

void showArgs(IO &io,char *args)
{
	if (mainSingleton!=NULL)
	{
		io.println("Commandline arguments:");
		Indenter i(io);
		for (int i=0;i<mainSingleton->argc;i++)
		{
			io.println((char*)mainSingleton->argv[i]);
		}
	}
}

Main::Main(IO &io,int argc,char** argv):io(io),funcc(0),argv(argv),argc(argc)
{
	addCommand("showCmds",showCmds);
	addCommand("showArgs",showArgs);
	if (mainSingleton!=NULL)
	{
		io.println("Error: Main may be instantiated only once");
		return;
	}
	else
	{
		mainSingleton=this;
	}
}

Main::~Main(void)
{
	mainSingleton=NULL;
}

void Main::run()
{
	showSplashScreen();
	rpl();
}
void Main::rpl()
{
	io.println("Starting RPL (Read-Print-Loop) Type \"exit\" to stop.");

	char *buffer=(char*)io.malloc(100);
	buffer[0]=0;

	while (strcmp(buffer,"exit")!=0)
	{
		io.print("rpl> ");
		io.readln(buffer,100);
		io.debugout("reaching command checking");
		int i;
		for(i=0;i<funcc;i++)
		{
			char *cmd=commands[i].name;
			unsigned int length=strlen(cmd);
			
			if (strlen(buffer)>=length&&strstr(buffer,cmd)!=0&&(buffer[length]==0||buffer[length]==' '))
			{
				commands[i].func(io,buffer+strlen(commands[i].name));
				break;
			}
		}

		if (i>=funcc)
		{
			io.print("Befehl konnte nicht gefunden werden: ");
			io.println(buffer);
		}
	}

	io.println("Exiting RPL");
}
void Main::showSplashScreen()
{
	io.println("Boot-time Computername Changer by Johannes Rudolph");
	io.println("v0.1");
	io.print("IO: ");
	io.println(io.getVersion());
}

void Main::addCommand(char *name,invokeFunc func)
{
	if (funcc>=maxFuncs)
	{
		io.println("Es kann kein neues Kommando hinzugefügt werden");
		return;
	}

	command c;
	c.func=func;
	c.name=name;

	commands[funcc]=c;

	funcc++;
}
