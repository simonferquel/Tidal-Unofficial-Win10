#pragma once
#include "QueryBase.h"
#include "AlbumResume.h"
namespace api {
	class GetAlbumResumeQuery : public QueryBase {
	private:
		std::int64_t _id;
	public:
		GetAlbumResumeQuery(std::int64_t id, Platform::String^ sessionId, Platform::String^ countryCode);
		GetAlbumResumeQuery(std::int64_t id, Platform::String^ countryCode);

		// Inherited via QueryBase
		virtual std::wstring url() const override;

		concurrency::task<std::shared_ptr<AlbumResume>> executeAsync(concurrency::cancellation_token cancelToken);
	};
}