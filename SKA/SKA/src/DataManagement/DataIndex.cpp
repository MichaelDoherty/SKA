//-----------------------------------------------------------------------------
// DataIndex.cpp
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
#include "Core/SystemConfiguration.h"
#include "DataManagement/DataIndex.h"
#include <map>
#include <fstream>
#include <cstring>
using namespace std;

struct ASF_FILE_DESC
{
	string ASF_number;
	string description;
	string filename;
	ASF_FILE_DESC(const string& _number, const string& _description, 
                      const string& _filename)
		: ASF_number(_number), description(_description), filename(_filename)
	{
	}
};

struct AMC_FILE_DESC
{
	string ASF_number;
	string AMC_number;
	string description;
	string framerate;
	string filename;
	AMC_FILE_DESC(const string& _ASF_number, const string& _AMC_number, 
                const string& _description,
		const string& _framerate, const string& _filename)
		: ASF_number(_ASF_number), AMC_number(_AMC_number), description(_description),
		framerate(_framerate), filename(_filename)
	{
	}
};

class ASF_FileDescList
{
private:
	map<string, ASF_FILE_DESC> descs;
public:
	void insert(const ASF_FILE_DESC& d)
	{
		descs.insert(pair<string, ASF_FILE_DESC>(d.ASF_number, d));
	}
	bool getData(const string& ASF_number, string& description, string& filename)
	{
		map<string, ASF_FILE_DESC>::iterator iter = descs.find(ASF_number);
		if (iter == descs.end()) return false;
		description = (*iter).second.description;
		filename = (*iter).second.filename;
		return true;
	}
};

class AMC_FileDescList
{
private:
	map<string, AMC_FILE_DESC> descs;
	void makekey(const string& ASF_num, const string& AMC_num, string& key)
	{
		key = ASF_num + AMC_num;
	}
public:
	void insert(const AMC_FILE_DESC& d)
	{
		string key;
		makekey(d.ASF_number, d.AMC_number, key);
		descs.insert(pair<string, AMC_FILE_DESC>(key, d));
	}
	bool getData(const string& ASF_number, const string& AMC_number,
		string& description, string& framerate, string& filename)
	{
		string key;
		makekey(ASF_number, AMC_number, key);
		map<string, AMC_FILE_DESC>::iterator iter = descs.find(key);
		if (iter == descs.end()) 
			return false;
		description = (*iter).second.description;
		framerate = (*iter).second.framerate;
		filename = (*iter).second.filename;
		return true;
	}
	int getAllMotions(vector<pair <string, string> >& motions)
	{
		motions.clear();
		map<string, AMC_FILE_DESC>::iterator iter = descs.begin();
		while (iter != descs.end())
		{
			motions.push_back(pair<string,string>((*iter).second.ASF_number, (*iter).second.AMC_number));
			iter++;
		}
		return motions.size();
	}
};

static ASF_FileDescList asf_list;
static AMC_FileDescList amc_list;

static char* next_field(char* s, char* t)
{
	char* p;
	for (p=s; (((*p)!='\t') && ((*p)!='\n') && ((*p)!='\0')); p++)
	{
		*t = *p;
		t++;
	}
	*t = '\0';
	if (((*p)=='\t') || ((*p)=='\n')) p++;
	return p;
}

DataIndex::DataIndex()
{
}

DataIndex::~DataIndex()
{
}

void DataIndex::loadASFAMCDescriptions(const char* index_file)
{
	char line[2000];
	int line_size = 2000;
	char* p;
	ifstream ifs;
	ifs.open(index_file);
	while (true)
	{
		ifs.getline(line, line_size);
		if (!ifs) break;
		p = line;
		char line_type[1000];
		p = next_field(p, line_type);
		if (strcmp(line_type, "SKELETON") == 0)
		{
			char subject_label[1000];
			char subject_description[2000];
			char subject_datafile[1000];
			p = next_field(p, subject_label);
			p = next_field(p, subject_description);
			p = next_field(p, subject_datafile);
			asf_list.insert(
				ASF_FILE_DESC(
					string(subject_label), 
					string(subject_description), 
					string(subject_datafile)));
		}
		else if (strcmp(line_type, "MOTION") == 0)
		{
			char subject_label[1000];
			char motion_label[1000];
			char motion_description[2000];
			char frame_rate[100];
			char subject_datafile[1000];
			char motion_datafile[1000];
			p = next_field(p, subject_label);
			p = next_field(p, motion_label);
			p = next_field(p, motion_description);
			p = next_field(p, frame_rate);
			p = next_field(p, subject_datafile);
			p = next_field(p, motion_datafile);
			amc_list.insert(
				AMC_FILE_DESC(
					string(subject_label), 
					string(motion_label), 
					string(motion_description), 
					string(frame_rate), 
					string(motion_datafile)));

		}
	}
	ifs.close();
}

bool DataIndex::getASFData(const string& ASF_number, string& description, string& filename)
{
	return asf_list.getData(ASF_number, description, filename);
}

bool DataIndex::getAMCData(const string& ASF_number, const string& AMC_number,
		string& description, string& framerate, string& filename)
{
	return amc_list.getData(ASF_number, AMC_number,
		description, framerate, filename);
}

int DataIndex::getAllIndexedMotions(vector<pair <string, string> >& motions)
{
	return amc_list.getAllMotions(motions);
}
