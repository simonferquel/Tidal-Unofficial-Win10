#include "pch.h"
#include "GetUserInfoQuery.h"
#include "UserInfo.h"
#include "../tools/StringUtils.h"
using namespace api;
api::GetUserInfoQuery::GetUserInfoQuery(std::int64_t userId, Platform::String ^ sessionId, Platform::String ^ countryCode) :
	QueryBase(sessionId, countryCode),
	_userId(userId)
{
}

std::wstring api::GetUserInfoQuery::url() const
{
	std::wstring url(L"users/");
	url.append(_userId.ToString()->Data());
	return url;
}

concurrency::task<std::shared_ptr<UserInfo>> api::GetUserInfoQuery::executeAsync(concurrency::cancellation_token cancelToken)
{

	auto json = await getAsync(cancelToken);
	tools::strings::WindowsWIStream stream(json);
	auto jsonVal = web::json::value::parse(stream);
	return std::make_shared<api::UserInfo>(jsonVal);
}
