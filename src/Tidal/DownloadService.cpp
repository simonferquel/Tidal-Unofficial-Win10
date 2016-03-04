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
#include "PlaylistsDataSources.h"
#include <map>
#include "AuthenticationService.h"
#include "UnauthenticatedDialog.h"
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
	if (!getAuthenticationService().authenticationState().isAuthenticated()) {
		showUnauthenticatedDialog();
		return;
	}
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
		
		LocalDB::executeSynchronously<localdata::imported_albumInsertOrReplaceDbQuery>(localCtx, db, importedAlbum);
		for (auto&& ti : tracks->items) {
			localdata::track_import_job job;
			job.artist = ti.artists[0].name;
			job.cover = albumInfo->cover;
			job.id = ti.id;
			job.import_timestamp = std::chrono::system_clock::now().time_since_epoch().count();
			job.local_size = 0;
			job.owning_albumid = albumInfo->id;
			job.owning_playlistid = L"";
			job.server_size = 0;
			job.server_timestamp = 0;
			job.title = ti.title;
			job.obuscated = 1;

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

concurrency::task<void> DownloadService::StartDownloadPlaylistAsync(const std::wstring& idRef)
{
	if (!getAuthenticationService().authenticationState().isAuthenticated()) {
		showUnauthenticatedDialog();
		return;
	}
	std::wstring id = idRef;
	auto playlist = await playlists::getPlaylistAsync(id, concurrency::cancellation_token::none(), true);
	auto tracks = await playlists::getPlaylistTracksAsync(id, playlist->numberOfTracks, concurrency::cancellation_token::none(), true);
	std::map<std::int64_t, std::wstring> albumCovers;
	for (auto trk : tracks->items) {
		albumCovers.insert(std::make_pair(trk.album.id, trk.album.cover));
	}
	std::vector<concurrency::task<Platform::String^>> coversTasks;
	for (auto&& album : albumCovers) {
		coversTasks.push_back(api::EnsureCoverInCacheAsync(album.first, tools::strings::toWindowsString(album.second), concurrency::cancellation_token::none()));
		coversTasks.push_back(api::EnsureCoverInCacheAsync(album.first, tools::strings::toWindowsString(album.second), 80, 80, concurrency::cancellation_token::none()));
	}
	coversTasks.push_back(api::EnsurePlaylistCoverInCacheAsync(id, tools::strings::toWindowsString(playlist->image), 320, 214, concurrency::cancellation_token::none()));
	coversTasks.push_back(api::EnsurePlaylistCoverInCacheAsync(id, tools::strings::toWindowsString(playlist->image), 160, 107, concurrency::cancellation_token::none()));
	coversTasks.push_back(api::EnsurePlaylistCoverInCacheAsync(id, tools::strings::toWindowsString(playlist->image), 1080, 720, concurrency::cancellation_token::none()));
	for (auto& t : coversTasks) {
		await t;
	}


	auto ctx = localdata::getDb();
	await ctx.executeAsync([ctx, tracks, playlist](sqlite3* db) {
		auto localCtx = ctx;
		LocalDB::SynchronousTransactionScope trans(localCtx, db);
		localdata::imported_playlist importedPlaylist;
		importedPlaylist.cover = playlist->image;
		importedPlaylist.id = playlist->uuid;
		importedPlaylist.import_timestamp = std::chrono::system_clock::now().time_since_epoch().count();
		importedPlaylist.json = playlist->toJson().serialize();
		importedPlaylist.title = playlist->title;
		importedPlaylist.updated_at = playlist->lastUpdated.time_since_epoch().count();
		auto jarr = web::json::value::array();
		for (auto&& ti : tracks->items) {
			jarr[jarr.size()] = ti.toJson();
		}
		importedPlaylist.tracks_json = jarr.serialize();

		LocalDB::executeSynchronously<localdata::imported_playlistInsertOrReplaceDbQuery>(localCtx, db, importedPlaylist);
		for (auto&& ti : tracks->items) {
			localdata::track_import_job job;
			job.artist = ti.artists[0].name;
			job.cover = ti.album.cover;
			job.id = ti.id;
			job.import_timestamp = std::chrono::system_clock::now().time_since_epoch().count();
			job.local_size = 0;
			job.owning_albumid = -1;
			job.owning_playlistid = playlist->uuid;
			job.server_size = 0;
			job.server_timestamp = 0;
			job.title = ti.title;
			job.obuscated = 1;

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
