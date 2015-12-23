#pragma once
#include <localdb/DBQuery.h>
#include "Entities.h"
namespace localdata {
	class GetExistingTrackImportJobQuery : public LocalDB::ResultSetDBQuery<track_import_job> {
	private:
		std::int64_t _id;
	protected:
		// Inherited via ResultSetDBQuery
		virtual std::string identifier() override;
		virtual std::string sql(int statementIndex) override;
		virtual track_import_job materialize(sqlite3 * db, sqlite3_stmt * statement) override;
		virtual void bindParameters(int statementIndex, sqlite3* db, sqlite3_stmt* statement) override;
	public:
		GetExistingTrackImportJobQuery(const LocalDB::DBContext& context, std::int64_t trackId) : LocalDB::ResultSetDBQuery<track_import_job>(context), _id(trackId) {}

	};

	class CountExistingTrackImportJobsQuery : public LocalDB::IntegerScalarDBQuery {
	protected:


		// Inherited via IntegerScalarDBQuery
		virtual std::string identifier() override;

		virtual std::string sql(int statementIndex) override;
	public:
		CountExistingTrackImportJobsQuery(const LocalDB::DBContext& context) : LocalDB::IntegerScalarDBQuery(context) {}

	};
}