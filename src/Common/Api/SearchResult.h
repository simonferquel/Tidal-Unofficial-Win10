#pragma once
#include <cpprest/json.h>
#include "ArtistInfo.h"
#include "AlbumResume.h"
#include "TrackInfo.h"
#include "VideoInfo.h"
#include "PlaylistResume.h"
#include "PaginatedList.h"
namespace api {
	struct SearchResults {
		api::PaginatedList<api::AlbumResume> albums;
		api::PaginatedList<api::ArtistInfo> artists;
		api::PaginatedList<api::PlaylistResume> playlists;
		api::PaginatedList<api::TrackInfo> tracks;
		api::PaginatedList<api::VideoInfo> videos;
		SearchResults() = default;
		SearchResults(const web::json::value& json);
	};
}