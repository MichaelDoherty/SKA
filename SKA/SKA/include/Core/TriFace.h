#ifndef TRIFACE_DOT_H
#define TRIFACE_DOT_H
#include "Core/SystemConfiguration.h"

//=====================================================================
// TriFace - Used to export faces for use outside of graphics

struct TriFace
{
	Vector3D v0;
	Vector3D v1;
	Vector3D v2;
	Vector3D normal;

	TriFace() { }

	TriFace(Vector3D _v0, Vector3D _v1, Vector3D _v2)
		: v0(_v0), v1(_v1), v2(_v2)
	{
		Vector3D p1 = v1-v0;
		Vector3D p2 = v2-v0;
		normal = p1.cross(p2);
		normal = normal.normalize();
	}
	virtual ~TriFace() { }
};

struct TriFaceCollector;

class TriFaceList
{
public:
	TriFaceList();
	virtual ~TriFaceList();
	long size();
	TriFace faceAt(long i);
	void addFace(TriFace& _face);
	void addFaceList(TriFaceList& _other);
	void clear();
private:
	TriFaceCollector* collector;
};

#endif