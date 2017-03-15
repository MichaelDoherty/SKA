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

#ifndef AMC_WRITER_DOT_H
#define AMC_WRITER_DOT_H
#include <Core/SystemConfiguration.h>

class Skeleton;
class MotionSequence;

class SKA_LIB_DECLSPEC AMC_Writer
{
public:
	bool writeAMC(const char* outputFilename, 
		Skeleton* skeleton, 
		MotionSequence* motion,
		bool overwrite=true);
};

#endif
