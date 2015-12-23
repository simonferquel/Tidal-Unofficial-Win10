#pragma once
#include <cpprest/json.h>
#include "ArtistInfo.h"
#include <chrono>
namespace api {
	struct PlaylistResume {
		std::chrono::system_clock::time_point created;
		ArtistInfo creator;
		std::wstring description;
		std::chrono::seconds duration;
		std::wstring image;
		std::chrono::system_clock::time_point lastUpdated;
		std::int32_t numberOfTracks;
		std::int32_t numberOfVideos;
		bool publicPlaylist;
		std::wstring title;
		std::wstring type;
		std::wstring url;
		std::wstring uuid;
		PlaylistResume() = default;
		PlaylistResume(const web::json::value& json);
	};
}
