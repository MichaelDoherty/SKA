//-----------------------------------------------------------------------------
// Point2D.h
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

#ifndef POINT2D_DOT_H
#define POINT2D_DOT_H
#include <Core/SystemConfiguration.h>
#include <cmath>
#include <fstream>
#include <cstring> // (for memcpy)
using namespace std;
#include <Math/Math.h>

class SKA_LIB_DECLSPEC Point2D
{
	SKA_LIB_DECLSPEC friend ostream& operator<<(ostream& out, Point2D& p);
public:
	// store as an array
	float p[2];
	// allow access through named fields
	float& x; float& y;
	float& u; float& v;

	// default constructor - all zeros
	Point2D() 
		: x(p[0]), y(p[1]), u(p[0]), v(p[1])
	{ 
		p[0] = p[1] = 0.0f; 
	}
	// constructor from x,y,z
	Point2D(float _x, float _y) 
		: x(p[0]), y(p[1]), u(p[0]), v(p[1])
	{ 
		p[0] = _x; p[1] = _y;
	}
	// constructor from array
	Point2D(float a[2]) 
		: x(p[0]), y(p[1]), u(p[0]), v(p[1])
	{ 
		memcpy(p, a, 2*sizeof(float)); 
	}
	// copy constructor
	Point2D(const Point2D& _v) 
		: x(p[0]), y(p[1]), u(p[0]), v(p[1])
	{ 
		memcpy(p, _v.p, 2*sizeof(float));  
	}
	// assignment operator
	Point2D& operator= (const Point2D& _v) 
	{ 
		memcpy(p, _v.p, 2*sizeof(float)); 
		return *this; 
	}
	// destructor - nothing to do
	virtual ~Point2D() { }
};

#endif
