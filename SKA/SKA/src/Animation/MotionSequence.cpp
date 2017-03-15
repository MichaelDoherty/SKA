//-----------------------------------------------------------------------------
// MotionSequence.cpp
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

#include <Core/SystemConfiguration.h>
#include <complex>
#include <Core/SystemLog.h>
#include <Core/Utilities.h>
#include <Signals/FFT.h>
#include <Animation/AnimationException.h>
#include <Animation/MotionSequence.h>
#include <Animation/Skeleton.h>

class ChannelMap
{
	friend ostream& operator<<(ostream&, ChannelMap&);
private:
	map<CHANNEL_ID, short> channels;
public:
	short addChannel(CHANNEL_ID& c)
	{ 
		// if channel already exists, just return its index
		short i = getChannelIndexFromID(c);
		if (i>=0) return i;
		// add new channel, with a new index
		short new_index = short(channels.size());
		channels.insert(pair<CHANNEL_ID,short>(c,new_index)); 
		return new_index;
	}
	short numChannels() 
	{ 
		return short(channels.size()); 
	}
	void clear()
	{
		channels.clear();
	}
	short getChannelIndexFromID(CHANNEL_ID& c)
	{
		map<CHANNEL_ID, short>::iterator iter = channels.find(c);
		if (iter == channels.end()) return -1;
		return (*iter).second;
	}
	CHANNEL_ID getChannelIDFromIndex(short index)
	{
		map<CHANNEL_ID, short>::iterator iter = channels.begin();
		while (iter != channels.end())
		{
			if (iter->second == index) return iter->first;
			iter++;
		}
		return CHANNEL_ID(0, CT_INVALID);
	}
	short getChannelList(CHANNEL_ID c[], short n)
	{
		short i = 0;
		map<CHANNEL_ID, short>::iterator iter = channels.begin();
		while ((iter != channels.end()) && (i < n))
		{
			c[i] = (*iter).first;
			iter++;
			i++;
		}
		return i;
	}
	short size() { return short(channels.size()); }
};

ostream& operator<<(ostream& out, ChannelMap& cm)
{
	map<CHANNEL_ID, short>::iterator iter = cm.channels.begin();
	while (iter != cm.channels.end())
	{
		CHANNEL_ID cid = (*iter).first;
		short cindex = (*iter).second;
		out << "<" << cid.bone_id << "," << cid.channel_type << "> -> " << cindex << endl;
		iter++;
	}
	return out;
}

// ----------- construction methods -----------------

MotionSequence::MotionSequence() 
	: motion_id(NULL), documentation(NULL), source(NULL)
{
	motion_id = strClone("UNDEF");
	documentation = strClone("UNDEF");
	source = strClone("UNDEF");

	channel_map = new ChannelMap;
	clear();
	frame_rate = 120.0f;
}

MotionSequence::MotionSequence(const MotionSequence& rhs)
	: motion_id(NULL), documentation(NULL), source(NULL)
{
	motion_id = strClone(rhs.motion_id);
	documentation = strClone(rhs.documentation);
	source = strClone(rhs.source);

	channel_map = new ChannelMap;
	channel_map = rhs.channel_map;
	num_frames = rhs.num_frames;
	frame_rate = rhs.frame_rate;
	duration = rhs.duration;
	data = rhs.data;
}

MotionSequence::~MotionSequence()
{
	strDelete(motion_id);
	strDelete(documentation);
	strDelete(source);
	delete channel_map;
}

void MotionSequence::clear()
{
	strDelete(motion_id);
	strDelete(documentation);
	strDelete(source);

	channel_map->clear();
	motion_id = strClone("UNDEF");
	documentation = strClone("UNDEF");
	source = strClone("UNDEF");
	num_frames = 0;
	duration = 0.0f;
	data.clear();
}

void MotionSequence::bulkBuild(CHANNEL_ID* _cid, short _num_channels, Array2D<float>& _raw_data)
{
	num_frames = _raw_data.getRows();
	channel_map->clear();
	data.resize(num_frames, _num_channels);
	for (short c=0; c<_num_channels; c++)
	{
		channel_map->addChannel(_cid[c]);
		//memcpy(data.getColumnPtr(c), raw_data.getColumnPtr(c), num_frames*sizeof(float));
	}
	data = _raw_data;
}

void MotionSequence::bulkLoad(Array2D<float>& _raw_data)
{
	data = _raw_data;
}

void MotionSequence::bulkLoad(long _rows, long _columns, void* _raw_data)
{
	data.loadRawData(_rows, _columns, _raw_data);
}

short MotionSequence::addChannel(CHANNEL_ID& channel)
{
	short i = channel_map->getChannelIndexFromID(channel);
	if (i>=0) return i;
	i = channel_map->addChannel(channel);
	if (i != data.getColumns()) throw AnimationException("MotionSequence::addChannel() - incorrect channel counts.");
	data.addColumns(1);
	return numChannels()-1;
}

void removeChannel(CHANNEL_ID& channel)
{
	throw AnimationException("MotionSequence::removeChannel() not implemented.");
}

void MotionSequence::adjustStorage()
{
	data.resize(num_frames, channel_map->size());
}

// ----------- modifier methods -----------------
void MotionSequence::setId(char* _id) 
{ 
	if (motion_id != NULL) strDelete(motion_id); 
	motion_id = strClone(_id); 
}

