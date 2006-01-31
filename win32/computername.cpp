#include "stdafx.h"
#include "io.h"

WCHAR KeyNameBuffer[]        = L"\\Registry\\Machine\\SYSTEM\\CurrentControlSet\\Control\\ComputerName\\ComputerName";
WCHAR KeyNameBuffer2[]        = L"\\Registry\\Machine\\SYSTEM\\CurrentControlSet\\Control\\ComputerName\\ActiveComputerName";
WCHAR Tcpip[]        = L"\\Registry\\Machine\\SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters";
WCHAR ComputerNameBuffer[]    = L"ComputerName";

void setRegistryValue(IO &io,WCHAR *keyName,WCHAR *valueName,WCHAR *value)
{
	UNICODE_STRING KeyName, ValueName;
    HANDLE SoftwareKeyHandle;
    ULONG Status;
    OBJECT_ATTRIBUTES ObjectAttributes;
    ULONG Disposition;

	//io.print("Schreibe Registry-Key ");
	
    //DbgBreakPoint();
    //
    // Open the Software key
    //
	NT::RtlInitUnicodeString(&KeyName,keyName);
    InitializeObjectAttributes( &ObjectAttributes, &KeyName,
            OBJ_CASE_INSENSITIVE, NULL, NULL );
    Status = ZwCreateKey( &SoftwareKeyHandle, KEY_ALL_ACCESS,
                    &ObjectAttributes, 0,  NULL, REG_OPTION_NON_VOLATILE,
                    &Disposition );
	
	CHECK_STATUS(Status,Öffnen des Schlüssels)

	return;

	NT::RtlInitUnicodeString(&ValueName,valueName);
    
    Status = ZwSetValueKey( SoftwareKeyHandle, &ValueName, 0, REG_SZ,
                        value,
                        wcslen( value ) * sizeof(WCHAR) );

	CHECK_STATUS(Status,Setzen des Schlüssels);

    Status = ZwClose(SoftwareKeyHandle);

	CHECK_STATUS(Status,Schließen des Schlüssels);
}

void setComputerName(IO &io,WCHAR *computerName)
{
	io.println("Setze Computernamen");
    setRegistryValue(io,KeyNameBuffer,ComputerNameBuffer,computerName);
    setRegistryValue(io,KeyNameBuffer2,ComputerNameBuffer,computerName);
    setRegistryValue(io,Tcpip,L"Hostname",computerName);
    setRegistryValue(io,Tcpip,L"NV Hostname",computerName);
}

void setCompnameFromFile(IO &io,char *args)
{
    NTSTATUS Status;
    UNICODE_STRING UnicodeFilespec;
    OBJECT_ATTRIBUTES ObjectAttributes;
    HANDLE FileHandle;
    IO_STATUS_BLOCK Iosb;
    PWCHAR buffer;
    PWCHAR buffer2;
    ULONG converted;
    wchar_t *valueName= L"\\device\\floppy0\\compname.txt";
	RtlInitUnicodeString(&UnicodeFilespec,valueName);

    InitializeObjectAttributes(&ObjectAttributes,           // ptr to structure
                               &UnicodeFilespec,            // ptr to file spec
                               OBJ_CASE_INSENSITIVE,        // attributes
                               NULL,                        // root directory handle
                               NULL );                      // ptr to security descriptor

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

	CHECK_STATUS(Status,Öffnen der Computernamensdatei)

	buffer = (PWCHAR)io.malloc(256);//RtlAllocateHeap( Heap, 0, 256 );
    Status = ZwReadFile(FileHandle,0,NULL,NULL,&Iosb,buffer,256,0,NULL);
    
	CHECK_STATUS(Status,Lesen des Computernamens);
	
	buffer2 = (PWCHAR)io.malloc(500);//RtlAllocateHeap( Heap, 0, 500 );
    
    ((char*)buffer)[Iosb.Information]=0;
    mbstowcs(buffer2,(char*)buffer,Iosb.Information+1);
    setComputerName(io,buffer2);
    
    Status = ZwClose(FileHandle);

	CHECK_STATUS(Status,Schließen der Datei);
}