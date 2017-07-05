//-----------------------------------------------------------------------------
// Matrix4x4.h
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

#ifndef MATRIX4x4_DOT_H
#define MATRIX4x4_DOT_H
#include <Core/SystemConfiguration.h>
#include <cmath>
#include <fstream>
using namespace std;
#include <Math/Math.h>
#include <Math/Vector3D.h>

class SKA_LIB_DECLSPEC Quaternion;

class SKA_LIB_DECLSPEC Matrix4x4
{
	SKA_LIB_DECLSPEC friend ostream& operator<<(ostream& out, Matrix4x4& m);

public:
	// column major storage
	//  0  4  8 12    [0,0] [0,1] [0,2] [0,3]
	//  1  5  9 13    [1,0] [1,1] [1,2] [1,3]    
	//  2  6 10 14    [2,0] [2,1] [2,2] [2,3]
	//  3  7 11 15    [3,0] [3,1] [3,2] [3,3]

	float m[16];

	// default constructor - all zeros
	Matrix4x4() 
	{ 
		memset(m, 0, 16*sizeof(float)); 
	}
	// copy constructor
	Matrix4x4(const Matrix4x4& _m) 
	{ 
		memcpy(m, _m.m, 16*sizeof(float)); 
	}
	// assignment operator
	Matrix4x4& operator= (const Matrix4x4& _m) 
	{ 
		memcpy(m, _m.m, 16*sizeof(float)); 
		return *this; 
	}
	// destructor - nothing to do
	virtual ~Matrix4x4() { }

	// access with row, column indexes
	float& operator()(unsigned int r, unsigned int c)
	{
		return m[r + 4*c];
	}

	float operator()(unsigned int r, unsigned int c) const
	{
		return m[r + 4*c];
	}

	static Matrix4x4 zero() 
	{ 
		Matrix4x4 M;
		return M;
	}

	// create new identity matrix
	static Matrix4x4 identity() 
	{ 
		Matrix4x4 M;
		M.m[0] = M.m[5] = M.m[10] = M.m[15] = 1.0f; 
		return M;
	}

	// Create new matrix that is transpose of this matrix
	Matrix4x4 transpose()
	{
		Matrix4x4 M;
		M.m[0]  = m[0]; M.m[1]  = m[4]; M.m[2]  = m[8];  M.m[3]  = m[12];
		M.m[4]  = m[1]; M.m[5]  = m[5]; M.m[6]  = m[9];  M.m[7]  = m[13];
		M.m[8]  = m[2]; M.m[9]  = m[6]; M.m[10] = m[10]; M.m[11] = m[14];
		M.m[12] = m[3]; M.m[13] = m[7]; M.m[14] = m[11]; M.m[15] = m[15];
		return M;
	}

	// FIXIT - needs a more appropriate name
	Matrix4x4 cheapInverse(bool check=false)
	{
		Matrix4x4 mout = transpose();
		if (check && !testIfInverse(mout))
			throw MathException("Math: Use of Transpose as Inverse Failed");
		return mout;
	}

	// create new scaling transformation matrix
	static Matrix4x4 scaleXYZ(float x, float y, float z)	
	{
		Matrix4x4 M = Matrix4x4::identity();
		M.m[0] = x;	M.m[5] = y;	M.m[10] = z;
		return M;
	}

	static Matrix4x4 scaleXYZ(Vector3D v)	
	{	return Matrix4x4::scaleXYZ(v.x, v.y, v.z);	}

	// FIXIT! make another one that accepts a vector or array
	// create new translation transformation matrix
	static Matrix4x4 translationXYZ(float x, float y, float z)	
	{
		Matrix4x4 M = Matrix4x4::identity();
		M.m[12] = x; M.m[13] = y; M.m[14] = z;
		return M;
	}
	static Matrix4x4 translationXYZ(Vector3D v)	
	{	return Matrix4x4::translationXYZ(v.x, v.y, v.z);	}

	// The following series of methods create rotation transformations
	// from Euler angles applied in various orders.
	// Note that since tranform application is from the right, 
	// the specified order is reversed from the applied order.
	// For example, rotationZYX() will be the transfrom Rx*Ry*Rz.

