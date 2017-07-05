//-----------------------------------------------------------------------------
// Quaternion.h
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

#ifndef QUATERNION_DOT_H
#define QUATERNION_DOT_H
#include <Core/SystemConfiguration.h>
#include <iostream>
using namespace std;
#include <Math/Math.h>
#include <Math/Matrix4x4.h>

class SKA_LIB_DECLSPEC Quaternion
{
public:
    // constructors
    inline Quaternion()
		: w(1.0f), x(0.0f), y(0.0f), z(0.0f)
    {}
    inline Quaternion(float _w, float _x, float _y, float _z) 
		: w(_w), x(_x), y(_y), z(_z)
    {}
    Quaternion(const Vector3D& axis, float angle);
    Quaternion(const Vector3D& from, const Vector3D& to);

	// explicit type conversions
    explicit Quaternion(const Vector3D& vector);
    explicit Quaternion(const Matrix4x4& rotation);
    
	// destructor
	inline ~Quaternion() {}

    // copy operations
    Quaternion(const Quaternion& other);
    Quaternion& operator=(const Quaternion& other);

    // text output
    SKA_LIB_DECLSPEC friend ostream& operator<<(ostream& out, const Quaternion& source);

    // accessors - these access the x, y and z components as q[0], q[1], and q[2]
    inline float& operator[](unsigned int i)         { return (&x)[i]; }
    inline float operator[](unsigned int i) const    { return (&x)[i]; }

    float magnitude() const;
    float norm() const;

    // comparison
    bool operator==(const Quaternion& other) const;
    bool operator!=(const Quaternion& other) const;
    bool isZero() const;
    bool isUnit() const;
    bool isIdentity() const;

    // manipulators
    inline void set( float _w, float _x, float _y, float _z );
    void set( const Vector3D& axis, float angle );
    void set( const Vector3D& from, const Vector3D& to );
    void set( const Matrix4x4& rotation );
    void set( float zRotation, float yRotation, float xRotation ); 

    void getAxisAngle( Vector3D& axis, float& angle );

    void clean();       // sets near-zero elements to 0
    inline void zero(); // sets all elements to 0
    void normalize();   // sets to unit quaternion
    inline void identity();    // sets to identity quaternion

    // complex conjugate
    SKA_LIB_DECLSPEC friend Quaternion conjugate( const Quaternion& quat );
    const Quaternion& conjugate();

    // invert quaternion
    SKA_LIB_DECLSPEC friend Quaternion inverse( const Quaternion& quat );
    const Quaternion& inverse();

    // operators

    // addition/subtraction
    Quaternion operator+( const Quaternion& other ) const;
    Quaternion& operator+=( const Quaternion& other );
    Quaternion operator-( const Quaternion& other ) const;
    Quaternion& operator-=( const Quaternion& other );

    Quaternion operator-() const;

    // scalar multiplication
    SKA_LIB_DECLSPEC friend Quaternion operator*( float scalar, const Quaternion& vector );
    Quaternion& operator*=( float scalar );

    // quaternion multiplication
    Quaternion operator*( const Quaternion& other ) const;
    Quaternion& operator*=( const Quaternion& other );

    // dot product
    float dot( const Quaternion& vector ) const;
    SKA_LIB_DECLSPEC friend float dot( const Quaternion& vector1, const Quaternion& vector2 );

    // vector rotation
    Vector3D rotate( const Vector3D& vector ) const;

    // interpolation
    SKA_LIB_DECLSPEC friend void lerp( Quaternion& result, const Quaternion& start, const Quaternion& end, float t );
    SKA_LIB_DECLSPEC friend void slerp( Quaternion& result, const Quaternion& start, const Quaternion& end, float t );
    SKA_LIB_DECLSPEC friend void approxSlerp( Quaternion& result, const Quaternion& start, const Quaternion& end, float t );

	// conversions to/from other rotation formats (testing March 2013)
	void fromRotationMatrix(const Matrix4x4& rotMatrix);
	void fromRotationMatrix2(const Matrix4x4& rotMatrix);
	void toRotationMatrix(Matrix4x4& rotMatrix) const;
	void fromAxisAngle(const Vector3D& axis, const float angle);
	void toAxisAngle(Vector3D& axis, float& angle) const;
	void fromEuler(const Vector3D& angles);
	void toEuler(Vector3D& angles) const;

public: 
    // member variables
    float w, x, y, z;      
};

// Set quaternion elements
inline void Quaternion::set( float _w, float _x, float _y, float _z )
{
    w = _w; x = _x; y = _y; z = _z;
} 

// Zero all elements
inline void Quaternion::zero()
{
    x = y = z = w = 0.0f;
} 

// Set to identity quaternion
inline void Quaternion::identity()
{
    x = y = z = 0.0f;
    w = 1.0f;
} 

#endif
