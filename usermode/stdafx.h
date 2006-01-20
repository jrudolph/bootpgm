// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
#endif
#include <stdio.h>
#include <tchar.h>

namespace NT {
	extern "C" {
#ifndef _X86_
	#define _X86_
#endif
#pragma warning(disable: 4005)
#include <ntddk.h>
#include <ntddkbd.h>
#pragma warning(default: 4005)
	}
}

// TODO: reference additional headers your program requires here
