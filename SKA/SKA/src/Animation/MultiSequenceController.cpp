//-----------------------------------------------------------------------------
// MultiSequenceController.cpp
//    Class MultiSequenceController cycles between 
//    multiple MotionSequenceControllers at some fixed time interval.
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
#include <Animation/MultiSequenceController.h>

bool MultiSequenceController::isValidChannel(CHANNEL_ID _channel, float _time)
{
	short i = selectController(_time);
	return seq_controllers[i]->isValidChannel(_channel, _time);
}

float MultiSequenceController::getValue(CHANNEL_ID _channel, float _time)
{
	short i = selectController(_time);
	return  seq_controllers[i]->getValue(_channel, _time);
}

short MultiSequenceController::selectController(float _time)
{
	if (_time >= next_switch_time) switchController(_time);
	return active_controller;
}

void MultiSequenceController::switchController(float _time)
{
	active_controller = (active_controller+1)%num_controllers;
	next_switch_time = _time + cycle_time;
}