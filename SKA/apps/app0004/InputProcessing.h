//-----------------------------------------------------------------------------
// app0004 - Builds with SKA Version 3.1 - Sept 01, 2012 - Michael Doherty
//-----------------------------------------------------------------------------
// InputProcessing.h
//-----------------------------------------------------------------------------
#ifndef INPUTPROCESSING_DOT_H
#define INPUTPROCESSING_DOT_H
#include "Core/SystemConfiguration.h"
#include "Input/InputFilter.h"

#define ESC 27
#define SPACE 32

class InputProcessor
{
private:
	InputFilter filter;
public:
	InputProcessor();
	void processInputs(float elapsed_time);
};

#endif // INPUTPROCESSING_DOT_H
