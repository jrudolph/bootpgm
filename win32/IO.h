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

#pragma once

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define CHECK_STATUS(status,name) io.handleStatus(status, #name,__FILE__ ,TOSTRING(__LINE__),true);
#define CHECK_STATUSA(status,name) io.handleStatus(status, #name,__FILE__ ,TOSTRING(__LINE__),false);

/*
Class: IO
Interface for the main input/output functions, contains some helper functions
*/
class IO
{
	unsigned char indent;
	unsigned char current;
public:
	/*
	Method: getChar
	has to be implemented by concrete controllers

	Returns: 
	one char per call from input device (keyboard)
	*/
	virtual char getChar()=0;
	/*
	Method: internalPrint
	prints buffer to screen, must be implemented by concrete controllers

	Parameters:
	buffer - null-terminated string to be printed to screen
	*/
	virtual void internalPrint(char *buffer)=0;
	/*
	Method: malloc
	allocates memory of specified size, must be implemented by concrete controllers

	Parameters:
	length - length of buffer to allocate

	Returns:
	pointer to allocated memory
	*/
	virtual void*malloc(unsigned int length)=0;
	virtual void free(void *buffer)=0;
	virtual char *getVersion()=0;
	virtual void handleCharEcho(char ch,char *buffer,unsigned int length)=0;

	IO(void);
	~IO(void);

	void print(char *buffer);
	void println(char *buffer);
	void readln(char *buffer,unsigned int length);
	wchar_t *char2wchar(char *buffer);
	NT::UNICODE_STRING getUnicodeString(char *buffer);
	void handleStatus(NTSTATUS status, char *function, char *file, char *line,bool onlyWhenDebugging);
	void debugout(char *string);
	void setIndent(unsigned char indent);
	unsigned char getIndent();
};

class Indenter
{
	unsigned char original;
	IO &io;
public:
	Indenter(IO &pio):io(pio)
	{
		original=io.getIndent();
		io.setIndent(original+2);
	}
	~Indenter()
	{
		io.setIndent(original);
	}
};