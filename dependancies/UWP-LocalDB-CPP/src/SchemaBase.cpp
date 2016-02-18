#include "pch.h"
#include "SchemaBase.h"
#include "SqliteSmartPtr.h"
#include "SchemaBuilder.h"
#include "DBQuery.h"
#include <cstdint>
#include <SchemaBuilder.h>

using namespace LocalDB;
using namespace LocalDB::SchemaDefinition;
using namespace std;

class SchemaVersionTableExistsQuery : public DBQuery {
protected:
	// Inherited via DBQuery
	virtual std::string identifier() override
	{
		return "SchemaVersionTableExistsQuery"s;
	}
	virtual std::string sql(int /*index*/) override
	{
		return "SELECT count(*) FROM sqlite_master WHERE type = 'table' and name = 'schema_version_info'";
	}
public:
	SchemaVersionTableExistsQuery(const DBContext& context) : DBQuery(context) {}
	bool ExecuteSynchronously(sqlite3* db, concurrency::cancellation_token cancelToken) {
		std::int64_t value = 0;
		executeSynchronously(db, [](void*untypedParam, sqlite3* /*db*/, int /*statementIndex*/, sqlite3_stmt* stmt)
		{
			std::int64_t* pvalue = static_cast<std::int64_t*>(untypedParam);

			*pvalue = sqlite3_column_int64(stmt, 0);
		}, &value, cancelToken);
		return value > 0;
	}

};

class CreateSchemaVersionTableQuery : public DBQuery {
private:
	int _initialVersion;
protected:
	// Inherited via DBQuery
	virtual std::string identifier() override
	{
		return "CreateSchemaVersionTableQuery"s;
	}
	virtual std::string sql(int index) override
	{
		switch (index) {
		case 0:
			return "create table schema_version_info(version integer not null);";
		case 1:
			return "insert into schema_version_info(version) values(@initversion);";

		}
		return "";
	}
	virtual void bindParameters(int statementIndex, sqlite3* db, sqlite3_stmt* statement) override {
		if (statementIndex == 1) {
			auto paramIndex = sqlite3_bind_parameter_index(statement, "@initversion");
			if (SQLITE_OK != sqlite3_bind_int(statement, paramIndex, _initialVersion)) {
				throw SqliteException(db);
			}
		}
	}
	virtual int statementCount() override {
		return 2;
	}
public:
	CreateSchemaVersionTableQuery(const DBContext& context, int initialVersion) : DBQuery(context), _initialVersion(initialVersion) {}
	void ExecuteSynchronously(sqlite3* db, concurrency::cancellation_token cancelToken) {
		executeSynchronously(db, nullptr, nullptr, cancelToken);
	}

};

class GetSchemaVersionQuery : public DBQuery {
protected:
	// Inherited via DBQuery
	virtual std::string identifier() override
	{
		return "GetSchemaVersionQuery"s;
	}
	virtual std::string sql(int /*statementIndex*/) override
	{
		return "select version from schema_version_info";
	}
public:
	GetSchemaVersionQuery(const DBContext& context) : DBQuery(context) {}
	std::int32_t ExecuteSynchronously(sqlite3* db, concurrency::cancellation_token cancelToken) {
		std::int32_t version;
		executeSynchronously(db, [](void*untypedParam, sqlite3* /*db*/, int /*statementIndex*/, sqlite3_stmt* stmt)
		{
			std::int32_t* pVersion = static_cast<std::int32_t*>(untypedParam);
			*pVersion = sqlite3_column_int(stmt, 0);
		}, &version, cancelToken);
		return version;
	}

};


class SetSchemaVersionQuery : public DBQuery {
private:
	int _version;
protected:
	// Inherited via DBQuery
	virtual std::string identifier() override
	{
		return "SetSchemaVersionQuery"s;
	}
	virtual std::string sql(int /*statementIndex*/) override
	{
		return "update schema_version_info set version = @newversion";
	}
	virtual void bindParameters(int /*statementIndex*/, sqlite3* db, sqlite3_stmt* statement) override {
		auto paramIndex = sqlite3_bind_parameter_index(statement, "@newversion");
		if (SQLITE_OK != sqlite3_bind_int(statement, paramIndex, _version)) {
			throw SqliteException(db);
		}
	}

public:
	SetSchemaVersionQuery(const DBContext& context, int version) : DBQuery(context), _version(version) {}
	void ExecuteSynchronously(sqlite3* db, concurrency::cancellation_token cancelToken) {
		executeSynchronously(db, nullptr, nullptr, cancelToken);
	}

};

LocalDB::SchemaBase::SchemaBase(const DBContext& dbContext) : _dbContext(dbContext)
{
}

void executeNonqueryStatement(sqlite3* db, sqlite3_stmt* stmt) {
	for (;;) {
		auto result = sqlite3_step(stmt);
		if (result == SQLITE_DONE) {
			break;
		}
		else if (result != SQLITE_ROW) {
			throw SqliteException(db);
		}
	}
}

concurrency::task<void> LocalDB::SchemaBase::initializeAsync(concurrency::cancellation_token cancelToken)
{
	auto that = shared_from_this();
	return _dbContext.executeAsync([that, cancelToken](sqlite3* db) {
		SynchronousTransactionScope trans(that->_dbContext, db);
		SchemaDefinition::SchemaBuilder builder;
		that->BuildSchema(builder);
		auto targetVersion = maxVersion(builder.definition());
		bool versionTableJustCreated = false;
		// todo

		if (!executeSynchronouslyWithCancel<SchemaVersionTableExistsQuery>(that->_dbContext, db, cancelToken)) {
			executeSynchronouslyWithCancel<CreateSchemaVersionTableQuery>(that->_dbContext, db, cancelToken, targetVersion);
			versionTableJustCreated = true;
		}

		std::string tablesScript, indicesScript;

		auto currentVersion = versionTableJustCreated ? 0 : executeSynchronouslyWithCancel<GetSchemaVersionQuery>(that->_dbContext, db, cancelToken);

		if (targetVersion > currentVersion) {
			buildSql(tablesScript, indicesScript, builder.definition(), currentVersion, targetVersion);
			that->BeforeDeploy(currentVersion, targetVersion, db);

			char* msg = nullptr;
			auto deployResult = sqlite3_exec(db, tablesScript.c_str(), nullptr, nullptr, &msg);
			if (msg) {
				sqlite3_free(msg);
			}
			if (deployResult != SQLITE_OK) {
				throw SqliteException(db);
			}

			that->AfterDeploy(currentVersion, targetVersion, db);
			if (!versionTableJustCreated) {
				executeSynchronouslyWithCancel<SetSchemaVersionQuery>(that->_dbContext, db, cancelToken, targetVersion);
			}

		}

		trans.commit();
		if (indicesScript.size() > 0) {
			char* msg = nullptr;
			sqlite3_exec(db, indicesScript.c_str(), nullptr, nullptr, &msg);
			if (msg) {
				sqlite3_free(msg);
			}
		}


	});
}

std::string LocalDB::SchemaBase::generateEntitiesClasses(const std::vector<std::string>& nameSpace)
{
	SchemaDefinition::SchemaBuilder builder;
	BuildSchema(builder);
	return LocalDB::SchemaDefinition::generateEntitiesClasses(builder.definition(), nameSpace);
}

