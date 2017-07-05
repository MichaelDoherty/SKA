//-----------------------------------------------------------------------------
// MotionDescriptors project - Builds with SKA Version 4.0
//-----------------------------------------------------------------------------
// QMathTest.cpp
//    Temporary code to validate rotational transformations between
//    Euler angles, rotational transform matrices and quaternions.
//    This is being used to develop code for extracting joint angles and 
//    angular velocities as quaternions.
//-----------------------------------------------------------------------------
// SKA configuration.
#include <Core/SystemConfiguration.h>
// C/C++ libraries
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;
// SKA modules
#include <Math/Quaternion.h>

ofstream ofs;

static void test1(Vector3D a0)
{
	Quaternion q1, q2, q3;
	Matrix4x4 m1, m3;
	Vector3D a1, a2, a3;

	// test 1
	m1 = Matrix4x4::rotationZXY(a0);
	m1.factorEulerXYZ(a1.pitch, a1.yaw, a1.roll);

	// test 2
	q2.fromEuler(a0);
	q2.toEuler(a2);

	// test 3
	q3.fromRotationMatrix(m1);
	q3.toRotationMatrix(m3);
	m3.factorEulerXYZ(a3.pitch, a3.yaw, a3.roll);

	ofs << "a0: " << a0 << endl;
	ofs << "m1: " << m1;
	ofs << "a1: " << a1 << endl;
	ofs << "q2: " << q2 << endl;
	ofs << "a2: " << a2 << endl;
	ofs << "q3: " << q3 << endl;
	ofs << "m3: " << m3;
	ofs << "a3: " << a3 << endl;
	ofs << endl;
}

static void test2(Vector3D& a0)
{
	Matrix4x4 m1, m2;
	Vector3D a1, a2;
	m1 = Matrix4x4::rotationXYZ(a0.x, a0.y, a0.z);
	m1.factorEulerXYZ(a1.x, a1.y, a1.z);
	
	// send result back through to see if it is equivalent
	m2 = Matrix4x4::rotationXYZ(a1.x, a1.y, a1.z);
	m2.factorEulerXYZ(a2.x, a2.y, a2.z);

	ofs << "a0: " << a0 << endl;
	ofs << "m1: " << m1;
	ofs << "a1: " << a1 << endl;
	ofs << "m2: " << m2;
	ofs << "a2: " << a2 << endl;
	ofs << endl;
}

static float randradian()
{
	int r = rand();
	r = r % 7200;
	r = r - 3600;
	float x = r / 10.0f;
	return deg2rad(x);
}

// CMU ASF file indicates Euler angle order is XYZ = PYR

void QMathTest()
{
	ofs.open("QMathTest.txt");
	/*
	float r, p, y; // Euler roll (rz), pitch (rx), yaw (ry)
	y = p = r = 0.0f;

	// <p,y,r>
	Vector3D a1(deg2rad(30.0f), deg2rad(0.0f), deg2rad(0.0f));
	Vector3D a2(deg2rad(0.0f), deg2rad(30.0f), deg2rad(0.0f));
	Vector3D a3(deg2rad(0.0f), deg2rad(0.0f), deg2rad(30.0f));
	Vector3D a4(deg2rad(-30.0f), deg2rad(0.0f), deg2rad(0.0f));
	Vector3D a5(deg2rad(0.0f), deg2rad(-30.0f), deg2rad(0.0f));
	Vector3D a6(deg2rad(0.0f), deg2rad(0.0f), deg2rad(-30.0f));

	test1(a1);
	test1(a2);
	test1(a3);
	test1(a4);
	test1(a5);
	test1(a6);
	*/

	for (int i = 0; i < 10; i++)
	{
		test1(Vector3D(randradian(), 0.0f, 0.0f));
		test1(Vector3D(0.0f, randradian(), 0.0f));
		test1(Vector3D(0.0f, 0.0f, randradian()));
	}


	ofs.close();
}

