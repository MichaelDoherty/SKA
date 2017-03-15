//-----------------------------------------------------------------------------
// InputFilter.cpp
//	 Class to filter out continuously pressed keys, 
//   when single strokes are expected.
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
#include <Input/InputFilter.h>
#include <list>
using namespace std;

struct InputDelayPair
{
	InputFilterType input_type;
	unsigned char c;
	float delay;
	InputDelayPair(unsigned char _c, float _delay, InputFilterType _type) 
		: input_type(_type), c(_c), delay(_delay) { }
};

class InputFilterStorage
{
public:
	list<InputDelayPair> active_keys;   // keys currently filtered
	list<InputDelayPair> key_filters;   // keys that should be filtered
};

InputFilter::InputFilter()
{ 
	store = new InputFilterStorage;
}

InputFilter::~InputFilter()
{ 
	delete store;
}

void InputFilter::addFilter(unsigned char c, float delay, InputFilterType _type)
{
	store->key_filters.push_back(InputDelayPair(c, delay, _type));
}

void InputFilter::advanceTime(float elapsed_time)
{
	list<InputDelayPair>::iterator iter = store->active_keys.begin();
	while (iter!=store->active_keys.end())
	{
		(*iter).delay -= elapsed_time;
		if ((*iter).delay <= 0.0f) store->active_keys.erase(iter++);
		else iter++;
	}
}

unsigned char InputFilter::testInput(unsigned char c, InputFilterType _type)
{
	list<InputDelayPair>::iterator iter;
	// check if key is in active list
	for (iter=store->active_keys.begin(); iter!=store->active_keys.end(); iter++)
	{
		if ((c == (*iter).c) && (_type == (*iter).input_type)) return -1;
	}
	// check if key should be added to active list
	for (iter=store->key_filters.begin(); iter!=store->key_filters.end(); iter++)
	{
		if ((c == (*iter).c) && (_type == (*iter).input_type))
		{ 
			store->active_keys.push_back(InputDelayPair(c, (*iter).delay, _type));
			return c;
		}
	}
	// if not active or filtered, allow the key
	return c;
}

void InputFilter::clearKeyFilters() { store->key_filters.clear(); }

void InputFilter::clearActiveKeys() { store->active_keys.clear(); }