//-----------------------------------------------------------------------------
// Models.h
//    Classes for defining and building rendering models.
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

#ifndef MODELS_DOT_H
#define MODELS_DOT_H

#include <Core/SystemConfiguration.h>
#include <vector>
using namespace std;
#include <Math/Math.h>
#include <Math/Point2D.h>
#include <Math/Vector3D.h>
#include <Math/Matrix4x4.h>
#include <Models/Color.h>

class SKA_LIB_DECLSPEC Model
{
public:
	virtual ~Model() { }
	virtual void allocateGraphicsResources() = 0;
	virtual void releaseGraphicsResources() = 0;
	virtual void render(Matrix4x4& world_xform) = 0;
};

class SKA_LIB_DECLSPEC ModelFace
{
public:
	Vector3D point[3];
	Vector3D normal;
	Color color;
public:
	ModelFace(Vector3D _point[3], Vector3D& _normal, Color& _color) 
	{
		point[0] = _point[0];
		point[1] = _point[1];
		point[2] = _point[2];
		normal = _normal;
		color = _color;
	}
	ModelFace(Vector3D& _p1, Vector3D& _p2, Vector3D& _p3, Vector3D& _normal, Color& _color) 
	{
		point[0] = _p1;
		point[1] = _p2;
		point[2] = _p3;
		normal = _normal;
		color = _color;
	}
	virtual ~ModelFace() { }
	virtual bool isTextured() { return false; }
	virtual bool isPointColored() { return false; }
	virtual void setAlpha(float _alpha) { color.a = _alpha; }
	virtual bool isTransparent() { return color.a < 1.0f; }
	virtual float getAlpha() { return color.a; }
};

class SKA_LIB_DECLSPEC PointColorModelFace
{
public:
	Vector3D point[3];
	Vector3D normal;
	Color color[3];
public:
	PointColorModelFace(Vector3D _point[3], Vector3D& _normal, Color _color[3]) 
	{
		point[0] = _point[0];
		point[1] = _point[1];
		point[2] = _point[2];
		normal = _normal;
		color[0] = _color[0];
		color[1] = _color[1];
		color[2] = _color[2];
	}
	PointColorModelFace(Vector3D& _p1, Vector3D& _p2, Vector3D& _p3, Vector3D& _normal, 
		Color& _c1, Color& _c2, Color& _c3) 
	{
		point[0] = _p1;
		point[1] = _p2;
		point[2] = _p3;
		normal = _normal;
		color[0] = _c1;
		color[1] = _c2;
		color[2] = _c3;
	}
	virtual ~PointColorModelFace() { }
	virtual bool isPointColored() { return true; }
};

class SKA_LIB_DECLSPEC TexturedModelFace : public ModelFace
{
public:
	Point2D texture_uv[3];
	unsigned int texture_id;
public:
	TexturedModelFace(Vector3D _point[3], Vector3D& _normal, Color& _color, 
		Point2D _uv[3], unsigned int _texture_id) 
		: ModelFace(_point, _normal, _color)
	{
		texture_uv[0] = _uv[0];
		texture_uv[1] = _uv[1];
		texture_uv[2] = _uv[2];
		texture_id = _texture_id;
	}
	TexturedModelFace(Vector3D& _p1, Vector3D& _p2, Vector3D& _p3, Vector3D& _normal, Color& _color,
		Point2D _uv1, Point2D _uv2, Point2D _uv3, unsigned int _texture_id)
		: ModelFace(_p1, _p2, _p3, _normal, _color)
	{
		texture_uv[0] = _uv1;
		texture_uv[1] = _uv2;
		texture_uv[2] = _uv3;
		texture_id = _texture_id;
	}
	virtual ~TexturedModelFace() { }
	virtual bool isTextured() { return true; }
};

class MeshFaceList
{
public:
	vector<ModelFace> faces;
	vector<TexturedModelFace> tfaces;
	vector<PointColorModelFace> pcfaces;
};

class SKA_LIB_DECLSPEC MeshModel : public Model
{
public:
	MeshFaceList* faces;
public:
	MeshModel() : Model() { faces = new MeshFaceList; }
	virtual ~MeshModel() { delete faces; }
	virtual void allocateGraphicsResources() { }
	virtual void releaseGraphicsResources() { }
	virtual void render(Matrix4x4& world_xform);
protected:
	virtual void addModelFace(ModelFace& f) { faces->faces.push_back(f); }
	virtual void addTexturedModelFace(TexturedModelFace& f) { faces->tfaces.push_back(f); }
	virtual void addPointColorModelFace(PointColorModelFace& f) { faces->pcfaces.push_back(f); }
};

// A DirectModel renders using coded graphics commands
class SKA_LIB_DECLSPEC DirectModel : public Model
{
public:
	DirectModel() : Model() { }
	virtual ~DirectModel() { } 
	virtual void allocateGraphicsResources() { }
	virtual void releaseGraphicsResources() { }
	virtual void render(Matrix4x4& world_xform) = 0;
};

#endif
