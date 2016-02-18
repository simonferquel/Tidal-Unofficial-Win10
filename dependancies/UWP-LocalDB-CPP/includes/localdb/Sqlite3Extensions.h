#pragma once
#include <string>
#include "DBContext.h"
#include <mutex>
#include "SqliteSmartPtr.h"
#include <cstdint>
#include <vector>
#include <chrono>
namespace LocalDB {
	
	// bind a wstring as a statement parameter
	inline int sqlite3_bind_string(sqlite3_stmt* stmt, int paramIndex, const std::wstring& value, bool nullIfEmpty = false) {
		if (nullIfEmpty && value.size() == 0) {
			return sqlite3_bind_null(stmt, paramIndex);
		}
		else {
			return sqlite3_bind_text16(stmt, paramIndex, &value[0], value.size() * sizeof(wchar_t), nullptr);
		}
	}

	// extract a wstring form a statement result row column
	inline std::wstring sqlite3_column_wstring(sqlite3_stmt* statement, int columnIndex) {
		return static_cast<const wchar_t*>(sqlite3_column_text16(statement, columnIndex));
	}

	// extract a std::chrono::seconds from a result row column
	inline std::chrono::seconds sqlite3_column_seconds(sqlite3_stmt* statement, int columnIndex) {
		return std::chrono::seconds(sqlite3_column_int64(statement, columnIndex));
	}

	// bind a std::chrono::seconds as a statement parameter
	inline int sqlite3_bind_seconds(sqlite3_stmt* stmt, int paramIndex, const std::chrono::seconds& value) {

		return sqlite3_bind_int64(stmt, paramIndex, value.count());

	}
	// extract a std::chrono::system_clock::time_point from a result row column
	inline std::chrono::system_clock::time_point sqlite3_column_timepoint(sqlite3_stmt* statement, int columnIndex) {
		return std::chrono::system_clock::time_point(std::chrono::system_clock::duration(sqlite3_column_int64(statement, columnIndex)));
	}

	// bind a std::chrono::system_clock::time_point as a statement parameter
	inline int sqlite3_bind_timepoint(sqlite3_stmt* stmt, int paramIndex, const std::chrono::system_clock::time_point& value) {

		return sqlite3_bind_int64(stmt, paramIndex, value.time_since_epoch().count());

	}

	// bind a string as a statement parameter
	inline int sqlite3_bind_string(sqlite3_stmt* stmt, int paramIndex, const std::string& value, bool nullIfEmpty = false) {
		if (nullIfEmpty && value.size() == 0) {
			return sqlite3_bind_null(stmt, paramIndex);
		}
		else {
			return sqlite3_bind_text(stmt, paramIndex, &value[0], value.size() * sizeof(char), nullptr);
		}
	}
	// extract a string form a statement result row column
	inline std::string sqlite3_column_string(sqlite3_stmt* statement, int columnIndex) {
		return std::string(reinterpret_cast<const char*>(sqlite3_column_text(statement, columnIndex)));
	}


}

