#include "pch.h"
#include "GetSimilarArtistsQuery.h"
#include "../tools/StringUtils.h"
#include "ApiErrors.h"
using namespace api;

api::GetSimilarArtistsQuery::GetSimilarArtistsQuery(std::int64_t id, int limit, int offset, Platform::String ^ sessionId, Platform::String ^ countryCode) :
	QueryBase(sessionId, countryCode), _id(id)
{
	addQueryStringParameter(L"limit", limit.ToString());
	addQueryStringParameter(L"offset", offset.ToString());
}

api::GetSimilarArtistsQuery::GetSimilarArtistsQuery(std::int64_t id, int limit, int offset, Platform::String ^ countryCode) :
	_id(id)
{
	addQueryStringParameter(L"limit", limit.ToString());
	addQueryStringParameter(L"offset", offset.ToString());
	addQueryStringParameter(L"countryCode", countryCode);
}

concurrency::task<std::shared_ptr<PaginatedList<ArtistInfo>>> api::GetSimilarArtistsQuery::executeAsync(concurrency::cancellation_token cancelToken)
{
	try {
		auto json = co_await getAsync(cancelToken);
		tools::strings::WindowsWIStream stream(json);
		auto jsonVal = web::json::value::parse(stream);
		return std::make_shared<PaginatedList<ArtistInfo>>(jsonVal);
	}
	catch (statuscode_exception& ex) {
		if (ex.getStatusCode() == Windows::Web::Http::HttpStatusCode::NotFound) {
			return std::make_shared<PaginatedList<ArtistInfo>>();
		}
		throw;
	}
}
