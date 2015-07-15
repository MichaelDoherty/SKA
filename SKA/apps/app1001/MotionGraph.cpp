//-----------------------------------------------------------------------------
// app1001 - Builds with SKA Version 3.1 - Sept 01, 2012 - Michael Doherty
//-----------------------------------------------------------------------------
// MotionGraph.cpp
// Based on MotionGraph and Connector classes developed by COMP 259 students, fall 2014

// SKA configuration
#include <Core/SystemConfiguration.h>
#include <Core/SystemLog.h>
#include <Core/Utilities.h>
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>
using namespace std;
// SKA modules
#include <DataManagement/DataManager.h>
#include <Math/Quaternion.h>
#include "AppConfig.h"
#include "MotionGraph.h"
#include "BVH2Quaternion.h"
#include <boost\timer.hpp>


MotionGraph::MotionGraph(MotionDataSpecification& motion_data_specs)
{	
	buildMotionGraph(motion_data_specs);
}

void MotionGraph::findTransitions(string& from_seqID, int from_frame, vector<Transition>& transitions, int jump_barrier)
{
	for (unsigned long g=0; g<graph.size(); g++)
	{
		for (unsigned long i = 0; i < graph[g].transitions.size(); i++)
		{
			if ((graph[g].from_seq_ID == from_seqID) && 
				(graph[g].transitions[i].from_frame > from_frame+jump_barrier))
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
	float m_dist = 12.0;
	size_t o_lim = 30;
	size_t d_lim = 5;

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
			computeTransitions(sequences[i], sequences[j], ts.transitions, m_dist, o_lim, d_lim);
		}
		graph.push_back(ts);
	}
}

static string verifyQuaternionFile(string& bvh_filename, string& quat_filename, int joint_count)
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

		convertBVH2Quaternion(bvh_fullfilepath, string(quat_filepath), joint_count);

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
	string line;
	int joint_count = 0;

	string bvh_filepath = data_manager.findFile(bvh_filename.c_str());

	// Get Joint count from BVH file
	ifstream jointsearchdata(bvh_filepath.c_str());
	if (!jointsearchdata)
	{
		stringstream sss;
		sss << "MotionGraph::fileReader cannot open file " << bvh_filename;
		throw AppException(sss.str().c_str());
	}
	while (!jointsearchdata.eof())
	{
		getline(jointsearchdata, line);
		if (line.find("JOINT") != std::string::npos)
		{
			joint_count++;
		}
	}

	string quat_filepath = verifyQuaternionFile(bvh_filename, quat_filename, joint_count);
	
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
	
	Frame tmp_frame;
	Quaternion tmp_quat;

	while (!data.eof())
	{
		getline(data, line);
		data >> tmp_frame.root_position.x;
		data >> tmp_frame.root_position.y;
		data >> tmp_frame.root_position.z;

		for (int j = 0; j < joint_count; j++)
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
	int primary_frame_index;
	string primary_frame_sequence;
	int secondary_frame_index;
	string secondary_frame_sequence;
	float distance;

	bool operator < (const CandidateTransition& str) const{
		return(distance < str.distance);
	}
};



