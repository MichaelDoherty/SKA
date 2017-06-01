//-----------------------------------------------------------------------------
// MotionDescriptors project - Builds with SKA Version 4.0
//-----------------------------------------------------------------------------
// ProcessControl.cpp
// Authors:    Michael Doherty and Trevor Martin - April 2017
//-----------------------------------------------------------------------------
// SKA configuration
#include <Core/SystemConfiguration.h>
// SKA modules
#include <Math/Vector3D.h>
// local application
#include "ProcessControl.h"

ProcessControl process_control;

/*
command file format:
INPUT_FOLDER=<path>
OUTPUT_FOLDER=<path>
ANIMATION=<Y|N>
PROCESS <file> MODE=<F|E|A> LOOP=<Y|N> SKIP=<number>
*/

bool isPrefix(string& s, string& prefix) {
	return s.find(prefix) == 0;
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
			input_folder = line.substr(14, line.length() - 15);
			if (input_folder[input_folder.length() - 1] != '\\') input_folder.append("\\");
		}
		else if (isPrefix(line, string("OUTPUT_FOLDER=")))
		{
			output_folder = line.substr(15, line.length() - 16);
			if (output_folder[output_folder.length() - 1] != '\\') output_folder.append("\\");
		}
		else if (isPrefix(line, string("ANIMATION=")))
		{
			if (line[10] == 'Y') process_control.enableAnimation();
			else if (line[10] == 'N') process_control.disableAnimation();
		}
		/*Trevor's code for tracking bone enabling*/
		else if (isPrefix(line, string("TRACK_BONE=")))
		{
			if (line[11] == 'Y') process_control.enableBoneTracking();
			else if (line[11] == 'N') process_control.disableBoneTracking();
		}
		else if (isPrefix(line, string("BONE_NAME="))) {
			//find bone name
			line = line.substr(9);
			int quote1 = line.find_first_of('\"');
			int quote2 = line.find_first_of('\"', quote1 + 1);
			string bone = line.substr(quote1 + 1, (quote2 - quote1) - 1);
			process_control.setBoneName(bone);
		}
		else if (isPrefix(line, string("PLANE="))) {
			//find plane name
			line = line.substr(6);
			int quote1 = line.find_first_of('\"');
			int quote2 = line.find_first_of('\"', quote1 + 1);
			string plane_string = line.substr(quote1 + 1, (quote2 - quote1) - 1);
			process_control.setPlaneName(plane_string);
		}
		else if (isPrefix(line, string("MOTION_SKIP="))) {
			line = line.substr(6);
			int quote1 = line.find_first_of('\"');
			int quote2 = line.find_first_of('\"', quote1 + 1);
			string frame_string = line.substr(quote1 + 1, (quote2 - quote1) - 1);
			int frames = atoi(frame_string.c_str());
			if (frames == 0)
				process_control.setFrameStep(1);
			else
				process_control.setFrameStep(frames);
			//cout << frames;
		}
		/*End Trevor's code*/
		else if (isPrefix(line, string("PROCESS")))
		{
			line = line.substr(7);
			int quote1 = line.find_first_of('\"');
			int quote2 = line.find_first_of('\"', quote1 + 1);
			string filename = line.substr(quote1 + 1, (quote2 - quote1) - 1);
			line = line.substr(quote2 + 1);

			char mode = 'E';
			char loop = 'N';
			float skip = 5.0f;

			int i = line.find("MODE=");
			if (i != string::npos) mode = line[i + 5];
			i = line.find("LOOP=");
			if (i != string::npos) loop = line[i + 5];
			i = line.find("SKIP=");
			if (i != string::npos) {
				string stmp = line.substr(i + 5);
				skip = float(atof(stmp.c_str()));
			}

			DATAMODE dm = EXTENSION;
			switch (mode) {
			case 'F': dm = FLEXION; break;
			case 'E': dm = EXTENSION; break;
			case 'A': dm = ABDUCTION; break;
			};
			bool loop_on = false;
			if (loop == 'Y') loop_on = true;

			ProcessingRequest preq(filename,
				input_folder + filename + ".bvh", output_folder + filename + ".csv",
				dm, loop_on, skip);
			process_control.addRequest(preq);
		}
		else
		{
			cerr << "INVALID COMMAND LINE: " << line << endl;
		}
	}
	cf.close();
	return true;
}
