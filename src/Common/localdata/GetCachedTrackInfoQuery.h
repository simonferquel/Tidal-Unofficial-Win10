#pragma once
#include <localdb/DBQuery.h>
#include "Entities.h"
namespace localdata {
	class GetCachedTrackInfoQuery : public LocalDB::ResultSetDBQuery<cached_track> {
	private:
		std::int64_t _id;
	protected:
		// Inherited via ResultSetDBQuery
		virtual std::string identifier() override;
		virtual std::string sql(int statementIndex) override;
		virtual cached_track materialize(sqlite3 * db, sqlite3_stmt * statement) override;
		virtual void bindParameters(int statementIndex, sqlite3* db, sqlite3_stmt* statement) override;
	public:
		GetCachedTrackInfoQuery(const LocalDB::DBContext& context, std::int64_t trackId) : LocalDB::ResultSetDBQuery<cached_track>(context), _id(trackId) {}
		
	};

	class GetImportedTrackInfoQuery : public LocalDB::ResultSetDBQuery<imported_track> {
	private:
		std::int64_t _id;
	protected:
		// Inherited via ResultSetDBQuery
		virtual std::string identifier() override;
		virtual std::string sql(int statementIndex) override;
		virtual imported_track materialize(sqlite3 * db, sqlite3_stmt * statement) override;
		virtual void bindParameters(int statementIndex, sqlite3* db, sqlite3_stmt* statement) override;
	public:
		GetImportedTrackInfoQuery(const LocalDB::DBContext& context, std::int64_t trackId) : LocalDB::ResultSetDBQuery<imported_track>(context), _id(trackId) {}

	};
}