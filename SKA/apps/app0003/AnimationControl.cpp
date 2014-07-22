//-----------------------------------------------------------------------------
// app0003: Adds a second character, defined and controlled by a BVH file.
//-----------------------------------------------------------------------------
// AnimationControl.cpp
//    Object that is the interface to the animation subsystem.
//-----------------------------------------------------------------------------
// Builds with SKA Version 3.0 - July 22, 2012 - Michael Doherty
//-----------------------------------------------------------------------------
// BVH reader and playback is still in development. 
// There are still problems, including:
//   1) incorrect interpretation of angles at joints with multple DOF
//   2) lack of integration with data manager module
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
#include <Animation/AnimSkeleton.h>
#include <DataManagement/DataManager.h>
#include <DataManagement/DataManagementException.h>
#include <DataManagement/BVH_Reader.h>
// local application
#include "AppConfig.h"
#include "AnimationControl.h"

// global single instance of the animation controller
AnimationControl anim_ctrl;

// This example will create and animate two characters

// ID of the ASF and AMC files for the first (orange) character
static string skeleton_id("01"); 
static string motion_id("01");

// BVH file for the 2nd (blue) character
static string bvh_filename("UOP_lab_01.bvh");
// scale the character to same size as character 1
float BVH_scale_size = 0.2f;

// alternate BVH file for the 2nd (blue) character
// This should be the exact same motion as ASF/AMC 01_01
//static string bvh_filename("01_01.bvh"); 
//float BVH_scale_size = 1.0f;

bool AnimationControl::updateAnimation(float _elapsed_time)
{
	if (!ready) return false;

	// time warp simply scales the elapsed time
	_elapsed_time *= time_warp;

	if (!freeze || single_step) // 
	{
		run_time += _elapsed_time;
		for (unsigned short c=0; c<characters.size(); c++)
			if (characters[c]->skeleton != NULL) 
				characters[c]->skeleton->update(run_time);
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
	char s[2000];

// ----------- create 1st character ---------------------------

	data_manager.setFilePathRoot(string(AMC_MOTION_FILE_PATH));

	// read the ASF file to create a skeleton definition instance
	SkeletonDefinition* skel = data_manager.loadSkeleton(skeleton_id);
	if (skel == NULL) 
	{
		sprintf(s, "Could not load ASF %s", skeleton_id.c_str());
		logout << s << endl;
		throw AnimationException();
	}

	// read the AMC file and store it as a motion sequence in a motion controller
	MotionSequence* ms = data_manager.loadMotion(skeleton_id, motion_id, skel);
	if (ms == NULL) 
	{ 
		sprintf(s, "Could not load AMC %s.%s", skeleton_id.c_str(), motion_id.c_str());
		logout << s << endl;
		throw AnimationException();			
	}
	MotionSequenceController* controller = new MotionSequenceController(ms);

	// create 1st character to link all the pieces together.
	characters.push_back(new Character);
	characters[0]->description1 = string("ASF") + skeleton_id;
	characters[0]->description2 = string("AMC") + motion_id;;
	data_manager.getDescriptions(skeleton_id, motion_id,
		characters[0]->description1, characters[0]->description2);

	// create an animated skeleton for this character.
	characters[0]->skeleton_definition = skel;
	characters[0]->skeleton = new AnimSkeleton(characters[0]->skeleton_definition);
	if (characters[0]->skeleton == NULL) 
	{
		sprintf(s, "Failed to build AnimSkeleton for %s", skeleton_id.c_str());
		logout << s << endl;
		throw AnimationException();
	}

	// create rendering model for the character and put the character's 
	// bone objects in the rendering list
	Color color(1.0f,0.4f,0.3f);
	characters[0]->skeleton->constructRenderObject(render_list, color);

	// attach motion controller to character
	characters[0]->skeleton->attachMotionController(controller);

// ----------- create 2nd character ---------------------------

	char bvh_fullfilename[1000];
	sprintf(bvh_fullfilename, "%s/%s", BVH_MOTION_FILE_PATH, bvh_filename.c_str());
	
	BVH_Reader bvh_reader;

	bvh_reader.setSizeScale(BVH_scale_size);
	pair<SkeletonDefinition*, MotionSequence*> bvh_result = bvh_reader.readBVH(bvh_fullfilename);

	SkeletonDefinition* skel2 = bvh_result.first;
	MotionSequence* ms2 = bvh_result.second;

	characters.push_back(new Character);
	characters[1]->description1 = string("BVH DATA");
	characters[1]->description2 = string("BVH DATA");

	// attach skeleton to character
	characters[1]->skeleton_definition = skel2;
	characters[1]->skeleton = new AnimSkeleton(characters[1]->skeleton_definition);
	if (characters[1]->skeleton == NULL) 
	{
		sprintf(s, "Failed to build AnimSkeleton for BVH character");
		throw AnimationException();
	}

	// create rendering model for the character
	Color color2(0.0f,0.4f,1.0f);
	characters[1]->skeleton->constructRenderObject(render_list, color2);
	
	// attach motion controller to character
	characters[1]->skeleton->attachMotionController(new MotionSequenceController(ms2));

	ready = true;
}


