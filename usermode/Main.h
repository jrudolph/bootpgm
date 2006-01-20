#pragma once
#include "io.h"

class Main
{
	IO &io;
public:
	Main(IO &io);
	void run();
	~Main(void);
private:
	void showSplashScreen();
};
