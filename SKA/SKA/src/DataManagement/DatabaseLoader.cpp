//-----------------------------------------------------------------------------
// DatabaseLoader.cpp
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
#include "Core/SystemConfiguration.h"
#include "DataManagement/DatabaseLoader.h"
#include <cstdlib>
#include <string>
#include <vector>
using namespace std;
#include "DataManagement/DataIndex.h"
#include "DataManagement/AMC_Reader.h"
#include "DataManagement/Database.h"
#include "Core/SystemTimer.h"
#include "Core/Utilities.h"
#include "Animation/MotionSequence.h"

bool DatabaseLoader::skeletonExists(string& skel_id)
{
	if (database == NULL) return false;
	if (!database->connectionIsOpen()) return false;
	string sql;
	sql = "SELECT * FROM skeleton WHERE skel_id LIKE '" + skel_id + "';";
	DatabaseResult dbr = database->runQuery(sql.c_str());
	if ((dbr == 0) || (database->getResultNumRows(dbr) < 1))
		return false;
	return true;
}

bool DatabaseLoader::motionExists(string& skel_id, string& motion_id)
{
	if (database == NULL) return false;
	if (!database->connectionIsOpen()) return false;
	string sql;
	sql = "SELECT * FROM motion WHERE skel_id LIKE '" + skel_id + "' "
		+ "and motion_id LIKE '" + motion_id + "';";
	DatabaseResult dbr = database->runQuery(sql.c_str());
	if ((dbr == 0) || (database->getResultNumRows(dbr) < 1))
		return false;
	return true;
}

bool DatabaseLoader::deleteSkeleton(string& skel_id, bool delete_motions)
{
	if (database == NULL) return false;
	if (!database->connectionIsOpen()) return false;
	string sql;
	bool success = true;

	if (delete_motions)
	{
		sql = "DELETE FROM motion_data WHERE skel_id LIKE '" + skel_id + "';";
		if (database->runCommand(sql.c_str()) < 0) 
		{
			logout << "database command failed: " << sql << endl;
			success = false;
		}
		sql = "DELETE FROM motion WHERE skel_id LIKE '" + skel_id + "';";
		if (!database->runCommand(sql.c_str())) 
		{
			logout << "database command failed: " << sql << endl;
			success = false;
		}
	}

	sql = "DELETE FROM bone WHERE skel_id LIKE '" + skel_id + "';";
	if (!database->runCommand(sql.c_str())) 
	{
		logout << "database command failed: " << sql << endl;
		success = false;
	}
	sql = "DELETE FROM skeleton_hierarchy WHERE skel_id LIKE '" + skel_id + "';";
	if (!database->runCommand(sql.c_str())) 
	{
		logout << "database command failed: " << sql << endl;
		success = false;
	}
	sql = "DELETE FROM skeleton WHERE skel_id LIKE '" + skel_id + "';";
	if (!database->runCommand(sql.c_str())) 
	{
		logout << "database command failed: " << sql << endl;
		success = false;
	}
	return success;
}

bool DatabaseLoader::deleteMotion(string& skel_id, string& motion_id)
{
	if (database == NULL) return false;
	if (!database->connectionIsOpen()) return false;
	string sql;
	bool success = true;

	sql = "DELETE FROM motion_data WHERE skel_id LIKE '" + skel_id + "' "
		+ "and motion_id LIKE '" + motion_id + "';";
	if (database->runCommand(sql.c_str()) < 0) 
	{
		logout << "database command failed: " << sql << endl;
		success = false;
	}
	sql = "DELETE FROM motion WHERE skel_id LIKE '" + skel_id + "' "
		+ "and motion_id LIKE '" + motion_id + "';";
	if (!database->runCommand(sql.c_str())) 
	{
		logout << "database command failed: " << sql << endl;
		success = false;
	}
	return success;
}

