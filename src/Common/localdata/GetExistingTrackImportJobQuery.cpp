#include "pch.h"
#include "GetExistingTrackImportJobQuery.h"
using namespace localdata;
std::string localdata::GetExistingTrackImportJobQuery::identifier()
{
	return "GetExistingTrackImportJobQuery";
}

std::string localdata::GetExistingTrackImportJobQuery::sql(int statementIndex)
{
	return "select " + track_import_job::getOrderedColumnsForSelect() + " from track_import_job where id = @id";
}

track_import_job localdata::GetExistingTrackImportJobQuery::materialize(sqlite3 * db, sqlite3_stmt * statement)
{
	return track_import_job::createFromSqlRecord(statement);
}

void localdata::GetExistingTrackImportJobQuery::bindParameters(int statementIndex, sqlite3 * db, sqlite3_stmt * statement)
{
	sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@id"), _id);
}

std::string localdata::CountExistingTrackImportJobsQuery::identifier()
{
	return "CountExistingTrackImportJobsQuery";
}

std::string localdata::CountExistingTrackImportJobsQuery::sql(int statementIndex)
{
	return "select count(*) from track_import_job";
}