void MotionGraph::computeTransitions(Sequence& motion1, Sequence& motion2, vector<Transition>& result, float transition_max_distance, int transition_frame_outter_limit, int transition_frame_difference_limit)
{
	// Calculate distances between each pair of frames from the two motions
	// Upper theshold for initial threshold culling -> transition_max_distance
	logout << "MotionGraph::findTransitions starting" << endl;
	
	boost::timer myTimer; //remove for final revision
	vector<CandidateTransition> candidate_transitions; //holder for the transitions
	vector<FrameHolder> vh; // temporary frame holder
	FrameHolder temp_holder; //FrameHolder is necessary so we can join all frame information from any amount of motion sequences
	//then we can allow for transitions to search for frames within their own sequence and other sequences
	
	// FUTUREWORK (150715) - computeTransitions should allow dynamic number of frame sequences instead of hardcoded 2
	// fill in the FrameHolder with all the frames from both sequence 1 and sequence 2
	for (unsigned int i = 0; i < motion1.frames.size(); i++){
		temp_holder.current_seq_id = motion1.seq_ID;
		temp_holder.current_frame = motion1.frames[i];
		temp_holder.original_seq_size = motion1.frames.size();
		temp_holder.original_seq_frame_location = i;
		vh.push_back(temp_holder);
	}
	for (unsigned int i = 0; i < motion2.frames.size(); i++){
		temp_holder.current_seq_id = motion2.seq_ID;
		temp_holder.current_frame = motion2.frames[i];
		temp_holder.original_seq_size = motion1.frames.size();
		temp_holder.original_seq_frame_location = i;
		vh.push_back(temp_holder);
	}

	//loop through every frame twice
	//so that we can compare every frame to every other frame
	for (unsigned int i = 0; i < vh.size(); i++){
		for (unsigned int j = 0; j < vh.size(); j++){
			float distance = 0;
			
			// if joints are differet - if skeletons don't align -> throw exception
			if (vh[i].current_frame.joints.size() != vh[j].current_frame.joints.size())
			{
				stringstream ss;
				ss << "Error in MotionGraph::findTransitions. Different number of joints. " 
					<< vh[i].current_seq_id << " frame " << i << " to " << vh[i].current_seq_id << " frame " << j;
				throw AppException(ss.str().c_str());
			}

			// distance is the sum of the quaterian differences of each joint
			for (unsigned short k = 0; k < vh[i].current_frame.joints.size(); k++) 
			{
				Quaternion diffq = vh[i].current_frame.joints[k] - vh[j].current_frame.joints[k];
				distance += diffq.magnitude();
			}

			// store this pair as a candidate transition if it meets the criteria
			if (abs(distance) < transition_max_distance && 
				distance != 0 && 
				vh[i].original_seq_frame_location < vh[i].original_seq_size - transition_frame_outter_limit && 
				vh[j].original_seq_frame_location < vh[j].original_seq_size - transition_frame_outter_limit && 
				vh[i].original_seq_frame_location > transition_frame_outter_limit && 
				vh[j].original_seq_frame_location > transition_frame_outter_limit)
			{

				CandidateTransition transition;
				transition.primary_frame_index = i;
				transition.primary_frame_sequence = vh[i].current_seq_id;
				transition.secondary_frame_index = j;
				transition.secondary_frame_sequence = vh[j].current_seq_id;
				transition.distance = distance;

				// dont add frame to candidate vector if it falls within the transition_frame_difference_limit range of all other frames
				bool ignoreTransition = false;
				for (unsigned int m = 0; m < candidate_transitions.size(); m++)
				{
					if(transition.primary_frame_index < candidate_transitions[m].primary_frame_index + transition_frame_difference_limit &&
						transition.primary_frame_index > candidate_transitions[m].primary_frame_index - transition_frame_difference_limit &&
						transition.secondary_frame_index < candidate_transitions[m].secondary_frame_index + transition_frame_difference_limit &&
						transition.secondary_frame_index < candidate_transitions[m].secondary_frame_index - transition_frame_difference_limit)
					{
						ignoreTransition = true;
					}
				}
				if (!ignoreTransition){
					candidate_transitions.push_back(transition);
				}
			}

		}
	}


	// sort the candidate transitions in order of increasing distance
	std::sort(candidate_transitions.begin(),candidate_transitions.end());

	
	logout << "MotionGraph::findTransitions found " << candidate_transitions.size() << " transitions " << endl;

	//return the candidates as results
	for (unsigned int i = 0; i < candidate_transitions.size(); i++)
	{
		Transition t;
		t.from_seqID = candidate_transitions[i].primary_frame_sequence;
		t.from_frame = candidate_transitions[i].primary_frame_index;
		t.to_seqID = candidate_transitions[i].secondary_frame_sequence;
		t.to_frame = candidate_transitions[i].secondary_frame_index;
		t.distance = candidate_transitions[i].distance;
		result.push_back(t);
	}

	logout << myTimer.elapsed() << endl;
	logout << "MotionGraph::findTransitions finished" << endl;
}
