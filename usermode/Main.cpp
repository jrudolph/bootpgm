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
}
void Main::showSplashScreen()
{
	io.println("Boot-time Computername Changer by Johannes Rudolph");
	io.println("v0.1");
	io.print("IO: ");
	io.println(io.getVersion());
}
