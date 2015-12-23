#pragma once
#include "ArtistInfo.h"
#include "PaginatedList.h"
#include "QueryBase.h"
namespace api {
	class GetSimilarArtistsQuery : public QueryBase {
	private:
		std::int64_t _id;
	public:
		GetSimilarArtistsQuery(std::int64_t id, int limit, int offset, Platform::String^ sessionId, Platform::String^ countryCode);
		GetSimilarArtistsQuery(std::int64_t id, int limit, int offset, Platform::String^ countryCode);

		// Inherited via QueryBase
		virtual std::wstring url() const override {
			std::wstring result = L"artists/";
			result.append(std::to_wstring(_id));
			result.append(L"/similar");
			return result;
		}

		concurrency::task<std::shared_ptr<PaginatedList< ArtistInfo>>> executeAsync(concurrency::cancellation_token cancelToken);
	};
}