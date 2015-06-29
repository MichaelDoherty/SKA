//-----------------------------------------------------------------------------
// app1001 - Builds with SKA Version 3.1 - Sept 01, 2012 - Michael Doherty
//-----------------------------------------------------------------------------
// AppConfig.h
//    Define application specific things, such as file locations.
//-----------------------------------------------------------------------------
#ifndef APPCONFIG_DOT_H
#define APPCONFIG_DOT_H
// SKA configuration
#include <Core/SystemConfiguration.h>
#include <Core/BasicException.h>

// root path to the BVH files
#define BVH_MOTION_FILE_PATH "../../data/motion/BVH/Baseball_Swings"
// textures are BMP files that are used to color some objects (such as the sky)
#define TEXTURE_FILE_PATH "../../data/textures"

class AppException : public BasicException
{
public:
	AppException() : BasicException("Unspecified Application exception") { }
	AppException(const char* _msg) : BasicException(_msg) { }
	AppException(const AppException& _other) : BasicException(_other) { }
};

#endif // APPCONFIG_DOT_H
