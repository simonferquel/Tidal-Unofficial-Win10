#include "pch.h"
#include "ArtistDataSources.h"
#include "AuthenticationService.h"

concurrency::task<std::shared_ptr<api::ArtistInfo>> artists::getArtistInfoAsync(std::int64_t id, concurrency::cancellation_token cancelToken)
{
	auto& authSvc = getAuthenticationService();
	if (!authSvc.authenticationState().isAuthenticated()) {
		api::GetArtistInfoQuery q(id, L"US");
		return q.executeAsync(cancelToken);
	}
	else {
		api::GetArtistInfoQuery q(id, authSvc.authenticationState().sessionId(), authSvc.authenticationState().countryCode());
		return q.executeAsync(cancelToken);
	}
}

concurrency::task<std::shared_ptr<api::PaginatedList<api::TrackInfo>>> artists::getArtistPopularTracksAsync(std::int64_t id, int trackCount, concurrency::cancellation_token cancelToken)
{
	auto& authSvc = getAuthenticationService();
	if (!authSvc.authenticationState().isAuthenticated()) {
		api::GetArtistPopularTracksQuery q(id, trackCount, 0, L"US");
		return q.executeAsync(cancelToken);
	}
	else {
		api::GetArtistPopularTracksQuery q(id, trackCount, 0, authSvc.authenticationState().sessionId(), authSvc.authenticationState().countryCode());
		return q.executeAsync(cancelToken);
	}
}

concurrency::task<std::shared_ptr<api::PaginatedList<api::TrackInfo>>> artists::getArtistRadioTracksAsync(std::int64_t id, int trackCount, concurrency::cancellation_token cancelToken)
{
	auto& authSvc = getAuthenticationService();
	if (!authSvc.authenticationState().isAuthenticated()) {
		api::GetArtistRadioTracksQuery q(id, trackCount, 0, L"US");
		return q.executeAsync(cancelToken);
	}
	else {
		api::GetArtistRadioTracksQuery q(id, trackCount, 0, authSvc.authenticationState().sessionId(), authSvc.authenticationState().countryCode());
		return q.executeAsync(cancelToken);
	}
}

concurrency::task<std::shared_ptr<api::PaginatedList<api::AlbumResume>>> artists::getArtistAlbumsAsync(std::int64_t id, concurrency::cancellation_token cancelToken, Platform::String ^ filter)
{
	auto& authSvc = getAuthenticationService();
	if (!authSvc.authenticationState().isAuthenticated()) {
		api::GetArtistAlbumsQuery q(id, 999, 0, L"US");
		if (filter) {
			q.addFilter(filter);
		}
		return q.executeAsync(cancelToken);
	}
	else {
		api::GetArtistAlbumsQuery q(id, 999, 0, authSvc.authenticationState().sessionId(), authSvc.authenticationState().countryCode());

		if (filter) {
			q.addFilter(filter);
		}
		return q.executeAsync(cancelToken);
	}
}

concurrency::task<std::shared_ptr<api::ArtistBio>> artists::getArtistBioAsync(std::int64_t id, concurrency::cancellation_token cancelToken)
{
	auto& authSvc = getAuthenticationService();
	if (!authSvc.authenticationState().isAuthenticated()) {
		api::GetArtistBioQuery q(id, L"US");
		return q.executeAsync(cancelToken);
	}
	else {
		api::GetArtistBioQuery q(id, authSvc.authenticationState().sessionId(), authSvc.authenticationState().countryCode());
		return q.executeAsync(cancelToken);
	}
}
