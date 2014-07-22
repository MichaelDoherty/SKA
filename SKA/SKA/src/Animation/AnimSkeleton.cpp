//-----------------------------------------------------------------------------
// AnimSkeleton.cpp
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
#include "Core/SystemConfiguration.h"
#include "Animation/AnimSkeleton.h"
#include "Animation/AnimationException.h"
#include "Animation/RawMotionController.h"

static const bool FORCE_UPDATE = true;

void AnimSkeleton::update(float _time, bool DEBUG_ANGLES)
{ 
	if (motion_controller == NULL) 
	{
		char s[1000];
		sprintf(s, "skeleton %s has no motion controller", description->getId().c_str());
		logout << s << endl;
		throw AnimationException(s);
		return;
	}

	// compute skeletal transform for offsets
	Matrix4x4 translation_xform = Matrix4x4::translationXYZ(offset_position);
	Matrix4x4 rotation_xform = Matrix4x4::rotationRPY(offset_rotation);

	// TESTING HACK JAN 27 2014
	if (DEBUG_ANGLES)
	{
		Matrix4x4 r_pitch = Matrix4x4::rotationPitch(offset_rotation.pitch);
		Matrix4x4 r_roll  = Matrix4x4::rotationRoll(offset_rotation.roll);
		Matrix4x4 r_yaw   = Matrix4x4::rotationYaw(offset_rotation.yaw);
		Matrix4x4 r_mod   = Matrix4x4::rotationYaw(-PI/4.0f);
		rotation_xform = r_mod * r_roll * r_pitch * r_yaw;
		//rotation_xform = r_roll * r_pitch * r_yaw;
	}

	world_xform = translation_xform*rotation_xform;

	for (short id=0; id<num_bones; id++)
	{
		AnimBone* bone = bone_array[id];
		if (bone != NULL)
		{
			short bid = bone->getID();
			Vector3D p, a;
			CHANNEL_ID cx(BONE_ID(bid),DOF_X);
			CHANNEL_ID cy(BONE_ID(bid),DOF_Y);
			CHANNEL_ID cz(BONE_ID(bid),DOF_Z);
			CHANNEL_ID cpitch(BONE_ID(bid),DOF_PITCH);
			CHANNEL_ID cyaw(BONE_ID(bid),DOF_YAW);
			CHANNEL_ID croll(BONE_ID(bid),DOF_ROLL);

			if (motion_controller->isValidChannel(cx, _time))
				p.x = motion_controller->getValue(cx, _time);
			if (motion_controller->isValidChannel(cy, _time))
				p.y = motion_controller->getValue(cy, _time);
			if (motion_controller->isValidChannel(cz, _time))
				p.z = motion_controller->getValue(cz, _time);
			if (motion_controller->isValidChannel(cpitch, _time))
				a.pitch = motion_controller->getValue(cpitch, _time);
			if (motion_controller->isValidChannel(cyaw, _time))
				a.yaw = motion_controller->getValue(cyaw, _time);
			if (motion_controller->isValidChannel(croll, _time))
				a.roll = motion_controller->getValue(croll, _time);
	
			if (id==0) p += offset_position;
			bone->setPose(p, a);
			//logout << "bone->setPose bone=" << id << " p=" << p << " a=" << a << endl;
		}
		else
		{
			logout << "Missing BONE (id" << id << ")  in ASF_Skeleton::update" << endl;
		}
	}
	bone_array[0]->update(); 
}


void AnimSkeleton::constructRenderObject(list<Object*>& objects, Color color)
{
	for (short id=0; id<num_bones; id++)
	{
		AnimBone* bone = bone_array[id];
		bone->constructRenderObject(color);
		objects.push_back(bone->bone_object);
		if (bone->base_box != NULL)
			objects.push_back(bone->base_box);
		if (bone->tip_box != NULL)
			objects.push_back(bone->tip_box);
	}
}

void AnimSkeleton::getBonePositions(const string& bone_name, Vector3D& start, Vector3D& end)
{
	short bone_id = description->boneIdFromName(bone_name);
	getBonePositions(bone_id, start, end);
}

void AnimSkeleton::getBonePositions(int bone_id, Vector3D& start, Vector3D& end)
{
	AnimBone* bone = bone_array[bone_id];
	start = bone->position;
	end = bone->end_position;
}

