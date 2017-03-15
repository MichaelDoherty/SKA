//-----------------------------------------------------------------------------
// SystemTimer.cpp
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

#include <Core/SystemConfiguration.h>
#include <Core/SystemTimer.h>
#include <ctime>

SystemTimer::SystemTimer()
: stopped(false), base_time(0.0), stop_time(0.0), checkpoint_time(0.0)
{
	base_time = checkpoint_time = systemTime();
}

double SystemTimer::systemTime()
{
	if (stopped) return stop_time;
	return double(clock())/double(CLOCKS_PER_SEC);
}

SystemTimer::~SystemTimer()
{
}

void SystemTimer::reset()
{
	double now = systemTime();
	base_time = now;
	checkpoint_time = now;
	stop_time = 0.0;
}

void SystemTimer::start()
{
	double now = systemTime();
	base_time = now - stop_time;
	stop_time = 0.0;
	checkpoint_time = now;
}

void SystemTimer::stop()
{
	double now = systemTime();
	stop_time = now;
	checkpoint_time = now;
}

double SystemTimer::appTime()
{
	double now = systemTime();
	return now - base_time;
}

double SystemTimer::elapsedTime()
{
	double now = systemTime();
	double e_time = now - checkpoint_time;
	checkpoint_time = now;
	return e_time;
}

SystemTimer system_timer;
