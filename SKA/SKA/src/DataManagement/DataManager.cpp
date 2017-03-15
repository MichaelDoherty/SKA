//-----------------------------------------------------------------------------
// DataManager.cpp
//	 Interface for reading and writing the various types of 
//   motion and skeleton files.
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
#include <cstdlib>
#include <vector>
#include <string>
using namespace std;
#include <DataManagement/DataManager.h>
#include <DataManagement/DataManagementException.h>
#include <DataManagement/ASF_Reader.h>
#include <DataManagement/AMC_Reader.h>
#include <DataManagement/ASF_Writer.h>
#include <DataManagement/AMC_Writer.h>
#include <DataManagement/BVH_Reader.h>
#include <DataManagement/SKS_ReaderWriter.h>
#include <DataManagement/SKM_ReaderWriter.h>
#include <DataManagement/FileSystem.h>
#include <Core/Utilities.h>
#include <Core/SystemTimer.h>
#include <Animation/Skeleton.h>
#include <Animation/MotionSequence.h>

struct DataManagerData {
	vector<string> paths;
};

DataManager data_manager;

DataManager::DataManager() 
{ 
	data = new DataManagerData; 
	data->paths.push_back(string("."));
}

DataManager::~DataManager() { delete data; }

void DataManager::addFileSearchPath(const char* _path)
{
	string p(_path);
	if (p.at(p.length()-1) != '/') p += '/';
	data->paths.push_back(p);
}

char* DataManager::findFile(const char* _file)
{
	string check_file = _file;
	if (FileSystem::fileExists(check_file.c_str()))
		return strClone(check_file.c_str());
	for (unsigned int i=0; i<data->paths.size(); i++)
	{
		string check_file = data->paths[i] + _file;
		if (FileSystem::fileExists(check_file.c_str()))
		{
			return strClone(check_file.c_str());
		}
	}
	return NULL;
}

//---------- ASF/AMC file management -------------------

pair<Skeleton*, MotionSequence*> DataManager::readASFAMC(
	const char* _asf_file, const char* _amc_file)
{
	Skeleton* skel = NULL;
	MotionSequence* ms = NULL;
	skel = readASF(_asf_file);
	if (skel != NULL)
	{
		try {
			ms = readAMC(skel, _amc_file);
		}
		catch (const DataManagementException& dme)
		{
			if (ms == NULL)	{ delete skel; skel = NULL;	}
			logout << "Rethrowing DataManagementException: " << dme.msg << endl;
			throw;
		}
	}
	return pair<Skeleton*, MotionSequence*>(skel, ms);
}

Skeleton* DataManager::readASF(const char* _asf_file)
{
	if (!FileSystem::fileExists(_asf_file)) 
	{
		string err = string("DataManager::readASF: Could not read ASF file ") + _asf_file + " (file not found).";
		logout << err << endl;
		throw DataManagementException(err.c_str());
	}

	ASF_Reader asf_reader;
	Skeleton* skel = asf_reader.readASF(_asf_file);
	if (skel == NULL)
	{
		string err = string("DataManager::readASF: Could not read ASF file ") + _asf_file + " (read failure).";
		logout << err << endl;
		throw DataManagementException(err.c_str());
	}
	return skel;
}

MotionSequence* DataManager::readAMC(
	Skeleton* _skel, const char* _amc_file)
{
	if (_skel == NULL) 
	{
		string err = string("DataManager::readAMC: Could not read AMC file ") + _amc_file + " (no skeleton supplied).";
		logout << err << endl;
		throw DataManagementException(err.c_str());
	}
	if (!FileSystem::fileExists(_amc_file)) 
	{ 
		string err = string("DataManager::readAMC: Could not read AMC file ") + _amc_file + " (file not found).";
		logout << err << endl;
		throw DataManagementException(err.c_str());
	}

	AMC_Reader amc_reader;
	MotionSequence* ms  = amc_reader.readAMC(_amc_file, _skel);

	if (ms == NULL) 
	{
		string err = string("DataManager::readAMC: Could not read AMC file ") + _amc_file + " (read failure).";
		logout << err << endl;
		throw DataManagementException(err.c_str());
	}

	return ms;
}

void DataManager::writeASFAMC(
	Skeleton* _skel, MotionSequence* _ms, 
	const char* _asf_file, const char* _amc_file)
{
	writeASF(_skel, _asf_file);
	writeAMC(_skel, _ms, _amc_file);
}

void DataManager::writeASF(
	Skeleton* _skel, const char* _asf_file)
{
	ASF_Writer asf_writer;
	if (!asf_writer.writeASF(_asf_file, _skel))
	{
		string err = string("DataManager::writeASF: Could not write ASF file ") + _asf_file + " (write failure).";
		logout << err << endl;
		throw DataManagementException(err.c_str());
	}
}

void DataManager::writeAMC(
	Skeleton* _skel, MotionSequence* _ms, const char* _amc_file)
{
	AMC_Writer amc_writer;
	if (!amc_writer.writeAMC(_amc_file, _skel, _ms))
	{
		string err = string("DataManager::writeAMC: Could not write AMC file ") + _amc_file + " (write failure).";
		logout << err << endl;
		throw DataManagementException(err.c_str());
	}
}

//---------- BVH file management -------------------

