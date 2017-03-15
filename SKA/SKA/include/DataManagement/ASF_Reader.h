//-----------------------------------------------------------------------------
// ASF_Reader.h
//	 Reads Acclaim Skeleton File (ASF)
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

#ifndef ASF_READER_DOT_H
#define ASF_READER_DOT_H
#include <Core/SystemConfiguration.h>
#include <DataManagement/ParsingUtilities.h>
#include <Animation/Skeleton.h>

class ASF_Reader_Local;

class SKA_LIB_DECLSPEC ASF_Reader
{
public:
	ASF_Reader();
	~ASF_Reader();
	Skeleton* readASF(const char* inputFilename);
private:
	ASF_Reader_Local* local_reader;
};

#endif