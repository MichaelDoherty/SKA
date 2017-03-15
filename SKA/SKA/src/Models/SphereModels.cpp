//-----------------------------------------------------------------------------
// SphereModels.cpp
//    Generated models for spheres and inverted spheres.
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
#include <Models/SphereModels.h>
#include <Math/Math.h>
#include <Graphics/Textures.h>

//=====================================================================

//-----------------------------------------------------------------------------
// generateSphere()
//
// Generates a triangle mesh approximating a sphere by
// recursive subdivision. First approximation is an octahedron;
// each level of refinement increases the number of triangles by
// a factor of 4. The number of triangles is 8*(4^level).
//
// Level 2 (128 triangles) is a good tradeoff if gouraud
// shading is used to render the database.
//
// ====== subdivision algorithm ===================================
//		
// Subdivide each triangle in the old approximation
//   and normalize the new points.
// Each input triangle with vertices labelled [p0,p1,p2] as shown
//  below will be turned into four new triangles:
//
// Make new points: a = (p0+p2)/2, b = (p0+p1)/2, c = (p1+p2)/2
/*
         p1
         /\		Normalize a, b, c
        /  \
      b/____\ c	    Construct new triangles
      /\    /\          [p0,b,a]
     /	\  /  \         [b,p1,c]
    /____\/____\        [a,b,c]
   p0    a     p2       [a,c,p2]
*/
//
// ================================================================
//
// The subdivision algorithm was based on code
// by Jon Leech (leech@cs.unc.edu) 3/24/89.
// Leech's code was found on the Graphics Gems web site.

struct Triangle {
    Vector3D pt[3];
	Triangle() { }
	Triangle(Vector3D& v1, Vector3D& v2, Vector3D& v3)
	{ pt[0] = v1; pt[1] = v2; pt[2] = v3; }
};

// Six equidistant points lying on the unit sphere 
static Vector3D XPLUS(  1.0f,  0.0f,  0.0f);
static Vector3D XMINUS(-1.0f,  0.0f,  0.0f);
static Vector3D YPLUS(  0.0f,  1.0f,  0.0f);
static Vector3D YMINUS( 0.0f, -1.0f,  0.0f);
static Vector3D ZPLUS(  0.0f,  0.0f,  1.0f);
static Vector3D ZMINUS( 0.0f,  0.0f, -1.0f);

// Normalize a point p 
static Vector3D normalize(const Vector3D& p)
{
    Vector3D r(p);
    double mag;
    mag = r.x * r.x + r.y * r.y + r.z * r.z;
    if (mag > 0.0f) r /= float(sqrt(mag));
    return r;
}

// Return the midpoint on the line between two points 
static Vector3D midpoint(Vector3D& a, Vector3D& b)
{
    return Vector3D( (a.x + b.x) * 0.5f, (a.y + b.y) * 0.5f, (a.z + b.z) * 0.5f);
}

static Triangle* generateSphere(int maxlevel, int& num_triangles)
{
	// level 0 sphere is an octahedron
    num_triangles = 8;
	Triangle* sphere = new Triangle[8];
	sphere[0] = Triangle( XPLUS,  ZPLUS,  YPLUS  );
	sphere[1] = Triangle( YPLUS,  ZPLUS,  XMINUS );
	sphere[2] = Triangle( XMINUS, ZPLUS,  YMINUS );
    sphere[3] = Triangle( YMINUS, ZPLUS,  XPLUS  );
    sphere[4] = Triangle( XPLUS,  YPLUS,  ZMINUS );
    sphere[5] = Triangle( YPLUS,  XMINUS, ZMINUS );
    sphere[6] = Triangle( XMINUS, YMINUS, ZMINUS );
    sphere[7] = Triangle( YMINUS, XPLUS,  ZMINUS );

    // Subdivide each starting triangle maxlevel times 
    for (int level = 1; level<=maxlevel; level++) 
	{
		// Allocate 4 times the number of points in the current approximation 
		int old_size = num_triangles;
		num_triangles *= 4;

		Triangle* old_sphere = sphere;
		sphere  = new Triangle[num_triangles];

		if (sphere == NULL) 
			throw BasicException("SphereModels: Out of memory in sphere generator");

		for (int i = 0; i<old_size; i++) 
		{
			Vector3D p0 = old_sphere[i].pt[0];
			Vector3D p1 = old_sphere[i].pt[1];
			Vector3D p2 = old_sphere[i].pt[2];

			Vector3D a = normalize(midpoint(p0,p2));
			Vector3D b = normalize(midpoint(p0,p1));
			Vector3D c = normalize(midpoint(p1,p2));

			int j = i*4;
			sphere[j]   = Triangle(p0, b, a);
			sphere[j+1] = Triangle(b, p1, c);
			sphere[j+2] = Triangle(a, b, c);
			sphere[j+3] = Triangle(a, c, p2);
		}

		delete [] old_sphere;
    }

    return sphere;
}


