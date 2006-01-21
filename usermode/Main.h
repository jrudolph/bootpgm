#pragma once
#include "io.h"

class Main
{
	IO &io;
public:
	Main(IO &io);
	void run();
	~Main(void);
	void rpl();
private:
	void showSplashScreen();
};
