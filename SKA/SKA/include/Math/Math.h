//-----------------------------------------------------------------------------
// Math.h
//	 Basic useful math stuff. 
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

#ifndef MATH_DOT_H
#define MATH_DOT_H
#include <Core/SystemConfiguration.h>
#include <cmath>
#include <fstream>
using namespace std;
#include <Core/BasicException.h>

class SKA_LIB_DECLSPEC MathException : public BasicException
{
public:
	MathException() : BasicException("Unspecified Math exception") { }
	MathException(const char* _msg) : BasicException(_msg) { }
	MathException(const MathException& _other) : BasicException(_other) { }
};

const float PI = 3.1415926534f;
const float HALF_PI = PI/2.0f;
const float TWO_PI = 2.0f*PI;
const float EPSILON = 1.0e-6f;

inline float cotangent(float a) { return 1.0f / tan(a); }
inline float rad2deg(float r) { return r*180.0f/PI; }
inline float deg2rad(float r) { return r*PI/180.0f; }

inline bool isZero(float x) { return ( fabs(x) < EPSILON ); } 

#endif
