//-----------------------------------------------------------------------------
// LineScanner.h
//   Class to read text files one line at a time, with a single pushback store.
//   Lines are returned as strings.
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
#ifndef LINESCANNER_DOT_H
#define LINESCANNER_DOT_H
#include "Core/SystemConfiguration.h"
#include <fstream>
#include <string>
using namespace std;

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