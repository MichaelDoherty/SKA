//-----------------------------------------------------------------------------
// SKM_ReaderWriter.cpp
//	 Reads and writes motions to/from SKA motion files (SKM)
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
#include <iostream> // test only
#include <fstream>
#include <string>
#include <vector>
using namespace std;
#include <Core/Array2D.h>
#include <DataManagement/DataManagementException.h>
#include <DataManagement/SKM_ReaderWriter.h>
#include <DataManagement/FileSystem.h>

// Motion data files are big endian binary. 
// If this is a little endian machine, endian conversion is needed.
static long endiantest = 1;
#define LITTLEENDIAN *((char*)(&endiantest)) // 1 for little-endian, 0 for big-endian

inline void swapEndian2(char s[2])
{
	char c=s[0]; s[0]=s[1]; s[1]=c;
}

inline void swapEndian4(char s[4])
{
	char c;
	c=s[0]; s[0]=s[3]; s[3]=c;
	c=s[1]; s[1]=s[2]; s[2]=c;
}

class MotionData
{
public:
	MotionData() : 	endiancheck(1), r(0), c(0), data(NULL)
	{ }
	~MotionData()
	{ clear(); }
	void clear()
	{ r=0; c=0; if (data != NULL) delete [] data; data = NULL; }
	bool isValid()
	{ return data != NULL; }

	bool getDimensions(long& _r, long& _c) 
	{ if (!isValid()) return false; _r = r; _c = c; return true; }
	bool getDatamode(char _dm[4]) 
	{ if (!isValid()) return false; memcpy(_dm, datamode, 4); return true; }
	bool getMotionId(char _mid[64])
	{ if (!isValid()) return false; memcpy(_mid, motion_id, 64); return true; }
	bool getSkeletonId(char _sid[64])
	{ if (!isValid()) return false; memcpy(_sid, skeleton_id, 64); return true; }
	char* getDataPtr() { return data; }

// methods for reading from a file
	bool readFromFile(string& filename);
// methods for writing to a file:
	void setup(long _r, long _c, char _mode[4], char* _mid, char* _sid, char* _data);
	bool writeToFile(string& filename);

private:
	long endiancheck;
	long r;
	long c;
	char datamode[4];
	char motion_id[64];
	char skeleton_id[64];
	char* data;
};

void MotionData::setup(long _r, long _c, char _mode[4], char* _mid, char* _sid, char* _data)
{
	clear();
	endiancheck = 1;
	r = _r;
	c = _c;
	memcpy(datamode, _mode, 4);
	memcpy(motion_id, _mid, 64); motion_id[63] = '\0';
	memcpy(skeleton_id, _sid, 64); motion_id[63] = '\0';
	data = new char[r*c*sizeof(float)];
	memcpy(data, _data, r*c*sizeof(float));
}

bool MotionData::writeToFile(string& filename)
{
	FILE *fp = fopen(filename.c_str(), "wb");
	if (fp == NULL) return false;
	
	long datasize = r*c*sizeof(float);

	char header[144];
	char* p = header;

	memcpy(p, &endiancheck, sizeof(long));
	if (LITTLEENDIAN) swapEndian4(p);
	p += sizeof(long);
	
	memcpy(p, &r, sizeof(long));
	if (LITTLEENDIAN) swapEndian4(p);
	p += sizeof(long);
	
	memcpy(p, &c, sizeof(long));
	if (LITTLEENDIAN) swapEndian4(p);
	p += sizeof(long);
	
	memcpy(p, datamode, 4);
	p += 4;
	
	memcpy(p, motion_id, 64);
	p += 64;
	
	memcpy(p, skeleton_id, 64);

	char* localdata = data;
	if (LITTLEENDIAN)
	{
		localdata = new char[datasize];
		memcpy(localdata, data, datasize);
		for (long i=0; i<datasize; i+=4) swapEndian4(&(localdata[i]));
	}

	fwrite(header, 1, 144, fp);
	fwrite(localdata, 1, datasize, fp);

	if (LITTLEENDIAN) delete [] localdata;

	fclose(fp);
	return true;
}

