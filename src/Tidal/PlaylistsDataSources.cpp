#include "pch.h"
#include "PlaylistsDataSources.h"
#include "AuthenticationService.h"
#include <Api/GetPlaylistQuery.h>

concurrency::task<std::shared_ptr<api::PlaylistResume>> playlists::getPlaylistAsync(const std::wstring & id, concurrency::cancellation_token cancelToken)
{
	auto& authSvc = getAuthenticationService();
	if (!authSvc.authenticationState().isAuthenticated()) {
		api::GetPlaylistQuery q(id, L"US");
		return q.executeAsync(cancelToken);
	}
	else {
		api::GetPlaylistQuery q(id, authSvc.authenticationState().sessionId(), authSvc.authenticationState().countryCode());
		return q.executeAsync(cancelToken);
	}
}

concurrency::task<std::shared_ptr<api::PaginatedList<api::TrackInfo>>> playlists::getPlaylistTracksAsync(const std::wstring & id, int trackCount, concurrency::cancellation_token cancelToken)
{
	auto& authSvc = getAuthenticationService();
	if (!authSvc.authenticationState().isAuthenticated()) {
		api::GetPlaylistTracksQuery q(id, trackCount, 0, L"US");
		return q.executeAsync(cancelToken);
	}
	else {
		api::GetPlaylistTracksQuery q(id, trackCount, 0, authSvc.authenticationState().sessionId(), authSvc.authenticationState().countryCode());
		return q.executeAsync(cancelToken);
	}
}