bool DatabaseLoader::storeBone(BoneDefinition& bone, string& skeleton_name, int bone_id)
{
	if (database == NULL) return false;
	if (!database->connectionIsOpen()) return false;

	string sql;
	
	sql = "INSERT INTO bone VALUES (";
	sql += string("\'") + skeleton_name + string("\', ");
	sql += string("\'") + bone.name + string("\', ");
	sql += toString<int>(bone_id) + string(", ");
	sql += toString<float>(bone.length) + string(", ");
	sql += toString<float>(bone.direction.x) + string(", ");
	sql += toString<float>(bone.direction.y) + string(", ");	
	sql += toString<float>(bone.direction.z) + string(", ");
	sql += toString<float>(bone.axis.x) + string(", ");
	sql += toString<float>(bone.axis.y) + string(", ");	
	sql += toString<float>(bone.axis.z) + string(", ");

	sql += string("\'") + toString<DOF_ID>(bone.axis_order[0]) + string("\', ");
	sql += string("\'") + toString<DOF_ID>(bone.axis_order[1]) + string("\', ");
	sql += string("\'") + toString<DOF_ID>(bone.axis_order[2]) + string("\', ");
	
	sql += toString<bool>(bone.dof[0].valid) + string(", ");
	sql += toString<float>(bone.dof[0].min) + string(", ");	
	sql += toString<float>(bone.dof[0].max) + string(", ");
	sql += toString<bool>(bone.dof[1].valid) + string(", ");
	sql += toString<float>(bone.dof[1].min) + string(", ");	
	sql += toString<float>(bone.dof[1].max) + string(", ");
	sql += toString<bool>(bone.dof[2].valid) + string(", ");
	sql += toString<float>(bone.dof[2].min) + string(", ");	
	sql += toString<float>(bone.dof[2].max) + string(", ");
	sql += toString<bool>(bone.dof[3].valid) + string(", ");
	sql += toString<float>(bone.dof[3].min) + string(", ");	
	sql += toString<float>(bone.dof[3].max) + string(", ");
	sql += toString<bool>(bone.dof[4].valid) + string(", ");
	sql += toString<float>(bone.dof[4].min) + string(", ");	
	sql += toString<float>(bone.dof[4].max) + string(", ");
	sql += toString<bool>(bone.dof[5].valid) + string(", ");
	sql += toString<float>(bone.dof[5].min) + string(", ");	
	sql += toString<float>(bone.dof[5].max) + string(", ");	

	sql += string("\'") + toString<DOF_ID>(bone.channel_order[0]) + string("\', ");
	sql += string("\'") + toString<DOF_ID>(bone.channel_order[1]) + string("\', ");
	sql += string("\'") + toString<DOF_ID>(bone.channel_order[2]) + string("\', ");
	sql += string("\'") + toString<DOF_ID>(bone.channel_order[3]) + string("\', ");
	sql += string("\'") + toString<DOF_ID>(bone.channel_order[4]) + string("\', ");
	sql += string("\'") + toString<DOF_ID>(bone.channel_order[5]) + string("\'");

	sql += ");";

	logout << sql << endl;

	if (!database->runCommand(sql.c_str())) 
	{
		logout << "database command failed: " << sql << endl;
		return false;
	}

	return true;
}

bool DatabaseLoader::loadBone(BoneDefinition& bone, string& skeleton_name, int bone_id)
{
	if (database == NULL) return false;
	if (!database->connectionIsOpen()) return false;

	string sql = "SELECT * FROM bone WHERE skel_id = \'" + skeleton_name + "\' and " +
		" id = " + toString<int>(bone_id) + ";";
	//logout << sql << endl;

	DatabaseResult dbr = database->runQuery(sql.c_str());
	if ((dbr == 0) || (database->getResultNumRows(dbr) < 1))
	{
		logout << "database query failed" << endl;
		return false;
	}

	if (database->getResultNumRows(dbr) > 1)
		logout << "more than one bone " << bone_id << " for skeleton " << skeleton_name 
		<< " retrieved from database, using first one" << endl;

	vector<string> column_types;
	vector<string> column_names;
	vector<string> column_values;
	database->getResultColumnNames(dbr, column_names);
	database->getResultColumnTypes(dbr, column_types);
	database->getResultNextRow(dbr, column_values);
	database->releaseResult(dbr);
	//for (unsigned int i=0; i<column_values.size(); i++)
	//{
	//	logout << column_types[i] << " " << column_names[i] << " = " << column_values[i] << endl;
	//}

	//logout << "bone dump before db load: " << endl;
	//logout << (*this) << endl;

	int cx = 0;
	cx++; // skip skelton_name (column_values[0])
	bone.name = column_values[cx++];
	cx++; // skip id (column_values[2])
	bone.length = (float)atof(column_values[cx++].c_str());
	bone.direction = Vector3D(
		(float)atof(column_values[cx].c_str()),
		(float)atof(column_values[cx+1].c_str()),
		(float)atof(column_values[cx+2].c_str()));
	cx+=3;
	bone.axis = Vector3D(
		(float)atof(column_values[cx].c_str()),
		(float)atof(column_values[cx+1].c_str()),
		(float)atof(column_values[cx+2].c_str()));
	cx+=3;
	bone.axis_order[0] = atoDOF(column_values[cx++].c_str());
	bone.axis_order[1] = atoDOF(column_values[cx++].c_str());
	bone.axis_order[2] = atoDOF(column_values[cx++].c_str());
	for (int d=0; d<6; d++)
	{
		bone.dof[d].valid = atoi(column_values[cx++].c_str()) != 0;
		bone.dof[d].min = (float)atof(column_values[cx++].c_str());
		bone.dof[d].max = (float)atof(column_values[cx++].c_str());
	}
	for (int c=0; c<6; c++)
	{
		bone.channel_order[c] = atoDOF(column_values[cx++].c_str());
	}
	//logout << "bone dump after db load: " << endl;
	//logout << (*this) << endl;
	return true;
}

