//-----------------------------------------------------------------------------
// BoneDefinition.h
//	 Stores the static definition of a bone.
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
#ifndef BONE_DEFINITION_DOT_H
#define BONE_DEFINITION_DOT_H
#include "Core/SystemConfiguration.h"
#include <iostream>
using namespace std;
#include <float.h>
#include "Math/Vector3D.h"
#include "Core/DOF.h"

struct BoneDefinition
{
	friend ostream& operator<<(ostream& out, BoneDefinition& bone);

	string name;
	
	float  length;				// length of bone
	Vector3D direction;			// direction vector (in parent's coordinate system)

	Vector3D axis;				// axis of rotation
	DOF_ID axis_order[3];		// order of axis rotations 

	// parameters for each degree of freedom, indexed by DOF_ID
	struct DOF
	{
		bool valid;
		float min;
		float max;
		DOF() : valid(false), min(-1.0f*FLT_MAX), max(FLT_MAX) { }
	} dof[6];
	
	// order that DOFs will appear in the AMC 
	// and order that they should be applied
	DOF_ID channel_order[6];  
	
	BoneDefinition()
		: name("UNNAMED"), length(1.0f), direction(0.0f,0.0f,1.0f), axis(0.0f,0.0f,1.0f)
	{
		axis_order[0] = DOF_X;
		axis_order[1] = DOF_Y;
		axis_order[2] = DOF_Z;
		channel_order[0] = DOF_INVALID;
		channel_order[1] = DOF_INVALID;
		channel_order[2] = DOF_INVALID;
		channel_order[3] = DOF_INVALID;
		channel_order[4] = DOF_INVALID;
		channel_order[5] = DOF_INVALID;
	}
};

#endif
