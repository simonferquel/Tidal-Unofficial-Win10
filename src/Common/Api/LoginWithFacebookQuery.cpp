#include "pch.h"
#include "LoginWithFacebookQuery.h"
#include "LoginResult.h"
#include "../tools/StringUtils.h"
#include "../tools/TimeUtils.h"
#include <cpprest/json.h>
using namespace std::chrono;
api::LoginWithFacebookQuery::LoginWithFacebookQuery(Platform::String ^ accessToken)
{
	addBodyContent(L"accessToken", accessToken);
}

std::wstring api::LoginWithFacebookQuery::url() const
{
	return L"login/facebook";
}

Windows::Foundation::TimeSpan api::LoginWithFacebookQuery::timeout() const
{
	return tools::time::ToWindowsTimeSpan(30s);
}

concurrency::task<std::shared_ptr<api::LoginResult>> api::LoginWithFacebookQuery::executeAsync(concurrency::cancellation_token cancelToken)
{
	auto json = await postAsync(cancelToken);
	tools::strings::WindowsWIStream stream(json);
	auto jsonVal = web::json::value::parse(stream);
	return std::make_shared<api::LoginResult>(jsonVal);
}
