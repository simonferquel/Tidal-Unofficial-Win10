#pragma once
#include <localdb/DBQuery.h>
#include "Entities.h"
namespace localdata {
	class GetTrackImportQueueQuery : public LocalDB::ResultSetDBQuery<track_import_job> {
	private:
	protected:
		// Inherited via ResultSetDBQuery
		virtual std::string identifier() override;
		virtual std::string sql(int statementIndex) override;
		virtual track_import_job materialize(sqlite3 * db, sqlite3_stmt * statement) override;
	public:
		GetTrackImportQueueQuery(const LocalDB::DBContext& context) : LocalDB::ResultSetDBQuery<track_import_job>(context) {}

	};
}