//-----------------------------------------------------------------------------
// AMC_Writer.cpp
//	 Writes an Acclaim AMC file (motion data).
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
#include <DataManagement/AMC_Writer.h>
#include <DataManagement/FileSystem.h>
#include <Animation/Skeleton.h>
#include <Animation/MotionSequence.h>
#include <Core/Array2D.h>
#include <Core/Utilities.h>
#include <Math/Math.h>
#include <fstream>
using namespace std;

bool AMC_Writer::writeAMC(const char* outputFilename, 
		Skeleton* skeleton, 
		MotionSequence* motion,
		bool overwrite)
{
	if (!overwrite && FileSystem::fileExists(outputFilename))
		return false;

	ofstream ofs(outputFilename);
	if (!ofs) return false;

	// comments
	//ofs << "# ASF:" << skeleton->getId() << " AMC:" << motion->getId() << endl;
	//ofs << "# Documentation: " << motion->getDocumentation() << endl;
	//ofs << "# Source: " << motion->getSource() << endl;
	//ofs << "# Framerate: " << motion->getFrameRate() << endl;

	// standard qualifiers
	ofs << ":FULLY-SPECIFIED" << endl;
	ofs << ":DEGREES" << endl;

	for (int frame=0; frame<motion->numFrames(); frame++)
	{
		// write frame number (start at 1)
		ofs << frame+1 << endl;
		for (int bone_id=0; bone_id<skeleton->numBones(); bone_id++)
		{
			string bone_name = skeleton->boneNameFromId(bone_id);
			Bone* bone = skeleton->getBone(bone_id);
			// skip bones with no valid DOF
			if (bone->getChannelOrder(0) == CT_INVALID) continue;
			ofs << bone_name << " ";
			for (int d=0; d<6; d++)
			{
				// FIXIT! do not write bones that have 0 DOF
				CHANNEL_TYPE dof_id = bone->getChannelOrder(d);
				if (dof_id != CT_INVALID)
				{
					CHANNEL_ID channel_id(bone_id, dof_id);
					float value = motion->getValue(channel_id, frame);
					if ((dof_id==CT_RX) || (dof_id==CT_RY) || (dof_id==CT_RZ))
						value = rad2deg(value);
					ofs << value << " ";
				}
			}
			ofs << endl;
		}
	}
	ofs.close();
	return true;
}

