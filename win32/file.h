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

class File
{
	HANDLE fileHandle;
	String name;
	ACCESS_MASK access;
public:
	HANDLE open()
	{
		IO &io=*file_tools::io;
		HANDLE result;

		NTSTATUS Status;
		OBJECT_ATTRIBUTES ObjectAttributes;
		IO_STATUS_BLOCK Iosb;

		InitializeObjectAttributes(&ObjectAttributes,
                               &name.getUnicodeString(),			     
                               OBJ_CASE_INSENSITIVE, 
                               NULL,                 
                               NULL );

		Status = ZwCreateFile(&result,						// returned file handle
                          access,						    // desired access
                          &ObjectAttributes,                // ptr to object attributes
                          &Iosb,                            // ptr to I/O status block
                          0,                                // allocation size
                          FILE_ATTRIBUTE_NORMAL,            // file attributes
                          0,                                // share access
                          FILE_OPEN,                        // create disposition
                          FILE_SYNCHRONOUS_IO_NONALERT,     // create options
                          NULL,                             // ptr to extended attributes
                          0);                               // length of ea buffer

		CHECK_STATUS(Status,÷ffnen einer Datei);

		fileHandle=result;
		return result;
	}
	File(char *name,ACCESS_MASK access):access(access),name(name)
	{
		open();
	}
	virtual ~File()
	{
		IO &io=*file_tools::io;
		NTSTATUS Status=ZwClose(fileHandle);
		CHECK_STATUS(Status,Schlieﬂen der Datei);
	}
};