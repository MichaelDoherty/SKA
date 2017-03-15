//-----------------------------------------------------------------------------
// GraphicsInterface.h
//	 Implements functions dependent on OpenGL.
//   Most methods defined here are for classes defined elsewhere.
//   The intention is to isolate the OpenGL dependencies.
//-----------------------------------------------------------------------------
// This software is part of the Skeleton Animation Toolkit (SKA) developed 
// at the University of the Pacific, under the guidance of Michael Doherty.
// For information please contact mdoherty@pacific.edu.
//-----------------------------------------------------------------------------
// This is open software. You are free to use it as you see fit.
// The University of the Pacific and identified authors would appreciate
// being credited for any significant use, particularly if used for
// commercial projects or academic research publications.
//-----------------------------------------------------------------------------

#ifndef GRAPHICSINTERFACE_DOT_H
#define GRAPHICSINTERFACE_DOT_H
#include <Core/SystemConfiguration.h>

SKA_LIB_DECLSPEC extern float time_fade_factor;

SKA_LIB_DECLSPEC void checkOpenGLError(int placecode);

#endif
