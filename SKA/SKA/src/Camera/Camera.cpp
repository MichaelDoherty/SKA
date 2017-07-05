//-----------------------------------------------------------------------------
// Camera.cpp
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

#include <Core/SystemConfiguration.h>
#include <Camera/Camera.h>

static Vector3D world_forward_vector(0.0f,0.0f,1.0f);
static Vector3D world_right_vector(1.0f,0.0f,0.0f);
static Vector3D world_up_vector(0.0f,1.0f,0.0f);

LookAtCamera::LookAtCamera()
{
    viewParameters(Vector3D(0.0f,0.0f,0.0f), 
		Vector3D(0.0f,0.0f,1.0f),
		Vector3D(0.0f,1.0f,0.0f));
    projectionParameters(PI/4.0f, 640.0f/480.0f, 0.0f, 1.0f);
}

LookAtCamera::~LookAtCamera()
{
}

void LookAtCamera::viewParameters(const Vector3D& _eye_point, 
								  const Vector3D& _lookat_point,
								 const Vector3D& _up_vector)
{
    // Set attributes for the view matrix
    eye_point			= _eye_point;
    lookat_point		= _lookat_point;
    up_vector			= _up_vector;
	Vector3D tmp = lookat_point - eye_point;
	forward_vector = tmp.normalize();
    right_vector = forward_vector.cross(up_vector);
	view_transformation = Matrix4x4::lookatMatrixLeftHand(eye_point, lookat_point, up_vector);
}

void LookAtCamera::projectionParameters(float _vert_fov, float _aspect_ratio, 
		float _near_plane_distance, float _far_plane_distance)
{
    // Set attributes for the projection matrix
	aspect_ratio = _aspect_ratio; 
	vert_fov = _vert_fov;
	near_plane_distance = _near_plane_distance;
	far_plane_distance = _far_plane_distance;
    projection_transformation = Matrix4x4::perspectiveProjectionLeftHand(
		vert_fov, aspect_ratio, near_plane_distance, far_plane_distance);
}

MovingCamera::MovingCamera()
:	LookAtCamera(), position(0.0f,0.0f,0.0f), velocity(0.0f,0.0f,0.0f),
	orientation(0.0f, 0.0f, 0.0f)
{ }

MovingCamera::MovingCamera(Vector3D _position, Vector3D _orientation)
:	LookAtCamera(), position(_position), velocity(0.0f,0.0f,0.0f),
	orientation(_orientation)
{ }

MovingCamera::~MovingCamera()
{ }

static const float acceleration_factor = 300.0;

void MovingCamera::move(float elapsed_time,
						float fwd_thrust, float hrz_thrust, float vrt_thrust,
						float yaw_thrust, float pitch_thrust, float roll_thrust)
{ 
	float pitch = orientation.pitch;
	float heading = orientation.yaw;
	float roll = orientation.roll;

	pitch += pitch_thrust * elapsed_time;
    if (pitch > 1.57f)  pitch = 1.57f;
    if (pitch < -1.57f) pitch = -1.57f;
	
	roll += roll_thrust * 0.5f * elapsed_time;
    if (roll > 1.57f)  roll = 1.57f;
    if (roll < -1.57f) roll = -1.57f;
	
    heading += yaw_thrust * elapsed_time;

	Matrix4x4 rotation_matrix = Matrix4x4::rotationZXY(pitch, heading, roll);

	Vector3D fwd_direction, hrz_direction, vrt_direction;
	
	fwd_direction = rotation_matrix * world_forward_vector;
	hrz_direction = rotation_matrix * world_right_vector;
	vrt_direction = rotation_matrix * world_up_vector;

	velocity +=		fwd_direction * (fwd_thrust * acceleration_factor * elapsed_time)
				+	hrz_direction * (hrz_thrust * acceleration_factor * elapsed_time)
				+	vrt_direction * (vrt_thrust * acceleration_factor * elapsed_time);

	position += velocity * elapsed_time;
	velocity = velocity * 0.0f;  // no momentum
	//velocity = velocity * 0.6;  // lose momentum

	orientation = Vector3D(pitch, heading, roll);

	//viewParameters(position, position + fwd_direction, world_up_vector);
	viewParameters(position, position + fwd_direction, vrt_direction);

}

void MovingCamera::moveTo(Vector3D new_position)
{
	position = new_position;
}

void MovingCamera::rotateTo(Vector3D new_rotation)
{
	orientation = new_rotation;
}
