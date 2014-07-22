//-----------------------------------------------------------------------------
// DataManager.cpp
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
#include "Core/SystemConfiguration.h"
#include <cstdlib>
#include "DataManagement/DataManager.h"
#include "DataManagement/DataManagementException.h"
#include "DataManagement/Database.h"
#include "DataManagement/DatabaseLoader.h"
#include "DataManagement/DataIndex.h"
#include "DataManagement/ASF_Reader.h"
#include "DataManagement/AMC_Reader.h"
#include "DataManagement/ASF_Writer.h"
#include "DataManagement/AMC_Writer.h"
#include "DataManagement/FileSystem.h"
#include "Core/Utilities.h"
#include "Core/SystemTimer.h"
#include "Animation/SkeletonDefinition.h"
#include "Animation/MotionSequence.h"
#include "Animation/Character.h"

DataManager data_manager;

DataManager::DataManager()
: database(NULL), use_database(false)
{ 
	data_index = new DataIndex;
}

DataManager::~DataManager()
{
	if (database != NULL)
	{
		database->close();
		delete database;
		database = NULL;
	}
	if (database_loader != NULL)
	{
		delete database_loader; 
		database_loader = NULL;
	}
	if (data_index != NULL)
	{
		delete data_index;
		data_index = NULL;
	}
}

bool DataManager::setupDatabase(const string& host, const string& user, const string& pw, const string& db)
{
	db_host = host;
	db_user = user;
	db_pw = pw;
	db_db = db;
	if (database == NULL) database = new Database;
	return database->open(db_host.c_str(), db_user.c_str(), db_pw.c_str(), db_db.c_str());
}
	
void DataManager::useDatabase(bool flag) 
{ 
	if (database == NULL)
	{
		use_database = false;
		return;
	}
	if (flag)
	{
		if (database_loader == NULL) database_loader = new DatabaseLoader(database);
		else database_loader->setDatabase(database);
		use_database = true;
	}
	else
	{
		if (database_loader != NULL) 
		{
			delete database_loader;
			database_loader = NULL;
		}
		use_database = flag; 
	}
}

void DataManager::setFilePathRoot(const string& root_path)
{
	file_root = root_path;
	// FIXIT! not the best place for this, but it forces it to get done.
	//        should be fixed when CMU_Index is replaced.
	string index_file = file_root + string("/CMU_index.txt");
	data_index->loadASFAMCDescriptions(index_file.c_str());
}

string DataManager::buildSkeletonDirname(const string& skel_id)
{
	string fn = file_root + string("/") + skel_id;
	return fn;
}

string DataManager::buildSkeletonFilename(const string& skel_id)
{
	string fn = file_root + string("/") + skel_id + string("/") + skel_id + string(".asf");
	return fn;
}

string DataManager::buildMotionFilename(const string& skel_id, const string& motion_id)
{
	string fn = file_root + string("/") + skel_id + string("/") + skel_id 
		+ string("_") + motion_id + string(".amc");
	return fn;
}

SkeletonDefinition* DataManager::loadSkeletonFromFile(const string& skel_id)
{
	ASF_Reader asf_reader;
	string fn = buildSkeletonFilename(skel_id);
	if (!FileSystem::fileExists(fn)) return NULL;
	SkeletonDefinition* skel = asf_reader.readASF(fn.c_str());
	if (skel != NULL)
	{
		skel->setId(skel_id);
		// if skeleton is in file index, override description and source.
		string description;
		string filename;
		if (data_index->getASFData(skel_id, description, filename)) 
		{
			skel->clearDocumentation();
			skel->addDocumentation(description);
			skel->setSource(string("CMU ASF: ") + skel_id);
		}
		return skel;
	}
	return NULL;
}

SkeletonDefinition* DataManager::loadSkeletonFromDatabase(const string& skel_id)
{
	if (database_loader == NULL) return NULL;
	return database_loader->loadSkeleton(skel_id);
}

