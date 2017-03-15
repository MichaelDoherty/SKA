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

#include <Core/SystemConfiguration.h>
#include <DataManagement/DataManagementException.h>
#include <DataManagement/ASF_Reader.h>
#include <fstream>
#include <cstdlib>
using namespace std;

static const int LOGBUFF_SIZE = 1000;
static char LOGBUFF[LOGBUFF_SIZE];

static void CHANNEL_TYPE_StringToIndexes(string& dof_s, CHANNEL_TYPE dof[3])
{
	// string is "XYZ", "ZYX", ...
	// represents order of axis values (and application order)
	for (short d=0; d<3; d++)
	{
		switch (dof_s[d])
		{
		case 'X': case 'x': dof[d] = CT_RX; break;
		case 'Y': case 'y': dof[d] = CT_RY; break;
		case 'Z': case 'z': dof[d] = CT_RZ; break;
		}
	}
}

// =============================================================
// readASF

class ASF_Reader_Local
{
public:
	ASF_Reader_Local() : skeleton(NULL), ASF_angles_are_degrees(true) { }
	Skeleton* readASF(const char* inputFilename);
private:
	Skeleton* skeleton;
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

Skeleton* ASF_Reader_Local::readASF(const char* inputFilename)
{
	LineScanner line_scanner(inputFilename);
	if (!line_scanner.fileIsOpen()) return NULL;

	skeleton = new Skeleton(inputFilename);
	string line;
	
	while (line_scanner.getNextLine(line))
	{
		if (ParsingUtilities::linePrefix(line, string("#")))
			continue;
		else if (ParsingUtilities::linePrefix(line, string(":version")))
			processVersionSection(line_scanner, line);
		else if (ParsingUtilities::linePrefix(line, string(":name")))
			processNameSection(line_scanner, line);
		else if (ParsingUtilities::linePrefix(line, string(":units")))
			processUnitsSection(line_scanner, line);
		else if (ParsingUtilities::linePrefix(line, string(":documentation")))
			processDocumentationSection(line_scanner, line);
		else if (ParsingUtilities::linePrefix(line, string(":root")))
			processRootSection(line_scanner, line);
		else if (ParsingUtilities::linePrefix(line, string(":bonedata")))
			processBonedataSection(line_scanner, line);
		else if (ParsingUtilities::linePrefix(line, string(":hierarchy")))
			processHierarchySection(line_scanner, line);
		else 
		{
			sprintf(LOGBUFF, "ASF_Reader BAD LINE: %s", line.c_str());
			throw DataManagementException(LOGBUFF);
		}
	}
	skeleton->finalizeInitialization();
	return skeleton;
}

bool ASF_Reader_Local::processComment(LineScanner& line_scanner, string& line)
{
	if (ParsingUtilities::linePrefix(line, string("# Documentation:"))) 
	{
		ParsingUtilities::stripPrefix(line, string("# Documentation:"));
		skeleton->addDocumentation(line.c_str());
	}
	else if (ParsingUtilities::linePrefix(line, string("# Source:"))) 
	{
		ParsingUtilities::stripPrefix(line, string("# Source:"));
		skeleton->setSource(line.c_str());
	}
	return true;
}

bool ASF_Reader_Local::processVersionSection(LineScanner& line_scanner, string& line)
{
	if (!ParsingUtilities::stripPrefix(line, string(":version"))) return false;
	// No longer stored. 
	//skeleton->setVersion(line);
	return true;
}

bool ASF_Reader_Local::processNameSection(LineScanner& line_scanner, string& line)
{
	if (!ParsingUtilities::stripPrefix(line, string(":name"))) return false;
	//  No longer stored.
	//skeleton->setAsfName(line);
	return true;
}

bool ASF_Reader_Local::processUnitsSection(LineScanner& line_scanner, string& line)
{
	while (true)
	{
		if (!line_scanner.getNextLine(line)) return true;
		if (ParsingUtilities::linePrefix(line, string(":")))
		{
			line_scanner.pushbackLine(line);
			return true;
		}
		string unit_name;
		string unit_value;
		ParsingUtilities::splitLine(line, unit_name, unit_value);
		skeleton->addUnits(unit_name.c_str(), unit_value.c_str());
		if ((unit_name == string("angle")) && (unit_value == string("deg")))
			ASF_angles_are_degrees = true;
	}
	return true;
}

bool ASF_Reader_Local::processDocumentationSection(LineScanner& line_scanner, string& line)
{
	while (true)
	{
		if (!line_scanner.getNextLine(line)) return true;
		if (ParsingUtilities::linePrefix(line, string(":")))
		{
			line_scanner.pushbackLine(line);
			return true;
		}
		skeleton->addDocumentation(line.c_str());
	}
	return true;
}

bool ASF_Reader_Local::processRootSection(LineScanner& line_scanner, string& line)
{
	skeleton->createBone(0, "root");
	while (true)
	{
		if (!line_scanner.getNextLine(line)) return true;
		if (ParsingUtilities::linePrefix(line, string(":")))
		{
			line_scanner.pushbackLine(line);
			return true;
		}
		string prefix, rest;
		if (ParsingUtilities::linePrefix(line, string("order")))
		{
			CHANNEL_TYPE dof_order[6];
			short num_dof = 0;
			ParsingUtilities::splitLine(line, prefix, rest);
			list<string> words;
			ParsingUtilities::parseWords(rest, words);
			list<string>::iterator iter = words.begin();
			while (iter != words.end())
			{
				string s = (*iter);
				if ((s == string("TX")) || (s == string("tx")))
					dof_order[num_dof++] = CT_TX;
				else if ((s == string("TY")) || (s == string("ty")))
					dof_order[num_dof++] = CT_TY;
				else if ((s == string("TZ")) || (s == string("tz")))
					dof_order[num_dof++] = CT_TZ;
				else if ((s == string("RX")) || (s == string("rx")))
					dof_order[num_dof++] = CT_RX;
				else if ((s == string("RY")) || (s == string("ry")))
					dof_order[num_dof++] = CT_RY;
				else if ((s == string("RZ")) || (s == string("rz")))
					dof_order[num_dof++] = CT_RZ;
				iter++;
			}
			skeleton->setBoneChannels(0, dof_order, num_dof);
		}
		else if (ParsingUtilities::linePrefix(line, string("axis")))
		{
			ParsingUtilities::splitLine(line, prefix, rest);
			float values[3] = { 0.0f, 0.0f, 0.0f };
			CHANNEL_TYPE dof[3];
			CHANNEL_TYPE_StringToIndexes(rest, dof);
			skeleton->setBoneAxis(0, values, dof);
		}
		else if (ParsingUtilities::linePrefix(line, string("position")))
		{
			ParsingUtilities::splitLine(line, prefix, rest);
			list<float> values;
			ParsingUtilities::parseFloats(rest, values);
			if (values.size() != 3)
			{
				sprintf(LOGBUFF, "ASF_Reader: INVALID number of values in root position line: %s", line.c_str());
				throw DataManagementException(LOGBUFF);
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
		else if (ParsingUtilities::linePrefix(line, string("orientation")))
		{
			ParsingUtilities::splitLine(line, prefix, rest);
			list<float> values;
			ParsingUtilities::parseFloats(rest, values);
			if (values.size() != 3)
			{
				sprintf(LOGBUFF, "ASF_Reader: INVALID number of values in root orientation line: %s", line.c_str());
				throw DataManagementException(LOGBUFF);
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

bool ASF_Reader_Local::processHierarchySection(LineScanner& line_scanner, string& line)
{
	while (true)
	{
		if (!line_scanner.getNextLine(line)) return true;
		if (ParsingUtilities::linePrefix(line, string(":")))
		{
			line_scanner.pushbackLine(line);
			return true;
		}
		list<string> bonenames;
		ParsingUtilities::parseWords(line, bonenames);
		if (bonenames.size() > 1)
		{
			list<string>::iterator iter = bonenames.begin();
			string parent = (*iter); iter++;
			while (iter != bonenames.end())
			{
				skeleton->addConnection(parent.c_str(), (*iter).c_str());
				iter++;
			}
		}
	}
}

bool ASF_Reader_Local::processBonedataSection(LineScanner& line_scanner, string& line)
{
	while (true)
	{
		if (!line_scanner.getNextLine(line)) return true;
		if (ParsingUtilities::linePrefix(line, string(":")))
		{
			line_scanner.pushbackLine(line);
			return true;
		}
		else if (ParsingUtilities::linePrefix(line, string("begin")))
		{
			processBone(line_scanner);
		}
		else 
		{
			sprintf(LOGBUFF, "ASF_Reader: INVALID line in Bonedata section: %s", line.c_str());
			throw DataManagementException(LOGBUFF);
		}
	}
}

bool ASF_Reader_Local::processBone(LineScanner& line_scanner)
{
	string line, prefix, rest;

	short id = -1;
	short num_limits = 0;

	while (true)
	{
		if (!line_scanner.getNextLine(line))
		{
			sprintf(LOGBUFF, "ASF_Reader: EOF FOUND before END of bonedata: %s", line.c_str());
			throw DataManagementException(LOGBUFF);
		}
		if (ParsingUtilities::linePrefix(line, string(":")))
		{
			line_scanner.pushbackLine(line);
			sprintf(LOGBUFF, "ASF_Reader: INVALID LINE FOUND before END of bonedata: %s", line.c_str());
			throw DataManagementException(LOGBUFF);
		}
		else if (ParsingUtilities::linePrefix(line, string("end")))
		{
			return true;
		}
		else if (ParsingUtilities::linePrefix(line, string("id")))
		{
			ParsingUtilities::splitLine(line, prefix, rest);
			list<long> values;
			if (ParsingUtilities::parseInts(rest, values))
			{
				list<long>::iterator iter = values.begin();
				id = (short)(*iter);
			}
		}
		else if (ParsingUtilities::linePrefix(line, string("name")))
		{
			ParsingUtilities::splitLine(line, prefix, rest);
			list<string> words;
			if (ParsingUtilities::parseWords(rest, words))
			{
				list<string>::iterator iter = words.begin();
				skeleton->createBone(id, (*iter).c_str());
			}
		}
		else if (ParsingUtilities::linePrefix(line, string("direction")))
		{
			ParsingUtilities::splitLine(line, prefix, rest);
			list<float> values;
			ParsingUtilities::parseFloats(rest, values);
			if (values.size() != 3)
			{
				sprintf(LOGBUFF, "ASF_Reader: INVALID number of values in BONE DIRECTION line: %s", line.c_str());
				throw DataManagementException(LOGBUFF);
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
		else if (ParsingUtilities::linePrefix(line, string("length")))
		{
			ParsingUtilities::splitLine(line, prefix, rest);
			list<float> values;
			ParsingUtilities::parseFloats(rest, values);
			if (values.size() != 1)
			{
				sprintf(LOGBUFF, "ASF_Reader: INVALID number of values in BONE LENGTH line: %s", line.c_str());
				throw DataManagementException(LOGBUFF);
			}
			else 
			{
				list<float>::iterator iter = values.begin();
				float x=(*iter);
				skeleton->setBoneLength(id, x);
			}
		}
		else if (ParsingUtilities::linePrefix(line, string("axis")))
		{
			list<string> words;
			ParsingUtilities::parseWords(line, words);
			if (words.size() != 5)
			{
				sprintf(LOGBUFF, "ASF_Reader: INVALID number of values in BONE AXIS line: %s", line.c_str());
				throw DataManagementException(LOGBUFF);
			}
			else
			{
				float values[3];
				CHANNEL_TYPE dof[3];

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

				CHANNEL_TYPE_StringToIndexes((*iter), dof);
				skeleton->setBoneAxis(id, values, dof);
			}
		}
		else if (ParsingUtilities::linePrefix(line, string("dof")))
		{
			CHANNEL_TYPE dof_order[6];
			short num_dof = 0;
			ParsingUtilities::splitLine(line, prefix, rest);
			list<string> words;
			ParsingUtilities::parseWords(rest, words);
			list<string>::iterator iter = words.begin();
			while (iter != words.end())
			{
				string s = (*iter);
				if ((s == string("RX")) || (s == string("rx")))
					dof_order[num_dof++] = CT_RX;
				else if ((s == string("RY")) || (s == string("ry")))
					dof_order[num_dof++] = CT_RY;
				else if ((s == string("RZ")) || (s == string("rz")))
					dof_order[num_dof++] = CT_RZ;
				else
				{
					sprintf(LOGBUFF, "ASF_Reader: INVALID dof values in BONE DOF line: %s", line.c_str());
					throw DataManagementException(LOGBUFF);
				}
				iter++;

				// num_limits needed since count here determines number of 
				// entries in "limits" section
				num_limits++; 
			}
			skeleton->setBoneChannels(id, dof_order, num_dof);
		}
		else if (ParsingUtilities::linePrefix(line, string("limits")))
		{
			int i=0;
			while (true) 
			{
				float min, max;
				ParsingUtilities::parseLimits(line, min, max);
				if (ASF_angles_are_degrees)
				{
					min = deg2rad(min);
					max = deg2rad(max);
				}
				skeleton->setBoneChannelLimit(id, i, min, max);
				i++;
				if (i>=num_limits) break;
				if (!line_scanner.getNextLine(line)) return false;
			}
		}
		else 
		{
			line_scanner.pushbackLine(line);
			sprintf(LOGBUFF, "ASF_Reader: INVALID LINE FOUND before END of bonedata: %s", line.c_str());
			throw DataManagementException(LOGBUFF);
		}
	}
}

ASF_Reader::ASF_Reader() 
{
	local_reader = new ASF_Reader_Local;
}

ASF_Reader::~ASF_Reader()
{
	delete local_reader;
}

Skeleton* ASF_Reader::readASF(const char* inputFilename)
{
	return local_reader->readASF(inputFilename);
}