#pragma once
#include <string>
#include <chrono>
#include <cpprest/json.h>
namespace api {
	struct PromotionArticle {
		std::wstring artifactId;
		std::chrono::system_clock::time_point created;
		bool featured;
		std::wstring group;
		std::wstring header;
		std::wstring imageId;
		std::wstring imageURL;
		std::wstring shortHeader;
		std::wstring shortSubHeader;
		std::wstring standaloneHeader;
		std::wstring subHeader;
		std::wstring text;
		std::wstring type;

		PromotionArticle() = default;
		PromotionArticle(const web::json::value& json);
	};
}