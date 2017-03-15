//-----------------------------------------------------------------------------
// CodeModels.cpp
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

#include <Core/SystemConfiguration.h>
#include <Models/CodedModels.h>
#include <Graphics/GraphicsInterface.h>
#include <Graphics/Textures.h>
#include <Core/SystemLog.h>
#include <Math/Math.h>

BoxModel::BoxModel(float _width, float _height, float _length, Color _color)
	: MeshModel()
{
	buildMesh(_width, _height, _length, _color);
}

void BoxModel::buildMesh(float _width, float _height, float _length, Color _color)
{
	float x = _width/2.0f;
	float y = _height/2.0f;
	float z = _length/2.0f;
	Vector3D p1( x,  y, -z);
	Vector3D p2(-x,  y, -z);
	Vector3D p3(-x, -y, -z);
	Vector3D p4( x, -y, -z);
	Vector3D p5( x,  y,  z);
	Vector3D p6(-x,  y,  z);
	Vector3D p7(-x, -y,  z);
	Vector3D p8( x, -y,  z);
	
	Vector3D n1( 0.0f,  0.0f, -1.0f); // back face
	Vector3D n2( 0.0f,  0.0f,  1.0f); // front face
	Vector3D n3( 0.0f,  1.0f,  0.0f); // top face
	Vector3D n4( 0.0f, -1.0f,  0.0f); // bottom face
	Vector3D n5( 1.0f,  0.0f,  0.0f); // right face
	Vector3D n6(-1.0f,  0.0f,  0.0f); // left face

	ModelFace mf[12] = {
		ModelFace(p1, p2, p3, n1, _color), // back face		
		ModelFace(p1, p3, p4, n1, _color),
		ModelFace(p8, p7, p6, n2, _color),	// front face		
		ModelFace(p8, p6, p5, n2, _color),	
		ModelFace(p5, p6, p2, n3, _color), // top face	
		ModelFace(p5, p2, p1, n3, _color),
		ModelFace(p4, p3, p7, n4, _color), // bottom face	
		ModelFace(p4, p7, p8, n4, _color),
		ModelFace(p8, p5, p1, n5, _color), // right face
		ModelFace(p8, p1, p4, n5, _color),
		ModelFace(p6, p7, p3, n6, _color), // left face
		ModelFace(p6, p3, p2, n6, _color)
	};
	for (short i=0; i<12; i++) addModelFace(mf[i]); 	
}

TexturedBoxModel::TexturedBoxModel(float _width, float _height, float _length, 
								   Color _color, char* _texture_file)
	: MeshModel()
{
	buildMesh(_width, _height, _length, _color, _texture_file);
}

void TexturedBoxModel::buildMesh(float _width, float _height, float _length, 
								 Color _color, char* _texture_file)
{
	unsigned int texture_id = texture_manager.loadTextureBMP(_texture_file);

	float x = _width/2.0f;
	float y = _height/2.0f;
	float z = _length/2.0f;
	Vector3D p1( x,  y, -z);
	Vector3D p2(-x,  y, -z);
	Vector3D p3(-x, -y, -z);
	Vector3D p4( x, -y, -z);
	Vector3D p5( x,  y,  z);
	Vector3D p6(-x,  y,  z);
	Vector3D p7(-x, -y,  z);
	Vector3D p8( x, -y,  z);
	
	Vector3D n1( 0.0f,  0.0f, -1.0f); // back face
	Vector3D n2( 0.0f,  0.0f,  1.0f); // front face
	Vector3D n3( 0.0f,  1.0f,  0.0f); // top face
	Vector3D n4( 0.0f, -1.0f,  0.0f); // bottom face
	Vector3D n5( 1.0f,  0.0f,  0.0f); // right face
	Vector3D n6(-1.0f,  0.0f,  0.0f); // left face

	Point2D uv0(0,0);
	Point2D uv1(1,0);
	Point2D uv2(1,1);
	Point2D uv3(0,1);

	TexturedModelFace tmf[12] = {
		TexturedModelFace(p1, p2, p3, n1, _color, uv0, uv1, uv2, texture_id), // back face		
		TexturedModelFace(p1, p3, p4, n1, _color, uv0, uv2, uv3, texture_id),
		TexturedModelFace(p8, p7, p6, n2, _color, uv2, uv3, uv0, texture_id), // front face		
		TexturedModelFace(p8, p6, p5, n2, _color, uv2, uv0, uv1, texture_id),	
		TexturedModelFace(p5, p6, p2, n3, _color, uv3, uv0, uv1, texture_id), // top face	
		TexturedModelFace(p5, p2, p1, n3, _color, uv3, uv1, uv2, texture_id),
		TexturedModelFace(p4, p3, p7, n4, _color, uv1, uv2, uv3, texture_id), // bottom face	
		TexturedModelFace(p4, p7, p8, n4, _color, uv1, uv3, uv0, texture_id),
		TexturedModelFace(p8, p5, p1, n5, _color, uv3, uv0, uv1, texture_id), // right face
		TexturedModelFace(p8, p1, p4, n5, _color, uv3, uv1, uv2, texture_id),
		TexturedModelFace(p6, p7, p3, n6, _color, uv1, uv2, uv3, texture_id), // left face
		TexturedModelFace(p6, p3, p2, n6, _color, uv1, uv3, uv0, texture_id)
	};
	for (short i=0; i<12; i++) addTexturedModelFace(tmf[i]); 
}

