//-----------------------------------------------------------------------------
// ASF_Reader.h
//	 Reads Acclaim Skeleton File (ASF)
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
#ifndef ASF_READER_DOT_H
#define ASF_READER_DOT_H
#include "Core/SystemConfiguration.h"
#include "DataManagement/LineScanner.h"
#include "DataManagement/ASF_AMC_ParseUtils.h"
#include "Animation/SkeletonDefinition.h"

class ASF_Reader
{
public:
	ASF_Reader() : skeleton(NULL), ASF_angles_are_degrees(true) { }
	SkeletonDefinition* readASF(const char* inputFilename);
private:
	SkeletonDefinition* skeleton;
	bool ASF_angles_are_degrees;

	bool processComment(LineScanner& line_scanner, string& line);
	bool processVersionSection(LineScanner& line_scanner, string& line);
	bool processNameSection(LineScanner& line_scanner, string& line);
	bool processUnitsSection(LineScanner& line_scanner, string& line);
	bool processDocumentationSection(LineScanner& line_scanner, string& line);
	bool processRootSection(LineScanner& line_scanner, string& line);
	bool processBonedataSection(LineScanner& line_scanner, string& line);
	bool processHierarchySection(LineScanner& line_scanner, string& line);

	bool processBone(LineScanner& line_scanner);
};

#endif