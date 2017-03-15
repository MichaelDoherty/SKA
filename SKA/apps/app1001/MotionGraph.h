//-----------------------------------------------------------------------------
// app1001 - Builds with SKA Version 4.0
//-----------------------------------------------------------------------------
// MotionGraph.h
// Based on MotionGraph and Connector classes developed by COMP 259 students, fall 2014.
//-----------------------------------------------------------------------------

#ifndef MOTIONGRAPH_DOT_H
#define MOTIONGRAPH_DOT_H
// SKA configuration
#include <Core/SystemConfiguration.h>
#include <vector>
using namespace std;
#include "AnimationControl.h"

class MotionGraph
{
public:

	MotionGraph(MotionDataSpecification& motion_data_specs);

	// FUTUREWORK (150618) - optimize the graph storage for faster retrieval
	//                       of queries through findTransitions()
	struct Transition
	{
		string from_seqID;
		int from_frame;
		string to_seqID;
		int to_frame;
		float distance;
	};

	struct TransitionSet
	{
		string from_seq_ID;
		vector<Transition> transitions;
	};

	vector<TransitionSet> graph;

	// find all transitions from the given motion sequence at some frame later than the given frame
	void findTransitions(string& from_seqID, int from_frame, vector<Transition>& transitions);

private:

	struct Frame
	{
		Vector3D root_position;
		std::vector<Quaternion> joints;
	};

	struct Sequence
	{
		vector<Frame> frames;
		string seq_ID;
		string source_filename;
		string source_full_pathname;
	};

	void buildMotionGraph(MotionDataSpecification& motion_data_specs);

	Sequence fileReader(MotionDataSpecification& motion_data_specs, short index);

	void computeTransitions(Sequence& motion1, Sequence& motion2, vector<Transition>& result);
};

#endif
