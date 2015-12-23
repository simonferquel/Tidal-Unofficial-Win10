#include "pch.h"
#include "LoginWithPasswordQuery.h"
#include "LoginResult.h"
#include "../tools/TimeUtils.h"
#include "../tools/StringUtils.h"
using namespace api;
api::LoginWithPasswordQuery::LoginWithPasswordQuery(Platform::String ^ userName, Platform::String ^ password)
{
	addBodyContent(L"username", userName);
	addBodyContent(L"password", password);
}

std::wstring api::LoginWithPasswordQuery::url() const
{
	return L"login/username";
}

Windows::Foundation::TimeSpan api::LoginWithPasswordQuery::timeout() const
{
	return tools::time::ToWindowsTimeSpan( std::chrono::seconds(30));
}

concurrency::task<std::shared_ptr<LoginResult>> api::LoginWithPasswordQuery::executeAsync(concurrency::cancellation_token cancelToken)
{
	auto json = await postAsync(cancelToken);
	tools::strings::WindowsWIStream stream(json);
	auto jsonVal = web::json::value::parse(stream);
	return std::make_shared<api::LoginResult>(jsonVal);
}
