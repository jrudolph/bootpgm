#include "stdafx.h"
#include "io.h"
#include "main.h"

#include "native.h"

#include <stdlib.h>
#include <stdio.h>

char keys[]={0,0,'1','2','3','4','5','6','7','8','9','0','ß','´',0/*Backspace*/ //0-14
			,0/*tab*/,'q','w','e','r','t','z','u','i','o','p','ü','+','\n'/*return*/ //15-28
			,0/*strg*/,'a','s','d','f','g','h','j','k','l','ö','ä','^',0/*left shift*/,'#' //29-43
			,'y','x','c','v','b','n','m',',','.','-',0/*right shift*/ //44-54
			,'*'/*num*/,0/*left alt*/,' ',0/*caps lock*/}; //55-58

char shiftkeys[]={0,0,'!','\"','§','$','%','&','/','(',')','=','?','`',0/*Backspace*/ //0-14
			,0/*tab*/,'Q','W','E','R','T','Z','U','I','O','P','Ü','*','\n'/*return*/ //15-28
			,0/*strg*/,'A','S','D','F','G','H','J','K','L','Ö','Ä','°',0/*left shift*/,'\'' //29-43
			,'Y','X','C','V','B','N','M',';',':','_',0/*right shift*/ //44-54
			,'*'/*num*/,0/*left alt*/,' ',0/*caps lock*/};

IO *myIO=0;

void fatal(char *msg)
{
	if (myIO!=0)
		myIO->println(msg);

	NtTerminateProcess( NtCurrentProcess(), 0 );
}

struct KeyboardState
{
	bool shiftDown;
	bool altDown;
	bool altGrDown;
	KeyboardState():shiftDown(false),altDown(false),altGrDown(false)
	{}
};

class KernelmodeIO:public IO{
	HANDLE Heap;
	HANDLE Keyboard;
	HANDLE KeyboardEvent;
	KeyboardState keyboardState;
private:
	void createHeap()
	{
		RTL_HEAP_DEFINITION  heapParams;
		memset( &heapParams, 0, sizeof( RTL_HEAP_DEFINITION ));
		heapParams.Length = sizeof( RTL_HEAP_DEFINITION );
		Heap = RtlCreateHeap( 2, 0, 0x100000, 0x1000, 0, &heapParams );
	}
	void openKeyboard()
	{
	    UNICODE_STRING UnicodeFilespec;
	    OBJECT_ATTRIBUTES ObjectAttributes;
		NTSTATUS Status;
		IO_STATUS_BLOCK Iosb;

		RtlInitUnicodeString(&UnicodeFilespec, L"\\device\\KeyboardClass0");
		InitializeObjectAttributes(&ObjectAttributes,           // ptr to structure
			                       &UnicodeFilespec,            // ptr to file spec
				                   OBJ_CASE_INSENSITIVE,        // attributes
						           NULL,                        // root directory handle
							       NULL );                      // ptr to security descriptor

		Status = ZwCreateFile(&Keyboard,                      // returned file handle
                          (GENERIC_READ|SYNCHRONIZE|FILE_READ_ATTRIBUTES),    // desired access
                          &ObjectAttributes,                // ptr to object attributes
                          &Iosb,                            // ptr to I/O status block
                          0,                                // allocation size
                          FILE_ATTRIBUTE_NORMAL,            // file attributes
                          0,                                // share access
                          FILE_OPEN,                          // create disposition
                          1,     // create options
                          NULL,                             // ptr to extended attributes
                          0);    // length of ea buffer

		if (Status!=STATUS_SUCCESS)
			fatal("Fehler: Keyboardhandle konnte nicht geöffnet werden");

		InitializeObjectAttributes(&ObjectAttributes,           // ptr to structure
                               NULL,            // ptr to file spec
                               0,        // attributes
                               NULL,                        // root directory handle
                               NULL );                      // ptr to security descriptor
		Status=NtCreateEvent(&KeyboardEvent,EVENT_ALL_ACCESS,&ObjectAttributes,SynchronizationEvent,FALSE);

		if (Status!=STATUS_SUCCESS)
			fatal("Fehler: Keyboardevent konnte nicht erstellt werden");
	}
	void updateKeyboardStatus(KEYBOARD_INPUT_DATA &kid)
	{
		if (((kid.MakeCode==42)||(kid.MakeCode==54))&&(kid.Flags&KEY_E0)==0&&(kid.Flags&KEY_E1)==0)
			keyboardState.shiftDown=!(kid.Flags&KEY_BREAK);
	}
	void printkid(KEYBOARD_INPUT_DATA &kid)
	{
		static char *buffer=(char*)malloc(100);
		
		int keyMake=kid.Flags&KEY_MAKE;
		int keyBreak=kid.Flags&KEY_BREAK;
		int e0=kid.Flags&KEY_E0;
		int e1=kid.Flags&KEY_E1;
		_snprintf(buffer,99,"Key: Code: %d\tMake: %d\tBreak: %d\te0: %d\te1: %d",kid.MakeCode,keyMake,keyBreak,e0,e1);
		println(buffer);
	}

public:
	KernelmodeIO()
	{
		createHeap();
		openKeyboard();
	}
	~KernelmodeIO()
	{
		//RtlDestroyHeap()
	}
	char getChar()
	{
		//DbgPrint("getChar startet\n");
		KEYBOARD_INPUT_DATA kid;
		
		int chr;

		do
		{
			NTSTATUS Status=waitForKeyboardInput(0,&kid);
			if (Status!=STATUS_SUCCESS)
			{
				//_snprintf(buffer,99,"Fehler beim Tastaturlesen: 0x%x",Status);
				println("Fehler beim Tastaturlesen");
			}
			else
			{
				updateKeyboardStatus(kid);

				if (((kid.Flags&KEY_BREAK)==0)&&kid.MakeCode<58&&kid.MakeCode>0)
					if (keyboardState.shiftDown)
						chr=shiftkeys[kid.MakeCode];
					else
						chr=keys[kid.MakeCode];
				else
					chr=0;

				//printkid(kid);
				//DbgBreakPoint();
			}
		}
		while(chr==0);

		return (char)chr;
	}
	void *malloc(unsigned int size)
	{
		return RtlAllocateHeap( Heap, 0, size);
	}
	void print(char *buffer)
	{
		unsigned int size=strlen(buffer)+1;
		wchar_t *buffer2=(wchar_t*)malloc(sizeof(wchar_t)*size);
		mbstowcs(buffer2,(char*)buffer,size);
		
		UNICODE_STRING UnicodeFilespec;
		RtlInitUnicodeString(&UnicodeFilespec, buffer2);

		NtDisplayString(&UnicodeFilespec);
	}
	char *getVersion()
	{
		return "Kernelmode IO";
	}

