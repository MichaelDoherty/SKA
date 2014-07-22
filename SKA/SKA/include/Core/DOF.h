//-----------------------------------------------------------------------------
// DOF.h
//   Enumerated type to give consistent indexing/labeling
//   of degrees of freedom for entire system
//-----------------------------------------------------------------------------
// This software is part of the Skeleton Animation Toolkit (SKA) developed by 
// at the University of the Pacific, under the guidance of Michael Doherty.
// For information please contact mdoherty@pacific.edu.
//-----------------------------------------------------------------------------
// This is open software. You are free to use it as you see fit.
// The University of the Pacific and identified authors should be
// credited for any significant use, particularly if used for commercial 
// projects or academic research publications.
//-----------------------------------------------------------------------------
// Version 1.0 - January 25, 2012 - Michael Doherty
//-----------------------------------------------------------------------------
#ifndef DOF_DOT_H
#define DOF_DOT_H
#include "Core/SystemConfiguration.h"
#include <iostream>
#include <cstring>
using namespace std;

enum DOF_ID
{ 
	DOF_X = 0, 
	DOF_Y, 
	DOF_Z, 
	DOF_PITCH, // around X axis
	DOF_YAW,   // around Y axis
	DOF_ROLL,  // around Z axis
	DOF_INVALID
};

inline ostream& operator<<(ostream& out, DOF_ID& dof_id)
{
	switch (dof_id)
	{
	case DOF_X: out << "tx"; break;
	case DOF_Y: out << "ty"; break;
	case DOF_Z: out << "tz"; break;
	case DOF_PITCH: out << "rx"; break;
	case DOF_YAW: out << "ry"; break;
	case DOF_ROLL: out << "rz"; break;
	default: out << "--"; break;
	}
	return out;
}

inline DOF_ID atoDOF(const char* s)
{
	if (strcmp(s,"tx")==0) return DOF_X;
	if (strcmp(s,"ty")==0) return DOF_Y;
	if (strcmp(s,"tz")==0) return DOF_Z;
	if (strcmp(s,"rx")==0) return DOF_PITCH;
	if (strcmp(s,"ry")==0) return DOF_YAW;
	if (strcmp(s,"rz")==0) return DOF_ROLL;
	return DOF_INVALID;
}

#endif
