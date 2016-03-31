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
	auto responseHolder = std::make_shared<ResponseHolder>();
	auto json = co_await getAsync(cancelToken, responseHolder);
	tools::strings::WindowsWIStream stream(json);
	auto jsonVal = web::json::value::parse(stream);
	
	auto result = std::make_shared<api::PaginatedList<TrackInfo>>(jsonVal);
	if ( responseHolder->response && responseHolder->response->Headers && responseHolder->response->Headers->HasKey(L"ETag")) {
		auto etag = responseHolder->response->Headers->Lookup(L"ETag");
		if (etag) {
			result->etag = tools::strings::toStdString(etag);
		}
	}

	return result;
}
