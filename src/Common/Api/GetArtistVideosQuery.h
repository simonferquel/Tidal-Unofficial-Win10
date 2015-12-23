#pragma once
#include "QueryBase.h"
#include "VideoInfo.h"
#include "PaginatedList.h"
namespace api {
	class GetArtistVideosQuery : public QueryBase {
	private:
		std::int64_t _id;
	public:
		GetArtistVideosQuery(std::int64_t id, int limit, int offset, Platform::String^ sessionId, Platform::String^ countryCode);
		GetArtistVideosQuery(std::int64_t id, int limit, int offset, Platform::String^ countryCode);

		// Inherited via QueryBase
		virtual std::wstring url() const override;

		concurrency::task<std::shared_ptr<api::PaginatedList<api::VideoInfo>>> executeAsync(concurrency::cancellation_token cancelToken);
	};
}