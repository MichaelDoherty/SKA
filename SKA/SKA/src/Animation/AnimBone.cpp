//-----------------------------------------------------------------------------
// AnimBone.cpp
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
#include "Core/SystemConfiguration.h"
#include "Animation/AnimBone.h"
#include "Animation/AnimSkeleton.h"

class BonesToTrack
{
public:
	BonesToTrack()
	{
		// ASF
		bones.push_back("root");
		bones.push_back("lowerback");
		//bones.push_back("upperback");
		//bones.push_back("thorax");
		//bones.push_back("lowerneck");
		//BVH
		bones.push_back("Hips");
		bones.push_back("LowerBack");
		//bones.push_back("Spine");
		//bones.push_back("Spine1");
		//bones.push_back("Neck");
	}
	bool isTracked(string b)
	{
		for (unsigned short i=0; i<bones.size(); i++)
			if (bones[i] == b) return true;
		return false;
	}
	vector<string> bones;
};
BonesToTrack bones_to_track;


void AnimBone::update()
{
	if (new_input_data) { applyInputData(); new_input_data = false; }

	if (parent == NULL) 
	{
		Matrix4x4 base_xform = skeleton->getWorldTransformation();
		computeOffsetTransform();			// Computes B
		computeRotationTransform();			// Computes M
		L = B*M;
		W = base_xform*L;
		end_position.x = W.m[12];
		end_position.y = W.m[13];
		end_position.z = W.m[14];
	}

	else
	{
		computeRotationTransform();
		L = C*M*Cinv*B;
		W = parent->W * L;

		// NOTE: W for this bone includes translate to end of bone

		// extract bone's global coordinates for rendering
		position = parent->end_position;
		end_position.x = W.m[12];
		end_position.y = W.m[13];
		end_position.z = W.m[14];
		bone_object->setEndpoints(position, end_position);
	}
	if (base_box != NULL) base_box->moveTo(position);
	if (tip_box != NULL) tip_box->moveTo(end_position);


	if (bones_to_track.isTracked(this->getName())) 
	{
		logout << "BONE " << getName() << " --------------------------" << endl;
		logout << "  start position: " << position << endl;
		logout << "    end position: " << end_position << endl;
		logout << "     orientation: " << orientation << endl;
		logthis = true;
		logout << *this;
	}

	// update children
	list<AnimBone*>::iterator i = children.begin();
	while (i != children.end())	{ (*i)->update(); i++; }
}

// Compute the rotation due to current AMC frame angles
void AnimBone::computeRotationTransform()
{
	bool logthis = 	bones_to_track.isTracked(this->getName());
	if (logthis) logout << endl << endl << "!!start!! AnimBone::computeRotationTransform " << this->getName() << " !!start!!" << endl;
	M = Matrix4x4::identity();
	Matrix4x4 r;
	if (logthis) logout << "AnimBone::computeRotationTransform for bone " << getName() << endl;
	for (short d=0; d<6; d++)
	{
		if (description->channel_order[d] != DOF_INVALID)
		{
			short dof_idx = description->channel_order[d];
			if (description->dof[dof_idx].valid)
			{
				switch (dof_idx)
				{
				case DOF_PITCH:
					r = Matrix4x4::rotationPitch(orientation.pitch);
					M = r * M; 
					if (logthis) logout << "  apply pitch: " << orientation.pitch << endl;
					break;
				case DOF_YAW:
					r = Matrix4x4::rotationYaw(orientation.yaw);
					M = r * M; 
					if (logthis) logout << "  apply yaw: " << orientation.yaw << endl;
					break;
				case DOF_ROLL:
					r = Matrix4x4::rotationRoll(orientation.roll);
					M = r * M; 
					if (logthis) logout << "  apply roll: " << orientation.roll << endl;
					break;
				}

			}
		}
	}
	if (logthis) logout << endl << "!!!end!!! AnimBone::computeRotationTransform " << this->getName() << " !!!end!!!" << endl << endl;

}

//====================================================================
// Bone Setup functions
//====================================================================

void AnimBone::precalcData()
{
	if (!bone_data_initialized) 
	{
		// C and Cinv from axis data
		computeLocalAxisTransform();		
		// use direction and length to create transform B from bone base to bone tip
		computeOffsetTransform();
		// update children
		list<AnimBone*>::iterator i = children.begin();
		while (i != children.end())	{ (*i)->precalcData(); i++;	}
	}
	bone_data_initialized = true;
}

void AnimBone::applyInputData()
{
	short d;
	for (d=0; d<6; d++)
	{
		if (!description->dof[d].valid) continue;
		if (!input_data_are_deltas)
		{
			switch (d)
			{
			case 0: position.x = input_position.x; break;
			case 1: position.y = input_position.y; break;
			case 2: position.z = input_position.z; break;
			case 3: orientation.pitch =input_angles.pitch; break;
			case 4: orientation.yaw = input_angles.yaw; break;
			case 5: orientation.roll = input_angles.roll; break;
			}
		}
		else
		{
			switch (d)
			{
			case 0: position.x += input_position.x; break;
			case 1: position.y += input_position.y; break;
			case 2: position.z += input_position.z; break;
			case 3: orientation.pitch += input_angles.pitch; break;
			case 4: orientation.yaw += input_angles.yaw; break;
			case 5: orientation.roll += input_angles.roll; break;
			}
		}
	}
}

