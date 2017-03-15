//-----------------------------------------------------------------------------
// Color.h
//    Class to hold a color value (red,green,blue,alpha)
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

#ifndef COLOR_DOT_H
#define COLOR_DOT_H
#include <Core/SystemConfiguration.h>
#include <memory.h>

class SKA_LIB_DECLSPEC Color
{
public:
	float c[4];
	float& r;
	float& g;
	float& b;
	float& a;

	// default constructor
	Color() 
		: r(c[0]), g(c[1]), b(c[2]), a(c[3]) 
	{ r=g=b=a=1.0f; }

	// general constructor
	Color(float _r, float _g, float _b, float _a=1.0f)
		: r(c[0]), g(c[1]), b(c[2]), a(c[3]) 
	{ r=_r; g=_g; b=_b; a=_a; }

	// general constructor
	Color(float _c[4])
		: r(c[0]), g(c[1]), b(c[2]), a(c[3]) 
	{ memcpy(c, _c, 4*sizeof(float)); }

	// copy constructor
	Color(const Color& _other)
		: r(c[0]), g(c[1]), b(c[2]), a(c[3])
	{ memcpy(c, _other.c, 4*sizeof(float)); }

	// assignment operator
	Color& operator= (const Color& _rhs) 
	{ 
		memcpy(c, _rhs.c, 4*sizeof(float));
		return *this; 
	}
	
	// destructor - nothing to do
	virtual ~Color() { }

	// equality test
	bool operator==(const Color &rhs) 
	{
		return (r==rhs.r) && (g==rhs.g) && (b==rhs.b) && (a==rhs.a);
	}
};

#endif
