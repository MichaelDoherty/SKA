//-----------------------------------------------------------------------------
// Utilities.h
//	 Some useful functions (currently just a string conversion template).
//-----------------------------------------------------------------------------
// This software is part of the Skeleton Animation Toolkit (SKA) developed by 
// at the University of the Pacific, under the guidance of Michael Doherty.
// For information please contact mdoherty@pacific.edu.
//-----------------------------------------------------------------------------
// This is open software. You are free to use it as you see fit.
// The University of the Pacific and identified authors should be
// credited for any significant use, particularly if used for commercial 
// projects or academic research publications.
//-----------------------------------------------------------------------------
// Version 1.0 - January 25, 2012 - Michael Doherty
//-----------------------------------------------------------------------------
#ifndef UTILITIES_DOT_H
#define UTILITIES_DOT_H
#include "Core/SystemConfiguration.h"
#include <string>
#include <sstream>
#include <fstream>
using namespace std;

template <class T>
inline string toString(T x)
{
	stringstream ss;
	ss << x;
	return ss.str();
}

#endif