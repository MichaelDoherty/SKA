//-----------------------------------------------------------------------------
// NetworkProcessing project - Builds with SKA Version 4.0
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
	setCameraPreset(0);
}

void AppCamera::bumpCamera()
{
	// bump the camera to force update (a bit of a hack)
	camera.move(0.001f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f);
}

void AppCamera::setCameraPreset(short preset)
{
	preset = preset % 8;
	switch (preset)
	{
	case 0:
		// looking down z-axis - from character's front
		camera.moveTo(Vector3D(0.0f, 10.0f, 100.0));
		camera.rotateTo(Vector3D(0.0f, 1.0f*PI, 0.0f));
		bumpCamera();
		break;
	case 1:
		// looking from character's front left
		camera.moveTo(Vector3D(70.0f, 10.0f, 70.0));
		camera.rotateTo(Vector3D(0.0f, 1.25f*PI, 0.0f));
		bumpCamera();
		break;
	case 2:
		// looking down x-axis - from character's left
		camera.moveTo(Vector3D(100.0f, 10.0f, 0.0));
		camera.rotateTo(Vector3D(0.0f, 1.5f*PI, 0.0f));
		bumpCamera();
		break;
	case 3:
		// looking from character's back left
		camera.moveTo(Vector3D(70.0f, 10.0f, -70.0));
		camera.rotateTo(Vector3D(0.0f, 1.75f*PI, 0.0f));
		bumpCamera();
		break;
	case 4:
		// looking up z-axis - from character's back
		camera.moveTo(Vector3D(0.0f, 10.0f, -100.0));
		camera.rotateTo(Vector3D(0.0f, 0.0f, 0.0f));
		bumpCamera();
		break;
	case 5:
		// looking from character's back right
		camera.moveTo(Vector3D(-70.0f, 10.0f, -70.0));
		camera.rotateTo(Vector3D(0.0f, 0.25f*PI, 0.0f));
		bumpCamera();
		break;
	case 6:
		// looking up x-axis - from character's right
		camera.moveTo(Vector3D(-100.0f, 10.0f, 0.0));
		camera.rotateTo(Vector3D(0.0f, 0.5f*PI, 0.0f));
		bumpCamera();
		break;
	case 7:
		// looking from character's front right
		camera.moveTo(Vector3D(-70.0f, 10.0f, 70.0));
		camera.rotateTo(Vector3D(0.0f, 0.75f*PI, 0.0f));
		bumpCamera();
		break;
	}
}