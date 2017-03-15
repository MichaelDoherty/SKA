//-----------------------------------------------------------------------------
// MotionDescriptors project - Builds with SKA Version 4.0
// THIS FILES IN THIS PROJECT ARE CURRENTLY JUST PLACE HOLDERS FOR 
// THE REAL CODE WHICH IS IN DEVELOPMENT (March 14 2017)
//-----------------------------------------------------------------------------
// MotionDescriptors: This project is ongoing research to extract 
//    motion descriptors from a motion.
//-----------------------------------------------------------------------------
// AppMain.cpp
//    The main program is mostly the connection between openGL, 
//    SKA and application specific code. It also controls the order of 
//    initialization, before control is turned over to openGL.
//-----------------------------------------------------------------------------
// SKA configuration.
#include <Core/SystemConfiguration.h>
// C/C++ libraries
#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;
// openGL library
//#include <GL/glut.h>
// SKA modules
#include <Core/BasicException.h>
#include <Core/SystemTimer.h>

// local application
#include "AppConfig.h"
#include "AppGraphics.h"
#include "AnimationControl.h"
#include "CameraControl.h"
#include "InputProcessing.h"
#include "Plane.h"
#include "DataRecorder.h"

static bool DISPLAY_ALL_RESULTS = false;
static bool SHOW_DIR_VECTORS = false;

static ofstream results_file;

enum DATAMODE {	FLEXION, EXTENSION, ABDUCTION };
string toString(DATAMODE dm) {
	switch (dm) {
	case FLEXION: return string("FLEXION");
	case EXTENSION: return string("EXTENSION");
	case ABDUCTION: return string("ABDUCTION");
	default: return string("UNKNOWN");
	}
}

enum BODYSIDE { LEFT, RIGHT };
string toString(BODYSIDE as) {
	switch (as) {
	case LEFT: return string("LEFT");
	case RIGHT: return string("RIGHT");
	default: return string("UNKNOWN");
	}
}

string toString(bool x) {
	if (x) return string("TRUE"); 
	else return string("FALSE");
}

struct ProcessingRequest {
	string take_label;
	string motion_file;
	string result_file;
	DATAMODE data_mode;
	BODYSIDE which_arm;
	bool loop;
	float init_skip_time;
	ProcessingRequest(string& _take_label, string& _motion_file, string& _result_file,
		DATAMODE _data_mode=FLEXION, BODYSIDE _which_arm=RIGHT, bool _loop = false, float _init_skip_time = 5.0f)
		: take_label(_take_label), motion_file(_motion_file), result_file(_result_file), 
		  data_mode(_data_mode), which_arm(_which_arm), loop(_loop), init_skip_time(_init_skip_time) {}
};

class ProcessControl {
public:
	ProcessControl() { reset(); }
	void reset() {
		requests.clear();
		animation_enabled = false;
		curr_request = -1;
	}
	bool active() { return curr_request < (short)requests.size();  }
	bool advance() { curr_request++; return active(); }
	bool animationIsEnabled() { return animation_enabled;  }
	void enableAnimation() { animation_enabled = true; }
	void disableAnimation() { animation_enabled = false; }

	void addRequest(ProcessingRequest& r) { 
		requests.push_back(r); 
		if (curr_request < 0) { curr_request = 0; }
	}

