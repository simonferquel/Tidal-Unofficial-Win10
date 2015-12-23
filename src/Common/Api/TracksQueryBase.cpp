#include "pch.h"
#include "TracksQueryBase.h"
#include "../tools/StringUtils.h"
using namespace api;

api::GetTracksQueryBase::GetTracksQueryBase(int limit, int offset, Platform::String ^ sessionId, Platform::String ^ countryCode)
	: QueryBase(sessionId, countryCode)
{
	addQueryStringParameter(L"limit", limit.ToString());
	addQueryStringParameter(L"offset", offset.ToString());
}

api::GetTracksQueryBase::GetTracksQueryBase(int limit, int offset, Platform::String ^ countryCode)
{
	addQueryStringParameter(L"limit", limit.ToString());
	addQueryStringParameter(L"offset", offset.ToString());
	addQueryStringParameter(L"countryCode", countryCode);
}

concurrency::task<std::shared_ptr<PaginatedList<TrackInfo>>> api::GetTracksQueryBase::executeAsync(concurrency::cancellation_token cancelToken)
{
	auto json = await getAsync(cancelToken);
	tools::strings::WindowsWIStream stream(json);
	auto jsonVal = web::json::value::parse(stream);
	return std::make_shared<api::PaginatedList<TrackInfo>>(jsonVal);
}
