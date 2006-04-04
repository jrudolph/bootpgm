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