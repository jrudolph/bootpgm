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

/*
File: computername.cpp
functions to change the computer name in the Windows registry
*/

#include "stdafx.h"
#include "io.h"
#include "main.h"

WCHAR KeyNameBuffer[]        = L"\\Registry\\Machine\\SYSTEM\\CurrentControlSet\\Control\\ComputerName\\ComputerName";
WCHAR KeyNameBuffer2[]        = L"\\Registry\\Machine\\SYSTEM\\CurrentControlSet\\Control\\ComputerName\\ActiveComputerName";
WCHAR Tcpip[]        = L"\\Registry\\Machine\\SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters";
WCHAR ComputerNameBuffer[]    = L"ComputerName";

/*
Function: setRegistryValue

Parameters:
io: reference to <IO>-Controller
*/
void setRegistryValue(IO &io,WCHAR *keyName,WCHAR *valueName,WCHAR *value)
{
	Indenter i(io);
	UNICODE_STRING KeyName, ValueName;
	HANDLE SoftwareKeyHandle;
	ULONG Status;
	OBJECT_ATTRIBUTES ObjectAttributes;
	ULONG Disposition;

	//io.print("Schreibe Registry-Key ");

	//DbgBreakPoint();
	//
	// Open the Software key
	//
	NT::RtlInitUnicodeString(&KeyName,keyName);
	InitializeObjectAttributes(
		&ObjectAttributes,
		&KeyName,
		OBJ_CASE_INSENSITIVE,
		NULL,
		NULL);
	Status = ZwCreateKey(
		&SoftwareKeyHandle,
		KEY_ALL_ACCESS,
		&ObjectAttributes,
		0,
		NULL,
		REG_OPTION_NON_VOLATILE,
		&Disposition);

	CHECK_STATUS(Status,Öffnen des Schlüssels)

	NT::RtlInitUnicodeString(&ValueName,valueName);

	Status = ZwSetValueKey(
		SoftwareKeyHandle,
		&ValueName,
		0,
		REG_SZ,
		value,
		(wcslen( value )+1) * sizeof(WCHAR));

	CHECK_STATUSA(Status,Setzen des Schlüssels);

	Status = ZwClose(SoftwareKeyHandle);

	CHECK_STATUS(Status,Schließen des Schlüssels);
}

/*
Function: setComputerName
sets the computer name in the registry to the specified one

Parameters:
io - reference to the <IO>-Controller
computerName - the designated computer name as UNICODE string
*/
void setComputerName(IO &io,WCHAR *computerName)
{
	Indenter i(io);
	io.println("Setze Computernamen ");
    setRegistryValue(io,KeyNameBuffer,ComputerNameBuffer,computerName);
    setRegistryValue(io,KeyNameBuffer2,ComputerNameBuffer,computerName);
    setRegistryValue(io,Tcpip,L"Hostname",computerName);
    setRegistryValue(io,Tcpip,L"NV Hostname",computerName);
}
/*
Function: setComputerNameCmd
command line command for setting the computer name manually

Parameters:
io - <IO>-Controller
args - command line argument string
*/
void setComputerNameCmd(IO &io,char *args)
{
	Indenter i(io);
	if (strlen(args)<2)
	{
		io.println("Syntax: setComputerName <newComputerName>");
		return;
	}
	io.print("Setting Computer Name to: ");
	io.println(args+1);
	setComputerName(io,io.char2wchar(args+1));
}

#define RETURN_NULL_IF_STATUS_UNSUCCESSFULL if (Status!=STATUS_SUCCESS) return 0;

/*
Constant: whitespaces
defines whitespace character group, each char of this array
designates one white space character
*/
const char whitespaces[]=" \t\n\x0B\f\r";