	DATAMODE dataMode() { return requests[curr_request].data_mode; }
	BODYSIDE whichArm() { return requests[curr_request].which_arm; }
	bool loop() { return requests[curr_request].loop; }
	float skip() { return requests[curr_request].init_skip_time; }
	string takeLabel() { return requests[curr_request].take_label; }
	string motionFile() { return requests[curr_request].motion_file; }
	string resultFile() { return requests[curr_request].result_file; }
private:
	bool animation_enabled;
	vector<ProcessingRequest> requests;
	short curr_request = -1;
public:
	void printProcessControl()
	{
		cout << "Process Control: " << endl;
		if (animation_enabled) cout << "ANIMATION IS ENABLED" << endl;
		else cout << "ANIMATION IS DISABLED" << endl;
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
static ProcessControl process_control;

/*
command file format:
INPUT_FOLDER=<path>
OUTPUT_FOLDER=<path>
ANIMATION=<Y|N>
PROCESS <file> MODE=<F|E|A> LOOP=<Y|N> SKIP=<number>
*/

bool isPrefix(string& s, string& prefix) {
	return s.find(prefix) == 0;
}

bool readCommandFile()
{
	string input_folder("");
	string output_folder("");
	string line;
	ifstream cf("skacommands.txt");
	if (!cf) return false;

	while (getline(cf, line))
	{
		if (line.length() < 1) continue;

		if (isPrefix(line, string("INPUT_FOLDER=")))
		{
			input_folder = line.substr(14,line.length()-15);
			if (input_folder[input_folder.length() - 1] != '\\') input_folder.append("\\");
		}
		else if (isPrefix(line, string("OUTPUT_FOLDER=")))
		{
			output_folder = line.substr(15, line.length()-16);
			if (output_folder[output_folder.length() - 1] != '\\') output_folder.append("\\");
		}
		else if (isPrefix(line, string("ANIMATION=")))
		{
			if (line[10] == 'Y') process_control.enableAnimation();
			else if (line[10] == 'N') process_control.disableAnimation();
		}
		else if (isPrefix(line, string("PROCESS")))
		{
			line = line.substr(7);
			int quote1 = line.find_first_of('\"');
			int quote2 = line.find_first_of('\"', quote1 + 1);
			string filename = line.substr(quote1+1, (quote2-quote1)-1);
			line = line.substr(quote2 + 1);
			
			char mode = 'E';
			char loop = 'N';
			float skip = 5.0f;
			char arm = 'R';
			
			int i = line.find("MODE=");
			if (i != string::npos) mode = line[i + 5];
			i = line.find("ARM=");
			if (i != string::npos) arm = line[i + 4];
			i = line.find("LOOP=");
			if (i != string::npos) loop = line[i + 5];
			i = line.find("SKIP=");
			if (i != string::npos) {
				string stmp = line.substr(i + 5);
				skip = float(atof(stmp.c_str()));
			}

			DATAMODE dm = EXTENSION;
			switch (mode) {
			case 'F': dm = FLEXION; break; 
			case 'E': dm = EXTENSION; break;
			case 'A': dm = ABDUCTION; break;
			};
			bool loop_on = false;
			if (loop == 'Y') loop_on = true;
			BODYSIDE which_arm = RIGHT;
			if (arm == 'L') which_arm = LEFT;

			ProcessingRequest preq(filename, 
				input_folder + filename + ".bvh", output_folder + filename + ".csv",
				dm, which_arm, loop_on, skip);
			process_control.addRequest(preq);
		}
		else
		{
			cerr << "INVALID COMMAND LINE: " << line << endl;
		}
	}
	cf.close();
	return true;
}

static DataRecorder data_recorder;

// abduction = lift of arm in coronal plane
// flexion = arm forward in sagittal plane
// extension = arm backward in sagittal plane
static float flexion=0.0f, extension=0.0f, abduction=0.0f;
static float dir_test = 0.0f;
static float max_flexion = 0.0f, max_extension = 0.0f, max_abduction = 0.0f;
static long animation_frame = 0;
static float animation_time = 0.0f;

static void resetMaxValues() {
	max_flexion = 0.0f; max_extension = 0.0f; max_abduction = 0.0f;
}

static bool real_time_mode = false;  //  true = step in clock time, false = step frame-by-frame
static bool animation_has_finished_one_pass = false;
static bool goto_next_process = false;

// probably unnecessary - used in case any cleanup is needed on exit.
void shutDown(int _exit_code)
{
	results_file.close();
	exit(_exit_code);
}

void processPTData()
{
	// abduction = angle between spine and arm when lifted in the coronal plane
	// flexion   = angle between spine and arm when lifted forward in the sagittal plane
	// extension = angle between spine and arm when lifted backward in the sagittal plane

	// extract necessary data from the skeleton in AnimationController

	Vector3D junk; // throw away data
	Vector3D root_position, root_orientation;
	Vector3D rightupperarm_pos, rightelbow_pos;
	Vector3D leftupperarm_pos, leftelbow_pos;
	Vector3D tailbone_pos, neck_pos;
	Vector3D rightshoulder_pos, leftshoulder_pos;

	if (!anim_ctrl.getBonePosition("root", junk, root_position))
		cerr << "Failed to find bone \"root\"" << endl;
	if (!anim_ctrl.getBoneOrientation("root", root_orientation))
		cerr << "Failed to find bone \"root\"" << endl;

	if (!anim_ctrl.getBonePosition("RightArm", rightupperarm_pos, rightelbow_pos))
		cerr << "Failed to find bone \"RightArm\"" << endl;

	if (!anim_ctrl.getBonePosition("LeftArm", leftupperarm_pos, leftelbow_pos))
		cerr << "Failed to find bone \"LeftArm\"" << endl;

	if (!anim_ctrl.getBonePosition("Spine", tailbone_pos, junk))
		cerr << "Failed to find bone \"Spine\"" << endl;
	if (!anim_ctrl.getBonePosition("Neck", neck_pos, junk))
		cerr << "Failed to find bone \"Neck\"" << endl;
	if (!anim_ctrl.getBonePosition("LeftShoulder", leftshoulder_pos, junk))
		cerr << "Failed to find bone \"LeftShoulder\"" << endl;
	if (!anim_ctrl.getBonePosition("RightShoulder", rightshoulder_pos, junk))
		cerr << "Failed to find bone \"RightShoulder\"" << endl;

	//---- analytical computations

	// Use vectors from tailbone to the shoulders to define the coronal plane.
	Vector3D cplane1 = leftshoulder_pos - tailbone_pos;	 cplane1.normalize();
	Vector3D cplane2 = rightshoulder_pos - tailbone_pos; cplane2.normalize();
	Plane coronal_plane(cplane1, cplane2);
	coronal_plane.moveTo(root_position);
	Vector3D forward_dir = coronal_plane.getNormal();
	Vector3D backward_dir = forward_dir*-1;

	// Define spine vector from neck down to tailbone and the project 
	// that vector onto the coronal plane.
	// For a symetric collar area, this should be equivalant to finding a vector
	// midway between the two tailbone/shoulder vectors (cplane1 and cplane2).
	Vector3D spine_dir = tailbone_pos - neck_pos; spine_dir = spine_dir.normalize();
	spine_dir = coronal_plane.projectVectorOntoPlane(spine_dir); spine_dir = spine_dir.normalize();

	// Use the spine and the coronal plane normal to define the sagittal plane.
	Plane sagittal_plane(spine_dir*-1, forward_dir);
	sagittal_plane.moveTo(root_position);
	Vector3D left_dir = sagittal_plane.getNormal();
	Vector3D right_dir = left_dir*-1;

	// Define the transverse plane to be perpendicular to both the coronal and sagittal planes.
	Plane transverse_plane(forward_dir, left_dir);
	transverse_plane.moveTo(root_position);
	Vector3D up_dir = transverse_plane.getNormal();
	Vector3D down_dir = up_dir*-1;
	
	// endpoints of the measurment line segments, projected onto the relative planes
	Vector3D upperarm_pos_cp, elbow_pos_cp, upperarm_pos_sp, elbow_pos_sp;
	Vector3D spine_top_cp, spine_top_sp, spine_bottom_cp, spine_bottom_sp;

	// project arm endpoints onto coronal plane and sagittal plane
	if (process_control.whichArm() == RIGHT)
	{
		upperarm_pos_cp = coronal_plane.projectPointOntoPlane(rightupperarm_pos);
		elbow_pos_cp = coronal_plane.projectPointOntoPlane(rightelbow_pos);
		upperarm_pos_sp = sagittal_plane.projectPointOntoPlane(rightupperarm_pos);
		elbow_pos_sp = sagittal_plane.projectPointOntoPlane(rightelbow_pos);
	}
	else
	{
		upperarm_pos_cp = coronal_plane.projectPointOntoPlane(leftupperarm_pos);
		elbow_pos_cp = coronal_plane.projectPointOntoPlane(leftelbow_pos);
		upperarm_pos_sp = sagittal_plane.projectPointOntoPlane(leftupperarm_pos);
		elbow_pos_sp = sagittal_plane.projectPointOntoPlane(leftelbow_pos);
	}

	// Move spine vector to have same start position as arm.
	// (This is just to make the visualization make sense.)
	spine_top_cp = upperarm_pos_cp;
	Vector3D spine_bottom = spine_top_cp + spine_dir*5.0f;
	spine_bottom_cp = coronal_plane.projectPointOntoPlane(spine_bottom);
	spine_top_sp = upperarm_pos_sp;
	spine_bottom = spine_top_sp + spine_dir*5.0f;
	spine_bottom_sp = sagittal_plane.projectPointOntoPlane(spine_bottom);
	
	// Convert measurement line segments into vectors.
	Vector3D arm_cp = elbow_pos_cp - upperarm_pos_cp;	arm_cp = arm_cp.normalize();
	Vector3D spine_cp = spine_bottom_cp - spine_top_cp;	spine_cp = spine_cp.normalize();
	Vector3D arm_sp = elbow_pos_sp - upperarm_pos_sp;	arm_sp = arm_sp.normalize();
	Vector3D spine_sp = spine_bottom_sp - spine_top_sp;	spine_sp = spine_sp.normalize();
	
	// Calculate critical angles
	float cp_out;
	if (process_control.whichArm() == RIGHT)
		cp_out = rad2deg(acos(arm_cp.ndot(right_dir)));
	else
		cp_out = rad2deg(acos(arm_cp.ndot(left_dir)));
	float cp_down = rad2deg(acos(arm_cp.ndot(down_dir)));
	float sp_down = rad2deg(acos(arm_sp.ndot(down_dir)));
	float sp_fwd = rad2deg(acos(arm_sp.ndot(forward_dir)));
	float sp_bwd = rad2deg(acos(arm_sp.ndot(backward_dir)));

	/*
	bool atest, etest, ftest;
	// check for abduction
	atest = cp_out < 50.0f;
	// check for extension
	etest = !atest && ((sp_bwd < 50.0f) || (sp_down > 130.0f));
	// check for flexion
	ftest = !atest && ((sp_fwd < 50.0f) || (sp_down > 130.0f));

	abduction = extension = flexion = 0.0f;
	if (atest && (process_control.dataMode() == ABDUCTION))
		abduction = cp_down;
	if (ftest && (process_control.dataMode() == FLEXION))
		if (sp_down < 90.0f) flexion = sp_down;
		else flexion = 90.0f + sp_fwd;
	if (etest && (process_control.dataMode() == EXTENSION))
		if (sp_down < 90.0f) extension = sp_down;
		else extension = 90.0f + sp_fwd;

	if (animation_time >= process_control.skip())
	{
		if (atest && (process_control.dataMode() == ABDUCTION))
			if (abduction > max_abduction) max_abduction = abduction;
		if (ftest && (process_control.dataMode() == FLEXION))
			if (flexion > max_flexion) max_flexion = flexion;
		if (etest && (process_control.dataMode() == EXTENSION))
			if (extension > max_extension) max_extension = extension;
	}
	*/
	abduction = extension = flexion = 0.0f;
	if (process_control.dataMode() == ABDUCTION)
		abduction = cp_down;
	if (process_control.dataMode() == FLEXION)
		if (sp_down < 90.0f) flexion = sp_down;
		else flexion = 90.0f + sp_fwd;
	if (process_control.dataMode() == EXTENSION)
		if (sp_down < 90.0f) extension = sp_down;
		else extension = 90.0f + sp_fwd;
	
	if (animation_time >= process_control.skip())
	{
		if (process_control.dataMode() == ABDUCTION)
			if (abduction > max_abduction) max_abduction = abduction;
		if (process_control.dataMode() == FLEXION)
			if (flexion > max_flexion) max_flexion = flexion;
		if (process_control.dataMode() == EXTENSION)
			if (extension > max_extension) max_extension = extension;
	}

	// test/debug information display
	if (process_control.dataMode() == ABDUCTION)
	{
		hud_data.misc[0] = abduction;
		hud_data.misc[1] = cp_down;
		hud_data.misc[2] = cp_out;
	}
	if (process_control.dataMode() == FLEXION)
	{
		hud_data.misc[0] = flexion;
		hud_data.misc[1] = sp_down;
		hud_data.misc[2] = sp_fwd;
	}
	if (process_control.dataMode() == EXTENSION)
	{
		hud_data.misc[0] = extension;
		hud_data.misc[1] = sp_down;
		hud_data.misc[2] = sp_bwd;
	}

	//---- data recording

	if (!animation_has_finished_one_pass)
	{
		if (animation_time < process_control.skip())
			data_recorder.recordNoData(animation_time, animation_frame);
		else 
			switch (process_control.dataMode()) {
			case ABDUCTION: 
				data_recorder.recordAbduction(animation_time, animation_frame, abduction);
				break;
			case EXTENSION:
				data_recorder.recordExtension(animation_time, animation_frame, extension);
				break;
			case FLEXION:
				data_recorder.recordFlexion(animation_time, animation_frame, flexion);
				break;
			}	
	}

	//---- visualization

	if (process_control.animationIsEnabled())
	{

		analysis_objects.root_marker->moveTo(root_position);
		analysis_objects.root_marker->rotateTo(root_orientation);

		if (process_control.whichArm() == RIGHT)
		{
			analysis_objects.upperarm_marker->moveTo(rightupperarm_pos);
			analysis_objects.elbow_marker->moveTo(rightelbow_pos);
		}
		else
		{
			analysis_objects.upperarm_marker->moveTo(leftupperarm_pos);
			analysis_objects.elbow_marker->moveTo(leftelbow_pos);
		}

		analysis_objects.upperarm_cp_marker->moveTo(upperarm_pos_cp);
		analysis_objects.elbow_cp_marker->moveTo(elbow_pos_cp);
		analysis_objects.spine_cp_marker->moveTo(spine_bottom_cp);

		analysis_objects.upperarm_sp_marker->moveTo(upperarm_pos_sp);
		analysis_objects.elbow_sp_marker->moveTo(elbow_pos_sp);
		analysis_objects.spine_sp_marker->moveTo(spine_bottom_sp);

		analysis_objects.arm_cp_bone->setEndpoints(upperarm_pos_cp, elbow_pos_cp);
		analysis_objects.arm_sp_bone->setEndpoints(upperarm_pos_sp, elbow_pos_sp);
		analysis_objects.spine_cp_bone->setEndpoints(upperarm_pos_cp, spine_bottom_cp);
		analysis_objects.spine_sp_bone->setEndpoints(upperarm_pos_sp, spine_bottom_sp);

		Vector3D p = root_position;
		analysis_objects.right_dir_marker->setEndpoints(p, p+(right_dir*20.0f));
		analysis_objects.forward_dir_marker->setEndpoints(p, p + (forward_dir*20.0f));
		analysis_objects.up_dir_marker->setEndpoints(p, p + (up_dir*20.0f));

		// (these should be aligned with the three Plane objects)
		analysis_objects.coronal_plane_image->moveTo(coronal_plane.getOrigin());
		analysis_objects.coronal_plane_image->rotateTo(root_orientation);
		analysis_objects.sagittal_plane_image->moveTo(sagittal_plane.getOrigin());
		analysis_objects.sagittal_plane_image->rotateTo(root_orientation);
		analysis_objects.transverse_plane_image->moveTo(transverse_plane.getOrigin());
		analysis_objects.transverse_plane_image->rotateTo(root_orientation);

		// FIXIT:170119 visibility adjustment should be done in AppGraphics
		analysis_objects.coronal_plane_image->setVisibility(analysis_objects.show_coronal_plane);
		analysis_objects.upperarm_cp_marker->setVisibility(analysis_objects.show_coronal_plane);
		analysis_objects.elbow_cp_marker->setVisibility(analysis_objects.show_coronal_plane);
		analysis_objects.spine_cp_marker->setVisibility(analysis_objects.show_coronal_plane);
		analysis_objects.arm_cp_bone->setVisibility(analysis_objects.show_coronal_plane);
		analysis_objects.spine_cp_bone->setVisibility(analysis_objects.show_coronal_plane);

		analysis_objects.sagittal_plane_image->setVisibility(analysis_objects.show_sagittal_plane);
		analysis_objects.upperarm_sp_marker->setVisibility(analysis_objects.show_sagittal_plane);
		analysis_objects.elbow_sp_marker->setVisibility(analysis_objects.show_sagittal_plane);
		analysis_objects.spine_sp_marker->setVisibility(analysis_objects.show_sagittal_plane);
		analysis_objects.arm_sp_bone->setVisibility(analysis_objects.show_sagittal_plane);
		analysis_objects.spine_sp_bone->setVisibility(analysis_objects.show_sagittal_plane);

		analysis_objects.transverse_plane_image->setVisibility(analysis_objects.show_transverse_plane);

		//analysis_objects.coronal_plane_image->setVisibility(true);
		//analysis_objects.sagittal_plane_image->setVisibility(true);
		//analysis_objects.transverse_plane_image->setVisibility(true);

		analysis_objects.right_dir_marker->setVisibility(SHOW_DIR_VECTORS);
		analysis_objects.forward_dir_marker->setVisibility(SHOW_DIR_VECTORS);
		analysis_objects.up_dir_marker->setVisibility(SHOW_DIR_VECTORS);

		// copy data to HUD
		hud_data.flexion = flexion;
		hud_data.extension = extension;
		hud_data.abduction = abduction;
		hud_data.dir_test = dir_test;
		hud_data.max_flexion = max_flexion;
		hud_data.max_extension = max_extension;
		hud_data.max_abduction = max_abduction;
		hud_data.animation_frame = animation_frame;
		hud_data.animation_time = animation_time;
		hud_data.animation_paused = anim_ctrl.isFrozen();
		hud_data.animation_time_warp = anim_ctrl.getTimeWarp();
		hud_data.analysis_mode;
		if (process_control.whichArm() == RIGHT)
			hud_data.analysis_mode = "RIGHT ARM ";
		else
			hud_data.analysis_mode = "LEFT ARM ";
		hud_data.analysis_mode += toString(process_control.dataMode());
		hud_data.recording = animation_time >= process_control.skip();

		if (DISPLAY_ALL_RESULTS)
		{
			hud_data.focus_abduction = true;
			hud_data.focus_extension = true;
			hud_data.focus_flexion = true;
		}
		else
		{
			hud_data.focus_abduction = process_control.dataMode() == ABDUCTION;
			hud_data.focus_extension = process_control.dataMode() == EXTENSION;
			hud_data.focus_flexion = process_control.dataMode() == FLEXION;
		}
	}
}

void loadNextMotion() {

	if (!process_control.active()) shutDown(0);

	cout << "Processing " << process_control.takeLabel() << endl;
	cout << "  mode = " << toString(process_control.dataMode()) << endl;
	cout << "  arm = " << toString(process_control.whichArm()) << endl;
	cout << "  loop = " << toString(process_control.loop()) << endl;
	cout << "  skip time = " << process_control.skip() << " seconds" << endl;
	anim_ctrl.loadCharacter(process_control.motionFile());
	cout << "  frames = " << anim_ctrl.numFrames() << endl;

	results_file << "Processing " << process_control.takeLabel() << endl;
	results_file << "  mode = " << toString(process_control.dataMode()) << endl;
	results_file << "  arm = " << toString(process_control.whichArm()) << endl;
	results_file << "  loop = " << toString(process_control.loop()) << endl;
	results_file << "  skip time = " << process_control.skip() << " seconds" << endl;
	results_file << "  frames = " << anim_ctrl.numFrames() << endl;

	analysis_objects.show_coronal_plane = false;
	analysis_objects.show_sagittal_plane = false;
	analysis_objects.show_transverse_plane = false;
	switch (process_control.dataMode()) {
	case ABDUCTION: 
		analysis_objects.show_coronal_plane = true; break;
	case EXTENSION:
	case FLEXION:
		analysis_objects.show_sagittal_plane = true; break;
	}

	if (!anim_ctrl.isReady())
	{
		results_file << "main(): Unable to load character. Aborting program." << endl;
		cerr << "main(): Unable to load character. Aborting program." << endl;
		shutDown(1);
	}
	hud_data.take_label = process_control.takeLabel();

	if (process_control.loop() == false) anim_ctrl.setStopAtLastFrame();
	else anim_ctrl.clearStopAtLastFrame();

	// Need to create the bones, even if we're not using graphics.
	// The skeleton uses the bone objects to store bone endpoints.
	// FIXIT:170123 Need to delete the old bones before clearing the render list.
	// FIXIT:170123 Verify the AnimationControl.loadCharacter properly cleans up from the previous load.
	character_render_list.clear();  
	anim_ctrl.getRenderList(character_render_list);

	// reset clock so that the time spent loading is not included in animation time
	system_timer.reset();
}

void updateAnimation()
{
	if (goto_next_process) {
		process_control.advance();
		loadNextMotion();
		resetMaxValues();
		goto_next_process = false;
		animation_has_finished_one_pass = false;
	}

	// Determine how much time has passed since the previous frame.
	double elapsed_time = system_timer.elapsedTime();

	// Check to see if any user inputs have been received since the last frame.
	input_processor.processInputs((float)elapsed_time);

	if (!anim_ctrl.isReady())
	{
		animation_time = 0.0f;
		animation_frame = 0;
		return;
	}
	
	bool status = false;
	if (real_time_mode)
		status = anim_ctrl.updateAnimation((float)elapsed_time);
	else
		status = anim_ctrl.framestepAnimation();

	if (anim_ctrl.looped() || anim_ctrl.stoppedAtLastFrame())
	{
		if (!process_control.loop())
			goto_next_process = true;
		if (!animation_has_finished_one_pass) {
			animation_has_finished_one_pass = true;
			switch (process_control.dataMode()) {
			case ABDUCTION:
				cout << "  max abduction: " << data_recorder.maxAbduction() << endl;
				results_file << "  max abduction: " << data_recorder.maxAbduction() << endl;
				break;
			case FLEXION:
				cout << "  max flexion: " << data_recorder.maxFlexion() << endl;
				results_file << "  max flexion: " << data_recorder.maxFlexion() << endl;
				break;
			case EXTENSION:
				cout << "  max extension: " << data_recorder.maxExtension() << endl;
				results_file << "  max extension: " << data_recorder.maxExtension() << endl;
				break;
			}
			data_recorder.writeToFile(process_control.resultFile());
			data_recorder.erase();
		}
	}
	animation_time = anim_ctrl.getAnimationTime();
	animation_frame = anim_ctrl.getAnimationFrame();
	
	processPTData();
}

int main(int argc, char **argv)
{
	char results_filename[2001];
	time_t t = time(NULL);
	strftime(results_filename, 2000, "results_%y-%m-%d_%H-%M-%S.txt", localtime(&t));
	results_file.open(results_filename);

	if (!readCommandFile())
	{
		results_file << "No command file found. Exiting program." << endl;
		cerr << "No command file found. Exiting program." << endl;
		exit(1);
	}
	//process_control.printProcessControl();

	if (!process_control.active()) 
	{
		results_file << "No processing requests found. Exiting program." << endl;
		cerr << "No processing requests found. Exiting program." << endl;
		exit(1);
	}

	// load first motion before entering the processing loops
	loadNextMotion();
	goto_next_process = false;
	animation_has_finished_one_pass = false;

	if (process_control.animationIsEnabled())
	{
		// initialize openGL and enter its rendering loop.
		try
		{
			// Start the global system timer/clock.
			system_timer.reset();

			// Initialize the graphics module.
			launchRenderer(argc, argv, updateAnimation);
		}
		catch (BasicException& excpt)
		{
			results_file << "BasicException caught at top level." << endl;
			results_file << "Exception message: " << excpt.msg << endl;
			results_file << "Aborting program." << endl;
			cerr << "Aborting due to exception. See log file for details." << endl;
			shutDown(1);
		}
	}
	else
	{
		real_time_mode = false;
		while (true)
		{
			updateAnimation();
		}
		shutDown(0);
	}
}

