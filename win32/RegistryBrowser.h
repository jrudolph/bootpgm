#pragma once

class RegKey;
class IO;
class Main;

class RegistryBrowser
{
	RegKey *current;
	RegKey *get_current_key();
public:
	RegistryBrowser(Main &m);
	~RegistryBrowser(void);

	void lk(IO &io,char *args);
	void lv(IO &io,char *args);
	void ck(IO &io,char *args);
	void tk(IO &io,char *args);
};
