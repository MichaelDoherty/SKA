//-----------------------------------------------------------------------------
// DataManager.h
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

#ifndef DATA_MANAGER_DOT_H
#define DATA_MANAGER_DOT_H
#include <Core/SystemConfiguration.h>
#include <utility>
using namespace std;

class Skeleton;
class MotionSequence;
struct DataManagerData;

class SKA_LIB_DECLSPEC DataManager
{
public:
	DataManager();
	virtual ~DataManager();

//---------- simple file search interface -------------------

	void addFileSearchPath(const char* _path);
	char* findFile(const char* _file);

//---------- ASF/AMC file management -------------------

	// A Skeleton is required to build the channel definitions
	//   when reading an AMC file.
	//   There is no check that the skeleton and the motion are compatible.
	//
	// force_angle_channels_active forces allocation of data space for all three angles
	//   for every bone, even when the ASF specfies limited DOF.
	//   This obviously wastes space, but it makes the motion compatible 
	//   for conversions to other formats.
	pair<Skeleton*, MotionSequence*> readASFAMC(
		const char* _asf_file, 
		const char* _amc_file);
	Skeleton* readASF(
		const char* _asf_file);
	MotionSequence* readAMC(
		Skeleton* _skel,
		const char* _amc_file);

	void writeASFAMC(
		Skeleton* _skel, 
		MotionSequence* _ms, 
		const char* _asf_file,
		const char* _amc_file);
	void writeASF(
		Skeleton* _skel,
		const char* _asf_file);
	void writeAMC(
		Skeleton* _skel, 
		MotionSequence* _ms, 
		const char* _amc_file);

//---------- BVH file management -------------------

	pair<Skeleton*, MotionSequence*> readBVH(
		const char* _bvh_file);

	void writeBVH(
		Skeleton* _skel, 
		MotionSequence* _ms, 
		const char* _bvh_file);

//---------- SKS/SKM file management -------------------

	pair<Skeleton*, MotionSequence*> readSKSSKM(
		const char* _asf_file, 
		const char* _amc_file);
	Skeleton* readSKS(
		const char* _asf_file);
	MotionSequence* readSKM(
		Skeleton* _skel,
		const char* _amc_file);

	void writeSKSSKM(
		Skeleton* _skel, 
		MotionSequence* _ms, 
		const char* _asf_file,
		const char* _amc_file);
	void writeSKS(
		Skeleton* _skel,
		const char* _asf_file);
	void writeSKM(
		Skeleton* _skel, 
		MotionSequence* _ms, 
		const char* _amc_file);

//---------- Format Conversion Utilities -------------------

	// This converts from formats that use an axis to avoid needing 
	// three channels for joints with less than 3 DOF (such as ASF)
	// to formats that simply use all three channels all the time (such as BVH).
	void openAllEulerChannels(
		Skeleton* _skel, 
		MotionSequence* _ms);

private:
	DataManagerData* data;
};

SKA_LIB_DECLSPEC extern DataManager data_manager;

#endif
