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