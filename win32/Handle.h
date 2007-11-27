#pragma once

class IO;

class UnicodeString
{
	NT::UNICODE_STRING string;
	UnicodeString(UNICODE_STRING str):string(str){}
public:
	UnicodeString(const wchar_t *chars);
	UnicodeString(const wchar_t *chars,unsigned int length);
	UnicodeString(const char *chars);
	UNICODE_STRING& unicode_string(){return string;}
	virtual ~UnicodeString();
	static UnicodeString& from_unicode(UNICODE_STRING str)
	{
		UNICODE_STRING str2 = str;
		str2.Buffer = new wchar_t[str.Length/2];
		memcpy(str2.Buffer,str.Buffer,str.Length);
		return *new UnicodeString(str2);
	}
	//UnicodeString& operator=(wchar_t *new_data);
	//UnicodeString& operator=(UnicodeString &other);
	char *chars(char *buffer,size_t len)
	{
		wcstombs(buffer,string.Buffer,min(len,(unsigned int)string.Length/2));
		buffer[min(len-1,(unsigned int)string.Length/2)]=0;
		return buffer;
	}
	UnicodeString operator+(UnicodeString&str2);
	UnicodeString operator+(wchar_t *ch)
	{
		return *this + UnicodeString(ch);
	}

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
	HANDLE get_handle(){return handle;}
	bool valid(){return handle != 0;}
};

class RegKey:public WinObject
{
private:
	static HANDLE open_key(HANDLE parent,UnicodeString &path);
	RegKey(HANDLE h):WinObject(h){}
public:
	RegKey(UnicodeString &path):WinObject(open_key(0,path)){}
	RegKey(const wchar_t *path):WinObject(open_key(0,UnicodeString(path))){}

	int get_value(UnicodeString *path,PULONG type,void *buffer,int length);
	UnicodeString get_string_value(UnicodeString *name);
	void set_value(UnicodeString *path,ULONG type,void *data,ULONG size);
	void flush();
	void save_to(HANDLE fileHandle);

	RegKey *subkey(UnicodeString&name);

	void print_subkeys(IO &io);
	void print_values(IO &io);
	static const char *type_to_name(ULONG type)
	{
		switch(type)
		{
		case REG_SZ:
			return "String";
		case REG_EXPAND_SZ:
			return "Expandable String";
		case REG_MULTI_SZ:
			return "Multiline String";
		case REG_DWORD:
			return "DWORD";
		default:
			return "Unknown";
		}
	}
};
