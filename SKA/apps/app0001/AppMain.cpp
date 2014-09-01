//-----------------------------------------------------------------------------
// app0001 - Builds with SKA Version 3.1 - Sept 01, 2012 - Michael Doherty
//-----------------------------------------------------------------------------
// app0001: Demo program demonstrates how to animate a character based on
//          an ASF/AMC file pair.
//          It also illustrated various useful things that aren't
//          directly related to the character animation.
//          (1) additional objects, such as ground, sky and coordinate axes
//          (2) moveable camera, controlled by the camera and mouse.
//          (3) keyboard filtering, to avoid multiple responses when
//              a single keystroke is expected.
//          (4) animation speed control (freeze, single step, time warp)
//          (5) heads-up display (2D text on screen)
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
using namespace std;
// openGL library
#include <GL/glut.h>
// SKA modules
#include <Core/BasicException.h>
#include <Core/SystemTimer.h>
#include <Core/Utilities.h>
#include <DataManagement/DataManagementException.h>
#include <Input/InputManager.h>
#include <Graphics/GraphicsInterface.h>
#include <Graphics/Lights.h>
#include <Graphics/Textures.h>
#include <Graphics/Graphics2D.h>
#include <Models/SphereModels.h>
// local application
#include "AppConfig.h"
#include "AnimationControl.h"
#include "CameraControl.h"
#include "InputProcessing.h"

// pointers to animated objects that need to be drawn (bones)
list<Object*> anim_render_list;
// pointers to background objects that need to be drawn
list<Object*> bg_render_list;

// default window size
static int window_height = 800;
static int window_width = 800;

// which objects background objects do we want to see?
static bool SHOW_SKY = true;
static bool SHOW_GROUND = true;
static bool SHOW_COORD_AXIS = true;

//  background color (black)
static float clear_color[4] = { 0.0f, 0.0f, 0.0f, 0.0f};

// heads-up display = 2D text on screen
void drawHUD()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	Color color(1.0f,1.0f,0.0f,1.0f);
	float y = 0.9f;
	string s;

	s = "Time Warp: " + toString(anim_ctrl.getTimeWarp());
	renderString(0.5f, y, 0.0f, color, s.c_str());
	y -= 0.05f;
	if (anim_ctrl.isFrozen())
	{
		s = "Animation Frozen";
		renderString(0.5f, y, 0.0f, color, s.c_str());
	}
}

// display() is the call back function from the openGL rendering loop.
// All recurring processing is initiated from this function.
void display(void)
{
	// Determine how much time has passed since the previous frame.
	double elapsed_time = system_timer.elapsedTime();

	// Check to see if any user inputs have been received since the last frame.
	input_processor.processInputs(elapsed_time);

	// Set up openGL to draw next frame.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	camera.setSceneView();
	glMatrixMode(GL_MODELVIEW);

	// draw background objects
	list<Object*>::iterator biter = bg_render_list.begin();
	while (biter != bg_render_list.end())
	{
		Object* go = (Object*)(*biter);
		if (go->isVisible()) 
		{
			Matrix4x4 world_xform;
			(*biter)->render(world_xform);
		}
		biter++;
	}

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

	// draw the heads-up display
	drawHUD();

	// Activate the new frame.
	glutSwapBuffers();

	// Record any redering errors.
	checkOpenGLError(203);
}

void buildObjects()
{
	// tell texture manager where to find texture BMP files
	texture_manager.addTextureFilepath((char*)TEXTURE_FILE_PATH);

	if (SHOW_SKY)
	{
		// create a sky model directly by creating an instance of InvertedSphereModel
		// which is a textured model
		SphereModel* skymod = new InvertedSphereModel(800, 3, 
			Color(1.0f,1.0f,0.5f),(char*)"skymap1.bmp");
		// build a sky object associated with the sky model 
		Object* sky = new Object(skymod, Vector3D(0.0f,0.0f,0.0f), Vector3D(0.0f,0.0f,0.0f));
		// store sky object for rendering
		bg_render_list.push_back(sky);
	}

	if (SHOW_GROUND)
	{
		// create a ground model indirectly by defining a ModelSpecification
		ModelSpecification groundspec("Ground");
		// build a ground object associated with the ground model 
		Object* ground = new Object(groundspec, 
			Vector3D(0.0f,0.0f,0.0f), Vector3D(0.0f,0.0f,0.0f), Vector3D(1.0f,1.0f,1.0f));
		// store ground object for rendering
		bg_render_list.push_back(ground);
	}

	if (SHOW_COORD_AXIS)
	{
		// create a coordinate axes model indirectly by defining a ModelSpecification
		ModelSpecification caxisspec("CoordinateAxis");
		caxisspec.addSpec("length", "100");
		// build a coordinate axes object associated with the coordinate axes model 
		Object* caxis = new Object(caxisspec, 
			Vector3D(0.0f,0.0f,0.0f), Vector3D(0.0f,0.0f,0.0f));
		// store coordinate axes object for rendering
		bg_render_list.push_back(caxis);
	}
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
	anim_ctrl.loadCharacters(anim_render_list);
	if (!anim_ctrl.isReady()) 
	{
		logout << "main(): Unable to load characters. Aborting program." << endl;
		return 1;
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
		camera.initializeCamera(window_width, window_height);
		
		// construct background objects
		buildObjects();

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
