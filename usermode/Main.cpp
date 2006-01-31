#include "StdAfx.h"
#include "Main.h"

Main::Main(IO &io):io(io),funcc(0)
{
}

Main::~Main(void)
{
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
		
		int i;
		for(i=0;i<funcc;i++)
		{
			if (strstr(buffer,commands[i].name)!=0)
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
