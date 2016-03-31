#include "pch.h"
#include "SearchQuery.h"

api::SearchQuery::SearchQuery(Platform::String ^ query, Platform::String ^ types, int limit, int offset, Platform::String ^ sessionId, Platform::String ^ countryCode) :
	QueryBase(sessionId, countryCode)
{
	addQueryStringParameter(L"query", query);
	addQueryStringParameter(L"types", types);
	addQueryStringParameter(L"limit", limit.ToString());
	addQueryStringParameter(L"offset", offset.ToString());
}

api::SearchQuery::SearchQuery(Platform::String ^ query, Platform::String ^ types, int limit, int offset, Platform::String ^ countryCode)
{
	addQueryStringParameter(L"query", query);
	addQueryStringParameter(L"types", types);
	addQueryStringParameter(L"limit", limit.ToString());
	addQueryStringParameter(L"offset", offset.ToString());

	addQueryStringParameter(L"countryCode", countryCode);
}

concurrency::task<std::shared_ptr<api::SearchResults>> api::SearchQuery::executeAsync(concurrency::cancellation_token cancelToken)
{
	auto json = co_await getAsync(cancelToken);
	tools::strings::WindowsWIStream stream(json);
	auto jsonVal = web::json::value::parse(stream);
	return std::make_shared<api::SearchResults>(jsonVal);
}

std::wstring api::SearchQuery::url() const
{
	return L"search";
}
