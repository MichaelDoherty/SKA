//-----------------------------------------------------------------------------
// ModelFactory.cpp
//    Single instance class for creating models from model specifications.
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
// Version 3.0 - July 18, 2014 - Michael Doherty
//-----------------------------------------------------------------------------
#include "Core/SystemConfiguration.h"
#include "Models/ModelFactory.h"
#include "Models/CodedModels.h"
#include <cstdlib>

ModelFactory model_factory;

Model* ModelFactory::buildModel(ModelSpecification& spec)
{
	// IMPROVEIT! ModelFactory should only create one instance of each model type

	if (spec.model_name == string("Bone"))
		return new BoneModel(spec.color);
	else if (spec.model_name == string("Ground"))
		return new GroundModel();
	else if (spec.model_name == string("Pointer"))
	{
		float length = 1.0f;
		float width = 1.0f;
		float height = 1.0f;
		for (unsigned short i=0; i<spec.specs.size(); i++)
		{
			if (spec.specs[i].first == string("length"))
				length = (float)atof(spec.specs[i].second.c_str());
			else if (spec.specs[i].first == string("width"))
				width = (float)atof(spec.specs[i].second.c_str());
			else if (spec.specs[i].first == string("height"))
				height = (float)atof(spec.specs[i].second.c_str());
		}
		return new PointerModel(Vector3D(width, height, length));
	}
	else if (spec.model_name == string("CoordinateAxis"))
	{
		float length = 1.0;
		for (unsigned short i=0; i<spec.specs.size(); i++)
			if (spec.specs[i].first == string("length"))
				length = (float)atof(spec.specs[i].second.c_str());
		return new CoordinateAxisModel(length);
	}
	else if (spec.model_name == string("Box"))
	{
		float length = 1.0f;
		float width = 1.0f;
		float height = 1.0f;
		for (unsigned short i=0; i<spec.specs.size(); i++)
		{
			if (spec.specs[i].first == string("length"))
				length = (float)atof(spec.specs[i].second.c_str());
			else if (spec.specs[i].first == string("width"))
				width = (float)atof(spec.specs[i].second.c_str());
			else if (spec.specs[i].first == string("height"))
				height = (float)atof(spec.specs[i].second.c_str());
		}
		return new BoxModel(width, height, length, spec.color);
	}
	else
		return NULL;
}
