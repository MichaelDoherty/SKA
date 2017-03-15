//-----------------------------------------------------------------------------
// SystemTimer.h
//	 Class providing timing functions.
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

#ifndef SYSTEMTIMER_DOT_H
#define SYSTEMTIMER_DOT_H
#include <Core/SystemConfiguration.h>


//====================================================================
// SystemTimer
//====================================================================
//    reset()        - reset the timer
//    start()        - start the timer
//    stop           - pause the timer
//    systemTime()   - get the absolute system time
//    appTime()      - get the time since last reset
//    elapsedTime()  - get the time since last elapsedTime() call
//
// All times are in seconds.

class SKA_LIB_DECLSPEC SystemTimer
{
public:
	SystemTimer();
	~SystemTimer();
	void reset();
	void start();
	void stop();
	double systemTime();
	double appTime();
	double elapsedTime();
private:	
	bool stopped;
	double base_time;
	double stop_time;
	double checkpoint_time;
};

SKA_LIB_DECLSPEC extern SystemTimer system_timer;

#endif