/* Latest from Eberly's Geometric Tools (July 2017) 

template <int N, typename Real>
void Rotation<N, Real>::Convert(Matrix<N, N, Real> const& r,
	Quaternion<Real>& q)
{
	static_assert(N == 3 || N == 4, "Dimension must be 3 or 4.");

	Real r22 = r(2, 2);
	if (r22 <= (Real)0)  // x^2 + y^2 >= z^2 + w^2
	{
		Real dif10 = r(1, 1) - r(0, 0);
		Real omr22 = (Real)1 - r22;
		if (dif10 <= (Real)0)  // x^2 >= y^2
		{
			Real fourXSqr = omr22 - dif10;
			Real inv4x = ((Real)0.5) / sqrt(fourXSqr);
			q[0] = fourXSqr*inv4x;
			q[1] = (r(0, 1) + r(1, 0))*inv4x;
			q[2] = (r(0, 2) + r(2, 0))*inv4x;
#if defined(GTE_USE_MAT_VEC)
			q[3] = (r(2, 1) - r(1, 2))*inv4x;
#else
			q[3] = (r(1, 2) - r(2, 1))*inv4x;
#endif
		}
		else  // y^2 >= x^2
		{
			Real fourYSqr = omr22 + dif10;
			Real inv4y = ((Real)0.5) / sqrt(fourYSqr);
			q[0] = (r(0, 1) + r(1, 0))*inv4y;
			q[1] = fourYSqr*inv4y;
			q[2] = (r(1, 2) + r(2, 1))*inv4y;
#if defined(GTE_USE_MAT_VEC)
			q[3] = (r(0, 2) - r(2, 0))*inv4y;
#else
			q[3] = (r(2, 0) - r(0, 2))*inv4y;
#endif
		}
	}
	else  // z^2 + w^2 >= x^2 + y^2
	{
		Real sum10 = r(1, 1) + r(0, 0);
		Real opr22 = (Real)1 + r22;
		if (sum10 <= (Real)0)  // z^2 >= w^2
		{
			Real fourZSqr = opr22 - sum10;
			Real inv4z = ((Real)0.5) / sqrt(fourZSqr);
			q[0] = (r(0, 2) + r(2, 0))*inv4z;
			q[1] = (r(1, 2) + r(2, 1))*inv4z;
			q[2] = fourZSqr*inv4z;
#if defined(GTE_USE_MAT_VEC)
			q[3] = (r(1, 0) - r(0, 1))*inv4z;
#else
			q[3] = (r(0, 1) - r(1, 0))*inv4z;
#endif
		}
		else  // w^2 >= z^2
		{
			Real fourWSqr = opr22 + sum10;
			Real inv4w = ((Real)0.5) / sqrt(fourWSqr);
#if defined(GTE_USE_MAT_VEC)
			q[0] = (r(2, 1) - r(1, 2))*inv4w;
			q[1] = (r(0, 2) - r(2, 0))*inv4w;
			q[2] = (r(1, 0) - r(0, 1))*inv4w;
#else
			q[0] = (r(1, 2) - r(2, 1))*inv4w;
			q[1] = (r(2, 0) - r(0, 2))*inv4w;
			q[2] = (r(0, 1) - r(1, 0))*inv4w;
#endif
			q[3] = fourWSqr*inv4w;
		}
	}
}

template <int N, typename Real>
void Rotation<N, Real>::Convert(Quaternion<Real> const& q, Matrix<N, N, Real>& r)
{
	static_assert(N == 3 || N == 4, "Dimension must be 3 or 4.");

	r.MakeIdentity();

	Real twoX = ((Real)2)*q[0];
	Real twoY = ((Real)2)*q[1];
	Real twoZ = ((Real)2)*q[2];
	Real twoXX = twoX*q[0];
	Real twoXY = twoX*q[1];
	Real twoXZ = twoX*q[2];
	Real twoXW = twoX*q[3];
	Real twoYY = twoY*q[1];
	Real twoYZ = twoY*q[2];
	Real twoYW = twoY*q[3];
	Real twoZZ = twoZ*q[2];
	Real twoZW = twoZ*q[3];

#if defined(GTE_USE_MAT_VEC)
	r(0, 0) = (Real)1 - twoYY - twoZZ;
	r(0, 1) = twoXY - twoZW;
	r(0, 2) = twoXZ + twoYW;
	r(1, 0) = twoXY + twoZW;
	r(1, 1) = (Real)1 - twoXX - twoZZ;
	r(1, 2) = twoYZ - twoXW;
	r(2, 0) = twoXZ - twoYW;
	r(2, 1) = twoYZ + twoXW;
	r(2, 2) = (Real)1 - twoXX - twoYY;
#else
	r(0, 0) = (Real)1 - twoYY - twoZZ;
	r(1, 0) = twoXY - twoZW;
	r(2, 0) = twoXZ + twoYW;
	r(0, 1) = twoXY + twoZW;
	r(1, 1) = (Real)1 - twoXX - twoZZ;
	r(2, 1) = twoYZ - twoXW;
	r(0, 2) = twoXZ - twoYW;
	r(1, 2) = twoYZ + twoXW;
	r(2, 2) = (Real)1 - twoXX - twoYY;
#endif
}
*/