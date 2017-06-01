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
#include "DataRecorder.h"

static MotionAnalyzer* motion_analyzer = NULL;
static ShoulderAnalyzer* shoulder_analyzer = NULL;

// probably unnecessary - used in case any cleanup is needed on exit.
void shutDown(int _exit_code)
{
	if (motion_analyzer != NULL) delete motion_analyzer;
	if (shoulder_analyzer != NULL) delete shoulder_analyzer;
	exit(_exit_code);
}

void loadNextMotion() {

	if (!process_control.active()) shutDown(0);

	cout << "Processing " << process_control.takeLabel() << endl;
	cout << "  mode = " << toString(process_control.dataMode()) << endl;
	cout << "  loop = " << toString(process_control.loop()) << endl;
	cout << "  skip time = " << process_control.skip() << " seconds" << endl;
	anim_ctrl.loadCharacter(process_control.motionFile());
	cout << "  frames = " << anim_ctrl.numFrames() << endl;
	/*Trevor's Code: Show if tracking is enabled*/
	if (process_control.trackingIsEnabled()) {
		cout << "  Tracking is enabled\n  Tracking " << process_control.getBoneName() << " bone" << endl;
		cout << "  Across " << process_control.getPlaneName() << " plane" << endl;
		cout << "  Tracking Skip: " << process_control.getFrameStep() << endl;
	}
	else {
		cout << "  Tracking is disabled\n";
	}

	analysis_objects.show_coronal_plane = true;
	analysis_objects.show_sagittal_plane = true;
	analysis_objects.show_transverse_plane = true;
	switch (process_control.dataMode()) {
	case ABDUCTION:
		analysis_objects.show_coronal_plane = true; break;
	case EXTENSION:
	case FLEXION:
		analysis_objects.show_sagittal_plane = true; break;
	}

	if (!anim_ctrl.isReady())
	{
		logout << "main(): Unable to load character. Aborting program." << endl;
		shutDown(1);
	}
	hud_data.take_label = process_control.takeLabel();

	// Need to create the bones, even if we're not using graphics.
	// The skeleton uses the bone objects to store bone endpoints.
	// FIXIT:170123 Need to delete the old bones before clearing the render list.
	// FIXIT:170123 Verify the AnimationControl.loadCharacter properly cleans up from the previous load.
	character_render_list.clear();
	anim_ctrl.getRenderList(character_render_list);

	motion_analyzer = new MotionAnalyzer(anim_ctrl.numFrames(), anim_ctrl.getFrameDuration(),
		anim_ctrl.getSkeleton());
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

	if (anim_ctrl.looped())
	{
		if (!process_control.loop())
			process_control.setGotoNextProcess();
		if (!process_control.animationHasLooped()) {
			process_control.setAnimationHasLooped();
			if (shoulder_analyzer != NULL) shoulder_analyzer->storeResults();
		}
	}

	// FIXIT!! If in real-time animation mode, we also need to pass the actual frame duration 
	//         to the motion analyzer.
	if (motion_analyzer != NULL) motion_analyzer->analyzeCurrentFrame(anim_ctrl.getAnimationFrame());
	if (shoulder_analyzer != NULL) shoulder_analyzer->processPTData();
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