SkeletonDefinition* DataManager::loadSkeleton(const string& skel_id)
{
	if (use_database)
	{
		SkeletonDefinition* skel = loadSkeletonFromDatabase(skel_id);
		if (skel != NULL)
		{
			logout << "skeleton " << skel_id << " loaded from database." << endl;
			return skel;
		}
	}
	SkeletonDefinition* skel = loadSkeletonFromFile(skel_id);
	if (skel != NULL)
	{
		logout << "skeleton " << skel_id << " loaded from file." << endl;
		return skel;
	}
	logout << "Could not load skeleton " << skel_id << "." << endl;
	return NULL;
}

MotionSequence* DataManager::loadMotionFromFile(const string& skel_id, const string& motion_id, SkeletonDefinition* skel)
{
	if (skel == NULL) return NULL;
	AMC_Reader amc_reader;
	MotionSequence* ms = NULL;

	string fn = buildMotionFilename(skel_id, motion_id);
	if (!FileSystem::fileExists(fn)) return NULL;
	ms = amc_reader.readAMC(fn.c_str(), skel);

	string description;
	string framerate;
	string filename;
	if (data_index->getAMCData(skel_id, motion_id, description, framerate, filename)) 
	{
		ms->clearDocumentation();
		ms->addDocumentation(description);
		ms->setSource(string("CMU AMC: ") + skel_id + string(".") + motion_id);
		ms->setFrameRate((float)atof(framerate.c_str()));
	}
	return ms;
}

MotionSequence* DataManager::loadMotionFromDatabase(const string& skel_id, const string& motion_id, SkeletonDefinition* skel)
{
	if (database_loader == NULL) return NULL;
	if (skel == NULL) return NULL;
	MotionSequence* ms = database_loader->loadMotion(skel_id, motion_id, skel);
	return ms;
}

MotionSequence* DataManager::loadMotion(const string& skel_id, const string& motion_id, SkeletonDefinition* skel)
{
	MotionSequence* ms = NULL;
	if (skel == NULL) 
	{
		logout << "No skeleton supplied for loading motion " << motion_id << endl;
		return NULL;
	}
	if (use_database)
	{
		ms = loadMotionFromDatabase(skel_id, motion_id, skel);
		if (ms != NULL) 
		{
			logout << "Motion " << skel_id << "." << motion_id << " loaded from database. " << endl;
			return ms;
		}
	}
	ms = loadMotionFromFile(skel_id, motion_id, skel);
	if (ms != NULL) 
	{
		logout << "Motion " << skel_id << "." << motion_id << " loaded from file. " << endl;
		return ms;
	}
	logout << "Motion " << skel_id << "." << motion_id << " could not be loaded. " << endl;
	return NULL;
}
/*
Character* DataManager::loadCharacter(const string& skel_id, const string& motion_id, SkeletonDefinition* skel)
{
	SkeletonDefinition* skel = loadSkeleton(skel_id);
	if (skel == NULL) return NULL;
	MotionSequence* ms = loadMotion(skel, motion_id);
	if (ms == NULL) { delete skel; return NULL;	}

	string skel_description, motion_description;
	string framerate;
	string skel_file, motion_file;
	
	if (!data_index->getASFData(skel_id, skel_description, skel_file))
		skel_description = string("UNDEFINED");
	if (!data_index->getAMCData(skel_id, motion_id,	motion_description, framerate, motion_file)) 
		motion_description = string("UNDEFINED");

	Character* character = new Character;
	character->skeleton_definition = skel;
	character->motion_sequence = ms;

	character->description1 = string("ASF ") + skel_id + string(": ") + skel_description;
	character->description2 = string("AMC ") + skel_id + string(".") + motion_id
		+ string(": ") + motion_description 
		+ " (" + toString<int>(character->motion_sequence->numFrames()) + " frames)";

	return character;	
}
*/

// FIXIT! This seems to be broken.
bool DataManager::getDescriptions(const string& skel_id, const string& motion_id, 
								  string& skel_description, string& motion_description)
{
	bool status = true;

	string skel_desc, motion_desc;
	string framerate;
	string skel_file, motion_file;
	
	if (!data_index->getASFData(skel_id, skel_description, skel_file))
	{
		skel_desc = string("UNDEFINED");
		status = false;
	}
	if (!data_index->getAMCData(skel_id, motion_id,	motion_description, framerate, motion_file)) 
	{
		motion_desc = string("UNDEFINED");
		status = false;
	}

	skel_description = string("ASF ") + skel_id + string(": ") + skel_desc;
	motion_description = string("AMC ") + skel_id + string(".") + motion_id
		+ string(": ") + motion_desc;

	return status;	
}

