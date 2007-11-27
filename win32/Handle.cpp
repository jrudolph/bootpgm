#include "StdAfx.h"
#include "Handle.h"
#include "main.h"

void debug(char *msg)
{
	mainSingleton->get_io().debugout(msg);
}

void handle(ULONG status,char *file,char *line)
{
	if (status!=STATUS_SUCCESS)
	{
		char *buffer = new char[5000];
		sprintf(buffer,"Fehler in %s Zeile %s: 0x%X",file,line,status);
		mainSingleton->get_io().println(buffer);
		mainSingleton->get_io().readln(buffer,sizeof(buffer));
	}
}

WinObject::~WinObject()
{
	char buffer[500];
	debug((L"Closing Handle to '" + get_name() + L"'" ).chars(buffer,500));
	ULONG status = ZwClose(handle);
	CHECKER(status)
}

UnicodeString WinObject::get_name()
{
	OBJECT_BASIC_INFORMATION info;
	ULONG status = NtQueryObject(
		handle
		,ObjectBasicInformation
		,&info
		,sizeof(info)
		,0);
	
	CHECKER(status);

	int len=info.NameInformationLength;

	len = len==0? 500 : len;
	
	if (len>0)
	{
		OBJECT_NAME_INFORMATION *poni=reinterpret_cast<OBJECT_NAME_INFORMATION*>(_alloca(len));
		status = NtQueryObject(
			handle
			,ObjectNameInformation
			,poni
			,len
			,0);

		CHECKER(status);

		return UnicodeString::from_unicode(poni->Name);
	}
	else
		return UnicodeString(L"");	
}

UnicodeString::UnicodeString(const wchar_t *chars)
{
	unsigned short len = (unsigned short)wcslen(chars);
	wchar_t *buffer=new wchar_t[len];
	string.Length= len * sizeof(wchar_t);
	string.Buffer=buffer;
	string.MaximumLength=string.Length;
	memcpy(buffer,chars,len*2);

	/*debug("UnicodeString created:");
	char buffer2[500];
	debug(this->chars(buffer2,500));*/
}
UnicodeString::UnicodeString(const wchar_t *chars,unsigned int length)
{
	string.Length = (USHORT)length;
	string.MaximumLength = (USHORT)length;
	string.Buffer = new wchar_t[length/2];
	memcpy(string.Buffer,chars,length);
}
UnicodeString::UnicodeString(const char *chars)
{
	unsigned short l=(unsigned short)strlen(chars);
	wchar_t *wcs=new wchar_t[l];
	mbstowcs(wcs,chars,l);
	string.Length = l*2;
	string.MaximumLength = l*2;
	string.Buffer = wcs;
}
/*UnicodeString &UnicodeString::operator=(wchar_t *chars)
{
	char buffer[1000];
	_snprintf(buffer,1000,"UnicodeString = called old = %S, new = %S",string.Buffer,chars);
	debug(buffer);

	unsigned short len= (unsigned short)wcslen(chars) * 2;

	if (len > string.MaximumLength)
	{
		delete string.Buffer;
		string.Buffer = new wchar_t[len/2];
		string.MaximumLength = len;
	}
	
	memcpy(string.Buffer,chars,len);
	string.Length = len;

	return *this;
}*/
UnicodeString::~UnicodeString()
{
	debug("UnicodeString deleted:");
	char buffer[500];
	debug(this->chars(buffer,500));

	delete string.Buffer;
}
UnicodeString UnicodeString::operator+(UnicodeString&str2)
{
	UNICODE_STRING str;

	str.Length = 0;
	str.MaximumLength = string.Length + str2.string.Length;
	str.Buffer = new wchar_t[str.MaximumLength/2];

	NT::RtlAppendUnicodeStringToString(&str,&string);
	NT::RtlAppendUnicodeStringToString(&str,&str2.string);

	return UnicodeString(str);
}

