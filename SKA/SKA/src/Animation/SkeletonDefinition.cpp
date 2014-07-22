//-----------------------------------------------------------------------------
// SkeletonDefinition.cpp
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
#include "Core/SystemConfiguration.h"
#include "Animation/SkeletonDefinition.h"
#include "Core/SystemLog.h"
#include "Core/Utilities.h"

void SkeletonDefinition::interpretAMCData(int bone_id, float data[6], Vector3D& pos, Vector3D& angles)
{
	for (short d=0; d<6; d++)
	{
		DOF_ID dof_idx = bone_descriptions[bone_id].channel_order[d];
		if (dof_idx != DOF_INVALID)
		{
			if (bone_descriptions[bone_id].dof[dof_idx].valid)
			{
				switch (dof_idx)
				{
				case DOF_X: pos.x = data[d]; break;
				case DOF_Y: pos.y = data[d]; break;
				case DOF_Z: pos.z = data[d]; break;
				case DOF_PITCH: angles.pitch = data[d]; break;
				case DOF_YAW: angles.yaw = data[d]; break;
				case DOF_ROLL: angles.roll = data[d]; break;
				case DOF_INVALID: break;
				}
			}
		}
	}
}

ostream& operator<<(ostream& out, SkeletonDefinition& skel)
{
	out << "ASF_Skeleton dump =================================" << endl;
	out << "Documentation:" << skel.documentation << endl;
	out << "Units:" << endl;
	map<string,string>::iterator units_iter = skel.units.begin();
	while (units_iter != skel.units.end())
	{
		out << "\t" << (*units_iter).first << " = " << (*units_iter).second << endl;
		units_iter++;
	}

	for (short b=0; b<skel.num_bones; b++)
	{
		out <<  "ASF BONE " << b << " --------------------------------" << endl;
		out << skel.bone_descriptions[b];
	}

	out << "HIERARCHY --------------------------" << endl;
	list<pair<string,string> >::iterator citer = skel.connections.begin();
	while (citer != skel.connections.end())
	{
		out << "\t" << (*citer).first << " " << (*citer).second << endl;
		citer++;
	}
	out << "======================== end ASF_Skeleton dump" << endl;
	return out;
}
