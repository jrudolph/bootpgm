//======================================================================
//
// Native.c
//
// Mark Russinovich
// http://www.ntinternals.com
//
// This is a demonstration of a Native NT program. These programs
// run outside of the Win32 environment and must rely on the raw
// services provided by NTDLL.DLL. AUTOCHK (the program that executes
// a chkdsk activity during the system boot) is an example of a
// native NT application.
//
// This example is a native 'hello world' program. When installed with
// the regedit file associated with it, you will see it print
// "hello world" on the initialization blue screen during the system
// boot. This program cannot be run from inside the Win32 environment.
//
//======================================================================
#include "ntddk.h" // include this for its native functions and defn's
#include "ntddkbd.h"
#include "stdio.h"
#include "stdlib.h"
#include "native.h"



WCHAR KeyNameBuffer[]        = L"\\Registry\\Machine\\SYSTEM\\CurrentControlSet\\Control\\ComputerName\\ComputerName";
WCHAR KeyNameBuffer2[]        = L"\\Registry\\Machine\\SYSTEM\\CurrentControlSet\\Control\\ComputerName\\ActiveComputerName";
WCHAR Tcpip[]        = L"\\Registry\\Machine\\SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters";
WCHAR ComputerNameBuffer[]    = L"ComputerName";

void setRegistryValue(WCHAR *keyName,WCHAR *valueName,WCHAR *value)
{
UNICODE_STRING KeyName, ValueName;
    HANDLE SoftwareKeyHandle;
    ULONG Status;
    OBJECT_ATTRIBUTES ObjectAttributes;
    ULONG Disposition;

    //DbgBreakPoint();
    //
    // Open the Software key
    //
    KeyName.Buffer = keyName;
    KeyName.Length = wcslen( keyName ) *sizeof(WCHAR);
    InitializeObjectAttributes( &ObjectAttributes, &KeyName,
            OBJ_CASE_INSENSITIVE, NULL, NULL );
    Status = ZwCreateKey( &SoftwareKeyHandle, KEY_ALL_ACCESS,
                    &ObjectAttributes, 0,  NULL, REG_OPTION_NON_VOLATILE,
                    &Disposition );


    //
    // Create the hidden value
    //
    ValueName.Buffer = valueName;
    ValueName.Length = wcslen( valueName ) *sizeof(WCHAR);
    Status = ZwSetValueKey( SoftwareKeyHandle, &ValueName, 0, REG_SZ,
                        value,
                        wcslen( value ) * sizeof(WCHAR) );

    Status = ZwClose(SoftwareKeyHandle);
}

void setComputerName(WCHAR *computerName)
{
    setRegistryValue(KeyNameBuffer,ComputerNameBuffer,computerName);
    setRegistryValue(KeyNameBuffer2,ComputerNameBuffer,computerName);
    setRegistryValue(Tcpip,L"Hostname",computerName);
    setRegistryValue(Tcpip,L"NV Hostname",computerName);
}
HANDLE Heap;
char* Message = "Hello world! I was written using the native NT API.\n";
void openFile(WCHAR *name)
{
    NTSTATUS Status;
    UNICODE_STRING UnicodeFilespec;
    OBJECT_ATTRIBUTES ObjectAttributes;
    HANDLE FileHandle;
    IO_STATUS_BLOCK Iosb;
    PWCHAR buffer;
    PWCHAR buffer2;
    ULONG converted;
    ULONG MessageLength = strlen(Message);
    wchar_t *valueName= L"\\device\\floppy0\\temp\\native2.txt";
    //printf("Starting OSR's Native NT API Example...\n");
    //DbgBreakPoint();
    //
    // Initialize a unicode string with the fully qualified path of the file
    // that we wish to create
    //
//    RtlInitUnicodeString(&UnicodeFilespec, L"\\device\\floppy0\\Temp\\native.txt");
    UnicodeFilespec.Buffer = valueName;
    UnicodeFilespec.Length = wcslen( valueName ) *sizeof(WCHAR);

    //
    // Setup the name in an object attributes structure.
    // Note that we create a name that is case INsensitive
    //
    InitializeObjectAttributes(&ObjectAttributes,           // ptr to structure
                               &UnicodeFilespec,            // ptr to file spec
                               OBJ_CASE_INSENSITIVE,        // attributes
                               NULL,                        // root directory handle
                               NULL );                      // ptr to security descriptor

    //
    // Do the create.  In this particular case, we'll have the I/O Manager
    // make our write requests syncrhonous for our convenience.
    //
    Status = ZwCreateFile(&FileHandle,                      // returned file handle
                          (GENERIC_READ | SYNCHRONIZE),    // desired access
                          &ObjectAttributes,                // ptr to object attributes
                          &Iosb,                            // ptr to I/O status block
                          0,                                // allocation size
                          FILE_ATTRIBUTE_NORMAL,            // file attributes
                          0,                                // share access
                          FILE_OPEN,                   // create disposition
                          FILE_SYNCHRONOUS_IO_NONALERT,     // create options
                          NULL,                             // ptr to extended attributes
                          0);                               // length of ea buffer

    //
    // The file has been successfully created.  Let's try WRITING to it!
    // Note we don't use, or need, an event handle here since we've opened
    // the file for synchronous I/O.
    //
    /*Status = ZwWriteFile(FileHandle,                   // file Handle
                         0,                            // event Handle
                         NULL,                         // APC entry point
                         NULL,                         // APC context
                         &Iosb,                        // IOSB address
                         Message,                      // ptr to data buffer
                         MessageLength,                // length
                         0,                            // byte offset
                         NULL);                        // key*/
    buffer = RtlAllocateHeap( Heap, 0, 256 );
    Status = ZwReadFile(FileHandle,0,NULL,NULL,&Iosb,buffer,256,0,NULL);
    buffer2 = RtlAllocateHeap( Heap, 0, 500 );
    //mbstowcs_s(&converted,buffer2,250,(char*)buffer,strlen((char*)buffer));
    ((char*)buffer)[Iosb.Information]=0;
    mbstowcs(buffer2,(char*)buffer,Iosb.Information+1);
    setComputerName(buffer2);
    /*RtlInitUnicodeString(&UnicodeFilespec, buffer2);
    NtDisplayString(&UnicodeFilespec);*/
       //
    // Well, That's all folks!
    //
    Status = ZwClose(FileHandle);
}

