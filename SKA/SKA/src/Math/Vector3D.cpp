//-----------------------------------------------------------------------------
// Vector3D.cpp
//    Class for holding and operating on triples of floats.
//    Supports standard operations for 3D vectors.
//    Also (somewhat inappropriately) used for 
//    orientations (roll, pitch, yaw) and 3D points.
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
#include <Math/Vector3D.h>

ostream& operator<<(ostream& out, Vector3D& v)
{
	out << "Vector<" << v.x << "," << v.y << "," << v.z << ">";
	return out;
}
