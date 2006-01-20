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
