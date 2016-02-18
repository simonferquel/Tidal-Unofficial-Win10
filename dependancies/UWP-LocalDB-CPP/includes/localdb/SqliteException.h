#pragma once
#include <exception>
#include <sqlite3.h>

namespace LocalDB {

	// exception wrapping a sqlite error code (sqlite error message available trough the what() method)
	class SqliteException : public std::exception{
	private:
		int _sqliteErrorCode;
	public:
		SqliteException(sqlite3* ctx) : std::exception(sqlite3_errmsg(ctx)), _sqliteErrorCode(sqlite3_errcode(ctx)) {
			
		}
		int errorCode() const {
			return _sqliteErrorCode;
		}
	};
}