SphereModel::SphereModel(float _radius, short _detail_level,
		Color _color, char* _texture_file)
{
	buildMesh(_radius, _detail_level, _color, _texture_file);
}

void SphereModel::buildMesh(float _radius, short _detail_level,
		Color _color, char* _texture_file)
{
	bool use_texture = (_texture_file != NULL);

	int num_triangles;
	Triangle* tri = generateSphere(_detail_level, num_triangles);

	short texture_id;
	if (use_texture)
		texture_id = texture_manager.loadTextureBMP(_texture_file);

	for (int f=0; f<num_triangles; f++)
	{
		Vector3D n[3];
		Vector3D p[3];
		Point2D uv[3];

		// Need 3 normals for smoooth shading.
		//Currently can only store one
		n[0] = tri[f].pt[0];
		n[1] = tri[f].pt[1];
		n[2] = tri[f].pt[2];

		p[0] = (tri[f].pt[0] * _radius);
		p[1] = (tri[f].pt[1] * _radius);
		p[2] = (tri[f].pt[2] * _radius);

		if (use_texture)
		{
			float tu[3], tv[3];

			// wrap the map counterclockwise around the x,z equator of the sphere,
			// and fold back up to the poles as abs(y) increases.
			// This compresses the texture at the poles, so the flat texture needs to 
			// be stretched (as a flat map of the earth is).
			for (short i=0; i<3; i++)
			{
				float theta;
				float x=n[i].x, y = n[i].y, z = n[i].z;

				theta = atan2f(z,x);
				if (z < 0.0f) theta += 2.0f*PI;

				tu[i] = theta/(2.0f*PI); 
				if (tu[i] < 0.0) tu[i] = 0.0;
				if (tu[i] > 1.0) tu[i] = 1.0;

				tv[i] = 1.0f - (y+1.0f)/2.0f;
			}

			// This corrects for wrapping around at the end of the texture.
			// As theta comes around to 2PI, it resets to 0 (as it should).
			// But then the other side of that triangle is still something < 2PI and
			// the texture map tries to map back across the entire map.
			// This forces those final thetas back to 2PI (1.0 on the map.)
			if ((tu[0] > 0.5f) || (tu[1] > 0.5f) || (tu[2] > 0.5f))
			{
				if (tu[0] == 0.0f) tu[0] = 1.0f;
				if (tu[1] == 0.0f) tu[1] = 1.0f;
				if (tu[2] == 0.0f) tu[2] = 1.0f;
			}
			
			// Now correct for the poles
			if (tv[0] <= 0.0f) { tu[0] = (tu[1]+tu[2])/2.0f; tv[0] = 0.005f; }
			if (tv[1] <= 0.0f) { tu[1] = (tu[0]+tu[2])/2.0f; tv[1] = 0.005f; }
			if (tv[2] <= 0.0f) { tu[2] = (tu[0]+tu[1])/2.0f; tv[2] = 0.005f; }
			if (tv[0] >= 1.0f) { tu[0] = (tu[1]+tu[2])/2.0f; tv[0] = 0.995f; }
			if (tv[1] >= 1.0f) { tu[1] = (tu[0]+tu[2])/2.0f; tv[1] = 0.995f; }
			if (tv[2] >= 1.0f) { tu[2] = (tu[0]+tu[1])/2.0f; tv[2] = 0.995f; }

			uv[0] = Point2D(1-tu[0],tv[0]);
			uv[1] = Point2D(1-tu[1],tv[1]);
			uv[2] = Point2D(1-tu[2],tv[2]);
		}

		if (!use_texture)
		{
			ModelFace mface(p[0], p[1], p[2], n[0], _color);
			addModelFace(mface);
		}
		else
		{
			TexturedModelFace tmface(p[0], p[1], p[2], n[0], _color, 
				uv[0], uv[1], uv[2], texture_id);
			addTexturedModelFace(tmface);
		}

	}

	delete [] tri;
}

//=====================================================================

InvertedSphereModel::InvertedSphereModel(float _radius, 
										 short _detail_level,
										 Color _color,
										 char* _texture_file)
: SphereModel(_radius, _detail_level, _color, _texture_file)
{
	for (unsigned int f=0; f<faces->faces.size(); f++)
	{
		faces->faces[f].normal = faces->faces[f].normal * -1.0;
		Vector3D tmp = faces->faces[f].point[0];
		faces->faces[f].point[0] = faces->faces[f].point[2];
		faces->faces[f].point[2] = tmp;
		// FIXIT! also need to reverse texture coordinates.
		// FIXIT! which really means going through the tfaces list.
	}

}

//=====================================================================
