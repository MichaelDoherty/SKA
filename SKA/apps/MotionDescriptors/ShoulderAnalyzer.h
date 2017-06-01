//-----------------------------------------------------------------------------
// MotionDescriptors project - Builds with SKA Version 4.0
//-----------------------------------------------------------------------------
// ShoulderAnalyzer.h
// Author: Michael Doherty - Fall 2016 - Spring 2017
//   Originally developed to support Laurel/Davenport physical therapy project.
//-----------------------------------------------------------------------------

#ifndef SHOULDERANALYZER_DOT_H
#define SHOULDERANALYZER_DOT_H
// SKA modules
#include <Core/SystemConfiguration.h>

class ShoulderAnalyzer {
public:
	ShoulderAnalyzer() {}
	void processPTData();
	void resetMaxValues();
	void storeResults();
};

#endif // SHOULDERANALYZER_DOT_H