// compute axis transformations from ASF axis angles
void AnimBone::computeLocalAxisTransform()
{
	DOF_ID* axis_order = getAxisOrder();
	Vector3D axis = getAxis();

	C = Matrix4x4::identity();
	for (short d=0; d<3; d++)
	{
		Matrix4x4 M;
		switch(axis_order[d])
		{
		case DOF_PITCH:
			M = Matrix4x4::rotationPitch(axis.pitch);
			C = M * C;
			break;
		case DOF_YAW:
			M = Matrix4x4::rotationYaw(axis.yaw);
			C = M * C;
			break;
		case DOF_ROLL:
			M = Matrix4x4::rotationRoll(axis.roll);
			C = M* C;
			break;
		case DOF_X:
		case DOF_Y:
		case DOF_Z:
		case DOF_INVALID:
			break;
		}
	}
	Cinv = C.cheapInverse(true);
}

void AnimBone::computeOffsetTransform()
{
	if (parent == NULL)
	{
		B = Matrix4x4::translationXYZ(position);
	}
	else
	{
		Vector3D scaled_dir = getDirection() * getLength();
		B = Matrix4x4::translationXYZ(scaled_dir);		
	}
}

//********************************************************************************
// Model Setup
//********************************************************************************

void AnimBone::constructRenderObject(Color color)
{
	if (bone_object != NULL) delete bone_object;
	bone_object = NULL;
	
	ModelSpecification bonespec;
	bonespec.model_name = "Bone";
	bonespec.color = color;
	bone_object = new BoneObject(bonespec,
			Vector3D(0.0f, 0.0f, 0.0f), 
			Vector3D(0.0f, 0.0f, 0.0f));

	bone_object->setLength(getLength());
	bone_object->drawAsMesh();

	string name = description->name;
	if ((name == string("lfemur")) || (name == string("lhumerus"))) 
		bone_object->special();

	if (skeleton->show_joint_boxes && (id > 0))
	{
		ModelSpecification tip_spec;
		tip_spec.model_name = "Box";
		tip_spec.color = Color(1.0f,1.0f,1.0f);
		tip_spec.addSpec(string("length"), string("0.5"));
		tip_spec.addSpec(string("width"), string("0.5"));
		tip_spec.addSpec(string("height"), string("0.5"));

		ModelSpecification base_spec;
		base_spec.model_name = "Box";
		base_spec.color = Color(0.0f,1.0f,0.0f);
		base_spec.addSpec(string("length"), string("0.5"));
		base_spec.addSpec(string("width"), string("0.5"));
		base_spec.addSpec(string("height"), string("0.5"));

		tip_box = new Object(tip_spec,
			Vector3D(0.0f, 0.0f, 0.0f), 
			Vector3D(0.0f, 0.0f, 0.0f));
		base_box = new Object(base_spec,
			Vector3D(0.0f, 0.0f, 0.0f), 
			Vector3D(0.0f, 0.0f, 0.0f));
		if (tip_box != NULL) 
		{
			tip_box->scaleTo(Vector3D(0.5f, 1.0f, 1.0f));
			tip_box->rotateTo(Vector3D(0.0f, HALF_PI, 0.0f));
		}
		if (base_box != NULL) 
		{
			base_box->scaleTo(Vector3D(1.0f, 1.0f, 0.5f));
			tip_box->rotateTo(Vector3D(0.0f, 0.0f, HALF_PI));
		}
	}
}

//********************************************************************************
// Debugging Dumps
//********************************************************************************

ostream& operator<<(ostream& out, AnimBone& bone)
{
	out << endl << "DUMP BONE " << bone.description->name << " id " << bone.id << " DEFINITION:" << endl;

	out << *bone.description;

	out << endl << "DUMP BONE " << bone.description->name << " id " << bone.id << " DATA: " << endl;

	out << "input position: " << bone.input_position << endl;
	out << "input angles: " << bone.input_angles << endl;
	out << "start position: " << bone.position << endl;
	out << "end position: " << bone.end_position << endl;
	out << "input orientation: " << bone.orientation << endl;

	float r,p,y;

	out << "M (motion data - dynamic angles):" << endl;
	out << bone.M;
	bone.M.toEulerAnglesFromXYZ(p, y, r);
	out << "M angles(deg) " << rad2deg(p) << "," << rad2deg(y) << "," << rad2deg(r) << endl;

	out << "B (translation relative to parent bone):" << endl;
	out << bone.B;
	bone.B.toEulerAnglesFromXYZ(p, y, r);
	out << "B angles(deg) " << rad2deg(p) << "," << rad2deg(y) << "," << rad2deg(r) << endl;

	out << "C (local axis transform - relative to parent bone):" << endl;
	out << bone.C;
	bone.C.toEulerAnglesFromXYZ(p, y, r);
	out << "C angles(deg) " << rad2deg(p) << "," << rad2deg(y) << "," << rad2deg(r) << endl;
	/*
	out << "Cinv (inverse of local axis transform):" << endl;
	out << bone.Cinv;
	bone.Cinv.toEulerAnglesFromXYZ(p, y, r);
	out << "Cinv angles(deg) " << rad2deg(p) << "," << rad2deg(y) << "," << rad2deg(r) << endl;
	*/
	out << "L (complete local transform):" << endl;
	out << bone.L;
	bone.L.toEulerAnglesFromXYZ(p, y, r);
	out << "L angles(deg) " << rad2deg(p) << "," << rad2deg(y) << "," << rad2deg(r) << endl;

	out << "W (complete world transform):" << endl;
	out << bone.W;
	bone.W.toEulerAnglesFromXYZ(p, y, r);
	out << "W angles(deg) " << rad2deg(p) << "," << rad2deg(y) << "," << rad2deg(r) << endl;

	return out;
}
