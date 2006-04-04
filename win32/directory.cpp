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

#include "stdafx.h"
#include "io.h"

struct file_tools
{
	class String;
	class File;
	class Directory;

	#include "string-tools.h"
	#include "file.h"	
	#include "directory.h"
	
	static IO *io;
};
IO *file_tools::io;
void initFileTools(IO *io)
{
	file_tools::io=io;
}
void testStringFunctions(IO &io,char *args)
{
	file_tools::String::test();
}