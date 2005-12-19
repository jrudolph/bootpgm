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
#include "stdio.h"
#include "native.h"


WCHAR KeyNameBuffer[]		= L"\\Registry\\Machine\\SYSTEM\\CurrentControlSet\\Control\\ComputerName\\ComputerName";
WCHAR KeyNameBuffer2[]		= L"\\Registry\\Machine\\SYSTEM\\CurrentControlSet\\Control\\ComputerName\\ActiveComputerName";
WCHAR Tcpip[]		= L"\\Registry\\Machine\\SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters";
WCHAR ComputerNameBuffer[]	= L"ComputerName";

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

//
// Our heap
//
HANDLE Heap;
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
    helloWorld.Buffer = L"das ist der neueste test";
    helloWorld.Length = wcslen( L"das ist der neueste test" ) * sizeof(WCHAR);
    helloWorld.MaximumLength = helloWorld.Length + sizeof(WCHAR);
    NtDisplayString( &helloWorld );
	
    setComputerName(L"letzter\0\0");
    //
    // Free heap
    //
    RtlFreeHeap( Heap, 0, stringBuffer );
    
    

    //
    // Terminate
    //
    NtTerminateProcess( NtCurrentProcess(), 0 );
}