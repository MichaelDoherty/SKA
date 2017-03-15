//-----------------------------------------------------------------------------
// SKM_ReaderWriter.h
//	 Reads and writes motions to/from SKA motion files (SKM)
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

#ifndef SKM_READERWRITER_DOT_H
#define SKM_READERWRITER_DOT_H
#include <Core/SystemConfiguration.h>
#include <Animation/Skeleton.h>
#include <Animation/MotionSequence.h>

class SKA_LIB_DECLSPEC SKM_ReaderWriter
{
public:
	static MotionSequence* readSKM(
		const char* inputFilename,
		Skeleton* skeleton);
	static bool writeSKM(
		const char* outputFilename,
		Skeleton* skeleton,
		MotionSequence* ms,
		bool overwrite=true);
};

#endif