pair<Skeleton*, MotionSequence*> DataManager::readBVH(
		const char* _bvh_file)
{
	pair<Skeleton*, MotionSequence*> result;
	result.first = NULL;
	result.second = NULL;
	if (!FileSystem::fileExists(_bvh_file)) 
	{
		string err = string("DataManager::readBVH: Could not read BVH file ") + _bvh_file + " (file not found).";
		logout << err << endl;
		throw DataManagementException(err.c_str());
	}
	BVH_Reader bvh_reader;
	result = bvh_reader.readBVH(_bvh_file);
	if ((result.first == NULL) || (result.second == NULL))
	{
		if (result.first != NULL) delete result.first;
		if (result.second != NULL) delete result.second;
		result.first = NULL;
		result.second = NULL;
		string err = string("DataManager::readBVH: Could not read BVH file ") + _bvh_file + " (read failure).";
		logout << err << endl;
		throw DataManagementException(err.c_str());
	}
	return result;
}

void DataManager::writeBVH(
	Skeleton* _skel, MotionSequence* _ms, const char* _bvh_file)
{
	string err = string("DataManager::writeBVH: Could not write BVH file ") + _bvh_file + " (function not yet available).";
	logout << err << endl;
	throw DataManagementException(err.c_str());
}

//---------- SKS/SKM file management -------------------

pair<Skeleton*, MotionSequence*> DataManager::readSKSSKM(
	const char* _sks_file, const char* _skm_file)
{
	Skeleton* skel = NULL;
	MotionSequence* ms = NULL;
	skel = readSKS(_sks_file);
	if (skel != NULL)
	{
		try {
			ms = readSKM(skel, _skm_file);
		}
		catch (const DataManagementException& dme)
		{
			if (ms == NULL)	{ delete skel; skel = NULL;	}
			logout << "Rethrowing DataManagementException: " << dme.msg << endl;
			throw;
		}
	}
	return pair<Skeleton*, MotionSequence*>(skel, ms);
}

Skeleton* DataManager::readSKS(const char* _sks_file)
{
	if (!FileSystem::fileExists(_sks_file)) 
	{
		string err = string("DataManager::readSKS: Could not read ASF file ") + _sks_file + " (file not found).";
		logout << err << endl;
		throw DataManagementException(err.c_str());
	}

	Skeleton* skel = SKS_ReaderWriter::readSKS(_sks_file);
	if (skel == NULL)
	{
		string err = string("DataManager::readSKS: Could not read ASF file ") + _sks_file + " (read failure).";
		logout << err << endl;
		throw DataManagementException(err.c_str());
	}
	return skel;
}

MotionSequence* DataManager::readSKM(
	Skeleton* _skel, const char* _skm_file)
{
	if (_skel == NULL) 
	{
		string err = string("DataManager::readAMC: Could not read AMC file ") + _skm_file + " (no skeleton supplied).";
		logout << err << endl;
		throw DataManagementException(err.c_str());
	}
	if (!FileSystem::fileExists(_skm_file)) 
	{ 
		string err = string("DataManager::readAMC: Could not read AMC file ") + _skm_file + " (file not found).";
		logout << err << endl;
		throw DataManagementException(err.c_str());
	}

	MotionSequence* ms  = SKM_ReaderWriter::readSKM(_skm_file, _skel);

	if (ms == NULL) 
	{
		string err = string("DataManager::readAMC: Could not read AMC file ") + _skm_file + " (read failure).";
		logout << err << endl;
		throw DataManagementException(err.c_str());
	}

	return ms;
}

void DataManager::writeSKSSKM(
	Skeleton* _skel, MotionSequence* _ms, const char* _sks_file, const char* _skm_file)
{
	writeSKS(_skel, _sks_file);
	writeSKM(_skel, _ms, _skm_file);
}

void DataManager::writeSKS(
	Skeleton* _skel, const char* _sks_file)
{
	if (!SKS_ReaderWriter::writeSKS(_sks_file, _skel))
	{
		string err = string("DataManager::writeASF: Could not write ASF file ") + _sks_file + " (write failure).";
		logout << err << endl;
		throw DataManagementException(err.c_str());
	}
}

void DataManager::writeSKM(
	Skeleton* _skel, MotionSequence* _ms, const char* _skm_file)
{
	if (!SKM_ReaderWriter::writeSKM(_skm_file, _skel, _ms))
	{
		string err = string("DataManager::writeAMC: Could not write AMC file ") + _skm_file + " (write failure).";
		logout << err << endl;
		throw DataManagementException(err.c_str());
	}
}

//---------- Format Conversion Utilities -------------------

void DataManager::openAllEulerChannels(
		Skeleton* _skel, 
		MotionSequence* _ms)
{
	CHANNEL_TYPE channel_order[3] = { CT_RX, CT_RY, CT_RZ };
	for (int b=1; b <_skel->numBones(); b++)
	{
		for (CHANNEL_TYPE ct=CT_RX; ct<=CT_RZ; ct=CHANNEL_TYPE(ct+1))
		{
			CHANNEL_ID cid(b,ct);
			if (!_skel->isActiveChannel(b, ct)) _ms->addChannel(cid); 
		}
		// TRICKY BIT: We also need to know the application order of the Euler channels
		//  Suppose the ASF said "dof rx rz", how do we decide if we want "ry rx rz", "rx ry rz" or "rx rz ry"?
		// The axisRemoval test (axrt app) assumes it should be "rx ry rz".
		// This currently assumes "rx ry rz".
		_skel->setBoneChannels(b,channel_order,3);
	}
}