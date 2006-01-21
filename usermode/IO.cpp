#include "StdAfx.h"
#include "IO.h"

IO::IO(void)
{
}

IO::~IO(void)
{
}
void IO::println(char *buffer)
{
	print(buffer);
	print("\n");
}

void IO::readln(char *buffer,unsigned int length)
{
	//NT::DbgPrint("readln startet\n");
	unsigned int curlength=0;
	char curChar[2];
	curChar[0]=0;
	curChar[1]=0;
	while (curlength<length&&curChar[0]!='\n')
	{
		curChar[0]=getChar();
		buffer[curlength]=curChar[0];
		print(curChar);
		curlength++;
	}
	buffer[curlength-1]=0;
	//NT::DbgPrint("readln beendet\n");
}
