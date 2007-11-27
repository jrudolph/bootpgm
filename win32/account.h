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

void *get_entry(char *data,unsigned int cEntries,unsigned int index,unsigned int *length);
void *get_V_entry(char *data,unsigned int index, unsigned int *length);

entry *read_entries(char *data,unsigned int cEntries);
int write_entries(entry *entries,unsigned int cEntries,char *buffer,unsigned int length);