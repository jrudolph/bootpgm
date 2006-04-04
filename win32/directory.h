class Directory
{
	File file;
	String name;
public:
	Directory(char *name):file(name,FILE_LIST_DIRECTORY),name(name)
	{		
	}
};