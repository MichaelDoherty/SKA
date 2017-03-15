//-----------------------------------------------------------------------------
// InputFilter.h
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

#ifndef INPUTFILTER_DOT_H
#define INPUTFILTER_DOT_H
#include <Core/SystemConfiguration.h>

enum InputFilterType { KEYBOARD, JOYSTICK, MOUSE };

class InputFilterStorage;

class SKA_LIB_DECLSPEC InputFilter
{
public:
	InputFilter();
	virtual ~InputFilter();

	void addFilter(unsigned char c, float delay, InputFilterType _type=KEYBOARD);
	void clearKeyFilters();

	void clearActiveKeys();
	unsigned char testInput(unsigned char c, InputFilterType _type=KEYBOARD);

	void advanceTime(float elapsed_time);

private:
	InputFilterStorage* store;
};

#endif
