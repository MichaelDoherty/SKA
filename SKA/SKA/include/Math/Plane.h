//-----------------------------------------------------------------------------
// Plane.h
//    Class for modeling geometric planes in 3D space.
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
#ifndef PLANE_DOT_H
#define PLANE_DOT_H
#include <Core/SystemConfiguration.h>
#include <Math/Vector3D.h>
#include <Math/Matrix4x4.h>

// A plane is defined by a point in the plane, two direction vectors (parallel to the plane)
// and a vector normal to the plane.
// Either the two direction vectors or the normal vector are required to initialize a 
// plane. The remaining vector(s) are computed from the supplied vector(s).
// If a point in the plane is not defined, it is assumed to be the origin (0,0,0).
// Since there are too many ambiguous overloads for the constructors, 
// the actual constructors are limited to definition by directional vectors,
// and a variety of inline pseudo-constructors are defined after the class declaration.

class Plane
{
public:
	// create a plane by supplying two direction vectors
	Plane(Vector3D _d1, Vector3D _d2, Vector3D _point_in_plane)
	{
		defineFromDirectionVectors(_d1, _d2, _point_in_plane);
	}

	// create a plane by supplying two direction vectors
	Plane(Vector3D _d1, Vector3D _d2)
	{
		defineFromDirectionVectors(_d1, _d2, Vector3D(0.0f,0.0f,0.0f));
	}

	// default constructor
	Plane()
	{
		defineFromDirectionVectors(Vector3D(1.0f, 0.0f, 0.0f), 
			Vector3D(0.0f, 1.0f, 0.0f), Vector3D(0.0f, 0.0f, 0.0f));
	}

	// copy constructor
	Plane(const Plane& _p)
	{
		defineFromDirectionVectors(_p.d1, _p.d2, _p.p);
	}

	// assignment operator
	Plane& operator=(const Plane& _p)
	{
		defineFromDirectionVectors(_p.d1, _p.d2, _p.p);
		return *this;
	}

	void defineFromDirectionVectors(Vector3D _d1, Vector3D _d2, Vector3D _point_in_plane)
	{
		p = _point_in_plane;
		d1 = _d1.normalize();
		d2 = _d2.normalize();
		n = d1.cross(d2);
		n = n.normalize();
	}

	void defineFromNormalVector(Vector3D _n, Vector3D _point_in_plane)
	{
		n = _n.normalize();
		p = _point_in_plane;
		// pick an axis that is not near parallel to the normal
		Vector3D a(1.0f, 0.0f, 0.0f);
		if (fabs(n.dot(a)) > 0.95) a = Vector3D(0.0f, 1.0f, 0.0f);
		// find directional vectors perpendicular to the normal vector
		d1 = n.cross(a);
		d2 = n.cross(d1);
	}

	void moveTo(Vector3D& _point_in_plane) { p = _point_in_plane; }

	void rotateTo(Vector3D& _rpy)
	{
		Matrix4x4 rot_xform = Matrix4x4::rotationZXY(_rpy);
		defineFromDirectionVectors(rot_xform.multVector(d1), rot_xform.multVector(d2), p);
	}

	Vector3D getNormal() { return n; }
	Vector3D getPointInPlane() { return p; }

	Vector3D projectVectorOntoPlane(Vector3D _v)
	{
		// projection of v onto plane is v - vn, where vn is the projection of v on the plane normal
		// https://www.maplesoft.com/support/help/Maple/view.aspx?path=MathApps/ProjectionOfVectorOntoPlane
		Vector3D v = _v.normalize();
		return v - n*v.dot(n);
	}

	Vector3D projectPointOntoPlane(Vector3D& _p)
	{
		/*
		inputs: origin = (x0,y0,z0), normal = (a,b,c), _p = (i0,j0,k0)
		equation of the plane:
			a(x-x0) + b(y-y0) + c(z-z0) = 0
		equation of the line made by passing the normal through _p:
			(i,j,k) = (i0+at, j0+bt, k0+ct)
		solving for t:
			a(i0+at-x0) + b(j0+bt-y0) + c(k0+ct-z0) = 0
			(a^2+b^2+c^2)t + a(i0-x0) + b(j0-y0) + c(k0-z0) = 0
			t = (a(x0-i0) + b(y0-j0) + c(z0-k0))/(a^2+b^2+c^2)
		*/
		float x0 = p.x, y0 = p.y, z0 = p.z;
		float i0 = _p.x, j0 = _p.y, k0 = _p.z;
		float a = n.x, b = n.y, c = n.z;
		float t = (a*(x0 - i0) + b*(y0 - j0) + c*(z0 - k0)) / (a*a + b*b + c*c);
		Vector3D intersection(i0 + a*t, j0 + b*t, k0 + c*t);
		return intersection;
	}

private:
	// d1 and d2 are the direction vectors (parallel to the plane)
	// n is the normal vector (perpendicular to the plane)
	// p is a point on the plane
	Vector3D d1, d2, n;
	Vector3D p;
};

// pseudo-contructors

// X = left, Y = up, Z = forward

// X-Y plane divides front/back
inline Plane CoronalPlane()
{
	Plane plane(Vector3D(1.0f, 0.0f, 0.0f), Vector3D(0.0f, 1.0f, 0.0f));
	return plane;
}

// Y-Z plane divides left/right
inline Plane SagittalPlane()
{
	Plane plane(Vector3D(0.0f, 1.0f, 0.0f), Vector3D(0.0f, 0.0f, 1.0f));
	return plane;
}

// Z-X plane divides top/bottom
inline Plane TransversePlane()
{
	Plane plane(Vector3D(0.0f, 0.0f, 1.0f), Vector3D(1.0f, 0.0f, 0.0f));
	return plane;
}

// create a plane normal to _n that passes through _point_in_plane
inline Plane PlaneFromNormal(Vector3D _n, Vector3D _point_in_plane)
{
	Plane plane;
	plane.defineFromNormalVector(_n, _point_in_plane);
	return plane;
}

// create a plane normal to _n that passes through point (0,0,0)
inline Plane PlaneFromNormal(Vector3D _n)
{
	Plane plane;
	plane.defineFromNormalVector(_n, Vector3D(0.0f, 0.0f, 0.0f));
	return plane;
}

// create a plane parallel to _d1 and _d2 that passes through _point_in_plane
inline Plane PlaneFromDirections(Vector3D _d1, Vector3D _d2, Vector3D _point_in_plane)
{
	Plane plane;
	plane.defineFromDirectionVectors(_d1, _d2, _point_in_plane);
	return plane;
}

// create a plane parallel to _d1 and _d2 that passes through point (0,0,0)
inline Plane PlaneFromDirections(Vector3D _d1, Vector3D _d2)
{
	Plane plane;
	plane.defineFromDirectionVectors(_d1, _d2, Vector3D(0.0f, 0.0f, 0.0f));
	return plane;
}

#endif