	//	rotationXYZ = Rz*Ry*Rx 
	static Matrix4x4 rotationXYZ(float rx, float ry, float rz);
	//	rotationXZY = Ry*Rz*Rx 
	static Matrix4x4 rotationXZY(float rx, float ry, float rz);
	//	rotationYXZ = Rz*Rx*Ry 
	static Matrix4x4 rotationYXZ(float rx, float ry, float rz);
	//	rotationYZX = Rz*Ry*Rx 
	static Matrix4x4 rotationYZX(float rx, float ry, float rz);
	//	rotationZXY = Ry*Rx*Rz 
	static Matrix4x4 rotationZXY(float rx, float ry, float rz);
	//	rotationZYX = Rx*Ry*Rz 
	static Matrix4x4 rotationZYX(float rx, float ry, float rz);

	static Matrix4x4 rotationXYZ(const Vector3D& v) { return Matrix4x4::rotationXYZ(v.x, v.y, v.z); }
	static Matrix4x4 rotationXZY(const Vector3D& v) { return Matrix4x4::rotationXZY(v.x, v.y, v.z); }
	static Matrix4x4 rotationYXZ(const Vector3D& v) { return Matrix4x4::rotationYXZ(v.x, v.y, v.z); }
	static Matrix4x4 rotationYZX(const Vector3D& v) { return Matrix4x4::rotationYZX(v.x, v.y, v.z); }
	static Matrix4x4 rotationZXY(const Vector3D& v) { return Matrix4x4::rotationZXY(v.x, v.y, v.z); }
	static Matrix4x4 rotationZYX(const Vector3D& v) { return Matrix4x4::rotationZYX(v.x, v.y, v.z); }

	//   1   0   0
	//   0  cc -sx
	//   0  sx  cx
	static Matrix4x4 rotationX(float rx)
	{
		Matrix4x4 M = Matrix4x4::identity();
		float cx = cos(rx), sx = sin(rx);
		M.m[5]  =  cx; M.m[9]  = -sx;
		M.m[6]  =  sx; M.m[10] =  cx;
		return M;
	}
	static Matrix4x4 rotationPitch(float pitch) { return rotationX(pitch); }

	//  cy   0  sy
	//   0   1   0
	// -sy   0  cy	
	static Matrix4x4 rotationY(float ry)
	{
		Matrix4x4 M = Matrix4x4::identity();
		float cy = cos(ry), sy = sin(ry);
		M.m[0]  =  cy; M.m[8]  =  sy;
		M.m[2]  = -sy; M.m[10] =  cy;
		return M;
	}
	static Matrix4x4 rotationYaw(float yaw) { return rotationY(yaw); }

	//  cz -sz   0
	//  sz  cz   0
	//   0   0   1	
	static Matrix4x4 rotationZ(float rz)
	{
		Matrix4x4 M = Matrix4x4::identity();
		float cz = cos(rz), sz = sin(rz);
		M.m[0]  =  cz; M.m[4]  = -sz;
		M.m[1]  =  sz; M.m[5]  =  cz;
		return M;
	}
	static Matrix4x4 rotationRoll(float roll) { return rotationZ(roll); }

	// Factor Euler angles from a rotation transformation matrix.
	// These methods assume the processed matrix is a pure rotation transformation.
	// These methods are the inverse of the rotation??? methods.
	// For example, rotationZYX() created transfrom Rx*Ry*Rz 
	//  and factorEulerZYX() will factor such that M = Rx*Ry*Rz. 
	// False return values indicate that the factorization was not unique.
	bool factorEulerXYZ(float& rx, float& ry, float& rz);
	bool factorEulerXZY(float& rx, float& ry, float& rz);
	bool factorEulerYXZ(float& rx, float& ry, float& rz);
	bool factorEulerYZX(float& rx, float& ry, float& rz);
	bool factorEulerZXY(float& rx, float& ry, float& rz);
	bool factorEulerZYX(float& rx, float& ry, float& rz);

	bool factorEulerXYZ(Vector3D& r) { return factorEulerXYZ(r.x, r.y, r.z); }
	bool factorEulerXZY(Vector3D& r) { return factorEulerXZY(r.x, r.y, r.z); }
	bool factorEulerYXZ(Vector3D& r) { return factorEulerYXZ(r.x, r.y, r.z); }
	bool factorEulerYZX(Vector3D& r) { return factorEulerYZX(r.x, r.y, r.z); }
	bool factorEulerZXY(Vector3D& r) { return factorEulerZXY(r.x, r.y, r.z); }
	bool factorEulerZYX(Vector3D& r) { return factorEulerZYX(r.x, r.y, r.z); }
	
