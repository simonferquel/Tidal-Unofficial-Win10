#pragma once

#include <Api/PlaylistResume.h>
#include <Api/GetPlaylistTracksQuery.h>
namespace playlists {
	concurrency::task<std::shared_ptr<api::PlaylistResume>> getPlaylistAsync(const std::wstring& id, concurrency::cancellation_token cancelToken);
	concurrency::task<std::shared_ptr<api::PaginatedList<api::TrackInfo>>> getPlaylistTracksAsync(const std::wstring& id, int trackCount, concurrency::cancellation_token cancelToken);
}