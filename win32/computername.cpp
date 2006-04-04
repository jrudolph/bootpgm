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

	NT::RtlInitUnicodeString(&ValueName,valueName);
    
    Status = ZwSetValueKey( SoftwareKeyHandle, &ValueName, 0, REG_SZ,
                        value,
                        (wcslen( value )+1) * sizeof(WCHAR) );

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

void setComputerNameCmd(IO &io,char *args)
{
	if (strlen(args)<2)
	{
		io.println("Syntax: setComputerName <newComputerName>");
		return;
	}
	io.print("Setting Computer Name to: ");
	io.println(args+1);
	setComputerName(io,io.char2wchar(args+1));
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
	wchar_t *valueName;

	if (mainSingleton->getArgc()>1)
		valueName=io.char2wchar(mainSingleton->getArgs()[1]);
	else 
		valueName= L"\\device\\floppy0\\compname.txt";

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
    ((char*)buffer)[Iosb.Information]=0;

	CHECK_STATUS(Status,Lesen des Computernamens);
	
	buffer2 = (PWCHAR)io.malloc(500);//RtlAllocateHeap( Heap, 0, 500 );
    
	io.print("Computername aus Datei: ");
	io.println((char*)buffer);
    
    mbstowcs(buffer2,(char*)buffer,Iosb.Information+1);
    setComputerName(io,buffer2);
    
    Status = ZwClose(FileHandle);

	CHECK_STATUS(Status,Schließen der Datei);
}