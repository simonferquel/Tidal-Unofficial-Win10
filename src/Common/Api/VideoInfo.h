#pragma once
#include <chrono>
#include <cpprest/json.h>
#include "ArtistInfo.h"
#include <vector>
namespace api {
	struct VideoInfo {
		ArtistInfo artist;
		std::vector<ArtistInfo> artists;
		std::chrono::seconds duration;
		std::int64_t id;
		std::wstring imageId;
		std::wstring imagePath;
		std::wstring quality;
		std::chrono::system_clock::time_point releasedDate;
		bool streamReady;
		std::chrono::system_clock::time_point streamStartDate;
		std::wstring title;
		VideoInfo() = default;
		VideoInfo(const web::json::value& json);
	};
}
