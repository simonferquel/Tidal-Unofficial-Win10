#include "pch.h"
#include "GetNewVideosQuery.h"
using namespace api;

api::GetNewVideosQuery::GetNewVideosQuery(int limit, int offset, Platform::String ^ sessionId, Platform::String ^ countryCode)
	:QueryBase(sessionId, countryCode)
{
	addQueryStringParameter(L"limit", limit.ToString());
	addQueryStringParameter(L"offset", offset.ToString());
}

api::GetNewVideosQuery::GetNewVideosQuery(int limit, int offset, Platform::String ^ countryCode)
{
	addQueryStringParameter(L"limit", limit.ToString());
	addQueryStringParameter(L"offset", offset.ToString());
	addQueryStringParameter(L"countryCode", countryCode);
}

std::wstring api::GetNewVideosQuery::url() const
{
	std::wstring result = _list->Data();
	result.push_back(L'/');
	result.append(_group->Data());
	result.append(L"/videos");
	return result;
}

concurrency::task<std::shared_ptr<PaginatedList<VideoInfo>>> api::GetNewVideosQuery::executeAsync(concurrency::cancellation_token cancelToken)
{
	return getAsync(cancelToken).then([](Platform::String^ json) {
		tools::strings::WindowsWIStream stream(json);
		auto jsonVal = web::json::value::parse(stream);
		return std::make_shared<api::PaginatedList<VideoInfo>>(jsonVal);
	});
}
