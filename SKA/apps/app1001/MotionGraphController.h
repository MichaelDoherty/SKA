//-----------------------------------------------------------------------------
// app1001 - Builds with SKA Version 3.1 - Sept 01, 2012 - Michael Doherty
//-----------------------------------------------------------------------------
// MotionGraphController.h
// Based on MotionGraphController class developed by COMP 259 students, fall 2014.

#ifndef MOTIONGRAPHCONTROLLER_DOT_H
#define MOTIONGRAPHCONTROLLER_DOT_H
#include <Core/SystemConfiguration.h>
#include <Animation/MotionController.h>
#include <Animation/MotionSequence.h>
#include <vector>
#include <map>
#include "AppConfig.h"
#include "MotionGraph.h"
#include <DataManagement/DataManager.h>
#include <DataManagement/DataManagementException.h>
#include <Animation/AnimationException.h>
using namespace std;

// FUTUREWORK (150625)
// 1) Add transfer of root position and orientation across transitions.
// 3) Blending across transitions.

class MotionGraphController : public MotionController
{
public:
	MotionGraphController(MotionGraph* _motion_graph,
		MotionDataSpecification& _motion_data_specs,
		float _character_size_scale);

	~MotionGraphController();

//---------- Runtime Access Methods ---------------
	// required by inheritance from MotionController
	virtual bool isValidChannel(CHANNEL_ID _channel, float _time);
	virtual float getValue(CHANNEL_ID _channel, float _time);

//---------- Internal state of the MotionGraphController ---------------
	// public to allow for display and logging
	struct State {
		string active_seqID;			// current sequence
		long active_frame;				// current frame number
		long transition_trigger_frame;	// what frame number is the transition
		string transition_seqID;		// sequence to transition to
		long transition_frame;			// what frame to start after transition
		float current_time;				// time of last internal state update
		float frame_zero_time;			// time that the current sequence effectively began
	};

//---------- Runtime Debugging Methods ---------------
	State getState() { return status; }

private:

	MotionGraph* motion_graph;

	// loaded motion sequences indexed by sequence ID
	map<string, MotionSequence*> motion_sequence_map;

	// status of the transition controller
	State status;	

	float frame_rate;				// frames per second of the source motion sequences
	float character_size_scale;		// the amount we need to scale the bones of the character

//---------- Internal Control Logic Methods ---------------

	// if time has advanced, update internal state
	void update(float _time);

	// update current position in current motion sequence
	void computeCurrentFrame();
	
	void setupNextTransition();

	MotionSequence* lookupMotionSequenceByID(string ID);
	
//---------- Setup Methods ---------------

	// reads all the motion sequences, stores in motion_sequence_map
	void readInMotionSequences(MotionDataSpecification& motion_data_specs);

//---------- Debugging Methods ---------------

	// log status
	void printStatus();
};
#endif