BoneModel::BoneModel() : MeshModel() 
{
	buildMesh(Color(1.0f, 1.0f, 1.0f, 1.0f));
}

BoneModel::BoneModel(Color color) : MeshModel() 
{
	buildMesh(color);
}

void BoneModel::buildMesh(Color color) 
{ 
	Vector3D p1( 0.20f,  0.20f, 0.0f);
	Vector3D p2(-0.20f,  0.20f, 0.0f);
	Vector3D p3(-0.20f, -0.20f, 0.0f);
	Vector3D p4( 0.20f, -0.20f, 0.0f);
	Vector3D p5( 0.01f,  0.01f, 1.0f);
	Vector3D p6(-0.01f,  0.01f, 1.0f);
	Vector3D p7(-0.01f, -0.01f, 1.0f);
	Vector3D p8( 0.01f, -0.01f, 1.0f);
	
	Vector3D n1( 0.0f,  0.0f, -1.0f); // base face
	Vector3D n2( 0.0f,  0.0f,  1.0f); // tip face
	Vector3D n3( 0.0f,  1.0f,  0.0f); // up face
	Vector3D n4( 0.0f, -1.0f,  0.0f); // down face
	Vector3D n5( 1.0f,  0.0f,  0.0f); // right face
	Vector3D n6(-1.0f,  0.0f,  0.0f); // left face

	ModelFace mf[12] = {
		ModelFace(p1, p2, p4, n1, color), // base face		
		ModelFace(p2, p3, p4, n1, color),
		ModelFace(p6, p5, p8, n2, color), // tip face		
		ModelFace(p8, p7, p6, n2, color),	
		ModelFace(p1, p5, p6, n3, color), // up face	
		ModelFace(p6, p2, p1, n3, color),
		ModelFace(p3, p7, p8, n4, color), // down face	
		ModelFace(p8, p4, p3, n4, color),
		ModelFace(p5, p1, p4, n5, color), // right face
		ModelFace(p4, p8, p5, n5, color),
		ModelFace(p2, p6, p7, n6, color), // left face
		ModelFace(p7, p3, p2, n6, color)
	};
	for (short i=0; i<12; i++) addModelFace(mf[i]); 
}

GroundModel::GroundModel() : MeshModel() 
{
	buildMesh();
}

