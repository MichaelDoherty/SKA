//-----------------------------------------------------------------------------
// Quaternion.cpp
//    Quaternion class.
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
// Code in this file is based on code supplied with 
// Essential Mathematics for Games & Interactive Applications
//   by James M. Van Verth and Lars M. Bishop
//   Morgan-Kaufmann / Elsevier, 2004
//   ISBN 1-55860-863-X
// Original file name: IvQuat.{h,cpp}
// Original code: Copyright (C) 2004 by Elsevier, Inc. All rights reserved.
//-----------------------------------------------------------------------------

#include <Core/SystemConfiguration.h>
#include <Core/SystemLog.h>
#include <Math/Quaternion.h>
#include <iomanip>
using namespace std;

// Axis-angle constructor
Quaternion::Quaternion(const Vector3D& axis, float angle)
{
    set(axis, angle);
}

// To-from vector constructor
Quaternion::Quaternion(const Vector3D& from, const Vector3D& to)
{
    set(from, to);
} 

// Vector constructor
Quaternion::Quaternion(const Vector3D& vector)
{
    set(0.0f, vector.x, vector.y, vector.z);
}

// Rotation matrix constructor
Quaternion::Quaternion(const Matrix4x4& rotation)
{
    //float trace = rotation.Trace();
	float trace = rotation(0,0) + rotation(1,1) + rotation(2,2);
    if (trace > 0.0f)
    {
        float s = sqrtf(trace + 1.0f);
        w = s*0.5f;
        float recip = 0.5f/s;
        x = (rotation(2,1) - rotation(1,2))*recip;
        y = (rotation(0,2) - rotation(2,0))*recip;
        z = (rotation(1,0) - rotation(0,1))*recip;
	}
    else
    {
        unsigned int i = 0;
        if (rotation(1,1) > rotation(0,0)) i = 1;
        if (rotation(2,2) > rotation(i,i)) i = 2;
        unsigned int j = (i+1)%3;
        unsigned int k = (j+1)%3;
        float s = sqrtf( rotation(i,i) - rotation(j,j) - rotation(k,k) + 1.0f );
        (*this)[i] = 0.5f*s;
        float recip = 0.5f/s;
        w = (rotation(k,j) - rotation(j,k))*recip;
        (*this)[j] = (rotation(j,i) + rotation(i,j))*recip;
        (*this)[k] = (rotation(k,i) + rotation(i,k))*recip;
    }
} 

// Copy constructor
Quaternion::Quaternion(const Quaternion& other) : 
    w( other.w ), x( other.x ), y( other.y ), z( other.z )
{}

// Assignment operator
Quaternion& Quaternion::operator=(const Quaternion& other)
{
    if ( this == &other ) return *this;
    w = other.w;
    x = other.x;
    y = other.y;
    z = other.z;
    return *this;
}

// Text representation
ostream& operator<<(ostream& out, const Quaternion& source)
{
	char s[1000];
	sprintf(s, "[ %8.5f , [ %8.5f , %8.5f , %8.5f ] ]", source.w, source.x, source.y, source.z);
    out << s;
    return out;  
}    

// Quaternion magnitude (square root of norm)
float Quaternion::magnitude() const
{
    return sqrt( w*w + x*x + y*y + z*z );
}

// Quaternion norm
float Quaternion::norm() const
{
    return ( w*w + x*x + y*y + z*z );
}

// comparison operator
bool Quaternion::operator==( const Quaternion& other ) const
{
	if ( ::isZero( other.w - w )
		&& ::isZero( other.x - x )
		&& ::isZero( other.y - y )
		&& ::isZero( other.z - z ) )
        return true;
    return false;   
}

// Comparison operator
bool Quaternion::operator!=( const Quaternion& other ) const
{
	if ( ::isZero( other.w - w )
		|| ::isZero( other.x - x )
		|| ::isZero( other.y - y )
		|| ::isZero( other.z - z ) )
        return false;
    return true;
}

// Check for zero quat
bool Quaternion::isZero() const
{
	return ::isZero(w*w + x*x + y*y + z*z);
}

// Check for unit quat
bool Quaternion::isUnit() const
{
	return ::isZero(1.0f - (w*w + x*x + y*y + z*z));
}

