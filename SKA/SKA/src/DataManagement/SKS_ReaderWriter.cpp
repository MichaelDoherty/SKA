//-----------------------------------------------------------------------------
// SKS_ReaderWriter.cpp
//	 Reads and writes skeletons to/from SKA Skeleton Files (SKS)
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
#include <iostream> // test only
#include <fstream>
#include <string>
#include <vector>
using namespace std;
#include <DataManagement/SKS_ReaderWriter.h>
#include <DataManagement/FileSystem.h>
#include <Animation/Skeleton.h>

#define SKELETON_ID_TAG "skeleton_id"
#define DESCRIPTION_TAG "description"
#define SOURCE_TAG "source"
#define BONE_TAG "bone"
#define JOINT_TAG "joint"

static string ripNextToken(string& s)
{
	string first, rest;
	unsigned int i;
	for (i=0; i<s.length(); i++)
	{
		if ((s[i] == ' ') || (s[i] == '\n') || (s[i] == '\t')) break;
		first += s[i];
	}
	// need to consume ALL whitespace
	s.erase(0,i+1);
	return first;
}

Skeleton* SKS_ReaderWriter::readSKS(const char* inputFilename)
{
	ifstream file;
	file.open(inputFilename);
	if (!file) return NULL;
	Skeleton* skel = new Skeleton;
	while (true) {
		string line;
		getline(file, line);
		if (!file) break;
		if (line[0] == '#') continue;

		string tag = ripNextToken(line);
		if (tag == SKELETON_ID_TAG)
		{
			string id = ripNextToken(line);
			skel->setId(id.c_str());
		}
		else if (tag == DESCRIPTION_TAG) 
		{
			skel->addDocumentation(line.c_str());
		}
		else if (tag == SOURCE_TAG) 
		{
			skel->setSource(line.c_str());
		}
		else if (tag == BONE_TAG)
		{
			short index = atoi(ripNextToken(line).c_str());
			string name = ripNextToken(line).c_str();
			float length = float(atof(ripNextToken(line).c_str()));
			float dirx = float(atof(ripNextToken(line).c_str()));
			float diry = float(atof(ripNextToken(line).c_str()));
			float dirz = float(atof(ripNextToken(line).c_str()));
			skel->createBone(index, name.c_str());
			skel->setBoneLength(index, length);
			skel->setBoneDirection(index, dirx, diry, dirz);

			// When bone is created, the channel map needs to be updated.
			// See following functions in BVH_Reader_Local:
			// createSkeleton, configureChannels, set_SKA2BVH_channel_map

			// This indicates a flaw in the SKS format. 
			// There is no way to store the channel application order for a bone.

		}
		else if (tag == JOINT_TAG)
		{
			string parent = ripNextToken(line);
			string child = ripNextToken(line);
			skel->addConnection(parent.c_str(), child.c_str());
		}
	}
	file.close();
	skel->finalizeInitialization();
	return skel;
}

bool SKS_ReaderWriter::writeSKS(const char* outputFilename,
		Skeleton* skeleton,
		bool overwrite)
{
	if (!overwrite && FileSystem::fileExists(outputFilename)) return false;
	ofstream file;
	file.open(outputFilename);
	if (!file) return false;

	file << SKELETON_ID_TAG << " " << skeleton->getId() << endl;
	file << DESCRIPTION_TAG << " " << skeleton->getDocumentation() << endl;
	file << SOURCE_TAG << " " << skeleton->getSource() << endl;
	vector<pair<string,string> > joints;
	for (int b=0; b<skeleton->numBones(); b++)
	{
		Bone* bone = skeleton->getBone(b);
		Vector3D direction = bone->getDirection();
		file << BONE_TAG << " "
			<< bone->getID() << " "
			<< bone->getName() << " "	
			<< bone->getLength() << " "
			<< direction.pitch << " "					
			<< direction.yaw << " "
			<< direction.roll << endl;
		Bone* parent = bone->getParent();
		if (parent != NULL)
			joints.push_back(pair<string,string>(parent->getName(),bone->getName()));
	}
	for (unsigned short i=0; i<joints.size(); i++)
		file << JOINT_TAG << " "
		 	<< joints[i].first << " "
		 	<< joints[i].second << endl;

	file.close();
	return true;
}