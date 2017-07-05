//-----------------------------------------------------------------------------
// Matrix4x4.cpp
//    4x4 float matrix, primarily intended for homogeneous transformations.
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
// Some code in this file is derived from the works attributed below.
// Use of this code may be restricted by their ownership.
//-----------------------------------------------------------------------------

#include <Core/SystemConfiguration.h>
#include <Math/Matrix4x4.h>
#include <Math/Quaternion.h>

ostream& operator<<(ostream& out, Matrix4x4& m)
{
	out << "Matrix[" 
		<< m.m[0] << "," << m.m[4] << "," << m.m[8] << "," << m.m[12] << "]" << endl; 
	out << "      [" 
		<< m.m[1] << "," << m.m[5] << "," << m.m[9] << "," << m.m[13] << "]" << endl; 
	out << "      [" 
		<< m.m[2] << "," << m.m[6] << "," << m.m[10] << "," << m.m[14] << "]" << endl; 
	out << "      [" 
		<< m.m[3] << "," << m.m[7] << "," << m.m[11] << "," << m.m[15] << "]" << endl; 
	return out;
}

//-----------------------------------------------------------------------------
// Matrix4x4::rotationFromQuaternion() is based on code supplied with 
// Essential Mathematics for Games & Interactive Applications
//   by James M. Van Verth and Lars M. Bishop
//   Morgan-Kaufmann / Elsevier, 2004
//   ISBN 1-55860-863-X
// Original code: Copyright (C) 2004 by Elsevier, Inc. All rights reserved.
//----------------------------------------------------------------------------

Matrix4x4 Matrix4x4::rotationFromQuaternion(const Quaternion& q)
{
	if (!q.isUnit())
		throw MathException("non-unit quaternion passed to Matrix4x4::rotationFromQuaternion()");

	Matrix4x4 m;
    float xs, ys, zs, wx, wy, wz, xx, xy, xz, yy, yz, zz;

    xs = q.x+q.x;   
    ys = q.y+q.y;
    zs = q.z+q.z;
    wx = q.w*xs;
    wy = q.w*ys;
    wz = q.w*zs;
    xx = q.x*xs;
    xy = q.x*ys;
    xz = q.x*zs;
    yy = q.y*ys;
    yz = q.y*zs;
    zz = q.z*zs;

    m.m[0] = 1.0f - (yy + zz);
    m.m[4] = xy - wz;
    m.m[8] = xz + wy;
    m.m[12] = 0.0f;
    
    m.m[1] = xy + wz;
    m.m[5] = 1.0f - (xx + zz);
    m.m[9] = yz - wx;
    m.m[13] = 0.0f;
    
    m.m[2] = xz - wy;
    m.m[6] = yz + wx;
    m.m[10] = 1.0f - (xx + yy);
    m.m[14] = 0.0f;

    m.m[3] = 0.0f;
    m.m[7] = 0.0f;
    m.m[11] = 0.0f;
    m.m[15] = 1.0f;

    return m;
}

// The Matrix4x4::rotation???() series of methods create rotation 
// transformations from Euler angles applied in various orders.
// Note that since tranform application is from the right, 
// the specified order is reversed from the applied order.
// For example, rotationZYX() will be the transfrom Rx*Ry*Rz.
// Math is adapted from 
//   Euler Angle Formulas, David Eberly, Geometric Tools, LLC
//   http ://www.geometrictools.com/
//   Copyright 1998 - 2016. All Rights Reserved.
//   Created : December 1, 1999
//   Last Modified : March 10, 2014
// rx = rotation around x-axis (pitch)
// ry = rotation around y-axis (yaw)
// rz = rotation arounf z-axis (roll)

