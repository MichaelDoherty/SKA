//-----------------------------------------------------------------------------
// AMC_Reader.cpp
//	 Reads an Acclaim AMC file (motion data).
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
#include <cstdlib>
#include "DataManagement/AMC_Reader.h"
#include "DataManagement/ASF_AMC_ParseUtils.h"
#include "Core/Array2D.h"
#include "Animation/SkeletonDefinition.h"
#include "Animation/MotionSequence.h"
	
MotionSequence* AMC_Reader::readAMC(const char* motionFilename, SkeletonDefinition* skeleton)
{
	string line;

	// read once, just to get the number of frames
	LineScanner line_scanner1(motionFilename);
	if (!line_scanner1.fileIsOpen()) return false;
	int frame_count = 0;
	while (line_scanner1.getNextLine(line))
	{
		int frame;
		if (AAPU::linePrefix(line, string("#"))) continue;
		if (AAPU::lineIsEmpty(line)) continue;
		if (AAPU::lineIsSingleInt(line, frame))
			frame_count = frame;	
	}
	line_scanner1.close();

	Array2D<float> data;
	motion = new MotionSequence;

	data.resize(frame_count, 6*skeleton->numBones());

	// now read for storage
	AMC_angles_are_degrees = false;
	int frame;

	LineScanner line_scanner2(motionFilename);
	if (!line_scanner2.fileIsOpen()) return false;
	while (line_scanner2.getNextLine(line))
	{
		if (AAPU::linePrefix(line, string("#"))) 
			processComment(line_scanner2, line);

		else if (AAPU::lineIsEmpty(line)) continue;

		// FIXIT! is there any need to store this flag?
		else if (AAPU::linePrefix(line, string(":FULLY-SPECIFIED"))) continue;

		else if (AAPU::linePrefix(line, string(":DEGREES")))
			AMC_angles_are_degrees = true;

		else if (AAPU::lineIsSingleInt(line, frame)) continue;
		
		else
		{
			string name, rest;
			AAPU::splitLine(line, name, rest);
			list<float> values;
			AAPU::parseFloats(rest, values);

			Vector3D p;
			Vector3D a;
			short bone_id = skeleton->boneIdFromName(name);
			float linedata[6] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
			list<float>::iterator iter = values.begin();
			int i=0;
			while (iter != values.end())
			{
				linedata[i] = (*iter); i++; iter++;
			}
				
			skeleton->interpretAMCData(bone_id, linedata, p, a);

			if (AMC_angles_are_degrees)
			{
				a.pitch = deg2rad(a.pitch);
				a.yaw = deg2rad(a.yaw);
				a.roll = deg2rad(a.roll);
			}

			data.element(frame-1, bone_id*6)   = p.x;
			data.element(frame-1, bone_id*6+1) = p.y;
			data.element(frame-1, bone_id*6+2) = p.z;
			data.element(frame-1, bone_id*6+3) = a.pitch;
			data.element(frame-1, bone_id*6+4) = a.yaw;
			data.element(frame-1, bone_id*6+5) = a.roll;
		}
	}
	line_scanner2.close();
	
	motion->convertData(data, skeleton);

	return motion;
}

bool AMC_Reader::processComment(LineScanner& line_scanner, string& line)
{
	if (AAPU::linePrefix(line, string("# Documentation:"))) 
	{
		AAPU::stripPrefix(line, string("# Documentation:"));
		motion->addDocumentation(line);
	}
	else if (AAPU::linePrefix(line, string("# Source:"))) 
	{
		AAPU::stripPrefix(line, string("# Source:"));
		motion->setSource(line);
	}
	else if (AAPU::linePrefix(line, string("# Framerate: "))) 
	{
		AAPU::stripPrefix(line, string("# Framerate: "));
		motion->setFrameRate((float)atof(line.c_str()));
	}
	return true;
}