bool MotionData::readFromFile(string& filename)
{
	clear();
	FILE *fp = fopen(filename.c_str(), "rb");
	if (fp == NULL) return false;
	
	long datasize = r*c*sizeof(float);

	char header[144];
	fread(header, 1, 144, fp);

	char* p = header;
	if (LITTLEENDIAN) swapEndian4(p);
	memcpy(&endiancheck, p, sizeof(long));
	if (endiancheck != 1) return false;
	p += sizeof(long);
	
	if (LITTLEENDIAN) swapEndian4(p);
	memcpy(&r, p, sizeof(long));
	p += sizeof(long);
	
	if (LITTLEENDIAN) swapEndian4(p);
	memcpy(&c, p, sizeof(long));
	p += sizeof(long);
	
	memcpy(datamode, p, 4);
	p += 4;
	
	memcpy(motion_id, p, 64);
	p += 64;
	
	memcpy(skeleton_id, p, 64);

	data = new char[datasize];
	fread(data, 1, datasize, fp);

	if (LITTLEENDIAN) 
	{
		for (long i=0; i<datasize; i+=4) swapEndian4(&(data[i]));
	}

	fclose(fp);
	return true;
}

MotionSequence* SKM_ReaderWriter::readSKM(
	const char* inputFilename,
	Skeleton* skeleton)
{
	MotionData read_buffer;
	string sfilename(inputFilename);
	if (!read_buffer.readFromFile(sfilename)) return NULL;

	long r;
	long c;
	read_buffer.getDimensions(r,c);
	
	char mode[4]; memset(mode, 0, 4);
	read_buffer.getDatamode(mode);

	char sid[64]; memset(sid, 0, 64);
	read_buffer.getSkeletonId(sid);

	char mid[64]; memset(mid, 0, 64);
	read_buffer.getMotionId(mid);

	char* data = read_buffer.getDataPtr();

	MotionSequence* ms = new MotionSequence;
	short frames = short(r);
	ms->setNumFrames(frames);
	ms->setFrameRate(120);
	
	//ms->buildChannelsFromSkeleton(skeleton);
	vector<CHANNEL_ID> channel_ids;
	for (short b=0; b<skeleton->numBones(); b++)
	{
		for (short channel_type=0; channel_type<6; channel_type++)
		{
			CHANNEL_ID c(b, CHANNEL_TYPE(channel_type));
			if (skeleton->isActiveChannel(b,channel_type))
				channel_ids.push_back(c);
		}
	}

	int chans = channel_ids.size();
	CHANNEL_ID* cid = new CHANNEL_ID[chans];
	for (unsigned short c=0; c<chans; c++) cid[c] = channel_ids[c];

	Array2D<float> data_matrix;
	data_matrix.resize(frames, chans);
	memcpy(data_matrix.getColumnPtr(0), data, chans*frames*sizeof(float));

	ms->bulkBuild(cid, chans, data_matrix);
	delete cid;

	ms->setId(mid);

	return ms;
}
	
bool SKM_ReaderWriter::writeSKM(
	const char* outputFilename,
	Skeleton* skeleton,
	MotionSequence* ms,
	bool overwrite)
{
	if (!overwrite && FileSystem::fileExists(outputFilename)) return false;

	long r = ms->numFrames();
	long c = ms->numChannels();
	
	char mode[4]; memset(mode, 0, 4);
	strcpy(mode, "ra"); // rotation angles
	
	char sid[64]; memset(sid, 0, 64);
	string skel_id = skeleton->getId();
	skel_id.resize(63,'\0');
	strcpy(sid, skel_id.c_str());

	char mid[64]; memset(mid, 0, 64);
	string ms_id = ms->getId();
	ms_id.resize(63,'\0');
	strcpy(mid, ms_id.c_str());

	char* data = (char*)ms->getChannelPtr(0);

	MotionData write_buffer;
	
	write_buffer.setup(r, c, mode, mid, sid, data);
	string sfilename(outputFilename);
	write_buffer.writeToFile(sfilename);
	return true;
}
