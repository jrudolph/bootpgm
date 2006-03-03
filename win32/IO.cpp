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
wchar_t *IO::char2wchar(char *buffer)
{
	unsigned int size=strlen(buffer)+1;
	wchar_t *buffer2=(wchar_t*)malloc(sizeof(wchar_t)*size);
	mbstowcs(buffer2,(char*)buffer,size);
	
	return buffer2;
}
NT::UNICODE_STRING IO::getUnicodeString(char *buffer)
{
	wchar_t *buffer2=char2wchar(buffer);
	
	NT::UNICODE_STRING UnicodeFilespec;
	RtlInitUnicodeString(&UnicodeFilespec, buffer2);

	return UnicodeFilespec;
}

void IO::handleStatus(NT::NTSTATUS status,char *place,char *file,char *line){
	if (status!=STATUS_SUCCESS)
	{
		print(file);
		print("(");
		print(line);
		println("):");
		print("\tFehler: ");
		println(place);
	}
	else
	{
		print("Erfolg: ");
		println(place);
	}
}
