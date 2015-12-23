#pragma once
#include <vector>
#include "ArtistInfo.h"
#include <chrono>
#include <cpprest/json.h>
#include "AlbumResume.h"
namespace api {
	struct TrackInfo {
		AlbumResume album;
		bool allowStreaming;
		ArtistInfo artist;
		std::vector<ArtistInfo> artists;
		std::wstring copyright;
		std::chrono::seconds duration;
		bool is_explicit;
		std::int64_t id;
		double peak;
		double popularity;
		bool premiumStreamingOnly;
		double replayGain;
		bool streamReady;
		std::chrono::system_clock::time_point streamStartDate;
		std::wstring title;
		int trackNumber;
		std::wstring url;
		std::wstring version;
		int volumeNumber;
		TrackInfo() = default;
		TrackInfo(const web::json::value& json);
		web::json::value toJson() const;
	};
}