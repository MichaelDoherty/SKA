//-----------------------------------------------------------------------------
// AnimSkeleton.h
//	 Animated skeleton = real-time data for a skeleton instance.
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
#ifndef ANIMSKELETON_DOT_H
#define ANIMSKELETON_DOT_H
#include "Core/SystemConfiguration.h"
#include <iostream>
using namespace std;
#include "Animation/SkeletonDefinition.h"
#include "Animation/MotionSequenceController.h"
#include "Animation/AnimBone.h"
#include "Objects/Object.h"

// FIXIT! Currently, there is no interpolation between frames

class SKA_LIB_DECLSPEC AnimSkeleton
{
	friend ostream& operator<<(ostream& out, const AnimSkeleton& skel);
	friend class AnimBone;

private:
	SkeletonDefinition* description;

	AnimBone* bone_array[MAX_ASF_BONES];
	short num_bones;

	MotionController* motion_controller;
	float sequence_time;	// seconds since beginning of sequence

	Vector3D offset_position;
	Vector3D offset_rotation;

	Matrix4x4 world_xform;

	bool show_joint_boxes;

public:

	AnimSkeleton(SkeletonDefinition* _description)
	{
		for (short b=0; b<MAX_ASF_BONES; b++)
			bone_array[b] = NULL;
		description = _description;
		buildBonesFromASF();
		sequence_time = 0.0f;
		offset_position = Vector3D(0.0f,0.0f,0.0f);
		offset_rotation = Vector3D(0.0f,0.0f,0.0f);
		show_joint_boxes = true;
	}
	
	virtual ~AnimSkeleton()
	{
		clear();
	}

	void clear()
	{
		for (int b=0; b<num_bones; b++)
		{
			if (bone_array[b] != NULL)
			{
				delete bone_array[b];
				bone_array[b] = NULL;
			}
		}
	}

	void buildBonesFromASF(float _scale=1.0f)
	{
		short b;
		num_bones = description->numBones();
		for (b=0; b<num_bones; b++)
		{
			bone_array[b] = new AnimBone(b, description, this);
		}
		for (b=0; b<num_bones; b++)
		{
			short parent_id = description->getParentBoneId(b);
			if (parent_id < 0) continue;
			bone_array[parent_id]->children.push_back(bone_array[b]);
			bone_array[b]->parent = bone_array[parent_id];
		}
		for (b=0; b<num_bones; b++)
			bone_array[b]->precalcData();
	}

	// === run-time interface ===

	void update(float _time, bool DEBUG_ANGLES=false);

	float currentSequenceTime() { return sequence_time; }
	void getBonePositions(const string& bone_name, Vector3D& start, Vector3D& end);
	void getBonePositions(int bone_id, Vector3D& start, Vector3D& end);
	BoneDefinition* getBoneDefinition(const string& bone_name);
	int numBones() { return num_bones; }
	float getChannelValue(CHANNEL_ID& _channel);

	bool buildPositionMatrix(Array2D<float>& pmat);

	MotionController* getMotionController() { return motion_controller; }
	SkeletonDefinition* getDescription() { return description; }

	void attachMotionController(MotionController* _controller)
	{
		motion_controller = _controller;
	}

	void setOffsetPosition(Vector3D& offset)
	{
		offset_position = offset;
	}

	void setOffsetRotation(Vector3D& rotation)
	{
		offset_rotation = rotation;
	}

	Matrix4x4& getWorldTransformation()
	{
		return world_xform;
	}

	void showJointBoxes(bool flag=true)
	{ 
		show_joint_boxes = flag; 
	}

	// === run-time setup interface ===

	void constructRenderObject(list<Object*>& objects, Color color=Color(1.0f,1.0f,0.0f));

	void dumpBoneList(ostream& ostr);
};

#endif
