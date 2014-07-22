//-----------------------------------------------------------------------------
// app0002: Demo program illustrating various useful things that aren't
//          directly related to the character animation.
//          (1) additional objects, such as ground, sky and coordinate axes
//          (2) moveable camera, controlled by the camera and mouse.
//          (3) keyboard filtering, to avoid multiple responses when
//              a single keystroke is expected.
//          (4) animation speed control (freeze, single step, time warp)
//          (5) heads-up display (2D text on screen)
//-----------------------------------------------------------------------------
// AnimationControl.cpp
//    Object that is the interface to the animation subsystem.
//-----------------------------------------------------------------------------
// Builds with SKA Version 3.0 - July 22, 2012 - Michael Doherty
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
// local application
#include "AppConfig.h"
#include "AnimationControl.h"

// global single instance of the animation controller
AnimationControl anim_ctrl;

// This example will create and animate one character.
// The character's skeleton will be defined by an ASF file.
// The character's skeleton will be defined by an AMC file.

static string skeleton_id("02"); // ID of the ASF file
static string motion_id("01");   // ID of the AMC file

bool AnimationControl::updateAnimation(float _elapsed_time)
{
	if (!ready) return false;

	// time warp simply scales the elapsed time
	_elapsed_time *= time_warp;

	if (!freeze || single_step) // 
	{
		run_time += _elapsed_time;
		if (character->skeleton != NULL) character->skeleton->update(run_time);
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

	// create a character to link all the pieces together.
	character =  new Character;
	character->description1 = string("UNDEFINED");
	character->description2 = string("UNDEFINED");
	data_manager.getDescriptions(skeleton_id, motion_id,
		character->description1, character->description2);

	// create an animated skeleton for this character.
	character->skeleton_definition = skel;
	character->skeleton = new AnimSkeleton(character->skeleton_definition);
	if (character->skeleton == NULL) 
	{
		sprintf(s, "Failed to build AnimSkeleton for %s", skeleton_id.c_str());
		logout << s << endl;
		throw AnimationException();
	}

	// create rendering model for the character and put the character's 
	// bone objects in the rendering list
	Color color(1.0f,0.4f,0.3f);
	character->skeleton->constructRenderObject(render_list, color);

	// attach motion controller to character
	character->skeleton->attachMotionController(controller);
	
	ready = true;
}


