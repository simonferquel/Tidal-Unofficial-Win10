#pragma once
#include <vector>
#include <cpprest/json.h>
#include <cstdint>
#include "JsonHelpers.h"
namespace api {
	template<typename T>
	struct PaginatedList {
		std::int32_t limit;
		std::int64_t offset;
		std::int64_t totalNumberOfItems;
		std::vector<T> items;

		PaginatedList() = default;
		PaginatedList(const web::json::value& json) {
			parseJson(&json, L"limit", limit);
			parseJson(&json, L"offset", offset);
			parseJson(&json, L"totalNumberOfItems", totalNumberOfItems);
			const web::json::array* itemsArr;
			if (parseJson(&json, L"items", itemsArr)) {
				for (auto& item : *itemsArr) {
					items.emplace_back(item);
				}
			}
			
		}
	};
}