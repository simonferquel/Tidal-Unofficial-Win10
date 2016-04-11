#include "pch.h"
#include "StorageStatistics.h"
#include <localdb/DBQuery.h>
#include "Entities.h"
#include "db.h"
#include <chrono>
#include <date.h>
#include "../LocalFileManager.h"
#include <experimental/coroutine>
#include <pplawait.h>
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

class DeleteCachedTrackQuery : public LocalDB::NoResultDBQuery {
private:
	std::int64_t _id;
public:
	DeleteCachedTrackQuery(LocalDB::DBContext& ctx, std::int64_t id) : LocalDB::NoResultDBQuery(ctx), _id(id){}
protected:
	virtual void bindParameters(int statementIndex, sqlite3* db, sqlite3_stmt* statement) override {
		sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@id"), _id);
	}
	// Inherited via IntegerScalarDBQuery
	virtual std::string identifier() override
	{
		return "DeleteCachedTrackQuery";
	}
	virtual std::string sql(int statementIndex) override
	{
		return "DELETE from cached_track where id = @id";
	}
};


class DeleteImportedTrackQuery : public LocalDB::NoResultDBQuery {
private:
	std::int64_t _id;
public:
	DeleteImportedTrackQuery(LocalDB::DBContext& ctx, std::int64_t id) : LocalDB::NoResultDBQuery(ctx), _id(id) {}
protected:
	virtual void bindParameters(int statementIndex, sqlite3* db, sqlite3_stmt* statement) override {
		sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@id"), _id);
	}
	// Inherited via IntegerScalarDBQuery
	virtual std::string identifier() override
	{
		return "DeleteImportedTrackQuery";
	}
	virtual std::string sql(int statementIndex) override
	{
		return "DELETE from imported_track where id = @id";
	}
};

class GetAllCachedTrackIdsQuery : public LocalDB::ResultSetDBQuery<std::int64_t> {
public:
	GetAllCachedTrackIdsQuery(LocalDB::DBContext& ctx) : LocalDB::ResultSetDBQuery<std::int64_t>(ctx) {}
protected:
	virtual std::string identifier() override
	{
		return "GetAllCachedTrackIdsQuery";
	}
	virtual std::string sql(int statementIndex) override
	{
		return "SELECT id FROM cached_track";
	}

	virtual std::int64_t materialize(sqlite3* db, sqlite3_stmt* statement) override {
		return sqlite3_column_int64(statement, 0);
	}
};
class GetAllImportedTrackIdsQuery : public LocalDB::ResultSetDBQuery<std::int64_t> {
public:
	GetAllImportedTrackIdsQuery(LocalDB::DBContext& ctx) : LocalDB::ResultSetDBQuery<std::int64_t>(ctx) {}
protected:
	virtual std::string identifier() override
	{
		return "GetAllImportedTrackIdsQuery";
	}
	virtual std::string sql(int statementIndex) override
	{
		return "SELECT id FROM imported_track";
	}

