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
// Version 3.1 - September 1, 2014 - Michael Doherty
//-----------------------------------------------------------------------------
#include <Core/SystemConfiguration.h>
#include <Math/Matrix4x4.h>
#include <Math/Quaternion.h>

//-----------------------------------------------------------------------------
// Method Matrix4x4::rotationFromQuaternion() is based on code supplied with 
// Essential Mathematics for Games & Interactive Applications
//   by James M. Van Verth and Lars M. Bishop
//   Morgan-Kaufmann / Elsevier, 2004
//   ISBN 1-55860-863-X
// Original code: Copyright (C) 2004 by Elsevier, Inc. All rights reserved.
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

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

//----------------------------------------------------------------------------
// toEulerAnglesFrom???() is a set of methods for factoring a
//   rotation matrix into Euler angles. Based on section 2.4 in
//   _3D Game Engine Design_ by David H. Eberly
//   Copyright (c) 2000, 2001.  All Rights Reserved
// For all these functions, a false return value indicates that the
//   factorization was not unique.
//----------------------------------------------------------------------------

bool Matrix4x4::toEulerAnglesFromXYZ(float& rx, float& ry, float& rz)
{
	if (m[8] <= -1)
	{
		rx = -atan2f(m[1],m[5]);
		ry = -HALF_PI;
		rz = 0.0f;
		return false;
    }
    else if (m[8] >= 1)
    {
        rx = atan2f(m[1],m[5]);
        ry = HALF_PI;
        rz = 0.0f;
        return false;
    }
	else
    {
		rx = atan2f(-m[9],m[10]);
		ry = (float)asin((double)m[8]);
		rz = atan2f(-m[4],m[0]);
		return true;
	}
}

bool Matrix4x4::toEulerAnglesFromXZY(float& rx, float& ry, float& rz)
{
	if (m[4] <= -1.0f) 
	{
		rx = atan2f(m[2],m[10]);
		rz = HALF_PI;
		ry = 0.0f;
		return false;
	}
	else if (m[4] >= 1.0f)
	{
		rx = atan2f(-m[2],m[10]);
        rz = -HALF_PI;
        ry = 0.0f;
        return false;
    }
	else
	{
		rx = atan2f(m[6],m[5]);
		rz = (float)asin(-(double)m[4]);
		ry = atan2f(m[8],m[0]);
		return true;
	}
}

bool Matrix4x4::toEulerAnglesFromYXZ(float& rx, float& ry, float& rz)
{
	if (m[9] <= -1.0f)
	{
		ry = atan2f(m[4],m[0]);
		rx = HALF_PI;
		rz = 0.0f;
		return false;
	}
	else if (m[9] >= 1.0f)
	{
		ry = atan2f(-m[4],m[0]);
		rx = -HALF_PI;
		rz = 0.0f;
		return false;
	}   
	else
	{
		ry = atan2f(m[8],m[10]);
		rx = (float)asin(-(double)m[9]);
		rz = atan2f(m[1],m[5]);
		return true;
	}   
}

bool Matrix4x4::toEulerAnglesFromYZX(float& rx, float& ry, float& rz)
{
	if (m[1] <= -1.0f)
	{
		ry = -atan2f(m[6],m[10]);
		rz = -HALF_PI;
		rx = 0.0f;
		return false;
    }
    else if (m[1] >= 1.0f)
    {
		ry = atan2f(m[6],m[10]);
		rz = HALF_PI;
		rx = 0.0f;
		return false;
	}
	else
	{
		ry = atan2f(-m[2],m[0]);
		rz = (float)asin((double)m[1]);
		rx = atan2f(-m[9],m[5]);
		return true;
	}
}

bool Matrix4x4::toEulerAnglesFromZXY(float& rx, float& ry, float& rz)
{
	if (m[6] <= -1.0f)
	{
		rz = -atan2f(m[8],m[0]);
		rx = -HALF_PI;
		ry = 0.0f;
		return false;
	}
    else if (m[6] >= 1.0f)
    {
		rz = atan2f(m[8],m[0]);
		rx = HALF_PI;
		ry = 0.0f;
		return false;
    }
	else
	{
		rz = atan2f(-m[4],m[5]);
		rx = (float)asin((double)m[6]);
		ry = atan2f(-m[2],m[10]);
		return true;
	}
}

bool Matrix4x4::toEulerAnglesFromZYX(float& rx, float& ry, float& rz)
{
	if (m[2] <= -1.0f)
	{
		rz = -atan2f(m[4],m[8]);
		ry = HALF_PI;
		rx = 0.0f;
		return false;
	}
	else if (m[2] >= 1.0f)
	{
		rz = atan2f(-m[4],-m[8]);
		ry = -HALF_PI;
		rx = 0.0f;
		return false;
	}
	else
	{
		rz = atan2f(m[1],m[0]);
		ry = (float)asin(-(double)m[2]);
		rx = atan2f(m[6],m[10]);
		return true;
	} 
}