bool DatabaseLoader::storeSkeleton(SkeletonDefinition* skeleton)
{
	if (database == NULL) return false;
	if (!database->connectionIsOpen()) return false;

	string sql;
	sql = "INSERT INTO skeleton VALUES (";

	// documentation strings need to be cleaned/escaped of quotes
	sql += string("\'") + skeleton->skel_id + string("\', ");

	// next line should extracted from units attribute
	sql += string("\'") + string("unit") + string("\', "); // length_units
	// next line should extracted from units attribute
	sql += string("\'") + string("radian") + string("\', "); // angle_units


	sql += toString<int>(skeleton->num_bones) + string(", ");
	
	sql += toString<float>(skeleton->init_root_position.x) + string(", ");
	sql += toString<float>(skeleton->init_root_position.y) + string(", ");
	sql += toString<float>(skeleton->init_root_position.z) + string(", ");

	sql += toString<float>(skeleton->init_root_orientation.roll) + string(", ");
	sql += toString<float>(skeleton->init_root_orientation.pitch) + string(", ");
	sql += toString<float>(skeleton->init_root_orientation.yaw) + string(", ");

	sql += string("\'") + skeleton->documentation + string("\', ");
	sql += string("\'") + skeleton->source + string("\', ");

	sql += string("now()");
	
	sql += string(");");

	logout << sql << endl;

	if (!database->runCommand(sql.c_str()))
		logout << "database command failed: " << sql << endl;

	sql = "INSERT INTO skeleton_hierarchy VALUES (";
	list<pair<string,string> >::iterator hiter = skeleton->connections.begin();
	while (hiter != skeleton->connections.end())
	{
		sql = "INSERT INTO skeleton_hierarchy VALUES (";
		sql += "\'" + skeleton->skel_id + "\', ";
		sql += "\'" + (*hiter).first + "\', ";
		sql += "\'" + (*hiter).second + "\');";
		logout << sql << endl;
		if (!database->runCommand(sql.c_str())) 
			logout << "database command failed: " << sql << endl;
		hiter++;
	}

	for (int b=0; b<skeleton->num_bones; b++)
		storeBone(skeleton->bone_descriptions[b], skeleton->skel_id, b);

	return true;
}

