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
//           structures made it apparent that the essential data was really about jpoints, not bones.
//-----------------------------------------------------------------------------

#ifndef JOINTDATA_DOT_H
#define JOINTDATA_DOT_H
//Includes for vars
#include <Core/SystemConfiguration.h>
#include <Math/Plane.h>
#include <Animation/Skeleton.h>
// SKA modules

#include <Objects/Object.h>
#include <Objects/BoneObject.h>

class JointData {

private:
	string joint_name; 
	int frame;

	Vector3D position;

	float velocity_mag, acceleration_mag, jerk_mag;
	Vector3D velocity_vec, acceleration_vec, jerk_vec;

	float curvature;
	float curvature_radius;

public:

	JointData(const string& _joint_name, int _frame) :
		joint_name(_joint_name), frame(_frame),
		velocity_mag(0.0f), acceleration_mag(0.0f), jerk_mag(0.0f),
		curvature(0.0), curvature_radius(0.0)
	{}

	JointData() :
		joint_name(""), frame(0),
		velocity_mag(0.0f), acceleration_mag(0.0f), jerk_mag(0.0f),
		curvature(0.0), curvature_radius(0.0)
	{}

	void setJointName(const string& _joint_name) { joint_name = _joint_name; }
	string getJointName() { return joint_name; }

	void setFrame(int frame) { frame = frame; }
	int getFrame() { return frame; }

	void setPosition(Vector3D start) { position = start; }
	Vector3D getPosition() { return position; }

	void setVelocity_mag(float vel) { velocity_mag = vel; }
	void setVelocity_vec(Vector3D vel) { velocity_vec = vel; }
	float getVelocity_mag() { return velocity_mag; }
	Vector3D getVelocity_vec() { return velocity_vec; }

	void setAcceleration_mag(float accel) { acceleration_mag = accel; }
	void setAcceleration_vec(Vector3D accel) { acceleration_vec = accel; }
	float getAcceleration_mag() { return acceleration_mag; }
	Vector3D getAcceleration_vec() { return acceleration_vec; }

	void setJerk_mag(float j) { jerk_mag = j; }
	void setJerk_vec(Vector3D j) { jerk_vec = j; }
	float getJerk_mag() { return jerk_mag; }
	Vector3D getJerk_vec() { return jerk_vec; }

	void setCurvature(float curve) { curvature = curve; }
	void setCurvatureRadius(float rad) { curvature_radius = rad; }
	float getCurvature() { return curvature; }
	float getCurvatureRadius() { return curvature_radius; }
};

#endif //JOINTDATA_DOT_H
