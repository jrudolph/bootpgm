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
#include "io.h"

typedef void (*invokeFunc)(IO &io,char *args);

struct command{
	invokeFunc func;
	char *name;
};

class Main
{
	IO &io;
	
	int funcc;
	static const int maxFuncs=10;
	command commands[maxFuncs];

	char** argv;
	int argc;
public:
	Main(IO &io,int argc,char** argv);
	void run();
	~Main(void);
	void rpl();
	void addCommand(char *name,invokeFunc func);
	void showSplashScreen();
private:
	friend void showCmds(IO &io,char *args);
	friend void showArgs(IO &io,char *args);

public:
	int getArgc()
	{
		return argc;
	}
	char **getArgs()
	{
		return argv;
	}
};

extern Main *mainSingleton;
char **split_args(IO &io,wchar_t* cmdLine,int *pargc);