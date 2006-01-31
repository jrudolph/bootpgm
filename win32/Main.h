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
public:
	Main(IO &io);
	void run();
	~Main(void);
	void rpl();
	void addCommand(char *name,invokeFunc func);
private:
	void showSplashScreen();
};
