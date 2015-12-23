#pragma once
#include <memory>
#include "QueryBase.h"
namespace api {
	struct LoginResult;
	class LoginWithPasswordQuery : public QueryBase
	{
	public:
		LoginWithPasswordQuery(Platform::String^ userName, Platform::String^ password);

		// Inherited via QueryBase
		virtual std::wstring url() const override;
		virtual Windows::Foundation::TimeSpan timeout() const override;
		concurrency::task<std::shared_ptr<LoginResult>> executeAsync(concurrency::cancellation_token cancelToken);
	};
}
