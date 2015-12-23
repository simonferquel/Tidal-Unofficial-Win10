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
