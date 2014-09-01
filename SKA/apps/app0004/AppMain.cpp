//-----------------------------------------------------------------------------
// app0004 - Builds with SKA Version 3.1 - Sept 01, 2012 - Michael Doherty
//-----------------------------------------------------------------------------
// app0004: Program controlled motion of legs using 
//          a new motion MotionController subclass.
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
#include "ExperimentalController.h"

// pointers to animated objects that need to be drawn (bones)
list<Object*> anim_render_list;
// pointers to background objects that need to be drawn
list<Object*> bg_render_list;

static int window_height = 800;
static int window_width = 800;

// which objects background objects do we want to see?
static bool SHOW_SKY = true;
static bool SHOW_GROUND = true;
static bool SHOW_COORD_AXIS = true;

//  background color
static float clear_color[4] = { 0.1f, 0.1f, 0.1f, 0.1f};

Object* createMarkerBox(Vector3D position)
{
	Color color = Color(0.8f,0.3f,0.3f);
	ModelSpecification markerspec("Box",color);
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
	Skeleton* skel = anim_ctrl.characters[0];

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	Color c1(1.0f,1.0f,0.0f,1.0f);
	Color c2(0.0f,1.0f,1.0f,1.0f);
	Color black(0.0f,0.0f,0.0f,1.0f);

	string s;
	// columns
	float x[10] = { -0.9f, -0.7f, -0.5f, -0.3f, -0.1f, 0.1f, 0.3f, 0.5f, 0.7f, 0.9f };
	// rows
	float y[10] = { 0.9f, 0.85f, 0.8f, 0.75f, 0.7f, 0.65f, 0.6f, 0.55f, 0.5f, 0.45f };

	s = "time: "; renderString(x[0], y[0], 0.0f, c1, s.c_str());
	s = toString(anim_ctrl.world_time);	renderString(x[2], y[0], 0.0f, c1, s.c_str());

	s = "root position: "; renderString(x[0], y[1], 0.0f, c1, s.c_str());
	CHANNEL_ID c01(ROOT_BONE_ID, CT_TX);
	CHANNEL_ID c02(ROOT_BONE_ID, CT_TY);
	CHANNEL_ID c03(ROOT_BONE_ID, CT_TZ);
	s = toString(skel->getChannelValue(c01)); renderString(x[2], y[1], 0.0f, c1, s.c_str());
	s = toString(skel->getChannelValue(c02)); renderString(x[4], y[1], 0.0f, c1, s.c_str()); 
	s = toString(skel->getChannelValue(c03)); renderString(x[6], y[1], 0.0f, c1, s.c_str());

	s = "root orientation: "; renderString(x[0], y[2], 0.0f, c1, s.c_str());
	CHANNEL_ID c04(ROOT_BONE_ID, CT_RX);
	CHANNEL_ID c05(ROOT_BONE_ID, CT_RY);
	CHANNEL_ID c06(ROOT_BONE_ID, CT_RZ);
	s = toString(skel->getChannelValue(c04)); renderString(x[2], y[2], 0.0f, c1, s.c_str());
	s = toString(skel->getChannelValue(c05)); renderString(x[4], y[2], 0.0f, c1, s.c_str()); 
	s = toString(skel->getChannelValue(c06)); renderString(x[6], y[2], 0.0f, c1, s.c_str());

	s = "lfemur orientation: "; renderString(x[0], y[3], 0.0f, c1, s.c_str());
	CHANNEL_ID c07(LFEMUR_BONE_ID, CT_RX);
	CHANNEL_ID c08(LFEMUR_BONE_ID, CT_RY);
	CHANNEL_ID c09(LFEMUR_BONE_ID, CT_RZ);
	s = toString(skel->getChannelValue(c07)); renderString(x[2], y[3], 0.0f, c1, s.c_str());
	s = toString(skel->getChannelValue(c08)); renderString(x[4], y[3], 0.0f, c1, s.c_str()); 
	s = toString(skel->getChannelValue(c09)); renderString(x[6], y[3], 0.0f, c1, s.c_str());

	s = "rfemur orientation: "; renderString(x[0], y[4], 0.0f, c1, s.c_str());
	CHANNEL_ID c10(RFEMUR_BONE_ID, CT_RX);
	CHANNEL_ID c11(RFEMUR_BONE_ID, CT_RY);
	CHANNEL_ID c12(RFEMUR_BONE_ID, CT_RZ);
	s = toString(skel->getChannelValue(c10)); renderString(x[2], y[4], 0.0f, c1, s.c_str());
	s = toString(skel->getChannelValue(c11)); renderString(x[4], y[4], 0.0f, c1, s.c_str()); 
	s = toString(skel->getChannelValue(c12)); renderString(x[6], y[4], 0.0f, c1, s.c_str());

	s = "ltibia orientation: "; renderString(x[0], y[5], 0.0f, c1, s.c_str());
	CHANNEL_ID c13(LTIBIA_BONE_ID, CT_RX);
	CHANNEL_ID c14(LTIBIA_BONE_ID, CT_RY);
	CHANNEL_ID c15(LTIBIA_BONE_ID, CT_RZ);
	s = toString(skel->getChannelValue(c13)); renderString(x[2], y[5], 0.0f, c1, s.c_str());
	s = toString(skel->getChannelValue(c14)); renderString(x[4], y[5], 0.0f, c1, s.c_str()); 
	s = toString(skel->getChannelValue(c15)); renderString(x[6], y[5], 0.0f, c1, s.c_str());

	s = "rtibia orientation: "; renderString(x[0], y[6], 0.0f, c1, s.c_str());
	CHANNEL_ID c16(RTIBIA_BONE_ID, CT_RX);
	CHANNEL_ID c17(RTIBIA_BONE_ID, CT_RY);
	CHANNEL_ID c18(RTIBIA_BONE_ID, CT_RZ);
	s = toString(skel->getChannelValue(c16)); renderString(x[2], y[6], 0.0f, c1, s.c_str());
	s = toString(skel->getChannelValue(c17)); renderString(x[4], y[6], 0.0f, c1, s.c_str()); 
	s = toString(skel->getChannelValue(c18)); renderString(x[6], y[6], 0.0f, c1, s.c_str());

	s = "lfoot orientation: "; renderString(x[0], y[7], 0.0f, c1, s.c_str());
	CHANNEL_ID c19(LFOOT_BONE_ID, CT_RX);
	CHANNEL_ID c20(LFOOT_BONE_ID, CT_RY);
	CHANNEL_ID c21(LFOOT_BONE_ID, CT_RZ);
	s = toString(skel->getChannelValue(c19)); renderString(x[2], y[7], 0.0f, c1, s.c_str());
	s = toString(skel->getChannelValue(c20)); renderString(x[4], y[7], 0.0f, c1, s.c_str()); 
	s = toString(skel->getChannelValue(c21)); renderString(x[6], y[7], 0.0f, c1, s.c_str());

	s = "rfoot orientation: "; renderString(x[0], y[8], 0.0f, c1, s.c_str());
	CHANNEL_ID c22(RFOOT_BONE_ID, CT_RX);
	CHANNEL_ID c23(RFOOT_BONE_ID, CT_RY);
	CHANNEL_ID c24(RFOOT_BONE_ID, CT_RZ);
	s = toString(skel->getChannelValue(c22)); renderString(x[2], y[8], 0.0f, c1, s.c_str());
	s = toString(skel->getChannelValue(c23)); renderString(x[4], y[8], 0.0f, c1, s.c_str()); 
	s = toString(skel->getChannelValue(c24)); renderString(x[6], y[8], 0.0f, c1, s.c_str());
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
	anim_ctrl.loadCharacters(anim_render_list);
	if (!anim_ctrl.isReady()) 
		logout << "main(): Unable to load characters. Proceeding with no animation." << endl;

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
