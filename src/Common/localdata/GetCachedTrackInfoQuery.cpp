#include "pch.h"
#include "GetCachedTrackInfoQuery.h"
using namespace localdata;

std::string localdata::GetCachedTrackInfoQuery::identifier()
{
	return "GetCachedTrackInfoQuery";
}

std::string localdata::GetCachedTrackInfoQuery::sql(int)
{
	return "select " + cached_track::getOrderedColumnsForSelect() + " from cached_track where id = @id";
}

localdata::cached_track localdata::GetCachedTrackInfoQuery::materialize(sqlite3 * db, sqlite3_stmt * statement)
{
	return cached_track::createFromSqlRecord(statement);
}

void localdata::GetCachedTrackInfoQuery::bindParameters(int statementIndex, sqlite3 * db, sqlite3_stmt * statement)
{
	sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@id"), _id);
}

std::string localdata::GetImportedTrackInfoQuery::identifier()
{
	return "GetImportedTrackInfoQuery";
}

std::string localdata::GetImportedTrackInfoQuery::sql(int statementIndex)
{
	return "select " + imported_track::getOrderedColumnsForSelect() + " from imported_track where id = @id";
}

imported_track localdata::GetImportedTrackInfoQuery::materialize(sqlite3 * db, sqlite3_stmt * statement)
{
	return imported_track::createFromSqlRecord(statement);
}

void localdata::GetImportedTrackInfoQuery::bindParameters(int statementIndex, sqlite3 * db, sqlite3_stmt * statement)
{
	sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@id"), _id);
}