/*
Function: isWhitespace
Helper function for regexp parser. Tests if c is white space.

Parameters:
c - character to test

Returns: 
true if character is white space as defined in <whitespaces>
*/
bool isWhitespace(char c)
{
	for (int i=0;i<sizeof(whitespaces);i++)
		if (whitespaces[i]==c)
			return true;

	return false;
}
/*
Function: isCapitalLetter
Helper function for regexp parser. Tests if c is a capital letter.

Parameters:
c - character to test
*/
bool isCapitalLetter(char c)
{
	return c>='A'&&c<='Z';
}
bool isDigit(char c)
{
	return c>='0'&&c<='9';
}
bool isSmallLetter(char c)
{
	return c>='a'&&c<='z';
}

bool isWordCharacter(char c)
{
	return isDigit(c)||isCapitalLetter(c)||isSmallLetter(c)||c=='-';
}
bool char_matcher(char c,char d)
{
	return c==d;
}
//const char pattern[]="Computername:\\s+(\\w+)";
/*
Constant: pattern
regular expression pattern which defines the place in a file
to read the computer name from.

The computer name will be the match for the first bracketed expression.

Example:
: <computername\\s+param=\"(\\w+)\"
will match *test* in *<computername param="test" />*
*/
const char pattern[]="<computername\\s+param=\"(\\w+)\"";

/*
Function: parseComputerNameFile
uses <pattern> to find a match in the buffer

Parameters:
io - <IO>-Controller
buffer - source buffer
length - length of source buffer

Returns:
a string containing the match or 0 otherwise
*/
char *parseComputerNameFile(IO &io,char *buffer,unsigned int length)
{
	int patternpos=0;
	int capture_start=-1;
	int capture_end=-1;
	void *matcher=0;
	char lastData=0;
	for (unsigned int i=0;i<length;i++)
	{
		bool matched=true;
		switch (pattern[patternpos])
		{
		case '\\':
			patternpos++;
			switch(pattern[patternpos])
			{
			case 's':
				matcher=isWhitespace;
				lastData=0;
				break;
			case 'w':
				matcher=isWordCharacter;
				lastData=0;
				break;
			}
			break;
		case '(':
			capture_start=i;
			patternpos++;
			matcher=0;
			continue;
		case ')':
			capture_end=i;
			patternpos++;
			matcher=0;
			continue;
		case '+':
			matched=true;
			while (matched)
			{
				if (lastData!=0)
					// use binary matcher
					matched=((bool(*)(char,char))matcher)(buffer[i],lastData);
				else
					// use unary matcher
					matched=((bool(*)(char))matcher)(buffer[i]);

				if (matched&&i<length)
					i++;
			}
			if (i>0)
				i--;
			patternpos++;
			continue;
		default:
			lastData=pattern[patternpos];
			matcher=char_matcher;
			break;
		}

		matched=false;
		if (lastData!=0)
			matched=((bool(*)(char,char))matcher)(buffer[i],lastData);
		else
			matched=((bool(*)(char))matcher)(buffer[i]);

		if (matched)
			patternpos++;
		else
			patternpos=0;
	}
	if (pattern[patternpos]==')')
		capture_end=length;

	if (capture_start!=-1&&capture_end!=-1)
	{
		int matchlength=capture_end-capture_start;
		char *returnBuffer=(char*)io.malloc(matchlength+1);
		memcpy(returnBuffer,buffer+capture_start,matchlength);
		returnBuffer[matchlength]=0;
		return returnBuffer;
	}
	else
		return 0;
}

void testMatcher(IO &io,char *args)
{
	char string[]="Computername:      ";
	char *return0=parseComputerNameFile(io,string,sizeof(string));
}

