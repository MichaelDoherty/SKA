//-----------------------------------------------------------------------------
// Object.h
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

#ifndef OBJECT_DOT_H
#define OBJECT_DOT_H
#include <Core/SystemConfiguration.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <Core/SystemLog.h>
#include <Models/ModelFactory.h>

class SKA_LIB_DECLSPEC Object
{
public:
	Object();

	Object(ModelSpecification& _model_spec, 
		Vector3D _pos = Vector3D(0.0f,0.0f,0.0f), 
		Vector3D _rot = Vector3D(0.0f,0.0f,0.0f), 
		Vector3D _scale = Vector3D(1.0f,1.0f,1.0f));

	Object(Model* _model, 
		Vector3D _pos = Vector3D(0.0f,0.0f,0.0f), 
		Vector3D _rot = Vector3D(0.0f,0.0f,0.0f), 
		Vector3D _scale = Vector3D(1.0f,1.0f,1.0f));

	virtual ~Object() { }

	virtual Matrix4x4 worldTransformation() const;

	virtual Model* renderingModel() const { return rendering_model; }

	virtual void updateState(float time_delta=0.0) { }

	// An object has expired if it will never again be visible
	// and will never again interact with anoth object.
	// Expired objects can be deleted at the convenience of the
	// object manager.
	virtual bool hasExpired() const { return expired; }

	// An object is visible if it can potentially be seen from
	// the current viewpoint.  For any frame/scene, an object is
	// passed to the game engine for rendering if and only if it
	// returns true for this operation.
	virtual bool isVisible() const { return visible && !expired; }
	virtual void setVisibility(bool _vis) { visible = _vis; }

	void changeModel(ModelSpecification& _model_spec);
	void changeModel(Model* _model);

	virtual void render(Matrix4x4& world_xform) 
	{
		if (renderingModel() == NULL) return;
		world_xform = worldTransformation();
		renderingModel()->render(world_xform);
	}

protected:
	bool expired;
	bool visible;
	Model* rendering_model;
	char* model_name;

public:
	virtual Vector3D currPosition() { return pos; };
	virtual Vector3D currRotation() { return rot; };
	virtual Vector3D currScaling()  { return scale; }
	virtual void moveTo(Vector3D new_pos) { pos = new_pos; }
	virtual void rotateTo(Vector3D new_rot) { rot = new_rot; }
	virtual void scaleTo(Vector3D new_scale) { scale = new_scale; }
	virtual void rotate(Vector3D& r) { rot += r; }

protected:
	Vector3D pos;		// position = <x, y, z>
	Vector3D rot;		// rotation = <pitch, yaw, roll> 
	Vector3D scale;		// scaling = <x, y, z>
};

#endif
