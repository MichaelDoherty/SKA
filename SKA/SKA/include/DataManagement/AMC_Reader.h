//-----------------------------------------------------------------------------
// AMC_Reader.h
//	 Reads an Acclaim AMC file (motion data).
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

#ifndef AMC_READER_DOT_H
#define AMC_READER_DOT_H
#include <Core/SystemConfiguration.h>

class Skeleton;
class MotionSequence;

class SKA_LIB_DECLSPEC AMC_Reader
{
public:
	AMC_Reader() : motion(NULL), AMC_angles_are_degrees(true) { }

	MotionSequence* readAMC(const char* motionFilename, Skeleton* skeleton);
	
private:
	MotionSequence* motion;
	bool AMC_angles_are_degrees;
};

#endif