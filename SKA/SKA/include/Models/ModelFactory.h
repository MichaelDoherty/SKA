//-----------------------------------------------------------------------------
// ModelFactory.h
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

#ifndef MODELFACTORY_DOT_H
#define MODELFACTORY_DOT_H
#include <Core/SystemConfiguration.h>
#include <Models/Models.h>

// Future Improvement: The model factory should recognize when it is asked to build duplicate models
// and return a reference to previously constructed models when possible.

class SKA_LIB_DECLSPEC ModelSpecification
{
public:
	ModelSpecification(const char* _model_name);
	ModelSpecification(const char* _model_name, const Color _color);
	~ModelSpecification();
	void addSpec(const char* _key, const char* _value);

	char* getModelName();
	Color getColor();
	char* getSpec(const char* _key);

private:
	struct ModelSpecData* data;
};

class ModelFactory
{
public:
	ModelFactory() { }
	virtual ~ModelFactory() { }
	Model* buildModel(ModelSpecification& spec);
};

extern ModelFactory model_factory;

#endif