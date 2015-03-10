#include <Core/SystemConfiguration.h>
#include <Core/SystemLog.h>
#include "MotionGraphController.h"
#include "Connector.h"

MotionGraphController::MotionGraphController(MotionGraph &input)
: last_transition_time(0), last_transition_frame(0), frame_rate(120.0f), character_size_scale(0.2f)
{
	g = input;
	//Connector(g.allFrames.at(0), g.allFrames.at(1));
	logout << "initializing Motion Graph Controller \n Reading all frames to test first \n then Checking for neighbors \n" << endl;
	// pretty much tests to see if all frames are readable in the graph;
	readAllFrames();
	//read in the motion sequences
	readInMotionSequences();
	//read all the ids in the vector of MsVNames
	readAllSequenceIDs();

	/* test code*/
	status.FrameNumber = 0;
	status.SeqID = "swing5.bvh";
	status.isTransitioning = true;
	status.TransitionToSeqId = "swing5.bvh";
	MotionSequence *MS;
	//MotionSequence *MS2;
	//MotionSequence *MS3;
	//MotionSequence *MS4;
	MS = returnMotionSequenceContainerFromID(status.SeqID).MS;
	status.FrameNumberTransition = MS->numFrames();
	status.FrameNumberTransitionTo = 0;

}

MotionGraphController::~MotionGraphController()
{
	//delete all the motion sequences;
	for (unsigned long i = 0; i < MsVector.size(); i++)
	{
		delete MsVector.at(i).MS;
	}

}

bool MotionGraphController::timeToTransition(float time)
{// need to find out how to figure out when the time matches with the frame number. 
  //MotionSequence *MS;
  //MS = returnMotionSequenceContainerFromID(status.SeqID).MS;
	int currentFrame = computeCurrentFrame(time);
	if (currentFrame >= status.FrameNumberTransition)
		return(true);
	else
		return(false);
}

int MotionGraphController::computeMotionSequenceFrame(MotionSequence *motion_sequence, float _time)
{
	float duration = motion_sequence->getDuration();
	long cycles = long(_time / duration);

	float sequence_time = _time - duration*cycles;
	if (sequence_time > duration) sequence_time = 0.0f;

	int frame = int(motion_sequence->numFrames()*sequence_time / duration);
	return(frame);
}

long MotionGraphController::computeCurrentFrame(float _time)
{
	//determine how long we have been playing the current motion
	float time_in_sequence = _time - last_transition_time;
	//determine how many frames have been played in the current motion
	long frames_in_sequence = long(time_in_sequence * frame_rate);
	//offset frames played by the start frame in the current motion
	long current_frame = frames_in_sequence + last_transition_frame;
	//logout << "last_transition_time " << last_transition_time << endl;
	//logout << "last_transition_frame " << last_transition_frame << endl;
	//logout << "COMPUTE CURRENT FRAME" << current_frame << endl;
	return(current_frame);
}

bool MotionGraphController::isValidChannel(CHANNEL_ID _channel, float _time)
{
	MotionSequence *motion_sequence = returnMotionSequenceContainerFromID(status.SeqID).MS;
	if (motion_sequence == NULL)
	{
		throw AnimationException("MotionGraphController has no attached MotionSequence");
		return false;
	}
	return motion_sequence->isValidChannel(_channel);
}

void MotionGraphController::iterateStatus()
{
	//transitions to follow
	if (path.size() > 0)
	{
		status.isTransitioning = true;
		// take the first one off the list;
		vertexTargets temp = path.front();
		path.pop_front();
		//first seq
		status.SeqID = temp.SeqID;
		//first seq framenumber where we transition 
		status.FrameNumberTransition = temp.FrameNumber;
		//what we are transitioning to
		status.TransitionToSeqId = temp.SeqID2;
		//what frame we are transitioning to on the second seq;
		status.FrameNumberTransitionTo = temp.FrameNumber2;
	}
	//let it play out
	else if (status.isTransitioning==true)
	{
		status.SeqID = status.TransitionToSeqId;
		status.FrameNumber = status.FrameNumberTransitionTo;
		status.FrameNumberTransition = returnMotionSequenceContainerFromID(status.SeqID).MS->numFrames();
		
		status.isTransitioning = false;
		printStatus();
	
	}// none left so repeat
	else
	{
			path = pathBackup;
		iterateStatus();
		printStatus();
		return;
		//status.isTransitioning = true;
		// the last transition is now the new seqId aka what is playing
		status.SeqID = status.TransitionToSeqId;
		//	MotionSequence *motion_sequence = returnMotionSequenceContainerFromID(status.SeqID).MS;
		//where to transition on the last frame
		//status.FrameNumberTransition = motion_sequence->numFrames();
		/// set transition to to fame 0 of the same motion sequence
		//status.FrameNumberTransitionTo = 0;
	}
	//iterate the motio

	



}

