//-----------------------------------------------------------------------------
// Channel.h
//   A channel is a stream of scalar data that controls some aspect of an 
//     animation. This file defines types used for identifying channels.
//	 CHANNEL_TYPE enumerates the kinds of data that can be 
//     stored in a motion channel.
//   CHANNEL_ID combines a CHANNEL_TYPE with a bone index to identify
//     a unique channel. 
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

#ifndef CHANNEL_DOT_H
#define CHANNEL_DOT_H
#include <Core/SystemConfiguration.h>
#include <string.h>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <fstream>
using namespace std;

typedef unsigned short BONE_ID;

enum CHANNEL_TYPE
{ 
	CT_TX = 0,	// translation along X axis
	CT_TY,		// translation along Y axis
	CT_TZ,		// translation along Z axis 
	CT_RX,		// rotation around X axis (pitch)
	CT_RY,		// rotation around X axis (yaw)
	CT_RZ,		// rotation around X axis (roll)
	CT_QW,		// quaternion W component
	CT_QX,		// quaternion X component
	CT_QY,		// quaternion Y component
	CT_QZ,		// quaternion Z component
	CT_INVALID
};

static const short NUMBER_OF_CHANNEL_TYPES = short(CT_INVALID);

inline ostream& operator<<(ostream& out, CHANNEL_TYPE& ct)
{
	switch (ct)
	{
	case CT_TX: out << "tx"; break;
	case CT_TY: out << "ty"; break;
	case CT_TZ: out << "tz"; break;
	case CT_RX: out << "rx"; break;
	case CT_RY: out << "ry"; break;
	case CT_RZ: out << "rz"; break;
	case CT_QW: out << "qw"; break;
	case CT_QX: out << "qx"; break;
	case CT_QY: out << "qy"; break;
	case CT_QZ: out << "qz"; break;
	default: out << "--"; break;
	}
	return out;
}

inline CHANNEL_TYPE stringToChannelType(const char* s)
{
	if (strcmp(s,"tx")==0) return CT_TX;
	if (strcmp(s,"ty")==0) return CT_TY;
	if (strcmp(s,"tz")==0) return CT_TZ;
	if (strcmp(s,"rx")==0) return CT_TX;
	if (strcmp(s,"ry")==0) return CT_TY;
	if (strcmp(s,"rz")==0) return CT_TZ;
	if (strcmp(s,"qw")==0) return CT_QW;
	if (strcmp(s,"qx")==0) return CT_QX;
	if (strcmp(s,"qy")==0) return CT_QY;
	if (strcmp(s,"qz")==0) return CT_QZ;
	return CT_INVALID;
}

struct CHANNEL_ID
{
	BONE_ID bone_id;
	CHANNEL_TYPE channel_type;
	CHANNEL_ID() { bone_id = -1; channel_type = CT_TX; }
	CHANNEL_ID(BONE_ID b, CHANNEL_TYPE d) { bone_id = b; channel_type = d; }
	friend bool operator<(const CHANNEL_ID& lhs, const CHANNEL_ID& rhs);
};

inline bool operator<(const CHANNEL_ID& lhs, const CHANNEL_ID& rhs)
{
	if (lhs.bone_id==rhs.bone_id) return lhs.channel_type<rhs.channel_type;
	return lhs.bone_id<rhs.bone_id;
}

inline ostream& operator<<(ostream& out, CHANNEL_ID& channel)
{
	out << "CHANNEL<" << channel.bone_id << "," << channel.channel_type << ">";
	return out;
}

#endif