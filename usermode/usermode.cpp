#include "stdafx.h"
#include "io.h"
#include "main.h"
#include <stdio.h>

class UsermodeIO:public IO{
public:
	char getChar()
	{
		return getchar();
	}
	void print(char *buffer)
	{
		printf(buffer);
	}
	char *getVersion()
	{
		return "Usermode IO";
	}
};

int __cdecl main(int argc, _TCHAR* argv[])
{
	UsermodeIO io;
	Main main(io);
	main.run();
}