float MotionGraphController::getValue(CHANNEL_ID _channel, float _time){
	//if it is not transitioning but its time to loop
	if (!status.isTransitioning &&  timeToTransition(_time))
	{
		//iterateStatus();
		//int frame3 = computeCurrentFrame(_time);
		MotionSequence *motion_sequence = returnMotionSequenceContainerFromID(status.SeqID).MS;
		if (motion_sequence == NULL)
			throw AnimationException("MotionSequenceController has no attached MotionSequence");

		if (!isValidChannel(_channel, _time))
		{
			string s = string("MotionSequenceController received request for invalid channel ")
				+ " bone: " + toString(_channel.bone_id) + " dof: " + toString(_channel.channel_type);
			throw AnimationException(s.c_str());
		}

//		int frame3 = computeCurrentFrame(_time);
		//set this for currentFrameCalculations
		last_transition_time = _time;
		last_transition_frame = status.FrameNumberTransitionTo;
		// set the frame number to the frame we transition to.
		status.FrameNumber = status.FrameNumberTransitionTo;

	//	int frame3 = computeCurrentFrame(_time);
		float value = motion_sequence->getValue(_channel, computeCurrentFrame(_time));

		//transition the graph using status information
		// only use when we have matching names of Motion sequences are the same as the filenames of the frames on the graph
		//transitionGraph();

		//update the status
		iterateStatus();
		return(value);

	}
	else if (status.isTransitioning &&  timeToTransition(_time))
	{
		MotionSequence *motion_sequence = returnMotionSequenceContainerFromID(status.TransitionToSeqId).MS;
		if (motion_sequence == NULL)
			throw AnimationException("MotionSequenceController has no attached MotionSequence");

		if (!isValidChannel(_channel, _time))
		{
			string s = string("MotionSequenceController received request for invalid channel ")
				+ " bone: " + toString(_channel.bone_id) + " dof: " + toString(_channel.channel_type);
			throw AnimationException(s.c_str());
		}
		//set this for currentFrameCalculations
		last_transition_time = _time;
		last_transition_frame = status.FrameNumberTransitionTo;
		// set the frame number to the frame we transition to.
		status.FrameNumber = status.FrameNumberTransitionTo;

		//int frame3 = computeCurrentFrame(_time);
		float value = motion_sequence->getValue(_channel, computeCurrentFrame(_time));

		//transition the graph using status information
		// only use when we have matching names of Motion sequences are the same as the filenames of the frames on the graph
		//transitionGraph();

		//update the status
		iterateStatus();
		return(value);
	}
	//not at right time to transition
	else{
		MotionSequence *motion_sequence = returnMotionSequenceContainerFromID(status.SeqID).MS;

		if (motion_sequence == NULL)
			throw AnimationException("MotionSequenceController has no attached MotionSequence");

		if (!isValidChannel(_channel, _time))
		{
			string s = string("MotionSequenceController received request for invalid channel ")
				+ " bone: " + toString(_channel.bone_id) + " dof: " + toString(_channel.channel_type);
			throw AnimationException(s.c_str());
		}
		int frame3 = computeCurrentFrame(_time);
		float value = motion_sequence->getValue(_channel, computeCurrentFrame(_time));
		if (frame3 > status.FrameNumber)
		{

			printStatus();
		}

		//update status
		status.FrameNumber = frame3;

		// only use when we have matching names of Motion sequences are the same as the filenames of the frames on the graph
		//iterate graph
		//iterateMotionGraph();
	
		return(value);
	}
}

MotionGraph::DirectedGraph::vertex_descriptor MotionGraphController::FindVertex(string sequenceID, int frameNumber)
{
	pair<MotionGraph::vertex_iter, MotionGraph::vertex_iter> vp;
	int i;
	for (vp = vertices(g.dgraph), i = 0; vp.first != vp.second; ++vp.first, i++)
	{
		MotionGraph::DirectedGraph::vertex_descriptor v = *(vp.first);
		if (g.dgraph[v].frame_data.fileName == sequenceID)
		{
			if (g.dgraph[v].frame_data.frame_number == frameNumber)
			{
				return(v);
			}
		}
	}
	logout << "could not find vertex" << endl;
	return 0;
}

void MotionGraphController::iterateMotionGraph()
{

	std::pair<MotionGraph::neighbor_iterator, MotionGraph::neighbor_iterator> neighbors =
		boost::adjacent_vertices(CurrentVertex, g.dgraph);
	for (; neighbors.first != neighbors.second; ++neighbors.first)
	{
		//if the neighbor has the same name as the vertex we are currently on and has the same frame number as the vertex we are on;
		if (g.dgraph[*neighbors.first].frame_data.fileName == status.SeqID&&g.dgraph[*neighbors.first].frame_data.frame_number == status.FrameNumber)
		{
			// set it to the neighbor with the correct filename
			CurrentVertex = *neighbors.first;
			return;
		}
	}

}

