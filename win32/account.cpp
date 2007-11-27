#include "stdafx.h"

#include "account.h"

void *get_entry(char *data,unsigned int cEntries,unsigned int index,unsigned int *length)
{
	data_entry *entries = reinterpret_cast<data_entry*>(data);
	if (length != 0)
		*length = entries[index].length;

	return data + sizeof(data_entry) * cEntries + entries[index].position;
}

void *get_V_entry(char *data,unsigned int index, unsigned int *length)
{
	return get_entry(data,17,index,length);
}

entry *read_entries(char *data,unsigned int cEntries)
{
	data_entry *entries = reinterpret_cast<data_entry*>(data);
	entry *fixed = new entry[cEntries];
	for(unsigned int i=0;i<cEntries;i++)
	{
		fixed[i].data = data + sizeof(data_entry) * cEntries + entries[i].position;
		fixed[i].length = entries[i].length;
		fixed[i].flags = entries[i].flags;
	}
	return fixed;
}

int align(int i)
{
	int mod=i%4;
	if (mod==0)
		return i;
	else
		return i + 4 - mod;
}

int write_entries(entry *entries,unsigned int cEntries,char *buffer,unsigned int length)
{
	data_entry *pents = reinterpret_cast<data_entry*>(buffer);

	int pos = 0;
	
	for(unsigned int i=0;i<cEntries;i++)
	{
		pents[i].position = pos;
		pents[i].length = entries[i].length;
		pents[i].flags = entries[i].flags;

		char *dataOffset = buffer + sizeof(data_entry) * cEntries + pos;

		memcpy(dataOffset,entries[i].data,entries[i].length);
		pos += align(entries[i].length);
	}

	return sizeof(data_entry) * cEntries + pos;
}