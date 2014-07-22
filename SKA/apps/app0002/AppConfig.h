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
// AppConfig.h
//    Define application specific things, such as file locations.
//-----------------------------------------------------------------------------
// Builds with SKA Version 3.0 - July 22, 2012 - Michael Doherty
//-----------------------------------------------------------------------------
#ifndef APPCONFIG_DOT_H
#define APPCONFIG_DOT_H
// SKA configuration
#include <Core/SystemConfiguration.h>

// root path to the ASF/AMC files
#define AMC_MOTION_FILE_PATH "../../data/motion/AMC"
// textures are BMP files that are used to color some objects (such as the sky)
#define TEXTURE_FILE_PATH "../../data/textures"

#endif // APPCONFIG_DOT_H
