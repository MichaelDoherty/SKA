//-----------------------------------------------------------------------------
// ASF_Reader.cpp
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
#include "Core/SystemConfiguration.h"
#include "DataManagement/DataManagementException.h"
#include "DataManagement/ASF_Reader.h"
#include <fstream>
#include <cstdlib>
using namespace std;

static const int LOGBUFF_SIZE = 1000;
static char LOGBUFF[LOGBUFF_SIZE];

static void DOF_StringToIndexes(string& dof_s, DOF_ID dof[3])
{
	// string is "XYZ", "ZYX", ...
	// represents order of axis values (and application order)
	for (short d=0; d<3; d++)
	{
		switch (dof_s[d])
		{
		case 'X': case 'x': dof[d] = DOF_PITCH; break;
		case 'Y': case 'y': dof[d] = DOF_YAW; break;
		case 'Z': case 'z': dof[d] = DOF_ROLL; break;
		}
	}
}

// =============================================================
// readASF

SkeletonDefinition* ASF_Reader::readASF(const char* inputFilename)
{
	LineScanner line_scanner(inputFilename);
	if (!line_scanner.fileIsOpen()) return NULL;

	skeleton = new SkeletonDefinition(string(inputFilename));
	string line;
	
	while (line_scanner.getNextLine(line))
	{
		if (AAPU::linePrefix(line, string("#")))
			continue;
		else if (AAPU::linePrefix(line, string(":version")))
			processVersionSection(line_scanner, line);
		else if (AAPU::linePrefix(line, string(":name")))
			processNameSection(line_scanner, line);
		else if (AAPU::linePrefix(line, string(":units")))
			processUnitsSection(line_scanner, line);
		else if (AAPU::linePrefix(line, string(":documentation")))
			processDocumentationSection(line_scanner, line);
		else if (AAPU::linePrefix(line, string(":root")))
			processRootSection(line_scanner, line);
		else if (AAPU::linePrefix(line, string(":bonedata")))
			processBonedataSection(line_scanner, line);
		else if (AAPU::linePrefix(line, string(":hierarchy")))
			processHierarchySection(line_scanner, line);
		else 
		{
			sprintf(LOGBUFF, "ASF_Reader BAD LINE: %s", line.c_str());
			throw DataManagementException(string(LOGBUFF));
		}
	}
	return skeleton;
}

bool ASF_Reader::processComment(LineScanner& line_scanner, string& line)
{
	if (AAPU::linePrefix(line, string("# Documentation:"))) 
	{
		AAPU::stripPrefix(line, string("# Documentation:"));
		skeleton->addDocumentation(line);
	}
	else if (AAPU::linePrefix(line, string("# Source:"))) 
	{
		AAPU::stripPrefix(line, string("# Source:"));
		skeleton->setSource(line);
	}
	return true;
}

bool ASF_Reader::processVersionSection(LineScanner& line_scanner, string& line)
{
	if (!AAPU::stripPrefix(line, string(":version"))) return false;
	// No longer stored. 
	//skeleton->setVersion(line);
	return true;
}

bool ASF_Reader::processNameSection(LineScanner& line_scanner, string& line)
{
	if (!AAPU::stripPrefix(line, string(":name"))) return false;
	//  No longer stored.
	//skeleton->setAsfName(line);
	return true;
}

bool ASF_Reader::processUnitsSection(LineScanner& line_scanner, string& line)
{
	while (true)
	{
		if (!line_scanner.getNextLine(line)) return true;
		if (AAPU::linePrefix(line, string(":")))
		{
			line_scanner.pushbackLine(line);
			return true;
		}
		string unit_name;
		string unit_value;
		AAPU::splitLine(line, unit_name, unit_value);
		skeleton->addUnits(unit_name, unit_value);
		if ((unit_name == string("angle")) && (unit_value == string("deg")))
			ASF_angles_are_degrees = true;
	}
	return true;
}

bool ASF_Reader::processDocumentationSection(LineScanner& line_scanner, string& line)
{
	while (true)
	{
		if (!line_scanner.getNextLine(line)) return true;
		if (AAPU::linePrefix(line, string(":")))
		{
			line_scanner.pushbackLine(line);
			return true;
		}
		skeleton->addDocumentation(line);
	}
	return true;
}

