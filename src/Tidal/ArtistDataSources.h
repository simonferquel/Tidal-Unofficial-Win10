#pragma once
#include <Api/ArtistInfo.h>
#include <Api/GetArtistInfoQuery.h>
#include <Api/GetArtistPopularTracksQuery.h>
#include <Api/GetArtistAlbumsQuery.h>
#include <Api/GetArtistBioQuery.h>
namespace artists {
	concurrency::task<std::shared_ptr<api::ArtistInfo>> getArtistInfoAsync(std::int64_t id, concurrency::cancellation_token cancelToken);
	concurrency::task < std::shared_ptr<api::PaginatedList<api::TrackInfo>>> getArtistPopularTracksAsync(std::int64_t id, int trackCount, concurrency::cancellation_token cancelToken);
	concurrency::task < std::shared_ptr<api::PaginatedList<api::TrackInfo>>> getArtistRadioTracksAsync(std::int64_t id, int trackCount, concurrency::cancellation_token cancelToken);
	concurrency::task < std::shared_ptr<api::PaginatedList<api::AlbumResume>>> getArtistAlbumsAsync(std::int64_t id, concurrency::cancellation_token cancelToken, Platform::String^ filter = nullptr);
	concurrency::task<std::shared_ptr<api::ArtistBio>> getArtistBioAsync(std::int64_t id, concurrency::cancellation_token cancelToken);
}