//-----------------------------------------------------------------------------
// BasicException.h
//	 Base class for all SKA exceptions.
//-----------------------------------------------------------------------------
// This software is part of the Skeleton Animation Toolkit (SKA) developed 
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
#ifndef BASICEXCEPTION_DOT_H
#define BASICEXCEPTION_DOT_H
#include "Core/SystemConfiguration.h"

#include <string>
using namespace std;

#ifdef _MSC_VER
template class SKA_LIB_DECLSPEC std::allocator<char>;
template class SKA_LIB_DECLSPEC std::basic_string<char, std::char_traits<char>, std::allocator<char> >;
#endif

class SKA_LIB_DECLSPEC BasicException
{
public:
	BasicException() : msg("Unspecified exception") { }
	BasicException(const string& _msg) : msg(_msg) { }
	string msg;
};

#endif
