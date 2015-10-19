#pragma once
#include <sqlite3.h>
#include <memory>
#include <SqliteException.h>
namespace LocalDB {
	// functor for destroying a sqlite statement
	struct SqliteStatementDestructor {
		void operator()(sqlite3_stmt* stmt) {
			if (stmt) {
				sqlite3_clear_bindings(stmt);
				sqlite3_finalize(stmt);
			}
		}
	};

	// sqlite statement unique_ptr
	using SqliteStatementUniquePtr = std::unique_ptr<sqlite3_stmt, SqliteStatementDestructor>;

	// prepare a sqlite statement and wrap it in a unique ptr
	inline SqliteStatementUniquePtr sqlite3_prepare_v2(
		sqlite3 *db,            /* Database handle */
		const char *zSql,       /* SQL statement, UTF-8 encoded */
		int nByte,              /* Maximum length of zSql in bytes. */
		const char **pzTail     /* OUT: Pointer to unused portion of zSql */
		) {
		sqlite3_stmt* tmp = nullptr;
		if (SQLITE_OK != sqlite3_prepare_v2(db, zSql, nByte, &tmp, pzTail)) {
			throw SqliteException(db);
		}
		return SqliteStatementUniquePtr(tmp);
	}

	// prepare a sqlite statement and wrap it in a shared ptr
	inline std::shared_ptr<sqlite3_stmt> sqlite3_prepare_v2_shared(
		sqlite3 *db,            /* Database handle */
		const char *zSql,       /* SQL statement, UTF-8 encoded */
		int nByte,              /* Maximum length of zSql in bytes. */
		const char **pzTail     /* OUT: Pointer to unused portion of zSql */
		) {
		sqlite3_stmt* tmp = nullptr;
		if (SQLITE_OK != sqlite3_prepare_v2(db, zSql, nByte, &tmp, pzTail)) {
			throw SqliteException(db);
		}
		return std::shared_ptr<sqlite3_stmt>(tmp,SqliteStatementDestructor());
	}

	// wraps an sqlite_statement in a smart pointer resetting it on destruction (for latter reuse)
	class auto_reset_statement_ptr {
	private:
		std::shared_ptr<sqlite3_stmt> _stmt;
	public:
		auto_reset_statement_ptr(const std::shared_ptr<sqlite3_stmt>& stmt) : _stmt(stmt) {}
		auto_reset_statement_ptr(const auto_reset_statement_ptr&) = delete;
		auto_reset_statement_ptr( auto_reset_statement_ptr&&) = default;
		auto_reset_statement_ptr& operator = (auto_reset_statement_ptr&& moved) {
			if (this == &moved) {
				return *this;
			}
			if (_stmt) {
				sqlite3_clear_bindings(_stmt.get());
				sqlite3_reset(_stmt.get());
			}
			_stmt = std::move(moved._stmt);
			return *this;
		}
		sqlite3_stmt * get() { return _stmt.get(); }
		~auto_reset_statement_ptr() {
			if (_stmt) {
				sqlite3_clear_bindings(_stmt.get());
				sqlite3_reset(_stmt.get());
			}
		}
	};
}