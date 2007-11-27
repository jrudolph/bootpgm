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

void command::invoke(IO &io,char *args)
{
	if (pObject)
	{
		// ugly hack to make member pointer working as needed...
		Main *p=(Main*)pObject;
		void(Main::*f)(IO&,char*)=*reinterpret_cast<void(Main::**)(IO&,char*)>(&func);
		(p->*f)(io,args);
	}
	else
		func(io,args);
}


void Main::showCmds(IO &io,char *args)
{
	io.println("Available commands:");
	Indenter i(io);
	for (int i=0;i<funcc;i++)
	{
		command &c=commands[i];
		char buffer[100];
		_snprintf(buffer,100,"%-20s | %-50s",c.name,c.description ? c.description : "");
		io.println(buffer);
	}
}

void Main::showArgs(IO &io,char *args)
{
	io.println("Commandline arguments:");
	Indenter i(io);
	for (int i=0;i<argc;i++)
	{
		char buffer[100];
		_snprintf(buffer,100,"[%d] %-70s",i,(char*)argv[i]);
		io.println(buffer);
	}
}

void Main::help(IO &io,char *args)
{
	if (!*args)
	{
		help(io," help");
		return;
	}

	command *cmd = findCommand(&args[1]);
	if (!cmd)
	{
		io.println("Command not found");
		return;
	}

	io.println(cmd->help ? cmd->help : "No help entry available");
}

command *Main::findCommand(char *name)
{
	for(int i=0;i<funcc;i++)
		if (strstr(name,commands[i].name)==name)
			return &commands[i];

	return 0;
}

Main::Main(IO &io,int argc,char** argv):io(io),funcc(0),argv(argv),argc(argc)
{
	addCommand("cmds",make_dg(this,&Main::showCmds)
		,"Show all available commands"
		,"Usage: cmds\nShow all available commands");
	addCommand("showArgs",make_dg(this,&Main::showArgs)
		,"Show the arguments of the program"
		,"Usage: showArgs\nShow the arguments of the program");
	addCommand("help",make_dg(this,&Main::help)
		,"Show the help entry for a command"
		,"Usage: help <command>\nShow help entries for command if available");

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

	char buffer[100];
	buffer[0]=0;

	while (strcmp(buffer,"exit")!=0)
	{
		io.print("rpl> ");
		io.readln(buffer,100);

		command *cmd=findCommand(buffer);
		if (cmd)
			cmd->invoke(io,buffer + strlen(cmd->name));
		else
			io.println("Command not found");
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

void Main::addCommand(char *name,invokeFunc func,char *desc,char *help,void*pObject)
{
	if (funcc>=maxFuncs)
	{
		io.println("Es kann kein neues Kommando hinzugefügt werden");
		return;
	}

	command c;
	c.func=func;
	c.name=name;
	c.description=desc;
	c.help=help;
	c.pObject=pObject;

	commands[funcc]=c;

	funcc++;
}

void Main::addCommand(char *name,deleg dg,char *desc,char *help)
{
	addCommand(name,(invokeFunc)dg.func,desc,help,dg.object);
}


void* __cdecl operator new(size_t sz) {
	IO& io=mainSingleton->get_io();
	void* m = io.malloc((unsigned int)sz);

	if(!m) 
		io.println("out of memory");

	return m;
}

void __cdecl operator delete(void* m) {
	mainSingleton->get_io().free(m);
}