	static Matrix4x4 rotationFromAxisAngle(Vector3D& axis, float angle)
	{
		Matrix4x4 M = Matrix4x4::identity();
		float ca = cos(angle);
		float sa = sin(angle);
		float t = 1.0f-ca;
		M.m[0] = ca + axis.x*axis.x*t;
		M.m[1] = axis.y*axis.x*t + axis.z*sa;
		M.m[2] = axis.z*axis.x*t - axis.y*sa;
		M.m[4] = axis.x*axis.y*t - axis.z*sa;
		M.m[5] = ca + axis.y*axis.y*t;
		M.m[6] = axis.z*axis.y*t + axis.x*sa;
		M.m[8] = axis.x*axis.z*t + axis.y*sa;
		M.m[9] = axis.y*axis.z*t - axis.x*sa;
		M.m[10] = ca + axis.z*axis.z*t;
		return M;
	}

	// Build a rotation matrix M such that v2 = Mv1
	static Matrix4x4 rotationFromVectors(Vector3D& v1, Vector3D& v2)
	{
		// CHECKIT! maybe numerical problems when v1 near v2
		if (v1 == v2) return Matrix4x4::identity();

		float v1mxv2m = v1.magnitude()*v2.magnitude();
		float angle = acos(v1.dot(v2)/v1mxv2m);
		if (fabs(angle) < EPSILON) return Matrix4x4::identity();
		Vector3D axis = v1.cross(v2)/(v1mxv2m*sin(angle));
		return Matrix4x4::rotationFromAxisAngle(axis, angle);
	}

	static Matrix4x4 rotationFromQuaternion(const Quaternion& q);

	// Multiply matrix by vertex/point
	Vector3D operator*(const Vector3D& v)
	{
		Vector3D vout;
		float x,y,z,w;
		x = m[0] *v.x + m[4] *v.y + m[8] *v.z + m[12];
		y = m[1] *v.x + m[5] *v.y + m[9] *v.z + m[13];
		z = m[2] *v.x + m[6] *v.y + m[10]*v.z + m[14];
		w = m[3] *v.x + m[7] *v.y + m[11]*v.z + m[15];
		vout.x = x/w;
		vout.y = y/w;
		vout.z = z/w;
		return vout; 
	}

	// Multiply matrix by vector/normal
	Vector3D multVector(const Vector3D& v)
	{
		Vector3D vout;
		float x,y,z,w;
		x = m[0] *v.x + m[4] *v.y + m[8] *v.z;
		y = m[1] *v.x + m[5] *v.y + m[9] *v.z;
		z = m[2] *v.x + m[6] *v.y + m[10]*v.z;
		w = m[3] *v.x + m[7] *v.y + m[11]*v.z + m[15];
		vout.x = x/w;
		vout.y = y/w;
		vout.z = z/w;
		return vout; 
	}

	Matrix4x4 operator*(const Matrix4x4& rhs)
	{
		Matrix4x4 M;
		M.m[0]  = m[0]*rhs.m[0]  + m[4]*rhs.m[1]  + m[8]*rhs.m[2]   + m[12]*rhs.m[3];
		M.m[1]  = m[1]*rhs.m[0]  + m[5]*rhs.m[1]  + m[9]*rhs.m[2]   + m[13]*rhs.m[3];
		M.m[2]  = m[2]*rhs.m[0]  + m[6]*rhs.m[1]  + m[10]*rhs.m[2]  + m[14]*rhs.m[3];
		M.m[3]  = m[3]*rhs.m[0]  + m[7]*rhs.m[1]  + m[11]*rhs.m[2]  + m[15]*rhs.m[3];

		M.m[4]  = m[0]*rhs.m[4]  + m[4]*rhs.m[5]  + m[8]*rhs.m[6]   + m[12]*rhs.m[7];
		M.m[5]  = m[1]*rhs.m[4]  + m[5]*rhs.m[5]  + m[9]*rhs.m[6]   + m[13]*rhs.m[7];
		M.m[6]  = m[2]*rhs.m[4]  + m[6]*rhs.m[5]  + m[10]*rhs.m[6]  + m[14]*rhs.m[7];
		M.m[7]  = m[3]*rhs.m[4]  + m[7]*rhs.m[5]  + m[11]*rhs.m[6]  + m[15]*rhs.m[7];
		
		M.m[8]  = m[0]*rhs.m[8]  + m[4]*rhs.m[9]  + m[8]*rhs.m[10]  + m[12]*rhs.m[11];
		M.m[9]  = m[1]*rhs.m[8]  + m[5]*rhs.m[9]  + m[9]*rhs.m[10]  + m[13]*rhs.m[11];
		M.m[10] = m[2]*rhs.m[8]  + m[6]*rhs.m[9]  + m[10]*rhs.m[10] + m[14]*rhs.m[11];
		M.m[11] = m[3]*rhs.m[8]  + m[7]*rhs.m[9]  + m[11]*rhs.m[10] + m[15]*rhs.m[11];
		
		M.m[12] = m[0]*rhs.m[12] + m[4]*rhs.m[13] + m[8]*rhs.m[14]  + m[12]*rhs.m[15];
		M.m[13] = m[1]*rhs.m[12] + m[5]*rhs.m[13] + m[9]*rhs.m[14]  + m[13]*rhs.m[15];
		M.m[14] = m[2]*rhs.m[12] + m[6]*rhs.m[13] + m[10]*rhs.m[14] + m[14]*rhs.m[15];
		M.m[15] = m[3]*rhs.m[12] + m[7]*rhs.m[13] + m[11]*rhs.m[14] + m[15]*rhs.m[15];
		return M;
	}