SkeletonDefinition* DatabaseLoader::loadSkeleton(const string& skel_id)
{
	if (database == NULL) return false;
	if (!database->connectionIsOpen()) return false;

	string sql = "SELECT * FROM skeleton WHERE skel_id =\'" + skel_id + "\';";
	//logout << sql << endl;

	DatabaseResult dbr = database->runQuery(sql.c_str());
	if ((dbr == 0) || (database->getResultNumRows(dbr) < 1))
	{
		logout << "database query failed" << endl;
		return NULL;
	}

	if (database->getResultNumRows(dbr) > 1)
		logout << "more than one skeleton named " << skel_id << " retrieved from database, using first one" << endl;

	vector<string> column_values; // (per row)
	database->getResultNextRow(dbr, column_values);
	database->releaseResult(dbr);

	SkeletonDefinition* skeleton = new SkeletonDefinition;
	
	/*
	skel_id VARCHAR(20) UNIQUE,
	
	length_units VARCHAR(10),
	angle_units VARCHAR(10),
	num_bones SMALLINT,
	init_root_position_x FLOAT,
	init_root_position_y FLOAT,
	init_root_position_z FLOAT,
	init_root_orientation_r FLOAT,
	init_root_orientation_p FLOAT,
	init_root_orientation_y FLOAT,
	
	documentation VARCHAR(200),	
	source VARCHAR(50),		
	creation_data DATE
	*/
	int cx=0;
	skeleton->skel_id = column_values[cx++]; 
	cx++; // skip length_units 
	cx++; // skip angle_units
	skeleton->num_bones = atoi(column_values[cx++].c_str());
	skeleton->init_root_position = Vector3D(
		(float)atof(column_values[cx].c_str()), 
		(float)atof(column_values[cx+1].c_str()), 
		(float)atof(column_values[cx+2].c_str()));
	cx+=3;
	skeleton->init_root_orientation = Vector3D(
		(float)atof(column_values[cx].c_str()), 
		(float)atof(column_values[cx+1].c_str()), 
		(float)atof(column_values[cx+2].c_str()));
	cx+=3;
	skeleton->documentation = column_values[cx++];
	skeleton->source = column_values[cx++];
	// remaining: creation_date

	sql = "SELECT * FROM skeleton_hierarchy WHERE skel_id =\'" + skeleton->skel_id + "\';";
	//logout << sql << endl;

	dbr = database->runQuery(sql.c_str());
	if ((dbr == 0) || (database->getResultNumRows(dbr) < 1))
	{
		logout << "database query failed" << endl;
		delete skeleton;
		return NULL;
	}

	column_values.clear();
	int num_rows = database->getResultNumRows(dbr);
	skeleton->connections.clear();
	for (int i=0; i<num_rows; i++)
	{
		column_values.clear();
		database->getResultNextRow(dbr, column_values);
		skeleton->connections.push_back(pair<string,string>(column_values[1], column_values[2]));
		//for (unsigned int j=0; j<column_values.size(); j++)
		//{
		//	logout << column_types[j] << " " << column_names[j] << " = " << column_values[j] << endl;
		//}
	}
	database->releaseResult(dbr);

	// FIXIT! should probably delete any bones before loading new bones.

	for (int b=0; b<skeleton->num_bones; b++)
	{
		loadBone(skeleton->bone_descriptions[b], skeleton->skel_id, b);
		skeleton->bone_name_to_id.insert(pair<string,short>(skeleton->bone_descriptions[b].name, b));
		skeleton->bone_id_to_name.insert(pair<short,string>(b, skeleton->bone_descriptions[b].name));
	}

	return skeleton;
}

bool DatabaseLoader::storeMotion(SkeletonDefinition* skeleton, MotionSequence* ms)
{
	if (database == NULL) return false;
	if (!database->connectionIsOpen()) return false;

	string sql;
	sql = "INSERT INTO motion VALUES (";
	sql += string("\'") + skeleton->getId() + string("\', ");
	sql += string("\'") + ms->getId() + string("\', ");
	sql += toString<int>(ms->num_frames) + string(", ");
	sql += toString<float>(ms->frame_rate) + string(", ");
	sql += string("\'") + ms->documentation + string("\', ");
	sql += string("\'") + ms->source + string("\', ");
	sql += string("now()");
	sql += string(");");

	if (!database->runCommand(sql.c_str())) 
		logout << "database command failed: " << sql << endl;

	string v_sub1 = string("(")
			+ string("\'") + skeleton->getId() + string("\', ")
			+ string("\'") + ms->getId() + string("\', ");
	int num_channels = ms->getChannelIndexSize();
	CHANNEL_ID* channels = new CHANNEL_ID[num_channels];
	ms->getChannelList(channels, num_channels);
	for (int c=0; c<num_channels; c++)
	{
		BONE_ID b = channels[c].bone_id;
		string bone_name = skeleton->boneNameFromId(b);
		DOF_ID d = channels[c].dof_id;
		short col = ms->getChannelIndex(channels[c]);
		float* fdata = ms->data.getColumnPtr(col);

		string v_sub2 = 
			  string("\'") + bone_name + string("\', ")
			+ string("\'") + toString<DOF_ID>(d) + string("\', ");
/*
 skel_id   | varchar(20) | NO   | PRI |         |       |
 motion_id | varchar(20) | NO   | PRI |         |       |
 bone_id   | varchar(20) | NO   | PRI |         |       |
 dof       | varchar(2)  | NO   | PRI |         |       |
 frame     | int(11)     | NO   | PRI | 0       |       |
 value     | float       | YES  |     | NULL    |       |
*/
		sql = "INSERT INTO motion_data VALUES ";
		for (int frame=0; frame<ms->num_frames; frame++)
		{
			if (frame > 0) sql += string(",");
			// v_sub1 = <skel_id, motion_id>
			// v_sub2 = <bone_name, dof>
			sql += v_sub1 + v_sub2 + toString<int>(frame) + string (", ");
			sql += toString<float>(fdata[frame]);
			sql += string(")");
		}
		sql += ";";
		if (!database->runCommand(sql.c_str())) 
			logout << "database command failed: " << sql << endl;
	}
	logout << skeleton->getId() << "." << ms->getId() << " is " << num_channels*ms->num_frames << " rows" << endl;
	delete channels;
	return true;
}

