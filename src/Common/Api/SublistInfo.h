#pragma once
#include <cpprest/json.h>
namespace api {
	struct SublistInfo {
		bool hasAlbums;
		bool hasArtists;
		bool hasPlaylists;
		bool hasTracks;
		bool hasVideos;
		std::wstring image;
		std::wstring name;
		std::wstring path;
		SublistInfo() = default;
		SublistInfo(const web::json::value& json);
	};
}