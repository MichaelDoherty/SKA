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
// Version 3.0 - July 18, 2014 - Michael Doherty
//-----------------------------------------------------------------------------
#include "Core/SystemConfiguration.h"
#include "DataManagement/FileSystem.h"
#include <fstream>
using namespace std;
#include <cstdlib>
#include <sys/stat.h>

#ifdef _WIN32

// Convert Unix style paths to Windows style paths
static string backslashFilepath(const string& path)
{
	string newpath = path;
	for (unsigned int i=0; i<newpath.length(); i++)
	{
		if (newpath[i] == '/') 
			newpath.replace(i, 1, 1, '\\');
	}
	return newpath;
}

bool FileSystem::makeDir(const string& path)
{
	string newpath = backslashFilepath(path);
	if (dirExists(newpath)) return true;
	string cmd = string("mkdir ") + newpath;
	system(cmd.c_str());
	return dirExists(newpath);
}

bool FileSystem::dirExists(const string& path)
{
	string newpath = backslashFilepath(path);
	struct stat st;
	int result = stat(newpath.c_str(),&st);
	if (result != 0) return false;
	if (st.st_mode & _S_IFDIR) return true;
	return false;
}

bool FileSystem::fileExists(const string& path)
{
	string newpath = backslashFilepath(path);
	struct stat st;
	int result = stat(newpath.c_str(),&st);
	if (result != 0) return false;
	if (st.st_mode & _S_IFREG) return true;
	return false;
}

#else

bool FileSystem::makeDir(const string& path)
{
	if (dirExists(path)) return true;
	string cmd = string("mkdir ") + path;
	system(cmd.c_str());
	return dirExists(path);
}

bool FileSystem::dirExists(const string& path)
{
	struct stat st;
	int result = stat(path.c_str(),&st);
	if (result != 0) return false;
	if (S_ISDIR(st.st_mode)) return true;
	return false;
}

bool FileSystem::fileExists(const string& path)
{
	struct stat st;
	int result = stat(path.c_str(),&st);
	if (result != 0) return false;
	if (S_ISREG(st.st_mode)) return true;
	return false;
}

#endif
