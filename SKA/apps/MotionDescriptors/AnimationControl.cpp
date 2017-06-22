//-----------------------------------------------------------------------------
// MotionDescriptors project - Builds with SKA Version 4.0
//-----------------------------------------------------------------------------
// AnimationControl.cpp
//-----------------------------------------------------------------------------
// SKA configuration
#include <Core/SystemConfiguration.h>
// C/C++ libraries
#include <cstdio>
#include <complex>
// SKA modules
#include <Core/Utilities.h>
#include <Animation/MotionSequenceController.h>
#include <Animation/AnimationException.h>
#include <Animation/Skeleton.h>
#include <DataManagement/DataManager.h>
#include <DataManagement/DataManagementException.h>
// local application
#include "AppConfig.h"
#include "AnimationControl.h"

// global single instance of the animation controller
AnimationControl anim_ctrl;

// scale the character to about 20%
// Most things in SKA were developed to fit the CMU ASF skeletons.
// The BVH files from Pacific's mocap lab are about 5 times to large
// for this world. In particular, bones will be too thin if stretched
// to 500% of expected size, since the bone width does not currently scale.
float BVH_character_size_scale = 0.2f;
float AMC_character_size_scale = 1.0f;

AnimationControl::AnimationControl() : character(NULL), file_path_defined(false)
{
	reset();
}

AnimationControl::~AnimationControl()
{
	reset();
}

void AnimationControl::reset()
{
	ready = false;
	run_time = 0.0f;
	single_step = false;
	freeze = false;
	time_warp = 1.0f;

	if (character != NULL) delete character;
	character = NULL;

	// Initialize current_frame to -1. It will be incremented before use if we
	// are in non-realtime mode. It will be computed from elapsed time if 
	// we are in realtime mode.
	current_frame = -1; 
	loop_count = 0;
}

bool AnimationControl::getBonePosition(const char* bonename, Vector3D& start, Vector3D& end)
{
	if (character->boneIdFromName(bonename) < 0) return false;
	character->getBonePositions(bonename, start, end);
	return true;
}

bool AnimationControl::getBoneOrientation(const char* bonename, Vector3D& orientation)
{
	if (character->boneIdFromName(bonename) < 0) return false;
	character->getBoneOrientation(bonename, orientation);
	return true;
}

bool AnimationControl::updateAnimation(float _elapsed_time)
{
	if (!ready || (character == NULL)) return false;

	// time warp simply scales the elapsed time
	_elapsed_time *= time_warp;

	if (!freeze || single_step) //
	{
		run_time += _elapsed_time;
		character->update(run_time);
	}
	if (single_step) // single step then freeze AFTER this frame
	{
		freeze = true;
		single_step = false;
	}

	// FIXIT:170118 The frame should be extracted from the skeleton & motion controller
	//              to ensure proper synchronization.
	current_frame = long(run_time / frame_duration);
	if (current_frame > num_frames)
	{
		loop_count++;
	}

	return true;
}

bool AnimationControl::framestepAnimation()
{
	if (!ready || (character == NULL)) return false;

	if (!freeze || single_step)
	{
		// compute time based on the recorded frame rate
		current_frame++;
		if (current_frame > num_frames)
		{
			current_frame = 0;
			loop_count++;
		}
		run_time = current_frame*frame_duration;
		character->update(run_time);
	}
	if (single_step) // single step then freeze AFTER this frame
	{
		freeze = true;
		single_step = false;
	}

	return true;
}

