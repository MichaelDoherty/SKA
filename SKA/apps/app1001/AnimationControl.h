//-----------------------------------------------------------------------------
// app0002 - Builds with SKA Version 3.1 - Sept 01, 2012 - Michael Doherty
//-----------------------------------------------------------------------------
// AnimationControl.h
//    Animation controller for a single character defined by a BVH file.
//-----------------------------------------------------------------------------
#ifndef ANIMATIONCONTROL_DOT_H
#define ANIMATIONCONTROL_DOT_H
// SKA configuration
#include <Core/SystemConfiguration.h>
// C/C++ libraries
#include <list>
using namespace std;
// SKA modules
#include <Objects/Object.h>

class Skeleton;

struct AnimationControl
{
private:
	bool ready;
	float run_time;
	Skeleton* character;
	// flags to control animation
	bool single_step;
	bool freeze;
	float time_warp;
public:
	AnimationControl();
	virtual ~AnimationControl();

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
