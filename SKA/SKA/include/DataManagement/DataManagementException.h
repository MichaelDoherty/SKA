//-----------------------------------------------------------------------------
// DataManagementException.h
//	 Exception thrown from within DataManagement module.
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
// Version 3.0 - July 18, 2014 - Michael Doherty
//-----------------------------------------------------------------------------
#ifndef DATA_MANAGEMENT_EXCEPTION_DOT_H
#define DATA_MANAGEMENT_EXCEPTION_DOT_H
#include "Core/SystemConfiguration.h"
#include "Core/BasicException.h"

class DataManagementException : public BasicException
{
public:
	DataManagementException() : BasicException(string("DataManagement exception")) { }
	DataManagementException(const string& _msg) : BasicException(_msg) { }
};

#endif
