#include "pch.h"
#include "DownloadService.h"
#include <Api/CoverCache.h>
#include "AlbumDataSources.h"
#include <tools/StringUtils.h>
#include <localdata/db.h>
#include <localdata/entities.h>
#include <localdata/GetExistingTrackImportJobQuery.h>
#include "AudioService.h"
#include "Mediators.h"
DownloadService & getDownloadService()
{
	static DownloadService g_instance;
	return g_instance;
}

DownloadService::DownloadService()
{
}

concurrency::task<void> DownloadService::StartDownloadAlbumAsync(std::int64_t id)
{
	auto albumInfo = await albums::getAlbumResumeAsync(id, concurrency::cancellation_token::none());
	auto tracks = await albums::getAlbumTracksAsync(id, albumInfo->numberOfTracks, concurrency::cancellation_token::none());
	std::vector<concurrency::task<Platform::String^>> coversTasks;
	coversTasks.push_back(api::EnsureCoverInCacheAsync(albumInfo->id, tools::strings::toWindowsString(albumInfo->cover), concurrency::cancellation_token::none()));
	coversTasks.push_back(api::EnsureCoverInCacheAsync(albumInfo->id, tools::strings::toWindowsString(albumInfo->cover), 80, 80, concurrency::cancellation_token::none()));
	coversTasks.push_back(api::EnsureCoverInCacheAsync(albumInfo->id, tools::strings::toWindowsString(albumInfo->cover), 160, 160, concurrency::cancellation_token::none()));
	coversTasks.push_back(api::EnsureCoverInCacheAsync(albumInfo->id, tools::strings::toWindowsString(albumInfo->cover), 320, 320, concurrency::cancellation_token::none()));
	coversTasks.push_back(api::EnsureCoverInCacheAsync(albumInfo->id, tools::strings::toWindowsString(albumInfo->cover), 1080, 1080, concurrency::cancellation_token::none()));
	for (auto& t : coversTasks) {
		await t;
	}

	auto ctx = localdata::getDb();
	await ctx.executeAsync([ctx, tracks, albumInfo](sqlite3* db) {
		auto localCtx = ctx;
		LocalDB::SynchronousTransactionScope trans(localCtx, db);
		localdata::imported_album importedAlbum;
		importedAlbum.artist = albumInfo->artists[0].name;
		importedAlbum.cover = albumInfo->cover;
		importedAlbum.id = albumInfo->id;
		importedAlbum.import_timestamp = std::chrono::system_clock::now().time_since_epoch().count();
		importedAlbum.json = albumInfo->toJson().serialize();
		importedAlbum.title = albumInfo->title;
		auto jarr = web::json::value::array();
		for (auto&& ti : tracks->items) {
			jarr[jarr.size()] = ti.toJson();
		}
		importedAlbum.tracks_json = jarr.serialize();
		
		LocalDB::executeSynchronously<localdata::imported_albumInsertDbQuery>(localCtx, db, importedAlbum);
		for (auto&& ti : tracks->items) {
			localdata::track_import_job job;
			job.artist = ti.artists[0].name;
			job.cover = albumInfo->cover;
			job.id = ti.id;
			job.import_timestamp = std::chrono::system_clock::now().time_since_epoch().count();
			job.local_size = 0;
			job.owning_albumid = albumInfo->id;
			job.owning_playlistid = -1;
			job.server_size = 0;
			job.server_timestamp = 0;
			job.title = ti.title;

			auto existing = LocalDB::executeSynchronously<localdata::GetExistingTrackImportJobQuery>(localCtx, db, ti.id);
			if (existing->size() == 0) {
				LocalDB::executeSynchronously<localdata::track_import_jobInsertDbQuery>(localCtx, db, job);
			}
		}

		trans.commit();
	});
	await getAudioService().wakeupDownloaderAsync(concurrency::cancellation_token::none());
	getTrackImportLaunchedMediator().raise(true);
}
