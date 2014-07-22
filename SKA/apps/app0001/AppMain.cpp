//-----------------------------------------------------------------------------
// app0001: Demo program illustrating how to build a basic SKA application.
//          This application reads a skeleton and motion from an ASF/AMC 
//          file pair and uses that data to drive a character.
//-----------------------------------------------------------------------------
// AppMain.h
//    The main program is mostly the connection between openGL, 
//    SKA and application specific code. It also controls the order of 
//    initialization, before control is turned over to openGL.
//-----------------------------------------------------------------------------
// Builds with SKA Version 3.0 - July 22, 2012 - Michael Doherty
//-----------------------------------------------------------------------------
// SKA configuration - should always be the first file included.
#include <Core/SystemConfiguration.h>
// C/C++ libraries
#include <cstdio>
#include <cstdlib>
#include <iostream>
using namespace std;
// openGL library
#include <GL/glut.h>
// SKA modules
#include <Core/BasicException.h>
#include <Core/SystemTimer.h>
#include <DataManagement/DataManagementException.h>
#include <Input/InputManager.h>
#include <Graphics/GraphicsInterface.h>
#include <Graphics/Lights.h>
// local application
#include "AppConfig.h"
#include "AnimationControl.h"
#include "CameraControl.h"
#include "InputProcessing.h"

// pointers to animated objects that need to be drawn (bones)
list<Object*> anim_render_list;

// default window size
static int window_height = 800;
static int window_width = 800;

//  background color (black)
static float clear_color[4] = { 0.0f, 0.0f, 0.0f, 0.0f};

// display() is the call back function from the openGL rendering loop.
// All recurring processing is initiated from this function.
void display(void)
{
	// Determine how much time has passed since the previous frame.
	double elapsed_time = system_timer.elapsedTime();

	// Check to see if any user inputs have been received since the last frame.
	input_processor.processInputs();

	// Set up openGL to draw next frame.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	camera.setSceneView();
	glMatrixMode(GL_MODELVIEW);

	// Tell the animation subsystem to update the character, then redraw it.
	if (anim_ctrl.isReady())
	{
		if (anim_ctrl.updateAnimation(elapsed_time))
		{
			list<Object*>::iterator aiter = anim_render_list.begin();
			while (aiter != anim_render_list.end())
			{
				Object* go = (Object*)(*aiter);
				if (go->isVisible())
				{
					Matrix4x4 world_xform;
					(*aiter)->render(world_xform);
				}
				aiter++;
			}
		}
	}

	// Activate the new frame.
	glutSwapBuffers();

	// Record any redering errors.
	checkOpenGLError(203);
}

// reshape() is a call back from openGL to indicate that the window has 
// changed size. Adjust the camera and view parameters accordingly.
static void reshape(int w, int h)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	window_width = w; window_height = h; 
	glViewport(0, 0, window_width, window_height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	camera.projectionParameters(
		40.0,									// vertical FOV
		float(window_width)/window_height,		// aspect ratio
		1.0,									// near plane
		1000.0);								// far plane
}

// initializeRenderer() is called one time during initialization.
// It sets various openGL parameters that control rendering.
void initializeRenderer()
{
    glEnable(GL_DEPTH_TEST);
	glClearDepth(1.0f);
	glShadeModel(GL_SMOOTH);
	glClearColor(clear_color[0], clear_color[1], clear_color[2], clear_color[3]);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

// initializeGLUT() is called one time during initialization.
// It initializes GLUT.
void initializeGLUT(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(window_width, window_height);
	glutCreateWindow("Animation");
}

int main(int argc, char **argv)
{
	// initialize the animation subsystem, which reads the
	// mocap data files and sets up the character(s)
	try
	{
		anim_ctrl.loadCharacters(anim_render_list);
	}
	catch (DataManagementException& e)
	{
		logout << "Exception while loading characters" << endl;
		logout << e.msg << endl;
		cerr << "Exception while loading characters" << endl;
		cerr << e.msg << endl;
		exit(1);
	}

	// initialize openGL and enter its rendering loop.
	try
	{
		initializeGLUT(argc, argv);
		initializeRenderer();
		// The input manager needs to have register call back function with GLUT
		// to receive keyboard and mouse actions. This needs to happen after GLUT
		// initialization, so it is implemented in a member functions, rather than
		// the InputManager constructor.
		input_manager.registerGlutHandlers();

		glutReshapeFunc(reshape); 
		glutDisplayFunc(display);
		glutIdleFunc(display);

		// Call into the SKA Graphics module to select the default lights.
		initializeDefaultLighting();

		// Application specific initialization of the camera/viewpoint.
		initializeCamera(window_width, window_height);
		
		// Record any openGL errors.
		checkOpenGLError(202);

		// Start the global system timer/clock.
		system_timer.reset();

		// Jump into the openGL render loop.
		glutMainLoop();
	}
	catch (BasicException& excpt)
	{
		logout << "BasicException caught at top level." << endl;
		logout << "Exception message: " << excpt.msg << endl;
		logout << "Aborting program." << endl;
		cerr << "Aborting due to exception. See log file for details." << endl;
		exit(1);
	}
}
