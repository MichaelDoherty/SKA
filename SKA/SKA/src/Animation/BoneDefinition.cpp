//-----------------------------------------------------------------------------
// BoneDefinition.cpp
//	 Stores the static definition of a bone.
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
#include "Animation/BoneDefinition.h"
#include "Core/SystemLog.h"
#include "Core/Utilities.h"

ostream& operator<<(ostream& out, BoneDefinition& bone)
{
	out << "Name: " << bone.name << endl;
	out << "Direction: " << bone.direction << endl;
	out << "Length: " << bone.length << endl;

	out << "Axis: " << bone.axis << " " 
		<< bone.axis_order[0] << bone.axis_order[1] << bone.axis_order[2] <<endl;

	for (short d=0; d<6; d++)
	{
		if (!bone.dof[d].valid)
			out << "DOF " << d << " is not enabled" << endl;
		else
			out << "DOF " << d << " limits(" 
				<< bone.dof[d].min << "," << bone.dof[d].max << ")" << endl;
	}
	return out;
}
