//-----------------------------------------------------------------------------
// MultiSequenceController.h
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

#ifndef MULTISEQUENCECONTROLLER_DOT_H
#define MULTISEQUENCECONTROLLER_DOT_H
#include <Core/SystemConfiguration.h>
#include <Animation/MotionSequenceController.h>

class SKA_LIB_DECLSPEC MultiSequenceController : public MotionController
{
public:
	MultiSequenceController() 
		: MotionController(), num_controllers(0), active_controller(0),
		  cycle_time(1.0f), prev_time(-1.0f), sequence_time(0.0f)
	{ }

	MultiSequenceController(MotionSequenceController* ctrls[], 
		                    short num_ctrls, float _cycle_time=1.0f) 
		: MotionController(), num_controllers(num_ctrls), active_controller(0),
		  cycle_time(_cycle_time), prev_time(-1.0f), sequence_time(0.0f)
	{ 
		if (num_controllers > 100) num_controllers = 100;
		for (short i=0; i<num_controllers; i++) 
			seq_controllers[i] = ctrls[i];
	}
	
	virtual ~MultiSequenceController() 
	{
		for (short i=0; i<num_controllers; i++) 
			if (seq_controllers[i] != NULL) delete seq_controllers[i];
	}

	virtual bool isValidChannel(CHANNEL_ID _channel, float _time);

	virtual float getValue(CHANNEL_ID _channel, float _time);

	void setCycleTime(float new_cycle_time) { cycle_time = new_cycle_time; }

private:
	MotionSequenceController* seq_controllers[100];
	short num_controllers;
	short active_controller;
	float cycle_time;			// seconds to play each sequence
	float prev_time;			// world time at last call to getValue()
	float sequence_time;		// current time in the MotionSequence cycle
	float next_switch_time;		// world time for next sequence switch

	short selectController(float _time);
	void switchController(float _time);
};

#endif

