//-----------------------------------------------------------------------------
// Channel.h
//	 Classes/functions for handling channel indexes, which combine a 
//   bone identifier with a DOF identifier.
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
#ifndef CHANNEL_DOT_H
#define CHANNEL_DOT_H
#include "Core/SystemConfiguration.h"
#include <string>
#include <map>
#include <set>
#include <vector>
#include <fstream>
using namespace std;
#include "Core/DOF.h"

typedef unsigned short BONE_ID;

struct CHANNEL_ID
{
	BONE_ID bone_id;
	DOF_ID dof_id;
	CHANNEL_ID() { bone_id = -1; dof_id = DOF_X; }
	CHANNEL_ID(BONE_ID b, DOF_ID d) { bone_id = b; dof_id = d; }
	friend bool operator<(const CHANNEL_ID& lhs, const CHANNEL_ID& rhs);
};

inline bool operator<(const CHANNEL_ID& lhs, const CHANNEL_ID& rhs)
{
	if (lhs.bone_id==rhs.bone_id) return lhs.dof_id<rhs.dof_id;
	return lhs.bone_id<rhs.bone_id;
}

inline ostream& operator<<(ostream& out, CHANNEL_ID& channel)
{
	out << "CHANNEL<" << channel.bone_id << "," << channel.dof_id << ">";
	return out;
}

class BoneSet
{
private:
	set<BONE_ID> bones;
public:
	void addBone(BONE_ID b) { bones.insert(b); }
	short numBones() { return bones.size(); }
	short getBones(BONE_ID b[], short n)
	{
		short i = 0;
		set<BONE_ID>::iterator iter = bones.begin();
		while ((iter != bones.end()) && (i < n))
		{
			b[i] = (*iter);
			iter++;
			i++;
		}
		return i;
	}
};

class ChannelSet
{
private:
	set<CHANNEL_ID> channels;
public:
	void addChannel(CHANNEL_ID& c) { channels.insert(c); }
	short numChannels() { return channels.size(); }
	short getChannels(CHANNEL_ID c[], short n)
	{
		short i = 0;
		set<CHANNEL_ID>::iterator iter = channels.begin();
		while ((iter != channels.end()) && (i < n))
		{
			c[i] = (*iter);
			iter++;
			i++;
		}
		return i;
	}
};

class ChannelMap
{
private:
	map<CHANNEL_ID, short> channels;
public:
	short addChannel(CHANNEL_ID& c)
	{ 
		// if channel already exists, just return its index
		map<CHANNEL_ID, short>::iterator iter = channels.find(c);
		if (iter != channels.end()) return (*iter).second;
		// add new channel, with a new index
		short new_index = channels.size();
		channels.insert(pair<CHANNEL_ID,short>(c,new_index)); 
		return new_index;
	}
	short numChannels() 
	{ 
		return channels.size(); 
	}
	void clear()
	{
		channels.clear();
	}
	short getChannelIndex(CHANNEL_ID& c)
	{
		map<CHANNEL_ID, short>::iterator iter = channels.find(c);
		if (iter == channels.end()) return -1;
		return (*iter).second;
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
	short size() { return channels.size(); }
};

class ChannelManager
{
private:
	map<string, BONE_ID> bone_name_to_bone_id;
	map<string, BoneSet> bone_sets;
	map<string, ChannelSet> channel_sets;

public:
	BONE_ID boneIdFromName(string& name)
	{
		map<string,BONE_ID>::iterator iter = bone_name_to_bone_id.find(name);
		if (iter == bone_name_to_bone_id.end()) return -1;
		return (*iter).second;
	}
	void setBoneNameAndId(string& name, BONE_ID id)
	{
		bone_name_to_bone_id.insert(pair<string,BONE_ID>(name, id));
	}

	CHANNEL_ID makeChannelId(string& bone_name, DOF_ID dof)
	{
		return CHANNEL_ID(boneIdFromName(bone_name), dof);
	}

	void makeBoneSet(string& set_name, BONE_ID bones[], short n)
	{
		BoneSet bs;
		for (short i=0; i<n; i++) bs.addBone(bones[i]);
		bone_sets.insert(pair<string, BoneSet>(set_name, bs));
	}

	short boneSetSize(string& set_name)
	{
		map<string,BoneSet>::iterator iter = bone_sets.find(set_name);
		if (iter == bone_sets.end()) return -1;
		return (*iter).second.numBones();
	}

	short getBoneSet(string& set_name, BONE_ID bones[], short n)
	{
		map<string,BoneSet>::iterator iter = bone_sets.find(set_name);
		if (iter == bone_sets.end()) return 0;
		return (*iter).second.getBones(bones, n);
	}

	void makeChannelSet(string& set_name, CHANNEL_ID channels[], short n)
	{
		ChannelSet cs;
		for (short i=0; i<n; i++) cs.addChannel(channels[i]);
		channel_sets.insert(pair<string, ChannelSet>(set_name, cs));
	}

	short channelSetSize(string& set_name)
	{
		map<string,ChannelSet>::iterator iter = channel_sets.find(set_name);
		if (iter == channel_sets.end()) return -1;
		return (*iter).second.numChannels();
	}

	short getChannelSet(string& set_name, CHANNEL_ID channels[], short n)
	{
		map<string,ChannelSet>::iterator iter = channel_sets.find(set_name);
		if (iter == channel_sets.end()) return 0;
		return (*iter).second.getChannels(channels, n);
	}
};

#endif