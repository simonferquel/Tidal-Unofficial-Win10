#pragma once
#include <chrono>
#include <cpprest/json.h>
namespace api {
	struct PlaybackReport {
		std::int64_t id;
		std::wstring quality;
		double deliveredDuration;
		std::chrono::system_clock::time_point playDate;
		std::wstring playlistUuid;
		web::json::value toJson() const;
	};
}