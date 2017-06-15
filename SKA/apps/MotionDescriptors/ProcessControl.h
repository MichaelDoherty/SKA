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
#include "AnimationControl.h"

class ProcessControl {
public:
	enum SHOULDERMODE { NONE, FLEXION, EXTENSION, ABDUCTION };

	static string toString(SHOULDERMODE dm) {
		switch (dm) {
		case NONE: return string("NONE");
		case FLEXION: return string("FLEXION");
		case EXTENSION: return string("EXTENSION");
		case ABDUCTION: return string("ABDUCTION");
		default: return string("UNKNOWN");
		}
	}

	enum MOCAPTYPE { UNK, BVH, AMC };
	static string toString(MOCAPTYPE mct) {
		switch (mct) {
		case BVH: return string("BVH");
		case AMC: return string("ASF/AMC");
		default: return string("UNKNOWN");
		}
	}

	static string toString(bool x) {
		if (x) return string("TRUE");
		else return string("FALSE");
	}

	struct ProcessingRequest {
		string take_label;
		MOCAPTYPE mocap_file_type;
		string input_folder;
		string output_folder;
		string skeleton_file;	// ASF or unused for BVH
		string motion_file; // AMC or BVH file
		string result_file; // name based on motion_file
		SHOULDERMODE shoulder_mode;
		bool loop;
		bool run_motion_analysis;
		float init_skip_time;
		ProcessingRequest(const string& _take_label, 
			MOCAPTYPE _mctype, 
			const string& _input_folder, const string& _output_folder,
			const string& _motion_file, const string& _skel_file,
			const string& _result_file,
			bool _motion_analysis = false, SHOULDERMODE _data_mode = NONE,
			bool _loop = false, float _init_skip_time = 5.0f)
			: take_label(_take_label), 
			  mocap_file_type(_mctype), 
			  input_folder(_input_folder), output_folder(_output_folder),
			  skeleton_file(_skel_file), motion_file(_motion_file), 
			  result_file(_result_file), 
			  shoulder_mode(_data_mode), loop(_loop), 
			  run_motion_analysis(_motion_analysis), init_skip_time(_init_skip_time) {}

		friend ostream& operator<<(ostream& os, const ProcessingRequest& p) {
			os << "mocap type:      " << toString(p.mocap_file_type) << endl;
			os << "input folder:    " << p.input_folder << endl;
			os << "output folder:   " << p.output_folder << endl;
			os << "motion file:     " << p.motion_file << endl;
			os << "skeleton file:   " << p.skeleton_file << endl;
			os << "result file:     " << p.result_file << endl;
			os << "motion analysis: " << p.run_motion_analysis << endl;
			os << "mode:            " << p.shoulder_mode << endl;
			os << "loop:            " << p.loop << endl;
			os << "skip time:       " << p.init_skip_time << " seconds" << endl;
			return os;
		}
	};

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

	void addRequest(ProcessingRequest& r) {
		requests.push_back(r);
		if (curr_request < 0) { curr_request = 0; }
	}

	ProcessingRequest& currentRequest() { return requests[curr_request]; }

private:
	bool animation_enabled;
	vector<ProcessingRequest> requests;
	short curr_request;

	bool real_time_mode;  //  true = step in clock time, false = step frame-by-frame
	bool animation_has_looped;
	bool goto_next_process;

public:
	friend ostream& operator<<(ostream& os, const ProcessControl& pc) {
		os << "Process Control: " << endl;
		if (pc.animation_enabled) os << "ANIMATION IS ENABLED" << endl;
		else os << "ANIMATION IS DISABLED" << endl;
		unsigned short p;
		for (p = 0; p < pc.requests.size(); p++)
		{
			os << pc.requests[p];
		}
		return os;
	}
};

extern ProcessControl process_control;

#endif // PROCESSCONTROL_DOT_H
