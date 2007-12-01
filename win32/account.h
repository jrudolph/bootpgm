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
 *
 * Work on this sructures was inspired by the Petter Nordahl's
 * Nt Offline Password Recovery tool.
 *
 * see http://home.eunet.no/pnordahl/ntpasswd/
 */ 

#pragma once

//#pragma pack(4)
struct data_entry {
	unsigned int position;
	unsigned int length;
	unsigned int flags;
};

struct entry {
	void *data;
	unsigned int length;
	unsigned int flags;
};

#define V_ENTRY_COUNT 17

#define USERNAME_E 1
#define FULLNAME_E 2
#define LANMANPW_E 13
#define NTPW_E 14

void *get_entry(char *data,unsigned int cEntries,unsigned int index,unsigned int *length);
void *get_V_entry(char *data,unsigned int index, unsigned int *length);

entry *read_entries(char *data,unsigned int cEntries);
int write_entries(entry *entries,unsigned int cEntries,char *buffer,unsigned int length);