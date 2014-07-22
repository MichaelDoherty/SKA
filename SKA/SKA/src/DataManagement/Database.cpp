//-----------------------------------------------------------------------------
// Database.cpp
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
#include "Core/SystemConfiguration.h"
#include "DataManagement/Database.h"
// MYSQL C API include files
#if ENABLE_MYSQL==1
#include <my_global.h>
#include <mysql.h>
#endif // ENABLE_MYSQL
#include <cstdlib>
#include <map>
using namespace std;

#if ENABLE_MYSQL==1
static MYSQL* connection = NULL;
static DatabaseResult next_result_id = 1;
static map<DatabaseResult, MYSQL_RES*> results;
static MYSQL_RES* findResult(DatabaseResult id)
{
	map<DatabaseResult, MYSQL_RES*>::iterator iter = results.find(id);
	if (iter == results.end()) return NULL;
	return (*iter).second;
}

bool Database::connectionIsOpen()
{
	return connection != NULL;
}

bool Database::open(const char* host, const char* user, const char* passwd, const char* database)
{
	close();
	connection = mysql_init(NULL);
	mysql_real_connect(connection, host, user, passwd, database, 0, NULL, 0);
	unsigned int error = mysql_errno(connection);
	if (error != 0)
	{
		close();
		return false;
	}
	return true;
}

void Database::close()
{
	if (connection != NULL) mysql_close(connection);
	connection = NULL;
}

int Database::runCommand(const char* command)
{
	mysql_query(connection, command);
	if (mysql_errno(connection) != 0) return -1;
	return (int)mysql_affected_rows(connection);
}

DatabaseResult Database::runQuery(const char* query)
{
	mysql_query(connection, query);
	if (mysql_errno(connection) != 0) return 0;
	MYSQL_RES* my_result = mysql_store_result(connection);
	DatabaseResult result_id = next_result_id++;
	results.insert(pair<DatabaseResult, MYSQL_RES*>(result_id, my_result));
	return result_id;
}

int Database::getResultColumnNames(DatabaseResult id, vector<string>& column_names)
{
	MYSQL_RES* r = findResult(id);
	if (r==NULL) return 0;

	int num_fields = mysql_num_fields(r);
	MYSQL_FIELD *fields = mysql_fetch_fields(r);
	for(int i=0; i<num_fields; i++)
	    column_names.push_back(string(fields[i].name));

	return num_fields;
}

int Database::getResultColumnTypes(DatabaseResult id, vector<string>& column_types)
{
	MYSQL_RES* r = findResult(id);
	if (r==NULL) return 0;

	int num_fields = mysql_num_fields(r);
	MYSQL_FIELD *fields = mysql_fetch_fields(r);
	for(int i=0; i<num_fields; i++)
	{
		switch (fields[i].type)
		{
		case MYSQL_TYPE_TINY:
		case MYSQL_TYPE_SHORT:
		case MYSQL_TYPE_LONG:
		case MYSQL_TYPE_INT24:
		case MYSQL_TYPE_LONGLONG:
			column_types.push_back(string("int")); 
			break;

		case MYSQL_TYPE_DECIMAL:
		case MYSQL_TYPE_NEWDECIMAL:
		case MYSQL_TYPE_FLOAT:
		case MYSQL_TYPE_DOUBLE:
			column_types.push_back(string("real")); 
			break;
		case MYSQL_TYPE_BIT:
		case MYSQL_TYPE_TIMESTAMP:
		case MYSQL_TYPE_DATE:
		case MYSQL_TYPE_TIME:
		case MYSQL_TYPE_DATETIME:
		case MYSQL_TYPE_YEAR:
		case MYSQL_TYPE_STRING:
		case MYSQL_TYPE_VAR_STRING:
		case MYSQL_TYPE_BLOB:
		case MYSQL_TYPE_SET:
		case MYSQL_TYPE_ENUM:
		case MYSQL_TYPE_GEOMETRY:
		case MYSQL_TYPE_NULL:
			column_types.push_back(string("string")); 
			break;
		default:
			column_types.push_back(string("unknown")); 
			break;
		}
	}
	return num_fields;
}

int Database::getResultNumRows(DatabaseResult id)
{
	MYSQL_RES* r = findResult(id);
	if (r==NULL) return 0;
	return (int)mysql_num_rows(r);
}

char** Database::getResultNextRow(DatabaseResult id, int& n)
{
	MYSQL_RES* r = findResult(id);
	if (r==NULL) { n = 0; return NULL; }
	n = mysql_num_fields(r);
	return mysql_fetch_row(r);
}

int Database::getResultNextRow(DatabaseResult id, vector<string>& values)
{
	MYSQL_RES* r = findResult(id);
	if (r==NULL) return 0;

	MYSQL_ROW row;
	int num_fields = mysql_num_fields(r);
	row = mysql_fetch_row(r);
	if (row == NULL) return 0;

	for (int i = 0; i < num_fields; i++)
	{
		if (row[i] == NULL) values.push_back(string(""));
		else values.push_back(string(row[i]));
	}

	return num_fields;
}

void Database::releaseResult(DatabaseResult id)
{
	MYSQL_RES* r = findResult(id);
	if (r==NULL) return;
	mysql_free_result(r);
	results.erase(id);
}

#else // ENABLE_MYSQL!= 1

bool Database::connectionIsOpen()
{
	return false;
}

bool Database::open(const char* host, const char* user, const char* passwd, const char* database)
{
	return false;
}

void Database::close()
{
}

int Database::runCommand(const char* command)
{
	return 0;
}

DatabaseResult Database::runQuery(const char* query)
{
	return -1;
}

int Database::getResultColumnNames(DatabaseResult id, vector<string>& column_names)
{
	return 0;
}

int Database::getResultColumnTypes(DatabaseResult id, vector<string>& column_types)
{
	return 0;	
}

int Database::getResultNumRows(DatabaseResult id)
{
	return 0;
}

char** Database::getResultNextRow(DatabaseResult id, int& n)
{
	return NULL;
}

int Database::getResultNextRow(DatabaseResult id, vector<string>& values)
{
	return 0;
}

void Database::releaseResult(DatabaseResult id)
{
}

#endif // ENABLE_MYSQL