MotionSequence* DatabaseLoader::loadMotion(const string& skel_id, const string& motion_id, SkeletonDefinition* skeleton)
{
	if (database == NULL) return NULL;
	if (!database->connectionIsOpen()) return NULL;

	string sql;
	sql = string("SELECT * FROM motion where ")
		+ string("skel_id like \'") + skel_id 
		+ string("\' and motion_id like \'") + motion_id + string("\';");

	DatabaseResult dbr = database->runQuery(sql.c_str());
	if ((dbr == 0) || (database->getResultNumRows(dbr) < 1))
	{
		logout << "database query failed: " << sql << endl;
		return NULL;
	}
	if (database->getResultNumRows(dbr) > 1)
		logout << "more than one motion " << motion_id << " for skeleton " 
		<< skeleton->getId() << " retrieved from database, using first one" << endl;

	MotionSequence* ms = new MotionSequence;

	vector<string> column_values;
	database->getResultNextRow(dbr, column_values);
	int cx = 1; // skip skel_id
	ms->motion_id = column_values[cx++];
	ms->num_frames = (int)atoi(column_values[cx++].c_str());
	ms->frame_rate = (float)atof(column_values[cx++].c_str());
	ms->documentation = column_values[cx++];
	ms->source = column_values[cx++];
	// remaining/unused attributes:
	//   creation_data DATE

	// build channel_index from skeleton information (or from database?)
	// FIXIT! The channels definitions should come from the database
	ms->buildChannelsFromSkeleton(skeleton);

	// resize data storage
	ms->adjustStorage();

	// loop query on frame from motion_data
	// motion_data
	//  skeleton_name VARCHAR(50),
	//  motion_name VARCHAR(50),
	//  frame INT,
	//  bone VARCHAR(50),
	//  dof VARCHAR(2),
	//  value FLOAT,
	int num_channels = ms->getChannelIndexSize();
	CHANNEL_ID* channels = new CHANNEL_ID[num_channels];
	ms->getChannelList(channels,num_channels);

	sql = string("select frame, value, bone_id, dof from motion_data where ")
		+ string("skel_id like \'") + skeleton->getId()
		+ string("\' and motion_id like \'") + motion_id
		+ string("\';");
	
	dbr = database->runQuery(sql.c_str());
	if ((dbr == 0) || (database->getResultNumRows(dbr) < 1))
	{
		logout << "database query failed: " << sql << endl;
		delete ms;
		return NULL;
	}
	int num_rows = database->getResultNumRows(dbr);
	for (int i=0; i<num_rows; i++)
	{
		int n;
		char** s = database->getResultNextRow(dbr, n);
		int frame = (int)atoi(s[0]);
		float value = (float)atof(s[1]);
		int b = skeleton->boneIdFromName(string(s[2]));
		DOF_ID d = atoDOF(s[3]);
		CHANNEL_ID c(b, d);
		short col = ms->getChannelIndex(c);
		ms->data.set(frame, col, value);
	}

	// initialize things not stored in database
	ms->duration = ms->num_frames/ms->frame_rate;

	return ms;
}
