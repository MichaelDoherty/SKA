//-----------------------------------------------------------------------------
// app0005 - Builds with SKA Version 3.1 - Sept 01, 2012 - Michael Doherty
//-----------------------------------------------------------------------------
// AnimationControl.h
//     Animation control for a three bone arm 
//     with inverse kinematics tracking a moving target.
//-----------------------------------------------------------------------------
#ifndef ANIMATIONCONTROL_DOT_H
#define ANIMATIONCONTROL_DOT_H
#include "Core/SystemConfiguration.h"
#include <string>
#include <list>
using namespace std;
#include "Animation/Skeleton.h"
#include "Animation/MotionSequence.h"
#include "Animation/Skeleton.h"
#include "Objects/Object.h"

struct AnimationControl
{
	bool ready;

	vector<Skeleton*> characters;

	bool single_step;
	bool freeze;
	float mocap_frame_duration; 
	float time_since_last_frame;
	unsigned int master_frame;
	float time_warp;

	AnimationControl();

	virtual ~AnimationControl();

	void loadCharacters(list<Object*>& render_list);

	bool updateAnimation(float _elapsed_time);

	void togglePause() { freeze = !freeze; }

	void singleStep() { single_step = true; }

	void slowDown() { time_warp /= 2.0f; }

	void fullSpeed() { time_warp = 1.0f; }

	float getTimeWarp() { return time_warp; }

	bool isReady() { return ready; }
};

extern AnimationControl anim_ctrl;

#endif // ANIMATIONCONTROL_DOT_H
