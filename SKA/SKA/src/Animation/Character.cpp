//-----------------------------------------------------------------------------
// Character.cpp
//	 A character object bundles up a skeleton definition, an animated skeleton
//   and a motion sequence.
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
#include "Core/SystemConfiguration.h"
#include "Animation/Character.h"
#include "Animation/SkeletonDefinition.h"
#include "Animation/MotionSequence.h"
#include "Animation/AnimSkeleton.h"

Character::~Character()
{
	if (skeleton_definition != NULL) delete skeleton_definition;
	if (skeleton != NULL) delete skeleton;
	if (motion_sequence != NULL) delete motion_sequence;
}