bool ASF_Reader::processRootSection(LineScanner& line_scanner, string& line)
{
	skeleton->setBoneName(0, string("root"));
	while (true)
	{
		if (!line_scanner.getNextLine(line)) return true;
		if (AAPU::linePrefix(line, string(":")))
		{
			line_scanner.pushbackLine(line);
			return true;
		}
		string prefix, rest;
		if (AAPU::linePrefix(line, string("order")))
		{
			DOF_ID dof_order[6];
			short num_dof = 0;
			AAPU::splitLine(line, prefix, rest);
			list<string> words;
			AAPU::parseWords(rest, words);
			list<string>::iterator iter = words.begin();
			while (iter != words.end())
			{
				string s = (*iter);
				if ((s == string("TX")) || (s == string("tx")))
					dof_order[num_dof++] = DOF_X;
				else if ((s == string("TY")) || (s == string("ty")))
					dof_order[num_dof++] = DOF_Y;
				else if ((s == string("TZ")) || (s == string("tz")))
					dof_order[num_dof++] = DOF_Z;
				else if ((s == string("RX")) || (s == string("rx")))
					dof_order[num_dof++] = DOF_PITCH;
				else if ((s == string("RY")) || (s == string("ry")))
					dof_order[num_dof++] = DOF_YAW;
				else if ((s == string("RZ")) || (s == string("rz")))
					dof_order[num_dof++] = DOF_ROLL;
				iter++;
			}
			skeleton->setBoneChannels(0, dof_order, num_dof);
		}
		else if (AAPU::linePrefix(line, string("axis")))
		{
			AAPU::splitLine(line, prefix, rest);
			float values[3] = { 0.0f, 0.0f, 0.0f };
			DOF_ID dof[3];
			DOF_StringToIndexes(rest, dof);
			skeleton->setBoneAxis(0, values, dof);
		}
		else if (AAPU::linePrefix(line, string("position")))
		{
			AAPU::splitLine(line, prefix, rest);
			list<float> values;
			AAPU::parseFloats(rest, values);
			if (values.size() != 3)
			{
				sprintf(LOGBUFF, "ASF_Reader: INVALID number of values in root position line: %s", line.c_str());
				throw DataManagementException(string(LOGBUFF));
			}
			else 
			{
				list<float>::iterator iter = values.begin();
				float x=(*iter); iter++;
				float y=(*iter); iter++;
				float z=(*iter); iter++;
				skeleton->setRootPosition(x,y,z);
			}
		}
		else if (AAPU::linePrefix(line, string("orientation")))
		{
			AAPU::splitLine(line, prefix, rest);
			list<float> values;
			AAPU::parseFloats(rest, values);
			if (values.size() != 3)
			{
				sprintf(LOGBUFF, "ASF_Reader: INVALID number of values in root orientation line: %s", line.c_str());
				throw DataManagementException(string(LOGBUFF));
			}
			else 
			{
				list<float>::iterator iter = values.begin();
				float x=(*iter); iter++;
				float y=(*iter); iter++;
				float z=(*iter); iter++;
				if (ASF_angles_are_degrees)
				{
					x = deg2rad(x);
					y = deg2rad(y);
					z = deg2rad(z);
				}
				skeleton->setRootOrientation(x,y,z);
			}
		}
	}
	return false;
}

bool ASF_Reader::processHierarchySection(LineScanner& line_scanner, string& line)
{
	while (true)
	{
		if (!line_scanner.getNextLine(line)) return true;
		if (AAPU::linePrefix(line, string(":")))
		{
			line_scanner.pushbackLine(line);
			return true;
		}
		list<string> bonenames;
		AAPU::parseWords(line, bonenames);
		if (bonenames.size() > 1)
		{
			list<string>::iterator iter = bonenames.begin();
			string parent = (*iter); iter++;
			while (iter != bonenames.end())
			{
				skeleton->addConnection(parent, (*iter));
				iter++;
			}
		}
	}
}

bool ASF_Reader::processBonedataSection(LineScanner& line_scanner, string& line)
{
	while (true)
	{
		if (!line_scanner.getNextLine(line)) return true;
		if (AAPU::linePrefix(line, string(":")))
		{
			line_scanner.pushbackLine(line);
			return true;
		}
		else if (AAPU::linePrefix(line, string("begin")))
		{
			processBone(line_scanner);
		}
		else 
		{
			sprintf(LOGBUFF, "ASF_Reader: INVALID line in Bonedata section: %s", line.c_str());
			throw DataManagementException(string(LOGBUFF));
		}
	}
}

