#pragma once
#include <cpprest/json.h>
#include "ArtistInfo.h"
#include <vector>
#include <chrono>
#include <date.h>
namespace api {
	struct AlbumResume {
		bool allowStreaming;
		ArtistInfo artist;
		std::vector<ArtistInfo> artists;
		std::wstring copyright;
		std::wstring cover;
		std::chrono::seconds duration;
		bool is_explicit;
		std::int64_t id;
		std::int32_t numberOfTracks;
		std::int32_t numberOfVolumes;
		bool premiumStreamingOnly;
		date::year_month_day releaseDate;
		bool streamReady;
		std::chrono::system_clock::time_point streamStartDate;
		std::wstring title;
		std::wstring type;
		std::wstring url;
		std::wstring version;
		AlbumResume();
		AlbumResume(const web::json::value& json);
		web::json::value toJson() const;
	};
}