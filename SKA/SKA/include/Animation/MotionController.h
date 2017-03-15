//-----------------------------------------------------------------------------
// MotionController.h
//    Abstract base class for all motion controllers
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

#ifndef MOTIONCONTROLLER_DOT_H
#define MOTIONCONTROLLER_DOT_H
#include <Core/SystemConfiguration.h>
#include <Animation/Channel.h>

class SKA_LIB_DECLSPEC MotionController
{
public:
	MotionController() { }
	virtual ~MotionController() { }

	// determine if _channel is controlled by this controller.
	// _time is neccessary to support controllers that may change
	//       the set of active controllers dynamically.
	// _time is world time in seconds
	virtual bool isValidChannel(CHANNEL_ID _channel, float _time) = 0;

	// get the value for _channel at _time 
	// _time is world time in seconds
	virtual float getValue(CHANNEL_ID _channel, float _time) = 0;
};

#endif
