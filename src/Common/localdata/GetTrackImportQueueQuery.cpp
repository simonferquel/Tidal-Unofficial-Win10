#include "pch.h"
#include "GetTrackImportQueueQuery.h"
using namespace localdata;

std::string localdata::GetTrackImportQueueQuery::identifier()
{
	return "GetTrackImportQueueQuery";
}

std::string localdata::GetTrackImportQueueQuery::sql(int statementIndex)
{
	return "select " + track_import_job::getOrderedColumnsForSelect() + " from track_import_job order by import_timestamp";
}

track_import_job localdata::GetTrackImportQueueQuery::materialize(sqlite3 * db, sqlite3_stmt * statement)
{
	return track_import_job::createFromSqlRecord(statement);
}
