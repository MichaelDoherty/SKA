//-----------------------------------------------------------------------------
// ASF_AMC_ParseUtils.cpp
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
#include "Core/SystemConfiguration.h"
#include <cstdlib>
#include "DataManagement/ASF_AMC_ParseUtils.h"

bool AAPU::linePrefix(string& line, const string& prefix)
{
	unsigned int i=0, j=0;
	while ((i<line.length()) && iswhitespace(line[i])) i++;
	if (i>=line.length()) return false;

	while (j < prefix.length())
	{
		if (i>=line.length()) return false;
		if (line[i++] != prefix[j++]) return false;
	}
	return true;
}

bool AAPU::stripPrefix(string& line, const string& prefix)
{
	unsigned int i=0, j=0;
	while ((i<line.length()) && iswhitespace(line[i])) i++;
	if (i>=line.length()) return false;
	while (j < prefix.length()) 
	{
		if (line[i++] != prefix[j++]) return false;
		if (i>=line.length()) return false;
	}
	while ((i<line.length()) && iswhitespace(line[i])) i++;

	string rest;
	while (i < line.length())
		rest += line[i++];
	line = rest;
	return true;
}

bool AAPU::splitLine(string& line, string& prefix, string& rest)
{
	prefix = "";
	rest = "";
	unsigned int i=0;
	while ((i<line.length()) && iswhitespace(line[i])) i++;
	while ((i<line.length()) && !iswhitespace(line[i])) prefix += line[i++];
	while ((i<line.length()) && iswhitespace(line[i])) i++;
	while (i<line.length()) rest += line[i++];  
	return true;
}

bool AAPU::parseFloats(string& line, list<float>& values)
{
	values.clear();
	unsigned int i=0;
	char s[100];
	while (true)
	{
		while ((i<line.length()) && iswhitespace(line[i])) i++;
		if (i>=line.length()) return true;
		unsigned int j=0;
		while ((i<line.length()) && (j<99) && !iswhitespace(line[i])) 
			s[j++] = line[i++];
		s[j] = '\0';
		float x = (float)atof(s);
		values.push_back(x);
	}
}

bool AAPU::parseInts(string& line, list<long>& values)
{
	values.clear();
	unsigned int i=0;
	char s[100];
	while (true)
	{
		while ((i<line.length()) && iswhitespace(line[i])) i++;
		if (i>=line.length()) return true;
		unsigned int j=0;
		while ((i<line.length()) && (j<99) && !iswhitespace(line[i])) 
			s[j++] = line[i++];
		s[j] = '\0';
		long x = atoi(s);
		values.push_back(x);
	}
}

bool AAPU::parseWords(string& line, list<string>& words)
{
	words.clear();
	unsigned int i=0;
	string s;
	while (true)
	{
		while ((i<line.length()) && iswhitespace(line[i])) i++;
		if (i>=line.length()) return true;
		s = "";
		while ((i<line.length()) && !iswhitespace(line[i])) 
			s += line[i++];
		words.push_back(s);
	}
}

bool AAPU::parseLimits(string& line, float& min, float& max)
{
	char b[100];
	unsigned int i=0;
	unsigned int j=0;
	while ((line[i] != '(') && (i < line.length())) i++;
	i++;
	if (i>=line.length()) return false;
	while ((i < line.length()) && !iswhitespace(line[i]))
	{
		if (i>=line.length()) return false;
		b[j++] = line[i++];
	}
	b[j] = '\0';
	min = (float)atof(b);
	while ((i < line.length()) && iswhitespace(line[i])) i++;
	if (i>=line.length()) return false;
	j=0;
	while ((i < line.length()) && (line[i] != ')')) 
	{
		if (i>=line.length()) return false;
		b[j++] = line[i++];
	}
	b[j] = '\0';
	max = (float)atof(b);
	return true;
}

bool AAPU::lineIsSingleInt(string& line, int& num)
{
	unsigned int i=0;
	while ((i < line.length()) && iswhitespace(line[i])) i++;
	if (i>=line.length()) return false;
	while ((i < line.length()) && isdigit(line[i])) i++;
	while ((i < line.length()) && iswhitespace(line[i])) i++;
	if (i < line.length()) return false;
	num = atoi(line.c_str());
	return true;
}

bool AAPU::lineIsEmpty(string& line)
{
	unsigned int i=0;
	while ((i < line.length()) && iswhitespace(line[i])) i++;
	if (i<line.length()) return false;
	return true;
}
