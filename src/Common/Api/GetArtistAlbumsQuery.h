#pragma once

#include "QueryBase.h"
#include "AlbumResume.h"
#include "PaginatedList.h"
namespace api {
	class GetArtistAlbumsQuery : public QueryBase {
	private:
		std::int64_t _artistId;
	public:
		GetArtistAlbumsQuery(std::int64_t artistId, int limit, int offset, Platform::String^ sessionId, Platform::String^ countryCode);
		GetArtistAlbumsQuery(std::int64_t artistId, int limit, int offset, Platform::String^ countryCode);

		void addFilter(Platform::String^ filter);
		// Inherited via QueryBase
		virtual std::wstring url() const override;

		concurrency::task<std::shared_ptr<PaginatedList<AlbumResume>>> executeAsync(concurrency::cancellation_token cancelToken);
	};
}