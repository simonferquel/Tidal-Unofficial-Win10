#pragma once
#include "ArtistInfo.h"
#include "QueryBase.h"
namespace api {
	class GetArtistInfoQuery : public QueryBase {
	private:
		std::int64_t _id;
	public:
		GetArtistInfoQuery(std::int64_t id, Platform::String^ sessionId, Platform::String^ countryCode);
		GetArtistInfoQuery(std::int64_t id, Platform::String^ countryCode);

		// Inherited via QueryBase
		virtual std::wstring url() const override {
			std::wstring result = L"artists/";
			result.append(std::to_wstring(_id));
			return result;
		}

		concurrency::task<std::shared_ptr<ArtistInfo>> executeAsync(concurrency::cancellation_token cancelToken);
	};
}