// Check for identity quat
bool Quaternion::isIdentity() const
{
	return ::isZero(1.0f - w)
		&& ::isZero( x ) 
		&& ::isZero( y )
		&& ::isZero( z );
} 

// Set quaternion based on axis-angle
void Quaternion::set( const Vector3D& axis, float angle )
{
    // if axis of rotation is zero vector, just set to identity quat
    float length = axis.magnitudeSquared();
	if ( ::isZero( length ) )
    {
        identity();
        return;
    }

    // take half-angle
    angle *= 0.5f;

    float sintheta, costheta;
    sintheta = sinf(angle);
	costheta = cosf(angle);

    float scaleFactor = sintheta/sqrt( length );

    w = costheta;
    x = scaleFactor * axis.x;
    y = scaleFactor * axis.y;
    z = scaleFactor * axis.z;
}

// Set quaternion based on start and end vectors
void Quaternion::set( const Vector3D& from, const Vector3D& to )
{
    // Ensure that our vectors are unit
    if (!from.isUnit() || !to.isUnit()) 
		throw MathException("non-unit vector passed to Quaternion::set()");
    // get axis of rotation
    Vector3D axis = from.cross( to );
    // get cos of angle between vectors
    float costheta = from.dot( to );

    // if vectors are 180 degrees apart
    if ( costheta <= -1.0f )
    {
        // find orthogonal vector
        Vector3D orthoVector;
        orthoVector = orthoVector.normalize();

        w = 0.0f;
        x = orthoVector.x;
        y = orthoVector.y;
        z = orthoVector.z;

        return;
    }

    // use trig identities to get the values we want
    float factor = sqrt( 2.0f*(1.0f + costheta) );
    float scaleFactor = 1.0f/factor;

    // set values
    w = 0.5f*factor;
    x = scaleFactor*axis.x;
    y = scaleFactor*axis.y;
    z = scaleFactor*axis.z;
}

// Set quaternion based on fixed angles
void Quaternion::set( float zRotation, float yRotation, float xRotation ) 
{
    zRotation *= 0.5f;
    yRotation *= 0.5f;
    xRotation *= 0.5f;

    // get sines and cosines of half angles
    float Cx, Sx;
    Sx = sinf(xRotation);
	Cx = cosf(xRotation);

    float Cy, Sy;
	Sy = sinf(yRotation);
	Cy = cosf(yRotation);

    float Cz, Sz;
	Sz = sinf(zRotation);
	Cz = cosf(zRotation);

    // multiply it out
    w = Cx*Cy*Cz - Sx*Sy*Sz;
    x = Sx*Cy*Cz + Cx*Sy*Sz;
    y = Cx*Sy*Cz - Sx*Cy*Sz;
    z = Cx*Cy*Sz + Sx*Sy*Cx;
}

// Get axis-angle based on quaternion
void Quaternion::getAxisAngle( Vector3D& axis, float& angle )
{
    angle = 2.0f*acosf( w );
    float length = sqrt( 1.0f - w*w );
	if ( ::isZero(length) )
        axis = Vector3D(); // set axis to zero
    else
    {
        length = 1.0f/length;
        axis = Vector3D(x*length, y*length, z*length);
    }
}

// Set elements close to zero equal to zero
void Quaternion::clean()
{
	if ( ::isZero( w ) ) w = 0.0f;
	if ( ::isZero( x ) ) x = 0.0f;
	if ( ::isZero( y ) ) y = 0.0f;
	if ( ::isZero( z ) ) z = 0.0f;
}

// normalize to unit quaternion
void Quaternion::normalize()
{
    float lengthsq = w*w + x*x + y*y + z*z;

	if ( ::isZero( lengthsq ) )
    {
        zero();
    }
    else
    {
        float factor = 1.0f/sqrtf( lengthsq );
        w *= factor;
        x *= factor;
        y *= factor;
        z *= factor;
    }
}

// Compute complex conjugate
Quaternion conjugate( const Quaternion& quat ) 
{
    return Quaternion( quat.w, -quat.x, -quat.y, -quat.z );
}

// Set self to complex conjugate
const Quaternion& Quaternion::conjugate()
{
    x = -x;
    y = -y;
    z = -z;
    return *this;
}

