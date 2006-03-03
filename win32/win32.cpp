#include "stdafx.h"
#include "io.h"
#include "main.h"

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
	void *malloc(unsigned int length)
	{
		return new char[length];
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

int __cdecl main(int argc, _TCHAR* argv[])
{
	UsermodeIO io;
	Main main(io,argc,(char**)argv);

	main.addCommand("test",wurst);
	main.addCommand("findAddress",findAddress);

	main.addCommand("setComputername",setCompnameFromFile);

	main.run();
}