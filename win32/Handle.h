#pragma once

class UnicodeString
{
	NT::UNICODE_STRING string;
	UnicodeString(UNICODE_STRING str):string(str){}
public:
	UnicodeString(const wchar_t *chars);
	UNICODE_STRING& unicode_string(){return string;}
	virtual ~UnicodeString();
	static UnicodeString& from_unicode(UNICODE_STRING str)
	{
		UNICODE_STRING str2 = str;
		str2.Buffer = new wchar_t[str.Length/2];
		memcpy(str2.Buffer,str.Buffer,str.Length);
		return *new UnicodeString(str2);
	}
	UnicodeString& operator=(wchar_t *new_data);
	char *chars(char *buffer,size_t len)
	{
		wcstombs(buffer,string.Buffer,min(len,string.Length/2));
		buffer[min(len-1,string.Length/2)]=0;
		return buffer;
	}
	UnicodeString operator+(UnicodeString&str2);

	friend UnicodeString operator+(wchar_t* ch,UnicodeString str)
	{
		return UnicodeString(ch) + str;
	}
};

class WinObject
{
protected:
	const HANDLE handle;
public:
	WinObject(HANDLE h):handle(h){}
public:
	~WinObject();
	UnicodeString get_name();
};

class RegKey:public WinObject
{
private:
	static HANDLE open_key(UnicodeString &path);
public:
	RegKey(UnicodeString &path):WinObject(open_key(path)){}
	RegKey(const wchar_t *path):WinObject(open_key(UnicodeString(path))){}
	int get_value(UnicodeString &path,PULONG type,void *buffer,int length);
	void get_string_value(UnicodeString &name,UnicodeString&res);
};
