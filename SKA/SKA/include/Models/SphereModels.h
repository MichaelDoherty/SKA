//-----------------------------------------------------------------------------
// SphereModels.h
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

#ifndef SPHEREMODELS_DOT_H
#define SPHEREMODELS_DOT_H
#include <Core/SystemConfiguration.h>
#include <Models/Models.h>

class SKA_LIB_DECLSPEC SphereModel : public MeshModel
{
public:
	SphereModel(float _radius, short _detail_level,
		Color _color, char* _texture_file=NULL);
	virtual ~SphereModel() { }
private:
	virtual void buildMesh(float _radius, short _detail_level,
		Color _color, char* _texture_file);
};

class SKA_LIB_DECLSPEC InvertedSphereModel : public SphereModel
{
public:
	InvertedSphereModel(float _radius, short _detail_level,
		Color _color, char* _texture_file=NULL);
	virtual ~InvertedSphereModel() { }
};

#endif
