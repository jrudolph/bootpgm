#include "stdafx.h"
#include "handle.h"
#include "account.h"
#include "io.h"
#include "main.h"

void showAutoLogonName(IO &io,char *args)
{
	RegKey k(L"Machine\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon");
	char buffer[40];
	io.println(k.get_string_value(&UnicodeString(L"DefaultUsername")).chars(buffer,40));
}

void get_privilege(long id)
{
	HANDLE h;
	ULONG status=NtOpenProcessToken(
		NtCurrentProcess()
		,0x20
		,&h);
	CHECKER(status)

	TOKEN_PRIVILEGES tp;
	tp.count = 1;
	tp.Privileges[0].Luid = NT::RtlConvertLongToLuid(id);
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	status = NtAdjustPrivilegesToken(
		h
		,false
		,&tp
		,0
		,0
		,0);
	CHECKER(status)

	ZwClose(h);
}

void loadSam(IO &io,char *args)
{
	get_privilege(SE_RESTORE_PRIVILEGE);

	RegKey machine(L"Machine\\");

	UnicodeString key(L"SAM");
	UnicodeString path(L"\\??\\C:\\WINDOWS\\system32\\config\\SAM");

	OBJECT_ATTRIBUTES dest;
	InitializeObjectAttributes(
		&dest,
		&key.unicode_string(),
		OBJ_CASE_INSENSITIVE,
		machine.get_handle(),
		NULL);
	
	OBJECT_ATTRIBUTES file;
	InitializeObjectAttributes(
		&file,
		&path.unicode_string(),
		OBJ_CASE_INSENSITIVE,
		NULL,
		NULL);

	ULONG status=NtLoadKey(&dest,&file);
	CHECKER(status)
}

void save_key_to(UnicodeString &key,UnicodeString &path)
{
	get_privilege(SE_BACKUP_PRIVILEGE);

	RegKey sam(key);
	sam.flush();

	OBJECT_ATTRIBUTES file;
	InitializeObjectAttributes(
		&file,
		&path.unicode_string(),
		OBJ_CASE_INSENSITIVE,
		NULL,
		NULL);
	HANDLE hFile;
	IO_STATUS_BLOCK ios;
	ULONG status = ZwCreateFile(
		&hFile
		,GENERIC_WRITE
		,&file
		,&ios
		,0
		,0
		,0
		,FILE_CREATE
		,0
		,0
		,0);
	CHECKER(status);

	sam.save_to(hFile);
	ZwClose(hFile);
}

void saveSam(IO &io,char *args)
{
	save_key_to(UnicodeString(L"Machine\\SAM")
		,UnicodeString(L"\\??\\C:\\WINDOWS\\system32\\config\\SAM.bak"));
}

void unloadSam(IO &io,char *args)
{
	RegKey(L"Machine\\SAM").flush();

	RegKey machine(L"Machine\\");
	UnicodeString key(L"SAM");

	OBJECT_ATTRIBUTES dest;
	InitializeObjectAttributes(
		&dest,
		&key.unicode_string(),
		OBJ_CASE_INSENSITIVE,
		machine.get_handle(),
		NULL);
	ULONG status = NtUnloadKey(&dest);
	CHECKER(status)
}

void showName(IO &io,char *args)
{
	if (!*args)
	{
		io.println("No username given");
		return;
	}


	wchar_t buffer[1000];
	_snwprintf(buffer,1000,L"Machine\\SAM\\SAM\\Domains\\Account\\Users\\Names\\%S",&args[1]);

	RegKey nameKey(buffer);

	char buf[1000];

	if (!nameKey.valid())
	{
		_snprintf(buf,sizeof(buf),"User not found: %s",&args[1]);
		io.println(buf);
		return;
	}
	
	ULONG type;
	int length = nameKey.get_value(&UnicodeString(L""),&type,buf,sizeof(buffer));

	_snwprintf(buffer,1000,L"Machine\\SAM\\SAM\\Domains\\Account\\Users\\%08X",type);

	RegKey userKey(buffer);
	length = userKey.get_value(&UnicodeString(L"V"),&type,buf,sizeof(buffer));
	
	entry *es=read_entries(buf,V_ENTRY_COUNT);

	char buf2[1000];
	io.print("Name: ");
	io.println(UnicodeString((wchar_t *)es[USERNAME_E].data,es[USERNAME_E].length).chars(buf2,sizeof(buf2)));
	io.print("Voller Name: ");
	io.println(UnicodeString((wchar_t *)es[FULLNAME_E].data,es[FULLNAME_E].length).chars(buf2,sizeof(buf2)));
}

