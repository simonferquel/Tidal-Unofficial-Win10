#pragma once
#include "QueryBase.h"
namespace api {
	struct SubscribtionInfo;
	class GetSubscribtionQuery : public QueryBase
	{
	private:
		std::int64_t _userId;
	public:
		GetSubscribtionQuery(std::int64_t userId, Platform::String^ sessionId, Platform::String^ countryCode);

		// Inherited via QueryBase
		virtual std::wstring url() const override;
		concurrency::task<std::shared_ptr<SubscribtionInfo>> executeAsync(concurrency::cancellation_token cancelToken);

	};
}

