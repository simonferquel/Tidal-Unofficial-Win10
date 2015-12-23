#pragma once
#include <cstdint>
#include "QueryBase.h"
#include "UrlInfo.h"
namespace api {
	class GetTrackStreamUrlQuery : public QueryBase {
	private:
		bool _offline;
		std::int64_t _trackId;
	public:
		GetTrackStreamUrlQuery(Platform::String^ sessionId, Platform::String ^ countryCode, std::int64_t id, Platform::String^ soundQuality, bool offline=false);

		// Inherited via QueryBase
		virtual std::wstring url() const override;

		concurrency::task<std::shared_ptr<UrlInfo>> executeAsync(concurrency::cancellation_token cancelToken);
	};
}