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

#include "stdafx.h"
#include "io.h"
#include "main.h"

#include "newnative.h"

#include <stdlib.h>
#include <stdio.h>

char keys[]={0,0,'1','2','3','4','5','6','7','8','9','0','ß','´',8/*Backspace*/ //0-14
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

class NativeBootIO:public IO{
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
    NativeBootIO()
    {
        createHeap();
        openKeyboard();
    }
    ~NativeBootIO()
    {
        //RtlDestroyHeap()
    }
    void handleCharEcho(char ch,char *buffer,unsigned int length)
    {
        char b[2];
        b[0]=ch;
        b[1]=0;
        if (ch==8)
        {
            println("");
            buffer[length]=0;
            print("> ");
            print(buffer);
        }
        else
            print(b);
    }
    char getChar()
    {
        debugout("getChar startet");
        KEYBOARD_INPUT_DATA kid;

        int chr=0;

        do
        {
            NTSTATUS Status=waitForKeyboardInput(0,&kid);
            if (Status!=STATUS_SUCCESS)
            {
                //_snprintf(buffer,99,"Fehler beim Tastaturlesen: 0x%x",Status);
                println("Fehler beim Tastaturlesen");
                debugout("Fehler beim Tastatur lesen");
            }
            else
            {
                debugout("Taste empfangen");

                updateKeyboardStatus(kid);

                if (((kid.Flags&KEY_BREAK)==0)&&kid.MakeCode<58&&kid.MakeCode>0)
                    if (keyboardState.shiftDown)
                        chr=shiftkeys[kid.MakeCode];
                    else
                        chr=keys[kid.MakeCode];
                else
                    chr=0;
            }
        }
        while(chr==0);
        debugout("getChar Ende");

        return (char)chr;
    }
    void *malloc(unsigned int size)
    {
        return RtlAllocateHeap( Heap, 0, size);
    }
    void free(void *buffer)
    {
        RtlFreeHeap(Heap,0,buffer);
    }
    void internalPrint(char *buffer)
    {
        UNICODE_STRING UnicodeFilespec=getUnicodeString(buffer);

        NtDisplayString(&UnicodeFilespec);
    }
    char *getVersion()
    {
        return "Native Boot IO Revision: $Rev$";
    }

    NTSTATUS waitForKeyboardInput(__int64 time,KEYBOARD_INPUT_DATA *kid)
    {
        LARGE_INTEGER bo;
        LARGE_INTEGER litime;
        NTSTATUS Status;
        IO_STATUS_BLOCK Iosb;

        bo.HighPart=0;
        bo.LowPart=0;

        debugout("wFKI: vor ZwReadFile");

        Status=ZwReadFile(Keyboard,
            KeyboardEvent,0,0,&Iosb,kid,sizeof(KEYBOARD_INPUT_DATA),&bo,NULL);

        debugout("wFKI: nach ZwReadFile");

        PLARGE_INTEGER pli=NULL;

        if (time!=0)
            pli=(PLARGE_INTEGER)&time;

        if (Status==STATUS_PENDING)
        {
            debugout("wFKI: vor WaitFor...");

            Status=NtWaitForMultipleObjects(1,&KeyboardEvent,1,1,pli);

            debugout("wFKI: nach WaitFor...");

            if (Status!=STATUS_SUCCESS)
            {
                NtCancelIoFile(Keyboard,&Iosb);
                return Status;
            }
        }
        return STATUS_SUCCESS;
    }
    void printKeyboardData(KEYBOARD_INPUT_DATA kid)
    {
        char buffer[100];
        int keyMake=kid.Flags&KEY_MAKE;
        int keyBreak=kid.Flags&KEY_BREAK;
        int e0=kid.Flags&KEY_E0;
        int e1=kid.Flags&KEY_E1;
        _snprintf(buffer,99,"Key: Code: %d\tMake: %d\tBreak: %d\te0: %d\te1: %d\n",kid.MakeCode,keyMake,keyBreak,e0,e1);
        debugout(buffer);
    }
    void testKeyboard()
    {
        KEYBOARD_INPUT_DATA kid;
        kid.MakeCode=0;
        char buffer[100];
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
                printKeyboardData(kid);
            }
        }
        println("Keyboardtest beendet");
    }
    void resetKeyboard()
    {
        debugout("Clearing Event");
        NtClearEvent(KeyboardEvent);
    }
};

