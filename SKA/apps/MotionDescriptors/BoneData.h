//-----------------------------------------------------------------------------
// MotionDescriptors project - Builds with SKA Version 4.0
//-----------------------------------------------------------------------------
// BoneData.h
//			A class that holds a variety of information on each bone per frame for the running animation
//			BoneStruct -> holds Bone frame
//			Vars -> vel, accel, more to be added
// Author: Trevor Martin
//
//
//-----------------------------------------------------------------------------

#ifndef BONEDATA_H
#define BONEDATA_H
//Includes for vars
#include <Core/SystemConfiguration.h>
#include "Plane.h"
#include "Animation/Skeleton.h"
// SKA modules

#include <Objects/Object.h>
#include <Objects/BoneObject.h>
struct BoneStruct {
	BoneStruct() : _bone(0),  _frame(0) {}
	Bone* _bone;
	int _frame;
	friend bool operator==(const BoneStruct& lhs, const BoneStruct& rhs)
	{
		if (lhs._bone->getID() == rhs._bone->getID()  && lhs._frame == rhs._frame) {
			return true;
		}
		else {
			return false;
		}
	}
};


class BoneData {
	//Bone vars
private:
	BoneStruct _BoneStruct;
	float velocity_mag = 0.0;
	float acceleration_mag = 0.0;
	float jerk_mag = 0.0;
	Vector3D start_position, end_position;
	Vector3D velocity_vec, acceleration_vec, jerk_vec;
	float x, y, z = 0.0;
	float Curvature = 0.0;
	float CurvatureRadius = 0.0;

public:
	//Constructor takes in info
	BoneData();
	BoneData(Bone *bone, int frame);

	//set and get functions
	void setVelocity_mag(float vel) { velocity_mag = vel; }
	void setAcceleration_mag(float accel) { acceleration_mag = accel; }
	void setJerk_mag(float j) { jerk_mag = j; }

	void setVelocity_vec(Vector3D vel) { velocity_vec = vel; }
	void setAcceleration_vec(Vector3D accel) { acceleration_vec = accel; }
	void setJerk_vec(Vector3D j) { jerk_vec = j; }

	void setCurvature(float curve) { Curvature = curve; }
	void setCurvatureRadius(float rad) { CurvatureRadius = rad; }


	void setBone(Bone *bone_) { _BoneStruct._bone = bone_; }
	void setFrame(int frame) { _BoneStruct._frame = frame; }
	void setX(float _x) { x = _x; }
	void setY(float _y) { y = _y; }
	void setZ(float _z) { z = _z; }
	void setStartPos(Vector3D start) { start_position = start; } //start of bone, also used as main position
	void setEndPos(Vector3D end) { end_position = end; } //end of bone

	Vector3D getStartPos() { return start_position; }
	Vector3D getEndPos() { return end_position; }

	float getVelocity_mag() { return velocity_mag; }
	float getAcceleration_mag() { return acceleration_mag; }
	float getJerk_mag() { return jerk_mag; }

	Vector3D getVelocity_vec() { return velocity_vec; }
	Vector3D getAcceleration_vec() { return acceleration_vec; }
	Vector3D getJerk_vec() { return jerk_vec; }

	float getCurvature() { return Curvature; }
	float getCurvatureRadius() { return CurvatureRadius; }

	Bone* getBone() { return _BoneStruct._bone; }
	int getFrame() { return _BoneStruct._frame; }
	float getX() {
		return x;
	}
	float getY() {
		return y;
	}
	float getZ() {
		return z;
	}

};

BoneData::BoneData(Bone *bone, int frame) {
	_BoneStruct._bone = bone;
	_BoneStruct._frame = frame;
}

BoneData::BoneData() {
	//_BoneStruct._bone = new Bone(0, NULL);
	_BoneStruct._frame = 0;
}

#endif //BONEDATA_H