//-----------------------------------------------------------------------------
// app0001: Demo program illustrating how to build a basic SKA application.
//          This application reads a skeleton and motion from an ASF/AMC 
//          file pair and uses that data to drive a character.
//-----------------------------------------------------------------------------
// InputProcessing.h
//    Object to monitor user input from mouse and keyboard.
//-----------------------------------------------------------------------------
// Builds with SKA Version 3.0 - July 22, 2012 - Michael Doherty
//-----------------------------------------------------------------------------
#ifndef INPUTPROCESSING_DOT_H
#define INPUTPROCESSING_DOT_H
// SKA configuration - should always be the first file included.
#include <Core/SystemConfiguration.h>

class InputProcessor
{
public:
	InputProcessor() {}
	// processInputs() checks input queues for keyboard 
	// and mouse actions received from GLUT.
	// It should be called regularly, at least once per frame.
	void processInputs();
};

// global single instance of the input processor
extern InputProcessor input_processor;

#endif // INPUTPROCESSING_DOT_H
