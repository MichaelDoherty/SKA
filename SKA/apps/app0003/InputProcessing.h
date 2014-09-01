//-----------------------------------------------------------------------------
// app0003 - Builds with SKA Version 3.1 - Sept 01, 2012 - Michael Doherty
//-----------------------------------------------------------------------------
// InputProcessing.h
//    Object to monitor user input from mouse and keyboard.
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
