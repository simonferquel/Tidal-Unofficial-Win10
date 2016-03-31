#include "pch.h"
#include "GetArtistAlbumsQuery.h"
using namespace api;

api::GetArtistAlbumsQuery::GetArtistAlbumsQuery(std::int64_t artistId, int limit, int offset, Platform::String ^ sessionId, Platform::String ^ countryCode)
	: QueryBase(sessionId, countryCode), _artistId(artistId)
{
	addQueryStringParameter(L"limit", limit.ToString());
	addQueryStringParameter(L"offset", offset.ToString());
}

api::GetArtistAlbumsQuery::GetArtistAlbumsQuery(std::int64_t artistId, int limit, int offset, Platform::String ^ countryCode) :
	 _artistId(artistId)
{
	addQueryStringParameter(L"limit", limit.ToString());
	addQueryStringParameter(L"offset", offset.ToString());
	addQueryStringParameter(L"countryCode", countryCode);
}

void api::GetArtistAlbumsQuery::addFilter(Platform::String ^ filter)
{
	addQueryStringParameter(L"filter", filter);
}

std::wstring api::GetArtistAlbumsQuery::url() const
{
	std::wstring url(L"artists/");
	url.append(_artistId.ToString()->Data());
	url.append(L"/albums");
	return url;
}


concurrency::task<std::shared_ptr<PaginatedList<AlbumResume>>> api::GetArtistAlbumsQuery::executeAsync(concurrency::cancellation_token cancelToken)
{

	auto json = co_await getAsync(cancelToken);
	tools::strings::WindowsWIStream stream(json);
	auto jsonVal = web::json::value::parse(stream);
	co_return std::make_shared<api::PaginatedList<AlbumResume>>(jsonVal);
}
