//-----------------------------------------------------------------------------
// app0004 - Builds with SKA Version 3.1 - Sept 01, 2012 - Michael Doherty
//-----------------------------------------------------------------------------
// ExperimentalController.h
//    Motion controller with program controlled motion
//    Controller assumes a specific skeleton composed of 8 bones as defined below.
//-----------------------------------------------------------------------------

#ifndef EXPERIMENTALCONTROLLER_DOT_H
#define EXPERIMENTALCONTROLLER_DOT_H
#include "Core/SystemConfiguration.h"
#include "Animation/MotionController.h"
#include "Animation/Skeleton.h"
#include "Signals/Signals.h"

#define ROOT_BONE_ID 0
#define LHIPJOINT_BONE_ID 1
#define LFEMUR_BONE_ID 2
#define LTIBIA_BONE_ID 3
#define LFOOT_BONE_ID 4
#define RHIPJOINT_BONE_ID 5
#define RFEMUR_BONE_ID 6
#define RTIBIA_BONE_ID 7
#define RFOOT_BONE_ID 8

class ExperimentalController : public MotionController
{
public:
	ExperimentalController();
	
	virtual ~ExperimentalController();
	
	virtual float getValue(CHANNEL_ID _channel, float _time);

	virtual bool isValidChannel(CHANNEL_ID _channel, float t=0.0f);

	void setSkelton(Skeleton* _skel) { skeleton = _skel; }

private:
	SignalGenerator* signals[9][6]; // first index is bone_id, second index = dof_id
	Skeleton* skeleton;         // pointer back to the skeleton this controller is controlling
};

#endif