/*
Function: readComputerNameFromFile
reads the computer name from the specified file, uses <pattern> to 
find computer name in it

Parameters:
io - <IO>-Controller
fileName - reads computer name from this file

Returns:
UNICODE string containing the new computer name, 0 in case of error or if 
the name couldn't be found
*/
wchar_t *readComputerNameFromFile(IO &io,wchar_t *fileName)
{
	Indenter i(io);
	NTSTATUS Status;
	UNICODE_STRING UnicodeFilespec;
	OBJECT_ATTRIBUTES ObjectAttributes;
	HANDLE FileHandle;
	IO_STATUS_BLOCK Iosb;
	char *buffer;
	PWCHAR buffer2;
	ULONG converted;

	RtlInitUnicodeString(&UnicodeFilespec,fileName);

	InitializeObjectAttributes(
		&ObjectAttributes,           // ptr to structure
		&UnicodeFilespec,            // ptr to file spec
		OBJ_CASE_INSENSITIVE,        // attributes
		NULL,                        // root directory handle
		NULL );                      // ptr to security descriptor

	Status = ZwCreateFile(
		&FileHandle,					// returned file handle
		(GENERIC_READ | SYNCHRONIZE),   // desired access
		&ObjectAttributes,              // ptr to object attributes
		&Iosb,                          // ptr to I/O status block
		0,                              // allocation size
		FILE_ATTRIBUTE_NORMAL,          // file attributes
		0,                              // share access
		FILE_OPEN,						// create disposition
		FILE_SYNCHRONOUS_IO_NONALERT,   // create options
		NULL,                           // ptr to extended attributes
		0);                             // length of ea buffer

	CHECK_STATUSA(Status,Öffnen der Computernamensdatei)
	RETURN_NULL_IF_STATUS_UNSUCCESSFULL

	buffer = (char*)io.malloc(256);//RtlAllocateHeap( Heap, 0, 256 );
	Status = ZwReadFile(FileHandle,0,NULL,NULL,&Iosb,buffer,256,0,NULL);
	((char*)buffer)[Iosb.Information]=0;

	CHECK_STATUSA(Status,Lesen des Computernamens);
	RETURN_NULL_IF_STATUS_UNSUCCESSFULL

	io.print("Trying to parse file ... ");
	char *parsed=parseComputerNameFile(io,buffer,Iosb.Information);

	if (parsed!=0)
	{
		io.print("successful: ");
		io.println(parsed);
	}
	else
	{
		io.println("failed.");
		io.free(buffer);
		io.free(parsed);
		return 0;
	}

	buffer2 = (PWCHAR)io.malloc(500);
    
    mbstowcs(buffer2,(char*)parsed,strlen(parsed));
    
    Status = ZwClose(FileHandle);

	CHECK_STATUS(Status,Schließen der Datei);

	io.free(buffer);
	io.free(parsed);

	return buffer2;
}

/*
Function: setCompnameFromFile
command line command to set computer name from file.

Uses a list of files to find computer name. List consists of 
command line parameters and hard coded *\\device\\floppy0\\compname.txt*.

Tries to read each file and extract the computer name in the list till a 
valid one is found. This is set as computer name afterwards.

Parameters:
io - <IO>-Controller
args - command line
*/
void setCompnameFromFile(IO &io,char *args)
{
	Indenter i(io);
	int numFiles=mainSingleton->getArgc();
	char **valueNames=(char**)io.malloc(4*numFiles);
	char **cmdargs=mainSingleton->getArgs();

	io.print("Computername file pipe: ");

	for (int i=1;i<numFiles;i++)
	{
		io.print(cmdargs[i]);
		io.print(", ");
		valueNames[i-1]=cmdargs[i];
	}
	
	valueNames[numFiles-1]="\\device\\floppy0\\compname.txt";
	io.println(valueNames[numFiles-1]);

	io.println("Reading computer-name from ...");

	for (int i=0;i<numFiles;i++)
	{
		io.println(valueNames[i]);
		wchar_t *buffer2=readComputerNameFromFile(io,io.char2wchar(valueNames[i]));
		if (buffer2!=0)
		{
			setComputerName(io,buffer2);
			io.free(buffer2);
			io.free(valueNames);
			return;
		}
	}
	io.free(valueNames);
}