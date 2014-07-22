//-----------------------------------------------------------------------------
// BoneSequence.h
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
#ifndef BONESEQUENCE_DOT_H
#define BONESEQUENCE_DOT_H
#include "Core/SystemConfiguration.h"
#include <vector>
using namespace std;
#include "Objects/Object.h"

// A sequence of bones linked end-to-end.
// The sequence can be manipulated by changing the joint angles.

class BoneObject;

class BoneSequence : public Object
{
public:
	BoneSequence();
	virtual ~BoneSequence();

	void addBone(float _length);
	virtual void render(Matrix4x4& world_xform);
	virtual void updateState(float time_delta=0.0);

	int numBones() { return bones.size(); }
	Vector3D getAngles(int bone_index);
	void setAngles(int bone_index, const Vector3D& angles);
	Vector3D getEndpoint(int bone_index);

private:
	vector<BoneObject*> bones;
};

#endif
