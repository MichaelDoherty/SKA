//-----------------------------------------------------------------------------
// Blender.h
//	 Class for blending motions. 
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

#ifndef BLENDER_DOT_H
#define BLENDER_DOT_h
#include <Core/SystemConfiguration.h>
#include <vector>
#include <fstream>
using namespace std;
#include <Animation/Channel.h>
#include <Animation/MotionSequence.h>

// DESIGN PRINCIPLES:
// Blender is "functional" in the sense that it is
//   (1) stateless
//   (2) inputs are immutable
//   (3) outputs are always new objects
// This is obviously an inefficient choice, but it will keep design clean.

struct BLEND_INPUT
{
	MotionSequence* ms;
	float blend_weight;
	short start_frame;
	short end_frame;
};

struct CHANNEL_BLEND_SPEC
{
	CHANNEL_ID channel;
	vector<BLEND_INPUT> inputs;
	short start_frame;
	short end_frame;
};

struct MOTION_BLEND_SPEC
{
	vector<CHANNEL_BLEND_SPEC> channel_specs;
	short num_frames;
};

class SKA_LIB_DECLSPEC Blender
{
private:
public:
	MotionSequence* blend(MOTION_BLEND_SPEC& spec);
};

SKA_LIB_DECLSPEC ostream& operator<<(ostream& out, MOTION_BLEND_SPEC& spec);

#endif