	virtual std::int64_t materialize(sqlite3* db, sqlite3_stmt* statement) override {
		return sqlite3_column_int64(statement, 0);
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


class GetCacheTracksNotPlayedSinceQuery : public LocalDB::ResultSetDBQuery<std::int64_t> {
private:
	std::int64_t _since;
public:
	GetCacheTracksNotPlayedSinceQuery(LocalDB::DBContext& ctx, std::int64_t since) : LocalDB::ResultSetDBQuery<std::int64_t>(ctx), _since(since) {}
protected:

	// Inherited via IntegerScalarDBQuery
	virtual std::string identifier() override
	{
		return "GetCacheTracksNotPlayedSinceQuery";
	}
	virtual std::string sql(int statementIndex) override
	{
		return "SELECT Id FROM cached_track where last_playpack_time < @since";
	}
	virtual void bindParameters(int statementIndex, sqlite3* db, sqlite3_stmt* statement) override {
		sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@since"), _since);
	};

	virtual std::int64_t materialize(sqlite3* db, sqlite3_stmt* statement) override {
		return sqlite3_column_int64(statement, 0);
	}
};
class GetImportTracksNotPlayedSinceQuery : public LocalDB::ResultSetDBQuery<std::int64_t> {
private:
	std::int64_t _since;
public:
	GetImportTracksNotPlayedSinceQuery(LocalDB::DBContext& ctx, std::int64_t since) : LocalDB::ResultSetDBQuery<std::int64_t>(ctx), _since(since) {}
protected:

	// Inherited via IntegerScalarDBQuery
	virtual std::string identifier() override
	{
		return "GetImportTracksNotPlayedSinceQuery";
	}
	virtual std::string sql(int statementIndex) override
	{
		return "SELECT Id FROM imported_track where last_playpack_time < @since";
	}
	virtual void bindParameters(int statementIndex, sqlite3* db, sqlite3_stmt* statement) override {
		sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@since"), _since);
	};

	virtual std::int64_t materialize(sqlite3* db, sqlite3_stmt* statement) override {
		return sqlite3_column_int64(statement, 0);
	}
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
	co_await getDb().executeAsync([result, cancelToken](sqlite3* db) {
		auto now = std::chrono::system_clock::now();
		auto since = now - date::days(8);
		result->totalCacheSize = LocalDB::executeSynchronouslyWithCancel<GetTotalCacheSizeQuery>(getDb(), db, cancelToken);
		result->totalImportSize = LocalDB::executeSynchronouslyWithCancel<GetTotalImportSizeQuery>(getDb(), db, cancelToken);
		result->cachedNotPlayedFor8DaysSize = LocalDB::executeSynchronouslyWithCancel<GetCacheSizeSinceQuery>(getDb(), db, cancelToken, since.time_since_epoch().count());
		result->importNotPlayedFor8DaysSize = LocalDB::executeSynchronouslyWithCancel<GetImportSizeSinceQuery>(getDb(), db, cancelToken, since.time_since_epoch().count());
	});
	return result;
}

concurrency::task<std::shared_ptr<storage_statistics>> localdata::clearCacheAndGetStorageStatisticsAsync()
{
	auto cacheFileMgr = co_await LocalFileManager::MakeLocalFileManager(false);
	auto ids = co_await LocalDB::executeAsync<GetAllCachedTrackIdsQuery>(getDb());
	for (auto id : *ids) {
		auto deleteResult = co_await cacheFileMgr->TryDeleteTrackFile(id);
		if (deleteResult != DeleteFileStatus::FileAccessProblem) {
			co_await LocalDB::executeAsync<DeleteCachedTrackQuery>(getDb(), id);
		}
	}
	return co_await getStorageStatisticsAsync(concurrency::cancellation_token::none());
}

concurrency::task<std::shared_ptr<storage_statistics>> localdata::clearImportsAndGetStorageStatisticsAsync()
{
	auto cacheFileMgr = co_await LocalFileManager::MakeLocalFileManager(true);
	auto ids = co_await LocalDB::executeAsync<GetAllImportedTrackIdsQuery>(getDb());
	for (auto id : *ids) {
		auto deleteResult = co_await cacheFileMgr->TryDeleteTrackFile(id);
		if (deleteResult != DeleteFileStatus::FileAccessProblem) {
			co_await LocalDB::executeAsync<DeleteImportedTrackQuery>(getDb(), id);
		}
	}
	return co_await getStorageStatisticsAsync(concurrency::cancellation_token::none());
}

concurrency::task<std::shared_ptr<storage_statistics>> localdata::clearCacheNotPlayedFor8DaysAngGetStorageStatisticsAsync()
{
	auto now = std::chrono::system_clock::now();
	auto since = now - date::days(8);
	auto cacheFileMgr = co_await LocalFileManager::MakeLocalFileManager(false);
	auto ids = co_await LocalDB::executeAsync<GetCacheTracksNotPlayedSinceQuery>(getDb(), since.time_since_epoch().count());
	for (auto id : *ids) {
		auto deleteResult = co_await cacheFileMgr->TryDeleteTrackFile(id);
		if (deleteResult != DeleteFileStatus::FileAccessProblem) {
			co_await LocalDB::executeAsync<DeleteCachedTrackQuery>(getDb(), id);
		}
	}
	return co_await getStorageStatisticsAsync(concurrency::cancellation_token::none());
}

concurrency::task<std::shared_ptr<storage_statistics>> localdata::clearImportsNotPlayedFor8DaysAngGetStorageStatisticsAsync()
{
	auto now = std::chrono::system_clock::now();
	auto since = now - date::days(8);
	auto cacheFileMgr = co_await LocalFileManager::MakeLocalFileManager(true);
	auto ids = co_await LocalDB::executeAsync<GetImportTracksNotPlayedSinceQuery>(getDb(), since.time_since_epoch().count());
	for (auto id : *ids) {
		auto deleteResult = co_await cacheFileMgr->TryDeleteTrackFile(id);
		if (deleteResult != DeleteFileStatus::FileAccessProblem) {
			co_await LocalDB::executeAsync<DeleteImportedTrackQuery>(getDb(), id);
		}
	}
	return co_await getStorageStatisticsAsync(concurrency::cancellation_token::none());
}
