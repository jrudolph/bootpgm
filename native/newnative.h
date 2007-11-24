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
 *	  Tomasz Nowak <tommy@ntinternals.net>
 *
 * Most of this content comes from http://undocumented.ntinternals.net/
 * This page and the information used is written by 
 * Tomasz Nowak <tommy@ntinternals.net>
 * There is an .chm version in ../doc/ntundoc.chm
 * The license presented on the page sounds:
 *
 *
 * LICENSE CONDITIONS
 * This software and / or documentation is provided at no cost
 * and can be redistributed freely, in its entirety or in parts,
 * as long as the Copyright notice and author's name are included.
 * You are hereby permited to use, view, read, copy, print, publish,
 * redistribute and modify this software and / or documentation,
 * under conditions described herein.
 * This software / documentation is provided to you "as is" without
 * warranty of any kind. By using this material you accept all of the
 * related risks and all direct and indirect consequences, including
 * potential data loss and hardware damage.
 * If you do not agree to these license conditions, please do not use
 * our software and / or documentation. 
 * 
 */



#pragma once

extern "C"{
#define PPVOID void**
#define BYTE char

	/* I mostly copied this structures from the source above.
	 * I removed parts which would introduce spurious dependencies.
	 * 
	 */
	typedef struct _RTL_DRIVE_LETTER_CURDIR {
		USHORT Flags;
		USHORT Length;
		ULONG TimeStamp;
		UNICODE_STRING DosPath;
	} RTL_DRIVE_LETTER_CURDIR, *PRTL_DRIVE_LETTER_CURDIR;

	typedef struct _RTL_USER_PROCESS_PARAMETERS {
		ULONG MaximumLength; 
		ULONG Length; 
		ULONG Flags; 
		ULONG DebugFlags; 
		PVOID ConsoleHandle; 
		ULONG ConsoleFlags; 
		HANDLE StdInputHandle; 
		HANDLE StdOutputHandle; 
		HANDLE StdErrorHandle; 
		UNICODE_STRING CurrentDirectoryPath; 
		HANDLE CurrentDirectoryHandle; 
		UNICODE_STRING DllPath; 
		UNICODE_STRING ImagePathName; 
		UNICODE_STRING CommandLine; 
		PVOID Environment; 
		ULONG StartingPositionLeft; 
		ULONG StartingPositionTop; 
		ULONG Width; 
		ULONG Height; 
		ULONG CharWidth; 
		ULONG CharHeight; 
		ULONG ConsoleTextAttributes; 
		ULONG WindowFlags; 
		ULONG ShowWindowFlags; 
		UNICODE_STRING WindowTitle; 
		UNICODE_STRING DesktopName; 
		UNICODE_STRING ShellInfo; 
		UNICODE_STRING RuntimeData; 
		RTL_DRIVE_LETTER_CURDIR DLCurrentDirectory[0x20];
	} RTL_USER_PROCESS_PARAMETERS, *PRTL_USER_PROCESS_PARAMETERS;

	typedef struct _PEB {
		BOOLEAN                 InheritedAddressSpace;
		BOOLEAN                 ReadImageFileExecOptions;
		BOOLEAN                 BeingDebugged;
		BOOLEAN                 Spare;
		HANDLE                  Mutant;
		PVOID                   ImageBaseAddress;
		PVOID/*PPEB_LDR_DATA*/  LoaderData;
		PRTL_USER_PROCESS_PARAMETERS ProcessParameters;
		PVOID                   SubSystemData;
		PVOID                   ProcessHeap;
		PVOID                   FastPebLock;
		PVOID/*PPEBLOCKROUTINE*/FastPebLockRoutine;
		PVOID/*PPEBLOCKROUTINE*/FastPebUnlockRoutine;
		ULONG                   EnvironmentUpdateCount;
		PPVOID                  KernelCallbackTable;
		PVOID                   EventLogSection;
		PVOID                   EventLog;
		PVOID/*PPEB_FREE_BLOCK*/FreeList;
		ULONG                   TlsExpansionCounter;
		PVOID                   TlsBitmap;
		ULONG                   TlsBitmapBits[0x2];
		PVOID                   ReadOnlySharedMemoryBase;
		PVOID                   ReadOnlySharedMemoryHeap;
		PPVOID                  ReadOnlyStaticServerData;
		PVOID                   AnsiCodePageData;
		PVOID                   OemCodePageData;
		PVOID                   UnicodeCaseTableData;
		ULONG                   NumberOfProcessors;
		ULONG                   NtGlobalFlag;
		BYTE                    Spare2[0x4];
		LARGE_INTEGER           CriticalSectionTimeout;
		ULONG                   HeapSegmentReserve;
		ULONG                   HeapSegmentCommit;
		ULONG                   HeapDeCommitTotalFreeThreshold;
		ULONG                   HeapDeCommitFreeBlockThreshold;
		ULONG                   NumberOfHeaps;
		ULONG                   MaximumNumberOfHeaps;
		PPVOID                  *ProcessHeaps;
		PVOID                   GdiSharedHandleTable;
		PVOID                   ProcessStarterHelper;
		PVOID                   GdiDCAttributeList;
		PVOID                   LoaderLock;
		ULONG                   OSMajorVersion;
		ULONG                   OSMinorVersion;
		ULONG                   OSBuildNumber;
		ULONG                   OSPlatformId;
		ULONG                   ImageSubSystem;
		ULONG                   ImageSubSystemMajorVersion;
		ULONG                   ImageSubSystemMinorVersion;
		ULONG                   GdiHandleBuffer[0x22];
		ULONG                   PostProcessInitRoutine;
		ULONG                   TlsExpansionBitmap;
		BYTE                    TlsExpansionBitmapBits[0x80];
		ULONG                   SessionId;
	} PEB, *PPEB;

	typedef struct _RTL_HEAP_DEFINITION {
		ULONG Length;
		ULONG Unknown[12];
	} RTL_HEAP_DEFINITION, *PRTL_HEAP_DEFINITION;

	NTSYSAPI 
	NTSTATUS
	NTAPI
	NtTerminateProcess(
	/*IN*/ HANDLE               ProcessHandle /*OPTIONAL*/,
	/*IN*/ NTSTATUS             ExitStatus );

	NTSYSAPI 
	NTSTATUS
	NTAPI
	NtDisplayString(
	/*IN*/ PUNICODE_STRING      String );

	NTSYSAPI 
	PVOID
	NTAPI
	RtlCreateHeap(
	  /*IN*/ ULONG                Flags,
	  /*IN*/ PVOID                Base /*OPTIONAL*/,
	  /*IN*/ ULONG                Reserve /*OPTIONAL*/,
	  /*IN*/ ULONG                Commit,
	  /*IN*/ BOOLEAN              Lock /*OPTIONAL*/,
	  /*IN*/ PRTL_HEAP_DEFINITION RtlHeapParams /*OPTIONAL*/ );

	NTSYSAPI 
	PVOID
	NTAPI
	RtlAllocateHeap(
	  /*IN*/ PVOID                HeapHandle,
	  /*IN*/ ULONG                Flags,
	  /*IN*/ ULONG                Size );

	NTSYSAPI 
	BOOLEAN
	NTAPI
	RtlFreeHeap(
		/*IN*/ PVOID                HeapHandle,
		/*IN*/ ULONG                Flags /*OPTIONAL*/,
		/*IN*/ PVOID                MemoryPointer );

	NTSYSAPI NTSTATUS NTAPI NtCreateEvent(PHANDLE,ACCESS_MASK,POBJECT_ATTRIBUTES,EVENT_TYPE,BOOLEAN);
	NTSYSAPI NTSTATUS NTAPI NtWaitForMultipleObjects(ULONG handlecount,PHANDLE handles,int wait_type,BOOLEAN alertable,PLARGE_INTEGER timeout);
	NTSYSAPI NTSTATUS NTAPI NtClearEvent(HANDLE Eventhandle);
	NTSYSAPI NTSTATUS NTAPI NtCancelIoFile(HANDLE Filehandle, PIO_STATUS_BLOCK IoStatusBlock);

	typedef enum _OBJECT_INFORMATION_CLASS
	{
		ObjectBasicInformation,			// Result is OBJECT_BASIC_INFORMATION structure
		ObjectNameInformation,			// Result is OBJECT_NAME_INFORMATION structure
		ObjectTypeInformation,			// Result is OBJECT_TYPE_INFORMATION structure
		ObjectAllInformation,			// Result is OBJECT_ALL_INFORMATION structure
		ObjectDataInformation			// Result is OBJECT_DATA_INFORMATION structure
		
	} OBJECT_INFORMATION_CLASS, *POBJECT_INFORMATION_CLASS;

	NTSYSAPI 
	NTSTATUS
	NTAPI
	NtQueryObject(
		HANDLE               ObjectHandle,
		OBJECT_INFORMATION_CLASS ObjectInformationClass,
		PVOID               ObjectInformation,
		ULONG                Length,
		PULONG              ResultLength );
	
	typedef struct _OBJECT_BASIC_INFORMATION {
		ULONG                   Attributes;
		ACCESS_MASK             GrantedAccess;
		ULONG                   HandleCount;
		ULONG                   ReferenceCount;
		ULONG                   PagedPoolQuota;
		ULONG                   NonPagedPoolQuota;
		ULONG					Unknown[3];
		ULONG					NameInformationLength;
		ULONG					TypeInformationLength;
		ULONG					SecurityDescriptorLength;
		LARGE_INTEGER			CreateTime;
	} OBJECT_BASIC_INFORMATION, *POBJECT_BASIC_INFORMATION;
}