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

#include <Core/SystemConfiguration.h>
#include <cstdlib>
using namespace std;
#include <Core/Utilities.h>
#include <Models/ModelFactory.h>
#include <Models/CodedModels.h>

ModelFactory model_factory;

static bool strMatch(const char* s1, const char* s2)
{
	return strcmp(s1,s2)==0;
}

Model* ModelFactory::buildModel(ModelSpecification& spec)
{
	char* model_name = spec.getModelName();
	if (model_name == NULL) return NULL;

	if (strMatch(model_name,"Bone"))
		return new BoneModel(spec.getColor());
	else if (strMatch(model_name,"Ground"))
		return new GroundModel();
	else if (strMatch(model_name,"Pointer"))
	{
		float length = 1.0f;
		float width = 1.0f;
		float height = 1.0f;
		char* s;
		s = spec.getSpec("length");
		if (s != NULL) length = (float)atof(s);
		s = spec.getSpec("width");
		if (s != NULL) width = (float)atof(s);
		s = spec.getSpec("height");
		if (s != NULL) height = (float)atof(s);
		return new PointerModel(Vector3D(width, height, length));
	}
	else if (strMatch(model_name, "Vector"))
	{
		float length = 1.0f;
		//float width = 1.0f;
		//float height = 1.0f;
		char* s;
		s = spec.getSpec("length");
		if (s != NULL) length = (float)atof(s);
		//s = spec.getSpec("width");
		//if (s != NULL) width = (float)atof(s);
		//s = spec.getSpec("height");
		//if (s != NULL) height = (float)atof(s);
		return new VectorModel(length, spec.getColor());
	}
	else if (strMatch(model_name,"CoordinateAxis"))
	{
		float length = 1.0;
		char* s = spec.getSpec("length");
		if (s != NULL) length = (float)atof(s);
		return new CoordinateAxisModel(length);
	}
	else if (strMatch(model_name,"Box"))
	{
		float length = 1.0f;
		float width = 1.0f;
		float height = 1.0f;
		char* s;
		s = spec.getSpec("length");
		if (s != NULL) length = (float)atof(s);
		s = spec.getSpec("width");
		if (s != NULL) width = (float)atof(s);
		s = spec.getSpec("height");
		if (s != NULL) height = (float)atof(s);
		return new BoxModel(width, height, length, spec.getColor());
	}
	else
		return NULL;
}


struct ModelSpecData
{
	char* model_name;
	Color color;
	vector<pair<char*,char*> > specs;
};

ModelSpecification::ModelSpecification(const char* _model_name)
{
	data = new ModelSpecData;
	data->model_name = strClone(_model_name);
	data->color = Color(0.0f,0.0f,0.0f);
}

ModelSpecification::ModelSpecification(const char* _model_name, const Color _color)
{
	data = new ModelSpecData;
	data->model_name = strClone(_model_name);
	data->color = _color;
}

ModelSpecification::~ModelSpecification()
{
	strDelete(data->model_name);
	unsigned int i;
	for (i=0; i<data->specs.size(); i++)
	{
		strDelete(data->specs[i].first);
		strDelete(data->specs[i].second);
	}
	delete data;
}
	
void ModelSpecification::addSpec(const char* _key, const char* _value)
{
	char* key = strClone(_key);
	char* value = strClone(_value);
	data->specs.push_back(pair<char*,char*>(key, value));
}

char* ModelSpecification::getModelName()
{
	return data->model_name;
}

Color ModelSpecification::getColor()
{
	return data->color;
}

char* ModelSpecification::getSpec(const char* _key)
{
	unsigned int i;
	for (i=0; i<data->specs.size(); i++)
	{
		if (strcmp(data->specs[i].first, _key)==0)
			return data->specs[i].second;
	}
	return NULL;
}
