#pragma once
#include <memory>
#include <ppltasks.h>
#include "DBContext.h"

namespace LocalDB {
	namespace SchemaDefinition {
		class SchemaBuilder;
	}

	// base class for initiating / upgrading a database schema
	class SchemaBase : public std::enable_shared_from_this<SchemaBase>
	{
	private:
		DBContext _dbContext;
	protected:
		SchemaBase(const DBContext& dbContext);
		// called to build the schema definition
		virtual void BuildSchema(SchemaDefinition::SchemaBuilder& builder) = 0;
#pragma warning (disable : 4100 )
		// called before schema deployment 
		virtual void BeforeDeploy(int fromVersion, int toVersion, sqlite3* db) {};
		// called after schema deployment 
		virtual void AfterDeploy(int fromVersion, int toVersion, sqlite3* db) {};
#pragma warning (default : 4100 )
	public:
		// initialize / upgrade the database schema
		concurrency::task<void> initializeAsync(concurrency::cancellation_token cancelToken);
		// generate entity classes and insert / update DBQueies
		std::string generateEntitiesClasses(const std::vector<std::string>& nameSpace);
		virtual ~SchemaBase() {}
	};

}