//	rotationXYZ = Rz*Ry*Rx 
Matrix4x4 Matrix4x4::rotationXYZ(float rx, float ry, float rz)
{
	Matrix4x4 M;
	float sx = sin(rx), sy = sin(ry), sz = sin(rz);
	float cx = cos(rx), cy = cos(ry), cz = cos(rz);

	M.m[0] = cy*cz; M.m[4] = cz*sx*sy - cx*sz; M.m[8] = cx*cz*sy + sx*sz;
	M.m[1] = cy*sz; M.m[5] = cx*cz + sx*sy*sz; M.m[9] = -cz*sx + cx*sy*sz;
	M.m[2] = -sy;   M.m[6] = cy*sx;            M.m[10] = cx*cy;

	M.m[3] = 0.0f; M.m[7] = 0.0f; M.m[11] = 0.0f;
	M.m[12] = 0.0f;	M.m[13] = 0.0f;	M.m[14] = 0.0f;	M.m[15] = 1.0f;
	return M;
}
//	rotationXZY = Ry*Rz*Rx 
Matrix4x4 Matrix4x4::rotationXZY(float rx, float ry, float rz)
{
	Matrix4x4 M;
	float sx = sin(rx), sy = sin(ry), sz = sin(rz);
	float cx = cos(rx), cy = cos(ry), cz = cos(rz);

	M.m[0] = cy*cz;  M.m[4] = sx*sy - cx*cy*sz; M.m[8] = cx*sy + cy*sx*sz;
	M.m[1] = sz;     M.m[5] = cx*cz;            M.m[9] = -cz*sx;
	M.m[2] = -cz*sy; M.m[6] = cy*sx + cx*sy*sz; M.m[10] = cx*cy - sx*sy*sz;

	M.m[3] = 0.0f; M.m[7] = 0.0f; M.m[11] = 0.0f;
	M.m[12] = 0.0f;	M.m[13] = 0.0f;	M.m[14] = 0.0f;	M.m[15] = 1.0f;
	return M;
}
//	rotationYXZ = Rz*Rx*Ry 
Matrix4x4 Matrix4x4::rotationYXZ(float rx, float ry, float rz)
{
	Matrix4x4 M;
	float sx = sin(rx), sy = sin(ry), sz = sin(rz);
	float cx = cos(rx), cy = cos(ry), cz = cos(rz);

	M.m[0] = cy*cz - sx*sy*sz; M.m[4] = -cx*sz; M.m[8] = cz*sy + cy*sx*sz;
	M.m[1] = cz*sx*sy + cy*sz; M.m[5] = cx*cz;  M.m[9] = -cy*cz*sx + sy*sz;
	M.m[2] = -cx*sy;           M.m[6] = sx;     M.m[10] = cx*cy;

	M.m[3] = 0.0f; M.m[7] = 0.0f; M.m[11] = 0.0f;
	M.m[12] = 0.0f;	M.m[13] = 0.0f;	M.m[14] = 0.0f;	M.m[15] = 1.0f;
	return M;
}
//	rotationYZX = Rz*Ry*Rx 
Matrix4x4 Matrix4x4::rotationYZX(float rx, float ry, float rz)
{
	Matrix4x4 M;
	float sx = sin(rx), sy = sin(ry), sz = sin(rz);
	float cx = cos(rx), cy = cos(ry), cz = cos(rz);

	M.m[0] = cy*cz; M.m[4] = cz*sx*sy - cx*sz; M.m[8] = cx*cz*sy + sx*sz;
	M.m[1] = cy*sz; M.m[5] = cx*cz + sx*sy*sz; M.m[9] = -cz*sx + cx*sy*sz;
	M.m[2] = -sy;   M.m[6] = cy*sx;            M.m[10] = cx*cy;

	M.m[3] = 0.0f; M.m[7] = 0.0f; M.m[11] = 0.0f;
	M.m[12] = 0.0f;	M.m[13] = 0.0f;	M.m[14] = 0.0f;	M.m[15] = 1.0f;
	return M;
}
//	rotationZXY = Ry*Rx*Rz 
Matrix4x4 Matrix4x4::rotationZXY(float rx, float ry, float rz)
{
	Matrix4x4 M;
	float sx = sin(rx), sy = sin(ry), sz = sin(rz);
	float cx = cos(rx), cy = cos(ry), cz = cos(rz);

	M.m[0] = cy*cz + sx*sy*sz;  M.m[4] = cz*sx*sy - cy*sz; M.m[8] = cx*sy;
	M.m[1] = cx*sz;             M.m[5] = cx*cz;            M.m[9] = -sx;
	M.m[2] = -cz*sy + cy*sx*sz; M.m[6] = cy*cz*sx + sy*sz; M.m[10] = cx*cy;

	M.m[3] = 0.0f; M.m[7] = 0.0f; M.m[11] = 0.0f;
	M.m[12] = 0.0f;	M.m[13] = 0.0f;	M.m[14] = 0.0f;	M.m[15] = 1.0f;
	return M;
}
//	rotationZYX = Rx*Ry*Rz 
Matrix4x4 Matrix4x4::rotationZYX(float rx, float ry, float rz)
{
	Matrix4x4 M;
	float sx = sin(rx), sy = sin(ry), sz = sin(rz);
	float cx = cos(rx), cy = cos(ry), cz = cos(rz);

	M.m[0] = cy*cz;             M.m[4] = -cy*sz;           M.m[8] = sy;
	M.m[1] = cz*sx*sy + cx*sz;  M.m[5] = cx*cz - sx*sy*sz; M.m[9] = -cy*sx;
	M.m[2] = -cx*cz*sy + sx*sz; M.m[6] = cz*sx + cx*sy*sz; M.m[10] = cx*cy;

	M.m[3] = 0.0f; M.m[7] = 0.0f; M.m[11] = 0.0f;
	M.m[12] = 0.0f;	M.m[13] = 0.0f;	M.m[14] = 0.0f;	M.m[15] = 1.0f;
	return M;
}

