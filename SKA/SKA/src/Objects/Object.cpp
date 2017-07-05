//-----------------------------------------------------------------------------
// Object.cpp
//    Base object class for all renderable objects.
//    Objects have a position, orientation, visibility and a rendering model.
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
#include <Objects/Object.h>
#include <Models/ModelFactory.h>

Object::Object(ModelSpecification& _model_spec, Vector3D _pos, Vector3D _rot, Vector3D _scale)
: expired(false), visible(true), rendering_model(NULL), pos(_pos), rot(_rot), scale(_scale) 
{
	changeModel(_model_spec);
}
Object::Object(Model* _model, Vector3D _pos, Vector3D _rot, Vector3D _scale)
: expired(false), visible(true), rendering_model(NULL), pos(_pos), rot(_rot), scale(_scale) 
{
	changeModel(_model);
}

Object::Object()
: expired(false), visible(true), rendering_model(NULL)
{
	pos = Vector3D(0.0f,0.0f,0.0f);
	rot = Vector3D(0.0f,0.0f,0.0f);
	scale = Vector3D(1.0f,1.0f,1.0f);
}

void Object::changeModel(ModelSpecification& _model_spec)
{
	// FIXIT! NO IMPLEMENTATION
	// DEALLOCATE OLD MODEL
	// ACCESS MODEL FACTORY TO GET NEW MODEL
	rendering_model = model_factory.buildModel(_model_spec);
}

void Object::changeModel(Model* _model)
{
	// FIXIT! NO IMPLEMENTATION
	// DEALLOCATE OLD MODEL
	// ACCESS MODEL FACTORY TO GET NEW MODEL
	rendering_model = _model;
}

Matrix4x4 Object::worldTransformation() const
{	
	Matrix4x4 scale_xform;
	scale_xform = Matrix4x4::scaleXYZ(scale);
	Matrix4x4 translation_xform;
	translation_xform = Matrix4x4::translationXYZ(pos);
	Matrix4x4 orientation_xform;
	orientation_xform = Matrix4x4::rotationZXY(rot);

	// translate, then rotate, then scale
	Matrix4x4 world_xform = translation_xform * orientation_xform * scale_xform;

	return world_xform;
}

