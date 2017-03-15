//-----------------------------------------------------------------------------
// Graphics2D.h
//	 Functions for 2D graphics (currently string rendering).
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

#ifndef GRAPHICS2D_DOT_H
#define GRAPHICS2D_DOT_H
#include <Core/SystemConfiguration.h>
#include <Models/Color.h>

SKA_LIB_DECLSPEC void renderBitmapString(float x, float y, float z, void *font, Color color, const char *string);

SKA_LIB_DECLSPEC void renderString(float x, float y, float z, Color color, const char* string);

#endif