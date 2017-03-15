//-----------------------------------------------------------------------------
// QObject.h
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

#ifndef QOBJECT_DOT_H
#define QOBJECT_DOT_H
#include <Core/SystemConfiguration.h>
#include <Objects/Object.h>
#include <Math/Quaternion.h>

class SKA_LIB_DECLSPEC QObject : public Object
{
public:
	QObject() : Object() { }

	QObject(ModelSpecification& _model_spec, 
		Vector3D _pos = Vector3D(0.0f,0.0f,0.0f), 
		Vector3D _rot = Vector3D(0.0f,0.0f,0.0f), 
		Vector3D _scale = Vector3D(1.0f,1.0f,1.0f))
		: Object(_model_spec, _pos, _rot, _scale)
	{}

	QObject(Model* _model, 
		Vector3D _pos = Vector3D(0.0f,0.0f,0.0f), 
		Vector3D _rot = Vector3D(0.0f,0.0f,0.0f), 
		Vector3D _scale = Vector3D(1.0f,1.0f,1.0f))
		: Object(_model, _pos, _rot, _scale)
	{}

	virtual ~QObject() { }

	virtual Matrix4x4 worldTransformation() const;
	virtual void rotate(Vector3D& r);

	Quaternion getQOrientation() { return qrot; }

private:
	Quaternion qrot;
};

#endif