	Matrix4x4 operator+(const Matrix4x4& rhs)
	{
		Matrix4x4 M;
		M.m[0]  = m[0] +rhs.m[0];
		M.m[1]  = m[1] +rhs.m[1];		
		M.m[2]  = m[2] +rhs.m[2];		
		M.m[3]  = m[3] +rhs.m[3];		
		M.m[4]  = m[4] +rhs.m[4];		
		M.m[5]  = m[5] +rhs.m[5];		
		M.m[6]  = m[6] +rhs.m[6];		
		M.m[7]  = m[7] +rhs.m[7];
		M.m[8]  = m[8] +rhs.m[8];		
		M.m[9]  = m[9] +rhs.m[9];		
		M.m[10] = m[10]+rhs.m[10];		
		M.m[11] = m[11]+rhs.m[11];		
		M.m[12] = m[12]+rhs.m[12];		
		M.m[13] = m[13]+rhs.m[13];		
		M.m[14] = m[14]+rhs.m[14];
		M.m[15] = m[15]+rhs.m[15];		
		return M;
	}

// left-handed perspective projection
// equivalent to D3DXMatrixLookAtLH
//		zaxis = normal(At - Eye)
//		xaxis = normal(cross(Up, zaxis))
//		yaxis = cross(zaxis, xaxis)
//    
//		xaxis.x           yaxis.x           zaxis.x          0
//		xaxis.y           yaxis.y           zaxis.y          0
//		xaxis.z           yaxis.z           zaxis.z          0
//	   -dot(xaxis, eye)  -dot(yaxis, eye)  -dot(zaxis, eye)  1
	static Matrix4x4 lookatMatrixLeftHand(Vector3D& _eye, Vector3D& _at, Vector3D& _up)
	{
		Vector3D zaxis = _at - _eye;
		zaxis = zaxis.normalize();
		Vector3D xaxis = _up.cross(zaxis);
		xaxis = xaxis.normalize();
		Vector3D yaxis = zaxis.cross(xaxis);
		
		Matrix4x4 M;
		M.m[0]  = xaxis.x;
		M.m[1]  = xaxis.y;
		M.m[2]  = xaxis.z;
		M.m[3]  = -1.0f*xaxis.dot(_eye);
		
		M.m[4]  = yaxis.x;
		M.m[5]  = yaxis.y;
		M.m[6]  = yaxis.z;
		M.m[7]  = -1.0f*yaxis.dot(_eye);
		
		M.m[8]  = zaxis.x;
		M.m[9]  = zaxis.y;
		M.m[10] = zaxis.z;
		M.m[11] = -1.0f*zaxis.dot(_eye);
		
		M.m[12] = 0.0f;
		M.m[13] = 0.0f;
		M.m[14] = 0.0f;
		M.m[15] = 1.0f;

		return M;
	}

// right-handed perspective projection
// equivalent to D3DXMatrixLookAtRH
//		zaxis = normal(Eye - At)
//		xaxis = normal(cross(Up, zaxis))
//		yaxis = cross(zaxis, xaxis)
//    
//		-xaxis.x           yaxis.x           -zaxis.x          0
//		-xaxis.y           yaxis.y           -zaxis.y          0
//		-xaxis.z           yaxis.z           -zaxis.z          0
//		 dot(xaxis, eye)   -dot(yaxis, eye)  -dot(zaxis, eye)  1
	static Matrix4x4 lookatMatrixRightHand(Vector3D& _eye, Vector3D& _at, Vector3D& _up)
	{
		Vector3D zaxis = _eye - _at;
		zaxis = zaxis.normalize();
		Vector3D xaxis = _up.cross(zaxis);
		xaxis = xaxis.normalize();
		Vector3D yaxis = zaxis.cross(xaxis);
		
		Matrix4x4 M;
		M.m[0]  = -xaxis.x;
		M.m[1]  = -xaxis.y;
		M.m[2]  = -xaxis.z;
		M.m[3]  = 1.0f*xaxis.dot(_eye);
		
		M.m[4]  = yaxis.x;
		M.m[5]  = yaxis.y;
		M.m[6]  = yaxis.z;
		M.m[7]  = -1.0f*yaxis.dot(_eye);
		
		M.m[8]  = -zaxis.x;
		M.m[9]  = -zaxis.y;
		M.m[10] = -zaxis.z;
		M.m[11] = -1.0f*zaxis.dot(_eye);
		
		M.m[12] = 0.0f;
		M.m[13] = 0.0f;
		M.m[14] = 0.0f;
		M.m[15] = 1.0f;
		return M;
	}

// perspective projection Matrix based on field of view
// equivalent to D3DXMatrixPerspectiveFovLH:
//		xScale     0          0               0
//		0        yScale       0               0
//		0          0       zf/(zf-zn)         1
//		0          0       -zn*zf/(zf-zn)     0
//		where: 
//			yScale = cot(fovY/2)
//			xScale = yScale / aspect ratio
	static Matrix4x4 perspectiveProjectionLeftHand(
		float _fov_y, float _aspect, 
		float _z_near, float _z_far)
	{
		Matrix4x4 M;
		float yScale = cotangent(_fov_y/2.0f);
		float xScale = yScale / _aspect;
		M.m[0] = xScale;
		M.m[5] = yScale;
		M.m[10] = _z_far / (_z_far - _z_near);
		M.m[11] = -1.0f*_z_near*_z_far / (_z_far - _z_near);
		M.m[14] = 1.0f;
		return M;
	}

// perspective projection Matrix based on field of view
// equivalent to D3DXMatrixPerspectiveFovRH:
//		xScale     0          0              0
//		0        yScale       0              0
//		0        0        zf/(zn-zf)        -1
//		0        0        zn*zf/(zn-zf)      0
//		where:
//			yScale = cot(fovY/2)
//			xScale = yScale / aspect ratio
	static Matrix4x4 perspectiveProjectionRightHand(
		float _fov_y, float _aspect, 
		float _z_near, float _z_far)
	{
		Matrix4x4 M;
		float yScale = cotangent(_fov_y/2.0f);
		float xScale = yScale / _aspect;
		M.m[0] = xScale;
		M.m[5] = yScale;
		M.m[10] = _z_far / (_z_near - _z_far);
		M.m[11] = _z_near*_z_far / (_z_near - _z_far);
		M.m[14] = -1.0f;
		return M;
	}
	char* toString(char* s, int n)
	{
		sprintf(s, "Matrix4x4< %f, %f, %f, %f>\n         < %f, %f, %f, %f>\n         < %f, %f, %f, %f>\n         < %f, %f, %f, %f>", 
			m[0], m[4], m[8],  m[12],
			m[1], m[5], m[9],  m[13],		
			m[2], m[6], m[10], m[14],		
			m[3], m[7], m[11], m[15]);
		return s;
	}

	// DEBUGGING FUNCTION
	bool testIfInverse(Matrix4x4& i)
	{
		Matrix4x4 check = (*this) * i;
		return check.testIfIdentity();
	}
	
	// DEBUGGING FUNCTION
	bool testIfIdentity()
	{
		float epsilon = 0.0001f;
		if (abs(m[0] - 1.0f) > epsilon) return false;
		if (abs(m[1]) > epsilon) return false;
		if (abs(m[2]) > epsilon) return false;
		if (abs(m[3]) > epsilon) return false;

		if (abs(m[4]) > epsilon) return false;
		if (abs(m[5] - 1.0f) > epsilon) return false;
		if (abs(m[6]) > epsilon) return false;
		if (abs(m[7]) > epsilon) return false;

		if (abs(m[8]) > epsilon) return false;
		if (abs(m[9]) > epsilon) return false;
		if (abs(m[10] - 1.0f) > epsilon) return false;
		if (abs(m[11]) > epsilon) return false;

		if (abs(m[12]) > epsilon) return false;
		if (abs(m[13]) > epsilon) return false;
		if (abs(m[14]) > epsilon) return false;
		if (abs(m[15] - 1.0f) > epsilon) return false;

		return true;
	}

};



#endif
