#pragma once
#include <string>
#include <cstdint>
#include <cpprest/json.h>
#include <chrono>
#include <date.h>
namespace api {
	struct UserInfo {
		std::int64_t id;
		std::wstring username;
		std::wstring firstName;
		std::wstring lastName;
		std::wstring email;
		std::chrono::system_clock::time_point created;
		std::wstring picture;
		bool newsletter;
		std::wstring gender;
		date::year_month_day dateOfBirth;
		std::int64_t facebookUid;
		UserInfo() = default;
		explicit UserInfo(const web::json::value& json);
	};
}