#include "pch.h"
#include "GetImportedAlbum.h"
#include <localdb/DBQuery.h>
#include "db.h"
using namespace localdata;

class GetImportedAlbumIfExistsQuery : public LocalDB::ResultSetDBQuery<imported_album> {
private:
	std::int64_t _id;
protected:

	// Inherited via ResultSetDBQuery
	virtual std::string identifier() override
	{
		return "GetImportedAlbumIfExistsQuery";
	}
	virtual std::string sql(int statementIndex) override
	{
		return "select " + imported_album::getOrderedColumnsForSelect() + " from imported_album where id = @id";
	}
	virtual imported_album materialize(sqlite3 * db, sqlite3_stmt * statement) override
	{
		return imported_album::createFromSqlRecord(statement);
	}
	virtual void bindParameters(int statementIndex, sqlite3* db, sqlite3_stmt* statement) override {
		sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@id"), _id);
	};
public:
	GetImportedAlbumIfExistsQuery(const LocalDB::DBContext& ctx, std::int64_t id) : LocalDB::ResultSetDBQuery<imported_album>(ctx), _id(id) {}
	
};


class GetImportedPlaylistIfExistsQuery : public LocalDB::ResultSetDBQuery<imported_playlist> {
private:
	std::wstring _id;
protected:

	// Inherited via ResultSetDBQuery
	virtual std::string identifier() override
	{
		return "GetImportedPlaylistIfExistsQuery";
	}
	virtual std::string sql(int statementIndex) override
	{
		return "select " + imported_playlist::getOrderedColumnsForSelect() + " from imported_playlist where id = @id";
	}
	virtual imported_playlist materialize(sqlite3 * db, sqlite3_stmt * statement) override
	{
		return imported_playlist::createFromSqlRecord(statement);
	}
	virtual void bindParameters(int statementIndex, sqlite3* db, sqlite3_stmt* statement) override {
		LocalDB::sqlite3_bind_string(statement, sqlite3_bind_parameter_index(statement, "@id"), _id);
	};
public:
	GetImportedPlaylistIfExistsQuery(const LocalDB::DBContext& ctx, const std::wstring& id) : LocalDB::ResultSetDBQuery<imported_playlist>(ctx), _id(id) {}

};


concurrency::task<std::shared_ptr<imported_album>> localdata::getImportedAlbumIfExistsAsync(std::int64_t id, concurrency::cancellation_token cancelToken)
{
	auto resultSet = await LocalDB::executeAsyncWithCancel<GetImportedAlbumIfExistsQuery>(getDb(), cancelToken, id);
	if (resultSet->size() == 0) {
		return nullptr;
	}
	else {
		return std::make_shared<imported_album>(resultSet->at(0));
	}

}

concurrency::task<std::shared_ptr<imported_playlist>> localdata::getImportedPlaylistIfExistsAsync(const std::wstring & id, concurrency::cancellation_token cancelToken)
{
	auto resultSet = await LocalDB::executeAsyncWithCancel<GetImportedPlaylistIfExistsQuery>(getDb(), cancelToken, id);
	if (resultSet->size() == 0) {
		return nullptr;
	}
	else {
		return std::make_shared<imported_playlist>(resultSet->at(0));
	}
}
