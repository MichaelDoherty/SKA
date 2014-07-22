//-----------------------------------------------------------------------------
// SkeletonDefinition.h
//	 Stores the static definition of a skeleton.
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
#ifndef SKELETON_DEFINITION_DOT_H
#define SKELETON_DEFINITION_DOT_H
#include "Core/SystemConfiguration.h"
#include <list>
#include <map>
#include <string>
#include <iostream>
using namespace std;
#include <float.h>
#include "Animation/BoneDefinition.h"
#include "Math/Math.h"
#include "Math/Vector3D.h"
#include "Core/DOF.h"
#include "Core/SystemLog.h"

static const short MAX_ASF_BONES = 50;

class SkeletonDefinition
{
	friend ostream& operator<<(ostream& out, SkeletonDefinition& skel);
	friend class DatabaseLoader;
	friend class ASF_Writer;

private:

	// unique identifier name
	string skel_id;

	// comments - not used for animation
	string documentation;
	string source;

	map<string, string> units;
	list<pair<string,string> > connections; // parent -> child
	map<string, short> bone_name_to_id;
	map<short, string> bone_id_to_name;

	BoneDefinition bone_descriptions[MAX_ASF_BONES];
	short num_bones;

	Vector3D init_root_position;
	Vector3D init_root_orientation;

public:

	SkeletonDefinition() : skel_id("UNNAMED")
	{
		documentation = string("");
		num_bones = 0;
	}

	SkeletonDefinition(const string& _id) : skel_id(_id)
	{
		documentation = string("");
		num_bones = 0;
	}

	~SkeletonDefinition()
	{
	}

	// === accessors ===

	string getId() { return skel_id; }

	short boneIdFromName(const string& name)
	{
		map<string,short>::iterator iter = bone_name_to_id.find(name);
		if (iter == bone_name_to_id.end()) return -1;
		return (*iter).second;
	}

	string boneNameFromId(short id)
	{
		map<short,string>::iterator iter = bone_id_to_name.find(id);
		if (iter == bone_id_to_name.end()) return string("UNKNOWN");
		return (*iter).second;
	}

	int numBones() { return num_bones; }

	bool isActiveDOF(short bone_index, short dof_index)
	{
		return bone_descriptions[bone_index].dof[dof_index].valid;
	}

	short getParentBoneId(short id)
	{
		string child_name = boneNameFromId(id);
		list<pair<string,string> >::iterator iter = connections.begin();
		while (iter != connections.end())
		{
			if ((*iter).second == child_name)
				return boneIdFromName((*iter).first);
			iter++;
		}
		return -1;
	}

	BoneDefinition* getBoneDescription(short id)
	{
		return &(bone_descriptions[id]);
	}

	// === ASF initialization interface ===
	void setId(const string& _id) { skel_id = _id; }

	void clearDocumentation() { documentation = string(""); }
	void addDocumentation(const string& d) { documentation += d; }
	string getDocumentation() { return documentation; }

	void setSource(const string& s) { source = s; }
	string getSource() { return source; }

	void addUnits(string& unit_name, string& unit_value)
	{
		units.insert(pair<string,string>(unit_name, unit_value));
	}

	void setRootPosition(float x, float y, float z)
	{
		init_root_position = Vector3D(x, y, z);
	}

	void setRootOrientation(float pitch, float yaw, float roll)
	{
		init_root_position = Vector3D(pitch, yaw, roll);
	}

	void setBoneName(short id, const string& name)
	{
		bone_descriptions[id].name = name;
		if (id>num_bones-1) num_bones = id+1;
		bone_name_to_id.insert(pair<string,short>(name,id));
		bone_id_to_name.insert(pair<short,string>(id,name));
	}

	void setBoneDirection(short id, float x, float y, float z)
	{
		bone_descriptions[id].direction = Vector3D(x,y,z);
	}

	void setBoneLength(short id, float length)
	{
		bone_descriptions[id].length = length;
	}
	
	void setBoneAxis(short id, float values[3], DOF_ID dof[3])
	{
		bone_descriptions[id].axis_order[0] = dof[0];
		bone_descriptions[id].axis_order[1] = dof[1];
		bone_descriptions[id].axis_order[2] = dof[2];
		for (short d=0; d<3; d++)
		{
			switch (dof[d])
			{
				case DOF_PITCH: bone_descriptions[id].axis.x = values[d]; break;
				case DOF_YAW: bone_descriptions[id].axis.y   = values[d]; break;
				case DOF_ROLL: bone_descriptions[id].axis.z  = values[d]; break;
				case DOF_X:
				case DOF_Y:
				case DOF_Z:
				case DOF_INVALID: break;
			}
		}
		logout << "SkeletonDefinition::setBoneAxis " 
			<< bone_descriptions[id].axis_order[0]  << "," 
			<< bone_descriptions[id].axis_order[1] << "," 
			<< bone_descriptions[id].axis_order[2] << endl;
		logout << "..... " 
			<< bone_descriptions[id].axis.x << ","
			<< bone_descriptions[id].axis.y << ","
			<< bone_descriptions[id].axis.z << endl;
	}

	void setBoneChannels(short id, DOF_ID dof_index[6], short num_dof)
	{
		for (short d=0; d<num_dof; d++)
		{
			bone_descriptions[id].channel_order[d] = dof_index[d];
			bone_descriptions[id].dof[dof_index[d]].valid = true;
		}	
	}

	void setBoneDOFLimit(short id, short idx, float min, float max)
	{
		short dof_idx = bone_descriptions[id].channel_order[idx];
		bone_descriptions[id].dof[dof_idx].min = min;
		bone_descriptions[id].dof[dof_idx].max = max;
	}

	void addConnection(string& parent, string& child)
	{
		connections.push_back(pair<string,string>(parent,child));
	}

	void scaleBoneLengths(float _scale=1.0f)
	{
		for (int b=0; b<num_bones; b++)
			bone_descriptions[b].length *=_scale;
	}

	void interpretAMCData(int bone_id, float data[6], Vector3D& pos, Vector3D& angles);
};

ostream& operator<<(ostream& out, BoneDefinition& bone);
ostream& operator<<(ostream& out, SkeletonDefinition& skel);

#endif
