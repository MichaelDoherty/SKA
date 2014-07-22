//-----------------------------------------------------------------------------
// ASF_AMC_ParseUtils.h
//	 Parsing functions used by ASF_Reader and AMC_Reader
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
#ifndef ASF_AMC_PARSEUTILS_DOT_H
#define ASF_AMC_PARSEUTILS_DOT_H
#include "Core/SystemConfiguration.h"
#include <list>
#include <string>
using namespace std;
#include "Core/BasicException.h"

class AAPU
{
public:
	static bool linePrefix(string& line, const string& prefix);
	static bool stripPrefix(string& line, const string& prefix);
	static bool splitLine(string& line, string& prefix, string& rest);
	static bool parseFloats(string& s, list<float>& values);
	static bool parseInts(string& s, list<long>& values);
	static bool parseWords(string& line, list<string>& words);
	static bool parseLimits(string& line, float& min, float& max);
	static bool iswhitespace(char c) { return ((c==' ')||(c=='\t')||(c=='\r')); }
	static bool lineIsSingleInt(string& line, int& num);
	static bool lineIsEmpty(string& line);
};

#endif
