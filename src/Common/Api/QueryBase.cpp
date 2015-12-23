#include "pch.h"
#include "QueryBase.h"
#include "config.h"
#include "../tools/TimeUtils.h"
#include "../tools/AsyncHelpers.h"
#include "ApiErrors.h"
#include "../tools/StringUtils.h"
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Web::Http;
using namespace Windows::Foundation;
using namespace std::chrono;
using namespace concurrency;

Windows::Foundation::TimeSpan api::QueryBase::timeout() const
{
	return tools::time::ToWindowsTimeSpan(30s);
}
void api::QueryBase::addBodyContent(Platform::String ^ key, Platform::String ^ value)
{
	if (!_requestContent)
	{
		_requestContent = ref new Map<String^, String^>();
	}
	_requestContent->Insert(key, value);
}

void api::QueryBase::addQueryStringParameter(Platform::String ^ key, Platform::String ^ value)
{
	if (!_queryString)
	{
		_queryString = ref new Map<String^, String^>();
	}
	_queryString->Insert(key, value);
}

api::QueryBase::QueryBase()
{
	addQueryStringParameter(L"token", config::apiToken());
}

api::QueryBase::QueryBase(Platform::String ^ sessionId, Platform::String^ countryCode)
{
	addQueryStringParameter(L"sessionId", sessionId);
	addQueryStringParameter(L"countryCode", countryCode);
}

concurrency::task<Platform::String^> api::QueryBase::getAsync(concurrency::cancellation_token cancelToken)
{
	auto client = ref new HttpClient();
	std::wstring urlBuilder(config::apiLocationPrefix()->Data());
	urlBuilder.append(url());
	if (_queryString && _queryString->Size > 0) {
		urlBuilder.push_back(L'?');
		bool first = true;
		for (auto&& p : _queryString){
			if (first){
				first = false;
			}
			else {
				urlBuilder.push_back(L'&');
			}
			urlBuilder.append(Uri::EscapeComponent(p->Key)->Data());
			urlBuilder.push_back(L'=');
			urlBuilder.append(Uri::EscapeComponent(p->Value)->Data());
		}
	}
	auto timeoutProvider = std::make_shared<tools::async::TimeoutCancelTokenProvider>(timeout());
	auto combinedTokens = tools::async::combineCancelTokens(cancelToken, timeoutProvider->get_token());
	try {
		auto response = await create_task(client->GetAsync(ref new Uri(tools::strings::toWindowsString(urlBuilder))), combinedTokens);
		auto contentString = await create_task(response->Content->ReadAsStringAsync(), combinedTokens);
		if (!response->IsSuccessStatusCode) {
			throw statuscode_exception(urlBuilder, response->StatusCode, contentString);
		}
		return contentString;
	}
	catch (task_canceled&) {
		if (!cancelToken.is_canceled()) {
			throw timeout_exception(urlBuilder);
		}
		throw;
	}
}

concurrency::task<Platform::String^> api::QueryBase::postAsync(concurrency::cancellation_token cancelToken)
{
	auto client = ref new HttpClient();
	std::wstring urlBuilder(config::apiLocationPrefix()->Data());
	urlBuilder.append(url());
	if (_queryString && _queryString->Size > 0) {
		urlBuilder.push_back(L'?');
		bool first = true;
		for (auto&& p : _queryString) {
			if (first) {
				first = false;
			}
			else {
				urlBuilder.push_back(L'&');
			}
			urlBuilder.append(Uri::EscapeComponent(p->Key)->Data());
			urlBuilder.push_back(L'=');
			urlBuilder.append(Uri::EscapeComponent(p->Value)->Data());
		}
	}
	auto timeoutProvider = std::make_shared<tools::async::TimeoutCancelTokenProvider>(timeout());
	auto combinedTokens = tools::async::combineCancelTokens(cancelToken, timeoutProvider->get_token());
	try {
		auto response = await create_task(client->PostAsync(ref new Uri(tools::strings::toWindowsString(urlBuilder)), _requestContent ? static_cast<IHttpContent^>(ref new HttpFormUrlEncodedContent(_requestContent)) : static_cast<IHttpContent^>(ref new HttpStringContent(L""))), combinedTokens);
		auto contentString = await create_task(response->Content->ReadAsStringAsync(), combinedTokens);
		if (!response->IsSuccessStatusCode) {
			throw statuscode_exception(urlBuilder, response->StatusCode, contentString);
		}
		return contentString;
	}
	catch (task_canceled&) {
		if (!cancelToken.is_canceled()) {
			throw timeout_exception(urlBuilder);
		}
		throw;
	}
}