NTSYSAPI NTSTATUS NTAPI NtCreateEvent(PHANDLE,ACCESS_MASK,POBJECT_ATTRIBUTES,EVENT_TYPE,BOOLEAN);
NTSYSAPI NTSTATUS NTAPI NtWaitForMultipleObjects(ULONG handlecount,PHANDLE handles,int wait_type,BOOLEAN alertable,PLARGE_INTEGER timeout);

void testKeyboard(){
     unsigned long Status;
    UNICODE_STRING UnicodeFilespec;
    OBJECT_ATTRIBUTES ObjectAttributes;
    HANDLE FileHandle;
    IO_STATUS_BLOCK Iosb;
    KEYBOARD_INPUT_DATA kid;
    HANDLE eventHandle;
    char *buffer;
    LARGE_INTEGER bo;
    ULONG MessageLength = strlen(Message);

    //printf("Starting OSR's Native NT API Example...\n");

    //
    // Initialize a unicode string with the fully qualified path of the file
    // that we wish to create
    //
    RtlInitUnicodeString(&UnicodeFilespec, L"\\device\\KeyboardClass0");
    InitializeObjectAttributes(&ObjectAttributes,           // ptr to structure
                               &UnicodeFilespec,            // ptr to file spec
                               OBJ_CASE_INSENSITIVE,        // attributes
                               NULL,                        // root directory handle
                               NULL );                      // ptr to security descriptor
    DbgBreakPoint();
                               
    Status = ZwCreateFile(&FileHandle,                      // returned file handle
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


    InitializeObjectAttributes(&ObjectAttributes,           // ptr to structure
                               NULL,            // ptr to file spec
                               0,        // attributes
                               NULL,                        // root directory handle
                               NULL );                      // ptr to security descriptor
    Status=NtCreateEvent(&eventHandle,EVENT_ALL_ACCESS,&ObjectAttributes,SynchronizationEvent,FALSE);
    
    buffer= RtlAllocateHeap( Heap, 0, 256 );

    bo.HighPart=0;
    bo.LowPart=0;

    Status=ZwReadFile(FileHandle,
            eventHandle,0,0,&Iosb,&kid,sizeof(KEYBOARD_INPUT_DATA),&bo,NULL);
            
    Status=NtWaitForMultipleObjects(1,&eventHandle,1,1,0);
}


//
// Our heap
//

//----------------------------------------------------------------------
//
// NtProcessStartup
//
// Instead of a 'main' or 'winmain', NT applications are entered via
// this entry point.
//
//----------------------------------------------------------------------
void NtProcessStartup( PSTARTUP_ARGUMENT Argument )
{
    PUNICODE_STRING commandLine;
    PWCHAR stringBuffer, argPtr;
    UNICODE_STRING helloWorld;
    RTL_HEAP_DEFINITION  heapParams;

    //
    // Initialize some heap
    //
    memset( &heapParams, 0, sizeof( RTL_HEAP_DEFINITION ));
    heapParams.Length = sizeof( RTL_HEAP_DEFINITION );
    Heap = RtlCreateHeap( 2, 0, 0x100000, 0x1000, 0, &heapParams );

    //
    // Point at command line
    //
    commandLine = &Argument->Environment->CommandLine;

    //
    // Locate the argument
    //
    argPtr = commandLine->Buffer;
    while( *argPtr != L' ' ) argPtr++;
    argPtr++;

    //
    // Print out the argument
    //
    stringBuffer = RtlAllocateHeap( Heap, 0, 256 );
    swprintf( stringBuffer, L"\n%s", argPtr );
    helloWorld.Buffer = L"das ist der allerneueste test";
    helloWorld.Length = wcslen( L"das ist der allerneueste test" ) * sizeof(WCHAR);
    helloWorld.MaximumLength = helloWorld.Length + sizeof(WCHAR);
    NtDisplayString( &helloWorld );

    testKeyboard();

    setComputerName(L"letzter\0\0");
    openFile(NULL);
    //
    // Free heap
    //
    RtlFreeHeap( Heap, 0, stringBuffer );



    //
    // Terminate
    //
    NtTerminateProcess( NtCurrentProcess(), 0 );
}