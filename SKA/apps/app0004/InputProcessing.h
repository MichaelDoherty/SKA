//-----------------------------------------------------------------------------
// app0004 - Builds with SKA Version 4.0
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
