#pragma once
#include "QueryBase.h"
#include "TrackInfo.h"
#include "PaginatedList.h"
namespace api {
	class GetTracksQueryBase : public QueryBase {
	public:
		GetTracksQueryBase(int limit, int offset, Platform::String^ sessionId, Platform::String^ countryCode);
		GetTracksQueryBase(int limit, int offset, Platform::String^ countryCode);


		concurrency::task<std::shared_ptr<PaginatedList<TrackInfo>>> executeAsync(concurrency::cancellation_token cancelToken);
	};
}