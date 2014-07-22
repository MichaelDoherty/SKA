//-----------------------------------------------------------------------------
// Database.h
//	 Wrapper interface around MySQL database API
//   This class is non-functional if not linked to MySQL database API.
//   See ENABLE_MYSQL flag in Core/SystemConfiguration.h.
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
#ifndef DATABASE_DOT_H
#define DATABASE_DOT_H
#include "Core/SystemConfiguration.h"
#include <vector>
#include <string>
using namespace std;

// FIXIT! since Database.cpp uses static global variables,
//        there can currently only be one instance of a Database.

typedef unsigned int DatabaseResult;

class Database
{
public:
	Database() {}

	virtual ~Database()	{ close(); }

	bool connectionIsOpen();

	bool open(const char* host, const char* user, const char* passwd, const char* database);

	void close();

	// If command failed, return value is -1,
	// otherwise, return value is number of affected rows.
	int runCommand(const char* command);

	DatabaseResult runQuery(const char* query);
	int getResultColumnNames(DatabaseResult id, vector<string>& column_names);
	int getResultColumnTypes(DatabaseResult id, vector<string>& column_types);
	int getResultNumRows(DatabaseResult id);
	int getResultNextRow(DatabaseResult id, vector<string>& values);
	char** getResultNextRow(DatabaseResult id, int& n);
	void releaseResult(DatabaseResult id);

};

#endif
