#include "pch.h"
#include "GetNewPlaylistsQuery.h"
using namespace api;
api::GetNewPlaylistsQuery::GetNewPlaylistsQuery(int limit, int offset, Platform::String ^ sessionId, Platform::String ^ countryCode)
	:QueryBase(sessionId, countryCode)
{
	addQueryStringParameter(L"limit", limit.ToString());
	addQueryStringParameter(L"offset", offset.ToString());
}

api::GetNewPlaylistsQuery::GetNewPlaylistsQuery(int limit, int offset, Platform::String ^ countryCode)
{
	addQueryStringParameter(L"limit", limit.ToString());
	addQueryStringParameter(L"offset", offset.ToString());
	addQueryStringParameter(L"countryCode", countryCode);
}

std::wstring api::GetNewPlaylistsQuery::url() const
{

	std::wstring result = _list->Data();
	result.push_back(L'/');
	result.append(_group->Data());
	result.append(L"/playlists");
	return result;
}

concurrency::task<std::shared_ptr<PaginatedList<PlaylistResume>>> api::GetNewPlaylistsQuery::executeAsync(concurrency::cancellation_token cancelToken)
{

	auto json = await getAsync(cancelToken);
	tools::strings::WindowsWIStream stream(json);
	auto jsonVal = web::json::value::parse(stream);
	return std::make_shared<api::PaginatedList<PlaylistResume>>(jsonVal);
}

