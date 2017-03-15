//-----------------------------------------------------------------------------
// Rotator.cpp
//    An object for testing rotations.
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
#include <Objects/Rotator.h>

static Vector3D x_axis(1.0f, 0.0f, 0.0f);
static Vector3D y_axis(0.0f, 1.0f, 0.0f);
static Vector3D z_axis(0.0f, 0.0f, 1.0f);

void Rotator::rotate(Vector3D& r)
{
	rot += r;

	Quaternion qx, qy, qz;
	qx.set(x_axis, r.pitch);
	qy.set(y_axis, r.yaw);
	qz.set(z_axis, r.roll);

	qrot = qrot*qz;
	qrot = qrot*qy;
	qrot = qrot*qx;
	// normalize to remove any numerical errors
	qrot.normalize();
}

void Rotator::rotate(const Quaternion& q)
{
	qrot = qrot*q;
	// normalize to remove any numerical errors
	qrot.normalize();
}

void Rotator::resetOrientation(const Quaternion& q)
{
	qrot = q;
	// make sure the quaternion is normalized
	qrot.normalize();
}

void Rotator::startInterpolation(Quaternion& _start, Quaternion& _end, float _step, short _which)
{
	start_q = _start;
	end_q = _end;
	interp_step = _step;
	interp_t = 0.0f;
	qrot = start_q;
	which_interpolation = _which;
	interp_active = true;
}

bool Rotator::stepInterpolation()
{
	if (!interp_active) return false;
	interp_t += interp_step;
	if (which_interpolation==2)
		lerp(qrot, start_q, end_q, interp_t);
	else if (which_interpolation==3)
		approxSlerp(qrot, start_q, end_q, interp_t);
	else
		slerp(qrot, start_q, end_q, interp_t);

	qrot.normalize();

	if (interp_t >= 1.0f) interp_active = false;
	return interp_active;
}

Matrix4x4 Rotator::worldTransformation() const
{	
	Matrix4x4 scale_xform;
	scale_xform = Matrix4x4::scaleXYZ(scale);
	Matrix4x4 translation_xform;
	translation_xform = Matrix4x4::translationXYZ(pos);
	Matrix4x4 orientation_xform;
	orientation_xform = Matrix4x4::rotationFromQuaternion(qrot);

	// translate, then rotate, then scale
	Matrix4x4 world_xform = translation_xform * orientation_xform * scale_xform;

	return world_xform;
}