void MotionGraphController::transitionGraph()
{
	//uses status info

	std::pair<MotionGraph::neighbor_iterator, MotionGraph::neighbor_iterator> neighbors =
		boost::adjacent_vertices(CurrentVertex, g.dgraph);
	for (; neighbors.first != neighbors.second; ++neighbors.first)
	{
		//if the neighbor has the same name as the vertex we are currently transitioning to and has the same frame number as the vertex we are transitioning to;
		if (g.dgraph[*neighbors.first].frame_data.fileName == status.TransitionToSeqId&&g.dgraph[*neighbors.first].frame_data.frame_number == status.FrameNumberTransitionTo)
		{
			// set it to the neighbor with the correct filename
			CurrentVertex = *neighbors.first;
			return;
		}
	}
}

bool MotionGraphController::isTransitionPoint(MotionGraph::DirectedGraph::vertex_descriptor m)
{
	// adjacency iterators or neighbors
	std::pair<MotionGraph::neighbor_iterator, MotionGraph::neighbor_iterator> neighbors =boost::adjacent_vertices(m, g.dgraph);
	//iterate through all neighbors
	int neighborCount = 0;
	for (; neighbors.first != neighbors.second; ++neighbors.first)
	{
		//	logout << "neighbors for  " << g.dgraph[m].frame_data.fileName << g.dgraph[m].frame_data.frame_number << " is" << g.dgraph[*neighbors.first].frame_data.fileName << g.dgraph[*neighbors.first].frame_data.frame_number << endl;
		neighborCount++;
	}
	// if it has 2 or more neighbors then that means it is a transition
	if (neighborCount >= 2)
	{
		logout << "IS TRANSITION POINT" << endl;
		return(true);
	}
	//logout << "not a transition point" << endl;
	return(false);
}

void  MotionGraphController::setPath(string startFileName, int startFrame ,list<vertexTargets> inputPath)
{
	
	path = inputPath;
	pathBackup = inputPath;
	iterateStatus();
	status.SeqID = startFileName;
	status.FrameNumber = startFrame;
	printStatus();

}

bool  MotionGraphController::updatePath(list<vertexTargets> inputPath)
{
	vertexTargets temp = inputPath.front();


	//first seq
// temp.frameNumber is the frame number we are transitioning on for the current seq
	//we also need to see if the path includes that we are playing 
	if (temp.FrameNumber <= status.FrameNumber||temp.SeqID!=status.SeqID)
	{

		return(false);
	}
	else
	{
		//set the path update variable. get value will update this after the next frame. 
		path=inputPath;
		pathBackup = inputPath;
		status.FrameNumberTransition = temp.FrameNumber;
		status.FrameNumberTransitionTo = temp.FrameNumber2;
		status.TransitionToSeqId = temp.SeqID2;
		//take the top off since we already updated it
		inputPath.pop_front();
		return(true);
	}
}
MotionGraphController::state MotionGraphController::getStatus()
{
	return(status);
}

list<MotionGraphController::vertexTargets> MotionGraphController::getPath()
{
	return(path);
}
void MotionGraphController::readInMotionSequences()
{
	logout << "reading motion Sequences" << endl;
	namespace fs = ::boost::filesystem;

	fs::path p(BVH_MOTION_FILE_PATHMOTIONS);
	if (!exists(p))    // does p actually exist?
		logout << "doesn't exist" << endl;
	fs::directory_iterator end_itr;

	// cycle through the directory
	for (fs::directory_iterator itr(p); itr != end_itr; ++itr)
	{
		// If it's not a directory, list it. If you want to list directories too, just remove this check.
		if (is_regular_file(itr->path())) {
			// assign current file name to current_file and echo it out to the console.
			string current_file = itr->path().string();
			current_file = itr->path().filename().string();

			logout << "Current File: " << current_file << endl;

			DataManager dataman;
			dataman.addFileSearchPath(BVH_MOTION_FILE_PATHMOTIONS);
			char* BVH_filename = NULL;
			string character_BVH2(current_file);
			try
			{
				BVH_filename = dataman.findFile(character_BVH2.c_str());
				if (BVH_filename == NULL)
				{
					logout << "MotionGraphController::readInMotionSequences: Unable to find character BVH file <" << character_BVH2 << ">. Aborting load." << endl;
					throw BasicException("ABORT");
				}
				pair<Skeleton*, MotionSequence*> read_result;
				try
				{
					read_result = data_manager.readBVH(BVH_filename);
				}
				catch (const DataManagementException& dme)
				{
					logout << "MotionGraphController::readInMotionSequences: Unable to load character data files. Aborting load." << endl;
					logout << "   Failure due to " << dme.msg << endl;
					throw BasicException("ABORT");
				}

				//Skeleton* skel = read_result.first;
				MotionSequence * ms = read_result.second;
				
				std::string x = current_file;
				char *y = new char[x.length() + 1];
				std::strcpy(y, x.c_str());
				//set the ID aka filename
				ms->setId(y);
				delete[] y;

				//scale each motion sequence once
				ms->scaleChannel(CHANNEL_ID(0, CT_TX), character_size_scale);
				ms->scaleChannel(CHANNEL_ID(0, CT_TY), character_size_scale);
				ms->scaleChannel(CHANNEL_ID(0, CT_TZ), character_size_scale);

				MotionSequenceContainer test;
				test.MS = ms;
				test.SeqID = current_file;
				MsVector.push_back(test);
				logout << "done loading:  "<<current_file << MsVector.size() << endl;
			}
			catch (BasicException& e) { logout << "EXCEPTION: " << e.msg << endl; }
		}
	}
	logout << "the size of the vector is : " << MsVector.size() << endl;
}

