//-----------------------------------------------------------------------------
// app1001 - Builds with SKA Version 4.0
//-----------------------------------------------------------------------------
// MotionGraph.cpp
// Based on MotionGraph and Connector classes developed by COMP 259 students, fall 2014
//-----------------------------------------------------------------------------

// SKA configuration
#include <Core/SystemConfiguration.h>
#include <Core/SystemLog.h>
#include <Core/Utilities.h>
#include <string>
#include <iostream>
#include <sstream>
using namespace std;
// SKA modules
#include <DataManagement/DataManager.h>
#include <Math/Quaternion.h>
#include "AppConfig.h"
#include "MotionGraph.h"
#include "BVH2Quaternion.h"

MotionGraph::MotionGraph(MotionDataSpecification& motion_data_specs)
{
	buildMotionGraph(motion_data_specs);
}

void MotionGraph::findTransitions(string& from_seqID, int from_frame, vector<Transition>& transitions)
{
	for (unsigned long g=0; g<graph.size(); g++)
	{
		for (unsigned long i = 0; i < graph[g].transitions.size(); i++)
		{
			// FUTUREWORK (150626) - The +10 is to avoid jumping too soon.
			//                       It should be parameterized.
			if ((graph[g].from_seq_ID == from_seqID) &&
				(graph[g].transitions[i].from_frame > from_frame+10))
			{
				Transition t;
				t.from_seqID = graph[g].transitions[i].from_seqID;
				t.to_seqID = graph[g].transitions[i].to_seqID;
				t.from_frame = graph[g].transitions[i].from_frame;
				t.to_frame = graph[g].transitions[i].to_frame;
				transitions.push_back(t);
			}
		}
	}
}

void MotionGraph::buildMotionGraph(MotionDataSpecification& motion_data_specs)
{
	vector<Sequence> sequences;

	for (unsigned short i=0; i<motion_data_specs.size(); i++)
	{
		Sequence seq = fileReader(motion_data_specs, i);
		sequences.push_back(seq);
	}

	// find transitions between each pair of sequences
	// FUTUREWORK (150618) - does not currently allow for any transitions to self
	for (unsigned short i=0; i<motion_data_specs.size(); i++)
	{
		TransitionSet ts;
		for (unsigned short j=0; j<motion_data_specs.size(); j++)
		{
			if (i == j) continue;
			ts.from_seq_ID = motion_data_specs.getSeqID(i);
			computeTransitions(sequences[i], sequences[j], ts.transitions);
		}
		graph.push_back(ts);
	}
}

static string verifyQuaternionFile(string& bvh_filename, string& quat_filename)
{
	if (bvh_filename == quat_filename)
	{
		stringstream ss;
		ss << "MotionGraph::fileReader bvh file and quat file have the same name "
			<< quat_filename << " == " << bvh_filename;
		throw AppException(ss.str().c_str());
	}

	char* quat_filepath = data_manager.findFile(quat_filename.c_str());

	// if we can't find the quaternion file, attempt to generate it
	if (quat_filepath == NULL)
	{
		char* bvh_filepath = data_manager.findFile(bvh_filename.c_str());
		if (bvh_filepath == NULL)
		{
			stringstream ss;
			ss << "MotionGraph::fileReader cannot find file "
				<< quat_filename << " or " << bvh_filename;
			throw AppException(ss.str().c_str());
		}
		string bvh_fullfilepath = bvh_filepath;

		// extract the directory of the located bvh file
		char* bvh_dir = strClone(bvh_filepath);
		char* name_starts = strrchr(bvh_dir, '/');
		if (name_starts == NULL) bvh_dir[0] = '\n';
		else *(name_starts+1) = '\0';

		// attempt to create a new file in the same directory as the source BVH file.
		quat_filepath = new char[strlen(bvh_dir)+quat_filename.length()+20];
		sprintf(quat_filepath, "%s%s", bvh_dir, quat_filename.c_str());

		// be careful not to override the source BVH file.
		if (strcmp(bvh_filepath, quat_filepath) == 0)
		{
			sprintf(quat_filepath, "%sqq_%s", quat_filepath, quat_filename.c_str());
		}

		string qfp(quat_filepath);
		convertBVH2Quaternion(bvh_fullfilepath, qfp);

		// verify that the new file exists
		char* tmp = data_manager.findFile(quat_filepath);
		if (tmp == NULL)
		{
			stringstream ss;
			ss << "MotionGraph::fileReader cannot find generated quaternion file " << quat_filepath;
			throw AppException(ss.str().c_str());
		}
		delete [] tmp;
	}
	string filepath = quat_filepath;
	delete [] quat_filepath;
	return filepath;
}

