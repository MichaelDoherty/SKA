//-----------------------------------------------------------------------------
// Camera.h
//	 Hierarchy of classes implementing cameras/viewers.
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

#ifndef CAMERA_DOT_H
#define CAMERA_DOT_H
#include <Core/SystemConfiguration.h>
#include <cstdio>
#include <cstring>
#include <Math/Math.h>
#include <Math/Matrix4x4.h>

// Camera abstract class - cameras must define viewport as two matrices
class SKA_LIB_DECLSPEC Camera
{
public:
	virtual Matrix4x4 viewTransformation() const = 0;
	virtual Matrix4x4 projectionTransformation() const = 0;
};

// LookAtCamera - a lookat viewer has it's position defined by
// an eyepoint and has its orientation defined by a "lookat point"
// and a "up vector" relative to the lookat point.
class SKA_LIB_DECLSPEC LookAtCamera : public Camera
{
public:
	// Accessors
	Vector3D eyePoint() const					{ return eye_point; }
	Vector3D lookatPoint() const				{ return lookat_point; }
	Vector3D upVector() const					{ return up_vector; }
	Vector3D forwardVector() const				{ return forward_vector; }
	Vector3D rightVector() const				{ return right_vector; }

	Matrix4x4  viewTransformation() const		{ return view_transformation; }
	Matrix4x4  projectionTransformation() const	{ return projection_transformation; }

	// Modifiers
	void viewParameters(const Vector3D& _eye_point, 
		const Vector3D& _lookat_point,
		const Vector3D& _up_vector);
	void projectionParameters(float _vert_fov, float _aspect_ratio, 
		float _near_plane_distance, float _far_plane_distance);

	// constructor
	LookAtCamera();
	virtual ~LookAtCamera();
	virtual void setSceneView();

private:

	// attributes that define the camera
	float	aspect_ratio, vert_fov;
	float	near_plane_distance, far_plane_distance;
	Vector3D eye_point;       
	Vector3D lookat_point;
	Vector3D up_vector;

	// derived attributes
	Vector3D forward_vector;
	Vector3D right_vector;

	Matrix4x4  view_transformation;
	Matrix4x4  projection_transformation;
};

class SKA_LIB_DECLSPEC MovingCamera : public LookAtCamera
{
public:
	void move(float elapsed_time=0.0, 
			float fwd_thrust=0.0f, 
			float hrz_thrust=0.0f, 
			float vrt_thrust=0.0f,
			float yaw_thrust=0.0f,
			float pitch_thrust=0.0f,
			float roll_thrust=0.0f);

	// constructor
	MovingCamera();
	MovingCamera(Vector3D _position, Vector3D _orientation);
	
	virtual ~MovingCamera();

	void moveTo(Vector3D new_position);
	void rotateTo(Vector3D new_rotation);

	Vector3D currPosition() { return position; }
	Vector3D currRotation() { return orientation; }

protected:
	Vector3D position;
	Vector3D velocity;
	Vector3D orientation;
};

#endif
