//-----------------------------------------------------------------------------
// DatabaseLoader.h
//	 Stores and loads skeletons and motion sequences to/from the database.
//   Will fail if database interface is disabled.
//   (See Database.{h,cpp} and ENABLE_MYSQL flag.)
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
#ifndef DATABASELOADER_DOT_H
#define DATABASELOADER_DOT_H
#include "Core/SystemConfiguration.h"
#include "Animation/BoneDefinition.h"
#include "Animation/SkeletonDefinition.h"
#include "Animation/MotionSequence.h"

class Database;

class DatabaseLoader
{
public:
	DatabaseLoader(Database* _database)
		: database(_database)
	{ }
	virtual ~DatabaseLoader()
	{ }
	void setDatabase(Database* _database) { database = _database; }

	bool storeSkeleton(SkeletonDefinition* skeleton);
	bool storeMotion(SkeletonDefinition* skeleton, MotionSequence* ms);

	SkeletonDefinition* loadSkeleton(const string& skel_id);
	// FIXIT! the skeleton parameter should not be necessary, that data could come from database.
	MotionSequence* loadMotion(const string& skel_id, const string& motion_id, SkeletonDefinition* skeleton);

	bool deleteSkeleton(string& skel_id, bool delete_motions=true);
	bool deleteMotion(string& skel_id, string& motion_id);

	bool skeletonExists(string& skel_id);
	bool motionExists(string& skel_id, string& motion_id);

private:
	bool loadBone(BoneDefinition& bone, string& skeleton_name, int bone_id);
	bool storeBone(BoneDefinition& bone, string& skeleton_name, int bone_id);

	Database *database;
};

#endif
