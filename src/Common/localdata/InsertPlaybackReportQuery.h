#pragma once
#include "Entities.h"
#include <localdb\DBQuery.h>
#include <localdb\Sqlite3Extensions.h>
namespace localdata {
	class InsertPlaybackReportQuery : public LocalDB::NoResultDBQuery
	{
	private:
		std::wstring _json;
	protected:
		virtual std::string identifier() override { return "InsertPlaybackReportQuery"; }
		virtual std::string sql(int) override {
			return "insert into playback_reports(json) values (@json)";
		}
		virtual  void bindParameters(int, sqlite3*, sqlite3_stmt* statement) override {
			LocalDB::sqlite3_bind_string(statement, sqlite3_bind_parameter_index(statement, "@json"), _json);
		}
	public:
		InsertPlaybackReportQuery(const LocalDB::DBContext& ctx, const std::wstring& json) : LocalDB::NoResultDBQuery(ctx), _json(json) {}
	};
	concurrency::task<std::shared_ptr<std::vector<localdata::playback_reports>>> GetPlaybackReportsAsync();
	concurrency::task<void> AddPlaybackReportAsync(const std::wstring& json);
	concurrency::task<void> DeletePlaybackReportsAsync(const std::vector<std::int64_t>& ids);
}

