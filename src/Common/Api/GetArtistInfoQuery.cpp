#include "pch.h"
#include "GetArtistInfoQuery.h"
#include "../tools/StringUtils.h"
using namespace api;
api::GetArtistInfoQuery::GetArtistInfoQuery(std::int64_t id, Platform::String ^ sessionId, Platform::String ^ countryCode) :
	QueryBase(sessionId, countryCode), _id(id)
{
}

api::GetArtistInfoQuery::GetArtistInfoQuery(std::int64_t id, Platform::String ^ countryCode) :
	_id(id)
{
	addQueryStringParameter(L"countryCode", countryCode);
}

concurrency::task<std::shared_ptr<ArtistInfo>> api::GetArtistInfoQuery::executeAsync(concurrency::cancellation_token cancelToken)
{
	auto json = co_await getAsync(cancelToken);
	tools::strings::WindowsWIStream stream(json);
	auto jsonVal = web::json::value::parse(stream);
	return std::make_shared<ArtistInfo>(jsonVal);
}
