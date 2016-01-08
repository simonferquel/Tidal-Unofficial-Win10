#include "pch.h"
#include "StorageStatistics.h"
#include <localdb/DBQuery.h>
#include "Entities.h"
#include "db.h"
#include <chrono>
#include <date.h>
using namespace localdata;

class GetTotalCacheSizeQuery : public LocalDB::IntegerScalarDBQuery {
public:
	GetTotalCacheSizeQuery(LocalDB::DBContext& ctx) : LocalDB::IntegerScalarDBQuery(ctx) {}
protected:

	// Inherited via IntegerScalarDBQuery
	virtual std::string identifier() override
	{
		return "GetTotalCacheSizeQuery";
	}
	virtual std::string sql(int statementIndex) override
	{
		return "SELECT SUM (local_size) FROM cached_track";
	}
};


class GetCacheSizeSinceQuery : public  LocalDB::IntegerScalarDBQuery {
private:
	std::int64_t _since;
public:
	GetCacheSizeSinceQuery(LocalDB::DBContext& ctx, std::int64_t since) : LocalDB::IntegerScalarDBQuery(ctx), _since(since) {}
protected:

	// Inherited via IntegerScalarDBQuery
	virtual std::string identifier() override
	{
		return "GetCacheSizeSinceQuery";
	}
	virtual std::string sql(int statementIndex) override
	{
		return "SELECT SUM (local_size) FROM cached_track where last_playpack_time < @since";
	}
	virtual void bindParameters(int statementIndex, sqlite3* db, sqlite3_stmt* statement) override{
		sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@since"), _since);
	};
};

class GetTotalImportSizeQuery : public  LocalDB::IntegerScalarDBQuery {
public:
	GetTotalImportSizeQuery(LocalDB::DBContext& ctx) : LocalDB::IntegerScalarDBQuery(ctx) {}
protected:

	// Inherited via IntegerScalarDBQuery
	virtual std::string identifier() override
	{
		return "GetTotalImportSizeQuery";
	}
	virtual std::string sql(int statementIndex) override
	{
		return "SELECT SUM (size) FROM imported_track";
	}
};

class GetImportSizeSinceQuery : public  LocalDB::IntegerScalarDBQuery {
private:
	std::int64_t _since;
public:
	GetImportSizeSinceQuery(LocalDB::DBContext& ctx, std::int64_t since) : LocalDB::IntegerScalarDBQuery(ctx), _since(since) {}
protected:

	// Inherited via IntegerScalarDBQuery
	virtual std::string identifier() override
	{
		return "GetImportSizeSinceQuery";
	}
	virtual std::string sql(int statementIndex) override
	{
		return "SELECT SUM (size) FROM imported_track where last_playpack_time < @since";
	}
	virtual void bindParameters(int statementIndex, sqlite3* db, sqlite3_stmt* statement) override {
		sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@since"), _since);
	};
};

concurrency::task<std::shared_ptr<storage_statistics>> localdata::getStorageStatisticsAsync(concurrency::cancellation_token cancelToken)
{
	auto result = std::make_shared<storage_statistics>();
	await getDb().executeAsync([result, cancelToken](sqlite3* db) {
		auto now = std::chrono::system_clock::now();
		auto since = now - date::days(8);
		result->totalCacheSize = LocalDB::executeSynchronouslyWithCancel<GetTotalCacheSizeQuery>(getDb(), db, cancelToken);
		result->totalImportSize = LocalDB::executeSynchronouslyWithCancel<GetTotalImportSizeQuery>(getDb(), db, cancelToken);
		result->cachedNotPlayedFor8DaysSize = LocalDB::executeSynchronouslyWithCancel<GetCacheSizeSinceQuery>(getDb(), db, cancelToken, since.time_since_epoch().count());
		result->importNotPlayedFor8DaysSize = LocalDB::executeSynchronouslyWithCancel<GetImportSizeSinceQuery>(getDb(), db, cancelToken, since.time_since_epoch().count());
	});
	return result;
}
