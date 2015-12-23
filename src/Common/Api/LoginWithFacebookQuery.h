#pragma once
#include <memory>
#include "QueryBase.h"
namespace api {
	struct LoginResult;
	class LoginWithFacebookQuery : public QueryBase
	{
	public:
		LoginWithFacebookQuery(Platform::String^ accessToken);

		// Inherited via QueryBase
		virtual std::wstring url() const override;
		virtual Windows::Foundation::TimeSpan timeout() const override;
		concurrency::task<std::shared_ptr<LoginResult>> executeAsync(concurrency::cancellation_token cancelToken);
	};
}

