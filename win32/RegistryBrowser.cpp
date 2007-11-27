#include "StdAfx.h"
#include "RegistryBrowser.h"
#include "io.h"
#include "main.h"
#include "handle.h"

RegKey *RegistryBrowser::get_current_key()
{
	if (!current)
		current = new RegKey(L"");

	return current;
}

void RegistryBrowser::lk(IO &io,char *args)
{
	get_current_key()->print_subkeys(io);
}
void RegistryBrowser::lv(IO &io,char *args)
{
	get_current_key()->print_values(io);
}

void RegistryBrowser::tk(IO &io,char *args)
{
	char buffer[1000];
	io.println(get_current_key()->get_name().chars(buffer,sizeof(buffer)));
}

void RegistryBrowser::ck(IO &io,char *args)
{
	if (*args==0)
	{
		if (current) delete current;
		current = 0;
		return;
	}

	UnicodeString name(&args[1]);
	RegKey *key=get_current_key()->subkey(name);
	if (key)
	{
		if (current) delete current;
		current = key;
	}
	else
		io.println("Key not found");
}

RegistryBrowser::RegistryBrowser(Main &main) : current(0)
{
	main.addCommand("lk",make_dg(this,&RegistryBrowser::lk)
		,"List subkeys"
		,"Usage: lk\nList all subkeys of the current key");
	main.addCommand("lv",make_dg(this,&RegistryBrowser::lv)
		,"List values"
		,"Usage: lv\nList the names of all values of the current key with their types");
	main.addCommand("ck",make_dg(this,&RegistryBrowser::ck)
		,"Change current registry key"
		,"Usage: ck [<subkey>]\nGo into the subkey if specified or else to the root of the registry");
	main.addCommand("tk",make_dg(this,&RegistryBrowser::tk)
		,"Show information about current key"
		,"Usage: tk\nShow information about the current key (*t*his *k*ey)");
}

RegistryBrowser::~RegistryBrowser(void)
{
	if (current)
		delete current;
	current = 0;
}
