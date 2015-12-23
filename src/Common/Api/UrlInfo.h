#pragma once
#include <string>
#include <cpprest/json.h>
namespace api {
	struct UrlInfo {
		std::wstring url;
		UrlInfo() = default;
		UrlInfo(const web::json::value& json);
	};
}