extern "C"
int __cdecl _purecall()
{
    DbgBreakPoint();
    return 0;
}

void debugBreak(IO &io,char *args)
{
    DbgBreakPoint();
}

void setCompnameFromFile(IO &io,char *args);
void setComputerNameCmd(IO &io,char *args);

void myitoa(int i,char *buffer)
{
    int length=0;
    if (i==0)
    {
        buffer[0]='0';
        length=1;
    }
    else
    {
        char buffer2[20];
        while (i>0)
        {
            buffer2[length]='0'+i%10;
            i/=10;
            length++;
        }
        for (i=0;i<length;i++)
        {
            buffer[length-i-1]=buffer2[i];
        }
    }
    buffer[length]=0;
}

bool keyPressedInTime(NativeBootIO &io,__int64 time,char key)
{
    KEYBOARD_INPUT_DATA kid;
    io.debugout("kPIT startet");
    NTSTATUS status=io.waitForKeyboardInput(time,&kid);
    io.debugout("kPIT wFKI fertig");
    //CHECK_STATUS(status,wFKI-from-kPIT)
    if (status!=STATUS_SUCCESS)
        return false;

    if (((kid.Flags&KEY_BREAK)==0)&&kid.MakeCode<58&&kid.MakeCode>0)
        if (keys[kid.MakeCode]==key)
        {
            io.debugout("Key pressed !!!");
            return true;
        }
        else
            io.debugout("Wrong key pressed");
    else
        io.printKeyboardData(kid);

    return false;
}

bool startupWithKeyInner(NativeBootIO &io,int maxtime,char key) //maxtime in seconds
{
    io.print("System starting up: ");

    for (int i=maxtime;i>=0;i--)
    {
        char buffer[2];
        myitoa(i,buffer);
        io.print(buffer);
        io.print(" ");

        if (keyPressedInTime(io,-3333000,key))
            return true;
        else
            io.print(".");

        if (keyPressedInTime(io,-3333000,key))
            return true;
        else
            io.print(".");

        if (keyPressedInTime(io,-3333000,key))
            return true;
        else
            io.print(" ");
    }
    return false;
}
void clearKeyboardPipe(NativeBootIO &io)
{
    io.debugout("Starting clearKeyboardPipe");
    io.resetKeyboard();
    KEYBOARD_INPUT_DATA kid;
    while (io.waitForKeyboardInput(-1,&kid)==STATUS_SUCCESS);
    io.resetKeyboard();
    io.debugout("Ending clearKeyboardPipe");


}
bool startupWithKey(NativeBootIO &io,int maxtime,char key) //maxtime in seconds
{
    bool res=startupWithKeyInner(io,maxtime,key);
    io.println(" ");
    clearKeyboardPipe(io);
    return res;
}
extern "C" void NtProcessStartup(::PPEB peb )
{
	NativeBootIO io;
	myIO=&io;

	UNICODE_STRING &cmdLine = peb->ProcessParameters->CommandLine;

	char **arguments;
	int argc;
	arguments=split_args(io,cmdLine.Buffer,cmdLine.Length/2,&argc);

	Main main(io,argc,arguments);

	main.addCommand("break",debugBreak);
	main.addCommand("setComputerNameFromFile",setCompnameFromFile);
	main.addCommand("setComputerName",setComputerNameCmd);

	main.showSplashScreen();

#ifdef INTERACTIVE
    if (startupWithKey(io,2,'v'))
        main.rpl();
    else
#endif
        setCompnameFromFile(io,0);

    NtTerminateProcess( NtCurrentProcess(), 0 );
}