void testRegKey(IO &io,char *args)
{
	wchar_t buffer[1000];
	_snwprintf(buffer,1000,L"Machine\\SAM\\SAM\\Domains\\Account\\Users\\Names\\%S",&args[1]);

	char buf[1000];
	UnicodeString str(buffer);
	io.println(str.chars(buf,sizeof(buffer)));

	*(unsigned int*)buf = 0xcafebeef;

	RegKey nameKey(str);
	
	ULONG type;
	int length = nameKey.get_value(&UnicodeString(L""),&type,buf,sizeof(buffer));

	unsigned int d = *(unsigned int*)buf;
	_snprintf(buf,sizeof(buffer),"Length: %d, value: 0x%8X type: 0x%08X",length,d,type);
	io.println(buf);

	_snwprintf(buffer,1000,L"Machine\\SAM\\SAM\\Domains\\Account\\Users\\%08X",type);
	RegKey userKey(buffer);
	length = userKey.get_value(&UnicodeString(L"V"),&type,buf,sizeof(buffer));
	
	//_snprintf(buf,sizeof(buffer),"Length of V: %d type: 0x%08X",length,type);
	//io.println(buf);
	entry *es=read_entries(buf,V_ENTRY_COUNT);

	char buf2[1000];
	io.println(UnicodeString((wchar_t *)es[1].data,es[1].length).chars(buf2,sizeof(buf2)));

	es[USERNAME_E].data = L"Gustav";
	es[USERNAME_E].length = 12;

	int written = write_entries(es,V_ENTRY_COUNT,buf2,sizeof(buf2));
	_snprintf(buf,sizeof(buffer),"Written %d",written);
	io.println(buf);
	userKey.set_value(&UnicodeString(L"V"),type,buf2,written);

	userKey.flush();
}

const int RdWrIoPort = 0x80;

void initReg(IO &io2,char *args)
{
	//ULONG status = NtInitializeRegistry(2);
	//CHECKER(status);
	unsigned int addr=0x8066eb34; //CmpNoWrite

	char *c=(char*)addr;
	
	BYTE Value;

	IO_STRUCT io;
	memset(&io, 0, sizeof(io));
	io.IoAddr = RdWrIoPort;
	io.pBuffer = (PVOID)(ULONG_PTR)addr;
	io.NumBytes = 1;
	io.Reserved4 = 1;
	io.Reserved6 = 1;
	ULONG status = ZwSystemDebugControl(DebugSysWriteIoSpace, &io, sizeof(io), NULL, 0,NULL);
	CHECKER(status)

	memset(&io, 0, sizeof(io));
	io.IoAddr = RdWrIoPort;
	io.pBuffer = &Value;
	io.NumBytes = 1;
	io.Reserved4 = 1;
	io.Reserved6 = 1;
	status = ZwSystemDebugControl(DebugSysReadIoSpace, &io, sizeof(io), NULL, 0,NULL);
	CHECKER(status);

	if (Value == 1)
	{
		io2.println("NoWrite set, now resetting");
		Value = 0;

		memset(&io, 0, sizeof(io));
		io.IoAddr = RdWrIoPort;
		io.pBuffer = &Value;
		io.NumBytes = 1;
		io.Reserved4 = 1;
		io.Reserved6 = 1;
		status = ZwSystemDebugControl(DebugSysWriteIoSpace, &io, sizeof(io), NULL, 0,NULL);
		CHECKER(status);	

		memset(&io, 0, sizeof(io));
		io.IoAddr = RdWrIoPort;
		io.pBuffer = (PVOID)(ULONG_PTR)addr;
		io.NumBytes = 1;
		io.Reserved4 = 1;
		io.Reserved6 = 1;
		status = ZwSystemDebugControl(DebugSysReadIoSpace, &io, sizeof(io), NULL, 0, NULL);
		CHECKER(status);
	}
	else
		io2.println("NoWrite not set");	
}

void register_experimental_cmds(Main &main)
{
	main.addCommand("setName",testRegKey);
	main.addCommand("loadSam",loadSam);
	main.addCommand("unloadSam",unloadSam);
	main.addCommand("saveSam",saveSam);
	main.addCommand("initReg",initReg);
	main.addCommand("showName",showName);
}