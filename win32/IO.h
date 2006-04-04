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
#define CHECK_STATUS(status,name) io.handleStatus(status, #name,__FILE__ ,TOSTRING(__LINE__));

class IO
{
public:
	IO(void);
	virtual char getChar()=0;
	virtual void print(char *buffer)=0;
	void println(char *buffer);
	virtual void*malloc(unsigned int length)=0;
	virtual void free(void *buffer)=0;
	~IO(void);
	virtual char *getVersion()=0;
	virtual void handleCharEcho(char ch,char *buffer,unsigned int length)=0;
	void readln(char *buffer,unsigned int length);
	wchar_t *char2wchar(char *buffer);
	NT::UNICODE_STRING getUnicodeString(char *buffer);
	void handleStatus(NTSTATUS status, char *function, char *file, char *line);
	void debugout(char *string);
};
