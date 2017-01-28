//-----------------------------------------------------------------------------
// app1001 - Builds with SKA Version 3.1 - Sept 01, 2012 - Michael Doherty
//-----------------------------------------------------------------------------
// MotionGraphController.cpp
// Based on MotionGraphController class developed by COMP 259 students, fall 2014.

#include <Core/SystemConfiguration.h>
#include <Core/SystemLog.h>
#include "MotionGraphController.h"

MotionGraphController::MotionGraphController(MotionGraph* _motion_graph,
	MotionDataSpecification& _motion_data_specs,
	float _character_size_scale)
: frame_rate(120.0f), character_size_scale(_character_size_scale)
{
	readInMotionSequences(_motion_data_specs);
	motion_graph = _motion_graph;

	// force a transition to start of first sequence
	string initial_seqID = _motion_data_specs.getSeqID(0); 
	status.active_seqID = initial_seqID;
	status.active_frame = 0;
	status.active_frame = LONG_MAX; 
	status.transition_trigger_frame = 0;
	status.transition_frame = 0;
	status.transition_seqID = initial_seqID;
	status.current_time = -1.0f;
	status.frame_zero_time = 0.0f;
}

MotionGraphController::~MotionGraphController()
{
	map<string, MotionSequence*>::iterator it;
	for (it = motion_sequence_map.begin(); it != motion_sequence_map.end(); it++)
		delete it->second; 
}

//---------- Runtime Access Methods ---------------

bool MotionGraphController::isValidChannel(CHANNEL_ID _channel, float _time)
{
	// update internal state (if time has passed since last external access)
	update(_time);

	MotionSequence *motion_sequence = lookupMotionSequenceByID(status.active_seqID);
	if (motion_sequence == NULL)
	{
		stringstream ss;
		ss << "MotionGraphController::isValidChannel: MotionGraphController has no attached MotionSequence for " << status.active_seqID;
		logout << ss.str();
		throw AppException(ss.str().c_str());
	}
	return motion_sequence->isValidChannel(_channel);
}

float MotionGraphController::getValue(CHANNEL_ID _channel, float _time)
{
	// update internal state (if time has passed since last external access)
	update(_time);

	MotionSequence *motion_sequence = lookupMotionSequenceByID(status.active_seqID);
	if (motion_sequence == NULL)
	{
		stringstream ss;
		ss << "MotionGraphController::getValue: MotionGraphController has no attached MotionSequence for " << status.active_seqID;
		logout << ss.str() << endl;
		throw AppException(ss.str().c_str());
	}
	if (!isValidChannel(_channel, _time))
	{
		stringstream ss;
		ss << "MotionSequenceController received request for invalid channel bone: " 
			<< _channel.bone_id << " dof: " << _channel.channel_type;
		throw AppException(ss.str().c_str());
	}
	return motion_sequence->getValue(_channel, status.active_frame);
}

//---------- Internal Control Logic Methods ---------------

void MotionGraphController::update(float _time)
{
	// if time has not advanced, there's nothing to do
	if ((_time-status.current_time) < 0.0001f) return;

	// store the current time
	status.current_time = _time;

	// determine position in active motion sequence
	computeCurrentFrame();

	// check for transitions to a new motion sequence
	
	if (status.active_frame >= status.transition_trigger_frame)
	{
		// make the transition
		status.active_seqID = status.transition_seqID;
		status.active_frame = status.transition_frame;
		status.frame_zero_time = status.current_time - status.active_frame/frame_rate;
		computeCurrentFrame();
		
		// set up next transition
		setupNextTransition();
	}
}

