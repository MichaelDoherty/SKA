//-----------------------------------------------------------------------------
// app0003 - Builds with SKA Version 3.1 - Sept 01, 2012 - Michael Doherty
//-----------------------------------------------------------------------------
// AnimationControl.cpp
//    animation controller for multiple characters.
//-----------------------------------------------------------------------------
// SKA configuration
#include <Core/SystemConfiguration.h>
// C/C++ libraries
#include <cstdio>
#include <complex>
// SKA modules
#include <Core/Utilities.h>
#include <Animation/MotionSequenceController.h>
#include <Animation/RawMotionController.h>
#include <Animation/AnimationException.h>
#include <Animation/Skeleton.h>
#include <DataManagement/DataManager.h>
#include <DataManagement/DataManagementException.h>
#include <DataManagement/BVH_Reader.h>
// local application
#include "AppConfig.h"
#include "AnimationControl.h"

// global single instance of the animation controller
AnimationControl anim_ctrl;

// This example will create and animate two characters

// ASF and AMC files for the first (orange) character
static string character1_ASF("02/02.asf"); 
static string character1_AMC("02/02_01.amc");  

// BVH file for the 2nd (blue) character
static string character2_BVH("UOP_lab_01.bvh");
// scale the character to same size as character 1
float character2_size_scale = 0.2f;

AnimationControl::AnimationControl() 
	: ready(false), run_time(0.0f), 
	single_step(false), freeze(false), time_warp(1.0f)
{ } 

AnimationControl::~AnimationControl()	
{		
	for (unsigned short c=0; c<characters.size(); c++)
		if (characters[c] != NULL) delete characters[c]; 
}

bool AnimationControl::updateAnimation(float _elapsed_time)
{
	if (!ready) return false;

	// time warp simply scales the elapsed time
	_elapsed_time *= time_warp;

	if (!freeze || single_step) // 
	{
		run_time += _elapsed_time;
		for (unsigned short c=0; c<characters.size(); c++)
			if (characters[c] != NULL) characters[c]->update(run_time);
	}
	if (single_step) // single step then freeze AFTER this frame
	{
		freeze = true;
		single_step = false;
	}
	return true;
}

static Skeleton* buildCharacter(
	Skeleton* _skel, 
	MotionSequence* _ms, 
	Color _bone_color, 
	const string& _description1, 
	const string& _description2,
	list<Object*>& _render_list)
{
	if ((_skel == NULL) || (_ms == NULL)) return NULL;

	MotionSequenceController* controller = new MotionSequenceController(_ms);
	_skel->constructRenderObject(_render_list, _bone_color);
	_skel->attachMotionController(controller);
	_skel->setDescription1(_description1.c_str());
	_skel->setDescription2(_description2.c_str());
	return _skel;
}

void AnimationControl::loadCharacters(list<Object*>& render_list)
{
	data_manager.addFileSearchPath(AMC_MOTION_FILE_PATH);
	data_manager.addFileSearchPath(BVH_MOTION_FILE_PATH);

	Skeleton* skel = NULL;
	MotionSequence* ms = NULL;
	string descr1, descr2;
	Color bone_color;
	char* filename1 = NULL;
	char* filename2 = NULL;
	Skeleton* character = NULL;
	pair<Skeleton*, MotionSequence*> read_result;

// ----------- create 1st character ---------------------------
	try
	{
		filename1 = data_manager.findFile(character1_ASF.c_str());
		if (filename1 == NULL)
		{
			logout << "AnimationControl::loadCharacters: Unable to find character ASF file <" << character1_ASF << ">. Aborting load." << endl;
			throw BasicException("ABORT 1A");
		}
	
		filename2 = data_manager.findFile(character1_AMC.c_str());
		if (filename2 == NULL)
		{
			logout << "AnimationControl::loadCharacters: Unable to find character AMC file <" << character1_AMC << ">. Aborting load." << endl;
			throw BasicException("ABORT 1B");
		}
		
		try {
			read_result = data_manager.readASFAMC(filename1, filename2);
		}
		catch (const DataManagementException& dme)
		{
			logout << "AnimationControl::loadCharacters: Unable to load character data files. Aborting load." << endl;
			logout << "   Failure due to " << dme.msg << endl;
			throw BasicException("ABORT 1C");
		}
		
		skel = read_result.first;
		ms = read_result.second;
		
		// create a character to link all the pieces together.
		descr1 = string("skeleton: ") + character1_ASF;
		descr2 = string("motion: ") + character1_AMC;
		bone_color = Color(1.0f,0.4f,0.3f);
		character = buildCharacter(skel, ms, bone_color, descr1, descr2, render_list);
		if (character != NULL) characters.push_back(character);
	}
	catch (BasicException&) { }
	strDelete(filename1); filename1 = NULL;
	strDelete(filename2); filename2 = NULL;

// ----------- create 2nd character ---------------------------
	try
	{
		filename1 = data_manager.findFile(character2_BVH.c_str());
		if (filename1 == NULL)
		{
			logout << "AnimationControl::loadCharacters: Unable to find character BVH file <" << character2_BVH << ">. Aborting load." << endl;
			throw BasicException("ABORT 2A");
		}
		try
		{
			read_result = data_manager.readBVH(filename1);
		}
		catch (const DataManagementException& dme)
		{
			logout << "AnimationControl::loadCharacters: Unable to load character data files. Aborting load." << endl;
			logout << "   Failure due to " << dme.msg << endl;
			throw BasicException("ABORT 2C");
		}

		skel = read_result.first;
		ms = read_result.second;
	
		skel->scaleBoneLengths(character2_size_scale);
		ms->scaleChannel(CHANNEL_ID(0,CT_TX), character2_size_scale);
		ms->scaleChannel(CHANNEL_ID(0,CT_TY), character2_size_scale);
		ms->scaleChannel(CHANNEL_ID(0,CT_TZ), character2_size_scale);
		
		// create a character to link all the pieces together.
		descr1 = string("skeleton: ") + character2_BVH;
		descr2 = string("motion: ") + character2_BVH;
		bone_color = Color(0.0f,0.4f,1.0f);
		character = buildCharacter(skel, ms, bone_color, descr1, descr2, render_list);
		if (character != NULL) characters.push_back(character);
	}
	catch (BasicException&) { }

	strDelete(filename1); filename1 = NULL;
	strDelete(filename2); filename2 = NULL;

	if (characters.size() > 0) ready = true;
}


