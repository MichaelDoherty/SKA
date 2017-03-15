//-----------------------------------------------------------------------------
// AnimationException.h
//    Exception thrown from within the Animation module.
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

#ifndef ANIMATIONEXCEPTION_DOT_H
#define ANIMATIONEXCEPTION_DOT_H
#include <Core/SystemConfiguration.h>
#include <Core/BasicException.h>

class SKA_LIB_DECLSPEC AnimationException : public BasicException
{
public:
	AnimationException() : BasicException("Unspecified Animation exception") { }
	AnimationException(const char* _msg) : BasicException(_msg) { }
	AnimationException(const AnimationException& _other) : BasicException(_other) { }
};

#endif
