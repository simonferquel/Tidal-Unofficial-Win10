#include "pch.h"
#include "AlbumDataSources.h"
#include "AuthenticationService.h"
#include "Api/GetAlbumResumeQuery.h"
#include <localdata/GetImportedAlbum.h>
using namespace api;
concurrency::task<std::shared_ptr<api::AlbumResume>> albums::getAlbumResumeAsync(std::int64_t id, concurrency::cancellation_token cancelToken)
{
	auto imported = await localdata::getImportedAlbumIfExistsAsync(id, cancelToken);
	if (imported) {
	 	return std::make_shared<api::AlbumResume>(web::json::value::parse(imported->json));
	}

	auto& authSvc = getAuthenticationService();
	if (!authSvc.authenticationState().isAuthenticated()) {
		api::GetAlbumResumeQuery q(id, L"US");
		return await q.executeAsync(cancelToken);
	}
	else {
		api::GetAlbumResumeQuery q(id, authSvc.authenticationState().sessionId(), authSvc.authenticationState().countryCode());
		return await q.executeAsync(cancelToken);
	}
}

concurrency::task<std::shared_ptr<api::PaginatedList<api::TrackInfo>>> albums::getAlbumTracksAsync(std::int64_t id, int trackCount, concurrency::cancellation_token cancelToken)
{
	auto imported = await localdata::getImportedAlbumIfExistsAsync(id, cancelToken);
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
		api::GetAlbumTracksQuery q(id, trackCount, 0, L"US");
		return await q.executeAsync(cancelToken);
	}
	else {
		api::GetAlbumTracksQuery q(id, trackCount, 0, authSvc.authenticationState().sessionId(), authSvc.authenticationState().countryCode());
		return await q.executeAsync(cancelToken);
	}
}
