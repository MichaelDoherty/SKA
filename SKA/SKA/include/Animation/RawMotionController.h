//-----------------------------------------------------------------------------
// RawMotionController.h
//    Controller designed for frame-by-frame access to
//    motions defined directly by a motion sequence.
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

#ifndef RAWMOTIONCONTROLLER_DOT_H
#define RAWMOTIONCONTROLLER_DOT_H
#include <Core/SystemConfiguration.h>
#include <Animation/MotionController.h>
#include <Animation/MotionSequence.h>

class SKA_LIB_DECLSPEC RawMotionController : public MotionController
{
public:
	RawMotionController() 
		: MotionController(), motion_sequence(NULL)
	{ }
	RawMotionController(MotionSequence* _ms) 
		: MotionController(), motion_sequence(_ms)
	{ }
	
	virtual ~RawMotionController() { }
	
	virtual float getValue(CHANNEL_ID _channel, float _time);

	virtual bool isValidChannel(CHANNEL_ID _channel);
	virtual bool isValidChannel(CHANNEL_ID _channel, float _time)
	{
		return isValidChannel(_channel);
	}

	virtual float getValueByFrame(CHANNEL_ID _channel, int _frame);
	virtual int numFrames() { return motion_sequence->numFrames(); }

private:
	MotionSequence* motion_sequence;
};

#endif
