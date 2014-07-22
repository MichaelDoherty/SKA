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
// Version 3.0 - July 18, 2014 - Michael Doherty
//-----------------------------------------------------------------------------
#include "Core/SystemConfiguration.h"
#include <complex>
#include "Core/SystemLog.h"
#include "Core/Utilities.h"
#include "Signals/FFT.h"
#include "Animation/MotionSequence.h"
#include "Animation/AnimSkeleton.h"

MotionSequence::MotionSequence() : motion_id(string("UNDEF"))
{
	channel_index = new ChannelMap;
	clear();
	frame_rate = 120.0f;
	data_is_deltas = false;
	//has_generators = false;
	//use_generators = false;
}

MotionSequence::MotionSequence(const MotionSequence& rhs)
{
	channel_index = new ChannelMap;
	motion_id = rhs.motion_id;
	channel_index = rhs.channel_index;
	num_frames = rhs.num_frames;
	frame_rate = rhs.frame_rate;
	duration = rhs.duration;
	data_is_deltas = rhs.data_is_deltas;
	data = rhs.data;
	//has_generators = rhs.has_generators;
	//use_generators = rhs.use_generators;
	//generator_specs = rhs.generator_specs;
}

MotionSequence::~MotionSequence()
{
	delete channel_index;
	/*
	map<CHANNEL_ID, SignalGenerator*>::iterator iter = generators.begin();
	while (iter != generators.end())
	{
		delete (*iter).second;
		iter++;
	}
	*/
}

void MotionSequence::clear()
{
	channel_index->clear();
	motion_id = string("UNDEF");
	num_frames = 0;
	duration = 0.0f;
	data.clear();
	//generator_specs.clear();
	//has_generators = false;
	data_is_deltas = false;
	//use_generators = false;
}

bool MotionSequence::analyzeSignals()
{
	/*
	// FIXIT! use dynamic alloc to make this length channel_index.size()
	CHANNEL_ID channels[500];
	int num_channels = channel_index.getChannelList(channels, 500);

	int c;
	for (c=0; c<num_channels; c++)
	{
		int c_ix = channel_index.getChannelIndex(channels[c]);
		if (c_ix < 0)
		{
			logout << "bad channel index " << channels[c] << " c is " << c << endl;
			continue;
		}
		float* source = data.getColumnPtr(c_ix);
		// compute mean of source
		double mean = 0.0f;
		for (int i=0; i<num_frames; i++)
			mean += source[i];
		mean = mean/num_frames;
		float* signal = new float[num_frames];
		for (int i=0; i<num_frames; i++)
			signal[i] = float(source[i]-mean);
		complex<float>* spectrum = new complex<float>[num_frames];
		if (computeFFT(signal, spectrum, num_frames))
		{
			vector<SignalSpec> signals;
			float freq_factor = 0.95f; // between 0 and 1
			int num_signals = int(freq_factor*(num_frames/2.0));
			num_signals = 20;
			int n2 = extractSignalsFromSpectrum(spectrum, num_frames, signals, num_signals);
			SignalSpec const_spec; 
			const_spec.amplitude = float(mean);
			const_spec.frequency = 0.0f;
			const_spec.phase = 0.0f;
			signals.push_back(const_spec);
			generator_specs.insert(pair<CHANNEL_ID, vector<SignalSpec> >(channels[c], signals));
			SignalGenerator* g = buildSignalGenerator(signals);
			generators.insert(pair<CHANNEL_ID,SignalGenerator*>(channels[c],g));
		}
		delete [] spectrum;
		delete [] signal;
	}
	has_generators = true;
	return has_generators;
	*/
	return false;
}
	/*
float MotionSequence::getGeneratedValue(CHANNEL_ID c, int frame)
{
	float time = float(frame) / frame_rate;
	map<CHANNEL_ID, SignalGenerator*>::iterator iter = generators.find(c);
	if (iter == generators.end()) return 0.0f;
	if ((*iter).second == NULL) return 0.0f;
	return (*iter).second->signal(time);
}
*/
/*
void MotionSequence::printGenerators(ostream& out)
{
	if (!has_generators)
	{
		out << "No Generators" << endl;
		return;
	}
	CHANNEL_ID channels[500];
	int num_channels = channel_index.getChannelList(channels, 500);
	int c;
	for (c=0; c<=num_channels; c++)
	{
		map<CHANNEL_ID, vector<SignalSpec> >::iterator iter = generator_specs.find(channels[c]);
		if (iter != generator_specs.end())
		{
			out << channels[c] << endl;
			vector<SignalSpec>& specs = (*iter).second;
			for (unsigned short s=0; s<specs.size(); s++)
				out << "A " << specs[s].amplitude
				<< " F " << specs[s].frequency
				<< " P " << specs[s].phase << endl;
		}
	}
}
*/

// This MotionSequence is composed of deltas from motion.
// First frame contains absolute values, not deltas.
void MotionSequence::computeDeltas(MotionSequence& motion)
{
	channel_index = motion.channel_index;
	num_frames = motion.num_frames;
	frame_rate = motion.frame_rate;
	duration = motion.duration;
	data.resize(num_frames, channel_index->size());
	data_is_deltas = true;

	int frame, channel;

	for (channel=0; channel<channel_index->size(); channel++)
		data.element(0, channel) = motion.data.element(0, channel);

	for (frame=1; frame<num_frames; frame++)
	{
		for (channel=0; channel<channel_index->size(); channel++)
		{
			data.element(frame, channel) = 
				motion.data.element(frame,channel) - motion.data.element(frame-1, channel);
		}
	}
}

