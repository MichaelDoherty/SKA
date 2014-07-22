//-----------------------------------------------------------------------------
// DataIndex.h
//	 Loads additional skeleton and motion information from a textfile.
//   That information is index by subject and motion IDs.
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
#ifndef DATAINDEX_DOT_H
#define DATAINDEX_DOT_H
#include "Core/SystemConfiguration.h"
#include <string>
#include <vector>
using namespace std;

// DataIndex class holds additional data about motions.
// Primarily used to hold descriptive information about CMU ASF/AMC files,
//  since those files do not hold descriptions internally.
// Data is read from a text file, which was initialized with data from CMU's website.

class DataIndex 
{
public:
	DataIndex();
	virtual ~DataIndex();

	void loadASFAMCDescriptions(const char* index_file);

	bool getASFData(const string& ASF_number, string& description, string& filename);
	bool getAMCData(const string& ASF_number, const string& AMC_number,
		string& description, string& framerate, string& filename);

	int getAllIndexedMotions(vector<pair <string, string> >& motions);
};

#endif
