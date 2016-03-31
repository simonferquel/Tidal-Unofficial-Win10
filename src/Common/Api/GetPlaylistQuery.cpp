#include "pch.h"
#include "GetPlaylistQuery.h"
#include "../tools/StringUtils.h"
using namespace api;

api::GetPlaylistQuery::GetPlaylistQuery(const std::wstring & playlistId, Platform::String ^ sessionId, Platform::String ^ countryCode)
	: QueryBase(sessionId, countryCode),
	_playlistId(playlistId)
{
}

api::GetPlaylistQuery::GetPlaylistQuery(const std::wstring & playlistId, Platform::String ^ countryCode)
	: _playlistId(playlistId)
{
	addQueryStringParameter(L"countryCode", countryCode);
}

std::wstring api::GetPlaylistQuery::url() const
{
	std::wstring url(L"playlists/");
	url.append(_playlistId);
	return url;
}

concurrency::task<std::shared_ptr<PlaylistResume>> api::GetPlaylistQuery::executeAsync(concurrency::cancellation_token cancelToken)
{
	auto json = co_await getAsync(cancelToken);
	tools::strings::WindowsWIStream stream(json);
	auto jsonVal = web::json::value::parse(stream);
	return std::make_shared<api::PlaylistResume>(jsonVal);
}
