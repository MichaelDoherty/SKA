//-----------------------------------------------------------------------------
// QObject.cpp
//    An object that is controlled by quaternions.
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
#include <Objects/QObject.h>

static Vector3D x_axis(1.0f, 0.0f, 0.0f);
static Vector3D y_axis(0.0f, 1.0f, 0.0f);
static Vector3D z_axis(0.0f, 0.0f, 1.0f);

void QObject::rotate(Vector3D& r)
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


Matrix4x4 QObject::worldTransformation() const
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
