//-----------------------------------------------------------------------------
// AMC_Writer.h
//	 Writes an Acclaim AMC file (motion data).
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
#ifndef AMC_WRITER_DOT_H
#define AMC_WRITER_DOT_H
#include "Core/SystemConfiguration.h"

class SkeletonDefinition;
class MotionSequence;

class AMC_Writer
{
public:
	bool writeAMC(const char* outputFilename, 
		SkeletonDefinition* skeleton, 
		MotionSequence* motion,
		bool overwrite=true);
};

#endif
