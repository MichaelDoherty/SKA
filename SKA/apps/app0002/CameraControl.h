//-----------------------------------------------------------------------------
// app0002: Demo program illustrating various useful things that aren't
//          directly related to the character animation.
//          (1) additional objects, such as ground, sky and coordinate axes
//          (2) moveable camera, controlled by the camera and mouse.
//          (3) keyboard filtering, to avoid multiple responses when
//              a single keystroke is expected.
//          (4) animation speed control (freeze, single step, time warp)
//          (5) heads-up display (2D text on screen)
//-----------------------------------------------------------------------------
// CameraControl.h
//    Object to control the camera/viewpoint.
//-----------------------------------------------------------------------------
// Builds with SKA Version 3.0 - July 22, 2012 - Michael Doherty
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
