#pragma once
#include <cpprest/json.h>
#include <chrono>
#include "QueryBase.h"
namespace api {
	struct ArtistBio {
		std::chrono::system_clock::time_point lastUpdated;
		std::wstring source;
		std::wstring summary;
		std::wstring text;
		ArtistBio() = default;
		ArtistBio(const web::json::value& json);
	};
	class GetArtistBioQuery : public QueryBase {
	private:
		std::int64_t _id;
	public:
		GetArtistBioQuery(std::int64_t id, Platform::String^ sessionId, Platform::String^ countryCode);
		GetArtistBioQuery(std::int64_t id, Platform::String^ countryCode);

		// Inherited via QueryBase
		virtual std::wstring url() const override {
			std::wstring result = L"artists/";
			result.append(std::to_wstring(_id));
			result.append(L"/bio");
			return result;
		}

		concurrency::task<std::shared_ptr<ArtistBio>> executeAsync(concurrency::cancellation_token cancelToken);
	};
}