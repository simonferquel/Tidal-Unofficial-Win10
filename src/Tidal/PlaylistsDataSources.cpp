#include "pch.h"
#include "PlaylistsDataSources.h"
#include "AuthenticationService.h"
#include <Api/GetPlaylistQuery.h>
#include <localdata/GetImportedAlbum.h>

concurrency::task<std::shared_ptr<api::PlaylistResume>> playlists::getPlaylistAsync(const std::wstring & idRef, concurrency::cancellation_token cancelToken)
{
	std::wstring id = idRef;
	auto imported = await localdata::getImportedPlaylistIfExistsAsync(id, cancelToken);
	if (imported) {
		return std::make_shared<api::PlaylistResume>(web::json::value::parse(imported->json));
	}

	auto& authSvc = getAuthenticationService();
	if (!authSvc.authenticationState().isAuthenticated()) {
		api::GetPlaylistQuery q(id, L"US");
		return await q.executeAsync(cancelToken);
	}
	else {
		api::GetPlaylistQuery q(id, authSvc.authenticationState().sessionId(), authSvc.authenticationState().countryCode());
		return await q.executeAsync(cancelToken);
	}
}

concurrency::task<std::shared_ptr<api::PaginatedList<api::TrackInfo>>> playlists::getPlaylistTracksAsync(const std::wstring & idRef, int trackCount, concurrency::cancellation_token cancelToken)
{
	std::wstring id = idRef;
	auto imported = await localdata::getImportedPlaylistIfExistsAsync(id, cancelToken);
	if (imported) {
		auto jarr = web::json::value::parse(imported->tracks_json);
		auto result = std::make_shared<api::PaginatedList<api::TrackInfo>>();
		for (auto&& item : jarr.as_array()) {
			result->items.emplace_back(item);
		}
		return result;
	}

	auto& authSvc = getAuthenticationService();
	if (!authSvc.authenticationState().isAuthenticated()) {
		api::GetPlaylistTracksQuery q(id, trackCount, 0, L"US");
		return await q.executeAsync(cancelToken);
	}
	else {
		api::GetPlaylistTracksQuery q(id, trackCount, 0, authSvc.authenticationState().sessionId(), authSvc.authenticationState().countryCode());
		return await q.executeAsync(cancelToken);
	}
}
