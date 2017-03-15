//-----------------------------------------------------------------------------
// app1001 - Builds with SKA Version 4.0
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
#include <Math/Quaternion.h>
// Application
#include "AppConfig.h"
#include "AnimationControl.h"
#include "MotionGraph.h"
#include "MotionGraphController.h"
#include <vector>
using namespace std;

// global single instance of the animation controller
AnimationControl anim_ctrl;

// scale the character to about 20%
// Most things in SKA were developed to fit the CMU ASF skeletons.
// The BVH files from Pacific's mocap lab are about 5 times to large 
// for this world. In particular, bones will be too thin if stretched
// to 500% of expected size, since the bone width does not currently scale.
float character_size_scale = 0.2f;

AnimationControl::AnimationControl() 
	: ready(false), run_time(0.0f), character(NULL), 
	single_step(false), freeze(false), time_warp(1.0f),
	motion_graph_controller(NULL), motion_graph(NULL)
{ 
	initializeMotionFileList();
} 

AnimationControl::~AnimationControl()	
{ 
	if (character != NULL) delete character; 
	if (motion_graph != NULL) delete motion_graph;
	if (motion_graph_controller != NULL) delete motion_graph_controller;
}

void AnimationControl::initializeMotionFileList()
{
	motion_data_specs.addSpec(string("swing1"), string("swing1.bvh"), string("swing1quat.bvh"));
	motion_data_specs.addSpec(string("swing2"), string("swing2.bvh"), string("swing2quat.bvh"));
	motion_data_specs.addSpec(string("swing3"), string("swing3.bvh"), string("swing3quat.bvh"));
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
	data_manager.addFileSearchPath(".");
	data_manager.addFileSearchPath(BVH_MOTION_FILE_PATH);

	logout << "MotionGraphController initialization starting" << endl;

	motion_graph = new MotionGraph(motion_data_specs);

	motion_graph_controller = new MotionGraphController(motion_graph, motion_data_specs, character_size_scale);

	logout << "MotionGraphController initialization complete" << endl;

	// Read the skeleton from the first BVH file.
	// Throw away the motion, since MotionGraphController reads all the motions.
	// FUTUREWORK (150618) - For consistency with other app, should all motion file reads be here,
	//                       rather than in the controller?

	string character_BVH = motion_data_specs.getBvhFilename(0);
	cout << "AnimationControl::loadCharacters is opening: " << character_BVH << endl;
	char* BVH_filename = NULL;
	try
	{
		BVH_filename = data_manager.findFile(character_BVH.c_str());
		if (BVH_filename == NULL)
		{
			logout << "AnimationControl::loadCharacters: Unable to find character BVH file <" << character_BVH << ">. Aborting load." << endl;
			throw AppException("ABORT");
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
			throw AppException("ABORT");
		}

		Skeleton* skel = read_result.first;
		skel->scaleBoneLengths(character_size_scale);

		// throw away the motion sequence, only using the skeleton.
		delete read_result.second;
		
		// create rendering model for the character and put the character's 
		// bone objects in the rendering list
		Color color(0.0f,0.4f,1.0f);
		skel->constructRenderObject(render_list, color);

		// attach motion controller to animated skeleton
		skel->attachMotionController(motion_graph_controller);

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

vector<string> AnimationControl::statusReport()
{
	stringstream ss[10];
	vector<string> report;

	ss[0] << "time warp: " << anim_ctrl.getTimeWarp();
	if (anim_ctrl.isFrozen()) ss[0] << " (frozen)";
	report.push_back(ss[0].str());

	if (motion_graph_controller == NULL)
	{
		report.push_back(string("No active motion graph controller"));
	}
	else
	{
		MotionGraphController::State mgcstate = motion_graph_controller->getState();
		
		ss[2] << "current time: " << mgcstate.current_time;
		report.push_back(ss[2].str());

		ss[3] << "frame zero time: " << mgcstate.frame_zero_time;
		report.push_back(ss[3].str());

		ss[4] << "active sequence: " << mgcstate.active_seqID;
		report.push_back(ss[4].str());

		ss[5] << "active frame: " << mgcstate.active_frame;
		report.push_back(ss[5].str());

		ss[6] << "transition trigger frame: " << mgcstate.transition_trigger_frame;
		report.push_back(ss[6].str());

		ss[7] << "transition sequence: " << mgcstate.transition_seqID;
		report.push_back(ss[7].str());
	
		ss[8] << "transition frame: " << mgcstate.transition_frame;
		report.push_back(ss[8].str());
	}

	return report;
}
