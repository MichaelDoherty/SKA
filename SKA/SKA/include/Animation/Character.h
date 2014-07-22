//-----------------------------------------------------------------------------
// Character.h
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
#ifndef CHARACTER_DOT_H
#define CHARACTER_DOT_H
#include "Core/SystemConfiguration.h"
#include <string>
using namespace std;

class SkeletonDefinition;
class AnimSkeleton;
class MotionSequence;

#ifdef _MSC_VER
template class SKA_LIB_DECLSPEC std::allocator<char>;
template class SKA_LIB_DECLSPEC std::basic_string<char, std::char_traits<char>, std::allocator<char> >;
#endif

class SKA_LIB_DECLSPEC Character
{
public:
	SkeletonDefinition* skeleton_definition;
	AnimSkeleton* skeleton;
	MotionSequence* motion_sequence;
	string description1;
	string description2;
	Character()
		: skeleton_definition(NULL), skeleton(NULL), motion_sequence(NULL)
	{ }
	virtual ~Character();
};

#endif