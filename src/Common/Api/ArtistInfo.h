#pragma once
#include <cpprest/json.h>
namespace api {
	struct ArtistInfo {
		std::int64_t id;
		std::wstring name;
		std::wstring type;
		std::wstring picture;
		ArtistInfo() = default;
		ArtistInfo(const web::json::value& json);

		web::json::value toJson() const;
	};
}