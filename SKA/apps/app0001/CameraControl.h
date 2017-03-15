//-----------------------------------------------------------------------------
// app0001 - Builds with SKA Version 4.0
//-----------------------------------------------------------------------------
// CameraControl.h
//    Object to control the camera/viewpoint.
//-----------------------------------------------------------------------------
#ifndef CAMERACONTROL_DOT_H
#define CAMERACONTROL_DOT_H
// SKA configuration
#include <Core/SystemConfiguration.h>
// SKA modules
#include <Camera/Camera.h>

class AppCamera : public MovingCamera
{
public:
	AppCamera();
	~AppCamera();
	// setup function, which should be called once before entering openGL loop.
	void initializeCamera(int window_width, int window_height);
	// move camera to predefine locations
	void setCameraLeft();
	void setCameraFront();
	void setCameraFrontLeft();
	// move camera slightly to force it to update position
	void bumpCamera(); 
};

// global single instance of the camera
extern AppCamera camera;

#endif // CAMERACONTROL_DOT_H
