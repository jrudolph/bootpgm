#include "StdAfx.h"
#include "Handle.h"
#include "main.h"

#define CHECKER(status) ::handle(status, __FILE__ ,TOSTRING(__LINE__));

void handle(ULONG status,char *file,char *line)
{
	if (status!=STATUS_SUCCESS)
	{
		char *buffer=new char[5000];
		sprintf_s(buffer,5000,"Fehler in %s Zeile %s: 0x%X",file,line,status);
		throw buffer;
	}
}

void debug(char *msg)
{
	mainSingleton->get_io().debugout(msg);
}

WinObject::~WinObject()
{
	char buffer[500];
	debug((L"Closing Handle to '" + get_name() + L"'" ).chars(buffer,500));
	ZwClose(handle);
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
	size_t len=wcslen(chars);
	wchar_t *buffer=new wchar_t[len];
	string.Length=len*sizeof(wchar_t);
	string.Buffer=buffer;
	string.MaximumLength=string.Length;
	memcpy(buffer,chars,len*2);

	/*debug("UnicodeString created:");
	char buffer2[500];
	debug(this->chars(buffer2,500));*/
}
UnicodeString & UnicodeString::operator=(wchar_t *chars)
{
	size_t len= wcslen(chars) * 2;

	if (len > string.MaximumLength)
	{
		delete string.Buffer;
		string.Buffer = new wchar_t[len/2];
		string.MaximumLength = len;
	}
	
	memcpy(string.Buffer,chars,len);
	string.Length = len;

	return *this;
}
UnicodeString::~UnicodeString()
{
	//debug("UnicodeString deleted:");
	//char buffer[500];
	//debug(this->chars(buffer,500));

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

int RegKey::get_value(UnicodeString &path,PULONG type,void *buffer,int length)
{
	int len=length + sizeof(KEY_VALUE_PARTIAL_INFORMATION);
	void *temp=_alloca(len);
	ULONG read;
	KEY_VALUE_PARTIAL_INFORMATION *pinfo = reinterpret_cast<KEY_VALUE_PARTIAL_INFORMATION*>(temp);
	ULONG status=ZwQueryValueKey(
		handle
		,&path.unicode_string()
		,KeyValuePartialInformation
		,pinfo
		,len
		,&read);

	CHECKER(status)

	memcpy(buffer,pinfo->Data,pinfo->DataLength);
	*type = pinfo->Type;
	return pinfo->DataLength;
}

void RegKey::get_string_value(UnicodeString &name,UnicodeString&res)
{
	char buffer[500];
	ULONG type;
	ULONG len=get_value(name,&type,buffer,sizeof(buffer));
	
	if (type!=REG_SZ)
		throw "Only REG_SZ allowed in get_string_value";
	
	wchar_t *value= reinterpret_cast<wchar_t*>(buffer);
	value[min(sizeof(buffer)-1,len)] = 0;
	res = value;
}

HANDLE RegKey::open_key(UnicodeString &path)
{
	OBJECT_ATTRIBUTES ObjectAttributes;
	
	UnicodeString realPath = UnicodeString(L"\\Registry\\") + path;

	InitializeObjectAttributes(
		&ObjectAttributes,
		const_cast<UNICODE_STRING*>(&realPath.unicode_string()),
		OBJ_CASE_INSENSITIVE,
		NULL,
		NULL);

	HANDLE h;
	ULONG Disposition;
	ULONG status = ZwCreateKey(
		&h,
		KEY_ALL_ACCESS,
		&ObjectAttributes,
		0,
		NULL,
		REG_OPTION_NON_VOLATILE,
		&Disposition);

	CHECKER(status);

	return status == STATUS_SUCCESS? h : 0;
}

void testRegKey(IO &io,char *args)
{
	RegKey k(L"Machine\\SYSTEM\\CurrentContro4lSet\\Control\\ComputerName\\ComputerName");
	UnicodeString res(L"");
	k.get_string_value(UnicodeString(L"ComputerName"),res);

	char buffer[500];
	io.println(res.chars(buffer,500));
}