void MotionSequence::scaleChannel(CHANNEL_ID channel, float scale)
{
	short i = channel_index->getChannelIndex(channel);
	if (i < 0) return;
	for (int frame=0; frame<num_frames; frame++)
		data.element(frame, i) = scale*data.element(frame, i);
}

int MotionSequence::numChannels() 
{ 
	return channel_index->size(); 
}

short MotionSequence::addChannel(CHANNEL_ID& channel)
{
	return channel_index->addChannel(channel);
}

void MotionSequence::adjustStorage()
{
	data.resize(num_frames, channel_index->size());
}

void MotionSequence::setNumFrames(short n)
{
	num_frames = n;
	data.resize(num_frames, channel_index->size());
}


bool MotionSequence::isValidChannel(CHANNEL_ID& c)
{
	return (channel_index->getChannelIndex(c) >= 0);
}

int MotionSequence::getChannelIndex(CHANNEL_ID& c)
{
	short i = channel_index->getChannelIndex(c);
	if (i<0) return -1;
	return i;
}

int MotionSequence::getChannelIndexSize()
{
	return channel_index->size();
}

// get the first num_channels channels
// channels is assumed to be large enough to hold num_channels channels
// returns number of channels actually retrieved, which may be <= num_channels
short MotionSequence::getChannelList(CHANNEL_ID* channels, short num_channels)
{
	return channel_index->getChannelList(channels, num_channels);
}

float* MotionSequence::getChannelPtr(CHANNEL_ID& c)
{
	short i = channel_index->getChannelIndex(c);
	if (i<0) return NULL;
	return data.getColumnPtr(i);
}

void MotionSequence::buildChannelsFromSkeleton(SkeletonDefinition* skeleton)
{
	channel_index->clear();
	for (short b=0; b<skeleton->numBones(); b++)
	{
		for (short dof=0; dof<6; dof++)
		{
			CHANNEL_ID c(b,DOF_ID(dof));
			if (skeleton->isActiveDOF(b,dof))
				channel_index->addChannel(c);
		}
	}
	data.resize(num_frames, channel_index->size());
}

bool MotionSequence::convertData(Array2D<float>& amc_data, SkeletonDefinition* skeleton)
{
	buildChannelsFromSkeleton(skeleton);
	num_frames = amc_data.getRows();
	data.resize(num_frames, channel_index->size());
	data_is_deltas = false;
	duration = num_frames/frame_rate;
	CHANNEL_ID channels[MAX_ASF_BONES*6];
	if (channel_index->getChannelList(channels, channel_index->size()) != channel_index->size())
		logout << "something screwy in MotionSequence::convertData" << endl;
	for (short c=0; c<channel_index->size(); c++)
	{
		short bone = channels[c].bone_id;
		short dof = channels[c].dof_id;
		short index = channel_index->getChannelIndex(channels[c]);
		memcpy(data.getColumnPtr(index), amc_data.getColumnPtr(bone*6+dof), 
			data.getRows()*sizeof(float));
	}
	return true;
}

bool MotionSequence::copyDataWithoutConversion(Array2D<float>& bvh_data)
{
	data = bvh_data;
	return true;
}

ostream& operator<<(ostream& out, MotionSequence& ms)
{
	ms.data.dump(out);
	return out;
}


void MotionSequence::dumpChannelList(ostream& ostr, AnimSkeleton* skel)
{
	// find max bone id
	int n = 100;
	CHANNEL_ID* channels = new CHANNEL_ID[n];
	n = getChannelList(channels, n);
	int max_bone = -1;
	for (int i=0; i<n; i++)
		if (channels[i].bone_id > max_bone) max_bone = channels[i].bone_id;
	delete [] channels;

	for (int b=0; b<=max_bone; b++)
	{
		string bname = skel->getDescription()->boneNameFromId(b);
		int first_dof = 3;
		if (b==0) first_dof = 0;
		for (int d=first_dof; d<6; d++)
		{
			CHANNEL_ID channel(b,DOF_ID(d));
			int index = getChannelIndex(channel);
			string DOF_label = bname + "_" + toString<DOF_ID>(DOF_ID(d));
			ostr << DOF_label << " , " << b << " , " << d << " , " << index << endl;
		}
	}
}

bool MotionSequence::extractAngleMatrix(Array2D<float>& amat)
{
	// find max bone id
	int n = 100;
	CHANNEL_ID* channels = new CHANNEL_ID[n];
	n = getChannelList(channels, n);
	int max_bone = -1;
	for (int i=0; i<n; i++)
		if (channels[i].bone_id > max_bone) max_bone = channels[i].bone_id;
	delete [] channels;
	int num_bones = max_bone+1;

	amat.resize(num_frames, 3*num_bones);

	for (int frame=0; frame<num_frames; frame++)
	{
		for (int b=0; b<num_bones; b++)
		{
			for (int d=3; d<6; d++)
			{
				CHANNEL_ID channel(b,DOF_ID(d));
				int index = getChannelIndex(channel);
				float value = 0.0f;
				if (index>= 0) value = data.get(frame, index);
				amat.set(frame, (b*3)+(d-3), value);
			}
		}
	}
	return true;
}

MotionSequence* MotionSequence::extractMotionSegment(int i, int j, SkeletonDefinition* skeleton)
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
			DOF_ID dof = channels[n].dof_id;
			curr_channel.bone_id = b;
			curr_channel.dof_id = dof;
			motionSeg->setValue(curr_channel, motionSegFrame, getValue(curr_channel, k));
		}
		motionSegFrame++;
	}
	return motionSeg;
}