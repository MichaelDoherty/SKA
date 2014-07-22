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
// Version 3.0 - July 18, 2014 - Michael Doherty
//-----------------------------------------------------------------------------
#ifndef FILESYSTEM_DOT_H
#define FILESYSTEM_DOT_H
#include "Core/SystemConfiguration.h"
#include <string>
using namespace std;

class FileSystem
{
public:
	static bool makeDir(const string& path);
	static bool dirExists(const string& path);
	static bool fileExists(const string& path);
};

#endif