// Read a quaternion format data file and store it as a Sequence
MotionGraph::Sequence MotionGraph::fileReader(MotionDataSpecification& motion_data_specs, short index)
{
	string seq_ID = motion_data_specs.getSeqID(index);
	string bvh_filename = motion_data_specs.getBvhFilename(index);
	string quat_filename = motion_data_specs.getQuatFilename(index);

	string quat_filepath = verifyQuaternionFile(bvh_filename, quat_filename);

	vector<Frame> single_sequence;
	Sequence sequence;
	sequence.seq_ID = seq_ID;
	sequence.source_filename = quat_filename;
	sequence.source_full_pathname = quat_filepath;

	cout << "MotionGraph::fileReader is opening: " << quat_filepath << endl;

	ifstream data(quat_filepath.c_str());
	if (!data)
	{
		stringstream ss;
		ss << "MotionGraph::fileReader cannot open file " << quat_filepath;
		throw AppException(ss.str().c_str());
	}
	string line;
	Frame tmp_frame;
	Quaternion tmp_quat;

	while (!data.eof())
	{
		getline(data, line);
		data >> tmp_frame.root_position.x;
		data >> tmp_frame.root_position.y;
		data >> tmp_frame.root_position.z;

		// FUTUREWORK (150626) - 20 joint count needs to be parameterized.
		//   needs to be coordinated with convertBVH2Quaternion()
		for (int j = 0; j < 20; j++)
		{
			data >> tmp_quat.w;
			data >> tmp_quat.z;
			data >> tmp_quat.x;
			data >> tmp_quat.y;
			tmp_frame.joints.push_back(tmp_quat);
		}

		single_sequence.push_back(tmp_frame);
		tmp_frame.joints.clear();
	}

	sequence.frames = single_sequence;
	return sequence;
}

struct CandidateTransition {
	int motion1_frame;
	int motion2_frame;
	float distance;
};

// FUTUREWORK (150618)
// This function would probably be more efficient if we used lists instead of vectors,
// since it is often erasing the first element in the vector/list.
void MotionGraph::computeTransitions(Sequence& motion1, Sequence& motion2, vector<Transition>& result)
{
	logout << "MotionGraph::findTransitions starting" << endl;

	vector<CandidateTransition> candidate_transitions;

	// Calculate distances between each pair of frames from the two motions

	// Upper theshold for initial threshold culling
	// FUTUREWORK (150618) - max_distance should be a parameter
	float max_distance = 12.0;

	// loop through all frame pairs <i,j>, where i is from motion 1 and j is from motion 2
	for (unsigned int i = 0; i < motion1.frames.size(); i += 1)
	{
		for (unsigned int j = 0; j < motion2.frames.size(); j += 1)
		{
			float distance = 0;

			if (motion1.frames[i].joints.size() != motion2.frames[j].joints.size())
			{
				stringstream ss;
				ss << "Error in MotionGraph::findTransitions. Different number of joints. "
					<< motion1.seq_ID << " frame " << i << " to " << motion2.seq_ID << " frame " << j;
				throw AppException(ss.str().c_str());
			}

			// distance is the sum of the quaterian differences of each joint
			for (unsigned short k = 0; k < motion1.frames[i].joints.size(); k++)
			{
				Quaternion diffq = motion1.frames[i].joints[k] - motion2.frames[j].joints[k];
				distance += diffq.magnitude();
			}

			// store this pair as a candidate transition if it meets the criteria
			// FUTUREWORK (150618) - 30 should be a parameter
			if (abs(distance) < max_distance &&
				distance != 0 &&
				j < motion2.frames.size() - 30 &&
				i < motion1.frames.size() - 30 &&
				i > 30 &&
				j > 30)
			{
				CandidateTransition transition;
				transition.motion1_frame = i;
				transition.motion2_frame = j;
				transition.distance = distance;
				candidate_transitions.push_back(transition);
			}
		}
	}

	// sort the candidate transitions in order of increasing distance

	vector<CandidateTransition> ordered_transitions;
	while (candidate_transitions.size() > 0)
	{
		int lowestCompValLocation = 0;
		for (unsigned int i = 1; i < candidate_transitions.size(); i++)
		{
			if (candidate_transitions[lowestCompValLocation].distance > candidate_transitions[i].distance)
				lowestCompValLocation = i;
		}
		ordered_transitions.push_back(candidate_transitions[lowestCompValLocation]);
		candidate_transitions.erase(candidate_transitions.begin() + lowestCompValLocation);
	}

	// select the best pairs from local sequences of pairs

	vector<CandidateTransition> selected_transitions;
	selected_transitions.push_back(ordered_transitions[0]);
	ordered_transitions.erase(ordered_transitions.begin());
	while (ordered_transitions.size() > 0)
	{
		bool addToList = true;
		for (unsigned int i = 0; i < selected_transitions.size(); i++)
		{
			// If the lowest comparison value in sortedComparisons frame 1 and 2 are within 10 frames
			// of any motion already stored then don't add to final list
			// FUTUREWORK (150618) - 5 should be a parameter
			if (ordered_transitions[0].motion1_frame < selected_transitions[i].motion1_frame + 5 &&
				ordered_transitions[0].motion1_frame > selected_transitions[i].motion1_frame - 5 &&
				ordered_transitions[0].motion2_frame < selected_transitions[i].motion2_frame + 5 &&
				ordered_transitions[0].motion2_frame < selected_transitions[i].motion2_frame - 5)
				addToList = false;
		}
		if (addToList)
			selected_transitions.push_back(ordered_transitions[0]);
		ordered_transitions.erase(ordered_transitions.begin());
	}

	logout << "MotionGraph::findTransitions found " << selected_transitions.size() << " transitions " << endl;

	for (unsigned int i = 0; i < selected_transitions.size(); i++)
	{
		Transition t;
		t.from_seqID = motion1.seq_ID;
		t.from_frame = selected_transitions[i].motion1_frame;
		t.to_seqID = motion2.seq_ID;
		t.to_frame = selected_transitions[i].motion2_frame;
		t.distance = selected_transitions[i].distance;
		result.push_back(t);
	}

	logout << "MotionGraph::findTransitions finished" << endl;
}
