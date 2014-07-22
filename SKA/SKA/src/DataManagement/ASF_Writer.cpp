//-----------------------------------------------------------------------------
// ASF_Writer.cpp
//	 Writes Acclaim Skeleton File (ASF)
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
#include "DataManagement/ASF_Writer.h"
#include "DataManagement/FileSystem.h"
#include "Animation/SkeletonDefinition.h"
#include "Core/DOF.h"
#include "Core/Utilities.h"
#include <fstream>
using namespace std;

bool ASF_Writer::writeASF(const char* outputFilename,
		SkeletonDefinition* skeleton,
		bool overwrite)
{
	if (!overwrite && FileSystem::fileExists(string(outputFilename))) 
		return false;

	ofstream ofs (outputFilename);
	if (!ofs) return false;

	// comments
	ofs << "# ASF:" << skeleton->getId() << endl;
	ofs << "# Documentation: " << skeleton->getDocumentation() << endl;
	ofs << "# Source: " << skeleton->getSource() << endl;

	// standard qualifiers
	ofs << ":version 1.10" << endl;
	ofs << ":name VICON" << endl;

	ofs << ":units" << endl;
	ofs << "  mass " << 1.0f << endl;		// FIXIT! 
	ofs << "  length " << 0.45f << endl;    // FIXIT!
	ofs << "  angle deg" << endl;

	ofs << ":documentation" << endl;
	ofs << "   Created at University of the Pacific" << endl;

	//:root
	//   order TX TY TZ RX RY RZ
	//   axis XYZ
	//   position 0 0 0  
	//   orientation 0 0 0 
	ofs << ":root" << endl;

	short root_id = skeleton->boneIdFromName(string("root"));
	BoneDefinition* root_bone = skeleton->getBoneDescription(root_id);
	ofs << "   order ";
	for (int d=0; d<6; d++)
	{
		switch (root_bone->channel_order[d])
		{
		case DOF_X: ofs << "TX "; break;
		case DOF_Y: ofs << "TY "; break;
		case DOF_Z: ofs << "TZ "; break;
		case DOF_PITCH: ofs << "RX "; break;
		case DOF_YAW: ofs << "RY "; break;
		case DOF_ROLL: ofs << "RZ "; break;
		case DOF_INVALID: ofs << "INVALID "; break;
		}
	}
	ofs << endl << "   axis ";
	for (int d=0; d<3; d++)
	{
		switch (root_bone->axis_order[d])
		{
		case DOF_PITCH: ofs << "X "; break;
		case DOF_YAW: ofs << "Y "; break;
		case DOF_ROLL: ofs << "Z "; break;
		case DOF_X:
		case DOF_Y:
		case DOF_Z:
		case DOF_INVALID: break;
		}
	}	
	ofs << endl;
	ofs << "   position " << 
		skeleton->init_root_position.x << " " << 
		skeleton->init_root_position.y << " " << 
		skeleton->init_root_position.z << endl;
	ofs << "   orientation " << 
		skeleton->init_root_orientation.pitch << " " << 
		skeleton->init_root_orientation.yaw << " " << 
		skeleton->init_root_orientation.roll << endl;

	ofs << ":bonedata" << endl;
	for (int bone_id=0; bone_id<skeleton->numBones(); bone_id++)
	{
		string bone_name = skeleton->boneNameFromId(bone_id);
		if (bone_name == string("root")) continue;
		BoneDefinition* bone = skeleton->getBoneDescription(bone_id);
		ofs << "  begin" << endl;
		
		ofs << "     id " << bone_id << endl;
		ofs << "     name " << bone_name << endl;
		ofs << "     direction " 
			<< bone->direction.x << " " 
			<< bone->direction.y << " " 
			<< bone->direction.z << endl;
		ofs << "     length " << bone->length << endl;
		ofs << "     axis ";
		for (int d=0; d<3; d++)
		{
			switch (bone->axis_order[d])
			{
			case DOF_PITCH: ofs << rad2deg(bone->axis.pitch) << " "; break;
			case DOF_YAW: ofs << rad2deg(bone->axis.yaw) << " "; break;
			case DOF_ROLL: ofs << rad2deg(bone->axis.roll) << " "; break;
			case DOF_X:
			case DOF_Y:
			case DOF_Z:
			case DOF_INVALID: break;
			}
		}
		ofs << " ";
		for (int d=0; d<3; d++)
		{
			switch (bone->axis_order[d])
			{
			case DOF_PITCH: ofs << "X"; break;
			case DOF_YAW: ofs << "Y"; break;
			case DOF_ROLL: ofs << "Z"; break;
			case DOF_X:
			case DOF_Y:
			case DOF_Z:
			case DOF_INVALID: break;
			}
		}
		ofs << endl;
		if (bone->channel_order[0] != DOF_INVALID)
		{
			// "dof" and "limits" lines only when bone has valid DOF
			ofs << "     dof ";
			for (int d=0; d<6; d++)
			{
				if (bone->channel_order[d] != DOF_INVALID) 
					ofs << bone->channel_order[d] << " ";
			}
			ofs << endl;
			ofs << "     limits ";
			for (int d=0; d<6; d++)
			{
				if (bone->channel_order[d] != DOF_INVALID) 
				{
					if (d>0) ofs << "            ";
					int d2 = 0;
					switch (bone->channel_order[d])
					{
					case DOF_PITCH: d2 = 3; break;
					case DOF_YAW: d2 = 4; break;
					case DOF_ROLL: d2 = 5; break;
					case DOF_X:
					case DOF_Y:
					case DOF_Z:
					case DOF_INVALID: break;
					}
					ofs << "(" 
						<< rad2deg(bone->dof[d2].min) << "," 
						<< rad2deg(bone->dof[d2].max) << ")" <<  endl;
				}
					
			}
		}
		ofs << "  end" << endl;
	}
	ofs << ":hierarchy" << endl;
	ofs << "  begin" << endl;

	for (int bone_id=0; bone_id<skeleton->numBones(); bone_id++)
	{
		string bone_name = skeleton->boneNameFromId(bone_id);
		list<pair<string,string> >::iterator iter = skeleton->connections.begin();
		int count = 0;
		while (iter != skeleton->connections.end())
		{
			if ((*iter).first == bone_name)
			{
				if (count == 0) ofs << "    " << bone_name << " ";
				count++;
				ofs << (*iter).second << " ";
			}
			iter++;
		}
		if (count > 0) ofs << endl;
	}
	ofs << "  end" << endl;

	ofs.close();
	return true;
}
