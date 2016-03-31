#include "pch.h"
#include "GetArtistBioQuery.h"
#include "JsonHelpers.h"
#include "../tools/StringUtils.h"
using namespace api;
api::ArtistBio::ArtistBio(const web::json::value & json)
{
	parseJson(&json, L"lastUpdated", lastUpdated);
	parseJson(&json, L"source", source);
	parseJson(&json, L"summary", summary);
	parseJson(&json, L"text", text);
}

api::GetArtistBioQuery::GetArtistBioQuery(std::int64_t id, Platform::String ^ sessionId, Platform::String ^ countryCode) :
	QueryBase(sessionId, countryCode),
	_id(id)
{
}

api::GetArtistBioQuery::GetArtistBioQuery(std::int64_t id, Platform::String ^ countryCode) :
	_id(id)
{
	addQueryStringParameter(L"countryCode", countryCode);
}

concurrency::task<std::shared_ptr<ArtistBio>> api::GetArtistBioQuery::executeAsync(concurrency::cancellation_token cancelToken)
{
	auto json = co_await getAsync(cancelToken);
	tools::strings::WindowsWIStream stream(json);
	auto jsonVal = web::json::value::parse(stream);
	co_return std::make_shared<ArtistBio>(jsonVal);
}
