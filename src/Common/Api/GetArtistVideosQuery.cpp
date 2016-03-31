#include "pch.h"
#include "GetArtistVideosQuery.h"
using namespace api;

api::GetArtistVideosQuery::GetArtistVideosQuery(std::int64_t id, int limit, int offset, Platform::String ^ sessionId, Platform::String ^ countryCode)
	:QueryBase(sessionId, countryCode), _id(id)
{
	addQueryStringParameter(L"limit", limit.ToString());
	addQueryStringParameter(L"offset", offset.ToString());
}

api::GetArtistVideosQuery::GetArtistVideosQuery(std::int64_t id, int limit, int offset, Platform::String ^ countryCode):
	_id(id)
{
	addQueryStringParameter(L"limit", limit.ToString());
	addQueryStringParameter(L"offset", offset.ToString());
	addQueryStringParameter(L"countryCode", countryCode);
}

std::wstring api::GetArtistVideosQuery::url() const
{
	std::wstring result(L"artists/");
	result.append(std::to_wstring(_id));
	result.append(L"/videos");
	return result;
}

concurrency::task<std::shared_ptr<PaginatedList<VideoInfo>>> api::GetArtistVideosQuery::executeAsync(concurrency::cancellation_token cancelToken)
{
	auto json = co_await getAsync(cancelToken);
	tools::strings::WindowsWIStream stream(json);
	auto jsonVal = web::json::value::parse(stream);
	co_return std::make_shared<api::PaginatedList<VideoInfo>>(jsonVal);
}
