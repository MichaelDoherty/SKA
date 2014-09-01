//-----------------------------------------------------------------------------
// app0005 - Builds with SKA Version 3.1 - Sept 01, 2012 - Michael Doherty
//-----------------------------------------------------------------------------
// app0005 - demonstrates control of a three bone arm 
//     with inverse kinematics tracking a moving target.
//-----------------------------------------------------------------------------
// AppMain.cpp
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

// Boxes positioned by the AnimationControl.
Object* target_box = NULL;
Object* effector_box = NULL;

// pointers to animated objects that need to be drawn (bones)
list<Object*> anim_render_list;
// pointerts to background objects that need to be drawn
list<Object*> bg_render_list;

static int window_height = 800;
static int window_width = 800;

static bool draw_coord_axis = true;
static bool draw_sky = false;
static bool draw_ground = false;
static bool draw_figures = true;

//  background color
static float clear_color[4] = { 0.1f, 0.1f, 0.1f, 0.1f};

Object* createMarkerBox(Vector3D position, Color c)
{
	ModelSpecification markerspec("Box",c);
	markerspec.addSpec("length", "0.5");
	markerspec.addSpec("width", "0.5");
	markerspec.addSpec("height", "0.5");
	Object* marker = new Object(markerspec, 
		position, Vector3D(0.0f,0.0f,0.0f));
	bg_render_list.push_back(marker);
	return marker;
}

InputProcessor input_processor;

void drawHUD()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	Color c1(1.0f,1.0f,0.0f,1.0f);
	Color c2(0.0f,1.0f,1.0f,1.0f);
	Color black(0.0f,0.0f,0.0f,1.0f);
	float y = 0.9f;

	float time_warp = anim_ctrl.getTimeWarp();
	if (time_warp < 1.0f)
	{
		string s = "time warp: " + toString(time_warp);
		renderString(0.5f, y, 0.0f, c1, s.c_str());
	}
}

void display(void)
{
	double elapsed_time = system_timer.elapsedTime();

	input_processor.processInputs(elapsed_time);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	camera.setSceneView();

	glMatrixMode(GL_MODELVIEW);

	time_fade_factor = 1.0f;
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

	if (anim_ctrl.ready)
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

	drawHUD();

	glutSwapBuffers();
	checkOpenGLError(203);
}

static void Reshape(int w, int h)
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

void buildObjects()
{
	texture_manager.addTextureFilepath((char*)TEXTURE_FILE_PATH);
	if (draw_sky)
	{
		char* skymapfile = (char*)"skymap1.bmp";
		SphereModel* skymod = new InvertedSphereModel(800, 3, Color(1.0f,1.0f,0.5f), skymapfile);
		Object* sky = new Object(skymod, 
			Vector3D(0.0f,0.0f,0.0f), Vector3D(0.0f,0.0f,0.0f));
		bg_render_list.push_back(sky);
	}

	if (draw_coord_axis)
	{
		ModelSpecification caxisspec("CoordinateAxis");
		caxisspec.addSpec("length", "100");
		Object* caxis = new Object(caxisspec, 
			Vector3D(0.0f,0.0f,0.0f), Vector3D(0.0f,0.0f,0.0f));
		bg_render_list.push_back(caxis);
	}

	if (draw_ground)
	{
		ModelSpecification groundspec("Ground");
		Object* ground = new Object(groundspec, 
			Vector3D(0.0f,0.0f,0.0f), Vector3D(0.0f,0.0f,0.0f), Vector3D(1.0f,1.0f,1.0f));
		bg_render_list.push_back(ground);
	}

	target_box = createMarkerBox(Vector3D(0.0f, 0.0f, 0.0f), Color(1.0f,0.0f,0.0f));
	bg_render_list.push_back(target_box);
	effector_box = createMarkerBox(Vector3D(0.0f, 0.0f, 0.0f), Color(0.0f,1.0f,0.0f));
	bg_render_list.push_back(effector_box);
}

void initializeRenderer()
{
    glEnable(GL_DEPTH_TEST);
	glClearDepth(1.0f);
	glShadeModel(GL_SMOOTH);
	glClearColor(clear_color[0], clear_color[1], clear_color[2], clear_color[3]);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void initializeGLUT(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(window_width, window_height);
	glutCreateWindow("Animation");
}

int main(int argc, char **argv)
{
	if (draw_figures)
	{

		anim_ctrl.loadCharacters(anim_render_list);
		if (!anim_ctrl.isReady()) 
			logout << "main(): Unable to load characters. Proceeding with no animation." << endl;
	}

	try
	{
		initializeGLUT(argc, argv);
		initializeRenderer();
		input_manager.registerGlutHandlers();

		glutReshapeFunc(Reshape); 
		glutDisplayFunc(display);
		glutIdleFunc(display);

		initializeDefaultLighting();
		initializeCamera(window_width, window_height);
		buildObjects();
		checkOpenGLError(202);
		system_timer.reset();
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