	NTSTATUS waitForKeyboardInput(unsigned long time,KEYBOARD_INPUT_DATA *kid)
	{
		LARGE_INTEGER bo;
		LARGE_INTEGER litime;
		NTSTATUS Status;
		IO_STATUS_BLOCK Iosb;
		
		bo.HighPart=0;
		bo.LowPart=0;

		Status=ZwReadFile(Keyboard,
            KeyboardEvent,0,0,&Iosb,kid,sizeof(KEYBOARD_INPUT_DATA),&bo,NULL);

		litime.HighPart=0;
		litime.LowPart=time;
		
		PLARGE_INTEGER pli=NULL;

		if (time!=0)
			pli=&litime;

		if (Status==STATUS_PENDING)
		{
			Status=NtWaitForMultipleObjects(1,&KeyboardEvent,1,1,pli);
			if (Status!=STATUS_SUCCESS)
				return Status;
		}
		return STATUS_SUCCESS;
	}

	void testKeyboard()
	{
		KEYBOARD_INPUT_DATA kid;
		kid.MakeCode=0;
		char *buffer=(char*)malloc(100);
		while(kid.MakeCode!=1)
		{
			NTSTATUS Status=waitForKeyboardInput(0,&kid);
			if (Status!=STATUS_SUCCESS)
			{
				_snprintf(buffer,99,"Fehler beim Tastaturlesen: 0x%x",Status);
				println(buffer);
			}
			else
			{
				int keyMake=kid.Flags&KEY_MAKE;
				int keyBreak=kid.Flags&KEY_BREAK;
				int e0=kid.Flags&KEY_E0;
				int e1=kid.Flags&KEY_E1;
				_snprintf(buffer,99,"Key: Code: %d\tMake: %d\tBreak: %d\te0: %d\te1: %d",kid.MakeCode,keyMake,keyBreak,e0,e1);
				println(buffer);
			}
		}
		println("Keyboardtest beendet");
	}
};

extern "C"
int __cdecl _purecall()
{
DbgBreakPoint();
return 0;
}



extern "C" void NtProcessStartup( PSTARTUP_ARGUMENT Argument )
{
	KernelmodeIO io;
	myIO=&io;

	//io.println("Keyboardtest:");
	//io.testKeyboard();

	Main main(io);
	main.run();	

	NtTerminateProcess( NtCurrentProcess(), 0 );
}