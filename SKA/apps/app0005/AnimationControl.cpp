//-----------------------------------------------------------------------------
// app0005 - Builds with SKA Version 3.1 - Sept 01, 2012 - Michael Doherty
//-----------------------------------------------------------------------------
// AnimationControl.cpp
//     Animation control for a three bone arm 
//     with inverse kinematics tracking a moving target.
//-----------------------------------------------------------------------------
#include "Core/SystemConfiguration.h"
#include <cstdio>
#include <complex>
#include "Animation/AnimationException.h"
#include "Animation/MultiSequenceController.h"
#include "DataManagement/DataManager.h"
#include "DataManagement/DataManagementException.h"
#include "Core/Utilities.h"
#include "AppConfig.h"
#include "AnimationControl.h"

//---------------------------------------------
// Define the ASF and AMC files to initialize the character
static string character1_ASF("502/502.asf"); 
static string character1_AMC("502/502_01.amc");  

//---------------------------------------------
// Define target movement

static const int num_targets = 16;
Vector3D target_coords[num_targets] = { 
	Vector3D(  0.0f,  15.0f,   7.0f),
	Vector3D( 20.0f,   5.0f,   5.0f),
	Vector3D(  0.0f,   5.0f,  10.0f),
	Vector3D(-20.0f,   5.0f,  -5.0f),
	Vector3D(  0.0f,  20.0f,   5.0f),
	Vector3D(  0.0f,  20.0f,   5.0f),
	Vector3D(  0.0f,  10.0f,  10.0f),
	Vector3D( 10.0f,   5.0f, -10.0f),
	Vector3D(-10.0f,  10.0f,   5.0f),
	Vector3D(  0.0f,  10.0f,  10.0f),
	Vector3D(  0.0f, 14.14f,  15.0f),
	Vector3D(  0.0f,   7.0f,  10.0f),
	Vector3D( -7.0f,   5.0f,  10.0f),
	Vector3D(  0.0f,   5.0f,  -5.0f),
	Vector3D(  0.0f,  15.0f,   5.0f),
	Vector3D( -5.0f,   5.0f,  13.0f)
};
// index of currently active target
static int current_target = 0;
// number of frames that target stays in each position
static int target_duration = 50;  
// count-down timer to next target change
static int target_timer = target_duration;
// current target position
Vector3D target_pos(0.0f, 15.0f, 7.0f);
// object drawn to visualize target position (declared in AppMain)
extern Object* target_box;

//---------------------------------------------
// Define end effector

// name of end effector bone
string end_bone_name("bone3");
// object drawn to visualize effector position (declared in AppMain)
extern Object* effector_box;


//---------------------------------------------
// Parameters controlling IK algorithm

// number of times the IK algoritm will be applied during each animation frame
// Set this to zero to disable IK.
static int CCD_CYCLES_PER_FRAME = 1;
// angular change made by IK algorithm in each step
static const float CCDIK_STEP_SIZE = 0.5f*(TWO_PI/180.0f);

// function to compute the current distance between the end effector and 
float computeIKDistance(Vector3D& target, string& end_effector, Skeleton* skel)
{
	skel->update(1);
	Vector3D start_pos, end_pos;
	skel->getBonePositions(end_effector.c_str(), start_pos, end_pos);
	return target.distance(end_pos);
}

