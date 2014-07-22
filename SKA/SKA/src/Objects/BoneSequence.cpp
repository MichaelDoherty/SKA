//-----------------------------------------------------------------------------
// BoneSequence.cpp
//    An object that is made up of a sequence of bones.
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
#include "Objects/BoneSequence.h"
#include "Objects/BoneObject.h"
#include "Core/SystemLog.h"

BoneSequence::BoneSequence()
{
}
	
BoneSequence::~BoneSequence()
{
	for (unsigned int b=0; b<bones.size(); b++)
	{
		if (bones[b] != NULL) delete bones[b];
	}
}

void BoneSequence::addBone(float _length)
{
	ModelSpecification bonespec;
	bonespec.model_name = "Bone";
	if (bones.size() == 0) bonespec.color = Color(1.0f, 0.0f, 1.0f);
	else bonespec.color = Color(1.0f, 0.7f, 0.0f);
	BoneObject* newbone = new BoneObject(bonespec,
			Vector3D(0.0f, 0.0f, 0.0f), 
			Vector3D(0.0f, 0.0f, 0.0f));
	newbone->setLength(_length);
	newbone->drawAsMesh();
	bones.push_back(newbone);
}

Vector3D BoneSequence::getAngles(int bone_index)
{
	return bones[bone_index]->currRotation();
}

void BoneSequence::setAngles(int bone_index, const Vector3D& angles)
{
	bones[bone_index]->rotateTo(angles);
}

Vector3D BoneSequence::getEndpoint(int bone_index)
{
	return bones[bone_index]->getEndpoint();
}


void BoneSequence::render(Matrix4x4& world_xform)
{
	for (unsigned int b=0; b<bones.size(); b++)
	{
		Matrix4x4 xform;
		bones[b]->render(xform);
	}
}

void BoneSequence::updateState(float time_delta)
{
	Vector3D prev_endpoint(0.0f, 0.0f, 0.0f);
	Matrix4x4 global_xform = Matrix4x4::identity();

	for (unsigned int b=0; b<bones.size(); b++)
	{
		Vector3D bone_angles = bones[b]->currRotation();
		Matrix4x4 r_xform = Matrix4x4::rotationRPY(bone_angles);
				
		Vector3D bone_translation(0.0f,0.0f,bones[b]->currLength());
		Matrix4x4 t_xfrom = Matrix4x4::translationXYZ(bone_translation);
			
		Matrix4x4 bone_xform = r_xform * t_xfrom;
		global_xform = global_xform * bone_xform;

		Vector3D startpoint = prev_endpoint;
		Vector3D endpoint(global_xform.m[12], global_xform.m[13], global_xform.m[14]);
		bones[b]->setEndpoints(startpoint, endpoint);
		prev_endpoint = endpoint;
	
	}
}
