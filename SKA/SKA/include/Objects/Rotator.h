//-----------------------------------------------------------------------------
// Rotator.h
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

#ifndef ROTATOR_DOT_H
#define ROTATOR_DOT_H
#include <Core/SystemConfiguration.h>
#include <Objects/Object.h>
#include <Math/Quaternion.h>

class SKA_LIB_DECLSPEC Rotator : public Object
{
public:
	Rotator() : Object(), 
		interp_active(false), which_interpolation(1)
	{ }

	Rotator(ModelSpecification& _model_spec, 
		Vector3D _pos = Vector3D(0.0f,0.0f,0.0f), 
		Vector3D _rot = Vector3D(0.0f,0.0f,0.0f), 
		Vector3D _scale = Vector3D(1.0f,1.0f,1.0f))
		: Object(_model_spec, _pos, _rot, _scale), 
		interp_active(false), which_interpolation(1)
	{}

	Rotator(Model* _model, 
		Vector3D _pos = Vector3D(0.0f,0.0f,0.0f), 
		Vector3D _rot = Vector3D(0.0f,0.0f,0.0f), 
		Vector3D _scale = Vector3D(1.0f,1.0f,1.0f))
		: Object(_model, _pos, _rot, _scale), 
		interp_active(false), which_interpolation(1)
	{}

	virtual ~Rotator() { }

	virtual Matrix4x4 worldTransformation() const;
	virtual void rotate(Vector3D& r);
	virtual void rotate(const Quaternion& q);
	virtual void resetOrientation(const Quaternion& q);

	virtual bool interpolationActive() { return interp_active; }
	virtual short whichInterpolation() { return which_interpolation; }
	virtual void startInterpolation(Quaternion& _start, Quaternion& _end, float _step, short _which=1);
	virtual bool stepInterpolation();

	Quaternion getQOrientation() { return qrot; }

private:
	Quaternion qrot;

	// interpolation attributes
	Quaternion start_q, end_q;
	float interp_step;
	float interp_t;
	bool interp_active;
	short which_interpolation; // 1 = SLERP, 2 = LERP, 3 = appoximate SLERP
};

#endif