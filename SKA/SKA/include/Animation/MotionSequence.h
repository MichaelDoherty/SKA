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

#ifndef MOTIONSEQUENCE_DOT_H
#define MOTIONSEQUENCE_DOT_H
#include <Core/SystemConfiguration.h>
#include <Math/Math.h>
#include <Core/Array2D.h>
#include <Animation/Skeleton.h>
#include <Animation/Channel.h>
#include <Core/SystemLog.h>

class ChannelMap;

class SKA_LIB_DECLSPEC MotionSequence
{
	SKA_LIB_DECLSPEC friend ostream& operator<<(ostream& out, MotionSequence& ms);

private:
	char* motion_id;
	char* documentation;
	char* source;

	ChannelMap* channel_map;
	Array2D<float> data;
	long num_frames;
	float frame_rate;  // frames / sec
	float duration;    // total time of sequence in seconds

public:

// ----------- construction methods -----------------
	MotionSequence();
	MotionSequence(const MotionSequence& rhs);
	virtual ~MotionSequence();
	void clear();

	// bulkBuild: (re)constructs this motion sequence.
	//   Any previous channel map or data is erased.
	//   The channel map is built from _cid array.
	//   Columns in _raw_data are assumed to match the channel indexes in _cid array
	void bulkBuild(CHANNEL_ID* _cid, short _num_channels, Array2D<float>& _raw_data);

	// bulkLoad: replaces the data in this motion sequence with _raw_data.
	//   FAILURE PRONE: There are no checks to see that the new data is properly sized.
	//   to match the configuration of this motion sequence.
	//   Previous channel map is preserved.
	void bulkLoad(Array2D<float>& _raw_data);

	// bulkLoad: replaces the data in this motion sequence with _raw_data.
	//   FAILURE PRONE: There are no checks to see that the new data is properly sized.
	//   to match the configuration of this motion sequence.
	//   Previous channel map is preserved.
	void bulkLoad(long _rows, long _columns, void* _raw_data);
	
	// following methods are only used by the Blender class. 
	// addChannel creates an index for a channel, but does not currently create storage space for the channel
	// adjustStorage resizes the Array2D, wiping out any data
	short addChannel(CHANNEL_ID& channel);
	void adjustStorage();
	void removeChannel(CHANNEL_ID& channel);

// ----------- modifier methods -----------------
	void setId(char* _id);
	void clearDocumentation();
	void addDocumentation(const char* d);
	void setSource(const char* s);
	void setFrameRate(float f);
	void setNumFrames(long n);
	void scaleChannel(CHANNEL_ID channel, float scale);
	void setValue(CHANNEL_ID channel, long frame, float value);
	void setValue(short channel_index, long frame, float value);

// ----------- accessor methods -----------------
	long numFrames()	{ return num_frames; }
	short numChannels();
	char* getId() { return motion_id; }
	char* getDocumentation() { return documentation; }
	char* getSource() { return source; }
	float getDuration() { return duration; }
	float getFrameRate() { return frame_rate; }

	bool isValidChannel(CHANNEL_ID& c);
	short getChannelIndex(CHANNEL_ID& c);
	CHANNEL_ID getChannelID(short channel_index);

	short getChannelIndexSize();
	float* getChannelPtr(CHANNEL_ID& c);
	float* getChannelPtr(short _channel_index);
	float getValue(CHANNEL_ID c, long frame);

	// specialty functions for extracting from a motion sequence
	bool extractAngleMatrix(Array2D<float>& amat);

	// needs to be reimplemented
	//MotionSequence* extractMotionSegment(int i, int j, Skeleton* skeleton);

// ----------- debugging methods -----------------
	void dumpChannelList(ostream& ostr);

private:
	// get the first num_channels channels
	// channels is assumed to be large enough to hold num_channels channels
	// returns number of channels actually retrieved, which may be <= num_channels
	short getChannelList(CHANNEL_ID* channels, short num_channels);
};

#endif
