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
// Version 3.0 - July 18, 2014 - Michael Doherty
//-----------------------------------------------------------------------------
#ifndef AMC_READER_DOT_H
#define AMC_READER_DOT_H
#include "Core/SystemConfiguration.h"
#include "DataManagement/LineScanner.h"

class SkeletonDefinition;
class MotionSequence;

class AMC_Reader
{
public:
	AMC_Reader() : motion(NULL), AMC_angles_are_degrees(true) { }

	MotionSequence* readAMC(const char* motionFilename, SkeletonDefinition* skeleton);
	
private:
	bool processComment(LineScanner& line_scanner, string& line);
	MotionSequence* motion;
	bool AMC_angles_are_degrees;
};

#endif