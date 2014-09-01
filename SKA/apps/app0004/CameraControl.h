//-----------------------------------------------------------------------------
// app0004 - Builds with SKA Version 3.1 - Sept 01, 2012 - Michael Doherty
//-----------------------------------------------------------------------------
// CameraControl.h
//-----------------------------------------------------------------------------
#ifndef CAMERACONTROL_DOT_H
#define CAMERACONTROL_DOT_H
#include "Core/SystemConfiguration.h"
#include "Camera/Camera.h"

void initializeCamera(int window_width, int window_height);
void setCameraLeft();
void setCameraFront();
void setCameraFrontLeft();

extern  MovingCamera camera;

#endif // CAMERACONTROL_DOT_H
