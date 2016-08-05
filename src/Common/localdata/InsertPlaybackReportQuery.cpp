#include "pch.h"
#include "InsertPlaybackReportQuery.h"
#include "db.h"

namespace localdata {
	class GetAllPlaybackReportsQuery : public LocalDB::ResultSetDBQuery<playback_reports> {
	private:
	protected:
		// Inherited via ResultSetDBQuery
		virtual std::string identifier() override {
			return "GetAllPlaybackReportsQuery";
		}
		virtual std::string sql(int statementIndex) override {
			return "SELECT " + playback_reports::getOrderedColumnsForSelect() + " FROM playback_reports";
		}
		virtual playback_reports materialize(sqlite3 * db, sqlite3_stmt * statement) override {
			return playback_reports::createFromSqlRecord(statement);
		}
	public:
		GetAllPlaybackReportsQuery(const LocalDB::DBContext& context) : LocalDB::ResultSetDBQuery<playback_reports>(context) {}

	};
	class DeletePlaybackReportsQuery : public LocalDB::NoResultDBQuery
	{
	private:
		std::int64_t _id;
	protected:
		virtual std::string identifier() override { return "DeletePlaybackReportsQuery"; }
		virtual std::string sql(int) override {
			return "delete from playback_reports where id = @id";
		}
		virtual  void bindParameters(int, sqlite3*, sqlite3_stmt* statement) override {
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@id"), _id);
		}
	public:
		DeletePlaybackReportsQuery(const LocalDB::DBContext& ctx, std::int64_t id) : LocalDB::NoResultDBQuery(ctx), _id(id) {}
	};

	concurrency::task<std::shared_ptr<std::vector<localdata::playback_reports>>> localdata::GetPlaybackReportsAsync()
	{
		return LocalDB::executeAsync<GetAllPlaybackReportsQuery>(getDb());
	}

	concurrency::task<void> localdata::AddPlaybackReportAsync(const std::wstring & json)
	{
		std::wstring localJson = json;
		co_await LocalDB::executeAsync<InsertPlaybackReportQuery>(getDb(), localJson);
	}

	concurrency::task<void> localdata::DeletePlaybackReportsAsync(const std::vector<std::int64_t>& ids)
	{
		std::vector<std::int64_t> localIds = ids;
		co_await getDb().executeAsync([&localIds](sqlite3* db) {
			LocalDB::SynchronousTransactionScope scope(getDb(), db);

			for (auto id : localIds) {
				LocalDB::executeSynchronously<DeletePlaybackReportsQuery>(getDb(), db, id);
			}

			scope.commit();
		});
	}

}