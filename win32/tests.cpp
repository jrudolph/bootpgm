#include "stdafx.h"
#include "main.h"
#include "handle.h"

class Test
{
	int *i;
	static void println(char *text)
	{
		mainSingleton->get_io().println(text);
	}
public:
	Test(int *v):i(v)
	{
		char buffer[1000];
		_snprintf(buffer,sizeof(buffer),"Test(int) called: this = %X , i = %d",(unsigned int)this,*i);
		println(buffer);
	}
	~Test()
	{
		char buffer[1000];
		_snprintf(buffer,sizeof(buffer),"~Test called: this = %X , i = %d",(unsigned int)this,*i);
		println(buffer);

		*i=0;
	}
	/*Test(Test&o)
	{
		char buffer[1000];
		_snprintf(buffer,sizeof(buffer),"Test(Test&) called: this = %X , other = %X",(unsigned int)this,&o);
		println(buffer);
	}*/
	Test operator+(int other)
	{
		char buffer[1000];
		_snprintf(buffer,sizeof(buffer),"Test + called: this = %X , i = %d, other = %d",(unsigned int)this,*i,other);
		println(buffer);
		return Test(new int(*i+other));
	}
};

void assert(int expected,int actual,char *msg)
{
	char buffer[500];
	if (expected == actual)
		_snprintf(buffer,sizeof(buffer),"%s... SUCCESSFUL",msg);
	else
		_snprintf(buffer,sizeof(buffer),"%s... FAILED: %d != %d",msg,actual,expected);

	mainSingleton->get_io().println(buffer);
}

void classtest(IO &io,char *args)
{
	UnicodeString test("test");
	assert(1,test.refs(),"After initializing only one reference should be there");
	
	UnicodeString test2 = test;
	assert(2,test.refs(),"After initializing a second US two reference should be there");

	test = test;
	assert(2,test.refs(),"After self-assigning still only two reference should be there");
}