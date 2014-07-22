//-----------------------------------------------------------------------------
// app0002: Demo program illustrating various useful things that aren't
//          directly related to the character animation.
//          (1) additional objects, such as ground, sky and coordinate axes
//          (2) moveable camera, controlled by the camera and mouse.
//          (3) keyboard filtering, to avoid multiple responses when
//              a single keystroke is expected.
//          (4) animation speed control (freeze, single step, time warp)
//          (5) heads-up display (2D text on screen)
//-----------------------------------------------------------------------------
// AnimationControl.h
//    Object that is the interface to the animation subsystem.
//-----------------------------------------------------------------------------
// Builds with SKA Version 3.0 - July 22, 2012 - Michael Doherty
//-----------------------------------------------------------------------------
#ifndef ANIMATIONCONTROL_DOT_H
#define ANIMATIONCONTROL_DOT_H
// SKA configuration
#include <Core/SystemConfiguration.h>
// C/C++ libraries
#include <list>
using namespace std;
// SKA modules
#include <Animation/Character.h>
#include <Objects/Object.h>

struct AnimationControl
{
private:
	bool ready;
	float run_time;
	Character* character;
	// flags to control animation
	bool single_step;
	bool freeze;
	float time_warp;
public:
	AnimationControl() : ready(false), run_time(0.0f), character(NULL), 
		single_step(false), freeze(false), time_warp(1.0f)
	{ } 
	virtual ~AnimationControl()	{ if (character != NULL) delete character; }

	// loadCharacters() sets up the characters and their motion control.
	// It places all the bone objects for each character into the render list,
	// so that they can be drawn by the graphics subsystem.
	void loadCharacters(list<Object*>& render_list);

	// updateAnimation() should be called every frame to update all characters.
	// _elapsed_time should be the time (in seconds) since the last frame/update.
	bool updateAnimation(float _elapsed_time);

	bool isReady()      { return ready; }
	void togglePause()  { freeze = !freeze; }
	void singleStep()   { single_step = true; }
	void slowDown()     { time_warp /= 2.0f; }
	void speedUp()      { time_warp *= 2.0f; }
	void normalSpeed()  { time_warp = 1.0f; }
	bool isFrozen()     { return freeze; }
	float getTimeWarp() { return time_warp; }
};

// global single instance of the animation controller
extern AnimationControl anim_ctrl;

#endif // ANIMATIONCONTROL_DOT_H
