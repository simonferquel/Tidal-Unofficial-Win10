#pragma once
#include "Api/AlbumResume.h"
#include "Api/GetAlbumTracksQuery.h"
namespace albums {
	concurrency::task<std::shared_ptr<api::AlbumResume>> getAlbumResumeAsync(std::int64_t id, concurrency::cancellation_token cancelToken);
	concurrency::task<std::shared_ptr<api::PaginatedList<api::TrackInfo>>> getAlbumTracksAsync(std::int64_t id, int trackCount, concurrency::cancellation_token cancelToken);
}