//-----------------------------------------------------------------------------
// app0001 - Builds with SKA Version 4.0
//-----------------------------------------------------------------------------
// AnimationControl.cpp
//    Animation controller for a single character
//    defined by an ASF/AMC file pair.
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
// The character's skeleton will be defined by an ASF file.
// The character's motion will be defined by an AMC file.

static string character_ASF("02/02.asf"); 
static string character_AMC("02/02_01.amc");  

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
	data_manager.addFileSearchPath(AMC_MOTION_FILE_PATH);
	char* ASF_filename = NULL;
	char* AMC_filename = NULL;

	try
	{
		ASF_filename = data_manager.findFile(character_ASF.c_str());
		if (ASF_filename == NULL)
		{
			logout << "AnimationControl::loadCharacters: Unable to find character ASF file <" << character_ASF << ">. Aborting load." << endl;
			throw BasicException("ABORT");
		}
	
		AMC_filename = data_manager.findFile(character_AMC.c_str());
		if (AMC_filename == NULL)
		{
			logout << "AnimationControl::loadCharacters: Unable to find character AMC file <" << character_AMC << ">. Aborting load." << endl;
			throw BasicException("ABORT");
		}
		
		pair<Skeleton*, MotionSequence*> read_result;
		try {
			read_result = data_manager.readASFAMC(ASF_filename, AMC_filename);
		}
		catch (const DataManagementException& dme)
		{
			logout << "AnimationControl::loadCharacters: Unable to load character data files. Aborting load." << endl;
			logout << "   Failure due to " << dme.msg << endl;
			throw BasicException("ABORT");
		}
		
		Skeleton* skel = read_result.first;
		MotionSequence* ms = read_result.second;
		MotionSequenceController* controller = new MotionSequenceController(ms);
		
		// create rendering model for the character and put the character's 
		// bone objects in the rendering list
		Color color(1.0f,0.4f,0.3f);
		skel->constructRenderObject(render_list, color);

		// attach motion controller to animated skeleton
		skel->attachMotionController(controller);
		
		// create a character to link all the pieces together.
		string d1 = string("skeleton: ") + character_ASF;
		string d2 = string("motion: ") + character_AMC;
		skel->setDescription1(d1.c_str());
		skel->setDescription2(d2.c_str());
		character = skel;
	} 
	catch (BasicException&) { }

	strDelete(ASF_filename); ASF_filename = NULL;
	strDelete(AMC_filename); AMC_filename = NULL;

	ready = true;
}
