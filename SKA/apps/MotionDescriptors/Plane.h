//-----------------------------------------------------------------------------
// PTAnalysis project - Builds with SKA Version 4.0
//-----------------------------------------------------------------------------
// Plane.h
//    Plane class for modeling geometrical planes in 3D space.
//-----------------------------------------------------------------------------
#ifndef PLANE_DOT_H
#define PLANE_DOT_H

#include <Math/Vector3D.h>
#include <Math/Matrix4x4.h>

class Plane
{
public:
	Plane(Vector3D _v1, Vector3D _v2, Vector3D _origin)
	{
		setup(_v1, _v2, _origin);
	}

	Plane(Vector3D _v1, Vector3D _v2)
	{
		setup(_v1, _v2, Vector3D(0.0f,0.0f,0.0f));
	}

	Plane(const Plane& _p)
	{
		setup(_p.v1, _p.v2, _p.origin);
	}

	Plane& operator=(const Plane& _p)
	{
		setup(_p.v1, _p.v2, _p.origin);
	}

	void moveTo(Vector3D& _origin) { origin = _origin; }

	void rotateTo(Vector3D& _rpy)
	{
		Matrix4x4 rot_xform = Matrix4x4::rotationRPY(_rpy);
		setup(rot_xform.multVector(v1), rot_xform.multVector(v2), origin);
	}

	Vector3D getNormal() { return n; }
	Vector3D getOrigin() { return origin; }

	Vector3D projectVectorOntoPlane(Vector3D _v)
	{
		// projection of v on plane is v - vn, where vn is the projection of v on the plane normal
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
		float x0 = origin.x, y0 = origin.y, z0 = origin.z;
		float i0 = _p.x, j0 = _p.y, k0 = _p.z;
		float a = n.x, b = n.y, c = n.z;
		float t = (a*(x0 - i0) + b*(y0 - j0) + c*(z0 - k0)) / (a*a + b*b + c*c);
		Vector3D intersection(i0 + a*t, j0 + b*t, k0 + c*t);
		return intersection;
	}


private:
	// Does the plane need an anchor point?
	// If it is just used for projections it may not need an anchor point.
	// If it is used for display, it will need an anchor point.

	// v1 and v2 lie in the plane. n is perpendicular to the plane.
	Vector3D v1, v2, n;
	Vector3D origin;

	void setup(Vector3D _v1, Vector3D _v2, Vector3D _origin)
	{
		origin = _origin;
		v1 = _v1.normalize();
		v2 = _v2.normalize();
		n = v1.cross(v2);
		n = n.normalize();
	}
};

// X = left
// Y = up
// Z = forward

// X-Y plane divides front/back
inline Plane CoronalPlane()
{
	Plane p(Vector3D(1.0f, 0.0f, 0.0f), Vector3D(0.0f, 1.0f, 0.0f));
	return p;
}

// Y-Z plane divides left/right
inline Plane SagittalPlane()
{
	Plane p(Vector3D(0.0f, 1.0f, 0.0f), Vector3D(0.0f, 0.0f, 1.0f));
	return p;
}

// Z-X plane divides top/bottom
inline Plane TransversePlane()
{
	Plane p(Vector3D(0.0f, 0.0f, 1.0f), Vector3D(1.0f, 0.0f, 0.0f));
	return p;
}

#endif

