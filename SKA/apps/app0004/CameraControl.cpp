//-----------------------------------------------------------------------------
// app0004 - Builds with SKA Version 3.1 - Sept 01, 2012 - Michael Doherty
//-----------------------------------------------------------------------------
// CameraControl.cpp
//-----------------------------------------------------------------------------
#include "Core/SystemConfiguration.h"
#include "Math/Vector3D.h"
#include "CameraControl.h"

MovingCamera camera;

void bumpCamera()
{
	// bump the camera to force update (a bit of a hack)
	camera.move(0.001f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f);
}

void setCameraLeft()
{
	// looking down x-axis - from character's left
	camera.moveTo(Vector3D(50.0f,10.0f,0.0));
	camera.rotateTo(Vector3D(0.0f,1.5f*PI,0.0f));
	bumpCamera();
}

void setCameraFront()
{
	// looking down z-axis - from character's front
	camera.moveTo(Vector3D(0.0f,10.0f,50.0));
	camera.rotateTo(Vector3D(0.0f,1.0f*PI,0.0f));
	bumpCamera();
}

void setCameraFrontLeft()
{
	// looking from charcater's front left
	camera.moveTo(Vector3D(50.0f,10.0f,50.0));
	camera.rotateTo(Vector3D(0.0f,1.25f*PI,0.0f));
	bumpCamera();
}

void initializeCamera(int window_width, int window_height)
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
	setCameraLeft();
}