// Compute quaternion inverse
Quaternion inverse( const Quaternion& quat )
{
    float norm = quat.w*quat.w + quat.x*quat.x + quat.y*quat.y + quat.z*quat.z;
    // if we're the zero quaternion, just return identity
    if (isZero(norm))
    {
		throw MathException("zero quaternion passed to Quaternion::inverse()");
        return Quaternion();
    }

    float normRecip = 1.0f / norm;
    return Quaternion( normRecip*quat.w, -normRecip*quat.x, -normRecip*quat.y, 
                   -normRecip*quat.z );
}

// Set self to inverse
const Quaternion& Quaternion::inverse()
{
    float norm = w*w + x*x + y*y + z*z;
    // if we're the zero quaternion, just return
	if ( ::isZero( norm ) ) return *this;

    float normRecip = 1.0f / norm;
    w = normRecip*w;
    x = -normRecip*x;
    y = -normRecip*y;
    z = -normRecip*z;

    return *this;
}

// Add quat to self and return answer
Quaternion Quaternion::operator+( const Quaternion& other ) const
{
    return Quaternion( w + other.w, x + other.x, y + other.y, z + other.z );
}

// Add quat to self, store in self
Quaternion& Quaternion::operator+=( const Quaternion& other )
{
    w += other.w;
    x += other.x;
    y += other.y;
    z += other.z;
    return *this;
}

// Subtract quat from self and return answer
Quaternion Quaternion::operator-( const Quaternion& other ) const
{
    return Quaternion( w - other.w, x - other.x, y - other.y, z - other.z );
}

// Subtract quat from self, store in self
Quaternion& Quaternion::operator-=( const Quaternion& other )
{
    w -= other.w;
    x -= other.x;
    y -= other.y;
    z -= other.z;
    return *this;
}

// Negate self and return answer
Quaternion Quaternion::operator-() const
{
    return Quaternion(-w, -x, -y, -z);
}

// Scalar multiplication
Quaternion operator*( float scalar, const Quaternion& quat )
{
    return Quaternion( scalar*quat.w, scalar*quat.x, scalar*quat.y, scalar*quat.z );
} 

// Scalar multiplication by self
Quaternion& Quaternion::operator*=( float scalar )
{
    w *= scalar;
    x *= scalar;
    y *= scalar;
    z *= scalar;
    return *this;
}

// Quaternion multiplication
Quaternion Quaternion::operator*( const Quaternion& other ) const
{
    return Quaternion( w*other.w - x*other.x - y*other.y - z*other.z,
                   w*other.x + x*other.w + y*other.z - z*other.y,
                   w*other.y + y*other.w + z*other.x - x*other.z,
                   w*other.z + z*other.w + x*other.y - y*other.x );
}

// Quaternion multiplication by self
Quaternion& Quaternion::operator*=( const Quaternion& other )
{
    set( w*other.w - x*other.x - y*other.y - z*other.z,
         w*other.x + x*other.w + y*other.z - z*other.y,
         w*other.y + y*other.w + z*other.x - x*other.z,
         w*other.z + z*other.w + x*other.y - y*other.x );
    return *this;
}

// Dot product by self
float Quaternion::dot( const Quaternion& quat ) const
{
    return ( w*quat.w + x*quat.x + y*quat.y + z*quat.z);
}

// Dot product friend operator
float dot( const Quaternion& quat1, const Quaternion& quat2 )
{
    return (quat1.w*quat2.w + quat1.x*quat2.x + quat1.y*quat2.y + quat1.z*quat2.z);
}

// Rotate vector by quaternion - assumes quaternion is normalized!
Vector3D Quaternion::rotate( const Vector3D& vector ) const
{
    if (!isUnit()) throw MathException("non-unit vector passed to Quaternion::rotate()");

    float pMult = w*w - x*x - y*y - z*z;
    float vMult = 2.0f*(x*vector.x + y*vector.y + z*vector.z);
    float crossMult = 2.0f*w;

    return Vector3D( pMult*vector.x + vMult*x + crossMult*(y*vector.z - z*vector.y),
                      pMult*vector.y + vMult*y + crossMult*(z*vector.x - x*vector.z),
                      pMult*vector.z + vMult*z + crossMult*(x*vector.y - y*vector.x) );
}   

