#pragma once
#include <string>
#include <cstdint>
#include <cpprest/json.h>
#include <chrono>
#include <date.h>
namespace api {
	struct SubscribtionInfo {
		std::chrono::system_clock::time_point validUntil;
		std::wstring status;
		std::wstring type;
		date::days offlineGracePeriod;
		std::wstring highestSoundQuality;
		bool premiumAccess;
		bool canGetTrial;
		SubscribtionInfo() = default;
		explicit SubscribtionInfo(const web::json::value& json);
	};
}