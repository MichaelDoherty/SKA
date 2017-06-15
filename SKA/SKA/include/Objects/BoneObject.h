//-----------------------------------------------------------------------------
// BoneObject.h
//    An object that represents a bone.
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

#ifndef BONEOBJECT_DOT_H
#define BONEOBJECT_DOT_H
#include <Core/SystemConfiguration.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <Objects/Object.h>
#include <Models/CodedModels.h>

class SKA_LIB_DECLSPEC BoneObject : public Object
{
public:
	BoneObject(ModelSpecification& _model_spec, 
		Vector3D _pos, Vector3D _rot, 
		float _length=1.0f)
		: Object(_model_spec, _pos, _rot), 
		length(_length), 
		start_pos(Vector3D(0.0f,0.0f,0.0f)), end_pos(Vector3D(0.0f,0.0f,0.0f))
	{
		external_world_xform = Matrix4x4::identity();
		use_external_xform = false;
		draw_as_line = true;
		special_bone = false;
	}
	BoneObject(ModelSpecification& _model_spec)
		: Object(_model_spec), length(1.0f),
		start_pos(Vector3D(0.0f,0.0f,0.0f)), end_pos(Vector3D(0.0f,0.0f,0.0f))
	{
		external_world_xform = Matrix4x4::identity();
		use_external_xform = false;
		draw_as_line = true;
		special_bone = false;
	}
	virtual ~BoneObject() { }

	virtual Matrix4x4 worldTransformation() const 
	{ 
		if (use_external_xform)	return external_world_xform;
		else return Object::worldTransformation();
	}
	virtual void setWorldTransformation(Matrix4x4& xform) { external_world_xform = xform; }

	virtual float currLength() { return length; }
	virtual void setLength(float len) { length = len; }

	virtual void setEndpoints(const Vector3D& start, const Vector3D& end)
	{
		start_pos = start; end_pos = end;
	}

	void drawAsLine() { draw_as_line = true; }
	void drawAsMesh() { draw_as_line = false; }
	void special(bool f=true) { special_bone = f; }

	virtual void useExternalWorldXform(bool b=true) { use_external_xform=b; }
	
	virtual void render(Matrix4x4& world_xform) 
	{
		if (draw_as_line)
		{
			Color c(1.0f, 1.0f, 0.0f);
			if (special_bone) c.g = 0.0f;
			LineModel line(start_pos, end_pos, c);
			Matrix4x4 world_xform;
			line.render(world_xform);
		}
		else
		{
			if (renderingModel() == NULL) return;
			Vector3D dir = end_pos - start_pos;
			Vector3D bone_base(0.0f,0.0f,1.0f);
			Matrix4x4 r = Matrix4x4::rotationFromVectors(bone_base, dir);
			Matrix4x4 t = Matrix4x4::translationXYZ(start_pos);
			Matrix4x4 s = Matrix4x4::scaleXYZ(1.0f, 1.0f, length);
			world_xform = t*r*s;
			renderingModel()->render(world_xform);
		}
	}

	Vector3D getEndpoint() { return end_pos; }

private:
	float length;
	Matrix4x4 external_world_xform;
	bool use_external_xform;
	Vector3D start_pos, end_pos;
	bool draw_as_line;
	bool special_bone;
};

#endif
