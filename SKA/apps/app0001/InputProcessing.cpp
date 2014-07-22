//-----------------------------------------------------------------------------
// app0001: Demo program illustrating how to build a basic SKA application.
//          This application reads a skeleton and motion from an ASF/AMC 
//          file pair and uses that data to drive a character.
//-----------------------------------------------------------------------------
// InputProcessing.cpp
//    Look for an escape key and use it to exit the application.
//-----------------------------------------------------------------------------
// Builds with SKA Version 3.0 - July 22, 2012 - Michael Doherty
//-----------------------------------------------------------------------------
// SKA configuration - should always be the first file included.
#include <Core/SystemConfiguration.h>
// SKA modules
#include <Input/InputManager.h>
// local application
#include "InputProcessing.h"

#define ESC 27 // ASCII code for the escape key.

// global single instance of the input processor
InputProcessor input_processor;

void InputProcessor::processInputs()
{ 
	InputActions* actions = input_manager.getInput();
	for (short i=0; i<actions->num_keys_pressed; i++)
	{
		switch (actions->keys_pressed[i])
		{
		case ESC: exit(0); break;
		}
	}
}

