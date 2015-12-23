#pragma once
#include "QueryBase.h"
namespace api {
	struct UserInfo;
	class GetUserInfoQuery : public QueryBase
	{
	private:
		std::int64_t _userId;
	public:
		GetUserInfoQuery(std::int64_t userId, Platform::String^ sessionId, Platform::String^ countryCode);



		// Inherited via QueryBase
		virtual std::wstring url() const override;
		concurrency::task<std::shared_ptr<UserInfo>> executeAsync(concurrency::cancellation_token cancelToken);

	};
}

