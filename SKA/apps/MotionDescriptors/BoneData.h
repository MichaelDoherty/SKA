//-----------------------------------------------------------------------------
// MotionDescriptors project - Builds with SKA Version 4.0
//-----------------------------------------------------------------------------
// BoneData.h
//			A class that holds a variety of information on each bone for thew running animation
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
	float velocity = 0.0;
	float acceleration = 0.0;
	float jerk = 0.0;
	Vector3D start_position, end_position;
	float x, y, z = 0.0;


public:
	//Constructor takes in info
	BoneData();
	BoneData(Bone *bone, int frame);

	//set and get functions
	void setVelocity(float vel) { velocity = vel; }
	void setAcceleraction(float accel) { acceleration = accel; }
	void setJerk(float j) { jerk = j; }
	void setBone(Bone *bone_) { _BoneStruct._bone = bone_; }
	void setFrame(int frame) { _BoneStruct._frame = frame; }
	void setX(float _x) { x = _x; }
	void setY(float _y) { y = _y; }
	void setZ(float _z) { z = _z; }
	void setStartPos(Vector3D start) { start_position = start; } //start of bone
	void setEndPos(Vector3D end) { end_position = end; } //end of bone

	float getVelocity() { return velocity; }
	float getAcceleration() { return acceleration; }
	float getJerk() { return jerk; }
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