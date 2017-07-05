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

#include <Core/SystemConfiguration.h>
#include <vector>
using namespace std;
#include <Objects/BoneSequence.h>
#include <Objects/BoneObject.h>
#include <Core/SystemLog.h>
#include <Models/ModelFactory.h>

struct BoneStore {
	vector<BoneObject*> bones;
};

BoneSequence::BoneSequence()
{
	bone_store = new BoneStore;
}
	
BoneSequence::~BoneSequence()
{
	for (unsigned int b=0; b<bone_store->bones.size(); b++)
	{
		if (bone_store->bones[b] != NULL) delete bone_store->bones[b];
	}
	delete bone_store;
}

int BoneSequence::numBones() { return bone_store->bones.size(); }

void BoneSequence::addBone(float _length)
{
	Color bonecolor = Color(1.0f, 0.7f, 0.0f);
	if (bone_store->bones.size() == 0) bonecolor = Color(1.0f, 0.0f, 1.0f);
	ModelSpecification bonespec("Bone", bonecolor);
	BoneObject* newbone = new BoneObject(bonespec,
			Vector3D(0.0f, 0.0f, 0.0f), 
			Vector3D(0.0f, 0.0f, 0.0f));
	newbone->setLength(_length);
	newbone->drawAsMesh();
	bone_store->bones.push_back(newbone);
}

Vector3D BoneSequence::getAngles(int bone_index)
{
	return bone_store->bones[bone_index]->currRotation();
}

void BoneSequence::setAngles(int bone_index, const Vector3D& angles)
{
	bone_store->bones[bone_index]->rotateTo(angles);
}

Vector3D BoneSequence::getEndpoint(int bone_index)
{
	return bone_store->bones[bone_index]->getEndpoint();
}


void BoneSequence::render(Matrix4x4& world_xform)
{
	for (unsigned int b=0; b<bone_store->bones.size(); b++)
	{
		Matrix4x4 xform;
		bone_store->bones[b]->render(xform);
	}
}

void BoneSequence::updateState(float time_delta)
{
	Vector3D prev_endpoint(0.0f, 0.0f, 0.0f);
	Matrix4x4 global_xform = Matrix4x4::identity();

	for (unsigned int b=0; b<bone_store->bones.size(); b++)
	{
		Vector3D bone_angles = bone_store->bones[b]->currRotation();
		Matrix4x4 r_xform = Matrix4x4::rotationZXY(bone_angles);
				
		Vector3D bone_translation(0.0f,0.0f,bone_store->bones[b]->currLength());
		Matrix4x4 t_xfrom = Matrix4x4::translationXYZ(bone_translation);
			
		Matrix4x4 bone_xform = r_xform * t_xfrom;
		global_xform = global_xform * bone_xform;

		Vector3D startpoint = prev_endpoint;
		Vector3D endpoint(global_xform.m[12], global_xform.m[13], global_xform.m[14]);
		bone_store->bones[b]->setEndpoints(startpoint, endpoint);
		prev_endpoint = endpoint;
	
	}
}
