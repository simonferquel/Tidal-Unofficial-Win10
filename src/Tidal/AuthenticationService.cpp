#include "pch.h"
#include "AuthenticationService.h"
#include "Api/LoginWithFacebookQuery.h"
#include "Api/LoginWithPasswordQuery.h"
#include "Api/LoginResult.h"
#include "Api/GetSubscribtionQuery.h"
#include "Api/GetUserInfoQuery.h"
#include "Api/SubscribtionInfo.h"
#include "Api/UserInfo.h"
#include "tools/StringUtils.h"
#include "Mediators.h"
using namespace Platform;
using namespace Windows::Storage;
AuthenticationService::AuthenticationService() 
{
	auto settingsValues = ApplicationData::Current->LocalSettings->Values;
	auto authStateSetting = dynamic_cast<Platform::String^>(settingsValues->Lookup(L"authenticationState"));
	if (!authStateSetting) {
		return;
	}
	tools::strings::WindowsWIStream stream(authStateSetting);
	auto json = web::json::value::parse(stream);
	_authState.loadJson(json);
	settingsValues->Insert(L"SessionId", _authState.sessionId());
	settingsValues->Insert(L"CountryCode", _authState.countryCode());
	//_authState = AuthenticationState(static_cast<IBox<std::int64_t>^>(objUserId)->Value, dynamic_cast<Platform::String^>(settingsValues->Lookup(L"sessionId")), dynamic_cast<Platform::String^>(settingsValues->Lookup(L"countryCode")));
	
}

concurrency::task<void> AuthenticationService::authenticateWithFacebookAsync(Hat<Platform::String> accessToken, concurrency::cancellation_token cancelToken)
{
	auto q = std::make_shared<api::LoginWithFacebookQuery>(accessToken.get());
	auto result = co_await q->executeAsync(cancelToken);
	auto userInfoQuery = std::make_shared<api::GetUserInfoQuery>(result->userId,
		tools::strings::toWindowsString(result->sessionId),
		tools::strings::toWindowsString(result->countryCode));
	auto userInfoTask = userInfoQuery->executeAsync(cancelToken);
	auto subscribtionQuery = std::make_shared<api::GetSubscribtionQuery>(result->userId,
		tools::strings::toWindowsString(result->sessionId),
		tools::strings::toWindowsString(result->countryCode));
	auto subscribtion = co_await subscribtionQuery->executeAsync(cancelToken);
	auto userInfo = co_await userInfoTask;
	_authState = AuthenticationState(*result, *subscribtion, *userInfo);
	
	auto settingsValues = ApplicationData::Current->LocalSettings->Values;
	auto json = _authState.toJson();
	auto jsonStr = json.serialize();

	settingsValues->Insert(L"authenticationState", tools::strings::toWindowsString(jsonStr));
	settingsValues->Insert(L"SessionId", _authState.sessionId());
	settingsValues->Insert(L"CountryCode", _authState.countryCode());
	getAuthenticationStateMediator().raise(_authState);
}

concurrency::task<void> AuthenticationService::authenticateWithPasswordAsync(Hat<Platform::String> userName, Hat<Platform::String> password, concurrency::cancellation_token cancelToken)
{
	auto  q= std::make_shared<api::LoginWithPasswordQuery>(userName.get(), password.get());
	auto result = co_await q->executeAsync(cancelToken);
	auto userInfoQuery = std::make_shared<api::GetUserInfoQuery>(result->userId,
		tools::strings::toWindowsString(result->sessionId),
		tools::strings::toWindowsString(result->countryCode));
	auto userInfoTask = userInfoQuery->executeAsync(cancelToken);
	auto subscribtionQuery = std::make_shared<api::GetSubscribtionQuery>(result->userId,
		tools::strings::toWindowsString(result->sessionId),
		tools::strings::toWindowsString(result->countryCode));
	auto subscribtion = co_await subscribtionQuery->executeAsync(cancelToken);
	auto userInfo = co_await userInfoTask;
	_authState = AuthenticationState(*result, *subscribtion, *userInfo);

	auto settingsValues = ApplicationData::Current->LocalSettings->Values;
	auto json = _authState.toJson();
	auto jsonStr = json.serialize();

	settingsValues->Insert(L"authenticationState", tools::strings::toWindowsString(jsonStr));
	settingsValues->Insert(L"SessionId", _authState.sessionId());
	settingsValues->Insert(L"CountryCode", _authState.countryCode());
	getAuthenticationStateMediator().raise(_authState);
}

void AuthenticationService::logout()
{
	_authState = AuthenticationState();

	auto settingsValues = ApplicationData::Current->LocalSettings->Values;
	if (settingsValues->HasKey(L"authenticationState")) {
		settingsValues->Remove(L"authenticationState");
	}
	if (settingsValues->HasKey(L"SessionId")) {
		settingsValues->Remove(L"SessionId");
	}
	if (settingsValues->HasKey(L"CountryCode")) {
		settingsValues->Remove(L"CountryCode");
	}
	getAuthenticationStateMediator().raise(_authState);
}

AuthenticationService & getAuthenticationService()
{
	static AuthenticationService authSvc;
	return authSvc;
}
