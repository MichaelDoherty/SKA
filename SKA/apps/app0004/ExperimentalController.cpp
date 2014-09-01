//-----------------------------------------------------------------------------
// app0004 - Builds with SKA Version 3.1 - Sept 01, 2012 - Michael Doherty
//-----------------------------------------------------------------------------
// ExperimentalController.cpp
//    Motion controller with program controlled motion
//    Controller assumes a specific skeleton composed of 8 bones
//      as defined in ExperimentalController.h.
//-----------------------------------------------------------------------------

#include "Core/SystemConfiguration.h"
#include "ExperimentalController.h"
#include "Animation/AnimationException.h"

ExperimentalController::ExperimentalController() 
	: MotionController(), skeleton(NULL)
{ 
	for (int b=0; b<9; b++)
		for (int d=0; d<6; d++)
			signals[b][d] = NULL;
	signals[ROOT_BONE_ID][CT_TY] = new ConstantGenerator(17.8f);
	//signals[LFEMUR_BONE_ID][CT_RX] = new SineGenerator(1.0f, 0.1f, 0.0f);
	signals[LFEMUR_BONE_ID][CT_RX] = new SignalAdder(new SineGenerator(1.0f, 0.1f, 0.0f),
		new SineGenerator(0.1f, 0.5f, 00.2f));
}

ExperimentalController::~ExperimentalController() 
{ 
	for (int b=0; b<9; b++)
		for (int d=0; d<6; d++)
			if (signals[b][d] != NULL) delete signals[b][d];
}

bool ExperimentalController::isValidChannel(CHANNEL_ID _channel,float t)
{	
	switch (_channel.bone_id)
	{
	case ROOT_BONE_ID: 
		return true; // all 6 DOFS are valid
	case LFEMUR_BONE_ID:
	case RFEMUR_BONE_ID:
		if ((_channel.channel_type == CT_RX) || 
			(_channel.channel_type == CT_RY) || 
			(_channel.channel_type == CT_RZ)) return true;
	case LTIBIA_BONE_ID: 
	case RTIBIA_BONE_ID: 
		if (_channel.channel_type == CT_RX) return true;
	case LFOOT_BONE_ID: 
	case RFOOT_BONE_ID: 
		if ((_channel.channel_type == CT_RX) || 
			(_channel.channel_type == CT_RZ)) return true;
	}
	return false;
}

float ExperimentalController::getValue(CHANNEL_ID _channel, float _time)
{
	if (!isValidChannel(_channel)) return 0.0f;

	if (skeleton != NULL)
	{
		if ((_channel.bone_id == LTIBIA_BONE_ID) && (_channel.channel_type == CT_RX))
		{
			CHANNEL_ID cid(LFEMUR_BONE_ID, CT_RX);
			float angle = 0.5f*skeleton->getChannelValue(cid);
			if (angle < 0.0f) angle = 0.0f;
			return angle;
		}
	}
	
	if (signals[_channel.bone_id][_channel.channel_type] == NULL) return 0.0f;

	return signals[_channel.bone_id][_channel.channel_type ]->signal(_time);
}
