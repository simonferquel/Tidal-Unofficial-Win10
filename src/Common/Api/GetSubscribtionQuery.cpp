#include "pch.h"
#include "GetSubscribtionQuery.h"
#include "tools\StringUtils.h"
#include "SubscribtionInfo.h"

using namespace api;

api::GetSubscribtionQuery::GetSubscribtionQuery(std::int64_t userId, Platform::String ^ sessionId, Platform::String^ countryCode)
	:QueryBase(sessionId, countryCode), _userId(userId)
{
}

std::wstring api::GetSubscribtionQuery::url() const
{
	std::wstring url(L"users/");
	url.append(_userId.ToString()->Data());
	url.append(L"/subscription");
	return url;
}

concurrency::task<std::shared_ptr<SubscribtionInfo>> api::GetSubscribtionQuery::executeAsync(concurrency::cancellation_token cancelToken)
{
	auto json = co_await getAsync(cancelToken);
	tools::strings::WindowsWIStream stream(json);
	auto jsonVal = web::json::value::parse(stream);
	return std::make_shared<api::SubscribtionInfo>(jsonVal);
}