int RegKey::get_value(UnicodeString *path,PULONG type,void *buffer,int length)
{
	int len=length + sizeof(KEY_VALUE_PARTIAL_INFORMATION);
	void *temp=_alloca(len);
	ULONG read;
	KEY_VALUE_PARTIAL_INFORMATION *pinfo = reinterpret_cast<KEY_VALUE_PARTIAL_INFORMATION*>(temp);
	ULONG status=ZwQueryValueKey(
		handle
		,&path->unicode_string()
		,KeyValuePartialInformation
		,pinfo
		,len
		,&read);

	CHECKER(status)

	if (status!=STATUS_SUCCESS)
		return 0;

	memcpy(buffer,pinfo->Data,pinfo->DataLength);
	*type = pinfo->Type;
	return pinfo->DataLength;
}

void RegKey::set_value(UnicodeString *path,ULONG type,void *data,ULONG size)
{
	ULONG status = ZwSetValueKey(
		handle
		,&path->unicode_string()
		,0
		,type
		,data
		,size);
	CHECKER(status)
}

UnicodeString RegKey::get_string_value(UnicodeString *name)
{
	char buffer[500];
	ULONG type;
	ULONG len=get_value(name,&type,buffer,sizeof(buffer));
	
	if (type!=REG_SZ){
		debug("Only REG_SZ allowed in get_string_value");
		return (wchar_t*)0;
	}
	
	wchar_t *value= reinterpret_cast<wchar_t*>(buffer);
	value[min(sizeof(buffer)-1,len)] = 0;
	return value;
}

HANDLE RegKey::open_key(HANDLE parent,UnicodeString &path)
{
	OBJECT_ATTRIBUTES ObjectAttributes;
	
	UnicodeString fullPath = UnicodeString(L"\\Registry\\") + path;
	UnicodeString *pp = parent ? &path : &fullPath;

	InitializeObjectAttributes(
		&ObjectAttributes,
		const_cast<UNICODE_STRING*>(&pp->unicode_string()),
		OBJ_CASE_INSENSITIVE,
		parent,
		NULL);

	HANDLE h;
	ULONG Disposition;
	ULONG status = ZwOpenKey(
		&h,
		KEY_ALL_ACCESS,
		&ObjectAttributes);

	if (status == 0xC000034){
		IO &io=mainSingleton->get_io();
		io.print("Not found: ");
		char buffer[1000];
		io.println(path.chars(buffer,sizeof(buffer)));
	}

	CHECKER(status);

	return status == STATUS_SUCCESS? h : 0;
}

void RegKey::flush()
{
	ULONG status=NtFlushKey(handle);
	CHECKER(status);
}

void RegKey::save_to(HANDLE fileHandle)
{
	ULONG status = NtSaveKey(handle,fileHandle);
	CHECKER(status);
}

void RegKey::print_subkeys(IO &io)
{
	ULONG status = STATUS_SUCCESS;
	int i=0;
	char buffer[1000];
	do
	{		
		ULONG res;
		KEY_BASIC_INFORMATION *info = reinterpret_cast<KEY_BASIC_INFORMATION*>(buffer);
		status = ZwEnumerateKey(
			handle
			,i
			,KeyBasicInformation
			,buffer
			,sizeof(buffer)
			,&res);

		if (status == STATUS_SUCCESS)
		{
			UnicodeString name(info->Name,info->NameLength);
			io.println(name.chars(buffer,sizeof(buffer)));
		}

		i++;
	} while (status == STATUS_SUCCESS);
}
void RegKey::print_values(IO &io)
{
	ULONG status = STATUS_SUCCESS;
	int i=0;
	char buffer[1000];
	do
	{
		ULONG len;
		status = ZwEnumerateValueKey(
			handle
			,i
			,KeyValueBasicInformation
			,buffer
			,sizeof(buffer)-2
			,&len);
		
		if (status==STATUS_SUCCESS)
		{
			KEY_VALUE_BASIC_INFORMATION *info = reinterpret_cast<KEY_VALUE_BASIC_INFORMATION*>(buffer);
			info->Name[info->NameLength/2] = 0;
			char buf[100];
			_snprintf(buf,sizeof(buf),"%-30S | %-15s",info->Name,type_to_name(info->Type));
			io.println(buf);			
		}

		i++;
	} while(status == STATUS_SUCCESS);
}
RegKey *RegKey::subkey(UnicodeString&name)
{
	HANDLE h = open_key(handle,name);
	if (h)
		return new RegKey(h);
	else
		return 0;
}
