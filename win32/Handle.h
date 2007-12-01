#pragma once

class IO;

class IBuffer
{
public:
	virtual char *buffer() = 0;
	virtual unsigned int length() = 0;
};

class Buffer : public IBuffer
{
	Buffer(Buffer&);
	Buffer& operator=(Buffer&);

	char *data;
	unsigned int len;
public:
	Buffer(unsigned int l)
	{
		data = new char[l];
		len = l;
	}
	virtual ~Buffer()
	{
		delete []data;
	}
	virtual char *buffer(){return data;}
	virtual unsigned int length(){return len;}
};

class UnicodeString
{
private:
	NT::UNICODE_STRING *string;
	unsigned int *count;

	void init_string(unsigned short cChars)
	{
		string = new UNICODE_STRING;
		string->Length = cChars * 2;
		string->MaximumLength = cChars * 2;
		string->Buffer = new wchar_t[cChars];
		count = new unsigned int;
		*count = 1;
	}

	UnicodeString(UNICODE_STRING str)
	{
		string = new UNICODE_STRING;
		*string  = str;
		count = new unsigned int;
		*count = 1;
	}
public:
	UnicodeString(const wchar_t *chars);
	UnicodeString(const wchar_t *chars,unsigned short length);
	UnicodeString(const char *chars);
	UNICODE_STRING& unicode_string(){return *string;}
	virtual ~UnicodeString();
	static UnicodeString& from_unicode(UNICODE_STRING str)
	{
		UNICODE_STRING str2 = str;
		str2.Buffer = new wchar_t[str.Length/2];
		memcpy(str2.Buffer,str.Buffer,str.Length);
		return *new UnicodeString(str2);
	}
	char *chars(IBuffer &b)
	{
		return chars(b.buffer(),b.length());
	}
	char *chars(char *buffer,size_t len)
	{
		wcstombs(buffer,string->Buffer,min(len,(unsigned int)string->Length/2));
		buffer[min(len-1,(unsigned int)string->Length/2)]=0;
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

	UnicodeString(const UnicodeString& other)
	{
		string = other.string;
		count = other.count;
		++*count;
	}
	UnicodeString& operator=(UnicodeString&other)
	{
		string = other.string;
		++*other.count;
		if (!--*count)
		{
			delete []string->Buffer;
			delete string;
			delete count;
		}

		count = other.count;
		
		return *this;
	}
	unsigned int refs()
	{
		return *count;
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