void MotionSequence::clearDocumentation() 
{ 
	if (documentation != NULL) strDelete(documentation); 
	documentation = strClone(""); 
}

void MotionSequence::addDocumentation(const char* d) 
{ 
	if (documentation == NULL) { documentation = strClone(d); return; }
	char* newd = new char[strlen(documentation)+strlen(d)+1];
	strcpy(newd, documentation);
	strcat(newd, d);
	strDelete(documentation); 
	documentation = newd;
}

void MotionSequence::setSource(const char* s) 
{
	if (source != NULL) strDelete(source); 
	source = strClone(s); 
}

void MotionSequence::setFrameRate(float f) 
{ 
	frame_rate = f; 
	duration = num_frames/frame_rate;
}

void MotionSequence::setNumFrames(long n)
{
	num_frames = n;
	data.resize(num_frames, channel_map->size());
}

void MotionSequence::scaleChannel(CHANNEL_ID channel, float scale)
{
	short i = channel_map->getChannelIndexFromID(channel);
	if (i < 0) return;
	for (long frame=0; frame<num_frames; frame++)
		data.element(frame, i) = scale*data.element(frame, i);
}

void MotionSequence::setValue(CHANNEL_ID channel, long frame, float value)
{
	short i = getChannelIndex(channel);
	if (i < 0) return; // FIXIT! throw exception
	data.element(frame, i) = value;
}

void MotionSequence::setValue(short channel_index, long frame, float value)
{
	data.element(frame, channel_index) = value;
}

// ----------- accessor methods -----------------

short MotionSequence::numChannels() 
{ 
	return channel_map->size(); 
}

bool MotionSequence::isValidChannel(CHANNEL_ID& c)
{
	return (channel_map->getChannelIndexFromID(c) >= 0);
}

short MotionSequence::getChannelIndex(CHANNEL_ID& c)
{
	return channel_map->getChannelIndexFromID(c);
}

CHANNEL_ID MotionSequence::getChannelID(short channel_index)
{
	return channel_map->getChannelIDFromIndex(channel_index);
}

// get the first num_channels channels
// channels is assumed to be large enough to hold num_channels channels
// returns number of channels actually retrieved, which may be <= num_channels
short MotionSequence::getChannelList(CHANNEL_ID* channels, short num_channels)
{
	return channel_map->getChannelList(channels, num_channels);
}

short MotionSequence::getChannelIndexSize()
{
	return channel_map->size();
}

float* MotionSequence::getChannelPtr(CHANNEL_ID& c)
{
	short i = channel_map->getChannelIndexFromID(c);
	if (i<0) return NULL;
	return data.getColumnPtr(i);
}

float* MotionSequence::getChannelPtr(short _channel_index)
{
	return data.getColumnPtr(_channel_index);
}

float MotionSequence::getValue(CHANNEL_ID c, long frame)
{
	short i = getChannelIndex(c);
	if (i < 0) return 0.0f;
	return data.element(frame, i);
}

bool MotionSequence::extractAngleMatrix(Array2D<float>& amat)
{
	// find max bone id
	short n = numChannels();
	CHANNEL_ID* channels = new CHANNEL_ID[n];
	n = getChannelList(channels, n);
	short max_bone = -1;
	for (short i=0; i<n; i++)
		if (channels[i].bone_id > max_bone) max_bone = channels[i].bone_id;
	delete [] channels;
	short num_bones = max_bone+1;

	amat.resize(num_frames, 3*num_bones);

	for (long frame=0; frame<num_frames; frame++)
	{
		for (short b=0; b<num_bones; b++)
		{
			for (short d=3; d<6; d++)
			{
				CHANNEL_ID channel(b, CHANNEL_TYPE(d));
				short index = getChannelIndex(channel);
				float value = 0.0f;
				if (index>= 0) value = data.get(frame, index);
				amat.set(frame, (b*3)+(d-3), value);
			}
		}
	}
	return true;
}

/*
MotionSequence* MotionSequence::extractMotionSegment(int i, int j, Skeleton* skeleton)
{
	int num_channels = 0;
	int motionSegFrame = 0;
	CHANNEL_ID curr_channel;
	CHANNEL_ID* channels;
	MotionSequence* motionSeg = new MotionSequence();
	motionSeg->setNumFrames(j-i);
	motionSeg->buildChannelsFromSkeleton(skeleton);
	//return a motion sequence from frame i to frame j of the original motion
	for(int k = i; k <= j; k++)//from frame i to frame j
	{
		num_channels = numChannels();
		channels = new CHANNEL_ID[num_channels];
		num_channels = getChannelList(channels, num_channels);
		for(int n = 0; n < num_channels; n++)
		{
			BONE_ID b = channels[n].bone_id;
			CHANNEL_TYPE dof = channels[n].channel_type;
			curr_channel.bone_id = b;
			curr_channel.channel_type = dof;
			motionSeg->setValue(curr_channel, motionSegFrame, getValue(curr_channel, k));
		}
		motionSegFrame++;
	}
	return motionSeg;
}
*/

// ----------- debugging methods -----------------

void MotionSequence::dumpChannelList(ostream& ostr)
{
	ostr << *channel_map << endl;
}

ostream& operator<<(ostream& out, MotionSequence& ms)
{
	ms.data.dump(out);
	return out;
}

