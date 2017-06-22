//-----------------------------------------------------------------------------
// MotionDescriptors project - Builds with SKA Version 4.0
//-----------------------------------------------------------------------------
// ProcessControl.cpp
// Authors:    Michael Doherty and Trevor Martin - April 2017
//-----------------------------------------------------------------------------
// SKA configuration
#include <Core/SystemConfiguration.h>
// C++ libraries
#include <iostream>
using namespace std;
// SKA modules
#include <Math/Vector3D.h>
#include <DataManagement/FileSystem.h>
// local application
#include "ProcessControl.h"

ProcessControl process_control;
#ifdef _WIN32
const bool SLASH_PATHS = false;
#else
const bool SLASH_PATHS = true;
#endif

/*
command file format:
INPUT_FOLDER=<path>
OUTPUT_FOLDER=<path>
ANIMATION=<Y|N>
PROCESS <file> MODE=<F|E|A> LOOP=<Y|N> SKIP=<number>
*/

static bool isPrefix(string& s, const string& prefix) {
	return s.find(prefix) == 0;
}

// Find the first pair of double quotes at or after string position p
// and return the substring inside those quotes.
static string extractQuotedString(const string& line, int p=0)
{
	string s = line.substr(p);
	int quote1 = s.find_first_of('\"');
	int quote2 = s.find_first_of('\"', quote1 + 1);
	string t = s.substr(quote1 + 1, (quote2 - quote1) - 1);
	return t;
}

bool ProcessControl::readCommandFile()
{
	string input_folder("");
	string output_folder("");
	string line;
	ifstream cf("skacommands.txt");
	if (!cf) return false;

	while (getline(cf, line))
	{
		if (line.length() < 1) continue;

		if (isPrefix(line, string("INPUT_FOLDER=")))
		{
			if (SLASH_PATHS) {
				input_folder = line.substr(14, line.length() - 15);
				FileSystem::forwardslashFilepath(input_folder);
				if (input_folder[input_folder.length() - 1] != '/') input_folder.append("/");
			}
			else {
				input_folder = line.substr(14, line.length() - 15);
				FileSystem::backslashFilepath(input_folder);
				if (input_folder[input_folder.length() - 1] != '\\') input_folder.append("\\");
			}
		}
		else if (isPrefix(line, string("OUTPUT_FOLDER=")))
		{
			if (SLASH_PATHS) {
				output_folder = line.substr(15, line.length() - 16);
				FileSystem::forwardslashFilepath(output_folder);
				if (output_folder[output_folder.length() - 1] != '/') output_folder.append("/");
			}
			else {
				output_folder = line.substr(15, line.length() - 16);
				FileSystem::backslashFilepath(output_folder);
				if (output_folder[output_folder.length() - 1] != '\\') output_folder.append("\\");
			}
		}
		else if (isPrefix(line, string("ANIMATION=")))
		{
			if (line[10] == 'Y') process_control.enableAnimation();
			else if (line[10] == 'N') process_control.disableAnimation();
		}
		else if (isPrefix(line, string("REALTIME=")))
		{
			if (line[9] == 'Y') process_control.setRealTimeMode();
			else if (line[9] == 'N') process_control.clearRealTimeMode();
		}
		else if (isPrefix(line, string("PROCESS")))
		{
			string skel_name;
			string motion_name;
			MOCAPTYPE mctype = UNK;
			char mode = 'N';
			char loop = 'N';
			char analysis = 'N';
			float skip = 5.0f;
			short fps = 120;
			unsigned int i;

			i = line.find("BVH=");
			if (i != string::npos) {
				motion_name = extractQuotedString(line, i + 4);
				mctype = BVH;
			}
			if (mctype == UNK) {
				i = line.find("AMC=");
				if (i != string::npos) {
					motion_name = extractQuotedString(line, i + 4);
					mctype = AMC;
				}
				i = line.find("ASF=");
				if (i != string::npos) {
					skel_name = extractQuotedString(line, i + 4);
				}
			}

			// support older command format by assuming first quoted 
			// string is a BVH fil name, if no explicit BVH or AMC command was detected.
			if (mctype == UNK) {
				motion_name = extractQuotedString(line);
			}

			i = line.find("SM=");
			if (i != string::npos) mode = line[i + 3];
			i = line.find("LOOP=");
			if (i != string::npos) loop = line[i + 5];
			i = line.find("MA=");
			if (i != string::npos) analysis = line[i + 3];
			i = line.find("SKIP=");
			if (i != string::npos) {
				string stmp = line.substr(i + 5);
				skip = float(atof(stmp.c_str()));
			}
			i = line.find("FPS=");
			if (i != string::npos) {
				string stmp = line.substr(i + 4);
				fps = atoi(stmp.c_str());
			}

			SHOULDERMODE dm = NONE;
			switch (mode) {
			case 'N': dm = NONE; break;
			case 'F': dm = FLEXION; break;
			case 'E': dm = EXTENSION; break;
			case 'A': dm = ABDUCTION; break;
			};
			bool loop_on = false;
			if (loop == 'Y') loop_on = true;
			bool analysis_on = false;
			if (analysis == 'Y') analysis_on = true;

			if (mctype == BVH) {
				ProcessingRequest preq(motion_name,
					BVH, input_folder, output_folder, 
					input_folder + motion_name + ".bvh", string(""),
					output_folder + motion_name + ".csv",
					analysis_on, dm, loop_on, skip, fps);
				addRequest(preq);
			}
			else if (mctype == AMC) {
				ProcessingRequest preq(motion_name,
					AMC, input_folder, output_folder, 
					input_folder + motion_name + ".amc", input_folder + skel_name + ".asf",
					output_folder + motion_name + ".csv",
					analysis_on, dm, loop_on, skip, fps);
				addRequest(preq);
			}
		}
		else
		{
			cerr << "INVALID COMMAND LINE: " << line << endl;
		}
	}
	cf.close();
	cout << *this << endl;
	return true;
}
