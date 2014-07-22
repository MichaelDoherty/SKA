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
#ifndef CAMERACONTROL_DOT_H
#define CAMERACONTROL_DOT_H
// SKA configuration - should always be the first file included.
#include <Core/SystemConfiguration.h>
// SKA modules
#include <Camera/Camera.h>

// Setup function, which should be called once before entering openGL loop.
void initializeCamera(int window_width, int window_height);

// global single instance of the camera
extern MovingCamera camera;

#endif // CAMERACONTROL_DOT_H
