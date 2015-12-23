#pragma once
#include "QueryBase.h"
#include "UrlInfo.h"
namespace api {
	class GetVideoUrlQuery : public QueryBase {
	private:
		Platform::String^ _videoId;
	public:
		GetVideoUrlQuery(Platform::String^ sessionId, Platform::String ^ countryCode, Platform::String^ videoId);

		// Inherited via QueryBase
		virtual std::wstring url() const override;

		concurrency::task<std::shared_ptr<UrlInfo>> executeAsync(concurrency::cancellation_token cancelToken);
	};
}