void MotionGraphController::setupNextTransition()
{
	// traverse the motion graph and find all transitions from remaining part of current sequence
	vector<MotionGraph::Transition> candidate_transitions;	
	motion_graph->findTransitions(status.active_seqID, status.active_frame, candidate_transitions);

	// if nothing else is available, next transition is loop back from end of sequence
	if (candidate_transitions.size() < 1) 
	{
		status.transition_seqID = status.active_seqID;
		status.transition_frame = 0;
		status.transition_trigger_frame = lookupMotionSequenceByID(status.active_seqID)->numFrames() - 1;
		return;
	}

	// pick a random transition
	unsigned int choice = rand() % candidate_transitions.size();

	// enable the selected transition
	status.transition_seqID = candidate_transitions[choice].to_seqID;
	status.transition_frame = candidate_transitions[choice].to_frame;
	status.transition_trigger_frame = candidate_transitions[choice].from_frame;
}


void MotionGraphController::computeCurrentFrame()
{
	// convert from application time to frame number in active sequence
	status.active_frame = long((status.current_time - status.frame_zero_time) * frame_rate);
}

MotionSequence* MotionGraphController::lookupMotionSequenceByID(string ID)
{
	map<string, MotionSequence*>::iterator iter =  motion_sequence_map.find(ID);
	if (iter == motion_sequence_map.end()) return NULL;
	return iter->second;
}

//---------- Setup Methods ---------------

void MotionGraphController::readInMotionSequences(MotionDataSpecification& motion_data_specs)
{
	for (unsigned short i = 0; i<motion_data_specs.size(); i++)
	{
		string current_file = motion_data_specs.getBvhFilename(i);
		cout << "MotionGraphController reading " << current_file << endl;
		logout << "MotionGraphController reading "  << current_file << endl;
		
		char* BVH_filename = NULL;
		string character_BVH2(current_file);
		try
		{
			BVH_filename = data_manager.findFile(character_BVH2.c_str());
			if (BVH_filename == NULL)
			{
				stringstream ss;
				ss << "MotionGraphController::readInMotionSequences: Unable to find character BVH file <" << character_BVH2 << ">. Aborting load.";
				logout << ss.str() << endl;
				throw AppException(ss.str().c_str());
			}
			pair<Skeleton*, MotionSequence*> read_result;
			try
			{
				read_result = data_manager.readBVH(BVH_filename);
			}
			catch (const DataManagementException& dme)
			{
				stringstream ss;
				ss << "MotionGraphController::readInMotionSequences: Aborting due to DataManager expection: " << dme.msg;
				logout << ss.str();
				throw AppException(ss.str().c_str());
			}
			
			// throw away the skeleton
			delete read_result.first;
			MotionSequence * ms = read_result.second;
			
			string seqID = motion_data_specs.getSeqID(i);
			char* tmp = new char[strlen(seqID.c_str())+1];
			strcpy(tmp, seqID.c_str());
			ms->setId(tmp);
			delete [] tmp;

			//scale the character position in the motion sequence
			ms->scaleChannel(CHANNEL_ID(0, CT_TX), character_size_scale);
			ms->scaleChannel(CHANNEL_ID(0, CT_TY), character_size_scale);
			ms->scaleChannel(CHANNEL_ID(0, CT_TZ), character_size_scale);
			
			motion_sequence_map.insert(pair<string, MotionSequence*>(seqID, ms)); 
		}
		catch (BasicException& e)
		{
			stringstream ss;
			ss << "MotionGraphController::readInMotionSequences: Aborting due to BasicException: " << e.msg;
			logout << ss.str();
			throw AppException(ss.str().c_str());
		}
	}
	logout << "the number of motion sequences is : " << motion_sequence_map.size() << endl;
	logout << "... MotionGraphController::readInMotionSequences finished" << endl;
}

//---------- Debugging Methods ---------------

void MotionGraphController::printStatus()
{
	logout << endl << "MotionGraphController::printStatus" << endl;
	logout << "current_time: " << status.current_time << endl;
	logout << "status.active_seqID: " << status.active_seqID << endl;
	logout << "status.active_frame: " << status.active_frame << endl;
	logout << "status.transition_trigger_frame: " << status.transition_trigger_frame << endl;
	logout << "status.transition_seqID: " << status.transition_seqID << endl;
	logout << "status.transition_frame: " << status.transition_frame << endl;
}
