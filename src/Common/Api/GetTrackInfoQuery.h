#pragma once
#include "TrackInfo.h"
#include "QueryBase.h"
namespace api {
	class GetTrackInfoQuery : public api::QueryBase {
	private:
		std::int64_t _id;
	public:
		GetTrackInfoQuery(std::int64_t id, Platform::String^ sessionId, Platform::String^ countryCode) :
		QueryBase(sessionId, countryCode),
			_id(id)
		{
		}
		GetTrackInfoQuery(std::int64_t id, Platform::String^ countryCode) :
			_id(id) {
			addQueryStringParameter(L"countryCode", countryCode);
		}

		// Inherited via QueryBase
		virtual std::wstring url() const override {
			std::wstring result = L"tracks/";
			result.append(std::to_wstring(_id));
			return result;
		}

		concurrency::task<std::shared_ptr<TrackInfo>> executeAsync(concurrency::cancellation_token cancelToken);
	};
}