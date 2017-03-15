//-----------------------------------------------------------------------------
// RawMotionController.cpp
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

#include <Core/SystemConfiguration.h>
#include <Animation/RawMotionController.h>
#include <Animation/AnimationException.h>

bool RawMotionController::isValidChannel(CHANNEL_ID _channel)
{	if (motion_sequence == NULL) 
	{
		throw AnimationException("RawMotionController has no attached MotionSequence");
		return false;
	}
	return motion_sequence->isValidChannel(_channel);
}

float RawMotionController::getValue(CHANNEL_ID _channel, float _time)
{
	if (motion_sequence == NULL) 
	{
		throw AnimationException("RawMotionController has no attached MotionSequence");
		return 0.0f;
	}
	
	//float duration = motion_sequence->getDuration();
	int num_frames = motion_sequence->numFrames();
	float frame_rate = motion_sequence->getFrameRate();

	int abs_frame = int(_time / frame_rate);
	int frame = abs_frame % num_frames;

	return getValueByFrame(_channel, frame);
}


float RawMotionController::getValueByFrame(CHANNEL_ID _channel, int _frame)
{
	return motion_sequence->getValue(_channel, _frame);
}
