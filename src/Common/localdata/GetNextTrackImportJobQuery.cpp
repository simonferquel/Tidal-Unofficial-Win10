#include "pch.h"
#include "GetNextTrackImportJobQuery.h"
#include "GetExistingTrackImportJobQuery.h"
#include "GetCachedTrackInfoQuery.h"
#include <Api/GetTrackInfoQuery.h>
using namespace localdata;
std::string localdata::GetNextTrackImportJobQuery::identifier()
{
	return "GetNextTrackImportJobQuery";
}

std::string localdata::GetNextTrackImportJobQuery::sql(int statementIndex)
{
	return "select " + track_import_job::getOrderedColumnsForSelect() + " from track_import_job order by import_timestamp limit 1";
}

track_import_job localdata::GetNextTrackImportJobQuery::materialize(sqlite3 * db, sqlite3_stmt * statement)
{
	return track_import_job::createFromSqlRecord(statement);
}


class DeleteTrackImportJobCommand : public LocalDB::NoResultDBQuery {
private:
	std::int64_t _id;
protected:
	virtual std::string identifier() override;
	virtual std::string sql(int statementIndex) override;
	virtual void bindParameters(int statementIndex, sqlite3* db, sqlite3_stmt* statement) override;
public:
	DeleteTrackImportJobCommand(const LocalDB::DBContext& context, std::int64_t id) : LocalDB::NoResultDBQuery(context), _id(id) {}
};


std::string DeleteTrackImportJobCommand::identifier()
{
	return "DeleteTrackImportJobCommand";
}

std::string DeleteTrackImportJobCommand::sql(int statementIndex)
{
	return "delete from track_import_job where id = @id";
}

void DeleteTrackImportJobCommand::bindParameters(int statementIndex, sqlite3 * db, sqlite3_stmt * statement)
{
	sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@id"), _id);
}


class DeleteImportedTrackCommand : public LocalDB::NoResultDBQuery {
private:
	std::int64_t _id;
protected:
	virtual std::string identifier() override;
	virtual std::string sql(int statementIndex) override;
	virtual void bindParameters(int statementIndex, sqlite3* db, sqlite3_stmt* statement) override;
public:
	DeleteImportedTrackCommand(const LocalDB::DBContext& context, std::int64_t id) : LocalDB::NoResultDBQuery(context), _id(id) {}
};


std::string DeleteImportedTrackCommand::identifier()
{
	return "DeleteImportedTrackCommand";
}

std::string DeleteImportedTrackCommand::sql(int statementIndex)
{
	return "delete from imported_track where id = @id";
}

void DeleteImportedTrackCommand::bindParameters(int statementIndex, sqlite3 * db, sqlite3_stmt * statement)
{
	sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@id"), _id);
}


concurrency::task<void> localdata::deleteImportedTrackAsync(LocalDB::DBContext & context, std::int64_t id, concurrency::cancellation_token cancelToken)
{
	return LocalDB::executeAsyncWithCancel<DeleteImportedTrackCommand>(context, cancelToken, id);
}

concurrency::task<void> localdata::transformTrackImportJobToImportedTrackAsync(LocalDB::DBContext& context, std::int64_t id, concurrency::cancellation_token cancelToken)
{
	auto settingsValues = Windows::Storage::ApplicationData::Current->LocalSettings->Values;
	api::GetTrackInfoQuery tiQuery(id, dynamic_cast<Platform::String^>(settingsValues->Lookup(L"SessionId")), dynamic_cast<Platform::String^>(settingsValues->Lookup(L"CountryCode")));
	auto ti = await tiQuery.executeAsync(cancelToken);
	await context.executeAsync([id, context, cancelToken, ti](sqlite3* db) {
		auto ctx = context;
		LocalDB::SynchronousTransactionScope trans(ctx, db);
		auto jobset = LocalDB::executeSynchronouslyWithCancel<localdata::GetExistingTrackImportJobQuery>(ctx, db, cancelToken, id);
		auto job = jobset->at(0);
		localdata::imported_track trk;
		trk.album_id = ti->album.id;
		trk.artist = ti->artists[0].name;
		trk.cover = ti->album.cover;
		trk.id = id;
		trk.import_timestamp = job.import_timestamp;
		trk.json = ti->toJson().serialize();
		trk.last_playpack_time = 0;
		trk.quality = job.quality;
		trk.title = job.title;
		
		auto existing = LocalDB::executeSynchronouslyWithCancel<localdata::GetImportedTrackInfoQuery>(ctx, db, cancelToken, id);
		if (existing->size() == 0) {
			LocalDB::executeSynchronouslyWithCancel<localdata::imported_trackInsertDbQuery>(ctx, db, cancelToken, trk);
		}
		else {
			LocalDB::executeSynchronouslyWithCancel<localdata::imported_trackUpdateDbQuery>(ctx, db, cancelToken, trk);
		}

		LocalDB::executeSynchronouslyWithCancel<DeleteTrackImportJobCommand>(ctx, db, cancelToken, id);

		trans.commit();
	});
}
