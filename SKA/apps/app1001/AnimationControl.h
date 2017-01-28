//-----------------------------------------------------------------------------
// app1001 - Builds with SKA Version 3.1 - Sept 01, 2012 - Michael Doherty
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
#include <vector>
using namespace std;
// SKA modules
#include <Objects/Object.h>

class Skeleton;

class MotionDataSpecification
{
private:
	struct MotionDataSpec 
	{
		string seqID;
		string BVH_file;
		string quat_file;
		MotionDataSpec(string _seqID, string _BVH_file, string _quat_file)
		{
			seqID = _seqID; BVH_file = _BVH_file; quat_file = _quat_file;
		}
	};
	vector<MotionDataSpec> specs;

public:
	short size() { return (short)specs.size(); }
	void addSpec(string _seqID, string _BVH_file, string _quat_file)
	{
		specs.push_back(MotionDataSpec(_seqID, _BVH_file, _quat_file));
	}
	string getSeqID(short i) { return specs[i].seqID; }
	string getBvhFilename(short i) { return specs[i].BVH_file; }
	string getQuatFilename(short i) { return specs[i].quat_file; }
};

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

	// in each pair: first name is the regular BVH for playback, second name is the quaterion file.
	MotionDataSpecification motion_data_specs;
	void initializeMotionFileList();

	// keep a pointer to the motion graph controller, so that state reports can be easily extracted
	class MotionGraphController* motion_graph_controller;
	class MotionGraph* motion_graph;

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

	// format a report on the state of the motion graph controller
	vector<string> statusReport();
};

// global single instance of the animation controller
extern AnimationControl anim_ctrl;

#endif // ANIMATIONCONTROL_DOT_H
