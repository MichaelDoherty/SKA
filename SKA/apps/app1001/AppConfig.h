//-----------------------------------------------------------------------------
// app0002 - Builds with SKA Version 3.1 - Sept 01, 2012 - Michael Doherty
//-----------------------------------------------------------------------------
// AppConfig.h
//    Define application specific things, such as file locations.
//-----------------------------------------------------------------------------
#ifndef APPCONFIG_DOT_H
#define APPCONFIG_DOT_H
// SKA configuration
#include <Core/SystemConfiguration.h>

// root path to the BVH files
#define BVH_MOTION_FILE_PATH "../../data/motion/BVH"
// textures are BMP files that are used to color some objects (such as the sky)
#define TEXTURE_FILE_PATH "../../data/textures"
//files that we are going to be loading
#define BVH_MOTION_FILE_PATHMOTIONS "../../data/motion/BVH/Baseball_Swings"
//directory of where we are goign to store converted files. 
#define QUAT_BVH_MOTION_FILE_PATHMOTIONS "../../data/motion/BVH/Baseball_Swings/QuatConverted"

#endif // APPCONFIG_DOT_H