// Linearly interpolate two quaternions
// This will always take the shorter path between them
void lerp( Quaternion& result, const Quaternion& start, const Quaternion& end, float t )
{
    // get cos of "angle" between quaternions
    float cosTheta = start.dot( end );

    // initialize result
    result = t*end;

    // if "angle" between quaternions is less than 90 degrees
    if ( cosTheta >= EPSILON )
    {
        // use standard interpolation
        result += (1.0f-t)*start;
    }
    else
    {
        // otherwise, take the shorter path
        result += (t-1.0f)*start;
    }
}  

// Spherical linearly interpolate two quaternions
// This will always take the shorter path between them
void slerp( Quaternion& result, const Quaternion& start, const Quaternion& end, float t )
{
    // get cosine of "angle" between quaternions
    float cosTheta = start.dot( end );
    float startInterp, endInterp;

    // if "angle" between quaternions is less than 90 degrees
    if ( cosTheta >= EPSILON )
    {
        // if angle is greater than zero
        if ( (1.0f - cosTheta) > EPSILON )
        {
            // use standard slerp
            float theta = acosf( cosTheta );
            float recipSinTheta = 1.0f/sinf( theta );

            startInterp = sinf( (1.0f - t)*theta )*recipSinTheta;
            endInterp = sinf( t*theta )*recipSinTheta;
        }
        // angle is close to zero
        else
        {
            // use linear interpolation
            startInterp = 1.0f - t;
            endInterp = t;
        }
    }
    // otherwise, take the shorter route
    else
    {
        // if angle is less than 180 degrees
        if ( (1.0f + cosTheta) > EPSILON )
        {
            // use slerp w/negation of start quaternion
            float theta = acosf( -cosTheta );
            float recipSinTheta = 1.0f/sinf( theta );

            startInterp = sinf( (t-1.0f)*theta )*recipSinTheta;
            endInterp = sinf( t*theta )*recipSinTheta;
        }
        // angle is close to 180 degrees
        else
        {
            // use lerp w/negation of start quaternion
            startInterp = t - 1.0f;
            endInterp = t;
        }
    }
    
    result = startInterp*start + endInterp*end;
}   

// Approximate spherical linear interpolation of two quaternions
// Based on "Hacking Quaternions", Jonathan Blow, Game Developer, March 2002.
// See Game Developer, February 2004 for an alternate method.
void approxSlerp( Quaternion& result, const Quaternion& start, const Quaternion& end, float t )
{
    float cosTheta = start.dot( end );

    // correct time by using cosine of angle between quaternions
    float factor = 1.0f - 0.7878088f*cosTheta;
    float k = 0.5069269f;
    factor *= factor;
    k *= factor;

    float b = 2*k;
    float c = -3*k;
    float d = 1 + k;

    t = t*(b*t + c) + d;

    // initialize result
    result = t*end;

    // if "angle" between quaternions is less than 90 degrees
    if ( cosTheta >= EPSILON )
    {
        // use standard interpolation
        result += (1.0f-t)*start;
    }
    else
    {
        // otherwise, take the shorter path
        result += (t-1.0f)*start;
    }
}

// conversions to/from other rotation formats (testing March 2013)

// The following functions are adapted from code by David Eberly,
// available at http://www.geometrictools.com.

