//-----------------------------------------------------------------------------
// app0002: Demo program illustrating various useful things that aren't
//          directly related to the character animation.
//          (1) additional objects, such as ground, sky and coordinate axes
//          (2) moveable camera, controlled by the camera and mouse.
//          (3) keyboard filtering, to avoid multiple responses when
//              a single keystroke is expected.
//          (4) animation speed control (freeze, single step, time warp)
//          (5) heads-up display (2D text on screen)
//-----------------------------------------------------------------------------
// InputProcessing.h
//    Object to monitor user input from mouse and keyboard.
//-----------------------------------------------------------------------------
// Builds with SKA Version 3.0 - July 22, 2012 - Michael Doherty
//-----------------------------------------------------------------------------
#ifndef INPUTPROCESSING_DOT_H
#define INPUTPROCESSING_DOT_H
// SKA configuration
#include <Core/SystemConfiguration.h>

class InputProcessor
{
public:
	InputProcessor();
	~InputProcessor();
	// processInputs() checks input queues for keyboard and mouse
	// Requires elapsed time to check key press times for filtered keys
	void processInputs(float elapsed_time);
};

extern InputProcessor input_processor;

#endif // INPUTPROCESSING_DOT_H
