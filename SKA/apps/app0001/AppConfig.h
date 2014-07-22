//-----------------------------------------------------------------------------
// app0001: Demo program illustrating how to build a basic SKA application.
//          This application reads a skeleton and motion from an ASF/AMC 
//          file pair and uses that data to drive a character.
//-----------------------------------------------------------------------------
// AppConfig.h
//    Define application specific things, such as file locations.
//-----------------------------------------------------------------------------
// Builds with SKA Version 3.0 - July 22, 2012 - Michael Doherty
//-----------------------------------------------------------------------------
#ifndef APPCONFIG_DOT_H
#define APPCONFIG_DOT_H
// SKA configuration - should always be the first file included.
#include <Core/SystemConfiguration.h>

// root path to the ASF/AMC files
#define AMC_MOTION_FILE_PATH "../../data/motion/AMC"

#endif // APPCONFIG_DOT_H
