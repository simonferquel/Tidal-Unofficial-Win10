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

void api::QueryBase::addHeader(Platform::String ^ key, Platform::String ^ value)
{
	if (!_customHeaders) {
		_customHeaders = ref new Map<String^, String^>();
	}
	_customHeaders->Insert(key, value);
}

api::QueryBase::QueryBase()
{
	addQueryStringParameter(L"token", config::apiToken());
}

api::QueryBase::QueryBase(Platform::String ^ sessionId, Platform::String^ countryCode)
{
	addHeader(L"X-Tidal-SessionId", sessionId);
	addQueryStringParameter(L"countryCode", countryCode);
}



concurrency::task<Platform::String^> api::QueryBase::getAsync(concurrency::cancellation_token cancelToken, std::shared_ptr<ResponseHolder> responseHolder)
{
	auto filter = ref new Windows::Web::Http::Filters::HttpBaseProtocolFilter();
	filter->AllowUI = false;
	auto client = ref new HttpClient(filter);
	client->DefaultRequestHeaders->UserAgent->Clear();
	client->DefaultRequestHeaders->UserAgent->Append(ref new Windows::Web::Http::Headers::HttpProductInfoHeaderValue(L"Tidal-Unofficial.Windows10", Windows::System::Profile::AnalyticsInfo::VersionInfo->DeviceFamily));
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
	auto request = ref new HttpRequestMessage(HttpMethod::Get, ref new Uri(tools::strings::toWindowsString(urlBuilder)));
	if (_customHeaders) {
		for (auto&& pair : _customHeaders) {
			request->Headers->Append(pair->Key, pair->Value);
		}
	}

	if (!responseHolder) {
		responseHolder = std::make_shared<ResponseHolder>();
	}
	return create_task(client->SendRequestAsync(request), combinedTokens)
		.then([responseHolder, combinedTokens](HttpResponseMessage^ response) {
		responseHolder->response = response;
		return create_task(response->Content->ReadAsStringAsync(), combinedTokens);
	})
		.then([responseHolder, urlBuilder](Platform::String^ contentString) {
		if (!responseHolder->response->IsSuccessStatusCode) {
			throw statuscode_exception(urlBuilder, responseHolder->response->StatusCode, contentString);
		}
		return contentString;
	}).then([cancelToken, urlBuilder](concurrency::task<Platform::String^> t) {
		try {
			return t.get();
		}
		catch (task_canceled&) {
			if (!cancelToken.is_canceled()) {
				throw timeout_exception(urlBuilder);
			}
			throw;
		}
	});

}

concurrency::task<Platform::String^> api::QueryBase::deleteAsync(concurrency::cancellation_token cancelToken, std::shared_ptr<ResponseHolder> responseHolder)
{
	auto filter = ref new Windows::Web::Http::Filters::HttpBaseProtocolFilter();
	filter->AllowUI = false;
	auto client = ref new Windows::Web::Http::HttpClient(filter);
	client->DefaultRequestHeaders->UserAgent->Clear();
	client->DefaultRequestHeaders->UserAgent->Append(ref new Windows::Web::Http::Headers::HttpProductInfoHeaderValue(L"Tidal-Unofficial.Windows10", Windows::System::Profile::AnalyticsInfo::VersionInfo->DeviceFamily));

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

	auto request = ref new HttpRequestMessage(HttpMethod::Delete, ref new Uri(tools::strings::toWindowsString(urlBuilder)));
	if (_customHeaders) {
		for (auto&& pair : _customHeaders) {
			request->Headers->Append(pair->Key, pair->Value);
		}
	}
	if (!responseHolder) {
		responseHolder = std::make_shared<ResponseHolder>();
	}
	return create_task(client->SendRequestAsync(request), combinedTokens)
		.then([responseHolder, combinedTokens](HttpResponseMessage^ response) {
		responseHolder->response = response;
		return create_task(response->Content->ReadAsStringAsync(), combinedTokens);
	})
		.then([responseHolder, urlBuilder](Platform::String^ contentString) {
		if (!responseHolder->response->IsSuccessStatusCode) {
			throw statuscode_exception(urlBuilder, responseHolder->response->StatusCode, contentString);
		}
		return contentString;
	}).then([cancelToken, urlBuilder](concurrency::task<Platform::String^> t) {
		try {
			return t.get();
		}
		catch (task_canceled&) {
			if (!cancelToken.is_canceled()) {
				throw timeout_exception(urlBuilder);
			}
			throw;
		}
	});

}

concurrency::task<Platform::String^> api::QueryBase::postAsync(concurrency::cancellation_token cancelToken, std::shared_ptr<ResponseHolder> responseHolder)
{
	auto filter = ref new Windows::Web::Http::Filters::HttpBaseProtocolFilter();
	filter->AllowUI = false;
	auto client = ref new Windows::Web::Http::HttpClient(filter);
	client->DefaultRequestHeaders->UserAgent->Clear();
	client->DefaultRequestHeaders->UserAgent->Append(ref new Windows::Web::Http::Headers::HttpProductInfoHeaderValue(L"Tidal-Unofficial.Windows10", Windows::System::Profile::AnalyticsInfo::VersionInfo->DeviceFamily));
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

	auto request = ref new HttpRequestMessage(HttpMethod::Post, ref new Uri(tools::strings::toWindowsString(urlBuilder)));
	if (_requestContent) {
		request->Content = ref new HttpFormUrlEncodedContent(_requestContent);
	}
	else {
		request->Content = ref new HttpStringContent(L"");
	}
	if (_customHeaders) {
		for (auto&& pair : _customHeaders) {
			request->Headers->Append(pair->Key, pair->Value);
		}
	}
	if (!responseHolder) {
		responseHolder = std::make_shared<ResponseHolder>();
	}
	return create_task(client->SendRequestAsync(request), combinedTokens)
		.then([responseHolder, combinedTokens](HttpResponseMessage^ response) {
		responseHolder->response = response;
		return create_task(response->Content->ReadAsStringAsync(), combinedTokens);
	})
		.then([responseHolder, urlBuilder](Platform::String^ contentString) {
		if (!responseHolder->response->IsSuccessStatusCode) {
			throw statuscode_exception(urlBuilder, responseHolder->response->StatusCode, contentString);
		}
		return contentString;
	}).then([cancelToken, urlBuilder](concurrency::task<Platform::String^> t) {
		try {
			return t.get();
		}
		catch (task_canceled&) {
			if (!cancelToken.is_canceled()) {
				throw timeout_exception(urlBuilder);
			}
			throw;
		}
	});
}

