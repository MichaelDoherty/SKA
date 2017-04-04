//-----------------------------------------------------------------------------
// Vector3D.h
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

#ifndef VECTOR3D_DOT_H
#define VECTOR3D_DOT_H
#include <Core/SystemConfiguration.h>
#include <cmath>
#include <fstream>
#include <cstring> // (for memcpy)
using namespace std;
#include <Math/Math.h>

class SKA_LIB_DECLSPEC Vector3D
{
	SKA_LIB_DECLSPEC friend ostream& operator<<(ostream& out, Vector3D& v);
public:
	// store as an array
	float v[3];
	// allow access through named fields
	float& x; float& y;	float& z;
	float& pitch; float& yaw; float& roll;

	// default constructor - all zeros
	Vector3D() 
		: x(v[0]), y(v[1]), z(v[2]), pitch(v[0]), yaw(v[1]), roll(v[2])
	{ 
		v[0] = v[1] = v[2] = 0.0f; 
	}
	// constructor from x,y,z
	Vector3D(float _x, float _y, float _z) 
		: x(v[0]), y(v[1]), z(v[2]), pitch(v[0]), yaw(v[1]), roll(v[2])
	{ 
		v[0] = _x; v[1] = _y; v[2] = _z; 
	}
	// constructor from array
	Vector3D(float a[3]) 
		: x(v[0]), y(v[1]), z(v[2]), pitch(v[0]), yaw(v[1]), roll(v[2])
	{ 
		memcpy(v, a, 3*sizeof(float)); 
	}
	// copy constructor
	Vector3D(const Vector3D& _v) 
		: x(v[0]), y(v[1]), z(v[2]), pitch(v[0]), yaw(v[1]), roll(v[2]) 
	{ 
		memcpy(v, _v.v, 3*sizeof(float));  
	}
	// assignment operator
	Vector3D& operator= (const Vector3D& _v) 
	{ 
		memcpy(v, _v.v, 3*sizeof(float)); 
		return *this; 
	}
	// destructor - nothing to do
	virtual ~Vector3D() { }

	float getX() { return x; }
	float getY() { return y; }
	float getZ() { return z; }

	bool operator==(const Vector3D &rhs) 
	{
		return (x==rhs.x) && (y==rhs.y) && (z==rhs.z);
	}
	Vector3D& operator+=(const Vector3D &rhs) 
	{
		x += rhs.x; y += rhs.y; z += rhs.z;
		return *this;
	}
	Vector3D& operator-=(const Vector3D &rhs) 
	{
		x -= rhs.x; y -= rhs.y; z -= rhs.z;
		return *this;
	}
	Vector3D& operator*=(const float s) 
	{
		x *= s; y *= s; z *= s;
		return *this;
	}
	Vector3D& operator/=(const float s) 
	{
		x /= s; y /= s; z /= s;
		return *this;
	}
	Vector3D operator+(const Vector3D &rhs) const
	{
		return Vector3D(*this) += rhs;
	}
	Vector3D operator-(const Vector3D &rhs) const
	{
		return Vector3D(*this) -= rhs;
	}
	Vector3D operator*(const float s) const
	{
		return Vector3D(*this) *= s;
	}
	Vector3D operator/(const float s) const
	{
		return Vector3D(*this) /= s;
	}
	float magnitudeSquared() const
	{
		return x*x+y*y+z*z;
	}
	float magnitude() const
	{
		return sqrt(x*x+y*y+z*z);
	}
	Vector3D normalize()
	{
		Vector3D v = *this;
		return v /= magnitude();
	}
	float dot(const Vector3D& rhs) const
	{
		return x*rhs.x + y*rhs.y + z*rhs.z; 
	}
	// ndot = normalized dot product
	float ndot(const Vector3D& rhs) const
	{
		float a = (x*rhs.x + y*rhs.y + z*rhs.z) / (magnitude()*rhs.magnitude());
		if (a > 1.0f) a = 1.0f;
		if (a < -1.0f) a = -1.0f;
		return a;
	}
	Vector3D cross(const Vector3D& rhs) const
	{
		return Vector3D(y*rhs.z-z*rhs.y, z*rhs.x-x*rhs.z, x*rhs.y-y*rhs.x);
	}
	bool isUnit() const
	{
		return isZero(magnitudeSquared()-1.0f);
	}
	float distance(const Vector3D& rhs) const
	{
		float dx = x - rhs.x;
		float dy = y - rhs.y;
		float dz = z - rhs.z;
		return sqrt(dx*dx+dy*dy+dz*dz);
	}
};

#endif
