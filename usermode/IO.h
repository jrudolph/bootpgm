#pragma once

class IO
{
public:
	IO(void);
	virtual char getChar()=0;
	virtual void print(char *buffer)=0;
	void println(char *buffer);
	virtual void*malloc(unsigned int length)=0;
	~IO(void);
	virtual char *getVersion()=0;
	void readln(char *buffer,unsigned int length);
};