BoneDefinition* AnimSkeleton::getBoneDefinition(const string& bone_name)
{
	short bone_id = description->boneIdFromName(bone_name);
	if (bone_id == -1) return NULL;
	AnimBone* bone = bone_array[bone_id];
	return bone->getBoneDefinition();
}

float AnimSkeleton::getChannelValue(CHANNEL_ID& _channel)
{
	AnimBone* bone = bone_array[_channel.bone_id];
	if (bone == NULL) return 0.0f;
	switch (_channel.dof_id)
	{
	case DOF_X: return bone->position.x;
	case DOF_Y: return bone->position.y;
	case DOF_Z: return bone->position.z;
	case DOF_PITCH: return bone->orientation.pitch;
	case DOF_YAW: return bone->orientation.yaw;
	case DOF_ROLL: return bone->orientation.roll;
	case DOF_INVALID: return 0.0f;
	}
	return 0.0f;
}

ostream& operator<<(ostream& out, const AnimSkeleton& skel)
{
	out << "AnimSkelton: instance of " << skel.description->getId() << endl;
	for (short id=0; id<skel.num_bones; id++)
	{
		out << "AnimSkelton: Bone id " << id << endl;
		out << skel.bone_array[id];
	}
	return out;
}


// Cycle through all frames in motion sequence and record
// all joint positions in the pmat table.

bool AnimSkeleton::buildPositionMatrix(Array2D<float>& pmat)
{ 
	if (motion_controller == NULL) 
	{
		char s[1000];
		sprintf(s, "AnimSkeleton::buildPositionMatrix failed - skeleton %s has no motion controller", description->getId().c_str());
		logout << s << endl;
		throw AnimationException(s);
		return false;
	}

	// DANGEROUS! - (but this is only special case code)
	RawMotionController* rawctrl = (RawMotionController*)motion_controller;
	int num_frames = rawctrl->numFrames();

	pmat.resize(num_frames, 3*num_bones);

	for (int frame=0; frame<num_frames; frame++)
	{
		// compute skeletal transform for offsets
		Matrix4x4 translation_xform = Matrix4x4::translationXYZ(offset_position);
		Matrix4x4 rotation_xform = Matrix4x4::rotationRPY(offset_rotation);
		world_xform = translation_xform*rotation_xform;

		for (short id=0; id<num_bones; id++)
		{
			AnimBone* bone = bone_array[id];
			if (bone != NULL)
			{
				short bid = bone->getID();
				Vector3D p, a;
				CHANNEL_ID cx(BONE_ID(bid),DOF_X);
				CHANNEL_ID cy(BONE_ID(bid),DOF_Y);
				CHANNEL_ID cz(BONE_ID(bid),DOF_Z);
				CHANNEL_ID cpitch(BONE_ID(bid),DOF_PITCH);
				CHANNEL_ID cyaw(BONE_ID(bid),DOF_YAW);
				CHANNEL_ID croll(BONE_ID(bid),DOF_ROLL);
				
				if (rawctrl->isValidChannel(cx))
					p.x = rawctrl->getValueByFrame(cx, frame);
				if (rawctrl->isValidChannel(cy))
					p.y = rawctrl->getValueByFrame(cy, frame);
				if (rawctrl->isValidChannel(cz))
					p.z = rawctrl->getValueByFrame(cz, frame);
				if (rawctrl->isValidChannel(cpitch))
					a.pitch = rawctrl->getValueByFrame(cpitch, frame);
				if (rawctrl->isValidChannel(cyaw))
					a.yaw = rawctrl->getValueByFrame(cyaw, frame);
				if (rawctrl->isValidChannel(croll))
					a.roll = rawctrl->getValueByFrame(croll, frame);

				if (id==0) p += offset_position;
				bone->setPose(p, a);
			}
		}
		bone_array[0]->update(); 

		// now cycle through bones and record positions
		for (short id=0; id<num_bones; id++)
		{
			Vector3D start, end;
			getBonePositions(id, start, end);
			pmat.set(frame, id*3,   end.x);
			pmat.set(frame, id*3+1, end.y);
			pmat.set(frame, id*3+2, end.z);
		}
	}

	return true;
}

void AnimSkeleton::dumpBoneList(ostream& ostr)
{
	int num_bones = description->numBones();
	for (int b=0; b<num_bones; b++)
	{
		string bone_name = description->boneNameFromId(b);
		ostr << bone_name << ", " << b << endl;
	}
}