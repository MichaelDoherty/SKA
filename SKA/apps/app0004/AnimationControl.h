//-----------------------------------------------------------------------------
// app0004 - Builds with SKA Version 4.0
//-----------------------------------------------------------------------------
// AnimationControl.h
//    animation controller using a program controlled motion controller
//-----------------------------------------------------------------------------
#ifndef ANIMATIONCONTROL_DOT_H
#define ANIMATIONCONTROL_DOT_H
#include "Core/SystemConfiguration.h"
#include <string>
#include <list>
using namespace std;
#include "Animation/Skeleton.h"
#include "Animation/MotionSequence.h"
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
	float world_time;

	AnimationControl();

	virtual ~AnimationControl();

	void loadCharacters(list<Object*>& render_list);

	bool updateAnimation(float _elapsed_time);

	void togglePause() { freeze = !freeze; }

	void singleStep() { single_step = true; }

	bool isReady() { return ready; }
};

extern AnimationControl anim_ctrl;

#endif // ANIMATIONCONTROL_DOT_H