//-----------------------------------------------------------------------------
// Matrix4x4::factorEuler???() is a set of methods for factoring a
//   rotation matrix into Euler angles.
// Math is adapted from 
//   Euler Angle Formulas, David Eberly, Geometric Tools, LLC
//   http ://www.geometrictools.com/
//   Copyright 1998 - 2016. All Rights Reserved.
//   Created : December 1, 1999
//   Last Modified : March 10, 2014
// For all these functions, a false return value indicates that the
//   factorization was not unique.
//----------------------------------------------------------------------------
// mapping Eberly's matrix elements to Matrix4x4 elements:
// r00 r01 r02   m[0] m[4] m[8]
// r10 r11 r12   m[1] m[5] m[9]
// r20 r21 r22   m[2] m[6] m[10]
//----------------------------------------------------------------------------

// factor into Rz*Ry*Rx
bool Matrix4x4::factorEulerXYZ(float& rx, float& ry, float& rz)
{
	if (m[2] < 1.0f) {
		if (m[2] > -1.0f) {
			ry = asin(-m[2]);
			rz = atan2(m[1], m[0]);
			rx = atan2(m[6], m[10]);
			return true;
		}
		else { // m[2] == -1
			// not a unique solution: rx-rz = atan2(-m[9],m[5])
			ry = HALF_PI;
			rz = -atan2(-m[9], m[5]);
			rx = 0.0f;
			return false;
		}
	}
	else { // m[2] == +1
		// not a unique solution: rx+rz = atan2(-m[9],m[5])
		ry = -HALF_PI;
		rz = -atan2(-m[9], m[5]);
		rx = 0.0f;
		return false;
	}
}

