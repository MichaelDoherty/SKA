//-----------------------------------------------------------------------------
// MotionDescriptors project - Builds with SKA Version 4.0
//-----------------------------------------------------------------------------
// JointData.h
//			A class that holds a variety of information on each joint per frame for the running animation
//			JointStruct -> holds Joint frame
//			Vars -> vel, accel, more to be added
// Author: Trevor Martin
// Modified: June 6, 2017, Mike Doherty - Converted from bone related data to joint related data.
//           While adding support for ASF/AMC files, generalization between multiple skeleton
//           structures made it apparent that the essential data was really about joints, not bones.
//-----------------------------------------------------------------------------

#ifndef JOINTDATA_DOT_H
#define JOINTDATA_DOT_H
#include <Core/SystemConfiguration.h>
#include <Math/Plane.h>
#include <Math/Quaternion.h>
#include <Animation/Skeleton.h>
#include <Objects/Object.h>
#include <Objects/BoneObject.h>

struct JointData {

	string joint_name; 
	int frame;

	Vector3D position;

	float velocity_mag, acceleration_mag, jerk_mag;
	Vector3D velocity_vec, acceleration_vec, jerk_vec;

	Vector3D orientation_euler;
	Quaternion orientation_quat;
	Quaternion ang_velocity_quat;
	float ang_velocity_mag;

	float curvature;
	float curvature_radius;

	JointData(const string& _joint_name, int _frame) :
		joint_name(_joint_name), frame(_frame),
		velocity_mag(0.0f), acceleration_mag(0.0f), jerk_mag(0.0f), ang_velocity_mag(0.0f),
		curvature(0.0), curvature_radius(0.0)
	{}

	JointData() :
		joint_name(""), frame(0),
		velocity_mag(0.0f), acceleration_mag(0.0f), jerk_mag(0.0f), ang_velocity_mag(0.0f),
		curvature(0.0), curvature_radius(0.0)
	{}
};

#endif //JOINTDATA_DOT_H
