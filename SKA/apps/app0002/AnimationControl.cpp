//-----------------------------------------------------------------------------
// app0002 - Builds with SKA Version 3.1 - Sept 01, 2012 - Michael Doherty
//-----------------------------------------------------------------------------
// AnimationControl.cpp
//    Animation controller for a single character defined by a BVH file.
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

// This example will create and animate one character.
// The character's skeleton and motion will be defined by a BVH file.

//static string character_BVH("UOP_lab_01.bvh");
//static string character_BVH("mocapdata.com/greeting-02-bye-azumi.bvh");
static string character_BVH("mocapdata.com/greeting-05-salute (chest)-takiguchi.bvh");
//static string character_BVH("mocapdata.com/greeting-08-shakehands-azumi.bvh");
//static string character_BVH("mocapdata.com/throw_toss-11-disc-yokoyama.bvh");
//static string character_BVH("mocapdata.com/throw_toss-16-pass to left heavy-azumi.bvh");
//static string character_BVH("mocapdata.com/throw_toss-17-pass to back light-azumi.bvh");
//static string character_BVH("mocapdata.com/walk-09-handbag-takiguchi.bvh");
//static string character_BVH("mocapdata.com/walk-13-baggage on the shoulder-azumi.bvh"); // problem
//static string character_BVH("mocapdata.com/walk-18-shuffle forward-yokoyama.bvh");

// scale the character to about 20%
// Most things in SKA were developed to fit the CMU ASF skeletons.
// The BVH files from Pacific's mocap lab are about 5 times to large 
// for this world. In particular, bones will be too thin if stretched
// to 500% of expected size, since the bone width does not currently scale.
float character_size_scale = 0.2f;

AnimationControl::AnimationControl() 
	: ready(false), run_time(0.0f), character(NULL), 
	single_step(false), freeze(false), time_warp(1.0f)
{ } 

AnimationControl::~AnimationControl()	
{ 
	if (character != NULL) delete character; 
}

bool AnimationControl::updateAnimation(float _elapsed_time)
{
	if (!ready) return false;

	// time warp simply scales the elapsed time
	_elapsed_time *= time_warp;

	if (!freeze || single_step) // 
	{
		run_time += _elapsed_time;
		if (character != NULL) character->update(run_time);
	}
	if (single_step) // single step then freeze AFTER this frame
	{
		freeze = true;
		single_step = false;
	}
	return true;
}

void AnimationControl::loadCharacters(list<Object*>& render_list)
{
	data_manager.addFileSearchPath(BVH_MOTION_FILE_PATH);
	char* BVH_filename = NULL;
	try
	{
		BVH_filename = data_manager.findFile(character_BVH.c_str());
		if (BVH_filename == NULL)
		{
			logout << "AnimationControl::loadCharacters: Unable to find character BVH file <" << character_BVH << ">. Aborting load." << endl;
			throw BasicException("ABORT");
		}
		pair<Skeleton*, MotionSequence*> read_result;
		try
		{
			read_result = data_manager.readBVH(BVH_filename);
		}
		catch (const DataManagementException& dme)
		{
			logout << "AnimationControl::loadCharacters: Unable to load character data files. Aborting load." << endl;
			logout << "   Failure due to " << dme.msg << endl;
			throw BasicException("ABORT");
		}

		Skeleton* skel = read_result.first;
		MotionSequence* ms = read_result.second;

		// scale the bones lengths and translation distances
		skel->scaleBoneLengths(character_size_scale);
		ms->scaleChannel(CHANNEL_ID(0,CT_TX), character_size_scale);
		ms->scaleChannel(CHANNEL_ID(0,CT_TY), character_size_scale);
		ms->scaleChannel(CHANNEL_ID(0,CT_TZ), character_size_scale);

		MotionSequenceController* controller = new MotionSequenceController(ms);
		
		// create rendering model for the character and put the character's 
		// bone objects in the rendering list
		Color color(0.0f,0.4f,1.0f);
		skel->constructRenderObject(render_list, color);

		// attach motion controller to animated skeleton
		skel->attachMotionController(controller);
		
		// create a character to link all the pieces together.
		string d1 = string("skeleton: ") + character_BVH;
		string d2 = string("motion: ") + character_BVH;
		skel->setDescription1(d1.c_str());
		skel->setDescription2(d2.c_str());
		character = skel;
	} 
	catch (BasicException&) { }

	strDelete(BVH_filename); BVH_filename = NULL;

	ready = true;
}
