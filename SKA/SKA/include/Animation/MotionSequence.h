//-----------------------------------------------------------------------------
// MotionSequence.h
//	 Stores motion data for a skeleton.
//   Motions are stored such that they can be indexed by <bone,DOF,frame>.
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
#ifndef MOTIONSEQUENCE_DOT_H
#define MOTIONSEQUENCE_DOT_H
#include "Core/SystemConfiguration.h"
#include <vector>
using namespace std;
#include "Math/Math.h"
#include "Core/Array2D.h"
#include "Animation/SkeletonDefinition.h"
#include "Animation/Channel.h"
#include "Signals/SignalSpec.h"
#include "Signals/Signals.h"
#include "Core/SystemLog.h"

class AnimSkeleton;
class ChannelMap;

class SKA_LIB_DECLSPEC MotionSequence
{
	friend ostream& operator<<(ostream& out, MotionSequence& ms);
	friend class FFTfilter;
	friend class DatabaseLoader;

private:
	string motion_id;
	string documentation;
	string source;

	ChannelMap* channel_index;
	Array2D<float> data;
	int num_frames;
	float frame_rate;  // frames / sec
	float duration;    // total time of sequence in seconds
	bool data_is_deltas;

	// FIXIT! This should be handled by a subclass
	/*
	bool has_generators;
	bool use_generators;
	map<CHANNEL_ID, vector<SignalSpec> > generator_specs;
	map<CHANNEL_ID, SignalGenerator*> generators;
	*/
public:

	MotionSequence();
	MotionSequence(const MotionSequence& rhs);
	virtual ~MotionSequence();

	void clear();

	bool analyzeSignals();
	/*
	float getGeneratedValue(CHANNEL_ID c, int frame);
	void printGenerators(ostream& out);
	bool hasGenerators() { return has_generators; }
	bool enableGenerators()
	{ 
		if (!has_generators) return false; 
		use_generators = true;
		return true;
	}
	void disableGenerators()
	{ 
		use_generators = false;
	}
	*/
	bool convertData(Array2D<float>& amc_data, SkeletonDefinition* skeleton);
	bool copyDataWithoutConversion(Array2D<float>& bvh_data);

	int numFrames()
	{
		return num_frames; 
	}

	int numChannels();

	short addChannel(CHANNEL_ID& channel);

	void buildChannelsFromSkeleton(SkeletonDefinition* skeleton);
	
	void adjustStorage();

	string getId() { return motion_id; }
	void setId(string& _id) { motion_id = _id; }

	void clearDocumentation() { documentation = string(""); }
	void addDocumentation(const string& d) { documentation += d; }
	string getDocumentation() { return documentation; }

	void setSource(const string& s) { source = s; }
	string getSource() { return source; }

	float getDuration() 
	{ 
		return duration; 
	}

	void setFrameRate(float f) 
	{ 
		frame_rate = f; 
		duration = num_frames/frame_rate;
	}

	void setNumFrames(short n);

	float getFrameRate() 
	{ 
		return frame_rate; 
	}

	bool isValidChannel(CHANNEL_ID& c);

	int getChannelIndex(CHANNEL_ID& c);
	
	// get the first num_channels channels
	// channels is assumed to be large enough to hold num_channels channels
	// returns number of channels actually retrieved, which may be <= num_channels
	short getChannelList(CHANNEL_ID* channels, short num_channels);

	int getChannelIndexSize();

	void dumpChannelList(ostream& ostr, AnimSkeleton* skel);

	float* getChannelPtr(CHANNEL_ID& c);

	bool isDeltaData() 
	{ 
		return data_is_deltas; 
	}

	bool isAbsoluteData() 
	{ 
		return !data_is_deltas; 
	}

	float getValue(CHANNEL_ID c, int frame)
	{
		short i = getChannelIndex(c);
		if (i < 0) return 0.0f;
		/*
		//if (use_generators && (i>5)) 
		if (use_generators) 
		{
			if (c.bone_id == 2)
				logout << c.bone_id << " " << c.dof_id << " " << frame
					<< " " << getGeneratedValue(c, frame) << " " << data.element(frame, i) << endl;
			return getGeneratedValue(c, frame);
		}
		*/
		return data.element(frame, i);
	}

	void setValue(CHANNEL_ID c, int frame, float value)
	{
		short i = getChannelIndex(c);
		if (i < 0) return; // FIXIT! throw exception
		data.element(frame, i) = value;
	}

	void setValueByChannelIndex(int c, int frame, float value)
	{
		data.element(frame, c) = value;
	}

	bool extractAngleMatrix(Array2D<float>& amat);

	/*
	CHANNEL_ID makeChannelId(string bone_name, DOF_ID d)
	{
		// FIXIT! This does nothing at the moment
		//return channel_index.makeChannelId(bone_name, d);
		return CHANNEL_ID();
	}
	*/

	// computeDeltas creates sets the current MotionSequence (*this) to
	// be computed from the frame deltas of the parameter motion.
	void computeDeltas(MotionSequence& motion);

	void scaleChannel(CHANNEL_ID channel, float scale);

	MotionSequence* extractMotionSegment(int i, int j, SkeletonDefinition* skeleton);

};

ostream& operator<<(ostream& out, MotionSequence& ms);

#endif