bool AnimationControl::loadBVHCharacter(string& BVH_filename)
{
	reset();
	// Search paths not used. Incoming filename must include a full path.
	bool success = true;
	char* BVH_fullfilename = NULL;
	try
	{
		BVH_fullfilename = data_manager.findFile(BVH_filename.c_str());
		if (BVH_fullfilename == NULL)
		{
			logout << "AnimationControl::loadCharacters: Unable to find character BVH file <" << BVH_filename << ">. Aborting load." << endl;
			throw BasicException("ABORT");
		}
		pair<Skeleton*, MotionSequence*> read_result;
		try
		{
			read_result = data_manager.readBVH(BVH_fullfilename);
		}
		catch (const DataManagementException& dme)
		{
			logout << "AnimationControl::loadCharacters: Unable to load character data files. Aborting load." << endl;
			logout << "   Failure due to " << dme.msg << endl;
			throw BasicException("ABORT");
		}

		Skeleton* skel = read_result.first;
		MotionSequence* ms = read_result.second;
		frame_duration = (1.0f/ms->getFrameRate());
		num_frames = ms->numFrames();

		// scale the bones lengths and translation distances
		skel->scaleBoneLengths(BVH_character_size_scale);
		ms->scaleChannel(CHANNEL_ID(0,CT_TX), BVH_character_size_scale);
		ms->scaleChannel(CHANNEL_ID(0,CT_TY), BVH_character_size_scale);
		ms->scaleChannel(CHANNEL_ID(0,CT_TZ), BVH_character_size_scale);

		MotionSequenceController* controller = new MotionSequenceController(ms);

		// attach motion controller to animated skeleton
		skel->attachMotionController(controller);

		// create a character to link all the pieces together.
		string d1 = string("skeleton: ") + BVH_filename;
		string d2 = string("motion: ") + BVH_filename;
		skel->setDescription1(d1.c_str());
		skel->setDescription2(d2.c_str());
		character = skel;
	}
	catch (BasicException&) { success = false; }

	strDelete(BVH_fullfilename); BVH_fullfilename = NULL;

	ready = success;
	return ready;
}

bool AnimationControl::loadAMCCharacter(string& ASF_filename, string& AMC_filename)
{
	reset();
	// Search paths not used. Incoming filename must include a full path.
	bool success = true;
	char* ASF_fullfilename = NULL;
	char* AMC_fullfilename = NULL;
	try
	{
		ASF_fullfilename = data_manager.findFile(ASF_filename.c_str());
		if (ASF_fullfilename == NULL)
		{
			logout << "AnimationControl::loadCharacters: Unable to find character ASF file <" << ASF_filename << ">. Aborting load." << endl;
			throw BasicException("ABORT");
		}
		AMC_fullfilename = data_manager.findFile(AMC_filename.c_str());
		if (AMC_fullfilename == NULL)
		{
			logout << "AnimationControl::loadCharacters: Unable to find character AMC file <" << AMC_filename << ">. Aborting load." << endl;
			throw BasicException("ABORT");
		}

		pair<Skeleton*, MotionSequence*> read_result;
		try
		{
			read_result = data_manager.readASFAMC(ASF_fullfilename, AMC_fullfilename);
		}
		catch (const DataManagementException& dme)
		{
			logout << "AnimationControl::loadCharacters: Unable to load character data files. Aborting load." << endl;
			logout << "   Failure due to " << dme.msg << endl;
			throw BasicException("ABORT");
		}

		Skeleton* skel = read_result.first;
		MotionSequence* ms = read_result.second;
		frame_duration = (1.0f / ms->getFrameRate());
		num_frames = ms->numFrames();

		// scale the bones lengths and translation distances
		skel->scaleBoneLengths(AMC_character_size_scale);
		ms->scaleChannel(CHANNEL_ID(0, CT_TX), AMC_character_size_scale);
		ms->scaleChannel(CHANNEL_ID(0, CT_TY), AMC_character_size_scale);
		ms->scaleChannel(CHANNEL_ID(0, CT_TZ), AMC_character_size_scale);

		MotionSequenceController* controller = new MotionSequenceController(ms);

		// attach motion controller to animated skeleton
		skel->attachMotionController(controller);

		// create a character to link all the pieces together.
		string d1 = string("skeleton: ") + ASF_filename;
		string d2 = string("motion: ") + AMC_filename;
		skel->setDescription1(d1.c_str());
		skel->setDescription2(d2.c_str());
		character = skel;
	}
	catch (BasicException&) { success = false; }

	strDelete(ASF_fullfilename); ASF_fullfilename = NULL;
	strDelete(AMC_fullfilename); AMC_fullfilename = NULL;

	ready = success;
	return ready;
}


bool AnimationControl::getRenderList(list<Object*>& render_list)
{
	if (!ready || (character == NULL)) return false;

	Color color(0.0f, 0.4f, 1.0f);
	character->constructRenderObject(render_list, color);
	return true;
}