bool ASF_Reader::processBone(LineScanner& line_scanner)
{
	string line, prefix, rest;

	short id = -1;
	short num_limits = 0;

	while (true)
	{
		if (!line_scanner.getNextLine(line))
		{
			sprintf(LOGBUFF, "ASF_Reader: EOF FOUND before END of bonedata: %s", line.c_str());
			throw DataManagementException(string(LOGBUFF));
		}
		if (AAPU::linePrefix(line, string(":")))
		{
			line_scanner.pushbackLine(line);
			sprintf(LOGBUFF, "ASF_Reader: INVALID LINE FOUND before END of bonedata: %s", line.c_str());
			throw DataManagementException(string(LOGBUFF));
		}
		else if (AAPU::linePrefix(line, string("end")))
		{
			return true;
		}
		else if (AAPU::linePrefix(line, string("id")))
		{
			AAPU::splitLine(line, prefix, rest);
			list<long> values;
			if (AAPU::parseInts(rest, values))
			{
				list<long>::iterator iter = values.begin();
				id = (short)(*iter);
			}
		}
		else if (AAPU::linePrefix(line, string("name")))
		{
			AAPU::splitLine(line, prefix, rest);
			list<string> words;
			if (AAPU::parseWords(rest, words))
			{
				list<string>::iterator iter = words.begin();
				skeleton->setBoneName(id, (*iter));
			}
		}
		else if (AAPU::linePrefix(line, string("direction")))
		{
			AAPU::splitLine(line, prefix, rest);
			list<float> values;
			AAPU::parseFloats(rest, values);
			if (values.size() != 3)
			{
				sprintf(LOGBUFF, "ASF_Reader: INVALID number of values in BONE DIRECTION line: %s", line.c_str());
				throw DataManagementException(string(LOGBUFF));
			}
			else 
			{
				list<float>::iterator iter = values.begin();
				float x=(*iter); iter++;
				float y=(*iter); iter++;
				float z=(*iter); 
				skeleton->setBoneDirection(id,x,y,z);
			}
		}
		else if (AAPU::linePrefix(line, string("length")))
		{
			AAPU::splitLine(line, prefix, rest);
			list<float> values;
			AAPU::parseFloats(rest, values);
			if (values.size() != 1)
			{
				sprintf(LOGBUFF, "ASF_Reader: INVALID number of values in BONE LENGTH line: %s", line.c_str());
				throw DataManagementException(string(LOGBUFF));
			}
			else 
			{
				list<float>::iterator iter = values.begin();
				float x=(*iter);
				skeleton->setBoneLength(id, x);
			}
		}
		else if (AAPU::linePrefix(line, string("axis")))
		{
			list<string> words;
			AAPU::parseWords(line, words);
			if (words.size() != 5)
			{
				sprintf(LOGBUFF, "ASF_Reader: INVALID number of values in BONE AXIS line: %s", line.c_str());
				throw DataManagementException(string(LOGBUFF));
			}
			else
			{
				float values[3];
				DOF_ID dof[3];

				list<string>::iterator iter = words.begin();
				iter++; // shift "axis"
				
				values[0] = (float)atof((*iter).c_str()); iter++;
				values[1] = (float)atof((*iter).c_str()); iter++;
				values[2] = (float)atof((*iter).c_str()); iter++;

				if (ASF_angles_are_degrees)
				{
					values[0] = deg2rad(values[0]);
					values[1] = deg2rad(values[1]);
					values[2] = deg2rad(values[2]);
				}

				DOF_StringToIndexes((*iter), dof);
				skeleton->setBoneAxis(id, values, dof);
			}
		}
		else if (AAPU::linePrefix(line, string("dof")))
		{
			DOF_ID dof_order[6];
			short num_dof = 0;
			AAPU::splitLine(line, prefix, rest);
			list<string> words;
			AAPU::parseWords(rest, words);
			list<string>::iterator iter = words.begin();
			while (iter != words.end())
			{
				string s = (*iter);
				if ((s == string("RX")) || (s == string("rx")))
					dof_order[num_dof++] = DOF_PITCH;
				else if ((s == string("RY")) || (s == string("ry")))
					dof_order[num_dof++] = DOF_YAW;
				else if ((s == string("RZ")) || (s == string("rz")))
					dof_order[num_dof++] = DOF_ROLL;
				else
				{
					sprintf(LOGBUFF, "ASF_Reader: INVALID dof values in BONE DOF line: %s", line.c_str());
					throw DataManagementException(string(LOGBUFF));
				}
				iter++;

				// num_limits needed since count here determines number of 
				// entries in "limits" section
				num_limits++; 
			}
			skeleton->setBoneChannels(id, dof_order, num_dof);
		}
		else if (AAPU::linePrefix(line, string("limits")))
		{
			int i=0;
			while (true) 
			{
				float min, max;
				AAPU::parseLimits(line, min, max);
				if (ASF_angles_are_degrees)
				{
					min = deg2rad(min);
					max = deg2rad(max);
				}
				skeleton->setBoneDOFLimit(id, i, min, max);
				i++;
				if (i>=num_limits) break;
				if (!line_scanner.getNextLine(line)) return false;
			}
		}
		else 
		{
			line_scanner.pushbackLine(line);
			sprintf(LOGBUFF, "ASF_Reader: INVALID LINE FOUND before END of bonedata: %s", line.c_str());
			throw DataManagementException(string(LOGBUFF));
		}
	}
}
