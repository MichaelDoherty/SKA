//-----------------------------------------------------------------------------
// AnimBone.h
//	 Animated bone = real-time data for a bone instance.
//-----------------------------------------------------------------------------
// This software is part of the Skeleton Animation Toolkit (SKA) developed 
// at the University of the Pacific, under the guidance of Michael Doherty.
// For information please contact mdoherty@pacific.edu.
//-----------------------------------------------------------------------------
// This is open software. You are free to use it as you see fit.
// The University of the Pacific and identified authors would appreciate
// being credited for any significant use, particularly if used for
// commercial projects or academic research publications.
//-----------------------------------------------------------------------------
// Version 3.0 - July 18, 2014 - Michael Doherty
//-----------------------------------------------------------------------------
#ifndef ANIMBONE_DOT_H
#define ANIMBONE_DOT_H
#include "Core/SystemConfiguration.h"
#include <list>
#include <iostream>
using namespace std;
#include "Animation/SkeletonDefinition.h"
#include "Objects/BoneObject.h"

class AnimSkeleton;

class AnimBone
{
	friend ostream& operator<<(ostream& out, AnimBone& bone);
	friend class AnimSkeleton;
private:
	short id; // this is also an index back to the bone description

	AnimSkeleton* skeleton;
	AnimBone* parent;			// if parent==NULL, this is the root 
	list<AnimBone*> children;		

	// data loaded but not yet activated
	bool new_input_data;
	bool input_data_are_deltas;
	Vector3D input_position;
	Vector3D input_angles;

	Matrix4x4 C;		// local axis transform (relative to parent bone)
	Matrix4x4 Cinv;		// inverse of local axis transform

	Matrix4x4 B;		// translation relative to parent bone (from direction and length)
						// B is actually translation from start_position to end_position?

	Matrix4x4 M;		// motion data (dynamic angles)
	Matrix4x4 L;		// complete local transform
	Matrix4x4 W;		// complete world transform

	Vector3D position;				// base of bone (world coordinates)
	Vector3D end_position;			// tip of bone (world coordinates) 
	Vector3D orientation;

	bool  bone_data_initialized; 

	// Connection to graphics system
	BoneObject* bone_object;
	Object* base_box;
	Object* tip_box;

	BoneDefinition* description;  // pointer back into ASF data

	bool logthis;

public:
	AnimBone(int _id, SkeletonDefinition* skeleton_description, AnimSkeleton* _skeleton)
		: id(_id)
	{
		skeleton = _skeleton;
		description = skeleton_description->getBoneDescription(id);
		bone_object = NULL; 
		base_box = NULL;
		tip_box = NULL;
		parent = NULL; 
		bone_data_initialized=false; 
		new_input_data = false;
		input_data_are_deltas = false;
		logthis = false;
	}

	virtual ~AnimBone()
	{
	}

	short getID() { return id; }
	string getName() { return description->name; }
	float getLength() { return description->length; }
	Vector3D getDirection() { return description->direction; }
	DOF_ID* getAxisOrder() { return description->axis_order; }
	Vector3D getAxis() { return description->axis; }
	BoneDefinition* getBoneDefinition() { return description; }

	void update();
	void computeRotationTransform();
	void precalcData();
	void applyInputData();
	void computeOffsetTransform();
	void computeLocalAxisTransform();
	void constructRenderObject(Color color=Color(1.0f,1.0f,0.0f));

	void setPose(Vector3D p, Vector3D a, bool are_deltas = false)
	{
		new_input_data = true;
		input_position = p;
		input_angles = a;
		input_data_are_deltas = are_deltas;
	}

};

#endif