bool DataManager::writeSkeletonToFile(SkeletonDefinition* skel)
{
	string dn = buildSkeletonDirname(skel->getId());
	if (!FileSystem::makeDir(dn)) return false;
	string fn = buildSkeletonFilename(skel->getId());
	ASF_Writer asf_writer;
	return asf_writer.writeASF(fn.c_str(), skel);
}

bool DataManager::writeMotionToFile(SkeletonDefinition* skel, MotionSequence* ms)
{
	string dn = buildSkeletonDirname(skel->getId());
	if (!FileSystem::makeDir(dn)) return false;
	string fn = buildMotionFilename(skel->getId(), ms->getId());
	AMC_Writer amc_writer;
	return amc_writer.writeAMC(fn.c_str(), skel, ms);
}

void DataManager::copyDataFromFilesToDatabase()
{
	if (database == NULL) return;
	if (!database->connectionIsOpen()) return;

	string last_ASF_name = string("none");
	SkeletonDefinition* skel = NULL;

	vector<pair <string, string> > motions;
	data_index->getAllIndexedMotions(motions);
/*
	// loop to copy all skeletons
	for (unsigned int i=0; i<motions.size(); i++)
	{
		string asf = motions[i].first;
		cout << "asf ---" << asf << "---" << endl;
		string fn, asf_desc, asf_filename;
		try
		{
			data_index->getASFData(asf, asf_desc, asf_filename);
			if (asf != last_ASF_name)
			{
				SkeletonDefinition* skel = loadSkeletonFromFile(asf);
				if (skel != NULL)
				{
					skel->setId(asf);
					skel->clearDocumentation();
					skel->addDocumentation(asf_desc);
					skel->setSource(string("CMU mocap ") + asf);
					logout << "storing skeleton " << asf << " to database " << endl;
					system_timer.elapsedTime();
					database_loader->storeSkeleton(skel);
					logout << asf << " data storage time " << system_timer.elapsedTime() << endl;
					delete skel;
				}
				last_ASF_name = asf;
			}

		}
		catch (DataManagementException& e)
		{
			logout << "data management exception : " << e.msg 
				<< " skeleton " << asf << " database storage aborted" << endl;
		}
	}
*/
	// loop to copy motions
	for (unsigned int i=0; i<motions.size(); i++)
	{
		string asf = motions[i].first;
		string amc = motions[i].second;
		cout << "---" << asf << "." << amc << "---" << endl;
		string fn, asf_desc, asf_filename, amc_desc, amc_filename, amc_framerate;
		try
		{
			data_index->getASFData(asf, asf_desc, asf_filename);
			data_index->getAMCData(asf, amc, amc_desc, amc_framerate, amc_filename);
			//if ((asf == string("01")) || (asf == string("141")) || (asf == string("142")) || (asf == string("143")))
			if (asf == string("01"))
			{

			if (asf != last_ASF_name)
			{
				if (skel != NULL) delete skel;
				skel = loadSkeleton(asf);
				last_ASF_name = asf;
			}

			if (skel != NULL)
			{
				MotionSequence* ms = loadMotionFromFile(asf, amc, skel);
				if (ms != NULL)
				{
					logout << "storing motion " << asf << "." << amc << endl;
					system_timer.elapsedTime();
					ms->setId(amc);
					ms->setFrameRate((float)atof(amc_framerate.c_str()));				
					ms->addDocumentation(amc_desc);
					ms->setSource(string("CMU mocap ")+asf+string(".")+amc);
					database_loader->storeMotion(skel, ms);
					cout << "stored " << asf << "." << amc << endl;
					logout << asf << "." << amc << " data storage time " << system_timer.elapsedTime() << endl;
				}
			}
			}
		}
		catch (DataManagementException& e)
		{
			logout << "no skeleton " << asf << " is defined" << endl;
			logout << "exception : " << e.msg << endl;
		}
	}
	
	cout << "!!!!! database loading complete" << endl;
}
