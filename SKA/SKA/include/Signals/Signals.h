//-----------------------------------------------------------------------------
// Signals.h
//   Signal generation classes.
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

#ifndef SIGNALS_DOT_H
#define SIGNALS_DOT_H
#include <Core/SystemConfiguration.h>
#include <cmath>
#include <vector>
#include <iostream>
using namespace std;
#include <Math/Math.h>
#include <Signals/SignalSpec.h>

class SignalGenerator
{
public:
	SignalGenerator() { }
	SignalGenerator(const SignalGenerator& s) { }
	SignalGenerator& operator=(const SignalGenerator& rhs) { return *this; }
	virtual ~SignalGenerator() { }

	virtual float signal(float time) 
	{
		return 0.0f;
	}
};

class ConstantGenerator : public SignalGenerator
{
private:
	float value;
public:
	ConstantGenerator() 
		: value(0.0f) 
	{ }
	ConstantGenerator(float v) 
		: value(v) 
	{ }
	ConstantGenerator(const ConstantGenerator& s) 
		: value(s.value)
	{ }
	ConstantGenerator& operator=(const ConstantGenerator& rhs) 
	{ 
		value = rhs.value;
		return *this; 
	}
	virtual ~ConstantGenerator() { }

	virtual float signal(float time) 
	{
		return value;
	}
};

class SineGenerator : public SignalGenerator
{
private:
	float amplitude; // unit
	float frequency; // cycles / sec
	float phase;     // in radians
public:
	SineGenerator()
		: amplitude(1.0f), frequency(1.0f), phase(0.0f)
	{}
	
	SineGenerator(float a, float f, float p=0.0f)
		: amplitude(a), frequency(f), phase(p)
	{}

	SineGenerator(const SineGenerator& s)
		:  amplitude(s.amplitude), frequency(s.frequency), phase(s.phase)
	{}

	SineGenerator& operator=(const SineGenerator& rhs)
	{
		amplitude = rhs.amplitude;
		frequency = rhs.frequency;
		phase = rhs.phase;
		return *this;
	}

	virtual ~SineGenerator() { }

	virtual float signal(float time)
	{
		return amplitude*sin(phase+TWO_PI*time*frequency);
	}
};

class CosineGenerator : public SignalGenerator
{
private:
	float amplitude; // unit
	float frequency; // cycles / sec
	float phase;     // in radians
public:
	CosineGenerator()
		: amplitude(1.0f), frequency(1.0f), phase(0.0f)
	{}
	
	CosineGenerator(float a, float f, float p=0.0f)
		: amplitude(a), frequency(f), phase(p)
	{}

	CosineGenerator(const CosineGenerator& s)
		:  amplitude(s.amplitude), frequency(s.frequency), phase(s.phase)
	{}

	CosineGenerator& operator=(const CosineGenerator& rhs)
	{
		amplitude = rhs.amplitude;
		frequency = rhs.frequency;
		phase = rhs.phase;
		return *this;
	}

	virtual ~CosineGenerator() { }

	virtual float signal(float time)
	{
		return amplitude*cos(phase + TWO_PI*time*frequency);
	}
};

class SignalAdder : public SignalGenerator
{
private:
	vector<SignalGenerator*> inputs;
public:
	SignalAdder()
	{
	}

	SignalAdder(SignalGenerator* sg1, SignalGenerator* sg2)
	{
		inputs.push_back(sg1);
		inputs.push_back(sg2);
	}

	SignalAdder(const SignalAdder& s)
	{
		inputs = s.inputs;
	}

	SignalAdder& operator=(const SignalAdder& rhs)
	{
		inputs = rhs.inputs;
		return *this;
	}

	virtual ~SignalAdder() 
	{ 
		for (unsigned short i=0; i<inputs.size(); i++)
			delete inputs[i];
	}

	void addInput(SignalGenerator* sg)
	{
		inputs.push_back(sg);
	}

	virtual float signal(float time)
	{
		float s = 0.0f;
		for (unsigned short i=0; i<inputs.size(); i++)
			s += inputs[i]->signal(time);
		return s;
	}
};

void testSignalGenerator();

SignalGenerator* buildSignalGenerator(vector<SignalSpec>& specs);

#endif
