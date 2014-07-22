//-----------------------------------------------------------------------------
// app0001: Demo program illustrating how to build a basic SKA application.
//          This application reads a skeleton and motion from an ASF/AMC 
//          file pair and uses that data to drive a character.
//-----------------------------------------------------------------------------
// CameraControl.h
//    Object to control the camera/viewpoint.
//-----------------------------------------------------------------------------
// Builds with SKA Version 3.0 - July 22, 2012 - Michael Doherty
//-----------------------------------------------------------------------------
// SKA configuration - should always be the first file included.
#include <Core/SystemConfiguration.h>
// SKA modules
#include <Math/Vector3D.h>
// local application
#include "CameraControl.h"

// global single instance of the camera
MovingCamera camera;

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
	
	// looking down x-axis - from character's left
	camera.moveTo(Vector3D(100.0f,10.0f,0.0));
	camera.rotateTo(Vector3D(0.0f,1.5f*PI,0.0f));
	// bump the camera to force it to update itself (a bit of a hack)
	camera.move(0.001f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f);
}
