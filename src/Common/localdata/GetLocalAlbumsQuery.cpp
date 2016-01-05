#include "pch.h"
#include "GetLocalAlbumsQuery.h"
#include <localdb/DBQuery.h>
#include "db.h"

class GetLocalAlbumsQuery : public LocalDB::ResultSetDBQuery<localdata::imported_album> {
private:
	int _skip;
	int _take;
public:
	GetLocalAlbumsQuery(const LocalDB::DBContext& ctx, int skip, int take) : LocalDB::ResultSetDBQuery<localdata::imported_album>(ctx), _skip(skip), _take(take) {

	}
protected:

	// Inherited via ResultSetDBQuery
	virtual std::string identifier() override
	{
		return "GetLocalAlbumsQuery";
	}
	virtual std::string sql(int statementIndex) override
	{
		return "select " + localdata::imported_album::getOrderedColumnsForSelect() + " from imported_album order by title LIMIT @take OFFSET @skip";
	}
	virtual localdata::imported_album materialize(sqlite3 * db, sqlite3_stmt * statement) override
	{
		return localdata::imported_album::createFromSqlRecord(statement);
	}
	virtual void bindParameters(int statementIndex, sqlite3* db, sqlite3_stmt* statement) override {
		sqlite3_bind_int(statement, sqlite3_bind_parameter_index(statement, "@take"), _take);
		sqlite3_bind_int(statement, sqlite3_bind_parameter_index(statement, "@skip"), _skip);
	}
};


class GetLocalPlaylistsQuery : public LocalDB::ResultSetDBQuery<localdata::imported_playlist> {
private:
	int _skip;
	int _take;
public:
	GetLocalPlaylistsQuery(const LocalDB::DBContext& ctx, int skip, int take) : LocalDB::ResultSetDBQuery<localdata::imported_playlist>(ctx), _skip(skip), _take(take) {

	}
protected:

	// Inherited via ResultSetDBQuery
	virtual std::string identifier() override
	{
		return "GetLocalPlaylistsQuery";
	}
	virtual std::string sql(int statementIndex) override
	{
		return "select " + localdata::imported_playlist::getOrderedColumnsForSelect() + " from imported_playlist order by title LIMIT @take OFFSET @skip";
	}
	virtual localdata::imported_playlist materialize(sqlite3 * db, sqlite3_stmt * statement) override
	{
		return localdata::imported_playlist::createFromSqlRecord(statement);
	}
	virtual void bindParameters(int statementIndex, sqlite3* db, sqlite3_stmt* statement) override {
		sqlite3_bind_int(statement, sqlite3_bind_parameter_index(statement, "@take"), _take);
		sqlite3_bind_int(statement, sqlite3_bind_parameter_index(statement, "@skip"), _skip);
	}
};


class GetLocalTracksQuery : public LocalDB::ResultSetDBQuery<localdata::imported_track> {
private:
	int _skip;
	int _take;
public:
	GetLocalTracksQuery(const LocalDB::DBContext& ctx, int skip, int take) : LocalDB::ResultSetDBQuery<localdata::imported_track>(ctx), _skip(skip), _take(take) {

	}
protected:

	// Inherited via ResultSetDBQuery
	virtual std::string identifier() override
	{
		return "GetLocalTracksQuery";
	}
	virtual std::string sql(int statementIndex) override
	{
		return "select " + localdata::imported_track::getOrderedColumnsForSelect() + " from imported_track order by artist, album_title, title LIMIT @take OFFSET @skip";
	}
	virtual localdata::imported_track materialize(sqlite3 * db, sqlite3_stmt * statement) override
	{
		return localdata::imported_track::createFromSqlRecord(statement);
	}
	virtual void bindParameters(int statementIndex, sqlite3* db, sqlite3_stmt* statement) override {
		sqlite3_bind_int(statement, sqlite3_bind_parameter_index(statement, "@take"), _take);
		sqlite3_bind_int(statement, sqlite3_bind_parameter_index(statement, "@skip"), _skip);
	}
};

concurrency::task<std::shared_ptr<std::vector<localdata::imported_album>>> localdata::getImportedAlbumsAsync(int skip, int take, concurrency::cancellation_token cancelToken)
{
	return LocalDB::executeAsyncWithCancel<GetLocalAlbumsQuery>(getDb(), cancelToken, skip, take);
}

concurrency::task<std::shared_ptr<std::vector<localdata::imported_track>>> localdata::getImportedTracksAsync(int skip, int take, concurrency::cancellation_token cancelToken)
{
	return LocalDB::executeAsyncWithCancel<GetLocalTracksQuery>(getDb(), cancelToken, skip, take);
}

concurrency::task<std::shared_ptr<std::vector<localdata::imported_playlist>>> localdata::getImportedPlaylistsAsync(int skip, int take, concurrency::cancellation_token cancelToken)
{
	return LocalDB::executeAsyncWithCancel<GetLocalPlaylistsQuery>(getDb(), cancelToken, skip, take);
}
