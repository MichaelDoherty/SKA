//-----------------------------------------------------------------------------
// DataManager.h
//	 Wrapper class around the various file and database interfaces.
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
#ifndef DATA_MANAGER_DOT_H
#define DATA_MANAGER_DOT_H
#include "Core/SystemConfiguration.h"
#include <vector>
#include <map>
#include <string>
using namespace std;

class Database;
class DatabaseLoader;
class SkeletonDefinition;
class MotionSequence;
class Character;
class DataIndex;

template class SKA_LIB_DECLSPEC std::allocator<char>;
template class SKA_LIB_DECLSPEC std::basic_string<char, std::char_traits<char>, std::allocator<char> >;

class SKA_LIB_DECLSPEC DataManager
{
public:
	DataManager();
	virtual ~DataManager();

	bool setupDatabase(const string& host, const string& user, const string& pw, const string& db);
	void useDatabase(bool flag=true);

	void setFilePathRoot(const string& root_path);
	string buildSkeletonDirname(const string& skel_id);
	string buildSkeletonFilename(const string& skel_id);
	string buildMotionFilename(const string& skel_id, const string& motion_id);

	SkeletonDefinition* loadSkeletonFromFile(const string& skel_id);
	SkeletonDefinition* loadSkeletonFromDatabase(const string& skel_id);
	SkeletonDefinition* loadSkeleton(const string& skel_id);

	// SkeletonDefinition is required to build the channel definitions
	// skel_id is used for definining the motion file name, and may be different
	//   from the skel_id of the SkeletonDefinition. If they are different, there is no check that they are compatible.
	MotionSequence* loadMotionFromFile(const string& skel_id, const string& motion_id, SkeletonDefinition* skel);
	MotionSequence* loadMotionFromDatabase(const string& skel_id, const string& motion_id, SkeletonDefinition* skel);
	MotionSequence* loadMotion(const string& skel_id, const string& motion_id, SkeletonDefinition* skel);

	bool getDescriptions(const string& skel_id, const string& motion_id, 
		string& skel_description, string& motion_description);
	//Character* loadCharacter(const string& skel_id, const string& motion_id, SkeletonDefinition* skel);

	bool writeSkeletonToFile(SkeletonDefinition* skel);
	bool writeMotionToFile(SkeletonDefinition* skel, MotionSequence* ms);

	void copyDataFromFilesToDatabase();

	DataIndex* getDataIndex() { return data_index; }

private:
	string file_root;

	Database* database;
	string db_host;
	string db_user;
	string db_pw;
	string db_db;

	bool use_database;
	DatabaseLoader* database_loader;

	DataIndex* data_index;
};

SKA_LIB_DECLSPEC extern DataManager data_manager;

#endif
