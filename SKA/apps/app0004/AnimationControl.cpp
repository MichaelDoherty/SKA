//-----------------------------------------------------------------------------
// app0004 - Builds with SKA Version 4.0
//-----------------------------------------------------------------------------
// AnimationControl.cpp
//    animation controller using a program controlled motion controller
//-----------------------------------------------------------------------------
#include "Core/SystemConfiguration.h"
#include <cstdio>
#include <complex>
#include "DataManagement/DataManager.h"
#include "DataManagement/DataManagementException.h"
#include "Core/Utilities.h"
#include "AnimationControl.h"
#include "Animation/MotionSequenceController.h"
#include "Animation/MultiSequenceController.h"
#include "Animation/AnimationException.h"
#include "ExperimentalController.h"
#include "AppConfig.h"

static string character1_ASF("501/501.asf"); 

bool AnimationControl::updateAnimation(float _elapsed_time)
{
	unsigned int c;
	float frame_time = _elapsed_time;
	if (!ready) return false;

	if (single_step) 
	{
		freeze = true;
	}
	else if (freeze) 
	{
		frame_time = 0.0f;
	}
	if (!freeze || single_step)
	{
		world_time += frame_time;
		for (c=0; c<characters.size(); c++)
			if (characters[c]!= NULL) characters[c]->update((float)world_time);
	}
	single_step = false;
	return true;
}

void AnimationControl::loadCharacters(list<Object*>& render_list)
{
	Skeleton* skel = NULL;
	char* ASF_filename = NULL;
	Color bone_color;
	string descr1;

	data_manager.addFileSearchPath(AMC_MOTION_FILE_PATH);

	try
	{
		ASF_filename = data_manager.findFile(character1_ASF.c_str());
		if (ASF_filename == NULL)
		{
			logout << "AnimationControl::loadCharacters: Unable to find character ASF file <" << character1_ASF << ">. Aborting load." << endl;
			throw BasicException("ABORT");
		}
		try {
			skel = data_manager.readASF(ASF_filename);
		}
		catch (const DataManagementException& dme)
		{
			logout << "AnimationControl::loadCharacters: Unable to load character data files. Aborting load." << endl;
			logout << "   Failure due to " << dme.msg << endl;
			if (ASF_filename != NULL) strDelete(ASF_filename);
			throw BasicException("ABORT");
		}
		descr1 = string("skeleton: ") + character1_ASF;
		skel->setDescription1(descr1.c_str());
		skel->setDescription2("Programmed Motion");
		// create rendering model for the character
		bone_color = Color(1.0f,0.4f,0.3f);
		skel->constructRenderObject(render_list, bone_color);
		// create a motion controller and attach it to the skeleton
		ExperimentalController *controller = new ExperimentalController();
		controller->setSkelton(skel);
		skel->attachMotionController(controller);
		characters.push_back(skel);
		ready = true;
	}
	catch (BasicException&) {} 
	strDelete(ASF_filename);
}

AnimationControl anim_ctrl;

AnimationControl::AnimationControl()
{
	ready = false;
	single_step = false;
	freeze = false;
	mocap_frame_duration = 0.1f;
	time_since_last_frame = 0.0f;
	master_frame = 0;
	world_time = 0.0f;
}

AnimationControl::~AnimationControl()
{
	for (unsigned int c=0; c<characters.size(); c++)
		if (characters[c] != NULL) delete characters[c];
	characters.clear();
}