// Inverse Kinematics through Cyclic Coordinate Descent
bool CCD_IK(Vector3D& target, string& end_effector, Skeleton* skel)
{
	Skeleton* skeldef = skel;
	
	// dangerous down cast!
	MotionSequenceController* motion_controller = (MotionSequenceController*)skel->getMotionController();
	// extract the motion sequence (from the AMC file) 
	// we'll hack into that data structure to modify the pose
	MotionSequence* ms = motion_controller->getMotionSequence();
	// pull out the channels available for the skeleton
	int num_channels = ms->numChannels();
	// iterate through the channels
	for (int c=0; c<num_channels; c++)
	{
		CHANNEL_ID cid = ms->getChannelID(c);
		BONE_ID b = cid.bone_id;
		CHANNEL_TYPE ct = cid.channel_type;
		string bone_name = skeldef->boneNameFromId(b);
		if (bone_name != string("root")) 	// skip root channels (0-5)
		{
			// get the bone definition:
			Bone* bone = skel->getBone(bone_name.c_str());
			// extract DOF restrictions and limits for this channel
			bool dof_is_valid = bone->isValidChannel(ct);
			float min_angle = bone->getChannelLowerLimit(ct);
			float max_angle = bone->getChannelUpperLimit(ct);

			// if it's legal to change this channel, 
			// attempt to move towards target by adjusting this channel
			if (dof_is_valid)
			{
				//logout << bone_name << " DOF " << dof << " min " << min_angle << " max " << max_angle << endl;

				// determine current error and channel value
				float initial_error = computeIKDistance(target, end_effector, skel);
				float curr_value = ms->getValue(cid, 0);

				// try a positive change
				float new_value = curr_value + CCDIK_STEP_SIZE;
				if ((new_value >= min_angle) && (new_value <= max_angle))
				{
					ms->setValue(cid, 0, new_value);
					float test_error = computeIKDistance(target, end_effector, skel);
					if (test_error < initial_error) continue;
				}

				// try a negative change
				new_value = curr_value - CCDIK_STEP_SIZE;
				if ((new_value >= min_angle) && (new_value <= max_angle))
				{
					ms->setValue(cid, 0, new_value);
					float test_error = computeIKDistance(target, end_effector, skel);
					if (test_error < initial_error) continue;
				}

				// no change, put back original value
				ms->setValue(cid, 0, curr_value);
			}
		}
	}
	return false;
}

bool AnimationControl::updateAnimation(float _elapsed_time)
{
	target_timer--;
	if (target_timer < 0)
	{
		target_timer = target_duration;
		current_target = (current_target+1)%num_targets;
		target_pos = target_coords[current_target];
	}
	target_box->moveTo(target_pos);
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
		for (c=0; c<characters.size(); c++)
			if (characters[c] != NULL) 
			{
				// apply the IK algorithm to the first character
				if (c==0)
				{
					for (int cycle=0; cycle<CCD_CYCLES_PER_FRAME; cycle++)
						CCD_IK(target_pos, end_bone_name, characters[c]);
					characters[c]->update(frame_time);
					Vector3D start_pos, end_pos;
					characters[c]->getBonePositions(end_bone_name.c_str(), start_pos, end_pos);
					effector_box->moveTo(end_pos);
				}
				else
				{
					// other characters just run normal mocap
					characters[c]->update(frame_time);
				}
			}
	}
	single_step = false;
	return true;
}

void AnimationControl::loadCharacters(list<Object*>& render_list)
{
	data_manager.addFileSearchPath(AMC_MOTION_FILE_PATH);

	Skeleton* skel = NULL;
	MotionSequence* ms = NULL;
	string descr1, descr2;
	Color bone_color;
	char* filename1 = NULL;
	char* filename2 = NULL;
	pair<Skeleton*, MotionSequence*> read_result;

	try
	{
		filename1 = data_manager.findFile(character1_ASF.c_str());
		if (filename1 == NULL)
		{
			logout << "AnimationControl::loadCharacters: Unable to find character ASF file <" << character1_ASF << ">. Aborting load." << endl;
			throw BasicException("ABORT");
		}
		filename2 = data_manager.findFile(character1_AMC.c_str());
		if (filename2 == NULL)
		{
			logout << "AnimationControl::loadCharacters: Unable to find character AMC file <" << character1_AMC << ">. Aborting load." << endl;
			throw BasicException("ABORT");
		}
		try {
			read_result = data_manager.readASFAMC(filename1, filename2);
		}
		catch (const DataManagementException& dme)
		{
			logout << "AnimationControl::loadCharacters: Unable to load character data files. Aborting load." << endl;
			logout << "   Failure due to " << dme.msg << endl;
			throw BasicException("ABORT");
		}
		skel = read_result.first;
		ms = read_result.second;
		if ((skel == NULL) || (ms==NULL)) throw BasicException("ABORT");
		MotionSequenceController* controller = new MotionSequenceController(ms);
		descr1 = string("skeleton: ") + character1_ASF;
		descr2 = string("motion: ") + character1_AMC;
		skel->setDescription1(descr1.c_str());
		skel->setDescription2(descr2.c_str());
		// create rendering model for the character
		Color color(1.0f,0.4f,0.3f);
		skel->constructRenderObject(render_list, color);
		// attach motion controller to character
		skel->attachMotionController(controller);
		characters.push_back(skel);
		ready = true;
	}
	catch (BasicException&) { }
	strDelete(filename1);
	strDelete(filename2);
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
}

AnimationControl::~AnimationControl()
{
	for (unsigned int c=0; c<characters.size(); c++)
		if (characters[c] != NULL) delete characters[c];
	characters.clear();
}

