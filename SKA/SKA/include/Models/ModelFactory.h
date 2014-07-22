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
// Version 3.0 - July 18, 2014 - Michael Doherty
//-----------------------------------------------------------------------------
#ifndef MODELFACTORY_DOT_H
#define MODELFACTORY_DOT_H
#include "Core/SystemConfiguration.h"
#include "Models/Models.h"

// IMPROVEIT! ModelFactory should only create one instance of each model type

class ModelFactory
{
public:
	ModelFactory() { }
	virtual ~ModelFactory() { }
	Model* buildModel(ModelSpecification& spec);
};

extern ModelFactory model_factory;

#endif