void GroundModel::buildMesh() 
{ 
	float tile_width = 10.0f;
	Vector3D normal( 0.0f,  1.0f,  0.0f); // up face
	Color c1(1.0f, 1.0f, 1.0f, 1.0f);
	Color c2(0.5f, 0.5f, 0.5f, 1.0f);
	Color c(1.0f, 1.0f, 1.0f, 1.0f);

	int color_switch = 0;
	for (float x=-10*tile_width; x<=10*tile_width; x+=tile_width)
	{
		for (float z=-10*tile_width; z<=10*tile_width; z+=tile_width)
		{
			Vector3D p1(x,            -0.01f, z);
			Vector3D p2(x,            -0.01f, z+tile_width);
			Vector3D p3(x+tile_width, -0.01f, z+tile_width);
			Vector3D p4(x+tile_width, -0.01f, z);
			if (color_switch == 0)
			{
				c = c1; color_switch = 1;
			}
			else
			{
				c = c2; color_switch = 0;
			}
			ModelFace mf[2] = {
				ModelFace(p1, p2, p3, normal, c),
				ModelFace(p3, p4, p1, normal, c)
			};
			for (short i=0; i<2; i++) addModelFace(mf[i]); 
		}
	}
}

XYPlaneModel::XYPlaneModel() : MeshModel()
{
	buildMesh(1.0f, 10, Color(1.0f, 1.0f, 1.0f, 1.0f));
}

XYPlaneModel::XYPlaneModel(float _size, short _faces, Color _color) : MeshModel()
{
	buildMesh(_size, _faces, _color);
}

void XYPlaneModel::buildMesh(float _size, short _faces, Color _color)
{
	float tile_width = _size / _faces;

	Vector3D normalFront(0.0f, 0.0f,  1.0f);
	Vector3D normalBack(0.0f, 0.0f, -1.0f);

	float x = -(_size / 2.0f);
	for (short i = 0; i<_faces; i++)
	{
		float y = -(_size / 2.0f);
		for (short j = 0; j<_faces; j++)
		{
			Vector3D p1(x, y, 0.0f);
			Vector3D p2(x, y + tile_width, 0.0f);
			Vector3D p3(x + tile_width, y + tile_width, 0.0f);
			Vector3D p4(x + tile_width, y, 0.0f);
			ModelFace mf[4] = {
				ModelFace(p1, p2, p3, normalFront, _color),
				ModelFace(p3, p4, p1, normalFront, _color),
				ModelFace(p1, p3, p2, normalBack, _color),
				ModelFace(p3, p1, p4, normalBack, _color)
			};
			for (short i = 0; i<4; i++) addModelFace(mf[i]);
			y += tile_width;
		}
		x += tile_width;
	}
}

YZPlaneModel::YZPlaneModel() : MeshModel()
{
	buildMesh(1.0f, 10, Color(1.0f, 1.0f, 1.0f, 1.0f));
}

YZPlaneModel::YZPlaneModel(float _size, short _faces, Color _color) : MeshModel()
{
	buildMesh(_size, _faces, _color);
}

void YZPlaneModel::buildMesh(float _size, short _faces, Color _color)
{
	float tile_width = _size / _faces;

	Vector3D normalLeft(1.0f, 0.0f, 0.0f);
	Vector3D normalRight(-1.0f, 0.0f, 0.0f);

	float y = -(_size / 2.0f);
	for (short i = 0; i<_faces; i++)
	{
		float z = -(_size / 2.0f);
		for (short j = 0; j<_faces; j++)
		{
			Vector3D p1(0.0f, y, z);
			Vector3D p2(0.0f, y, z + tile_width);
			Vector3D p3(0.0f, y + tile_width, z + tile_width);
			Vector3D p4(0.0f, y + tile_width, z);
			ModelFace mf[4] = {
				ModelFace(p1, p2, p3, normalLeft, _color),
				ModelFace(p3, p4, p1, normalLeft, _color),
				ModelFace(p1, p3, p2, normalRight, _color),
				ModelFace(p3, p1, p4, normalRight, _color)
			};
			for (short i = 0; i<4; i++) addModelFace(mf[i]);
			z += tile_width;
		}
		y += tile_width;
	}
}

ZXPlaneModel::ZXPlaneModel() : MeshModel()
{
	buildMesh(1.0f, 10, Color(1.0f,1.0f,1.0f,1.0f));
}

ZXPlaneModel::ZXPlaneModel(float _size, short _faces, Color _color) : MeshModel()
{
	buildMesh(_size, _faces, _color);
}

