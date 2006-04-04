class String
{
	UNICODE_STRING unicode;
	char *charBuffer;
	wchar_t *wcharBuffer;
public:
	UNICODE_STRING getUnicodeString()
	{
		if (unicode.Buffer==NULL)
		{
			if (wcharBuffer==NULL&&charBuffer!=NULL)
				wcharBuffer=file_tools::io->char2wchar(charBuffer);
			RtlInitUnicodeString(&unicode, wcharBuffer);
		}

		return unicode;
	}
	char *getCharBuffer()
	{
		unsigned int length;
		wchar_t *wbuffer;

		if (charBuffer!=NULL)
			return charBuffer;
		else if (unicode.Buffer!=0)
		{
			wbuffer=unicode.Buffer;
			length=unicode.Length;
		}
		else if (wcharBuffer!=0)
		{
			wbuffer=wcharBuffer;
			length=wcslen(wcharBuffer);
		}
		charBuffer=(char*)file_tools::io->malloc(length+1);
		wcstombs(charBuffer,wbuffer,length);
		charBuffer[length]=0;

		return charBuffer;
	}
	String(char *string)
	{
		charBuffer=(char*)file_tools::io->malloc(strlen(string)+1);
		memcpy(charBuffer,string,strlen(string)+1);
		unicode.Buffer=NULL;
		wcharBuffer=NULL;
	}
	~String()
	{
		if (unicode.Buffer!=NULL)
		{
			file_tools::io->free(unicode.Buffer);
		}

		if (charBuffer!=NULL)
			file_tools::io->free(charBuffer);

		if (wcharBuffer!=NULL)
			file_tools::io->free(wcharBuffer);
	}

	
	static void test()
	{
		IO &io=*file_tools::io;
		char buffer[]="wurst";
		{
			String str(buffer);
			if (strcmp(str.getCharBuffer(),buffer)!=0)
				io.println("String.getCharBuffer flawed");
			else
				io.println("String.getCharBuffer: success");
		}
	}
};