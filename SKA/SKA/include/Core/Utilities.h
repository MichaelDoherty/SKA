//-----------------------------------------------------------------------------
// Utilities.h
//	 Some useful functions (currently just a string conversion template).
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

#ifndef UTILITIES_DOT_H
#define UTILITIES_DOT_H
#include <Core/SystemConfiguration.h>
#include <cstring>
#include <string>
#include <sstream>
#include <fstream>
using namespace std;

// allocate memory for a copy of s and copy s into the clone
inline char* strClone(const char* s)
{
	char* t = new char[strlen(s)+1];
	strcpy(t,s);
	return t;
}

inline void strDelete(char* s)
{
	if (s != NULL) delete [] s;
}

template <class T>
inline string toString(T x)
{
	stringstream ss;
	ss << x;
	return ss.str();
}

#endif