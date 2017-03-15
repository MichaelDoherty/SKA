//-----------------------------------------------------------------------------
// SKS_ReaderWriter.h
//	 Reads and writes skeletons to/from SKA Skeleton Files (SKS)
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

#ifndef SKS_READERWRITER_DOT_H
#define SKS_READERWRITER_DOT_H
#include <Core/SystemConfiguration.h>
#include <Animation/Skeleton.h>

class SKA_LIB_DECLSPEC SKS_ReaderWriter
{
public:
	static Skeleton* readSKS(const char* inputFilename);
	static bool writeSKS(const char* outputFilename,
		Skeleton* skeleton,
		bool overwrite=true);
};

#endif