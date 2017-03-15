//-----------------------------------------------------------------------------
// CodedModels.h
//    Models that are defined by code.
//    Includes boxes, bones, ground, and vectors (arrows)
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

#ifndef CODED_MODELS_DOT_H
#define CODED_MODELS_DOT_H
#include <Core/SystemConfiguration.h>
#include <Models/Models.h>

class SKA_LIB_DECLSPEC LineModel : public DirectModel
{
public:
	LineModel(Vector3D& _start, Vector3D& _end, Color _c)
		: DirectModel(), start(_start), end(_end), color(_c)
	{}
	virtual ~LineModel() { }
	virtual void render(Matrix4x4& world_xform);
private:
	Vector3D start;
	Vector3D end;
	Color color;
};

class SKA_LIB_DECLSPEC CoordinateAxisModel : public DirectModel
{
public:
	CoordinateAxisModel(float _length, bool _arrows=false)
		: DirectModel(), length(_length), add_arrowheads(_arrows)
	{
	}
	virtual ~CoordinateAxisModel() { }
	virtual void render(Matrix4x4& world_xform);
private:
	float length;
	bool add_arrowheads;
};


class SKA_LIB_DECLSPEC BoxModel : public MeshModel
{
public:
	BoxModel(float _width, float _height, float _length, Color _color);
	virtual ~BoxModel() { }
private:
	void buildMesh(float _width, float _height, float _length, Color _color);
};


class SKA_LIB_DECLSPEC TexturedBoxModel : public MeshModel
{
public:
	TexturedBoxModel(float _width, float _height, float _length, 
		Color _color, char* _texture_file);
	virtual ~TexturedBoxModel() { }
private:
	void buildMesh(float _width, float _height, float _length, 
		Color _color, char* _texture_file);
};

class SKA_LIB_DECLSPEC VectorModel : public MeshModel
{
public:
	VectorModel();
	VectorModel(float _scale, Color _color=Color(1.0f,1.0f,1.0f,1.0f));
	virtual ~VectorModel() { }
private:
	void buildMesh(float scale, Color color);
};

class SKA_LIB_DECLSPEC BoneModel : public MeshModel
{
public:
	BoneModel();
	BoneModel(Color color);
	virtual ~BoneModel() { }
private:
	void buildMesh(Color color);
};

class SKA_LIB_DECLSPEC GroundModel : public MeshModel
{
public:
	GroundModel();
	virtual ~GroundModel() { } 
private:
	void buildMesh();
};

// A solid color two-sided plane.
class SKA_LIB_DECLSPEC XYPlaneModel : public MeshModel
{
public:
	XYPlaneModel();
	XYPlaneModel(float _size, short _faces = 10, Color _color = Color(1.0f, 1.0f, 1.0f, 1.0f));
	virtual ~XYPlaneModel() { }
private:
	void buildMesh(float _size, short _faces, Color _color);
};

class SKA_LIB_DECLSPEC YZPlaneModel : public MeshModel
{
public:
	YZPlaneModel();
	YZPlaneModel(float _size, short _faces = 10, Color _color = Color(1.0f, 1.0f, 1.0f, 1.0f));
	virtual ~YZPlaneModel() { }
private:
	void buildMesh(float _size, short _faces, Color _color);
};

class SKA_LIB_DECLSPEC ZXPlaneModel : public MeshModel
{
public:
	ZXPlaneModel();
	ZXPlaneModel(float _size, short _faces=10, Color _color=Color(1.0f, 1.0f, 1.0f, 1.0f));
	virtual ~ZXPlaneModel() { }
private:
	void buildMesh(float _size, short _faces, Color _color);
};

class SKA_LIB_DECLSPEC PointerModel : public MeshModel
{
public:
	PointerModel();
	PointerModel(const Vector3D& dimensions);
	virtual ~PointerModel() { } 
private:
	void buildMesh(const Vector3D& dimensions);
};

class SKA_LIB_DECLSPEC Flex13CameraModel : public MeshModel
{
public:
	Flex13CameraModel();
	Flex13CameraModel(Color& c);
	virtual ~Flex13CameraModel() { } 
private:
	void buildMesh(Color& c);
};

#endif
