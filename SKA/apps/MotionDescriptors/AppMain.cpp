//-----------------------------------------------------------------------------
// MotionDescriptors project - Builds with SKA Version 4.0
//-----------------------------------------------------------------------------
// AppMain.cpp
//    The main program is mostly the connection between openGL,
//    SKA and application specific code. It also controls the order of
//    initialization, before control is turned over to openGL.
//-----------------------------------------------------------------------------
// SKA configuration.
#include <Core/SystemConfiguration.h>
// C/C++ libraries
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
#include <Math/Plane.h>

// local application
#include "AppConfig.h"
#include "AppGraphics.h"
#include "AnimationControl.h"
#include "InputProcessing.h"
#include "ProcessControl.h"
#include "MotionAnalyzer.h"
#include "ShoulderAnalyzer.h"

static MotionAnalyzer* motion_analyzer = NULL;
static ShoulderAnalyzer* shoulder_analyzer = NULL;

void shutDown(int _exit_code)
{
	if (motion_analyzer != NULL) delete motion_analyzer;
	if (shoulder_analyzer != NULL) delete shoulder_analyzer;
	exit(_exit_code);
}

void loadNextMotion() {

	if (!process_control.active()) shutDown(0);

	ProcessControl::ProcessingRequest prequest = process_control.currentRequest();
	cout << "Active Processing Request:" << endl;
	cout << prequest;

	if (prequest.mocap_file_type == ProcessControl::BVH)
		anim_ctrl.loadBVHCharacter(prequest.motion_file);
	else
		anim_ctrl.loadAMCCharacter(prequest.skeleton_file, prequest.motion_file);

	cout << "  frames = " << anim_ctrl.numFrames() << endl;
	
	if (prequest.shoulder_mode == ProcessControl::NONE)
	{
		analysis_objects.show_coronal_plane = false;
		analysis_objects.show_sagittal_plane = false;
		analysis_objects.show_transverse_plane = false;
	}
	else
	{
		switch (prequest.shoulder_mode) {
		case ProcessControl::NONE:
			break;
		case ProcessControl::ABDUCTION:
			analysis_objects.show_coronal_plane = true; break;
		case ProcessControl::EXTENSION:
		case ProcessControl::FLEXION:
			analysis_objects.show_sagittal_plane = true; break;
		}
	}
	if (!anim_ctrl.isReady())
	{
		logout << "main(): Unable to load character. Aborting program." << endl;
		shutDown(1);
	}
	hud_data.take_label = process_control.currentRequest().take_label;

	// Need to create the bones, even if we're not using graphics.
	// The skeleton uses the bone objects to store bone endpoints.
	// FIXIT:170123 Need to delete the old bones before clearing the render list.
	// FIXIT:170123 Verify the AnimationControl.loadCharacter properly cleans up from the previous load.
	character_render_list.clear();
	anim_ctrl.getRenderList(character_render_list);

	if (motion_analyzer != NULL) { delete motion_analyzer; motion_analyzer = NULL; }
	if (process_control.currentRequest().run_motion_analysis)
	{
		float frame_duration;
		if (process_control.realTimeMode())
			frame_duration = anim_ctrl.getFrameDuration();
		else
			frame_duration = 1.0f/process_control.currentRequest().fps;
		motion_analyzer = new MotionAnalyzer(anim_ctrl.numFrames(), frame_duration,
			anim_ctrl.getSkeleton());
	}
	if (shoulder_analyzer != NULL) { delete shoulder_analyzer; shoulder_analyzer = NULL; }
	if (process_control.currentRequest().shoulder_mode != ProcessControl::NONE)
		shoulder_analyzer = new ShoulderAnalyzer();

	// reset clock so that the time spent loading is not included in animation time
	system_timer.reset();
}

void updateAnimation()
{
	if (process_control.gotoNextProcess()) {
		process_control.advance();
		loadNextMotion();
		if (shoulder_analyzer != NULL) shoulder_analyzer->resetMaxValues();
		process_control.clearGotoNextProcess();
		process_control.clearAnimationHasLooped();
	}

	// Determine how much time has passed since the previous frame.
	double elapsed_time = system_timer.elapsedTime();


	// Check to see if any user inputs have been received since the last frame.
	input_processor.processInputs((float)elapsed_time);

	if (!anim_ctrl.isReady())
	{
		return;
	}
	
	bool status = false;
	if (process_control.realTimeMode())
		status = anim_ctrl.updateAnimation((float)elapsed_time);
	else
		status = anim_ctrl.framestepAnimation();
	if (status == false) cout << "Error in AnimationController frame update." << endl;

	if (anim_ctrl.looped())
	{
		if (!process_control.currentRequest().loop)
		{
			process_control.setGotoNextProcess();
		}
		if (!process_control.animationHasLooped()) {
			process_control.setAnimationHasLooped();
			if (motion_analyzer != NULL) motion_analyzer->storeResults(process_control.currentRequest().output_folder, process_control.currentRequest().take_label);
			if (shoulder_analyzer != NULL) shoulder_analyzer->storeResults();
		}
	}

	long current_frame = anim_ctrl.getAnimationFrame();
	hud_data.animation_frame = current_frame;

	if (motion_analyzer != NULL)
	{
		float frame_duration;
		if (process_control.realTimeMode())
			frame_duration = anim_ctrl.getFrameDuration();
		else
			frame_duration = 1.0f/ process_control.currentRequest().fps;
		motion_analyzer->analyzeCurrentFrame(current_frame, frame_duration);
	}
	if (shoulder_analyzer != NULL) 
		shoulder_analyzer->processPTData();
}

int main(int argc, char **argv)
{
	if (!process_control.readCommandFile())
	{
		cerr << "No command file found. Exiting program." << endl;
		exit(1);
	}
	//process_control.printProcessControl();

	if (!process_control.active())
	{
		cerr << "No processing requests found. Exiting program." << endl;
		exit(1);
	}

	// load first motion before entering the processing loops
	loadNextMotion();
	process_control.clearGotoNextProcess();
	process_control.clearAnimationHasLooped();

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
			logout << "BasicException caught at top level." << endl;
			logout << "Exception message: " << excpt.msg << endl;
			logout << "Aborting program." << endl;
			cerr << "Aborting due to exception. See log file for details." << endl;
			shutDown(1);
		}
	}
	else
	{
		process_control.clearRealTimeMode();
		while (true)
		{
			updateAnimation();
		}
		shutDown(0);
	}
}

