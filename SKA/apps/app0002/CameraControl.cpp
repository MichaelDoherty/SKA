//-----------------------------------------------------------------------------
// app0002 - Builds with SKA Version 3.1 - Sept 01, 2012 - Michael Doherty
//-----------------------------------------------------------------------------
// CameraControl.cpp
//    Object to control the camera/viewpoint.
//-----------------------------------------------------------------------------
// SKA configuration
#include <Core/SystemConfiguration.h>
// SKA modules
#include <Math/Vector3D.h>
// local application
#include "CameraControl.h"

AppCamera camera;

AppCamera::AppCamera() : MovingCamera()
{
}

AppCamera::~AppCamera()
{
}

void AppCamera::initializeCamera(int window_width, int window_height)
{
	camera.projectionParameters(
		40.0,									// vertical FOV
		float(window_width)/window_height,		// aspect ratio
		1.0,									// near plane
		1000.0);								// far plane
	camera.viewParameters(
		Vector3D(10.0f,5.0f,-10.0f),	// eye point
		Vector3D(0.0f,0.0f,0.0f),		// lookat point
		Vector3D(0.0f,1.0f,0.0f));		// up vector
	
	// initial position
	setCameraLeft();
}

void AppCamera::bumpCamera()
{
	// bump the camera to force update (a bit of a hack)
	camera.move(0.001f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f);
}

void AppCamera::setCameraLeft()
{
	// looking down x-axis - from character's left
	camera.moveTo(Vector3D(100.0f,10.0f,0.0));
	camera.rotateTo(Vector3D(0.0f,1.5f*PI,0.0f));
	bumpCamera();
}

void AppCamera::setCameraFront()
{
	// looking down z-axis - from character's front
	camera.moveTo(Vector3D(0.0f,10.0f,100.0));
	camera.rotateTo(Vector3D(0.0f,1.0f*PI,0.0f));
	bumpCamera();
}

void AppCamera::setCameraFrontLeft()
{
	// looking from charcater's front left
	camera.moveTo(Vector3D(100.0f,10.0f,100.0));
	camera.rotateTo(Vector3D(0.0f,1.25f*PI,0.0f));
	bumpCamera();
}
