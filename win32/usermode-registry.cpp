// usermode-registry.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"

using namespace NT;
/*#include "ntddk.h" // include this for its native functions and defn's
#include "stdio.h"
#include "stdlib.h"*/

//WCHAR KeyNameBuffer[]        = L"\\Registry\\Machine\\SOFTWARE";
WCHAR KeyNameBuffer[]        = L"\\Registry\\Machine\\SYSTEM\\CurrentControlSet\\Control\\ComputerName\\ComputerName";
WCHAR ComputerNameBuffer[]    = L"ComputerName";

void setRegistryValue(WCHAR *keyName,WCHAR *valueName,WCHAR *value)
{
UNICODE_STRING KeyName, ValueName;
    HANDLE SoftwareKeyHandle;
    ULONG Status;
    OBJECT_ATTRIBUTES ObjectAttributes;
    ULONG Disposition;

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
                        wcslen( value ) * sizeof(WCHAR)+2 );

    Status = ZwClose(SoftwareKeyHandle);
}

void setComputerName(WCHAR *computerName)
{
    setRegistryValue(KeyNameBuffer,ComputerNameBuffer,computerName);
}

char* Message = "Hello world! I was written using the native NT API.\n";
void openFile(WCHAR *name)
{
    NTSTATUS Status;
    UNICODE_STRING UnicodeFilespec;
    OBJECT_ATTRIBUTES ObjectAttributes;
    HANDLE FileHandle;
    IO_STATUS_BLOCK Iosb;
    ULONG MessageLength = strlen(Message);

    //printf("Starting OSR's Native NT API Example...\n");

    //
    // Initialize a unicode string with the fully qualified path of the file
    // that we wish to create
    //
    RtlInitUnicodeString(&UnicodeFilespec, L"\\device\\floppy0\\Temp\\native.txt");


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
                          FILE_OPEN,                          // create disposition
                          FILE_DIRECTORY_FILE,     // create options
                          NULL,                             // ptr to extended attributes
                          0);                               // length of ea buffer

    //
    // The file has been successfully created.  Let's try WRITING to it!
    // Note we don't use, or need, an event handle here since we've opened
    // the file for synchronous I/O.
    //
  /*  Status = ZwWriteFile(FileHandle,                   // file Handle
                         0,                            // event Handle
                         NULL,                         // APC entry point
                         NULL,                         // APC context
                         &Iosb,                        // IOSB address
                         Message,                      // ptr to data buffer
                         MessageLength,                // length
                         0,                            // byte offset
                         NULL);                        // key
*/
    //Status = ZwReadFile(FileHandle,0,NULL,NULL,&Iosb,

       //
    // Well, That's all folks!
    //
    Status = ZwClose(FileHandle);
}

extern "C"{
NTSYSAPI NTSTATUS NTAPI NtCreateEvent(PHANDLE,ACCESS_MASK,POBJECT_ATTRIBUTES,EVENT_TYPE,BOOLEAN);
NTSYSAPI NTSTATUS NTAPI
  NtReadFile(
    HANDLE  FileHandle,
    HANDLE  Event  OPTIONAL,
    PIO_APC_ROUTINE  ApcRoutine  OPTIONAL,
    PVOID  ApcContext  OPTIONAL,
    PIO_STATUS_BLOCK  IoStatusBlock,
    PVOID  Buffer,
    ULONG  Length,
    PLARGE_INTEGER  ByteOffset  OPTIONAL,
    PULONG  Key  OPTIONAL
    );
NTSYSAPI NTSTATUS NTAPI NtWaitForMultipleObjects(ULONG handlecount,PHANDLE handles,int wait_type,BOOLEAN alertable,PLARGE_INTEGER timeout);
}

int main2(int argc, _TCHAR* argv[])
{
//    setComputerName(L"GAMBISTICS2");
    //openFile(NULL);

	unsigned long Status;
    UNICODE_STRING UnicodeFilespec;
    OBJECT_ATTRIBUTES ObjectAttributes;
    HANDLE FileHandle;
    IO_STATUS_BLOCK Iosb;
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

	HANDLE eventHandle;
	InitializeObjectAttributes(&ObjectAttributes,           // ptr to structure
                               NULL,            // ptr to file spec
                               0,        // attributes
                               NULL,                        // root directory handle
                               NULL );                      // ptr to security descriptor
	Status=NtCreateEvent(&eventHandle,EVENT_ALL_ACCESS,&ObjectAttributes,SynchronizationEvent,FALSE);
	
	KEYBOARD_INPUT_DATA kid;
	
	//char *buffer=new char[1000];
	LARGE_INTEGER bo;
	bo.HighPart=0;
	bo.LowPart=0;

	Status=NtReadFile(FileHandle,
			eventHandle,0,0,&Iosb,&kid,sizeof(KEYBOARD_INPUT_DATA),&bo,NULL);

	NtWaitForMultipleObjects(1,&eventHandle,1,1,0);
			
//	delete []buffer;

    return 0;
}