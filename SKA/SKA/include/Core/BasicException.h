//-----------------------------------------------------------------------------
// BasicException.h
//	 Base class for all SKA exceptions.
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

#ifndef BASICEXCEPTION_DOT_H
#define BASICEXCEPTION_DOT_H
#include <Core/SystemConfiguration.h>
#include <cstring>

class SKA_LIB_DECLSPEC BasicException
{
public:
	BasicException() : msg(NULL) { storeMessage("Unspecified exception"); }
	BasicException(const char* _msg) : msg(NULL) { storeMessage(_msg); }
	BasicException(const BasicException& _other) : msg(NULL) { storeMessage(_other.msg); }
	virtual ~BasicException() { if (msg != NULL) delete [] msg; }
	char* msg;
private:
	void storeMessage(const char* _msg) 
	{
		if (msg != NULL) delete [] msg;
		msg = new char[strlen(_msg)+1];
		strcpy(msg, _msg);
	}
};

#endif
