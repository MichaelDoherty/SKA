//-----------------------------------------------------------------------------
// MotionDescriptors project - Builds with SKA Version 4.0
//-----------------------------------------------------------------------------
// ProcessControl.h
// Authors:    Michael Doherty and Trevor Martin - April 2017
//-----------------------------------------------------------------------------

#ifndef PROCESSCONTROL_DOT_H
#define PROCESSCONTROL_DOT_H
// SKA modules
#include <Core/SystemConfiguration.h>
#include <Math/Plane.h>
#include <Animation/Skeleton.h>
#include <Objects/Object.h>
#include <Objects/BoneObject.h>
// local application
#include "BoneData.h"
#include "FullBodyData.h"
#include "AnimationControl.h"

static bool DISPLAY_ALL_RESULTS = false;

enum DATAMODE { FLEXION, EXTENSION, ABDUCTION };
inline string toString(DATAMODE dm) {
	switch (dm) {
	case FLEXION: return string("FLEXION");
	case EXTENSION: return string("EXTENSION");
	case ABDUCTION: return string("ABDUCTION");
	default: return string("UNKNOWN");
	}
}

inline string toString(bool x) {
	if (x) return string("TRUE");
	else return string("FALSE");
}

struct ProcessingRequest {
	string take_label;
	string motion_file;
	string result_file;
	DATAMODE data_mode;
	bool loop;
	float init_skip_time;
	ProcessingRequest(string& _take_label, string& _motion_file, string& _result_file,
		DATAMODE _data_mode = FLEXION, bool _loop = false, float _init_skip_time = 5.0f)
		: take_label(_take_label), motion_file(_motion_file), result_file(_result_file), data_mode(_data_mode),
		loop(_loop), init_skip_time(_init_skip_time) {}
};

class ProcessControl {
public:
	ProcessControl() { reset(); }
	void reset() {
		requests.clear();
		animation_enabled = false;
		curr_request = -1;
		real_time_mode = false; 
		animation_has_looped = false;
		goto_next_process = false;
	}
	bool readCommandFile();
	bool active() { return curr_request < (short)requests.size(); }
	bool advance() { curr_request++; return active(); }
	bool animationIsEnabled() { return animation_enabled; }
	void enableAnimation() { animation_enabled = true; }
	void disableAnimation() { animation_enabled = false; }

	bool realTimeMode() { return real_time_mode; }
	void clearRealTimeMode() { real_time_mode = false; }
	void setRealTimeMode() { real_time_mode = true; }

	bool animationHasLooped() { return animation_has_looped; }
	void clearAnimationHasLooped() { animation_has_looped = false; }
	void setAnimationHasLooped() { animation_has_looped = true; }

	bool gotoNextProcess() { return goto_next_process; }
	void clearGotoNextProcess() { goto_next_process = false; }
	void setGotoNextProcess() { goto_next_process = true; }

	/*Trevor's code for input file enabling bone tracking*/
	void enableBoneTracking() { bone_tracking = true; }
	void disableBoneTracking() { bone_tracking = false; }
	bool trackingIsEnabled() { return bone_tracking; }
	/*End Trevor's code*/

	void addRequest(ProcessingRequest& r) {
		requests.push_back(r);
		if (curr_request < 0) { curr_request = 0; }
	}

	DATAMODE dataMode() { return requests[curr_request].data_mode; }
	bool loop() { return requests[curr_request].loop; }
	float skip() { return requests[curr_request].init_skip_time; }
	string takeLabel() { return requests[curr_request].take_label; }
	string motionFile() { return requests[curr_request].motion_file; }
	string resultFile() { return requests[curr_request].result_file; }
	/*Set and Gets for bone tracking*/
	void setBoneName(string name) { bone_tracking_name = name; }
	string getBoneName() { return bone_tracking_name; }
	void setPlaneName(string plane) { plane_name = plane; }
	string getPlaneName() { return plane_name; }
	void setFrameStep(int frames) { frame_step = frames; }
	int getFrameStep() { return frame_step; }

private:
	int frame_step = 1;
	bool animation_enabled;
	bool bone_tracking;
	string bone_tracking_name;
	string plane_name;
	vector<ProcessingRequest> requests;
	short curr_request = -1;

	// FIXIT!! move these global flags into the class
	bool real_time_mode;  //  true = step in clock time, false = step frame-by-frame
	bool animation_has_looped;
	bool goto_next_process;

public:
	void printProcessControl()
	{
		cout << "Process Control: " << endl;
		if (animation_enabled) cout << "ANIMATION IS ENABLED" << endl;
		else cout << "ANIMATION IS DISABLED" << endl;
		if (bone_tracking) cout << "BONE TRACKING ENABLED\nTRACKING: " << bone_tracking_name << "AND" << plane_name << endl;
		else cout << "BONE TRACKING DISABLED" << endl;
		unsigned short p;
		for (p = 0; p < requests.size(); p++)
		{
			ProcessingRequest preq = requests[p];
			cout << "Request " << p << " ... " << endl;
			cout << "motion file: " << preq.motion_file << endl;
			cout << "result file: " << preq.result_file << endl;
			cout << "mode: " << preq.data_mode << endl;
			if (preq.loop) cout << "loop: on" << endl;
			else cout << "loop: off" << endl;
			cout << "skip time: " << preq.init_skip_time << " seconds" << endl;
		}
	}
};

extern ProcessControl process_control;

#endif // PROCESSCONTROL_DOT_H