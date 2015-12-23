#pragma once
#include "QueryBase.h"
#include "SearchResult.h"
namespace api {
	class SearchQuery : public QueryBase {
	public:
		SearchQuery(Platform::String^ query, Platform::String^ types, int limit, int offset, Platform::String^ sessionId, Platform::String^ countryCode);
		SearchQuery(Platform::String^ query, Platform::String^ types, int limit, int offset, Platform::String^ countryCode);

		concurrency::task<std::shared_ptr<api::SearchResults>> executeAsync(concurrency::cancellation_token cancelToken);

		// Inherited via QueryBase
		virtual std::wstring url() const override;
	};
}