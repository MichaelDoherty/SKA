//-----------------------------------------------------------------------------
// Point2D.cpp
//    A pair of floats treated as a 2D point.
//    Does not currently support and mathematical operations.
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
#include <Math/Point2D.h>

ostream& operator<<(ostream& out, Point2D& p)
{
	out << "Point<" << p.x << "," << p.y << ">";
	return out;
}