void ZXPlaneModel::buildMesh(float _size, short _faces, Color _color)
{
	float tile_width = _size/_faces;

	Vector3D normalUp(0.0f, 1.0f, 0.0f);
	Vector3D normalDown(0.0f, -1.0f, 0.0f);

	float x = -(_size / 2.0f);
	for (short i=0; i<_faces; i++)
	{
		float z = -(_size / 2.0f);
		for (short j = 0; j<_faces; j++)
		{
			Vector3D p1(x, 0.0f, z);
			Vector3D p2(x, 0.0f, z + tile_width);
			Vector3D p3(x + tile_width, 0.0f, z + tile_width);
			Vector3D p4(x + tile_width, 0.0f, z);
			ModelFace mf[4] = {
				ModelFace(p1, p2, p3, normalUp, _color),
				ModelFace(p3, p4, p1, normalUp, _color),
				ModelFace(p1, p3, p2, normalDown, _color),
				ModelFace(p3, p1, p4, normalDown, _color)
			};
			for (short i = 0; i<4; i++) addModelFace(mf[i]);
			z += tile_width;
		}
		x += tile_width;
	}
}

VectorModel::VectorModel() : MeshModel() 
{
	buildMesh(1.0f, Color(1.0f, 1.0f, 1.0f, 1.0f));
}

VectorModel::VectorModel(float _scale, Color _color) : MeshModel() 
{
	buildMesh(_scale, _color);
}

void VectorModel::buildMesh(float scale, Color color) 
{ 
	logout << "VectorModel scale " << scale << endl;
	Vector3D p1( 0.20f,  0.20f, 0.0f);
	Vector3D p2(-0.20f,  0.20f, 0.0f);
	Vector3D p3(-0.20f, -0.20f, 0.0f);
	Vector3D p4( 0.20f, -0.20f, 0.0f);
	Vector3D p5( 0.01f,  0.01f, scale);
	Vector3D p6(-0.01f,  0.01f, scale);
	Vector3D p7(-0.01f, -0.01f, scale);
	Vector3D p8( 0.01f, -0.01f, scale);
	
	Vector3D n1( 0.0f,  0.0f, -1.0f); // base face
	Vector3D n2( 0.0f,  0.0f,  1.0f); // tip face
	Vector3D n3( 0.0f,  1.0f,  0.0f); // up face
	Vector3D n4( 0.0f, -1.0f,  0.0f); // down face
	Vector3D n5( 1.0f,  0.0f,  0.0f); // right face
	Vector3D n6(-1.0f,  0.0f,  0.0f); // left face

	ModelFace mf[12] = {
		ModelFace(p1, p2, p4, n1, color), // base face		
		ModelFace(p2, p3, p4, n1, color),
		ModelFace(p6, p5, p8, n2, color), // tip face		
		ModelFace(p8, p7, p6, n2, color),	
		ModelFace(p1, p5, p6, n3, color), // up face	
		ModelFace(p6, p2, p1, n3, color),
		ModelFace(p3, p7, p8, n4, color), // down face	
		ModelFace(p8, p4, p3, n4, color),
		ModelFace(p5, p1, p4, n5, color), // right face
		ModelFace(p4, p8, p5, n5, color),
		ModelFace(p2, p6, p7, n6, color), // left face
		ModelFace(p7, p3, p2, n6, color)
	};
	for (short i=0; i<12; i++) addModelFace(mf[i]); 
}

PointerModel::PointerModel() : MeshModel()
{
	buildMesh(Vector3D(1.0,1.0,1.0));
}

PointerModel::PointerModel(const Vector3D& dimensions) : MeshModel()
{
	buildMesh(dimensions);
}

