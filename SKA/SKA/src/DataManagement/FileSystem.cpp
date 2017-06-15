//-----------------------------------------------------------------------------
// FileSystem.cpp
//	 Interface to basic file system functionality, which
//   hides OS specific details.
//-----------------------------------------------------------------------------
// This software is part of the Skeleton Animation Toolkit (SKA) developed 
// at the University of the Pacific, under the guidance of Michael Doherty.
// For information please contact mdoherty@pacific.edu.
//-----------------------------------------------------------------------------
// This is open software. You are free to use it as you see fit.
// The University of the Pacific and identified authors would appreciate
// being credited for any significant use, particularly if used for
// commercial projects or academic research publications.
//-----------------------------------------------------------------------------

#include <Core/SystemConfiguration.h>
#include <fstream>
using namespace std;
#include <cstdlib>
#include <sys/stat.h>
#include <Core/Utilities.h>
#include <DataManagement/FileSystem.h>

#ifdef _WIN32

char* FileSystem::backslashFilepath(char* path)
{
	unsigned int i;
	for (i = 0; i<strlen(path); i++)
	{
		if (path[i] == '/') path[i] = '\\';
	}
	return path;
}

const char* FileSystem::backslashFilepath(string& path)
{
	unsigned int i;
	for (i = 0; i<path.length(); i++)
	{
		if (path[i] == '/') path[i] = '\\';
	}
	return path.c_str();
}

char* FileSystem::forwardslashFilepath(char* path)
{
	unsigned int i;
	for (i = 0; i<strlen(path); i++)
	{
		if (path[i] == '\\') path[i] = '/';
	}
	return path;
}

const char* FileSystem::forwardslashFilepath(string& path)
{
	unsigned int i;
	for (i = 0; i<path.length(); i++)
	{
		if (path[i] == '\\') path[i] = '/';
	}
	return path.c_str();
}

bool FileSystem::makeDir(const char* path)
{
	char* newpath = strClone(path);
	backslashFilepath(newpath);
	if (dirExists(newpath)) return true;
	string cmd = string("mkdir ") + newpath;
	system(cmd.c_str());
	bool answer = dirExists(newpath);
	strDelete(newpath);
	return answer;
}

bool FileSystem::dirExists(const char* path)
{
	char* newpath = strClone(path);
	backslashFilepath(newpath);
	struct stat st;
	int result = stat(newpath,&st);
	strDelete(newpath);
	if (result != 0) return false;
	if (st.st_mode & _S_IFDIR) return true;
	return false;
}

bool FileSystem::fileExists(const char* path)
{
	char* newpath = strClone(path);
	backslashFilepath(newpath);
	struct stat st;
	int result = stat(newpath,&st);
	strDelete(newpath);
	if (result != 0) return false;
	if (st.st_mode & _S_IFREG) return true;
	return false;
}

#else

bool FileSystem::makeDir(const char* path)
{
	if (dirExists(path)) return true;
	string cmd = string("mkdir ") + path;
	system(cmd.c_str());
	return dirExists(path);
}

bool FileSystem::dirExists(const char* path)
{
	struct stat st;
	int result = stat(path,&st);
	if (result != 0) return false;
	if (S_ISDIR(st.st_mode)) return true;
	return false;
}

bool FileSystem::fileExists(const char* path)
{
	struct stat st;
	int result = stat(path,&st);
	if (result != 0) return false;
	if (S_ISREG(st.st_mode)) return true;
	return false;
}

#endif