/* Eberly's code:
template <typename Real>
void Quaternion<Real>::FromRotationMatrix (const Matrix3<Real>& rot)
{
    // Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
    // article "Quaternion Calculus and Fast Animation".

    const int next[3] = { 1, 2, 0 };

    Real trace = rot(0,0) + rot(1,1) + rot(2,2);
    Real root;

    if (trace > (Real)0)
    {
        // |w| > 1/2, may as well choose w > 1/2
        root = Math<Real>::Sqrt(trace + (Real)1);  // 2w
        mTuple[0] = ((Real)0.5)*root;
        root = ((Real)0.5)/root;  // 1/(4w)
        mTuple[1] = (rot(2,1) - rot(1,2))*root;
        mTuple[2] = (rot(0,2) - rot(2,0))*root;
        mTuple[3] = (rot(1,0) - rot(0,1))*root;
    }
    else
    {
        // |w| <= 1/2
        int i = 0;
        if (rot(1,1) > rot(0,0))
        {
            i = 1;
        }
        if (rot(2,2) > rot(i,i))
        {
            i = 2;
        }
        int j = next[i];
        int k = next[j];

        root = Math<Real>::Sqrt(rot(i,i) - rot(j,j) - rot(k,k) + (Real)1);
        Real* quat[3] = { &mTuple[1], &mTuple[2], &mTuple[3] };
        *quat[i] = ((Real)0.5)*root;
        root = ((Real)0.5)/root;
        mTuple[0] = (rot(k,j) - rot(j,k))*root;
        *quat[j] = (rot(j,i) + rot(i,j))*root;
        *quat[k] = (rot(k,i) + rot(i,k))*root;
    }
}
*/
void Quaternion::fromRotationMatrix(const Matrix4x4& rotMatrix)
{
    // Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
    // article "Quaternion Calculus and Fast Animation".

    const int next[3] = { 1, 2, 0 };

    float trace = rotMatrix(0,0) + rotMatrix(1,1) + rotMatrix(2,2);
    float root;

    if (trace > 0.0f)
    {
        // |w| > 1/2, may as well choose w > 1/2
        root = sqrtf(trace + 1.0f);  // 2w
        w = 0.5f*root;
        root = 0.5f/root;  // 1/(4w)
        x = (rotMatrix(2,1) - rotMatrix(1,2))*root;
        y = (rotMatrix(0,2) - rotMatrix(2,0))*root;
        z = (rotMatrix(1,0) - rotMatrix(0,1))*root;
    }
    else
    {
        // |w| <= 1/2
        int i = 0;
        if (rotMatrix(1,1) > rotMatrix(0,0))
        {
            i = 1;
        }
        if (rotMatrix(2,2) > rotMatrix(i,i))
        {
            i = 2;
        }
        int j = next[i];
        int k = next[j];

        root = sqrtf(rotMatrix(i,i) - rotMatrix(j,j) - rotMatrix(k,k) + 1.0f);
		float* quat[3] = { &x, &y, &z };
        *quat[i] = 0.5f*root;
        root = 0.5f/root;
        w = (rotMatrix(k,j) - rotMatrix(j,k))*root;
        *quat[j] = (rotMatrix(j,i) + rotMatrix(i,j))*root;
        *quat[k] = (rotMatrix(k,i) + rotMatrix(i,k))*root;
	}
}

void Quaternion::fromRotationMatrix2(const Matrix4x4& rotMatrix)
{
	// From appendix B.4 in Quaternions, Interpolation and Animation
	//   Erik B.Dam Martin Koch Martin Lillholm
	//   Technical Report DIKU - TR - 98 / 5
	//   Department of Computer Science
	//   University of Copenhagen

	float trace = rotMatrix(0, 0) + rotMatrix(1, 1) + rotMatrix(2, 2) + rotMatrix(3,3);
	w = 0.5f*sqrtf(trace);
	float inv4w = 1.0f / (4.0f*w);
	x = (rotMatrix(2, 1) - rotMatrix(1, 2))*inv4w;
	y = (rotMatrix(0, 2) - rotMatrix(2, 0))*inv4w;
	z = (rotMatrix(1, 0) - rotMatrix(0, 1))*inv4w;
}

/* Eberly's code
template <typename Real>
void Quaternion<Real>::ToRotationMatrix (Matrix3<Real>& rot) const
{
    Real twoX  = ((Real)2)*mTuple[1];
    Real twoY  = ((Real)2)*mTuple[2];
    Real twoZ  = ((Real)2)*mTuple[3];
    Real twoWX = twoX*mTuple[0];
    Real twoWY = twoY*mTuple[0];
    Real twoWZ = twoZ*mTuple[0];
    Real twoXX = twoX*mTuple[1];
    Real twoXY = twoY*mTuple[1];
    Real twoXZ = twoZ*mTuple[1];
    Real twoYY = twoY*mTuple[2];
    Real twoYZ = twoZ*mTuple[2];
    Real twoZZ = twoZ*mTuple[3];

    rot(0,0) = (Real)1 - (twoYY + twoZZ);
    rot(0,1) = twoXY - twoWZ;
    rot(0,2) = twoXZ + twoWY;
    rot(1,0) = twoXY + twoWZ;
    rot(1,1) = (Real)1 - (twoXX + twoZZ);
    rot(1,2) = twoYZ - twoWX;
    rot(2,0) = twoXZ - twoWY;
    rot(2,1) = twoYZ + twoWX;
    rot(2,2) = (Real)1 - (twoXX + twoYY);
}
*/
void Quaternion::toRotationMatrix(Matrix4x4& rotMatrix) const
{
    float twoX  = 2.0f*x;
    float twoY  = 2.0f*y;
    float twoZ  = 2.0f*z;
    float twoWX = twoX*w;
    float twoWY = twoY*w;
    float twoWZ = twoZ*w;
    float twoXX = twoX*x;
    float twoXY = twoY*x;
    float twoXZ = twoZ*x;
    float twoYY = twoY*y;
    float twoYZ = twoZ*y;
    float twoZZ = twoZ*z;

	rotMatrix = Matrix4x4::identity();
    rotMatrix(0,0) = 1.0f - (twoYY + twoZZ);
    rotMatrix(0,1) = twoXY - twoWZ;
    rotMatrix(0,2) = twoXZ + twoWY;
    rotMatrix(1,0) = twoXY + twoWZ;
    rotMatrix(1,1) = 1.0f - (twoXX + twoZZ);
    rotMatrix(1,2) = twoYZ - twoWX;
    rotMatrix(2,0) = twoXZ - twoWY;
    rotMatrix(2,1) = twoYZ + twoWX;
    rotMatrix(2,2) = 1.0f - (twoXX + twoYY);
}

