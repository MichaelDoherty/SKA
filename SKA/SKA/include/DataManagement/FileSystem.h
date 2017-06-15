//-----------------------------------------------------------------------------
// FileSystem.h
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

#ifndef FILESYSTEM_DOT_H
#define FILESYSTEM_DOT_H
#include <string>
using namespace std;
#include <Core/SystemConfiguration.h>

class SKA_LIB_DECLSPEC FileSystem
{
public:
	// Convert Unix style paths to Windows style paths
	// Converts path parameter in place and returns pointer to same.
	static char* backslashFilepath(char* path);
	const static char* backslashFilepath(string& path);

	// Convert Windows style paths to Unix style paths
	// Converts path parameter in place and returns pointer to same.
	static char* forwardslashFilepath(char* path);
	const static char* forwardslashFilepath(string& path);

	// Creates the directory specified by path and returns success or failure of the operation.
	static bool makeDir(const char* path);

	// Returns true if path specifies a valid directory.
	static bool dirExists(const char* path);

	// Returns true if path specifies a valid directory.
	static bool fileExists(const char* path);
};

#endif
