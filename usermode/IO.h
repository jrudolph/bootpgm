#pragma once

class IO
{
public:
	IO(void);
	virtual char getChar()=0;
	virtual void print(char *buffer)=0;
	void println(char *buffer);
	~IO(void);
	virtual char *getVersion()=0;
};
