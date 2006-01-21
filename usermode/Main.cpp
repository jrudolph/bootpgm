#include "StdAfx.h"
#include "Main.h"

Main::Main(IO &io):io(io)
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
		io.print("Read: ");
		io.println(buffer);
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
