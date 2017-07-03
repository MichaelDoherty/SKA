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
#include "JointData.h"
#include "FullBodyData.h"
#include "AnimationControl.h"

class MotionAnalyzer {
public:
	// definition of joints tracked by this module
	enum JointID {
		// The C1 vertebra, known as the atlas, is the superior-most vertebra in the spinal column. 
		sacrum, mid_spine, upper_spine, atlas, skull_top,

		left_hip, left_knee, left_ankle, left_toetip,
		right_hip, right_knee, right_ankle, right_toetip,

		left_shoulder, left_elbow, left_wrist, left_fingertip,
		right_shoulder, right_elbow, right_wrist, right_fingertip
	};

	static string toString(JointID jid) {
		switch (jid) {
		case sacrum: return string("sacrum");
		case mid_spine: return string("mid_spine");
		case upper_spine: return string("upper_spine");
		case atlas: return string("atlas");
		case skull_top: return string("skull_top");
		case left_hip: return string("left_hip");
		case left_knee: return string("left_knee");
		case left_ankle: return string("left_ankle");
		case left_toetip: return string("left_toetip");
		case right_hip: return string("right_hip");
		case right_knee: return string("right_knee");
		case right_ankle: return string("right_ankle");
		case right_toetip: return string("right_toetip");
		case left_shoulder: return string("left_shoulder");
		case left_elbow: return string("left_elbow");
		case left_wrist: return string("left_wrist");
		case left_fingertip: return string("left_fingertip");
		case right_shoulder: return string("right_shoulder");
		case right_elbow: return string("right_elbow");
		case right_wrist: return string("right_wrist");
		case right_fingertip: return string("right_fingertip");
		}
		return string("");
	}
public:
	MotionAnalyzer(long _num_frames = 0, float _frame_duration = 0.0, Skeleton* _skel = NULL)
	: ourSkel(NULL)
	{ 
		num_joints = right_fingertip + 1;
		joint_positions.resize(num_joints);
		joint_orientations.resize(num_joints);
		initialize(_num_frames, _frame_duration, _skel);
	}

	// Reinitialize with a new animation.
	// This will discard any previously collected data.
	void initialize(long _num_frames=0, float _frame_duration=0.0, Skeleton* _skel = NULL);
	
	// Do analysis for the current frame.
	void analyzeCurrentFrame(long frame_id, float _frame_duration = 0.0);

	void storeResults(const string& directory, const string& tag);

private:
	// data that is controlled by this module

	//initialize 2d vector of Joint data for frame x Joint data struct
	//frame -> (Joint -> JointData) //get data for all joints for a single frame
	//will have number of frames x number of joints
	//Parallel arrays per frame(both indexed by frame)
	short num_joints;
	
	// These are temporary locations for extracting data from the skeleton.
	vector<Vector3D> joint_positions;
	vector<Vector3D> joint_orientations; 

	// These are the permanent locations indexed by frame.
	vector<vector<JointData> > joint_data;
	vector<FullBodyData> body_data;
	vector<bool> frame_data_calculated;

	std::map<std::string, float> weightMap;//used for CoM
	std::map<std::string, float> QoMWeightMap; //used for QoM

	// data that comes from the animation module
	Skeleton* ourSkel;
	short skeleton_type;
	float frame_duration;
	long num_frames;
	long animation_frame;

	// private methods

	string trackDirection(Vector3D bone_pos, Plane _plane, string side, int frame, Vector3D & last_location);
	pair <Vector3D, float> calcBoundingSphere(vector <Vector3D> points);
	pair <Vector3D, float> calcVel(Vector3D p0, Vector3D p1);
	pair <Vector3D, float> calcAccel(Vector3D p0, Vector3D p1, Vector3D p2);
	pair <Vector3D, float> calcJerk(Vector3D p0, Vector3D p1, Vector3D p2, Vector3D p3);
	pair <Quaternion, float> calcAngularVel(Quaternion& q0, Quaternion& q1);
	float calcCurvature(Vector3D accel, Vector3D velVector, float velMag);
	float calcRadiusOfCurvature(float curve);
	float calculateVelocity(Vector3D bone_pos, Plane _plane, int frames, Vector3D last_location);
	float calculateQoM(int frame);
	Vector3D calculateCoM(int frame);

	void extractJointPositionsAndOrientations();

};

#endif // MOTIONANALYZER_DOT_H
