//-----------------------------------------------------------------------------
// BVH_Reader.h
//	 Reads Biovision Hierarchical data file (BVH)
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

#ifndef BVH_READER_DOT_H
#define BVH_READER_DOT_H
#include <Core/SystemConfiguration.h>
#include <DataManagement/ParsingUtilities.h>
#include <Animation/Skeleton.h>
#include <Animation/MotionSequence.h>
#include <Core/Array2D.h>

#include <list>
#include <vector>
#include <iostream>
using namespace std;

class SKA_LIB_DECLSPEC BVH_Reader
{
public:
	BVH_Reader();
	virtual ~BVH_Reader();
	pair<Skeleton*, MotionSequence*> readBVH(const char* inputFilename);
};

#endif