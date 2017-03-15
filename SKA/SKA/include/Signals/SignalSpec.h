//-----------------------------------------------------------------------------
// SignalSpec.h
//   Simple structure to define a sine wave signal.
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

#ifndef SIGNALSPEC_DOT_H
#define SIGNALSPEC_DOT_H
#include <Core/SystemConfiguration.h>

struct SignalSpec
{
	float amplitude;
	float frequency;
	float phase;
};

#endif