/* Eberly's code:
template <typename Real>
void Quaternion<Real>::FromAxisAngle (const Vector3<Real>& axis, Real angle)
{
    // assert:  axis[] is unit length
    //
    // The quaternion representing the rotation is
    //   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)

    Real halfAngle = ((Real)0.5)*angle;
    Real sn = Math<Real>::Sin(halfAngle);
    mTuple[0] = Math<Real>::Cos(halfAngle);
    mTuple[1] = sn*axis[0];
    mTuple[2] = sn*axis[1];
    mTuple[3] = sn*axis[2];
}
*/
void Quaternion::fromAxisAngle(const Vector3D& axis, const float angle)
{
    // The quaternion representing the rotation is
    //   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)

	// axis must be unit length
	Vector3D naxis = axis;
	naxis = naxis.normalize();

    float halfAngle = 0.5f*angle;
    float sn = sinf(halfAngle);
    w = cosf(halfAngle);
    x = sn*naxis.x;
    y = sn*naxis.y;
    z = sn*naxis.z;
}

/* Eberly's code:
template <typename Real>
void Quaternion<Real>::ToAxisAngle (Vector3<Real>& axis, Real& angle) const
{
    // The quaternion representing the rotation is
    //   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)

    Real sqrLength = mTuple[1]*mTuple[1] + mTuple[2]*mTuple[2]
        + mTuple[3]*mTuple[3];

    if (sqrLength > Math<Real>::ZERO_TOLERANCE)
    {
        angle = ((Real)2)*Math<Real>::ACos(mTuple[0]);
        Real invLength = Math<Real>::InvSqrt(sqrLength);
        axis[0] = mTuple[1]*invLength;
        axis[1] = mTuple[2]*invLength;
        axis[2] = mTuple[3]*invLength;
    }
    else
    {
        // Angle is 0 (mod 2*pi), so any axis will do.
        angle = (Real)0;
        axis[0] = (Real)1;
        axis[1] = (Real)0;
        axis[2] = (Real)0;
    }
}
*/
void Quaternion::toAxisAngle(Vector3D& axis, float& angle) const
{
    float sqrLength = x*x + y*y + z*z;

    if (!::isZero(sqrLength))
    {
        angle = 2.0f*cosf(w);
        float invLength = 1.0f/sqrtf(sqrLength);
        axis.x = x*invLength;
        axis.y = y*invLength;
        axis.z = z*invLength;
    }
    else
    {
        // Angle is 0 (mod 2*pi), so any axis will do.
        angle = 0.0f;
        axis.x = 1.0f;
        axis.y = 0.0f;
        axis.z = 0.0f;
    }
}

void Quaternion::fromEuler(const Vector3D& angles)
{
	Matrix4x4 rotMatrix = Matrix4x4::rotationZYX(angles);
	fromRotationMatrix(rotMatrix);
}

void Quaternion::toEuler(Vector3D& angles) const
{
	Matrix4x4 rotMatrix;
	toRotationMatrix(rotMatrix);
	float r,p,y;
	rotMatrix.factorEulerZYX(p,y,r);
	angles.pitch = p;
	angles.yaw = y;
	angles.roll = r;
}
