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

#include <Core/SystemConfiguration.h>
#include <cstdlib>
#include <DataManagement/AMC_Reader.h>
#include <DataManagement/ParsingUtilities.h>
#include <Core/Array2D.h>
#include <Animation/Skeleton.h>
#include <Animation/MotionSequence.h>
	
MotionSequence* AMC_Reader::readAMC(const char* motionFilename, Skeleton* skeleton)
{
	string line;

	// read once, just to get the number of frames
	LineScanner line_scanner1(motionFilename);
	if (!line_scanner1.fileIsOpen()) return NULL;
	int frame_count = 0;
	while (line_scanner1.getNextLine(line))
	{
		int frame;
		if (ParsingUtilities::linePrefix(line, string("#"))) continue;
		if (ParsingUtilities::lineIsEmpty(line)) continue;
		if (ParsingUtilities::lineIsSingleInt(line, frame))
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
	if (!line_scanner2.fileIsOpen()) return NULL;
	while (line_scanner2.getNextLine(line))
	{
		if (ParsingUtilities::linePrefix(line, string("#"))) 
		{
			//processComment(line_scanner2, line);
			if (ParsingUtilities::linePrefix(line, string("# Documentation:"))) 
			{
				ParsingUtilities::stripPrefix(line, string("# Documentation:"));
				motion->addDocumentation(line.c_str());
			}
			else if (ParsingUtilities::linePrefix(line, string("# Source:"))) 
			{
				ParsingUtilities::stripPrefix(line, string("# Source:"));
				motion->setSource(line.c_str());
			}
			else if (ParsingUtilities::linePrefix(line, string("# Framerate: "))) 
			{
				ParsingUtilities::stripPrefix(line, string("# Framerate: "));
				motion->setFrameRate((float)atof(line.c_str()));
			}
		}
		else if (ParsingUtilities::lineIsEmpty(line)) continue;

		// Is it necessary to store this flag?
		else if (ParsingUtilities::linePrefix(line, string(":FULLY-SPECIFIED"))) continue;

		else if (ParsingUtilities::linePrefix(line, string(":DEGREES")))
			AMC_angles_are_degrees = true;

		else if (ParsingUtilities::lineIsSingleInt(line, frame)) continue;
		
		else
		{
			string name, rest;
			ParsingUtilities::splitLine(line, name, rest);
			list<float> values;
			ParsingUtilities::parseFloats(rest, values);

			Vector3D p;
			Vector3D a;
			short bone_id = skeleton->boneIdFromName(name.c_str());
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
	
	// parsing is done
	// Convert the data into a MotionSequence

	vector<CHANNEL_ID> channel_ids;
	for (short b=0; b<skeleton->numBones(); b++)
	{
		for (short channel_type=0; channel_type<6; channel_type++)
		{
			CHANNEL_ID c(b, CHANNEL_TYPE(channel_type));
			if (skeleton->isActiveChannel(b,channel_type)) //|| (force_angle_channels_active && (channel_type>=4)))
				channel_ids.push_back(c);
		}
	}
	
	int frames = data.getRows();
	motion->setNumFrames(frames);
	motion->setFrameRate(120);

	int chans = channel_ids.size();
	Array2D<float> data2; // data with "false" channels removed.
	data2.resize(frames, chans);

	CHANNEL_ID* cid = new CHANNEL_ID[chans];
	for (unsigned short c=0; c<chans; c++)	
	{
		cid[c] = channel_ids[c];
		memcpy(data2.getColumnPtr(c), data.getColumnPtr(cid[c].bone_id*6+cid[c].channel_type), frames*sizeof(float));
	}
	motion->bulkBuild(cid, chans, data2);
	delete cid;

	return motion;
}
