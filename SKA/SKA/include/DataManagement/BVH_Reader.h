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
// Version 3.0 - July 18, 2014 - Michael Doherty
//-----------------------------------------------------------------------------
#ifndef BVH_READER_DOT_H
#define BVH_READER_DOT_H
#include "Core/SystemConfiguration.h"
#include "DataManagement/LineScanner.h"
#include "DataManagement/ASF_AMC_ParseUtils.h"
#include "Animation/SkeletonDefinition.h"
#include "Animation/MotionSequence.h"
#include "Core/Array2D.h"

#include <list>
#include <vector>
#include <iostream>
using namespace std;

class SKA_LIB_DECLSPEC BVH_Reader
{
public:
	BVH_Reader();
	virtual ~BVH_Reader();
	pair<SkeletonDefinition*, MotionSequence*> readBVH(const char* inputFilename);

	// HACK! This scales bones as they are read from the BVH. 
	// It would probably be cleaner if bones were scaled in the resulting skeleton later.
	float size_scale;
	void setSizeScale(float s) { size_scale = s; }
};

#endif