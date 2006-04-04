//======================================================================
//
// Native.h
//
// Mark Russinovich
// http://www.ntinternals.com
//
// This file includes the definitions required by the Native.exe sample
// NT native program to do what it does.
//
//======================================================================

#pragma once

extern "C"{
//
// Environment information, which includes command line and
// image file name
//
typedef struct {
       ULONG            Unknown[21];
       UNICODE_STRING   CommandLine;
       UNICODE_STRING   ImageFile;
} ENVIRONMENT_INFORMATION, *PENVIRONMENT_INFORMATION;

//
// This structure is passed as NtProcessStartup's parameter
//
typedef struct {
       ULONG                     Unknown[3];
       PENVIRONMENT_INFORMATION  Environment;
} STARTUP_ARGUMENT, *PSTARTUP_ARGUMENT;

//
// Data structure for heap definition. This includes various
// sizing parameters and callback routines, which, if left NULL,
// result in default behavior
//
typedef struct {
	ULONG    	Length;
	ULONG    	Unknown[11];
} RTL_HEAP_DEFINITION, *PRTL_HEAP_DEFINITION;

//
// Native NT api function to write something to the boot-time
// blue screen
//
NTSTATUS
NTAPI
NtDisplayString(
		PUNICODE_STRING String
		);

//
// Native applications must kill themselves when done - the job
// of this native API
//
NTSTATUS
NTAPI
NtTerminateProcess(
		   HANDLE ProcessHandle,
		   LONG ExitStatus
		   );

//
// Definition to represent current process
//
//#define NtCurrentProcess() ( (HANDLE) -1 )

//
// Heap creation routine
//
HANDLE
NTAPI
RtlCreateHeap(
	      ULONG Flags,
	      PVOID BaseAddress,
	      ULONG SizeToReserve,
	      ULONG SizeToCommit,
	      PVOID Unknown,
	      PRTL_HEAP_DEFINITION Definition
	      );

//
// Heap allocation function (ala "malloc")
//
PVOID
NTAPI
RtlAllocateHeap(
		HANDLE Heap,
		ULONG Flags,
		ULONG Size
		);

//
// Heap free function (ala "free")
//
BOOLEAN
NTAPI
RtlFreeHeap(
	    HANDLE Heap,
	    ULONG Flags,
	    PVOID Address
	    );

NTSYSAPI NTSTATUS NTAPI NtCreateEvent(PHANDLE,ACCESS_MASK,POBJECT_ATTRIBUTES,EVENT_TYPE,BOOLEAN);
NTSYSAPI NTSTATUS NTAPI NtWaitForMultipleObjects(ULONG handlecount,PHANDLE handles,int wait_type,BOOLEAN alertable,PLARGE_INTEGER timeout);
NTSYSAPI NTSTATUS NTAPI NtClearEvent(HANDLE Eventhandle);
NTSYSAPI NTSTATUS NTAPI NtCancelIoFile(HANDLE Filehandle, PIO_STATUS_BLOCK IoStatusBlock);
NTSYSAPI 
NTSTATUS
NTAPI

LdrGetDllHandle(

int pwPath , int Unused , PUNICODE_STRING ModuleFileName, PHANDLE pHModule ); 

NTSYSAPI 
NTSTATUS
NTAPI

LdrGetProcedureAddress(

HANDLE ModuleHandle, PANSI_STRING FunctionName , int Oridinal , PVOID *FunctionAddress ); 
NTSYSAPI 
NTSTATUS
NTAPI

LdrLoadDll(
IN PWCHAR PathToFile OPTIONAL, IN ULONG Flags OPTIONAL, IN PUNICODE_STRING ModuleFileName, OUT PHANDLE ModuleHandle ); 
} // extern "C" end

