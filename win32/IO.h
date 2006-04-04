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
