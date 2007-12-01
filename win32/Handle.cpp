/* The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 * 
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 * License for the specific language governing rights and limitations
 * under the License.
 * 
 * The Initial Developer of the Original Code is Johannes Rudolph.
 * Portions created by the Initial Developer are Copyright (C) 2006
 * the Initial Developer. All Rights Reserved.
 * 
 * Contributor(s): 
 *    Johannes Rudolph <johannes_rudolph@gmx.de>
 */ 

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
	if (valid())
	{
		char buffer[500];
		debug((L"Closing Handle to '" + get_name() + L"'" ).chars(buffer,500));
		ULONG status = ZwClose(handle);
		CHECKER(status)
	}
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

	init_string(len);
	memcpy(string->Buffer,chars,len*2);
}
UnicodeString::UnicodeString(const wchar_t *chars,unsigned short length)
{
	init_string(length/2);
	memcpy(string->Buffer,chars,length);
}
UnicodeString::UnicodeString(const char *chars)
{
	unsigned short count=(unsigned short)strlen(chars);

	init_string(count);

	mbstowcs(string->Buffer,chars,count);
}
UnicodeString::~UnicodeString()
{
	if (!--*count)
	{
		delete []string->Buffer;
		delete string;
		delete count;
	}
}
UnicodeString UnicodeString::operator+(UnicodeString&str2)
{
	UNICODE_STRING str;

	str.Length = 0;
	str.MaximumLength = string->Length + str2.string->Length;
	str.Buffer = new wchar_t[str.MaximumLength/2];

	NT::RtlAppendUnicodeStringToString(&str,string);
	NT::RtlAppendUnicodeStringToString(&str,str2.string);

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
		
		status = ZwEnumerateKey(
			handle
			,i
			,KeyBasicInformation
			,buffer
			,sizeof(buffer)
			,&res);

		if (status == STATUS_SUCCESS)
		{
			KEY_BASIC_INFORMATION *info = reinterpret_cast<KEY_BASIC_INFORMATION*>(buffer);
			UnicodeString name(info->Name,(unsigned short)info->NameLength);
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