// factor into Ry*Rz*Rx
bool Matrix4x4::factorEulerXZY(float& rx, float& ry, float& rz)
{
	if (m[1] < 1.0f) {
		if (m[1] > -1.0f) {
			rz = asin(m[1]);
			ry = atan2(-m[2], m[0]);
			rx = atan2(-m[9], m[5]);
			return true;
		}
		else { // m[1] == -1
			// not a unique solution: rx-ry = atan2(m[6],m[10])
			rz = -HALF_PI;
			ry = -atan2(m[6], m[10]);
			rx = 0.0f;
			return false;
		}
	}
	else { // m[1] == +1
		// not a unique solution: rx+ry = atan2(m[6],m[10])
		rz = HALF_PI;
		ry = atan2(m[6], m[10]);
		rx = 0.0f;
		return false;
	}
}

// factor into Rz*Rx*Ry
bool Matrix4x4::factorEulerYXZ(float& rx, float& ry, float& rz)
{
	if (m[6] < 1.0f) {
		if (m[6] > -1.0f) {
			rx = asin(m[6]);
			rz = atan2(-m[4], m[5]);
			ry = atan2(-m[2], m[10]);
			return true;
		}
		else { // m[6] == -1
			// not a unique solution: ry-rz = atan2(m[8], m[0])
			rx = -HALF_PI;
			rz = -atan2(m[8], m[0]);
			ry = 0.0f;
			return false;
		}
	}
	else { // m[6] == +1
		// not a unique solution: ry+rz = atan2(m[8], m[0])
		rx = HALF_PI;
		rz = atan2(m[8], m[0]);
		ry = 0.0f;;
		return false;
	}
}

// factor into Rx*Rz*Ry
bool Matrix4x4::factorEulerYZX(float& rx, float& ry, float& rz)
{
	if (m[4] < 1.0f) {
		if (m[4] > -1.0f) {
			rz = asin(-m[4]);
			rx = atan2(m[6], m[5]);
			ry = atan2(m[8], m[0]);
			return true;
		}
		else { // m[4] == -1
			// not a unique solution: ry-rx = atan2(-m[2], m[10])
			rz = HALF_PI;
			rx = -atan2(-m[2], m[10]);
			ry = 0.0f;
			return false;
		}
	}
	else { // m[4] == +1
		// not a unique solution: ry+rx = atan2(-m[2], m[10])
		rz = -HALF_PI;
		rx = atan2(-m[2], m[10]);
		ry = 0.0f;;
		return false;
	}
}

// factor into Ry*Rx*Rz
bool Matrix4x4::factorEulerZXY(float& rx, float& ry, float& rz)
{
	if (m[9] < 1.0f) {
		if (m[9] > -1.0f) {
			rx = asin(-m[9]);
			ry = atan2(m[8], m[10]);
			rz = atan2(m[1], m[5]);
			return true;
		}
		else { // m[9] == -1
			// not a unique solution: rz-ry = atan2(-m[4], m[0])
			rx = HALF_PI;
			ry = -atan2(-m[4], m[0]);
			rz = 0.0f;
			return false;
		}
	}
	else { // m[9] == +1
		// not a unique solution: rz+ry = atan2(-m[4], m[0])
		rx = -HALF_PI;
		ry = atan2(-m[4], m[0]);
		rz = 0.0f;;
		return false;
	}
}

// factor into Rx*Ry*Rz
bool Matrix4x4::factorEulerZYX(float& rx, float& ry, float& rz)
{
	if (m[8] < 1.0f) {
		if (m[8] > -1.0f) {
			ry = asin(m[8]);
			rx = atan2(-m[9], m[10]);
			rz = atan2(-m[4], m[0]);
			return true;
		}
		else { // m[8] == -1
			 // not a unique solution: rz-rx = atan2(m[1], m[5]);
			ry = -HALF_PI;
			rx = -atan2(m[1], m[5]);
			rz = 0.0f;
			return false;
		}
	}
	else { // m[8] == +1
		// not a unique solution: rz+rx = atan2(m[1], m[5]);
		ry = HALF_PI;
		rx = atan2(m[1], m[5]);
		rz = 0.0f;
		return false;
	}
}
