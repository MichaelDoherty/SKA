//-----------------------------------------------------------------------------
// ParsingUtilities.h
//	 Utility functions for parsing text files
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

#ifndef PARSINGUTILITIES_DOT_H
#define PARSINGUTILITIES_DOT_H
#include <Core/SystemConfiguration.h>
#include <fstream>
#include <list>
#include <string>
using namespace std;
#include <Core/BasicException.h>

class ParsingUtilities
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

class LineScanner
{
private:
	bool pushback_full;
	string pushback_buffer;
	bool file_open;
	ifstream ifs;
public:
	LineScanner(const char* filename)
	{
		pushback_full = false;
		file_open = false;
		ifs.open(filename);
		if (ifs) file_open = true;
	}

	~LineScanner()
	{
		if (file_open) ifs.close();
	}

	void close()
	{
		if (file_open) ifs.close();
		file_open = false;
		pushback_full = false;
	}

	bool getNextLine(string& s)
	{
		if (pushback_full)
		{
			s = pushback_buffer;
			pushback_full = false;
			return true;
		}
		getline(ifs, s);
		if (!ifs) return false;
		return true;
	}

	bool pushbackLine(string& s)
	{
		if (pushback_full) return false;
		pushback_buffer = s;
		pushback_full = true;
		return true;
	}

	bool fileIsOpen() { return file_open; }
};

#endif