void PointerModel::buildMesh(const Vector3D& dimensions)
{
	float x = dimensions.x;
	float y = dimensions.y;
	float z = dimensions.z;

	Vector3D p[4] = // vertices
	{
		Vector3D(    x,  0.0f, 0.0f), // +x = right
		Vector3D( 0.0f,     y, 0.0f), // +y = up
		Vector3D( 0.0f,  0.0f,    z), // +z = forward
		Vector3D( 0.0f,  0.0f, 0.0f)  // origin
	};
	Color c[4] = // vertex colors
	{
		Color(1.0f,0.0f,0.0f),	// right
		Color(0.0f,1.0f,0.0f),	// forward
		Color(0.0f,0.0f,1.0f),	// up
		Color(1.0f,1.0f,1.0f)	// origin
	};
	Vector3D n[4] = // normals
	{
		Vector3D( -1.0f, 0.0f, 0.0f),    // y-z plane
		Vector3D( 0.0f, -1.0f, 0.0f),	// z-x plane
		Vector3D( 0.0f, 0.0f, -1.0f),	// x-y plane
		Vector3D( -1.0f, -1.0f, -1.0f)
	};

	// x-y plane
	//	PNDS_VERTEX(p[3], n[2], c[3]), 
    //  PNDS_VERTEX(p[1], n[2], c[1]),
    //  PNDS_VERTEX(p[0], n[2], c[0])));
	PointColorModelFace pcmf1(p[3], p[1], p[0], n[2], c[3], c[1], c[0]); 
	addPointColorModelFace(pcmf1); 

	// y-z plane
	//	PNDS_VERTEX(p[3], n[0], c[3]),
    //  PNDS_VERTEX(p[2], n[0], c[2]),
    //  PNDS_VERTEX(p[1], n[0], c[1])));
	PointColorModelFace pcmf2(p[3], p[2], p[1], n[0], c[3], c[2], c[1]); 
	addPointColorModelFace(pcmf2); 

	// z-x plane
	//	PNDS_VERTEX(p[3], n[1], c[3]),
    //  PNDS_VERTEX(p[0], n[1], c[0]),
    //  PNDS_VERTEX(p[2], n[1], c[2])));
	PointColorModelFace pcmf3(p[3], p[0], p[2], n[1], c[3], c[0], c[2]); 
	addPointColorModelFace(pcmf3); 
	// hood
	//	PNDS_VERTEX(p[0], n[3], c[0]),
    //  PNDS_VERTEX(p[1], n[3], c[1]),
    //  PNDS_VERTEX(p[2], n[3], c[2])));
	PointColorModelFace pcmf4(p[0], p[1], p[2], n[3], c[0], c[1], c[2]); 
	addPointColorModelFace(pcmf4); 
}

Flex13CameraModel::Flex13CameraModel() : MeshModel()
{   
	Color c(1.0f,0.0f,0.0f);
	buildMesh(c);
}

Flex13CameraModel::Flex13CameraModel(Color& c) : MeshModel()
{
	buildMesh(c);
}

void Flex13CameraModel::buildMesh(Color& c)
{
	float hfov = deg2rad(56.0f);
	float vfov = deg2rad(46.0f); // degrees
	float depth = 10.0f; // feet

	float y = tanf(vfov/2.0f) * depth;
	float x = tanf(hfov/2.0f) * depth;

	// set alpha
	c.a = 0.5f;

	Vector3D p[5] = // vertices
	{
		Vector3D(   x,    y, depth), 
		Vector3D(  -x,    y, depth), 
		Vector3D(  -x,   -y, depth), 
		Vector3D(   x,   -y, depth),
		Vector3D(0.0f, 0.0f, 0.0f)
	};

	Vector3D n[4] = // normals (currently wrong)
	{
		Vector3D( 1.0f, 0.0f, 0.0f),    // right
		Vector3D( 0.0f, 1.0f, 0.0f),	// up
		Vector3D(-1.0f, 0.0f, 0.0f),	// left
		Vector3D( 0.0f,-1.0f, 0.0f)     // down
	};

	// up face
	ModelFace mf1(p[4], p[0], p[1], n[1], c); 
	addModelFace(mf1);
	// right face
	ModelFace mf2(p[4], p[3], p[0], n[0], c); 
	addModelFace(mf2);
	// down face
	ModelFace mf3(p[4], p[2], p[3], n[3], c); 
	addModelFace(mf3);
	// left face
	ModelFace mf4(p[4], p[1], p[2], n[2], c); 
	addModelFace(mf4);
}