void MotionGraphController::readAllSequenceIDs()
{
	for (unsigned long i = 0; i < MsVector.size(); i++)
	{
		logout << "names " << i << " :" << MsVector.at(i).SeqID << endl;
	}
}

// 150201::DOHERTY - We shouldn't return copies of MotionSequenceContainers.
//                   This should return a reference or pointer.
MotionGraphController::MotionSequenceContainer MotionGraphController::returnMotionSequenceContainerFromID(string ID)
{
	for (unsigned long i = 0; i < MsVector.size(); i++)
	{
		if (MsVector.at(i).SeqID == ID)
			return MsVector.at(i);
	}
	// 150201::DOHERTY failsafe - just return the first one.
	return MsVector.at(0);
}

void MotionGraphController::readAllFrames()
{
	pair<MotionGraph::vertex_iter, MotionGraph::vertex_iter> vp;
	int jo = 0;
	for (vp = vertices(g.dgraph); vp.first != vp.second; vp.first++)
	{
		//logout << "FileName: " << g.dgraph[*vp.first].frame_data.fileName << endl;
		//logout << "Frame Number" << g.dgraph[*vp.first].frame_data.frame_number << endl;
		//logout << "Vertex Name" << g.dgraph[*vp.first].frame_data.vertexName << endl;
		//logout << jo << endl;
		//logout << endl;
		//logout << endl;
		testLinearOfMotionGraph(*vp.first);
		jo++;
		//	Sleep(2000);

	}
}

void MotionGraphController::printStatus()
{
	logout << endl << "MotionGraphController::printStatus" << endl;
	logout << "SeqID: " << status.SeqID << endl;
	logout << "FrameNumber: " << status.FrameNumber << endl;
	logout << "FrameNumberTransition: " << status.FrameNumberTransition << endl;
	logout << "TransitionToSeqId: " << status.TransitionToSeqId << endl;
	logout << "FrameNumberTransitionTo: " << status.FrameNumberTransitionTo << endl;
	logout << "isTransitioning: " << status.isTransitioning << endl;
	logout << "STATUS VARIABLE" << endl;
	logout << "SeqID: " << status.SeqID << endl;
	logout << "FrameNumber: " << status.FrameNumber << endl;
	logout << "FrameNumberTransition: " << status.FrameNumberTransition << endl;
	logout << "TransitionToSeqId: " << status.TransitionToSeqId << endl;
	logout << "FrameNumberTransitionTo: " << status.FrameNumberTransitionTo << endl;
	logout << "isTransitioning: " << status.isTransitioning << endl;
}

bool MotionGraphController::testLinearOfMotionGraph(MotionGraph::DirectedGraph::vertex_descriptor m)
{

	// adjacency iterators or neighbors
	std::pair<MotionGraph::neighbor_iterator, MotionGraph::neighbor_iterator> neighbors = boost::adjacent_vertices(m, g.dgraph);
	//iterate through all neighbors
	int neighborCount = 0;
	for (; neighbors.first != neighbors.second; ++neighbors.first)
	{
		//	logout << "neighbors for  " << g.dgraph[m].frame_data.fileName << g.dgraph[m].frame_data.frame_number << " is" << g.dgraph[*neighbors.first].frame_data.fileName << g.dgraph[*neighbors.first].frame_data.frame_number << endl;
		neighborCount++;

	}
	// no neighbors for end of each file
	if (neighborCount == 0)
	{
		//for no neighbors aka end of linear file
		logout << "No neighbors for  " << g.dgraph[m].frame_data.fileName << "frame number: " << g.dgraph[m].frame_data.frame_number << endl;
	}
	// if it has 2 or more neighbors then that means it is a transition
	if (neighborCount >= 2)
	{
		logout << "IS TRANSITION POINT" << endl;
		return(true);
	}
	//logout << "not a transition point" << endl;
	return(false);
}
