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
#include <string.h>
#include <stdlib.h>

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

using namespace NT;
#include "../native/newnative.h"

// TODO: reference additional headers your program requires here
