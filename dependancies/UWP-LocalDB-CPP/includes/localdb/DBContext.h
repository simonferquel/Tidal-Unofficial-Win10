#pragma once
#include <memory>
#include <functional>
#include <sqlite3.h>
#include <ppltasks.h>
#include "SqliteSmartPtr.h"
namespace LocalDB {
	// represents a DB associated with a dedicated processing thread
	// this uses the pointer to shared implementation idiom
	class DBContext
	{
	private:
		class impl;
		std::shared_ptr<impl> _impl;
	public:
		// creates an instance of the DB context.
		// name : file name to use (will be stored in local data folder)
		DBContext(const wchar_t* name);
		// get an existing cached statement (or null if not already cached). When the returned smart pointer is destroyed, it unbinds any statement binding, and reset the statement for reuse
		// note : prefer to use DBQuery inherited class instead of direct use of statement cache
		// name : identifier of the statement
		auto_reset_statement_ptr getCachedStatement(const std::string& name);

		// register a cached statement (for avoiding unnecessary further sql statement compilation)
		// note : prefer to use DBQuery inherited class instead of direct use of statement cache
		// name : identifier of the statement
		// statement : prepared sqlite statement
		void registerCachedStatement(const std::string& name, const std::shared_ptr<sqlite3_stmt>& statement);

		// execute a job in the sqlite processing thread
		concurrency::task<void> executeAsync(std::function<void(sqlite3* db)>&& job);
		~DBContext();
	};
}

