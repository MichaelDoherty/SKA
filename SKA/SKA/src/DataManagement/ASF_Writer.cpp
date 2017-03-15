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

#include <Core/SystemConfiguration.h>
#include <DataManagement/ASF_Writer.h>
#include <DataManagement/FileSystem.h>
#include <Animation/Skeleton.h>
#include <Core/Utilities.h>
#include <fstream>
using namespace std;

bool ASF_Writer::writeASF(const char* outputFilename,
		Skeleton* skeleton,
		bool overwrite)
{
	if (!overwrite && FileSystem::fileExists(outputFilename))
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

	short root_id = skeleton->boneIdFromName("root");
	Bone* root_bone = skeleton->getBone(root_id);
	ofs << "   order ";
	for (int d=0; d<6; d++)
	{
		switch (root_bone->getChannelOrder(d))
		{
		case CT_TX: ofs << "TX "; break;
		case CT_TY: ofs << "TY "; break;
		case CT_TZ: ofs << "TZ "; break;
		case CT_RX: ofs << "RX "; break;
		case CT_RY: ofs << "RY "; break;
		case CT_RZ: ofs << "RZ "; break;
		default: ofs << "INVALID "; break;
		}
	}
	ofs << endl << "   axis ";
	for (int d=0; d<3; d++)
	{
		switch (root_bone->getAxisOrder(d))
		{
		case CT_RX: ofs << "X "; break;
		case CT_RY: ofs << "Y "; break;
		case CT_RZ: ofs << "Z "; break;
		default: break;
		}
	}	
	ofs << endl;
	Vector3D initposition = skeleton->getRootPosition();
	Vector3D initorientation = skeleton->getRootOrientation();
	ofs << "   position " << 
		initposition.x << " " << 
		initposition.y << " " << 
		initposition.z << endl;
	ofs << "   orientation " << 
		initorientation.pitch << " " << 
		initorientation.yaw << " " << 
		initorientation.roll << endl;

	ofs << ":bonedata" << endl;
	for (int bone_id=0; bone_id<skeleton->numBones(); bone_id++)
	{
		string bone_name = skeleton->boneNameFromId(bone_id);
		if (bone_name == string("root")) continue;
		Bone* bone = skeleton->getBone(bone_id);
		ofs << "  begin" << endl;
		
		ofs << "     id " << bone_id << endl;
		ofs << "     name " << bone_name << endl;
		Vector3D bone_direction = bone->getDirection();
		ofs << "     direction " 
			<< bone_direction.x << " " 
			<< bone_direction.y << " " 
			<< bone_direction.z << endl;
		ofs << "     length " << bone->getLength() << endl;
		ofs << "     axis ";
		for (int d=0; d<3; d++)
		{
			switch (bone->getAxisOrder(d))
			{
			case CT_RX: ofs << rad2deg(bone->getAxis().pitch) << " "; break;
			case CT_RY: ofs << rad2deg(bone->getAxis().yaw) << " "; break;
			case CT_RZ: ofs << rad2deg(bone->getAxis().roll) << " "; break;
			default: break;
			}
		}
		ofs << " ";
		for (int d=0; d<3; d++)
		{
			switch (bone->getAxisOrder(d))
			{
			case CT_RX: ofs << "X"; break;
			case CT_RY: ofs << "Y"; break;
			case CT_RZ: ofs << "Z"; break;
			default: break;
			}
		}
		ofs << endl;
		if (bone->getChannelOrder(0) != CT_INVALID)
		{
			// "dof" and "limits" lines only when bone has valid DOF
			ofs << "     dof ";
			for (int d=0; d<6; d++)
			{
				if (bone->getChannelOrder(d) != CT_INVALID) 
				{
					switch (bone->getChannelOrder(d))
					{
					case CT_TX: ofs << "TX"; break;
					case CT_TY: ofs << "TY"; break;
					case CT_TZ: ofs << "TZ"; break;
					case CT_RX: ofs << "RX"; break;
					case CT_RY: ofs << "RY"; break;
					case CT_RZ: ofs << "RZ"; break;
					case CT_QW: ofs << "QW"; break;
					case CT_QX: ofs << "QX"; break;
					case CT_QY: ofs << "QY"; break;
					case CT_QZ: ofs << "QZ"; break;
					default: ofs << "--"; break;
					}
					ofs << " ";
				}
			}
			ofs << endl;
			ofs << "     limits ";
			for (int d=0; d<6; d++)
			{
				if (bone->getChannelOrder(d) != CT_INVALID) 
				{
					if (d>0) ofs << "            ";
					int d2 = 0;
					switch (bone->getChannelOrder(d))
					{
					case CT_RX: d2 = 3; break;
					case CT_RY: d2 = 4; break;
					case CT_RZ: d2 = 5; break;
					default: break;
					}
					ofs << "(";
					if (bone->getChannelLowerLimit(d2) == -1.0f*FLT_MAX) ofs << "-inf";
					else ofs << rad2deg(bone->getChannelLowerLimit(d2));
					ofs << " ";
					if (bone->getChannelUpperLimit(d2) == FLT_MAX) ofs << "inf";
					else ofs << rad2deg(bone->getChannelUpperLimit(d2)); 
					ofs << ")";
					ofs <<  endl;
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
		list<pair<char*,char*> > connections = skeleton->getConnections();
		list<pair<char*,char*> >::iterator iter = connections.begin();
		int count = 0;
		while (iter != connections.end())
		{
			if (strcmp((*iter).first, bone_name.c_str()) == 0)
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
