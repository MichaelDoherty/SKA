//-----------------------------------------------------------------------------
// MotionDescriptors project - Builds with SKA Version 4.0
//-----------------------------------------------------------------------------
// MotionAnalyzer.h
// Author:     Trevor Martin - April 2017
// Refactored: Michael Doherty - May 2017
//-----------------------------------------------------------------------------

#ifndef MOTIONANALYZER_DOT_H
#define MOTIONANALYZER_DOT_H
// SKA modules
#include <Core/SystemConfiguration.h>
//#include <Math/Plane.h>
#include <Animation/Skeleton.h>
// local application
#include "BoneData.h"
#include "FullBodyData.h"
#include "AnimationControl.h"

class MotionAnalyzer {
public:
	MotionAnalyzer(long _num_frames = 0, float _frame_duration = 0.0, Skeleton* _skel = NULL)
	: ourSkel(NULL)
	{ 
		initialize(_num_frames, _frame_duration, _skel);
	}

	// Reinitialize with a new animation.
	// This will discard any previously collected data.
	void initialize(long _num_frames=0, float _frame_duration=0.0, Skeleton* _skel = NULL);
	
	// Do analysis for the current frame.
	void analyzeCurrentFrame(long frame_id, float _frame_duration = 0.0);

private:
	// data that is controlled by this module

	//initialize 2d vector of Bone data for frame x Bone data struct
	//frame -> (Bone -> BoneData) //get data for all bones for a single frame
	//will have number of frames x number of bones
	//Parallel arrays per frame(both indexed by frame)
	vector<vector<BoneData>> bone_data;
	vector<FullBodyData> body_data;
	vector<bool> frame_data_valid;
	std::map<std::string, float> weightMap;//used for CoM
	std::map<std::string, float> QoMWeightMap; //used for QoM

	// data that comes from the animation module
	Skeleton* ourSkel;
	float frame_duration;
	long num_frames;
	long animation_frame;

	// private methods

	string trackDirection(Vector3D bone_pos, Plane _plane, string side, int frame, Vector3D & last_location);
	pair <Vector3D, float> calcBoundingSphere(vector <Vector3D> points);
	pair <Vector3D, float> calcVel(Vector3D p0, Vector3D p1);
	pair <Vector3D, float> calcAccel(Vector3D p0, Vector3D p1, Vector3D p2);
	pair <Vector3D, float> calcJerk(Vector3D p0, Vector3D p1, Vector3D p2, Vector3D p3);
	float calcCurvature(Vector3D accel, Vector3D velVector, float velMag);
	float calcRadiusOfCurvature(float curve);
	float calculateVelocity(Vector3D bone_pos, Plane _plane, int frames, Vector3D last_location);
	float calculateQoM(int frame);
	Vector3D calculateCoM(int frame);
};

#endif // MOTIONANALYZER_DOT_H