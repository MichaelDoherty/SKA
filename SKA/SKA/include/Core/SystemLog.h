//-----------------------------------------------------------------------------
// SystemLog.h
//   Global output stream for logging system events/data.
//   Connected to file "system_log.txt".
//-----------------------------------------------------------------------------
// This software is part of the Skeleton Animation Toolkit (SKA) developed by 
// at the University of the Pacific, under the guidance of Michael Doherty.
// For information please contact mdoherty@pacific.edu.
//-----------------------------------------------------------------------------
// This is open software. You are free to use it as you see fit.
// The University of the Pacific and identified authors should be
// credited for any significant use, particularly if used for commercial 
// projects or academic research publications.
//-----------------------------------------------------------------------------
// Version 1.0 - January 25, 2012 - Michael Doherty
//-----------------------------------------------------------------------------
#ifndef SYSTEMLOG_DOT_H
#define SYSTEMLOG_DOT_H
#include "Core/SystemConfiguration.h"
#include <fstream>
using namespace std;

SKA_LIB_DECLSPEC extern ofstream logout;

#endif
