#pragma once
#include <string>
#include <cstdint>
#include <cpprest/json.h>
namespace api {
	struct LoginResult {
		std::int64_t userId;
		std::wstring sessionId;
		std::wstring countryCode;

		LoginResult() = default;
		explicit LoginResult(const web::json::value& json);
	};
}