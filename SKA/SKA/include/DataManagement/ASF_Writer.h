//-----------------------------------------------------------------------------
// ASF_Writer.h
//	 Writes Acclaim Skeleton File (ASF)
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
#ifndef ASF_WRITER_DOT_H
#define ASF_WRITER_DOT_H
#include "Core/SystemConfiguration.h"

class SkeletonDefinition;

class ASF_Writer
{
public:
	bool writeASF(const char* outputFilename,
		SkeletonDefinition* skeleton,
		bool overwrite=true);
};

#endif